/* image_output.cpp
 * a class to make it easier to switch between saving opencv output
 * and outputing to the screen

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

 * If by some miracle you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

 */

#include "image_output.h"

using namespace cv;
using namespace std;

//@constructor with default values
ImageOutput::ImageOutput(){
  to_file     = false;
  video_list  = NULL;
  name_list   = ['tracking_video', 'debugging_video'];
  video_count = 2;
}

//@constructor with specific values
ImageOutput::ImageOutput(new_to_file, VideoWriter** new_video_list, char** new_name_list, int new_video_count){
  to_file     = new_to_file;
  video_list  = new_video_list; //TODO create these in class
  name_list   = new_name_list;
  video_count = new_video_count;
}

//@destructor
ImageOutput::~ImageOutput(){
  //TODO we're probably going to have to destry the video writers
}


//@updates data members
void ImageOutput::setup(new_to_file, VideoWriter** new_video_list, char** new_name_list, int new_video_count){
  //TODO close and destroy existing videos
  to_file     = new_to_file;
  video_list  = new_video_list; //TODO create these in class
  name_list   = new_name_list;
  video_count = new_video_count;
}

//@outputs the requested frames
void ImageOutput::output_track_frame(Mat &frame){

}

void ImageOutput::output_debug_frames(Mat** frame){

}

void ImageOutput::output_index_frame(Mat &frame, int ind){

}


//@closes output
void ImageOutput::close_track_frame(Mat &frame){

}

void ImageOutput::close_debug_frames(Mat** frame){

}

void ImageOutput::close_index_frame(Mat &frame, int ind){

}


//@getters (no setters only initial setup)
bool ImageOutput::get_to_file() const{
  return to_file;
}

int  ImageOutput::get_video_count() const{
  return video_count;
}

