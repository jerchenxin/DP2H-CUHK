cd ../build

stdbuf -o 0 ./DP2H --filePath=../test/$1 --benchmark=gen --loadBinary=true --num=$2  --showLabelNum=true > $1.gen.$2.binary