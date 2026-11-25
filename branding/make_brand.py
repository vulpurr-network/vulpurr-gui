#!/usr/bin/env python3
"""Vulpurr branding generator: renders every branded image of the wallet from the Nocturnal Fox mark."""
import os, shutil, struct, subprocess, tempfile

BRAND = "#FF8A3D"   # fox amber
DARK = "#1A1030"    # nocturnal violet (dark wordmark)
LIGHT = "#FBF6FF"   # light wordmark
WORDMARK_FONT = "Manrope"

# Nocturnal account card gradients (start, end, glow accent) matching Style.qml accountColors
CARDS = [
    ("#3C2272", "#1A1030", "#FF8A3D"),
    ("#6B2248", "#211028", "#FF8A3D"),
    ("#1E5A4A", "#132528", "#7AF0C4"),
    ("#2A2F7A", "#161436", "#9668FF"),
    ("#7A3B1E", "#251328", "#FFC46B"),
    ("#5E2570", "#1F1133", "#FF7AA8"),
    ("#1C5268", "#132032", "#7AF0C4"),
    ("#352E4A", "#1A1429", "#B9A9D6"),
]

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG = os.path.join(ROOT, "images")
ORIG_IMG = "/root/gui-images-orig"

SILHOUETTE = (
    "M51 39 C60 39 94 69 107 90 Q128 82 149 90 C162 69 196 39 205 39 "
    "C211 53 211 99 202 131 L220 147 L200 167 L205 174 L182 182 L128 220 "
    "L74 182 L51 174 L56 167 L36 147 L54 131 C45 99 45 53 51 39 Z"
)

FOX_DEFS = """<radialGradient id="vbg" cx="38%" cy="28%" r="78%">
      <stop offset="0%" stop-color="#37216b"/>
      <stop offset="55%" stop-color="#201240"/>
      <stop offset="100%" stop-color="#120926"/>
    </radialGradient>
    <linearGradient id="vrim" x1="12%" y1="0%" x2="88%" y2="100%">
      <stop offset="0%" stop-color="#ffd280" stop-opacity="0.9"/>
      <stop offset="48%" stop-color="#ff8a3d" stop-opacity="0.65"/>
      <stop offset="100%" stop-color="#9668ff" stop-opacity="0.7"/>
    </linearGradient>
    <linearGradient id="vfur" x1="50%" y1="0%" x2="50%" y2="100%">
      <stop offset="0%" stop-color="#ffbe5c"/>
      <stop offset="45%" stop-color="#ff832e"/>
      <stop offset="100%" stop-color="#e84e1b"/>
    </linearGradient>
    <linearGradient id="vearL" x1="15%" y1="0%" x2="85%" y2="100%">
      <stop offset="0%" stop-color="#ffb44d"/>
      <stop offset="100%" stop-color="#e34d1a"/>
    </linearGradient>
    <linearGradient id="vearR" x1="85%" y1="0%" x2="15%" y2="100%">
      <stop offset="0%" stop-color="#ffc366"/>
      <stop offset="100%" stop-color="#eb5a22"/>
    </linearGradient>
    <linearGradient id="vcheek" x1="50%" y1="0%" x2="50%" y2="100%">
      <stop offset="0%" stop-color="#ffffff"/>
      <stop offset="100%" stop-color="#f2e8fb"/>
    </linearGradient>
    <linearGradient id="vcrown" x1="50%" y1="0%" x2="50%" y2="100%">
      <stop offset="0%" stop-color="#ffe7a8" stop-opacity="0.72"/>
      <stop offset="100%" stop-color="#ff8a3d" stop-opacity="0"/>
    </linearGradient>"""


