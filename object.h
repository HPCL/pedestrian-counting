/* object.h
 * a class that defines an object for tracking in openCV
 
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
  //@post id set to -1, is_counted to false
  Object();
  //@creates new object
  //@post id set to -1, is_counted to false
  //      center is center of mass of contour
  Object(const vector<Point> &contour);
  //@creates new object
  //@post is_counted set to false, others set to inputs
  Object(const Point2d &new_center, int new_id, Rect2d &new_box);
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
  //@credit Sean Gravelle 
  bool find_overlap_area(const Object &other_object) const;


  //@sets center to input
  void set_center(Point2d &new_center);
  //@sets center to center of mass of contour
  void set_center(vector<Point> &contour);
  //@sets id to input, use -1 for undefined id
  void set_id(int new_id);
  //@marks objects as counted (or input false to unmark)
  void set_is_counted(bool new_is_counted = true);
  //@sets box to input
  void set_box(Rect2d &new_box);

  //@copies center to out_center
  void get_center(Point2d &out_center) const;
  //@returns id
  int  get_id() const;
  //@returns is_counted
  bool get_is_counted() const;
  //@copies box to out_box
  void get_box(Rect2d &out_box) const;
  //@gets some of the box properties
  double get_box_height();
  double get_box_width();

private:
  Point2d center;       // point that indicates object center
  int     id;           // object id; -1 means none assigned yet
  bool    is_counted;   // if the object has been counted
  Rect2d  box;          // bounding box for object (x, y, width, height are double)
};

#endif