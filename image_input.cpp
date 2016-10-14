/* camera.cpp
 * class for retrieving camera input regardless of whether it's a stream or file
 */

#include "image_input.h"

using namespace cv;
using namespace std;

ImageInput::ImageInput(){
  isLive = true;
}

ImageInput::ImageInput(String vid_name){
  isLive = false;
  this->vid_name = vid_name;
}

ImageInput::~ImageInput() {}

bool ImageInput::open(){
  if (isLive){
    return rasp_cam.open();
  } else {
    vid_capture.open(vid_name);
    return vid_capture.isOpened();
  }
}

bool ImageInput::read(Mat& frame) {
  if(isLive) {
    rasp_cam.grab();
    rasp_cam.retrieve(frame);
    return !(frame.empty());
  } else {
    return vid_capture.read(frame);
  }
}

void ImageInput::release() {
  if (isLive) {
    rasp_cam.release();
  } else {
    vid_capture.release();
  }
}
