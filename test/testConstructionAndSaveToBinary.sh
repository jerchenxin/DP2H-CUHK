cd build

stdbuf -o 0 ./DP2H_MAP --filePath=/home/xchen/dataset/$1 --benchmark=construction  --save=true --showLabelNum=true > $1.construction