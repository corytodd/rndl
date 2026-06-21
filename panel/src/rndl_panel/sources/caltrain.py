"""Minutes until the next northbound/southbound Caltrain at a station, via
the 511.org SF Bay transit SIRI API.

Get a free API key at https://511.org/open-data/token and set RNDL_CALTRAIN_API_KEY
"""

import argparse
import json
import os
import time
from datetime import datetime, timezone
from pathlib import Path

import httpx

from ..common import FRAME_SIZE_BYTES, write_frame_atomic
from ..font import draw_text
from .base import PanelSource

DEFAULT_API_URL = "https://api.511.org/transit"
# Bayshore Caltrain station, northbound and southbound platforms
DEFAULT_STOP_CODE_NB = "70031"
DEFAULT_STOP_CODE_SB = "70032"
DEFAULT_POLL_INTERVAL_S = 300.0
MIN_POLL_INTERVAL_S = 60.0  # 511.org rate limit: at most one request per minute

NORTH_COLOR = (0, 180, 255)
SOUTH_COLOR = (255, 120, 0)
NORTH_LABEL_Y = 1
SOUTH_LABEL_Y = 9
TEXT_X = 4


def _parse_arrival_time(raw: str) -> datetime:
    return datetime.fromisoformat(raw.replace("Z", "+00:00"))


def fetch_next_arrival(client: httpx.Client, api_url: str, api_key: str, stop_code: str) -> datetime | None:
    """Query StopMonitoring for one platform/stop code and return the soonest
    predicted arrival time, if any."""
    resp = client.get(
        f"{api_url}/StopMonitoring",
        params={"api_key": api_key, "agency": "CT", "stopCode": stop_code, "format": "json"},
    )
    resp.raise_for_status()
    # TODO: 511.org sometimes prepends a UTF-8 BOM??
    payload = json.loads(resp.text.lstrip("﻿"))

    visits = payload.get("ServiceDelivery", {}).get("StopMonitoringDelivery", {}).get("MonitoredStopVisit", [])
    arrivals = []
    for visit in visits:
        call = visit.get("MonitoredVehicleJourney", {}).get("MonitoredCall", {})
        raw_time = call.get("ExpectedArrivalTime") or call.get("AimedArrivalTime")
        if raw_time:
            arrivals.append(_parse_arrival_time(raw_time))
    return min(arrivals) if arrivals else None


def minutes_until(arrival: datetime | None) -> int | None:
    if arrival is None:
        return None
    delta = arrival - datetime.now(tz=timezone.utc)
    return max(0, round(delta.total_seconds() / 60))


def render_frame(north_minutes: int | None, south_minutes: int | None) -> bytes:
    frame = bytearray(FRAME_SIZE_BYTES)

    north_text = "--"
    if north_minutes is not None:
        north_text = f"{min(north_minutes, 99):2d}"

    south_text = "--"
    if south_minutes is not None:
        south_text = f"{min(south_minutes, 99):2d}"

    draw_text(frame, north_text, TEXT_X, NORTH_LABEL_Y, NORTH_COLOR)
    draw_text(frame, south_text, TEXT_X, SOUTH_LABEL_Y, SOUTH_COLOR)
    return bytes(frame)


class CaltrainSource(PanelSource):
    name = "caltrain"
    help = "Minutes until next northbound/southbound Caltrain at a station. top=north, bottom=south"

    def add_arguments(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            "--api-url",
            default=os.environ.get("RNDL_CALTRAIN_API_URL", DEFAULT_API_URL),
            help=f"511.org transit API base URL (default: {DEFAULT_API_URL})",
        )
        parser.add_argument(
            "--api-key",
            default=os.environ.get("RNDL_CALTRAIN_API_KEY", ""),
            help="511.org API key (default: $RNDL_CALTRAIN_API_KEY)",
        )
        parser.add_argument(
            "--stop-code-nb",
            default=os.environ.get("RNDL_CALTRAIN_STOP_CODE_NB", DEFAULT_STOP_CODE_NB),
            help=f"511.org GTFS stop code for the northbound platform (default: Bayshore, {DEFAULT_STOP_CODE_NB})",
        )
        parser.add_argument(
            "--stop-code-sb",
            default=os.environ.get("RNDL_CALTRAIN_STOP_CODE_SB", DEFAULT_STOP_CODE_SB),
            help=f"511.org GTFS stop code for the southbound platform (default: Bayshore, {DEFAULT_STOP_CODE_SB})",
        )
        parser.add_argument(
            "--poll-interval",
            type=float,
            default=float(os.environ.get("RNDL_CALTRAIN_POLL_INTERVAL_SECONDS", DEFAULT_POLL_INTERVAL_S)),
            help=f"Seconds between API polls (default: {DEFAULT_POLL_INTERVAL_S})",
        )

    def run(self, framebuffer_path: Path, args: argparse.Namespace) -> None:
        if not args.api_key:
            raise RuntimeError(
                "Caltrain source requires an API key: set RNDL_CALTRAIN_API_KEY or pass --api-key"
            )
        if args.poll_interval < MIN_POLL_INTERVAL_S:
            raise RuntimeError(
                f"--poll-interval {args.poll_interval}s is below the 511.org rate limit of "
                f"{MIN_POLL_INTERVAL_S:.0f}s; each poll makes 2 requests (NB+SB)"
            )

        print(f"Polling {args.api_url} every {args.poll_interval:.0f}s "
              f"(NB stop {args.stop_code_nb}, SB stop {args.stop_code_sb})")
        print(f"Writing frames to {framebuffer_path}")

        with httpx.Client(timeout=10.0) as client:
            while True:
                try:
                    north = fetch_next_arrival(client, args.api_url, args.api_key, args.stop_code_nb)
                    south = fetch_next_arrival(client, args.api_url, args.api_key, args.stop_code_sb)
                    north_min = minutes_until(north)
                    south_min = minutes_until(south)
                    print(f"{time.strftime('%H:%M:%S')} north={north_min} min, south={south_min} min")
                    write_frame_atomic(render_frame(north_min, south_min), framebuffer_path)
                except httpx.HTTPError as e:
                    print(f"{time.strftime('%H:%M:%S')} caltrain fetch failed: {e}")
                time.sleep(args.poll_interval)


SOURCE = CaltrainSource()
