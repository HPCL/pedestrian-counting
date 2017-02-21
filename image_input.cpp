/* camera.cpp
 * class for retrieving camera input regardless of whether it's a stream or file
 */

#include "image_input.h"

using namespace cv;
using namespace std;

#if IS_RASP_PI
ImageInput::ImageInput(){
  isLive = true;
}
#else
ImageInput::ImageInput(){
  isLive = false;
}
#endif

ImageInput::ImageInput(String vid_name){
  isLive = false;
  this->vid_name = vid_name;
}

ImageInput::~ImageInput() {}


#if IS_RASP_PI
bool ImageInput::open(){
  if (isLive){
    return rasp_cam.open();
  } else {
    vid_capture.open(vid_name);
    return vid_capture.isOpened();
  }
}
#else
bool ImageInput::open(){
  vid_capture.open(vid_name);
  return vid_capture.isOpened();
}
#endif


#if IS_RASP_PI
bool ImageInput::read(Mat& frame) {
  if(isLive) {
    rasp_cam.grab();
    rasp_cam.retrieve(frame);
    return !(frame.empty());
  } else {
    return vid_capture.read(frame);
  }
}
#else
bool ImageInput::read(Mat& frame) {
  return vid_capture.read(frame);
}
#endif

double ImageInput::get(int prop_id){
  return vid_capture.get(prop_id);
}


#if IS_RASP_PI
void ImageInput::release() {
  if (isLive) {
    rasp_cam.release();
  } else {
    vid_capture.release();
  }
}
#else
void ImageInput::release() {
  vid_capture.release();
}
#endif
