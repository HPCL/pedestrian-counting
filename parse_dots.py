
# runners = \
# ['./counter.out dots_',
#  './counter_LA.out dots_',
#  './counter_KAL.out dots_',
#  './counter_atlas.out dots_']

n = 25
fout = open("all_dots.csv", "w") 
fin  = open("all_dots.out")

fout.write(",base,atlas,LA,KAL\n")
out_li = []
n = 0
nn = 0

for line in fin:
    line_li = line.split()
    try:
        nn = int(line_li[0])
    except:
        if len(line_li) > 0 and line_li[0] == 't':
            out_li.append(line_li[4])
    else:
        if len(out_li) > 1:
            fout.write(str(n) + ',' + out_li[0] + ',' + out_li[3] + ',' + out_li[1] + ',' + out_li[2] + "\n")
        n = nn
        out_li = []

if len(out_li) > 1:
    fout.write(str(n) + ',' + out_li[0] + ',' + out_li[3] + ',' + out_li[1] + ',' + out_li[2] + "\n")
       
fin.close()
fout.close()
