cd ../build

stdbuf -o 0 ./DP2H --filePath=../test/$1 --benchmark=together --loadBinary=true --round=$2  --showLabelNum=true > $1.together.$2.binary