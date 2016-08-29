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

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

 */

#ifndef OBJECT_H
#define OBJECT_H

#include <opencv/cv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

class Object {
public:
  Object();
  Object(vector<Point> contour);
  Object(Point2d new_center, int new_id);
  ~Object();

  double find_distance_sqd(Point2d other_center);
  double find_distance_sqd(Object other_object);

  void set_center(Point2d &new_center);
  void set_id(int new_id);
  void set_is_counted(bool new_is_counted = true);

  void get_center(Point2d &out_center);
  int  get_id();

private:
  Point2d center;
  int     id;
  bool    is_counted;
};

#endif