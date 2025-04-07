BIN=build

export PMIX_MCA_pcompress_base_silence_warning=1

dest=""
out=${3:-""}

POINT_CNT=${POINT_CNT:-"5000"}
LAYER_CNT=${LAYER_CNT:-"5001"}
T_STEP=${T_STEP:-"0.005"}
X_STEP=${X_STEP:-"0.01"}
EVERY_N=${EVERY_N:-$(( $LAYER_CNT / 20 ))}

if [ "$out" = "--png" ]; then
    dest="&> .tmp/out.data"
fi

if [ $2 = "diff" ]; then
    echo "time mpirun -n $1 $BIN/diffusion $POINT_CNT $LAYER_CNT $T_STEP $X_STEP $EVERY_N $dest"
    eval "time mpirun -n $1 $BIN/diffusion $POINT_CNT $LAYER_CNT $T_STEP $X_STEP $EVERY_N $dest" &&
    if [ $3 = "--png" ]; then
        eval "./.venv/bin/python3 plot.py"
    fi
fi

if [ $2 = "pi" ]; then
    if [ $3 = "--png" ]; then
        echo "--png is for \"diff\" exeution"
    fi
    eval "mpirun -n $1 $BIN/pi_eval"
fi

if [ $2 = "time" ]; then
    if [ $3 = "--png" ]; then
        echo "--png is for \"diff\" exeution"
    fi
    eval "mpirun -n $1 $BIN/timer"
fi
