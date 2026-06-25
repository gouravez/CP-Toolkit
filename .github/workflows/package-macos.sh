#!/usr/bin/env bash
# =============================================================================
# package-macos.sh — Build CPT and produce a distributable .dmg on macOS
# =============================================================================
# Usage:
#   chmod +x package-macos.sh
#   ./package-macos.sh [version]
#
# Examples:
#   ./package-macos.sh            # version = "dev"
#   ./package-macos.sh v1.0.0    # version = "v1.0.0"
#
# Requirements (install once):
#   brew install cmake qt@6 create-dmg
# =============================================================================

set -euo pipefail

# ── Config ────────────────────────────────────────────────────────────────────
VERSION="${1:-dev}"
APP_NAME="CPT"
DMG_NAME="${APP_NAME}-${VERSION}-macOS.dmg"
BUILD_DIR="build-release"
ICON_PATH="resources/icons/cpt.icns"

# ── Preflight checks ──────────────────────────────────────────────────────────
for tool in cmake; do
  if ! command -v "$tool" &>/dev/null; then
    echo "❌  Required tool not found: $tool"
    echo "    Install with: brew install cmake"
    exit 1
  fi
done

# Detect Qt prefix from Homebrew (works for both Apple Silicon and Intel)
detect_qt_prefix() {
  # Apple Silicon
  if [[ -d /opt/homebrew/opt/qt ]]; then
    echo "/opt/homebrew/opt/qt"
  # Intel
  elif [[ -d /usr/local/opt/qt ]]; then
    echo "/usr/local/opt/qt"
  # Qt installed via aqtinstall or custom path
  elif command -v qmake6 &>/dev/null; then
    dirname "$(dirname "$(which qmake6)")"
  elif command -v qmake &>/dev/null; then
    dirname "$(dirname "$(which qmake)")"
  else
    echo ""
  fi
}

QT_PREFIX="$(detect_qt_prefix)"

if [[ -z "$QT_PREFIX" ]]; then
  echo "❌  Qt 6 not found. Install it with:"
  echo "    brew install qt@6"
  exit 1
fi

echo "✅  Qt prefix : $QT_PREFIX"
echo "✅  Version   : $VERSION"
echo "✅  DMG name  : $DMG_NAME"
echo ""

# ── 1. Build ──────────────────────────────────────────────────────────────────
echo "🔨  Configuring CMake..."
cmake -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX"

echo "🔨  Compiling..."
cmake --build "$BUILD_DIR" --config Release -j"$(sysctl -n hw.logicalcpu)"

APP_PATH="${BUILD_DIR}/${APP_NAME}.app"

if [[ ! -d "$APP_PATH" ]]; then
  echo "❌  Build succeeded but ${APP_NAME}.app not found at ${APP_PATH}"
  exit 1
fi

echo "✅  Build complete: $APP_PATH"

# ── 2. Bundle Qt frameworks ───────────────────────────────────────────────────
MACDEPLOYQT="${QT_PREFIX}/bin/macdeployqt"

if [[ ! -x "$MACDEPLOYQT" ]]; then
  echo "❌  macdeployqt not found at $MACDEPLOYQT"
  echo "    Make sure Qt is installed via Homebrew: brew install qt@6"
  exit 1
fi

echo ""
echo "📦  Bundling Qt frameworks with macdeployqt..."
"$MACDEPLOYQT" "$APP_PATH" -verbose=1

# ── 3. Create DMG ─────────────────────────────────────────────────────────────
echo ""
echo "💿  Creating DMG..."

# Remove stale DMG
rm -f "$DMG_NAME"

# Resolve volicon flag
VOLICON_FLAG=()
if [[ -f "$ICON_PATH" ]]; then
  VOLICON_FLAG=(--volicon "$ICON_PATH")
else
  echo "⚠️   Icon not found at $ICON_PATH — DMG will use default volume icon"
fi

if command -v create-dmg &>/dev/null; then
  # Pretty DMG with drag-to-Applications UI
  create-dmg \
    --volname "CP Toolkit" \
    "${VOLICON_FLAG[@]}" \
    --window-pos 200 120 \
    --window-size 600 400 \
    --icon-size 100 \
    --icon "${APP_NAME}.app" 175 190 \
    --hide-extension "${APP_NAME}.app" \
    --app-drop-link 425 190 \
    --no-internet-enable \
    "$DMG_NAME" \
    "$APP_PATH"
  EXIT_CODE=$?

  # Exit code 2 = no code signing, DMG is still valid
  if [[ $EXIT_CODE -ne 0 && $EXIT_CODE -ne 2 ]]; then
    echo "❌  create-dmg failed with exit code $EXIT_CODE"
    exit $EXIT_CODE
  fi

  # If create-dmg still didn't produce the file, fall back
  if [[ ! -f "$DMG_NAME" ]]; then
    echo "⚠️   create-dmg didn't produce a file; falling back to hdiutil..."
    hdiutil create -volname "CP Toolkit" \
      -srcfolder "$APP_PATH" \
      -ov -format UDZO \
      "$DMG_NAME"
  fi
else
  echo "ℹ️   create-dmg not found; using hdiutil (install create-dmg for a nicer DMG):"
  echo "    brew install create-dmg"
  hdiutil create -volname "CP Toolkit" \
    -srcfolder "$APP_PATH" \
    -ov -format UDZO \
    "$DMG_NAME"
fi

# ── 4. Summary ────────────────────────────────────────────────────────────────
echo ""
echo "═══════════════════════════════════════════════"
echo "✅  Done!  →  $(pwd)/${DMG_NAME}"
DMG_SIZE=$(du -sh "$DMG_NAME" | cut -f1)
echo "    Size: ${DMG_SIZE}"
echo "═══════════════════════════════════════════════"
echo ""
echo "To test: open \"$DMG_NAME\""