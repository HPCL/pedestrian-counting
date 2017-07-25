CC = g++

#TODO figure out which I actually need
# note: order may or may not matter
ALL_LIBS  = -lopencv_imgproc # 
ALL_LIBS += -lopencv_highgui # I think this one lets us get keyboard input
ALL_LIBS += -lopencv_objdetect #
ALL_LIBS += -lopencv_core -lopencv_videoio # basic opencv
ALL_LIBS += -lopencv_video # background subtractor MOG2
ALL_LIBS += -lopencv_imgcodecs #
ALL_LIBS += -lraspicam -lraspicam_cv # to live stream with raspicam not on arya
OCV_PATH  = -I/usr/include/ # on pi
#OCV_PATH  = -L/usr/local/packages/opencv/lib # on arya 
#OCV_PATH += -I/usr/local/packages/opencv/include # on arya

KALMAN_PATH = ../kalman-filters/experiments/

KALMAN_FILES  = $(KALMAN_PATH)linear_algebra.o # 
KALMAN_FILES += $(KALMAN_PATH)kalman_filter.o #

BLAS_KAL_FILES  = ../kalman-filters/kalman-blas/kalman_filter.o #
CBLAS_LINK  = -L/usr/lib/ -llapacke -lopenblas
CBLAS_LINK += -llapacke
CBLAS_LINK += -lopenblas

TUNED_KAL_FILES  = $(KALMAN_PATH)basic-c/tuning_kalman/non_tuned.o 
TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/_predict.o 
TUNED_KAL_FILES += $(KALMAN_PATH)basic-c/tuning_kalman/_correct.o 

TUNED_LA_FILES  = ../kalman-filters/basic-c/tuning/non_tuned.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_add_mat.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_compute_LUP_inline.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_multiply_matrix.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_multiply_matrix_by_scalar.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_transpose_matrix.o
TUNED_LA_FILES += $(KALMAN_PATH)kalman_filter.o # 

all: counter counter_blas counter_LA
		./counter.out config_recommended.txt
		./counter_blas.out config_recommended.txt
		./counter_LA.out config_recommended.txt


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
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(KALMAN_FILES) -o counter.out $(ALL_LIBS)

counter_blas: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) $(CBLAS_LINK) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(BLAS_KAL_FILES) -o counter_blas.out $(ALL_LIBS)

counter_LA: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_LA_FILES) -o counter_LA.out $(ALL_LIBS)

counter_gdb:
	  $(CC) -g $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o -o counter.out $(ALL_LIBS)

get_background: get_background.cpp
		make useful_functions.o
		$(CC) $(OCV_PATH) get_background.cpp useful_functions.cpp -o get_background.out $(ALL_LIBS)


clean:
	rm *.o
	rm *.out
	rm *.h264

