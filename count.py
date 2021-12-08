import random
import sys
import math

in_file = sys.argv[1]
out_file = sys.argv[2]
skip_line = int(sys.argv[3])

zero_flag = False

max_num = 0

max_label = 0

result = []

with open(in_file, "r") as f:
    for i in range(skip_line):
        f.readline()
        
    line = f.readline()
    while line:
        tmp = line.split()
        
        u = int(tmp[0])
        v = int(tmp[1])
        label = int(tmp[2])

        if u == 0 or v == 0:
            zero_flag = True

        max_num = max(max_num, u, v)
        max_label = max(max_label, label)
        result.append([u, v, label])
        
        line = f.readline()

    if zero_flag:
        max_num = max_num + 1

        for i in result:
            i[0] = i[0] + 1
            i[1] = i[1] + 1

result = sorted(result)

with open(out_file, "w") as f:
    f.write("{} {} {}\n".format(max_num, len(result), max_label + 1))

    for i in result:
        f.write("{} {} {}\n".format(i[0], i[1], i[2]))
