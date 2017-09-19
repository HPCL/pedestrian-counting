
conf_1 = \
'''# settings for ped counter
# order and format:
# one line per item, for comments start line with '#'
# do not use blank lines
#
# video name or (soon) CAMERA
# background file name or NONE
# MAX_DIST_SQD
# SENSITIVITY_VALUE_1 only 3 colors to filter black (0), gray (128ish), or white(255)
# SENSITIVITY_VALUE_2 more filtering options
# BLUR_SIZE_1
# BLUR_SIZE_2
# MIN_OBJ_AREA
# background subtraction algorithm (M for MOG2, default will be non-adaptive)
# Tracking algorithm (D for dist, O for overlap)
#
'''


conf_2 = \
'''
NONE
#MDS
#6000000
400
# Sensitivity
100
30
# Blur Size
5
5
# MOA
70
# BS algo
M
# tracking
D
'''
n = 25
for i in xrange(19):
    f = open("dots_" + str(n) + ".txt", "w")
    f.write(conf_1 + "../test_videos/dots_" + str(n) + ".mp4" + conf_2)
    f.close()    
    n += 25


runners = \
['./counter.out dots_',
 './counter_LA.out dots_',
 './counter_KAL.out dots_',
 './counter_mkl.out dots_',
 './counter_atlas.out dots_', 
 './counter_openblas.out dots_' ]

n = 25
f = open("run_dots.sh", "w")
for i in range(19):
    f.write("echo\necho\necho " + str(n) + "\necho\necho\n")
    for j in runners:
        f.write(j + str(n) + ".txt\n")  
    f.write("echo\necho\necho\n")
    n += 25
f.close()
