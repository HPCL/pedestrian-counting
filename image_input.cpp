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
  } else {
    capture.open(vid_name);
    return capture.isOpened();
  }
}

bool ImageInput::read(Mat& frame) {
  if(isLive) {
  } else {
    return capture.read(frame);
  }
}

void ImageInput::release() {
  if (isLive) {
  } else {
    capture.release();
  }
}
