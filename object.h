/* object.h
 * a class that defines an object for tracking in openCV
 * these are blobs of movement that have been id'd by openCV
 * maybe one, several, or part of a trackable thing (target)
 
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

#ifndef OBJECT_H
#define OBJECT_H


#include <opencv/cv.hpp>
#include "opencv2/core.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

class Object {
public:
  //@creates new object
  Object();
  //@creates new object
  //      center is center of mass of contour
  Object(const vector<Point> &contour);
  //@creates new object
  //@post data set to inputs
  Object(const Point2d &new_center, Rect2d &new_box);
  //@copies input object to a new one
  Object(const Object &other);
  //@default destructor
  ~Object();

  //@calculate the distance between the center and the input point
  //@pre other center is defined
  //@returns the euclidian distance ^2 (because sqrt is $$$)
  double find_distance_sqd(const Point2d &other_center) const;
  //@calculate the distance between the center and the center of the input
  //@pre center of other_object is defined
  //@returns the euclidian distance ^2 (because sqrt is $$$)
  double find_distance_sqd(const Object &other_object) const;
  
  //@checks if two objects overlap
  bool overlaps(const Object &other_object) const;
  //@calculates area of overlap
  double find_overlap_area(const Object &other_object) const;
  //@calculates the overlap area divided by this object's area
  //  intended to be less dependant on object area
  double find_overlap_fraction(const Object &other_object) const;


  //@sets center to input
  void set_center(Point2d &new_center);
  //@sets center to center of mass of contour
  void set_center(vector<Point> &contour);
  //@sets box to input
  void set_box(Rect2d &new_box);
  //@sets is found to true (or false)
  void set_is_found(bool _is_found = true);

  //@copies center to out_center
  void get_center(Point2d &out_center) const;
  //@copies box to out_box
  void get_box(Rect2d &out_box) const;
  //@gets some of the box properties
  double get_box_height();
  double get_box_width();
  //@what do you think it does?
  bool get_is_found() const;

private:
  Point2d center;    // point that indicates object center
  Rect2d  box;       // bounding box for object (x, y, width, height are double)
  bool    is_found;  // boolean indicating that the object has been id'd as an existing target(s)
};

#endif