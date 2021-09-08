cd ../build

stdbuf -o 0 ./DP2H --filePath=../test/$1 --benchmark=construction  --save=true --showLabelNum=true > $1.construction