SOURCE="${BASH_SOURCE[0]}"
SOURCE_DIR=`dirname $SOURCE`
export LIQUIBOOK_ROOT=`readlink -f $SOURCE_DIR`
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIQUIBOOK_ROOT/lib