/* image_output.h
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


#ifndef IMAGE_OUTPUT_H
#define IMAGE_OUTPUT_H

#include <opencv/cv.hpp>
#include "opencv2/core.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>


using namespace cv;
using namespace std;

class ImageOutput {
public:
  //@constructor with default values
  ImageOutput();
  //@constructor with specific values
  ImageOutput(new_to_file, VideoWriter** new_video_list, char** new_name_list, int video_count);
  //@destructor
  ~ImageOutput();

  //@updates data members
  void setup(new_to_file, VideoWriter** new_video_list, char** new_name_list, int video_count);

  //@outputs the requested frames
  void output_track_frame(Mat &frame);
  void output_debug_frames(Mat** frame);
  void output_index_frame(Mat &frame, int ind);

  //@closes output
  void close_track_frame(Mat &frame);
  void close_debug_frames(Mat** frame);
  void close_index_frame(Mat &frame, int ind);

private:
  bool to_file;  // whether the output should be written to file or screen
                 // note: cannot be changed after initial setup

  VideoWriter** video_list;  // list of video files for output
  char**        name_list;   // list of video names
                             // note: first in list is main (tracking) video
  int           video_count; // how many videos there are
};

#endif