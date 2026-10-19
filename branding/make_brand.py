#!/usr/bin/env python3
"""Vulpurr branding generator: renders every branded image of the wallet from one vector mark.
Change the palette below and re-run to recolor everything."""
import os, subprocess, tempfile

BRAND = "#FF7A1A"   # fox orange (temporary until the palette is chosen)
DARK = "#1E1A24"    # eyes, nose, dark wordmark
LIGHT = "#FFFFFF"
WORDMARK_FONT = "URW Gothic"
# account card gradients (start colour, matches the GUI's account colour order)
CARDS = ["#654F40", "#752C33", "#48752C", "#672C75", "#343072", "#75642C", "#2C7275", "#656565"]

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG = os.path.join(ROOT, "images")

HEAD = "M58 54 L106 104 L150 104 L198 54 L204 134 C204 146 199 156 190 164 L128 216 L66 164 C57 156 52 146 52 134 Z"


def mark(circle=BRAND, head=LIGHT, ears=None, eyes=DARK, whiskers=None):
    ears = ears or circle
    whiskers = whiskers or head
    return f'''<circle cx="128" cy="128" r="128" fill="{circle}"/>
  <path fill="{head}" d="{HEAD}"/>
  <path fill="{ears}" d="M72 82 L94 106 L76 120 Z"/><path fill="{ears}" d="M184 82 L162 106 L180 120 Z"/>
  <path fill="{eyes}" d="M88 142 C96 128 116 128 122 142 C114 152 96 152 88 142 Z"/>
  <path fill="{eyes}" d="M168 142 C160 128 140 128 134 142 C142 152 160 152 168 142 Z"/>
  <path fill="{eyes}" d="M118 176 L138 176 L128 188 Z"/>
  <g stroke="{whiskers}" stroke-width="6" stroke-linecap="round">
    <line x1="60" y1="170" x2="22" y2="160"/><line x1="62" y1="182" x2="26" y2="186"/>
    <line x1="196" y1="170" x2="234" y2="160"/><line x1="194" y1="182" x2="230" y2="186"/>
  </g>'''


def svg(w, h, body, vb=None):
    return f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="{vb or f"0 0 {w} {h}"}">{body}</svg>'


def render(svg_text, out, w, h):
    with tempfile.NamedTemporaryFile("w", suffix=".svg", delete=False) as f:
        f.write(svg_text)
    subprocess.run(["rsvg-convert", "-w", str(w), "-h", str(h), f.name, "-o", out], check=True)
    os.unlink(f.name)


def place(x, y, size, body):
    s = size / 256.0
    return f'<g transform="translate({x} {y}) scale({s})">{body}</g>'


def mark_svg():
    return svg(256, 256, mark())


def card(start, end, icon_head, scale):
    w, h = 260, 135
    body = f'''<defs><linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0" stop-color="{start}"/><stop offset="1" stop-color="{end}"/></linearGradient>
      <clipPath id="c"><rect width="{w}" height="{h}" rx="10"/></clipPath></defs>
    <g clip-path="url(#c)">
      <rect width="{w}" height="{h}" fill="url(#g)"/>
      <g opacity="0.07">{place(118, -6, 190, f'<path fill="#FFFFFF" d="{HEAD}"/>')}</g>
    </g>
    {place(20, 22, 30, mark(circle=LIGHT, head=icon_head, ears=LIGHT, eyes=LIGHT, whiskers=LIGHT))}'''
    return svg(w * scale, h * scale, body, f"0 0 {w} {h}")


def wordmark(w, h, text_color, with_mark=True, font_size=None):
    fs = font_size or h * 0.70
    x = h + h * 0.35 if with_mark else 0
    body = (place(0, 0, h, mark()) if with_mark else "") + \
        f'<text x="{x}" y="{h * 0.80}" font-family="{WORDMARK_FONT}" font-weight="600" font-size="{fs}" textLength="{w - x - 1}" lengthAdjust="spacingAndGlyphs" fill="{text_color}">VULPURR</text>'
    return svg(w, h, body)


def main():
    # vector mark (splash) and small icons
    open(os.path.join(IMG, "monero-vector.svg"), "w").write(mark_svg())
    render(mark_svg(), os.path.join(IMG, "moneroIcon-28x28.png"), 28, 28)
    render(mark_svg(), os.path.join(IMG, "moneroIcon-28x28@2x.png"), 56, 56)

    # app icons
    pngs = []
    for size in (16, 24, 32, 48, 64, 96, 128, 256):
        out = os.path.join(IMG, "appicons", f"{size}x{size}.png")
        render(mark_svg(), out, size, size)
        pngs.append(out)
    subprocess.run(["convert"] + pngs + [os.path.join(IMG, "appicon.ico")], check=True)

    # account cards
    for i, start in enumerate(CARDS):
        for scale, suffix in ((1, ""), (2, "@2x")):
            render(card(start, "#4E4E4E", start, scale), os.path.join(IMG, f"card-background-black{i}{suffix}.png"), 260 * scale, 135 * scale)
    for scale, suffix in ((1, ""), (2, "@2x")):
        white = card("#F0F0F0", "#FFFFFF", BRAND, scale).replace('fill="#FFFFFF" d="M58', 'fill="#000000" d="M58')
        white = white.replace(f'<circle cx="128" cy="128" r="128" fill="{LIGHT}"/>', f'<circle cx="128" cy="128" r="128" fill="{BRAND}"/>', 1)
        white = white.replace(f'<path fill="{BRAND}" d="{HEAD}"/>', f'<path fill="{LIGHT}" d="{HEAD}"/>', 1)
        render(white, os.path.join(IMG, f"card-background-white{suffix}.png"), 260 * scale, 135 * scale)

    # title bar logos (dark theme: white text, light theme: dark text)
    for scale, suffix in ((1, ""), (2, "@2x")):
        render(wordmark(125, 28, LIGHT), os.path.join(IMG, f"titlebarLogo{suffix}.png"), 125 * scale, 28 * scale)
        render(wordmark(125, 28, DARK), os.path.join(IMG, "themes", "white", f"titlebarLogo{suffix}.png"), 125 * scale, 28 * scale)
    render(wordmark(132, 22, LIGHT, with_mark=False, font_size=20), os.path.join(IMG, "moneroLogo_white.png"), 132, 22)

    # mining pickaxe: cover the old badge with our mark
    for size, cx, cy, d in ((32, 9.75, 24.25, 8.5), (64, 19.5, 48.5, 17)):
        p = os.path.join(IMG, "miningxmr.png" if size == 32 else "miningxmr@2x.png")
        badge = p + ".badge.png"
        render(mark_svg(), badge, int(round(d)), int(round(d)))
        subprocess.run(["convert", p, badge, "-geometry", f"+{int(round(cx - d / 2))}+{int(round(cy - d / 2))}", "-composite", p], check=True)
        os.unlink(badge)
    print("branding rendered with", BRAND)


if __name__ == "__main__":
    main()
