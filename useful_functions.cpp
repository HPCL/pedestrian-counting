/* useful_functions.cpp
 * various functions that are useful to the project but don't belong anywhere in particular
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

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

#include "useful_functions.h"

using namespace std;
using namespace cv;

/************************************** utilities **************************************/
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


//@displays window or crashes program
void display_window(string name, Mat& img, bool wait /*= false*/) {
  if(img.empty()) {
    cout <<  "Could not open or find the image" << std::endl ;
    exit(1);
  }
	imshow(name, img);
	resizeWindow(name, 512, 384);
	waitKey(1);
	if(wait)
		waitKey();
}

/******************************* average for background ********************************/

void get_background(string vid_name, Mat& avg_frame) {
	Mat next_frame;
	VideoCapture capture;
	int frame_count = 0;
	bool success;

	cout << "opening video" << endl;
	capture.open(vid_name);
	if(!capture.isOpened()){
		cout<<"ERROR ACQUIRING VIDEO FEED\n";
		getchar();
		exit(1);
	}

	success = capture.read(avg_frame);
	avg_frame.convertTo(avg_frame, CV_32F);
	if(!success){
		cout << endl << "ERROR: next frame failed to be read" << endl;
		exit(1);
	}
	frame_count++;
	cvtColor(avg_frame, avg_frame, COLOR_BGR2GRAY);

  // display_window("Average", avg_frame);

	success = capture.read(next_frame);
	next_frame.convertTo(next_frame, CV_32F);
	if(!success){
		cout << endl << "ERROR: next frame failed to be read" << endl;
		exit(1);
	}

	while( success ) {
		// cout << "frame: " << frame_count << endl;
		frame_count++;
		cvtColor(next_frame, next_frame, COLOR_BGR2GRAY);

		//avg_frame = ( ((frame_count-1)*avg_frame) + next_frame) / frame_count;
		avg_frame = avg_frame + next_frame;

  	// display_window("Average", avg_frame);
		
		success = capture.read(next_frame);
		next_frame.convertTo(next_frame, CV_32F);
	} // inner while loop
	avg_frame = avg_frame / frame_count;
	cout << "done processing." << endl;
	
	// avg_frame.convertTo(avg_frame, CV_16U);
	//display_window("Average", avg_frame, true);
	//release the capture before re-opening and looping again.
	capture.release();
}
