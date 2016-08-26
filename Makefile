CC = g++

LIBS = -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_core -lopencv_videoio 
OCV_PATH = -I/usr/include/
VID_PATH = /home/pi/test_videos

counter: counter.cpp 
		$(CC) $(OCV_PATH) counter.cpp -o counter.out $(LIBS)


