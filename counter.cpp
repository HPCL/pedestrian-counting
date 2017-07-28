/* count_things.cpp
 * a program to count pedestrians and bikers moving through a video
 * hopefully a live stream too

 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * Dani White
 * white.dani.m@gmail.com

 * Some of this code is based on helpful tutorials available here:
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

 * If by some miracle you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

 */

#include <opencv/cv.hpp>
#include "opencv2/core.hpp"
// #include <raspicam/raspicam_cv.h>
// #include "opencv2/background_segm.hpp"
// #include <opencv/highgui.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include "object.h"
#include "useful_functions.h"
#include "image_input.h"
#include "image_output.h"
#include "trackers.h"
#include "Target.hpp"

#define REMOTE 1 == 1

#define EMPTY_LIMIT 4
#define MIN_ALIVE   5

using namespace std;
using namespace cv;

//our sensitivity value to be used in the threshold() function
static double MAX_DIST_SQD = 6000000; // maximum distance between to centers to consider it one object
static int SENSITIVITY_VALUE_1 = 200; // values for cleaning noise out of difference images
static int SENSITIVITY_VALUE_2 = 50; 
//size of blur used to smooth the image to remove possible noise and
//increase the size of the object we are trying to track. (Much like dilate and erode)
static int BLUR_SIZE_1 = 200;
static int BLUR_SIZE_2 = 200;
static double MIN_OBJ_AREA = 1000;

