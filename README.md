# pedestrian-counting
a program to count pedestrians and bikers with a raspberry pi and OpenCV

## TODO
  * clean up code
  * add kalman filter 
  * target class (might be deprecated)
  * algorithms
  * accurate counting
    * distance error?
    * discretization
    * overlap
  * make sure everything is properly documented
    * use readthedocs.org instead of markdown
  * add more options to the settings
  * test live stream option
  * get more test videos

## counter.cpp
  * main counting program

## object.h and .cpp
  * object class
  * essentially a center point that id's an object being tracked

## image_input.h and .cpp
  * image input class
  * wraps the raspicamera and video file options up so that the other programs don't care

## image_output.h and .cpp
  * image output class
  * abstracts outputing to file or to the screen

## trackers.h and .cpp
  * static class that basically just a bunch of tracking algorithms
  * matches object or targets from frame to frame

## get_background.cpp
  * simple program to average a video and find background
  * not working that great, but could be due to shaky video

## useful_functions.cpp
  * a couple functions that I wanted to have available in multiple places

## Also good to mention
  * make sure you add the opencv stuff to your path: [path to opencv]/include
    * in our case /usr/local/packages/opencv/include
    * or do the -I thing when compiling
    * also -L for the libraries

## Acknowledgements
### Much of this code is based on helpful tutorials available here:
  * http://docs.opencv.org/3.1.0/d5/d07/tutorial_multitracker.html#gsc.tab=0
  * https://www.youtube.com/user/khounslow/featured
  * http://docs.opencv.org/3.1.0/d1/dc5/tutorial_background_subtraction.html

### Also:
  * http://opencv.org/
