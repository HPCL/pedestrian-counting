/* target.cpp
 * an implementation of a class for pedestrians, bikers, or cars moving through a video
 * these objects are things that have been identified and are being tracked through a video

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


#include "target.h"

using namespace std;
using namespace cv;

int Target::next_id = 0;
double Target::default_x_velocity = 0.0;
double Target::default_y_velocity = 0.0;

Target::Target() {
  id = next_id++;
  frame_count = 1;
  vector<Object> obj_list;
  double x_velocity = default_x_velocity;
  double y_velocity = default_y_velocity;
  Rect2d box;
}

Target::Target(const Object obj) {
  id = next_id++;
  frame_count = 1;
  vector<Object> obj_list;
  obj_list.push_back(obj);
  double x_velocity = default_x_velocity;
  double y_velocity = default_y_velocity;
  Rect2d box;
  other.get_box(box);
}

//TODO
Target::Target(const Target other) {
  id = other.get_frame_count();
  frame_count = other.get_frame_count();
  vector<Object> obj_list;
  other.get_obj_list(obj_list);
  double x_velocity = other.get_x_velocity();
  double y_velocity = other.get_y_velocity();
  Rect2d box;
  other.get_box(box);
}

//TODO
Target::~Target() {

}



double Target::get_overlap_area(Target other) {
  Rect2d other_box;

  other.get_box(other_box);
  return (box & other_box).area();
}


double Target::get_overlap_area(Object other) {
  Rect2d other_box;

  other.get_box(other_box);
  return (box & other_box).area();
}

double Target::get_overlap_fraction(Target other) {
  Rect2d other_box;

  other.get_box(other_box);
  return (box & other_box).area() / box.area();
}


double Target::get_overlap_fraction(Object other) {
  Rect2d other_box;

  other.get_box(other_box);
  return (box & other_box).area() / box.area();
}

//TODO
Rect2d Target::get_predicted_target() {

}

//TODO some type of update function?

/****** Getters and Setters ******/
void Target::set_id(int new_id) {
  id = new_id;
}

void Target::set_frame_count(int new_frame_count) {
  frame_count = new_frame_count;
}

void Target::inc_frame_count(int inc = 1) {
  frame_count += inc;
}

void Target::set_obj_list(const vector<Object> &new_obj_list) {
  for (vector<Object>::iterator i = new_obj_list.begin(); i != new_obj_list.end(); i++) {
    obj_list.push_back(*it);
  }
}

void Target::set_x_velocity(double new_x) {
  x_velocity = new_x;
}

void Target::set_y_velocity(double new_y) {
  y_velocity = new_y;
}

void Target::set_box(Rect2d &new_box) {
  box = Rect2d(new_box);
}


int  Target::get_id() const {
  return id;
}

int  Target::get_frame_count() const {
  return frame_count;
}


void Target::get_obj_list(vector<Object> &new_obj_list) const {
  for (vector<Object>::iterator i = obj_list.begin(); i != new_obj_list.end(); i++) {
    new_obj_list.push_back(*it);
  }
}

double Target::get_x_velocity() const {
  return x_velocity;
}

double Target::get_y_velocity() const {
  return y_velocity;
}

void Target::get_box(Rect2d &out_box) const {
  out_box = Rect2d(box);
}

//@gets some of the box properties
double Target::get_box_height() const {
  return box.height;
}

double Target::get_box_width() const {
  return box.width;
}