// TODO all of this is wrong and will need to change
static KALMAN_TYPE dt = 0.25;
static KALMAN_TYPE A_init[] = {1, dt, 0, 0, 0, 0, 0, 1, dt, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, dt, 0, 0, 0, 0, 0, 1, dt, 0, 0, 0, 0, 0, 1};
static KALMAN_TYPE C_init[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
static KALMAN_TYPE Q_init[] = {1e-2, 0, 0, 0, 0, 0, 0, 5.0, 0, 0, 0, 0, 0, 0, 1e-2, 0, 0, 0, 0, 0, 0, 1e-2, 0, 0, 0, 0, 0, 0, 5.0, 0, 0, 0, 0, 0, 0, 1e-2};
static KALMAN_TYPE R_init[] = {5.0, 0, 0, 5.0};
static KALMAN_TYPE P_init[] = {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
static KALMAN_TYPE x_hat_init[] = {0, 0, 0, 0, 0, 0};
static int n = 6;
static int m = 2;

//TODO don't do this
ImageOutput* video_out; 

void set_background(string back_name, bool background_is_video, Mat& grayBackground, bool& use_static_back);
void track_with_non_adaptive_BS(ImageInput* capture, Mat& grayBackground, bool use_static_back,
																double& total_targets, int& count_LR, int& count_RL);
void do_non_adaptive_BS(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage);

void track_with_adaptive_BS(ImageInput* capture, Mat& grayBackground, bool use_static_back,
														double& total_targets, int& count_LR, int& count_RL);
void do_adaptive_BS(Ptr<BackgroundSubtractorMOG2> subtractor, Mat &image, bool debugMode, Mat &thresholdImage);
void search_for_movement(Mat &thresholdImage, Mat &display, 
												bool loop_switch, double &total_targets, int &count_LR, int &count_RL,
												vector<Target*> &targets);
void dynamic_threshold(Mat& input_image, Mat& threshold_image, float percent_peak, bool debugMode);

char is_center_crossed(const Point2d &a, const Point2d &b, double middle);
char is_center_crossed(const Object &obj_a, const Object &obj_b, double middle);

void get_settings_inline(int argc, char** argv, string& vid_name, string& back_name);
void get_settings_file(int argc, char** argv, string& vid_name, string& back_name, char& bs_type);
void interpret_input(char c, bool &debugMode, bool &trackingEnabled, bool &pause);
void draw_rectangles(vector<Rect2d> &obj_rects, Mat &display);
void draw_centers(vector<Target*> &targets, Mat &display);

void show_help();


int main(int argc, char** argv){

	//TODO combine static back and bs type?
	//TODO make this thing a freaking class
	bool use_static_back = false;		  // use a static image for background
	bool background_is_video = true; 	// obtain static back from video
	bool success = false;					    // boolean set when image capture works
	char bs_type = 'N';						    // back subtraction algo 'M' for MOG2, non-adaptive is default
	double total_targets = 0;					    	// the next id to use
	int count_LR = 0, count_RL = 0;		// counts of objects
	string vid_name;									// name of video file to use
	string back_name;									// optional name for background file

	Mat grayBackground;
	ImageInput* capture;

	int num_videos = 4;
	char** name_list   = new char*[4]; 
  name_list[0] = (char*)"tracking_video";
  name_list[1] = (char*)"difference_image";
  name_list[2] = (char*)"threshold_image";
  name_list[3] = (char*)"final_threshold";

	if(argc == 2) {
		get_settings_file(argc, argv, vid_name, back_name, bs_type);
	} else if( (argc >= 3) && (argc < 8) ) {
		get_settings_inline(argc, argv, vid_name, back_name);
	} else {
		show_help();
	}

	//TODO move to get settings?
	Trackers::set_max_dist_sqd(MAX_DIST_SQD);
	Target::max_dist_sqd = MAX_DIST_SQD;

	if(vid_name == "RASPICAM") {
		capture = new ImageInput();
		cout << "using live stream" << endl;
	} else {
		capture = new ImageInput(vid_name);
		cout << "using video: " << vid_name << endl;
	}
	
	if(use_static_back)
		set_background(back_name, background_is_video, grayBackground, use_static_back);

	success = capture->open();
	if(!success){
			cout << "ERROR ACQUIRING VIDEO FEED named \"" << vid_name << "\"\n";
			getchar();
			exit(1);
	}

	Size S =  Size((int) capture->get(CV_CAP_PROP_FRAME_WIDTH), (int) capture->get(CV_CAP_PROP_FRAME_HEIGHT));
  video_out = new ImageOutput();
  if(!video_out->setup(REMOTE, name_list, S, num_videos))
  	exit(1);

	//TODO we won't need this loop for live streaming
	while(1){

		if(!success){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			exit(1);
		} 

		if (bs_type == 'M') {
			cout << endl <<  "Using adaptive (MOG2) Background subtraction" << endl;
			track_with_adaptive_BS(capture, grayBackground, use_static_back, total_targets, count_LR, count_RL);
		} else {
			cout << endl <<  "Using non adaptive (Naive) Background subtraction" << endl;
			track_with_non_adaptive_BS(capture, grayBackground, use_static_back, total_targets, count_LR, count_RL);
		}

		//release the capture before re-opening and looping again.
		capture->release();
		cout << "total targets created:        " << Target::next_id << endl;
		cout << "total targets detected:       " << total_targets << endl;
		cout << "objects moving Left to Right: " << count_LR << endl;
		cout << "objects moving Right to Left: " << count_RL << endl;

		if(REMOTE)
			break;
	
		success = capture->open();

	} // outer while loop (infinite)

	delete[] name_list;
	return 0;
} //main

//@sets background image for static background subtraction based on input file
void set_background(string back_name, bool background_is_video, Mat& grayBackground, bool& use_static_back) {
	if (back_name.compare("NONE") == 0)
		use_static_back = false;
	else if (background_is_video)
		get_background(back_name, grayBackground);
	else
		grayBackground = imread(back_name);

	//cvtColor(grayBackground, grayBackground, COLOR_BGR2GRAY);
	if(use_static_back && grayBackground.empty()) {
		cout << "ERROR: Could not read background image" << endl;
		getchar();
		exit(1);
	}
}

void track_with_non_adaptive_BS(ImageInput* capture, Mat& grayBackground, bool use_static_back,
																double& total_targets, int& count_LR, int& count_RL) {
	bool debugMode       = false;
	bool trackingEnabled = false;
	bool pause           = false;
	bool success         = false;
	bool loop_switch     = true;

	if (REMOTE) {
		trackingEnabled =  true;
		debugMode = true;
	}

	cout << endl;
	cout << "Non adaptive BS" << endl;
	cout << "Tracking: " << ((trackingEnabled) ? "Enabled" : "Disabled") << endl;
	cout << "Debug:    " << ((debugMode) ? "Enabled" : "Disabled") << endl;
	cout << endl;

	Mat frame1, frame2;
	Mat grayImage1, grayImage2;
	vector<Target*> targets;
	Mat thresholdImage;

	success = capture->read(frame1);
	if(!success){
		cout << endl << "ERROR: frame 1 failed to be read" << endl;
		getchar();
		exit(1);
	}
	cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
	success = capture->read(frame2);
	if(!success){
		cout << endl << "ERROR: frame 2 failed to be read" << endl;
		getchar();
		exit(1);
	}

	while( success ) {
		// cout << "new frame" << endl;

		cvtColor(frame2, grayImage2, COLOR_BGR2GRAY);
		if(use_static_back)
			do_non_adaptive_BS(grayBackground, grayImage2, debugMode, thresholdImage);
		else
			do_non_adaptive_BS(grayImage1, grayImage2, debugMode, thresholdImage);

		if(trackingEnabled) {
			search_for_movement(thresholdImage, frame2, loop_switch, total_targets, count_LR, count_RL, targets); 
		}

		char c = video_out->output_track_frame(frame2);
		interpret_input(c, debugMode, trackingEnabled, pause);


		if(!use_static_back) {
			frame2.copyTo(frame1);
			cvtColor(frame1, grayImage1, COLOR_BGR2GRAY);
		}
		success = capture->read(frame2);
		loop_switch = !loop_switch;
	} // while

	for (vector<Target*>::iterator it = targets.begin(); it != targets.end(); it++) {
    (*it)->~Target();
  }
} // track with non-adaptive BS

//@compares two grayscale images using simple background sutraction
//	also displays the stages if requested
void do_non_adaptive_BS(Mat &grayImage1, Mat &grayImage2, bool debugMode, Mat &thresholdImage) {
	Mat mat_list[3];
	Mat differenceImage, blurImage, firstThreshold;
	int diff = 0, thresh = 1, final = 2; // TODO maybe define these more universally

	absdiff(grayImage1, grayImage2, differenceImage);
	threshold(differenceImage, firstThreshold, SENSITIVITY_VALUE_1, 255, THRESH_BINARY);

	blur(firstThreshold, blurImage, Size(BLUR_SIZE_1, BLUR_SIZE_1));
	threshold(blurImage, thresholdImage, SENSITIVITY_VALUE_2, 255, THRESH_BINARY);

	if(debugMode){
		mat_list[diff]   = differenceImage;
		mat_list[thresh] = firstThreshold;
		mat_list[final]  = thresholdImage;
		video_out->output_debug_frames(mat_list);
	}
	else {
		video_out->close_debug_frames();
	}

}

//track objects through video using GMM background subtraction
//TODO do we actually want gray images for this version?
void track_with_adaptive_BS(ImageInput* capture, Mat& grayBackground, bool use_static_back,
														double& total_targets, int& count_LR, int& count_RL) {
	bool debugMode = false;
	bool trackingEnabled = false;
	bool pause = false;
	bool success = false;
	bool loop_switch = true;

	int frames = 1;
	double tot_time = 0;
	clock_t start_t;

	if (REMOTE) {
		trackingEnabled =  true;
		debugMode = true;
	}

	cout << endl;
	cout << "Adaptive BS" << endl;
	cout << "Tracking: " << ((trackingEnabled) ? "Enabled" : "Disabled") << endl;
	cout << "Debug:    " << ((debugMode) ? "Enabled" : "Disabled") << endl;
	cout << endl;

	Ptr<BackgroundSubtractorMOG2> subtractor = createBackgroundSubtractorMOG2();
	Mat frame, image;
	Mat thresholdImage;
	vector<Target*> targets;

	success = capture->read(frame);
	if(!success){
		cout << endl << "ERROR: frame failed to be read" << endl;
		getchar();
		exit(1);
	}

	while( success ) {
		start_t = clock();
		image = frame.clone();
		do_adaptive_BS(subtractor, image, debugMode, thresholdImage);

		if(trackingEnabled) {
			search_for_movement(thresholdImage, frame, loop_switch, total_targets, count_LR, count_RL, targets); 
		}

		char c = video_out->output_track_frame(frame);
		interpret_input(c, debugMode, trackingEnabled, pause);

		success = capture->read(frame);
		loop_switch = !loop_switch;
		tot_time += double(clock() - start_t ) /  CLOCKS_PER_SEC;
		frames++;
	} 

	cout << "Time    = " << tot_time << endl;
	cout << "Frames  = " << frames << endl;
	cout << "t per f = " << tot_time / (double)frames << endl;
	for (vector<Target*>::iterator it = targets.begin(); it != targets.end(); it++) {
    (*it)->~Target();
  }
}

//@finds movement blobs based on GMM background subtraction
//	also displays the stages if requested
void do_adaptive_BS(Ptr<BackgroundSubtractorMOG2> subtractor, Mat &image, bool debugMode, Mat &thresholdImage) {
	Mat mat_list[3];
	Mat differenceImage, blurImage, firstThreshold;
	int diff = 0, thresh = 1, final = 2; // TODO maybe define these more universally

	subtractor->apply(image, differenceImage);
	//blur(differenceImage, blurImage, Size(BLUR_SIZE_1, BLUR_SIZE_1));
	threshold(differenceImage, firstThreshold, SENSITIVITY_VALUE_1, 255, THRESH_BINARY);

	// TODO determine if this is useful
	blur(firstThreshold, blurImage, Size(BLUR_SIZE_2, BLUR_SIZE_2));
	//TODO paramertize %
	//TODO figure out if this is actually better
	//dynamic_threshold(blurImage, thresholdImage, 0.5, debugMode);
	threshold(blurImage, thresholdImage, SENSITIVITY_VALUE_2, 255, THRESH_BINARY);

	//TODO maybe add blur image
	if(debugMode){
		mat_list[diff]   = differenceImage;
		mat_list[thresh] = firstThreshold;
		mat_list[final]  = thresholdImage;
		video_out->output_debug_frames(mat_list);
		// video_out->output_one_frame_to_file(differenceImage, diff+1);
		// video_out->output_one_frame_to_file(firstThreshold, thresh+1);
		// video_out->output_one_frame_to_file(thresholdImage, final+1);
	}
	else {
		video_out->close_debug_frames();
	}
}

//@identifies objects based on threshold image and previous objects
//@
void search_for_movement(Mat &thresholdImage, Mat &display, 
												bool loop_switch, double &total_targets, int &count_LR, int &count_RL,
												vector<Target*> &targets){

	int obj_count = 0, i = 0;
	double mid_row = (double)(thresholdImage.cols >> 1); // half way across the screen
	double obj_area = 0;
	vector< vector<Point> > contours;
	Mat temp;
	Rect2d temp_rect;
	vector<Rect2d> obj_rects;
	vector<Vec4i> hierarchy;
	vector<Object> objects;
	Object *prev_obj = NULL;	
  Target* temp_target;
  Point2d temp_point;

  bool cont_delete = true;

	thresholdImage.copyTo(temp);

	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
  for(vector< vector<Point> >::iterator it_0 = contours.begin(); it_0 != contours.end(); it_0++) {
		temp_rect = boundingRect(*it_0);
		obj_area = temp_rect.area();

		if(obj_area >= MIN_OBJ_AREA){
			obj_count++;
			obj_rects.push_back(Rect2d(temp_rect));
			objects.push_back(Object(*it_0));
		}
	}

  for(vector<Target*>::iterator it = targets.begin(); it != targets.end(); it++) {
  	(*it)->update(objects, dt);

  	if( ((*it)->get_num_steps() > MIN_ALIVE) && !((*it)->get_is_counted()) ) {
      (*it)->set_is_counted();
      total_targets++;
      // cout << "Target " << (*it)->get_id_num() << " recorded." << endl;
    }
  }

  while (cont_delete) {
	  cont_delete = false;
	  for(vector<Target*>::iterator it = targets.begin(); it != targets.end(); it++) {
	  	if((*it)->get_num_empty_steps() > EMPTY_LIMIT) {
	      (*it)->~Target();
	      // cout << "Target " << (*it)->get_id_num() << " removed." << endl;
	      targets.erase(it);
	      cont_delete = true;
	      break;
	    } 
	  }
  }
  
  for(vector<Object>::iterator it_obj = objects.begin(); it_obj != objects.end(); it_obj++) {
	  if( !(it_obj->get_is_found()) ) {
	  	it_obj->get_center(temp_point);
	  	x_hat_init[0] = temp_point.x;
	  	x_hat_init[3] = temp_point.y;
	    temp_target = new Target(n, m, A_init, C_init, Q_init, R_init, P_init, x_hat_init);
	    targets.push_back(temp_target);
  	}
  }
	draw_centers(targets, display);
	draw_rectangles(obj_rects, display);
	line(display, Point(mid_row, 0), Point(mid_row, display.cols), Scalar( 0, 255, 0 ), 2, 1);

} //search for movement


//@perform thresholding in a dynamic manner
//@params input_image     - the difference image you want to threshold
//				threshold_image - the output threshold image
//        percent_peak    - percent of histogram peak value at which to cut off
//@pre input_image is a grayscale image of type CV_8U
//TODO probably not actually useful
void dynamic_threshold(Mat& input_image, Mat& threshold_image, float percent_peak, bool debugMode) {
	int hist_size = 256;
	float range[] = {0, hist_size};
	const float* hist_range = {range};
	bool uniform = true;
	bool accumulate = false;
	Mat hist;

	int hist_peak = 0;
	int hist_peak_val = 0;
	int percent_of_peak = 0;
	int k = 0;
	int thresh = 30;

	calcHist(&input_image, 1, 0, Mat(), hist, 1, &hist_size, &hist_range, uniform, accumulate );

	for( int i = 1; i < hist_size; i++ ) {
		if (cvRound(hist.at<float>(i)) > hist_peak_val) {
			hist_peak_val = cvRound(hist.at<float>(i));
			hist_peak = i;
		}
	}

	percent_of_peak = (int)((float)hist_peak_val * percent_peak / 100.0);
	k = hist_peak;
	while(cvRound(hist.at<float>(k)) > percent_of_peak)
		k++;
	thresh = k;
	threshold(input_image, threshold_image, thresh, 255, THRESH_BINARY);

  if(debugMode && !(REMOTE)){ //OUTPUT
  	//TODO we probably don't need this anymore
  	//credit: http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
  	int hist_w = 512; int hist_h = 400;
	  int bin_w = cvRound( (double) hist_w/hist_size );
	  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	  normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	  for( int i = 1; i < hist_size; i++ ) {
	      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
	                       Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
	                       Scalar( 255, 0, 0), 2, 8, 0  );
	  }

		namedWindow("Histogram", CV_WINDOW_NORMAL);
  	imshow("Histogram", histImage );
		resizeWindow("Histogram", 512, 384);
	}
	else {
		destroyWindow("Histogram");
	}

} //dynamic_threshold


//@checks if the center is crossed
//@returns N- no, L- right to left, R- left to right
char is_center_crossed(const Point2d &a, const Point2d &b, double middle) {
	if( (a.x < middle) && (b.x >= middle) )
		return 'R';
	else if( (b.x < middle) && (a.x >= middle) )
		return 'L';
	else
		return 'N';
}

//@checks if the center is crossed
//@returns N- no, L- right to left, R- left to right
char is_center_crossed(const Object &obj_a, const Object &obj_b, double middle) {
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



/*****************************************************************************\

																IO FUNCTIONS


\*****************************************************************************/


//@parse command line parameters to use as settings for the program
//TODO update with new options (already implemented in the file version)
void get_settings_inline(int argc, char** argv, string& vid_name, string& back_name) {
	vid_name  = argv[1];
	back_name = argv[2];

	if(argc > 3)
		MAX_DIST_SQD = char_to_int(argv[3]);

	if(argc > 4){
		SENSITIVITY_VALUE_1 = char_to_int(argv[4]);
		SENSITIVITY_VALUE_2 = SENSITIVITY_VALUE_1;
	}

	if(argc > 5)
		BLUR_SIZE_1 = char_to_int(argv[5]);
		BLUR_SIZE_2 = char_to_int(argv[5]);

	if(argc > 6)
		MIN_OBJ_AREA = char_to_int(argv[6]);
}

//@read file to get  proper settings and file names
//TODO dynamic threshold setting (maybe not)
//TODO tracking algo
void get_settings_file(int argc, char** argv, string& vid_name, string& back_name, char& bs_type) {
	string next_line;
	int input_cnt = 0;
	bool done = false;
	ifstream file;
	file.open(argv[1]);

	if (file.is_open()) {
		while ( getline(file, next_line) && !done ) {
			if(next_line[0] != '#') {
				switch (input_cnt) {
					case 0:
						//TODO handle live stream
						vid_name = next_line.c_str();
						break;
					case 1:
						back_name = next_line.c_str();
						break;
					case 2:
						MAX_DIST_SQD = str_to_int(next_line);
						break;
					case 3:
						SENSITIVITY_VALUE_1 = str_to_int(next_line);
						break;
					case 4:
						SENSITIVITY_VALUE_2 = str_to_int(next_line);
						break;
					case 5:
						BLUR_SIZE_1 = str_to_int(next_line);
						break;
					case 6:
						BLUR_SIZE_2 = str_to_int(next_line);
						break;
					case 7:
						MIN_OBJ_AREA = str_to_int(next_line);
						break;
					case 8:
						bs_type = next_line[0];
						break;
					case 9:
						Trackers::set_algo(next_line[0]);
						break;
				} //switch
				input_cnt++;
			} // if not comment
		} //while
		file.close();
	} else {
		cout << "ERROR: Could nt open configuration file." << endl;
		getchar();
		exit(1);
	}
}

//@interpret keyboard input for runtime options
void interpret_input(char c, bool &debugMode, bool &trackingEnabled, bool &pause) {
	char c2 = 'x';
	bool wait = pause;
	//TODO set defines or somthing for these numbers
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
		wait = pause;
		cout << "Code paused, press 'p' again to resume, 's' to step" << endl;
	}

	if(pause == true){
		while (wait){
			//stay in this loop until
			c2 = waitKey(10);
			switch (c2){
			case 112: // p is for unpause
				pause = false;
				wait  = false;
				cout << "Code resumed." << endl;
				break;
			case 115: // s is for step
				pause = true;
				wait  = false;
				break;
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
//TODO make this work with targets
void draw_centers(vector<Target*> &targets, Mat &display) {
	Point2d temp_pt;
	for(vector<Target*>::iterator it = targets.begin(); it != targets.end(); it++) {
		(*it)->prev_obj.get_center(temp_pt);
	  circle( display, temp_pt, 5, Scalar( 0, 0, 255 ), 2, 1 );
	  // circle( display, temp_pt, MAX_DIST_SQD, Scalar( 0, 255, 255 ), 2, 1 );
	  putText(display,"Object: " + int_to_str((*it)->get_id_num()), temp_pt, 1, 1, Scalar(255,0,0), 2);
	}
}

//@print instructions to standard output and crash program
void show_help() {
  cout << endl <<
  " Usage: ./counter.out <video_name> <gray background image> [MAX_DIST_SQD] [SENSITIVITY_VALUE] [BLUR_SIZE] [MIN_OBJ_AREA]\n"
  " examples:\n"
  " ./counter.out /home/pi/test_videos/my_vid.h264 NONE\n"
  " ./counter.out /home/pi/test_videos/my_vid.h264 /home/pi/test_videos/my_background.jpg \n"
  " ./counter.out /home/pi/test_videos/my_vid.h264 NONE 50 20 10 10\n"
  "\n"
  "OR \n"
  "\n"
  " Usage: ./counter.out <configuration file>\n"
  " example:\n"
  " ./counter.out config_example.txt\n"
  << endl << endl;
  exit(1);
}
