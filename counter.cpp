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
#include "object.h"

using namespace std;
using namespace cv;

//our sensitivity value to be used in the threshold() function
static int SENSITIVITY_VALUE = 50; // original 20
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
static int BLUR_SIZE = 200; // original 10
static double MIN_OBJ_AREA = 1000;
static double MAX_DIST_SQD = 6000000; // maximum distance between to centers to consider it one object

void compare_frames(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage);
void search_for_movement(Mat thresholdImage, Mat &display, 
												bool loop_switch, double &next_id, int &count_LR, int &count_RL,
												vector<Object> &objects_0, vector<Object> &objects_1);
char is_center_crossed(Point2d &a, Point2d &b, double middle);
char is_center_crossed(Object &obj_a, Object &obj_b, double middle);
void interpret_input(char c, bool &debugMode, bool &trackingEnabled, bool &pause);
void draw_rectangles(vector<Rect2d> &obj_rects, Mat &display);
void draw_centers(vector<Object> &objects, Mat &display);

int char_to_int(char* c);
string int_to_str(int i);
void show_help();


int main(int argc, char** argv){

	bool debugMode = false;
	bool trackingEnabled = false;
	bool pause = false;
	bool success, loop_switch;
	double next_id = 0;
	int count_LR = 0, count_RL = 0;

	Mat frame1, frame2;
	Mat grayImage1, grayImage2;
	vector<Object> objects_0, objects_1;
	Mat thresholdImage;
	
	VideoCapture capture;

  if(argc < 2) 
	  show_help();

	string vid_name = argv[1];
	
	if(argc > 2) 
		MAX_DIST_SQD = char_to_int(argv[2]);

	if(argc > 3) 
		SENSITIVITY_VALUE = char_to_int(argv[3]);
	
	if(argc > 4) 
		BLUR_SIZE = char_to_int(argv[4]);

	if(argc > 5) 
		MIN_OBJ_AREA = char_to_int(argv[5]);

	namedWindow("Frame1", CV_WINDOW_NORMAL);

	while(1){

		cout << "reopening video" << endl;
		capture.open(vid_name);
		if(!capture.isOpened()){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}

		success = capture.read(frame1);
		if(!success){
			cout << endl << "ERROR: frame 1 failed to be read" << endl;
			exit(1);
		}
		cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
		success = capture.read(frame2);
		if(!success){
			cout << endl << "ERROR: frame 2 failed to be read" << endl;
			exit(1);
		}

		loop_switch = true;
		while( success ) {
			// cout << "new frame" << endl;

			cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
			compare_frames(grayImage1, grayImage2, debugMode, thresholdImage);

			if(trackingEnabled) {
				search_for_movement( thresholdImage, frame1, loop_switch, next_id, count_LR, count_RL, objects_0, objects_1); 
			}

			imshow("Frame1",frame1);
			resizeWindow("Frame1", 512, 384);

			interpret_input(waitKey(10), debugMode, trackingEnabled, pause);

			frame2.copyTo(frame1);
			cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
			success = capture.read(frame2);
			loop_switch = !loop_switch;
		} // inner while loop
		
		//release the capture before re-opening and looping again.
		capture.release();
		cout << "Next id for object (total id'd): " << next_id << endl;
		cout << "objects moving Left to Right:    " << count_LR << endl;
		cout << "objects moving Right to Left:    " << count_RL << endl;
	} // outer while loop (infinite)

	return 0;
}

