#!/usr/bin/env python3
"""Vulpurr: rebrand user-visible text. Touches only string literals in QML/JS and the
<source>/<translation> text in .ts files, using the same rules so translations keep matching."""
import glob, os, re, sys

ROOT = sys.argv[1]
RULES = [
    (re.compile(r"\bMonero\b"), "Vulpurr"),
    (re.compile(r"\bXMR\b"), "VUP"),
    (re.compile(r"\bМонеро\b|\bмонеро\b"), "Vulpurr"),
]


def apply(text):
    for rx, repl in RULES:
        text = rx.sub(repl, text)
    return text


STRING = re.compile(r'"(?:[^"\\\n]|\\.)*"')
changed = 0

for path in glob.glob(os.path.join(ROOT, "**", "*.qml"), recursive=True) + glob.glob(os.path.join(ROOT, "**", "*.js"), recursive=True):
    rel = os.path.relpath(path, ROOT)
    if rel.startswith(("build", "monero" + os.sep, "branding")):
        continue
    src = open(path, encoding="utf-8").read()
    out = STRING.sub(lambda m: apply(m.group(0)), src)
    if out != src:
        open(path, "w", encoding="utf-8").write(out)
        changed += 1

TS_TEXT = re.compile(r"(<(source|translation)[^>]*>)(.*?)(</\2>)", re.S)
for path in glob.glob(os.path.join(ROOT, "translations", "*.ts")):
    src = open(path, encoding="utf-8").read()
    out = TS_TEXT.sub(lambda m: m.group(1) + apply(m.group(3)) + m.group(4), src)
    if out != src:
        open(path, "w", encoding="utf-8").write(out)
        changed += 1

print("files changed:", changed)
