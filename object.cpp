/* object.cpp
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

 #include "object.h"
using namespace std;
using namespace cv;

Object::Object() {
  id = -1;
  is_counted = false;
}

Object::Object(vector<Point> &contour) {
  Moments temp_moment = moments(contour);
  center = Point2d(temp_moment.m10/temp_moment.m00 , temp_moment.m01/temp_moment.m00);
  id = -1;
  is_counted = false;
}

Object::Object(Point2d &new_center, int new_id) {
  center = Point2d(new_center);
  id = new_id;
  is_counted = false;
}

Object::~Object() {

}


double Object::find_distance_sqd(Point2d &other_center) {
  Point2d diff = center - other_center;
  return (diff.x * diff.x) + (diff.y * diff.y);
}

double Object::find_distance_sqd(Object &other_object) {
  return find_distance(other_object.get_center());
}


/*************************** Setters ***************************/

void Object::set_center(Point2d &new_center) {
  center = Point2d(new_center);
}

void Object::set_id(int new_id) {
  id = new_id;
}

void Object::set_is_counted(bool new_is_counted = true){
  is_counted = new_is_counted;
}


/*************************** Getters ***************************/

void Object::get_center(Point2d &out_center) {
  out_center = Point2d(center);
}

int  Object::get_id() {
  return id;
}

bool Object::get_is_counted() {
  return is_counted;
}