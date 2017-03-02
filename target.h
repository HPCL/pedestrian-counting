/* target.h
 * a class for pedestrians, bikers, or cars moving through a video
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


#ifndef TARGET_H
#define TARGET_H

#include <opencv/cv.hpp>
#include "opencv2/core.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

using namespace std;
using namespace cv;

class Target {
public:
  Target();
  Target(Object obj);
  Target(Target other);
  ~Target();

  //TODO documentation
  double get_overlap_area(const Target other);
  double get_overlap_area(const Object other);
  double get_overlap_fraction(const Target other);
  double get_overlap_fraction(const Object other);
  Rect2d get_predicted_target();
  //TODO some type of update function?

  /****** Getters and Setters ******/
  void set_id(int new_id);
  void set_frame_count(int new_frame_count);
  void inc_frame_count(int inc /*= 1*/);
  void set_obj_list(const vector<Object> &new_obj_list);
  void set_x_velocity(double new_x);
  void set_y_velocity(double new_y);
  void set_box(Rect2d &new_box);

  int  get_id() const;
  int  get_frame_count() const;
  void get_obj_list(vector<Object> &new_obj_list) const;
  double get_x_velocity() const;
  double get_y_velocity() const;
  void get_box(Rect2d &out_box) const;
  //@gets some of the box properties
  double get_box_height() const;
  double get_box_width() const;


private:
  int id;                   // unique id number
  int frame_count;          // how long this thing has been tracked
  vector<Object> obj_list;  // shallow list of objects that make up this target
  double x_velocity;        // speed in the x direction
  double y_velocity;        // speed in the y direction
  Rect2d box;               // the box around the target

  static int next_id;
  static double default_x_velocity; //TODO make this meaningful
  static double default_y_velocity;

};


#endif