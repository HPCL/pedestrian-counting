/* trackers.h
 * a class that defines tracking functions for the pedestrian couter
 * note that all methods in this class will be static
 
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



#ifndef TRACKERS_H
#define TRACKERS_H

#include <opencv/cv.hpp>
#include "opencv2/core.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "object.h"
#include "useful_functions.h"

class Trackers
{
public:
  // tracking algorithms
  //@match object from one set to another
  //@params old_objs is the list of objects from the first frame
  //        curr_obj one of the objects from the second frome
  //@returns pointer to the old object that is expected to be associated with curr
  //         or void if nothing relevant is located
  //@usage find previous object is called and chooses a specific func based on algo
  static Object* find_previous_object(vector<Object> &old_objs, Object &curr_obj);
  static Object* find_previous_object_dist(vector<Object> &old_objs, Object &curr_obj);
  static Object* find_previous_object_overlap(vector<Object> &old_objs, Object &curr_obj);

  //getters and setters
  static char get_algo();
  static void set_algo(char new_algo);

  static double get_max_dist_sqd();
  static void   set_max_dist_sqd(double new_max_dist_sqd);

private:
  static char algo; // algorithm choice
  static double max_dist_sqd; // aximum distance between to centers to consider it one object
};

#endif