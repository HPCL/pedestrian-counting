CC = g++

#TODO figure out which I need
BASE_LIBS = -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_core -lopencv_videoio
ALL_LIBS  = -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_core -lopencv_videoio 
OCV_PATH  = -I/usr/include/
VID_PATH  = /home/pi/test_videos

object.o: object.cpp object.h
		$(CC) $(OCV_PATH) -c object.cpp $(BASE_LIBS)

counter: counter.cpp
	  make object.o
		$(CC) $(OCV_PATH) counter.cpp object.o -o counter.out $(ALL_LIBS)

counter_gdb:
	  $(CC) $(OCV_PATH) counter.cpp object.cpp -g -o counter.out $(ALL_LIBS) 


