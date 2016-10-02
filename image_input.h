/* image_input.h
 * class for retrieving camera input regardless of whether it's a stream or file
 */

#ifndef IMAGE_INPUT_H
#define IMAGE_INPUT_H

#include <raspicam/raspicam.h>
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;

class ImageInput {
public:
  ImageInput();
  // Constructor for live feed
  ImageInput(cv::String vid_name);
  // Constructor for video from file
  ~ImageInput();
  // Generic destructor
  bool open();
  // opens the capture
  bool read(Mat& frame);
  // Get next frame
  void release();
  // Release input
private:
  bool         isLive;    // whether to use a live stream or video file
  string       vid_name;  // filepath of video, if capturing from file
  VideoCapture capture;   // oject to read video from file
};

 #endif
