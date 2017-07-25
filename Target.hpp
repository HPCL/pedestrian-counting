/*
 * Target.hpp
 * wrapper to manage data going in and out of Kalman filter
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * See LICENSE file for licensing information and boring legal stuff

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate, coffee, or introductions to potential employers.

*/

#ifndef __TARGET_HPP__
#define __TARGET_HPP__



// my Kalman stuff
extern "C" {
#include "../kalman-filters/basic-c/kalman_filter.h"
#include "../kalman-filters/basic-c/linear_algebra.h"
}

// ped counting stuff
#include "object.h"

// opencv
#include <opencv/cv.hpp>
#include "opencv2/core.hpp"

// generic c++ things
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>

class Target {
public:
 Target(int _n, int _m, TYPE* A_init, TYPE* C_init, TYPE* Q_init, TYPE* R_init, TYPE* P_init, TYPE* x_hat_init);
  ~Target();

  void update(vector<Object> &objects, double dt);
  void predict();
  void correct(bool found);
  bool choose_next_point(vector<Object> &objects);

  int  get_num_steps();
  void set_num_steps(int steps);

  int  get_num_empty_steps();
  void set_num_empty_steps(int steps);

  double get_radius();
  void   set_radius(double r);

  int  get_id_num();
  void set_id_num(int _id_num);

  bool get_is_counted();
  void set_is_counted(bool new_ic = true);

  static int next_id;
  static double max_dist_sqd;

  Object prev_obj;

private:
  
  TYPE *A, *C, *Q, *R, *P, *K, *x, *y, *x_hat,
       *x_hat_new, *A_T, *C_T, *id,
       *temp_1, *temp_2, *temp_3, *temp_4;

  int num_steps, num_empty_steps;
  int n, m;
  double radius; //note this is radius squared
  int id_num;
  bool is_counted;

};

#endif