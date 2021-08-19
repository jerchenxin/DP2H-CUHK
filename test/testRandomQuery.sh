cd build

stdbuf -o 0 ./DP2H --filePath=/home/xchen/dataset/$1 --benchmark=query --loadBinary=true --num=$2  --showLabelNum=true > $1.query.$2.binary