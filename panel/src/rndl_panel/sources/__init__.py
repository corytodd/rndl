"""Auto-discovery for panel source plugins."""

import importlib
import pkgutil

from .base import PanelSource


def discover() -> dict[str, PanelSource]:
    registry: dict[str, PanelSource] = {}
    for module_info in pkgutil.iter_modules(__path__, prefix=f"{__name__}."):
        if module_info.name.rsplit(".", 1)[-1] == "base":
            continue
        module = importlib.import_module(module_info.name)
        source = getattr(module, "SOURCE", None)
        if isinstance(source, PanelSource):
            if not source.name:
                raise ValueError(f"{module_info.name} defines SOURCE but PanelSource.name is empty")
            registry[source.name] = source
    return registry
