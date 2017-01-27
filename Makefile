CC = g++

#TODO figure out which I actually need
# note: order may or may not matter
ALL_LIBS  = -lopencv_imgproc # 
ALL_LIBS += -lopencv_highgui # I think this one lets us get keyboard input
ALL_LIBS += -lopencv_objdetect #
ALL_LIBS += -lopencv_core -lopencv_videoio # basic opencv
ALL_LIBS += -lopencv_video # background subtractor MOG2
ALL_LIBS += -lopencv_imgcodecs #
ALL_LIBS += -lraspicam -lraspicam_cv # to live stream with raspicam
OCV_PATH  = -I/usr/include/
VID_PATH  = /home/pi/test_videos

object.o: object.cpp object.h
		$(CC) $(OCV_PATH) -c object.cpp $(ALL_LIBS)

useful_functions.o: useful_functions.cpp useful_functions.h
		$(CC) $(OCV_PATH) -c useful_functions.cpp $(ALL_LIBS)

useful_tests: useful_functions.cpp useful_tests.cpp useful_functions.h
		make useful_functions.o
		$(CC) $(OCV_PATH) useful_tests.cpp useful_functions.o -o useful_tests.out $(ALL_LIBS)

image_input.o: image_input.cpp image_input.h
		$(CC) $(OCV_PATH) -c image_input.cpp $(ALL_LIBS)

image_output.o: image_output.cpp image_output.h
		$(CC) $(OCV_PATH) -c image_output.cpp useful_functions.o $(ALL_LIBS)

counter: counter.cpp
		make useful_functions.o
		make image_input.o
		make image_output.o
	  make object.o
		$(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o -o counter.out $(ALL_LIBS)

counter_gdb:
	  $(CC) $(OCV_PATH) counter.cpp useful_functions.cpp object.o image_input.o image_output.o -g -o counter.out $(ALL_LIBS) 

get_background: get_background.cpp
		make useful_functions.o
		$(CC) $(OCV_PATH) get_background.cpp useful_functions.cpp -o get_background.out $(ALL_LIBS)

