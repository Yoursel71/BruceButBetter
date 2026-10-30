#!/usr/bin/env bash
# Publish a release's board images to the browser flasher.
#
# The flasher page (docs/index.html) reads versions.json and the images from the
# Yoursel71/BruceButBetter-bins repo (served on GitHub Pages). This script downloads the
# ESP32 / ESP32-S3 images attached to a release, adds them as a <tag>/ folder there, lists the
# version in versions.json and makes it the latest. The page only offers those two chip
# families, so other images (ESP32-C5, ...) are not copied.
#
# Usage:  tools/publish_web_flasher.sh v1.4.1
# Needs:  git, gh (logged in, push access to the bins repo), node
set -euo pipefail

TAG="${1:?usage: $0 <release tag, e.g. v1.4.1>}"
REPO="Yoursel71/BruceButBetter"
BINS="Yoursel71/BruceButBetter-bins"
ROOT="$(git rev-parse --show-toplevel)"
AUTHOR_NAME="$(git -C "$ROOT" config user.name || true)"
AUTHOR_EMAIL="$(git -C "$ROOT" config user.email || true)"
if [ -z "$AUTHOR_NAME" ] || [ -z "$AUTHOR_EMAIL" ]; then
    echo "Set git user.name and user.email in $ROOT first; the bins commit uses them." >&2
    exit 1
fi
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# Chip family of every board, from the release matrix: - { env: "<env>", family: "<family>",}
sed -n 's/.*env: *"\([^"]*\)", *family: *"\([^"]*\)".*/\1 \2/p' \
    "$ROOT/.github/workflows/release.yml" > "$WORK/families.txt"

git clone --quiet --depth 1 --filter=blob:none --sparse "https://github.com/$BINS.git" "$WORK/bins"
cd "$WORK/bins"
# MSYS_NO_PATHCONV stops Git Bash on Windows from rewriting "/versions.json" into a Windows path.
MSYS_NO_PATHCONV=1 git sparse-checkout set --no-cone /versions.json "/$TAG/"

if [ ! -f versions.json ]; then
    echo "versions.json is missing from $BINS; not publishing." >&2
    exit 1
fi
if node -e 'process.exit(require("./versions.json").versions.some(v => v.id === process.argv[1]) ? 0 : 1)' "$TAG"; then
    echo "$TAG is already in versions.json; nothing to do." >&2
    exit 1
fi

mkdir -p "$TAG"
gh release download "$TAG" --repo "$REPO" --pattern 'Bruce-*.bin' --dir "$TAG"

: > "$WORK/boards.txt"
for f in "$TAG"/Bruce-*.bin; do
    env="${f#"$TAG"/Bruce-}"
    env="${env%.bin}"
    family="$(awk -v e="$env" '$1 == e { print $2 }' "$WORK/families.txt")"
    case "$family" in
        ESP32 | ESP32-S3) echo "$env $family" >> "$WORK/boards.txt" ;;
        *) rm -f "$f" ;;
    esac
done

if ! grep -q '^esp32-s3-devkitc-1-psram ' "$WORK/boards.txt"; then
    echo "The release has no Bruce-esp32-s3-devkitc-1-psram.bin; not publishing." >&2
    exit 1
fi

node - "$TAG" "$WORK/boards.txt" <<'JS'
const fs = require("fs");
const [tag, boardsFile] = process.argv.slice(2);
const boards = fs.readFileSync(boardsFile, "utf8").trim().split("\n").map(line => {
    const [env, chip] = line.split(" ");
    return { env, chip };
}).sort((a, b) => a.env.toLowerCase().localeCompare(b.env.toLowerCase()));
const data = JSON.parse(fs.readFileSync("versions.json", "utf8"));
data.versions.unshift({ id: tag, label: tag, dir: tag, boards });
data.latest = tag;
fs.writeFileSync("versions.json", JSON.stringify(data, null, 1));
console.log(`${tag}: ${boards.length} boards`);
JS

count="$(wc -l < "$WORK/boards.txt" | tr -d ' ')"
git add -- versions.json "$TAG"
message="bins: add $TAG ($count boards) and make it the latest"
# Optional trailer lines (e.g. Co-Authored-By) for the bins commit.
if [ -n "${BINS_COMMIT_TRAILER:-}" ]; then message="$message"$'\n\n'"$BINS_COMMIT_TRAILER"; fi
git -c user.name="$AUTHOR_NAME" -c user.email="$AUTHOR_EMAIL" commit --quiet -m "$message"
git push --quiet origin HEAD
echo "Pushed $TAG to $BINS. GitHub Pages serves it at https://yoursel71.github.io/BruceButBetter-bins/ after its build."
