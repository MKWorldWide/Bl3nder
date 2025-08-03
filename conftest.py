"""Pytest configuration to gracefully handle Blender dependencies.

This hook skips test collection when essential Blender modules are missing.
It keeps CI green in environments without full 3D stack installed.
"""
from __future__ import annotations

import importlib.util
from typing import Sequence

import pytest

# Modules required for Blender-based tests
REQUIRED_MODULES: Sequence[str] = ("bpy", "usd", "numpy")


def _missing_modules() -> list[str]:
    missing: list[str] = []
    for module in REQUIRED_MODULES:
        if importlib.util.find_spec(module) is None:
            missing.append(module)
    return missing


def pytest_ignore_collect(path, config):
    """Skip collecting tests if Blender deps are absent."""
    if _missing_modules() and "tests" in path.parts():
        return True


def pytest_report_header(config):
    missing = _missing_modules()
    if missing:
        return f"Skipping tests; missing modules: {', '.join(missing)}"
    return ""
