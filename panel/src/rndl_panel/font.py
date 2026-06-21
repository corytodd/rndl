"""3x5 pixel font for rendering digits on the panel."""

from .common import PANEL_HEIGHT, PANEL_WIDTH

GLYPH_WIDTH = 3
GLYPH_HEIGHT = 5

_DIGITS = {
    "0": ["111", "101", "101", "101", "111"],
    "1": ["010", "110", "010", "010", "111"],
    "2": ["111", "001", "111", "100", "111"],
    "3": ["111", "001", "111", "001", "111"],
    "4": ["101", "101", "111", "001", "001"],
    "5": ["111", "100", "111", "001", "111"],
    "6": ["111", "100", "111", "101", "111"],
    "7": ["111", "001", "001", "001", "001"],
    "8": ["111", "101", "111", "101", "111"],
    "9": ["111", "101", "111", "001", "111"],
    "-": ["000", "000", "111", "000", "000"],
    " ": ["000", "000", "000", "000", "000"],
}


def draw_glyph(frame: bytearray, ch: str, x0: int, y0: int, color: tuple[int, int, int]) -> None:
    glyph = _DIGITS.get(ch)
    if glyph is None:
        raise ValueError(f"unsupported glyph: {ch!r}")
    for row_i, row in enumerate(glyph):
        y = y0 + row_i
        if y < 0 or y >= PANEL_HEIGHT:
            continue
        for col_i, bit in enumerate(row):
            if bit != "1":
                continue
            x = x0 + col_i
            if x < 0 or x >= PANEL_WIDTH:
                continue
            idx = (y * PANEL_WIDTH + x) * 3
            frame[idx : idx + 3] = bytes(color)


def draw_text(frame: bytearray, text: str, x0: int, y0: int, color: tuple[int, int, int]) -> None:
    """Draw left-to-right, GLYPH_WIDTH+1 px per char."""
    for i, ch in enumerate(text):
        draw_glyph(frame, ch, x0 + i * (GLYPH_WIDTH + 1), y0, color)
