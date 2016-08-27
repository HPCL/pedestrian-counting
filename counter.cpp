/* count_things.cpp
 * a program to count pedestrians and bikers moving through a video
 * hopefully a live stream too
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * Much of this code is based on helpful tutorials available here:
 * http://docs.opencv.org/3.1.0/d5/d07/tutorial_multitracker.html#gsc.tab=0
 * https://www.youtube.com/user/khounslow/featured

 * Apparently this is something I'm supposed to do:
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

 */

#include <opencv/cv.hpp>
//#include <opencv/highgui.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cv;

//our sensitivity value to be used in the threshold() function
static int SENSITIVITY_VALUE = 50; // original 20
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
static int BLUR_SIZE = 200; // original 10
static double MIN_OBJ_AREA = 1000;
static double MAX_DIST_SQD = 1; // maximum distance between to centers to consider it one object
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = {0,0};
//Rect objectBoundingRectangle = Rect(0,0,0,0);

void compare_frames(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage);
void searchForMovement(Mat thresholdImage, Mat &cameraFeed);
int char_to_int(char* c);
string int_to_str(int i);
void show_help();


int main(int argc, char** argv){

	bool debugMode = false;
	bool trackingEnabled = false;
	bool pause = false;
	bool success;

	Mat frame1, frame2;
	Mat grayImage1, grayImage2;
	Mat thresholdImage;
	
	VideoCapture capture;

  if(argc < 2) 
	  show_help();

	string vid_name = argv[1];
	
	if(argc > 2) 
		SENSITIVITY_VALUE = char_to_int(argv[2]);
	
	if(argc > 3) 
		BLUR_SIZE = char_to_int(argv[3]);

	if(argc > 4) 
		MIN_OBJ_AREA = char_to_int(argv[4]);

	namedWindow("Frame1", CV_WINDOW_NORMAL);

	while(1){

		//we can loop the video by re-opening the capture every time the video reaches its last frame
		capture.open(vid_name);
		if(!capture.isOpened()){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}

		//read first frame
		success = capture.read(frame1);
		if(!success){
			cout << endl << "ERROR: frame 1 failed to be read" << endl;
			exit(1);
		}
		//convert frame1 to gray scale for frame differencing
		cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
		//copy second frame
		success = capture.read(frame2);
		if(!success){
			cout << endl << "ERROR: frame 2 failed to be read" << endl;
			exit(1);
		}

		//while( !(frame2.rows == 0 || frame2.cols ==0) ) {
		while( success ) {

			//convert frame2 to gray scale for frame differencing
			cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
			compare_frames(grayImage1, grayImage2, debugMode, thresholdImage)

			if(trackingEnabled) {
				searchForMovement(thresholdImage, frame1);
			}

			//show our captured frame
			imshow("Frame1",frame1);
			resizeWindow("Frame1", 512, 384);

			switch(waitKey(10)){
			case 1048603:
			// case 27: //'esc' key has been pressed, exit program.
				return 0;
			case 1048692:
			// case 116: //'t' has been pressed. this will toggle tracking
				trackingEnabled = !trackingEnabled;
				if(trackingEnabled == false) cout << "Tracking disabled." << endl;
				else cout << "Tracking enabled." << endl;
				break;
			case 1048676:
			// case 100: //'d' has been pressed. this will debug mode
				debugMode = !debugMode;
				if(debugMode == false) cout << "Debug mode disabled." << endl;
				else cout << "Debug mode enabled." << endl;
				break;
			case 1048688:
			// case 112: //'p' has been pressed. this will pause/resume the code.
				pause = !pause;
				if(pause == true){ 
					cout << "Code paused, press 'p' again to resume" << endl;
					while (pause == true){
						//stay in this loop until 
						switch (waitKey()){
							//a switch statement inside a switch statement? Mind blown.
						case 1048688:
						// case 112: 
							//change pause back to false
							pause = false;
							cout << "Code resumed." << endl;
							break;
						}
					}
				}
			} //big switch statement

			frame2.copyTo(frame1);
			cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
			success = capture.read(frame2);
		} // inner while loop
		
		//release the capture before re-opening and looping again.
		capture.release();
	} // outer while loop (infinite)

	return 0;
}