def mark_body(with_defs=True):
    defs = f"<defs>{FOX_DEFS}</defs>" if with_defs else ""
    return f"""{defs}
  <circle cx="128" cy="128" r="123" fill="url(#vbg)" stroke="url(#vrim)" stroke-width="6"/>
  <g transform="translate(128 130) scale(0.94) translate(-128 -130)">
    <path fill="url(#vearL)" d="M51 39 C60 39 94 69 107 90 L54 131 C45 99 45 53 51 39 Z"/>
    <path fill="url(#vearR)" d="M205 39 C196 39 162 69 149 90 L202 131 C211 99 211 53 205 39 Z"/>
    <path fill="#1b1033" d="M61 62 C72 69 87 82 95 94 L65 114 C60 96 59 75 61 62 Z"/>
    <path fill="#1b1033" d="M195 62 C184 69 169 82 161 94 L191 114 C196 96 197 75 195 62 Z"/>
    <path fill="#ff7d47" opacity="0.72" d="M68 78 L89 94 L70 108 Z"/>
    <path fill="#ff7d47" opacity="0.72" d="M188 78 L167 94 L186 108 Z"/>
    <path fill="url(#vfur)" d="M104 88 Q128 81 152 88 L201 121 L220 147 L190 175 L128 220 L66 175 L36 147 L55 121 Z"/>
    <path fill="#d64015" opacity="0.22" d="M55 121 L36 147 L96 149 L104 88 Z"/>
    <path fill="#ffd98a" opacity="0.26" d="M201 121 L220 147 L160 149 L152 88 Z"/>
    <path fill="url(#vcrown)" d="M104 88 Q128 81 152 88 L139 160 L128 176 L117 160 Z"/>
    <path fill="url(#vcheek)" d="M36 147 C60 145 86 153 110 171 L128 184 L146 171 C170 153 196 145 220 147 L200 167 L205 174 L182 182 L128 220 L74 182 L51 174 L56 167 Z"/>
    <path fill="url(#vfur)" d="M113 151 L128 182 L143 151 Q128 159 113 151 Z"/>
    <path fill="#160c2b" d="M74 140 C85 127 105 130 112 145 C99 152 84 150 74 140 Z"/>
    <path fill="#160c2b" d="M182 140 C171 127 151 130 144 145 C157 152 172 150 182 140 Z"/>
    <circle cx="91" cy="138" r="3.4" fill="#ffffff"/>
    <circle cx="161" cy="138" r="3.4" fill="#ffffff"/>
    <path fill="#160c2b" d="M115 178 Q128 174 141 178 L131 193 Q128 196 125 193 Z"/>
    <ellipse cx="128" cy="180" rx="5.5" ry="1.8" fill="#ffffff" opacity="0.4"/>
  </g>"""


def svg(w, h, body, vb=None):
    viewbox = vb or f"0 0 {w} {h}"
    return f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="{viewbox}">{body}</svg>'


def render(svg_text, out, w, h):
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with tempfile.NamedTemporaryFile("w", suffix=".svg", delete=False) as f:
        f.write(svg_text)
        tmp = f.name
    try:
        subprocess.run(["rsvg-convert", "-w", str(w), "-h", str(h), tmp, "-o", out], check=True)
    finally:
        os.unlink(tmp)


def place(x, y, size, body):
    s = size / 256.0
    return f'<g transform="translate({x} {y}) scale({s})">{body}</g>'


def mark_svg():
    return svg(256, 256, mark_body(with_defs=True))


def card(start, end, glow, scale, light_mode=False):
    w, h = 260, 135
    wm_fill = "#1A1030" if light_mode else "#FFFFFF"
    wm_op = "0.06" if light_mode else "0.09"
    glow_op = "0.22" if light_mode else "0.32"
    sil_elem = f'<path fill="{wm_fill}" d="{SILHOUETTE}"/>'
    body = f"""<defs>
      <linearGradient id="cg" x1="0" y1="0" x2="1" y2="1">
        <stop offset="0%" stop-color="{start}"/>
        <stop offset="100%" stop-color="{end}"/>
      </linearGradient>
      <radialGradient id="cglow" cx="85%" cy="15%" r="72%">
        <stop offset="0%" stop-color="{glow}" stop-opacity="{glow_op}"/>
        <stop offset="100%" stop-color="{glow}" stop-opacity="0"/>
      </radialGradient>
      {FOX_DEFS}
      <clipPath id="cc"><rect width="{w}" height="{h}" rx="10"/></clipPath>
    </defs>
    <g clip-path="url(#cc)">
      <rect width="{w}" height="{h}" fill="url(#cg)"/>
      <rect width="{w}" height="{h}" fill="url(#cglow)"/>
      <g opacity="{wm_op}">{place(128, -10, 184, sil_elem)}</g>
    </g>
    {place(18, 18, 32, mark_body(with_defs=False))}"""
    return svg(w * scale, h * scale, body, f"0 0 {w} {h}")


