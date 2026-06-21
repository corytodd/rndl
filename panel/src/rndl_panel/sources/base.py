"""Plugin contract for panel sources."""

import argparse
from abc import ABC, abstractmethod
from pathlib import Path


class PanelSource(ABC):
    name: str = ""
    help: str = ""

    def add_arguments(self, parser: argparse.ArgumentParser) -> None:
        """Override to add source-specific CLI flags."""

    @abstractmethod
    def run(self, framebuffer_path: Path, args: argparse.Namespace) -> None:
        """Blocking loop: write frames to framebuffer_path until killed."""
