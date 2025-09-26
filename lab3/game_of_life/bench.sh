set -euo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"

cp $SCRIPT_DIR/initial_state.txt $BUILD_DIR

for ((i=1; i<11; i++)); do
  num=$(($i * 500))
  echo "BENCH ${i} -> $num iterations"
  echo "serial:"
  time $BUILD_DIR/game_of_life_serial $num
  echo "parallel:"
  time $BUILD_DIR/game_of_life_parallel $num
  echo; echo
done


