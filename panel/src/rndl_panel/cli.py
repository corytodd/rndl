import argparse
import asyncio
import os
import threading
from pathlib import Path

from . import bench, common, daemon, probe, sources
from .sources.base import PanelSource


def _run_source_or_die(source: PanelSource, framebuffer_path: Path, args: argparse.Namespace) -> None:
    """Thread target for a source's run() in the 'run' combo command."""
    try:
        source.run(framebuffer_path, args)
    except BaseException as e:
        print(f"source {source.name!r} failed: {e}")
        os._exit(1)


def build_parser() -> tuple[argparse.ArgumentParser, dict]:
    registry = sources.discover()

    parser = argparse.ArgumentParser(prog="rndl-panel", description="Host-side tools for the rndl LED panel")
    parser.add_argument(
        "--framebuffer",
        type=Path,
        default=common.DEFAULT_FRAMEBUFFER_PATH,
        help=f"Path to the shared framebuffer file (default: {common.DEFAULT_FRAMEBUFFER_PATH})",
    )
    parser.add_argument("--device-name", default=common.DEVICE_NAME, help="BLE advertised name of the panel")

    sub = parser.add_subparsers(dest="command")

    def add_source_subparsers(parent: argparse.ArgumentParser) -> None:
        # Always required: a source's CLI args (e.g. caltrain's --api-key) are only added to
        # the namespace when its subcommand is actually selected, so there's no way to fall
        # back to an implicit default source once any plugin has required configuration.
        source_sub = parent.add_subparsers(dest="source_name", required=True)
        for name, source in registry.items():
            sp = source_sub.add_parser(name, help=source.help)
            source.add_arguments(sp)

    run_parser = sub.add_parser("run", help="Run a panel source + BLE daemon together")
    add_source_subparsers(run_parser)

    source_parser = sub.add_parser("source", help="Run only a panel source, writing the framebuffer file")
    add_source_subparsers(source_parser)

    sub.add_parser("daemon", help="Run only the BLE daemon, watching the framebuffer file")

    probe_parser = sub.add_parser("probe", help="Dump GATT services and push random test pixels")
    probe_parser.add_argument("--num-pixels", type=int, default=16)
    probe_parser.add_argument(
        "--brightness-cap",
        type=int,
        default=bench.DEFAULT_BRIGHTNESS_CAP,
        help=f"Max per-channel value (0-255) for test pixels, to bound worst-case current draw "
        f"(default: {bench.DEFAULT_BRIGHTNESS_CAP})",
    )

    bench_parser = sub.add_parser("bench", help="Measure BLE write throughput to the panel")
    bench_parser.add_argument("--duration", type=float, default=5.0, help="Seconds to run each write mode")
    bench_parser.add_argument(
        "--brightness-cap",
        type=int,
        default=bench.DEFAULT_BRIGHTNESS_CAP,
        help=f"Max per-channel value (0-255) for benchmark pixels, to bound worst-case current draw "
        f"(default: {bench.DEFAULT_BRIGHTNESS_CAP})",
    )

    return parser, registry


def main() -> None:
    parser, registry = build_parser()
    args = parser.parse_args()
    command = args.command
    if command is None:
        parser.print_usage()
        raise SystemExit(2)

    if command in ("run", "source"):
        source = registry[args.source_name]
        if command == "source":
            source.run(args.framebuffer, args)
        else:
            stats_thread = threading.Thread(
                target=_run_source_or_die, args=(source, args.framebuffer, args), daemon=True
            )
            stats_thread.start()
            asyncio.run(daemon.run(args.framebuffer, args.device_name))
    elif command == "daemon":
        asyncio.run(daemon.run(args.framebuffer, args.device_name))
    elif command == "probe":
        asyncio.run(probe.run(args.device_name, args.num_pixels, args.brightness_cap))
    elif command == "bench":
        asyncio.run(bench.run(args.device_name, args.duration, args.brightness_cap))


if __name__ == "__main__":
    main()