void searchForMovement(Mat thresholdImage, Mat &display){

	int obj_count = 0, i = 0;
	int mid_row = thresholdImage.rows >> 1; // half way across the screen
	double obj_area = 0, distance;
	Mat temp;
	Rect2d temp_rect;
	vector<Rect2d> obj_rects;
	thresholdImage.copyTo(temp);
	
	vector< vector<Point> > contours_0;
	vector< vector<Point> > contours_1;
	vector<Vec4i> hierarchy;

	if(trackingEnabled) {
		searchForMovement(thresholdImage, frame1);
	}

	if(/* somthing is even? */){
		findContours(temp, contours_1, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if(contours_1.size() > 0){
			i = contours_1.size()-1;
			for(i; i >= 0; i--) { //TODO make this iterators
				temp_rect = boundingRect(contours_1.at(i));
				obj_area = temp_rect.area();

				if(obj_area >= MIN_OBJ_AREA){
					obj_count++;
					obj_rects.push_back(Rect2d(temp_rect));
					// TODO center get
					for(int j = 0; j < contours_2.size(); j++) { //TODO iterators
						//TODO find closest with some minimum
					}
					//TODO give ID (new or old)
					//TODO check if center crossed and count
				} else {
					//TODO remove small contours
				}
			}
		}

	} else {
		findContours(temp, contours_2, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if(contours_2.size() > 0){
			i = contours_2.size()-1;
			for(i; i >= 0; i--) {
				temp_rect = boundingRect(contours_2.at(i));
				obj_area = temp_rect.area();

				if(obj_area >= MIN_OBJ_AREA){
					obj_count++;
					obj_rects.push_back(Rect2d(temp_rect));
				}
			} 
		}
	}
	
	for(unsigned j = 0; j < obj_rects.size(); j++) {
	  rectangle( display, obj_rects[j], Scalar( 255, 0, 0 ), 2, 1 ); // draw rectangle around object
	  int mid_x = obj_rects[j].x + (obj_rects[j].width / 2);
	  int mid_y = obj_rects[j].y - (obj_rects[j].height / 2);
	}
}

//@compares two grayscale images using simple background sutraction
//	also displays the stages if requested
void compare_frames(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage) {
	Mat differenceImage;

	absdiff(grayImage1, grayImage2, differenceImage);
	threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

	if(debugMode) {
		namedWindow("Difference Image", CV_WINDOW_NORMAL);
		imshow("Difference Image", differenceImage);
		resizeWindow("Difference Image", 512, 384);
		namedWindow("Threshold Image", CV_WINDOW_NORMAL);
		imshow("Threshold Image", thresholdImage);
		resizeWindow("Threshold Image", 512, 384);
	} else {
		destroyWindow("Difference Image");
		destroyWindow("Threshold Image");
	}

	blur(thresholdImage, thresholdImage, Size(BLUR_SIZE, BLUR_SIZE));
	threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

	if(debugMode){
		namedWindow("Final Threshold Image", CV_WINDOW_NORMAL);
		imshow("Final Threshold Image", thresholdImage);
		resizeWindow("Final Threshold Image", 512, 384);
	}
	else {
		destroyWindow("Final Threshold Image");
	}
}

void draw_rectangles() {

}

/******************************* utilities ********************************/

int char_to_int(char* c) {
	int i = 0; 
	int ret = 0;
	int mult = 1;

	while(c[i] !='\0')
		i++;
	i--;

	for (i; i >= 0; i--) {
		ret += mult * ((int)c[i] - (int)48);
		mult *= 10;
	}
	return ret;
}

string int_to_str(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}

void show_help() {
  cout << endl << 
  " Usage: ./motionTracking_modified.out <video_name> [SENSITIVITY_VALUE] [BLUR_SIZE] [MIN_OBJ_AREA]\n"
  " examples:\n"
  " ./motionTracking_modified.out /home/pi/videos/my_vid.h264\n"
  " ./motionTracking_modified.out /home/pi/videos/my_vid.h264 20 10 10\n"
  << endl << endl;
  exit(1); 
}
