set -ex

OWN_PATH=`realpath $0`

ROOT_DIR=`readlink -f "$(dirname $OWN_PATH)/../`
SRC_DIR="$ROOT_DIR/src"
DIST_DIR="$ROOT_DIR/dist"

