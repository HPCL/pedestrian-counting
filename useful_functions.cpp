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

int str_to_int(string s) {
	return atoi(s.c_str());
}

string int_to_str(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}

//@concatonates char* to a string
string char_cat(char* first, char* second) {
	stringstream ss;
	ss << first << second;
	return ss.str();
}

//@copies a list of char* to a different list of char*
//@pre out should be empty
//@note you must delete the chars in the new list
void copy_char_list(char** in, int in_count, char** &out) {
	int char_count = 0;
	out = new char*[in_count]; 

	for (int i = 0; i < in_count; i++) {

		char_count = 0;
		while(in[i][char_count++] != '\0'); //includes \0 in count

		out[i] = new char[char_count];
		for (int j = 0; j < char_count; j++) {
			out[i][j] = in[i][j];
		}

	} //outer loop
} // copy_char_count

void delete_char_list(char** &list, int len) {
  for (int i = 0; i < len; i++) {
    delete list[i];
  }
  delete[] list;  
}

//@displays window or crashes program (not really in use)
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
	
	avg_frame.convertTo(avg_frame, CV_8UC1);
	//display_window("Average", avg_frame, true);
	//release the capture before re-opening and looping again.
	capture.release();
}