void search_for_movement(Mat thresholdImage, Mat &display, 
												bool loop_switch, double &next_id, int &count_LR, int &count_RL,
												vector<Object> &objects_0, vector<Object> &objects_1){

	int obj_count = 0, i = 0;
	double mid_row = (double)(thresholdImage.cols >> 1); // half way across the screen
	double obj_area = 0, dist = -1, min_dist = -1;
	vector< vector<Point> > contours;
	Mat temp;
	Rect2d temp_rect;
	vector<Rect2d> obj_rects;
	vector<Vec4i> hierarchy;
	Object *prev_obj = NULL;

	thresholdImage.copyTo(temp);
	 
	//TODO clean this up so it makes sense. maybe make some functions
	if(loop_switch){
		findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if(contours.size() > 0){
			for(vector< vector<Point> >::iterator it_0 = contours.begin(); it_0 != contours.end(); it_0++) {
				temp_rect = boundingRect(*it_0);
				obj_area = temp_rect.area();

				if(obj_area >= MIN_OBJ_AREA){
					obj_count++;
					obj_rects.push_back(Rect2d(temp_rect));
					objects_0.push_back(Object(*it_0));
					prev_obj = NULL;
					if(objects_1.size() > 0) {
						min_dist = -1;
						for(vector<Object>::iterator it_1 = objects_1.begin(); it_1 != objects_1.end(); it_1++) {
							dist = it_1->find_distance_sqd(*objects_0.end());
							if(dist <= MAX_DIST_SQD) {
								if( (min_dist == -1) || (dist < min_dist) ) {
									min_dist = dist;
									prev_obj = &(*it_1);
								}
							}
						} //inner for
					}
					if(prev_obj == NULL) {
						objects_0.end()->set_id(next_id++);
						objects_0.end()->set_is_counted(false);
					} else {
						objects_0.end()->set_id(prev_obj->get_id());
						objects_0.end()->set_is_counted(prev_obj->get_is_counted());
						if( !(objects_0.end()->get_is_counted()) ) {
							switch(is_center_crossed(*prev_obj, *objects_0.end(), mid_row)) {
							case 'R':
								objects_0.end()->set_is_counted();
								count_LR++;
								// cout << "objects moving Left to Right: " << count_LR << endl;
								break;
							case 'L':
								objects_0.end()->set_is_counted();
								count_RL++;
								// cout << "objects moving Right to Left: " << count_RL << endl;
								break;	
							} //switch
						}
					}//else
				}//if obj_area >= MIN_OBJ_AREA
			draw_centers(objects_0, display);
			draw_centers(objects_1, display);
			objects_1.clear();
			}//outer for
		} //if contour_1 > 0
	} else {
		findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if(contours.size() > 0){
			for(vector< vector<Point> >::iterator it_0 = contours.begin(); it_0 != contours.end(); it_0++) {
				temp_rect = boundingRect(*it_0);
				obj_area = temp_rect.area();

				if(obj_area >= MIN_OBJ_AREA){
					obj_count++;
					obj_rects.push_back(Rect2d(temp_rect));
					objects_1.push_back(Object(*it_0));
					prev_obj = NULL;
					if(objects_0.size() > 0) {
						min_dist = -1;
						for(vector<Object>::iterator it_1 = objects_0.begin(); it_1 != objects_0.end(); it_1++) {
							dist = it_1->find_distance_sqd(*objects_1.end());
							if(dist <= MAX_DIST_SQD) {
								if( (min_dist == -1) || (dist < min_dist) ) {
									min_dist = dist;
									prev_obj = &(*it_1);
								}
							}
						}
					}
					if(prev_obj == NULL) {
						objects_1.end()->set_id(next_id++);
						objects_1.end()->set_is_counted(false);
					} else {
						objects_1.end()->set_id(prev_obj->get_id());
						objects_1.end()->set_is_counted(prev_obj->get_is_counted());
						if( !(objects_1.end()->get_is_counted()) ) {
							switch(is_center_crossed(*prev_obj, *objects_1.end(), mid_row)) {
							case 'R':
								objects_1.end()->set_is_counted();
								count_LR++;
								// cout << "objects moving Left to Right: " << count_LR << endl;
								break;
							case 'L':
								objects_1.end()->set_is_counted();
								count_RL++;
								// cout << "objects moving Right to Left: " << count_RL << endl;
								break;
							}
						}
					}

				}
			draw_centers(objects_0, display);
			draw_centers(objects_1, display);
			objects_0.clear();
			}
		}
	}

	draw_rectangles(obj_rects, display);
	line(display, Point(mid_row, 0), Point(mid_row, display.cols), Scalar( 0, 255, 0 ), 2, 1);

}

