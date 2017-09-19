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

KALMAN_PATH = ../kalman-filters/basic-c

KALMAN_FILES  = linear_algebra.o # 
KALMAN_FILES += kalman_filter.o #

BLAS_KAL_FILES  = ../kalman-filters/kalman-blas/kalman_filter.o #
CBLAS_LINK  = -L/usr/lib/ -llapacke -lopenblas
CBLAS_LINK += -llapacke
CBLAS_LINK += -lopenblas

TUNED_KAL_FILES  = ../kalman-filters/basic-c/tuning_kalman/non_tuned.o 
TUNED_KAL_FILES += ../kalman-filters/basic-c/tuning_kalman/_predict.o 
TUNED_KAL_FILES += ../kalman-filters/basic-c/tuning_kalman/_correct.o 

TUNED_LA_FILES  = ../kalman-filters/basic-c/tuning/non_tuned.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_add_mat.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_compute_LUP_inline.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_multiply_matrix.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_multiply_matrix_by_scalar.o
TUNED_LA_FILES += ../kalman-filters/basic-c/tuning/_transpose_matrix.o
TUNED_LA_FILES += kalman_filter.o # 

TUNED_LA_OMP_FILES  = ../kalman-filters/basic-c/tuning_omp/non_tuned.o
TUNED_LA_OMP_FILES += ../kalman-filters/basic-c/tuning_omp/_add_mat.o
TUNED_LA_OMP_FILES += ../kalman-filters/basic-c/tuning_omp/_compute_LUP_inline.o
TUNED_LA_OMP_FILES += ../kalman-filters/basic-c/tuning_omp/_multiply_matrix.o
TUNED_LA_OMP_FILES += ../kalman-filters/basic-c/tuning_omp/_multiply_matrix_by_scalar.o
TUNED_LA_OMP_FILES += ../kalman-filters/basic-c/tuning_omp/_transpose_matrix.o
TUNED_LA_OMP_FILES += kalman_filter.o # 


ATLAS_LINK = -L/usr/local/atlas/lib -lcblas -llapack -lptcblas -lpthread -latlas
ATLAS_INC  = -I/usr/local/atlas/include
ATLAS_SYS  = -DSYS=\"sys_atlas.h\"

CBLAS_LINK = -L/usr/lib/ -llapacke -lopenblas
CBLAS_SYS  = -DSYS=\"sys_cblas.h\"

all: counter counter_LA counter_LA_omp counter_KAL counter_ATLAS
	./counter_LA_omp.out dots.txt
	./counter.out dots.txt
	./counter_ATLAS.out dots.txt
	./counter_LA.out dots.txt
	./counter_KAL.out dots.txt

recommended: counter counter_LA counter_LA_omp counter_KAL counter_ATLAS
		./counter.out config_recommended.txt
		./counter_ATLAS.out config_recommended.txt
		./counter_LA.out config_recommended.txt
		./counter_LA_omp.out config_recommended.txt
		./counter_KAL.out config_recommended.txt

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
		gcc -c $(KALMAN_PATH)/kalman_filter.c $(KALMAN_PATH)/linear_algebra.c 
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(KALMAN_FILES) -o counter.out $(ALL_LIBS)

counter_blas: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		gcc ../kalman-filters/kalman-blas/kalman_filter.c -c $(CBLAS_SYS) $(CBLAS_LINK)
		$(CC) $(OCV_PATH) $(CBLAS_LINK) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o kalman_filter.o -o counter_blas.out $(ALL_LIBS)

counter_ATLAS: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		gcc ../kalman-filters/kalman-blas/kalman_filter.c -c $(ATLAS_SYS) $(ATLAS_LINK) $(ATLAS_INC)
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o kalman_filter.o -o counter_ATLAS.out $(ATLAS_LINK) $(ATLAS_INC) $(ALL_LIBS)

counter_LA: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		gcc -c $(KALMAN_PATH)/kalman_filter.c
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_LA_FILES) -o counter_LA.out $(ALL_LIBS)

counter_LA_omp: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		gcc -c $(KALMAN_PATH)/kalman_filter.c
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_LA_OMP_FILES) -o counter_LA_omp.out $(ALL_LIBS) -fopenmp

counter_KAL: counter.cpp useful_functions.o image_input.o image_output.o object.o trackers.o Target.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o $(TUNED_KAL_FILES) -o counter_KAL.out $(ALL_LIBS)

counter_gdb:
	  $(CC) -g $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o Target.o -o counter.out $(ALL_LIBS)

get_background: get_background.cpp
		make useful_functions.o
		$(CC) $(OCV_PATH) get_background.cpp useful_functions.cpp -o get_background.out $(ALL_LIBS)


clean:
	rm *.o
	rm *.out
	rm *.h264