def wordmark(w, h, text_color, with_mark=True, font_size=None):
    fs = font_size or h * 0.58
    x = h + h * 0.30 if with_mark else 0
    body = (f"<defs>{FOX_DEFS}</defs>" if with_mark else "") + \
        (place(0, 0, h, mark_body(with_defs=False)) if with_mark else "") + \
        f'<text x="{x:.1f}" y="{h * 0.71:.1f}" font-family="{WORDMARK_FONT}" font-weight="800" font-size="{fs:.1f}" letter-spacing="0.6" fill="{text_color}">VULPURR</text>'
    return svg(w, h, body)


def write_icns(out_path):
    entries = [
        (b"icp4", 16),
        (b"icp5", 32),
        (b"icp6", 64),
        (b"ic07", 128),
        (b"ic08", 256),
        (b"ic09", 512),
        (b"ic10", 1024),
    ]
    chunks = bytearray()
    with tempfile.TemporaryDirectory() as td:
        for ostype, sz in entries:
            png_path = os.path.join(td, f"{sz}.png")
            render(mark_svg(), png_path, sz, sz)
            data = open(png_path, "rb").read()
            chunks += ostype + struct.pack(">I", 8 + len(data)) + data
    with open(out_path, "wb") as f:
        f.write(b"icns" + struct.pack(">I", 8 + len(chunks)) + chunks)


def main():
    msvg = mark_svg()
    open(os.path.join(IMG, "monero-vector.svg"), "w").write(msvg)
    render(msvg, os.path.join(IMG, "moneroIcon-28x28.png"), 28, 28)
    render(msvg, os.path.join(IMG, "moneroIcon-28x28@2x.png"), 56, 56)

    ico_pngs = []
    for size in (16, 24, 32, 48, 64, 96, 128, 256, 512):
        out = os.path.join(IMG, "appicons", f"{size}x{size}.png")
        render(msvg, out, size, size)
        if size <= 256:
            ico_pngs.append(out)
    subprocess.run(["convert"] + ico_pngs + [os.path.join(IMG, "appicon.ico")], check=True)
    write_icns(os.path.join(IMG, "appicon.icns"))

    for i, (start, end, glow) in enumerate(CARDS):
        for scale, suffix in ((1, ""), (2, "@2x")):
            render(card(start, end, glow, scale), os.path.join(IMG, f"card-background-black{i}{suffix}.png"), 260 * scale, 135 * scale)
    for scale, suffix in ((1, ""), (2, "@2x")):
        render(card("#FFFFFF", "#EFE8FB", "#FF8A3D", scale, light_mode=True), os.path.join(IMG, f"card-background-white{suffix}.png"), 260 * scale, 135 * scale)

    for scale, suffix in ((1, ""), (2, "@2x")):
        render(wordmark(125, 28, LIGHT), os.path.join(IMG, f"titlebarLogo{suffix}.png"), 125 * scale, 28 * scale)
        render(wordmark(125, 28, DARK), os.path.join(IMG, "themes", "white", f"titlebarLogo{suffix}.png"), 125 * scale, 28 * scale)
    render(wordmark(132, 22, LIGHT, with_mark=False, font_size=20), os.path.join(IMG, "moneroLogo_white.png"), 132, 22)
    open(os.path.join(IMG, "themes", "white", "Monero-Logo.svg"), "w").write(wordmark(282, 75, DARK, font_size=36))

    for size, cx, cy, d in ((32, 9.75, 24.25, 8.5), (64, 19.5, 48.5, 17)):
        fname = "miningxmr.png" if size == 32 else "miningxmr@2x.png"
        p = os.path.join(IMG, fname)
        orig = os.path.join(ORIG_IMG, fname)
        if os.path.exists(orig):
            shutil.copy2(orig, p)
        badge = p + ".badge.png"
        render(msvg, badge, int(round(d)), int(round(d)))
        subprocess.run(["convert", p, badge, "-geometry", f"+{int(round(cx - d / 2))}+{int(round(cy - d / 2))}", "-composite", p], check=True)
        os.unlink(badge)
    print("branding rendered with Nocturnal Fox", BRAND)


if __name__ == "__main__":
    main()
