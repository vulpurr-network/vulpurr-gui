#!/usr/bin/env bash
# Build Vulpurr-linux-x64.deb and Vulpurr-linux-x64.AppImage from compiled Linux x64 binaries.
set -euo pipefail

BIN_DIR="${1:?Usage: build-packages.sh <bin_dir> <out_dir> <version> <build> <appicons_dir> <desktop_file>}"
OUT_DIR="${2:?missing out_dir}"
VERSION="${3:-0.18.5.3}"
BUILD="${4:-15}"
ICONS_DIR="${5:?missing appicons_dir}"
DESKTOP_IN="${6:?missing desktop_file}"

mkdir -p "$OUT_DIR"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# --- 1. Build .deb package ---
DEB="$WORK/deb"
mkdir -p "$DEB/DEBIAN" "$DEB/opt/vulpurr-wallet" "$DEB/usr/bin" "$DEB/usr/share/applications"
install -m 0755 "$BIN_DIR/vulpurr-wallet" "$DEB/opt/vulpurr-wallet/vulpurr-wallet"
install -m 0755 "$BIN_DIR/vulpurrd"       "$DEB/opt/vulpurr-wallet/vulpurrd"
install -m 0755 "$BIN_DIR/vulpurr-p2pool" "$DEB/opt/vulpurr-wallet/vulpurr-p2pool"

for b in vulpurr-wallet vulpurrd vulpurr-p2pool; do
  cat > "$DEB/usr/bin/$b" <<WRAP
#!/bin/sh
exec /opt/vulpurr-wallet/$b "\$@"
WRAP
  chmod 0755 "$DEB/usr/bin/$b"
done

for sz in 64 128 256; do
  install -D -m 0644 "$ICONS_DIR/${sz}x${sz}.png" "$DEB/usr/share/icons/hicolor/${sz}x${sz}/apps/vulpurr.png"
done

sed 's|^Exec=.*|Exec=/opt/vulpurr-wallet/vulpurr-wallet %u|; s|^Icon=.*|Icon=vulpurr|' "$DESKTOP_IN" > "$DEB/usr/share/applications/vulpurr.desktop"
printf 'MimeType=x-scheme-handler/vulpurr;x-scheme-handler/vulpurrseed;\n' >> "$DEB/usr/share/applications/vulpurr.desktop"
chmod 0644 "$DEB/usr/share/applications/vulpurr.desktop"

INST_KB=$(du -sk "$DEB/opt" | awk '{print $1}')
cat > "$DEB/DEBIAN/control" <<CTL
Package: vulpurr-wallet
Version: ${VERSION}-${BUILD}
Section: utils
Priority: optional
Architecture: amd64
Depends: libopengl0 | libgl1, libegl1, libxkbcommon0, libxkbcommon-x11-0, libxcb-cursor0, libxcb-icccm4, libxcb-image0, libxcb-keysyms1, libxcb-randr0, libxcb-render-util0, libxcb-shm0, libxcb-sync1, libxcb-xfixes0, libxcb-render0, libxcb-shape0, libxcb-xkb1, libxcb1
Maintainer: Vulpurr Network <vulpurr-network@users.noreply.github.com>
Installed-Size: ${INST_KB}
Homepage: https://vulpurr.com
Description: Vulpurr (VUP) Desktop Wallet, Daemon and P2Pool
 Private CPU-mineable cryptocurrency desktop wallet with a built-in node,
 one-click P2Pool mining, and signed auto-updates.
CTL
chmod 0644 "$DEB/DEBIAN/control"

dpkg-deb --build --root-owner-group "$DEB" "$OUT_DIR/Vulpurr-linux-x64.deb"

# --- 2. Build Type-2 AppImage ---
RUNTIME="${APPIMAGE_RUNTIME:-/root/vp/upd/appimage-runtime-x86_64}"
if [ ! -f "$RUNTIME" ]; then
  RUNTIME="$WORK/runtime-x86_64"
  curl -fsSL "https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64" -o "$RUNTIME"
fi

APP="$WORK/AppDir"
mkdir -p "$APP/usr/bin" "$APP/usr/share/applications" "$APP/usr/share/icons/hicolor/256x256/apps"
install -m 0755 "$BIN_DIR/vulpurr-wallet" "$APP/usr/bin/vulpurr-wallet"
install -m 0755 "$BIN_DIR/vulpurrd"       "$APP/usr/bin/vulpurrd"
install -m 0755 "$BIN_DIR/vulpurr-p2pool" "$APP/usr/bin/vulpurr-p2pool"

cat > "$APP/AppRun" <<'APPRUN'
#!/bin/sh
HERE="$(dirname "$(readlink -f "$0")")"
export PATH="$HERE/usr/bin:$PATH"
exec "$HERE/usr/bin/vulpurr-wallet" "$@"
APPRUN
chmod 0755 "$APP/AppRun"

install -m 0644 "$ICONS_DIR/256x256.png" "$APP/vulpurr.png"
install -m 0644 "$ICONS_DIR/256x256.png" "$APP/.DirIcon"
install -m 0644 "$ICONS_DIR/256x256.png" "$APP/usr/share/icons/hicolor/256x256/apps/vulpurr.png"

sed 's|^Exec=.*|Exec=vulpurr-wallet %u|; s|^Icon=.*|Icon=vulpurr|' "$DESKTOP_IN" > "$APP/vulpurr.desktop"
printf 'MimeType=x-scheme-handler/vulpurr;x-scheme-handler/vulpurrseed;\n' >> "$APP/vulpurr.desktop"
chmod 0644 "$APP/vulpurr.desktop"
cp "$APP/vulpurr.desktop" "$APP/usr/share/applications/vulpurr.desktop"

SQFS="$WORK/app.sqfs"
if mksquashfs "$APP" "$SQFS" -root-owned -noappend -comp zstd -Xcompression-level 15 >/dev/null 2>&1; then
  true
else
  rm -f "$SQFS"
  mksquashfs "$APP" "$SQFS" -root-owned -noappend -comp xz >/dev/null
fi

cat "$RUNTIME" "$SQFS" > "$OUT_DIR/Vulpurr-linux-x64.AppImage"
chmod 0755 "$OUT_DIR/Vulpurr-linux-x64.AppImage"
ls -lh "$OUT_DIR/Vulpurr-linux-x64.deb" "$OUT_DIR/Vulpurr-linux-x64.AppImage"