//@compares two grayscale images using simple background sutraction
//	also displays the stages if requested
void compare_frames(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage) {
	Mat differenceImage, blurImage;

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

	blur(thresholdImage, blurImage, Size(BLUR_SIZE, BLUR_SIZE));
	threshold(blurImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

	if(debugMode){
		namedWindow("Final Threshold Image", CV_WINDOW_NORMAL);
		imshow("Final Threshold Image", thresholdImage);
		resizeWindow("Final Threshold Image", 512, 384);
	}
	else {
		destroyWindow("Final Threshold Image");
	}
}

//@checks if the center is crossed
//@returns N- no, L- right to left, R- left to right
char is_center_crossed(Point2d &a, Point2d &b, double middle) {
	if( (a.x < middle) && (b.x >= middle) )
		return 'R';
	else if( (b.x < middle) && (a.x >= middle) )
		return 'L';
	else
		return 'N';
}

//@checks if the center is crossed
//@returns N- no, L- right to left, R- left to right
char is_center_crossed(Object &obj_a, Object &obj_b, double middle) {
	Point2d a, b;
	obj_a.get_center(a);
	obj_b.get_center(b);
	if( (a.x < middle) && (b.x >= middle) )
		return 'R';
	else if( (b.x < middle) && (a.x >= middle) )
		return 'L';
	else
		return 'N';
}

//@interpret keyboard input
void interpret_input(char c, bool &debugMode, bool &trackingEnabled, bool &pause) {
	switch(c){
	// case 1048603:
	case 27: //'esc' key has been pressed, exit program.
		cout << "Have a nice day! :)" << endl;
		exit(0);
	// case 1048692:
	case 116: //'t' has been pressed. this will toggle tracking
		trackingEnabled = !trackingEnabled;
		if(trackingEnabled == false) cout << "Tracking disabled." << endl;
		else cout << "Tracking enabled." << endl;
		break;
	// case 1048676:
	case 100: //'d' has been pressed. this will debug mode
		debugMode = !debugMode;
		if(debugMode == false) cout << "Debug mode disabled." << endl;
		else cout << "Debug mode enabled." << endl;
		break;
	// case 1048688:
	case 112: //'p' has been pressed. this will pause/resume the code.
		pause = !pause;
		if(pause == true){ 
			cout << "Code paused, press 'p' again to resume" << endl;
			while (pause == true){
				//stay in this loop until 
				switch (waitKey()){
					//a switch statement inside a switch statement? Mind blown.
				// case 1048688:
				case 112: 
					//change pause back to false
					pause = false;
					cout << "Code resumed." << endl;
					break;
				}
			}
		}
	} 
}

//@draws the rectagles
void draw_rectangles(vector<Rect2d> &obj_rects, Mat &display) {
	for(unsigned j = 0; j < obj_rects.size(); j++) {
	  rectangle( display, obj_rects[j], Scalar( 255, 0, 0 ), 2, 1 ); // draw rectangle around object
	  // int mid_x = obj_rects[j].x + (obj_rects[j].width / 2);  // was this important?
	  // int mid_y = obj_rects[j].y - (obj_rects[j].height / 2);
	}
}

//@draws the rectagles
void draw_centers(vector<Object> &objects, Mat &display) {
	Point2d temp_pt;
	for(unsigned j = 0; j < objects.size(); j++) {
		objects[j].get_center(temp_pt);
	  circle( display, temp_pt, 5, Scalar( 0, 0, 255 ), 2, 1 ); 
	  // circle( display, temp_pt, MAX_DIST_SQD, Scalar( 0, 0, 255 ), 2, 1 ); 
	}
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
  " Usage: ./counter.out <video_name> [MAX_DIST_SQD] [SENSITIVITY_VALUE] [BLUR_SIZE] [MIN_OBJ_AREA]\n"
  " examples:\n"
  " ./counter.out /home/pi/videos/my_vid.h264\n"
  " ./counter.out /home/pi/videos/my_vid.h264 50 20 10 10\n"
  << endl << endl;
  exit(1); 
}
