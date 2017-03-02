CC = g++

#TODO figure out which I actually need
# note: order may or may not matter
ALL_LIBS  = -lopencv_imgproc # 
ALL_LIBS += -lopencv_highgui # I think this one lets us get keyboard input
ALL_LIBS += -lopencv_objdetect #
ALL_LIBS += -lopencv_core -lopencv_videoio # basic opencv
ALL_LIBS += -lopencv_video # background subtractor MOG2
ALL_LIBS += -lopencv_imgcodecs #
#ALL_LIBS += -lraspicam -lraspicam_cv # to live stream with raspicam not on arya
#OCV_PATH  = -I/usr/include/ # on pi
OCV_PATH  = -L/usr/local/packages/opencv/lib # on arya 
OCV_PATH += -I/usr/local/packages/opencv/include # on arya

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

target.o: target.cpp target.h
		$(CC) $(OCV_PATH) -c target.cpp

counter: counter.cpp
		make useful_functions.o
		make image_input.o
		make image_output.o
	  make object.o
	  make trackers.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o trackers.o -o counter.out $(ALL_LIBS)

counter_gdb:
	  $(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.cpp image_input.cpp image_output.cpp -g -o counter.out $(ALL_LIBS) 

get_background: get_background.cpp
		make useful_functions.o
		$(CC) $(OCV_PATH) get_background.cpp useful_functions.cpp -o get_background.out $(ALL_LIBS)


clean:
	rm *.o
	rm *.out
	rm *.h264

