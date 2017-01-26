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
  name_list   = ['tracking_video', 'debugging_video'];
  video_count = 2;
  video_list  = new (VideoWriter*)[video_count];
}

//@constructor with specific values
ImageOutput::ImageOutput(bool new_to_file, char** new_name_list, Size new_size, int new_video_count){
  to_file     = new_to_file;
  video_list  = new (VideoWriter*)[video_count];
  name_list   = new_name_list;
  frame_size  = new_size;
  video_count = new_video_count;
}

//@destructor
ImageOutput::~ImageOutput(){
  for (int i = 0; i < video_count; i++) {
    delete video_list[i];
  }
  delete[] video_list;
}


//@updates data members and setup video files if neceessary
//@params
//@return true if success false for failure
bool ImageOutput::setup(bool new_to_file, char** new_name_list, Size new_size, int new_video_count){
  //TODO close and destroy existing videos
  to_file     = new_to_file;
  video_list  = (VideoWriter **)calloc(new_video_count, sizeof(VideoWriter*)); //TODO create these in class
  name_list   = new_name_list;
  video_count = new_video_count;

  //TODO make a func out of this?
  if (to_file) {
    int ex = VideoWriter::fourcc('X','2','6','4');    //TODO make more general?

    for (int i = 0; i < video_count; i++) {
      video_list[i] = new VideoWriter(name_list[i] + ".h264",  ex, 4.0, frame_size);  
      if(!video_list[i]->isOpened()) {
        cout << "ERROR: video writer didn't open" << endl;
        getchar();
        return false;
      }
    }
  } else {
    namedWindow(name_list[i], CV_WINDOW_NORMAL);
  }

  return true;
}

//@outputs the requested frames
char ImageOutput::output_track_frame(Mat &frame){
  if(to_file) {
    video_list[0]->set(CAP_PROP_FRAME_WIDTH, frame.size().width);
    video_list[0]->set(CAP_PROP_FRAME_HEIGHT , frame.size().height);
    video_list[0]->write(frame);
  } else {
    imshow(name_list[0],frame);
    resizeWindow(name_list[0], WIN_HIEGHT, WIN_LENGTH);
    return waitKey(10);
  }
}
 
void ImageOutput::output_debug_frames(Mat** frames){
  for (int i = 1; i < video_count; i++) {
    namedWindow(name_list[i], CV_WINDOW_NORMAL);
    imshow(name_list[i], *(frames[i]);
    resizeWindow(name_list[i], WIN_HIEGHT, WIN_LENGTH);
  }
}



//@closes output
void ImageOutput::close_track_frame(Mat &frame){
  destroyWindow(name_list[0]);
}

void ImageOutput::close_debug_frames(Mat** frame){
  for (int i = 1; i < video_count; i++)
    destroyWindow(name_list[i]);
}


//@getters (no setters only initial setup)
bool ImageOutput::get_to_file() const{
  return to_file;
}

int  ImageOutput::get_video_count() const{
  return video_count;
}

