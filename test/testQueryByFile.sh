cd ../build

stdbuf -o 0 ./DP2H --filePath=../test/$1 --benchmark=query --loadBinary=true --queryByFile=true  --showLabelNum=true > $1.queryByFile.binary