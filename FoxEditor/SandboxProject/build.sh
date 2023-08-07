echo `dirname "$0"`
cd -- "$(dirname -- "$0")"
premake5 gmake
make
cd -
