import random
import sys

in_file = sys.argv[1]

graph = {}

with open(in_file, "r") as f:
    f.readline()
    
    line = f.readline()
    while line:
        tmp = line.split()
        
        u = int(tmp[0])
        v = int(tmp[1])
        label = int(tmp[2])
        
        if not graph.get(label):
            graph[label] = [(u, v), ]
        else:
            graph[label].append((u, v))
        
        line = f.readline()

keys = graph.keys()
key_num = len(keys)

label_list = random.sample(keys, 500)
random.shuffle(label_list)

for num in range(0, 5):
    with open(in_file + ".cut." + str(num), "w") as f:
        for index in range(100*num, 100*(num+1)):
            i = label_list[index]
            for j in graph[i]:
                f.write("{} {} {}\n".format(j[0], j[1], i))


