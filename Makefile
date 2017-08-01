CC = icpc
# CC = g++

#TODO figure out which I actually need
# note: order may or may not matter
ALL_LIBS  = -lopencv_imgproc # 
ALL_LIBS += -lopencv_highgui # I think this one lets us get keyboard input
ALL_LIBS += -lopencv_objdetect #
ALL_LIBS += -lopencv_core -lopencv_videoio # basic opencv
ALL_LIBS += -lopencv_video # background subtractor MOG2
ALL_LIBS += -lopencv_imgcodecs #
# ALL_LIBS += -lraspicam -lraspicam_cv # to live stream with raspicam not on arya
# OCV_PATH  = -I/usr/include/ # on pi
OCV_PATH  = -L/usr/local/packages/opencv/lib # on arya 
OCV_PATH += -I/usr/local/packages/opencv/include # on arya

KALMAN_PATH = ~/kalman-filters/

KALMAN_FILES  = $(KALMAN_PATH)basic-c/linear_algebra.o # 
KALMAN_FILES += $(KALMAN_PATH)basic-c/kalman_filter.o #

BLAS_KAL_FILES  = kalman_filter.o #
CBLAS_LINK  = -L/usr/lib/ -llapacke -lopenblas
CBLAS_LINK += -llapacke
CBLAS_LINK += -lopenblas

MKL_LINK = -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl #
MKL_OPT  = -DMKL_ILP64 -I${MKLROOT}/include #
MKL_SYS  = -DSYS=\"sys_mkl.h\"

ATLAS_LINK = -L${ATLASLIB} -lcblas -llapack -lptcblas -lpthread -latlas
ATLAS_INC  = -I${ATLASINC}
ATLAS_SYS  = -DSYS=\"sys_atlas.h\"

CBLAS_LINK = -I${OPENBLASINC} -L${OPENBLASLIB} -lopenblas -lgfortran
CBLAS_SYS  = -DSYS=\"sys_cblas.h\"

TUNED_KAL_FILES  = $(KALMAN_PATH)basic-c/tuning_kalman/non_tuned.o 
# TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/_predict.o 
# TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/_correct.o 
TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/predict.o 
TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/correct.o 

TUNED_LA_FILES  = $(KALMAN_PATH)/basic-c/tuning/non_tuned.o
TUNED_LA_FILES += $(KALMAN_PATH)/basic-c/tuning/_add_mat.o
TUNED_LA_FILES += $(KALMAN_PATH)/basic-c/tuning/_compute_LUP_inline.o
TUNED_LA_FILES += $(KALMAN_PATH)/basic-c/tuning/_multiply_matrix.o
TUNED_LA_FILES += $(KALMAN_PATH)/basic-c/tuning/_multiply_matrix_by_scalar.o
TUNED_LA_FILES += $(KALMAN_PATH)/basic-c/tuning/_transpose_matrix.o
TUNED_LA_FILES += $(KALMAN_PATH)basic-c/kalman_filter.o  # 

all: counter counter_KAL counter_LA counter_mkl counter_atlas counter_openblas 
		./counter.out config_recommended.txt
		./counter_LA.out config_recommended.txt
		./counter_KAL.out config_recommended.txt
		./counter_mkl.out config_recommended.txt
		./counter_atlas.out config_recommended.txt
		./counter_openblas.out config_recommended.txt


object.o: object.cpp object.h
		$(CC) $(OCV_PATH) -c object.cpp

useful_functions.o: useful_functions.cpp useful_functions.h
		$(CC) $(OCV_PATH) -c useful_functions.cpp 

useful_tests: useful_functions.cpp useful_tests.cpp useful_functions.h
		make useful_functions.o
		$(CC) $(OCV_PATH) useful_tests.cpp useful_functions.o -g -o useful_tests.out $(ALL_LIBS)

image_input.o: image_input.cpp image_input.h
		$(CC) $(OCV_PATH) -c image_input.cpp 

image_output.o: image_output.cpp image_output.h
		$(CC) $(OCV_PATH) -c image_output.cpp 

trackers.o: trackers.cpp trackers.h
		$(CC) $(OCV_PATH) -c trackers.cpp

Target.o: Target.cpp Target.hpp
		$(CC) $(OCV_PATH) -c Target.cpp

counter: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.o object.o image_input.o image_output.o trackers.o Target.o $(KALMAN_FILES) -o counter.out $(ALL_LIBS)

counter_tau: counter.cpp
	  gcc -c -g $(KALMAN_PATH)basic-c/linear_algebra.c 
	  gcc -c -g $(KALMAN_PATH)basic-c/kalman_filter.c
		taucxx $(OCV_PATH) counter.cpp useful_functions.cpp object.cpp image_input.cpp image_output.cpp trackers.cpp Target.cpp kalman_filter.o linear_algebra.o -o counter.out $(ALL_LIBS)

counter_gprof: counter.cpp
	  gcc -c -g -pg $(KALMAN_PATH)basic-c/linear_algebra.c 
	  gcc -c -g -pg $(KALMAN_PATH)basic-c/kalman_filter.c
		g++ -pg $(OCV_PATH) counter.cpp useful_functions.cpp object.cpp image_input.cpp image_output.cpp trackers.cpp Target.cpp kalman_filter.o linear_algebra.o -o counter.out $(ALL_LIBS)

counter_mkl: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
	  icc $(KALMAN_PATH)kalman-blas/kalman_filter.c -c $(MKL_SYS) $(MKL_OPT) $(MKL_LINK)
		$(CC) $(OCV_PATH) $(CBLAS_LINK) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(BLAS_KAL_FILES) -o counter_mkl.out $(ALL_LIBS) $(MKL_SYS) $(MKL_OPT) $(MKL_LINK)

counter_atlas: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
	  icc $(KALMAN_PATH)kalman-blas/kalman_filter.c -c $(ATLAS_SYS) $(ATLAS_LINK) $(ATLAS_INC)
		$(CC) $(OCV_PATH) $(CBLAS_LINK) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(BLAS_KAL_FILES) -o counter_atlas.out $(ALL_LIBS) $(ATLAS_SYS) $(ATLAS_LINK) $(ATLAS_INC)

counter_openblas: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
	  icc $(KALMAN_PATH)kalman-blas/kalman_filter.c -c $(CBLAS_LINK) $(CBLAS_SYS) 
		$(CC) $(OCV_PATH) $(CBLAS_LINK) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(BLAS_KAL_FILES) -o counter_openblas.out $(ALL_LIBS) $(CBLAS_LINK) $(CBLAS_SYS)

counter_LA: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_LA_FILES) -o counter_LA.out $(ALL_LIBS)

counter_KAL: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_KAL_FILES) -o counter_KAL.out $(ALL_LIBS)

counter_gdb:
	  $(CC) -g $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o -o counter.out $(ALL_LIBS)

get_background: get_background.cpp
		make useful_functions.o
		$(CC) $(OCV_PATH) get_background.cpp useful_functions.cpp -o get_background.out $(ALL_LIBS)


clean:
	rm -f *.o *.out *.h264

