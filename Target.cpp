/*
 * Target.cpp
 * wrapper to manage data going in and out of Kalman filter
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * See LICENSE file for licensing information and boring legal stuff

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate, coffee, or introductions to potential employers.

*/

#include "Target.hpp"

int Target::next_id = 0;
double Target::update_time = 0.0;
double Target::max_dist_sqd = 6000000;

Target::Target(int _n, int _m, KALMAN_TYPE* A_init, KALMAN_TYPE* C_init, KALMAN_TYPE* Q_init, KALMAN_TYPE* R_init, KALMAN_TYPE* P_init, KALMAN_TYPE* x_hat_init) {
  n = _n;
  m = _m;

  num_steps = 0;
  num_empty_steps = 0;
  update_time = 0.0;

  radius = 10000;
  id_num = next_id++;

  is_counted = false;

  bool success;

  success = allocate_matrices(&A, &C, &Q, &R, &P, &K, n, m);
  success = success && allocate_vectors(&x, &y, &x_hat, n, m);
  success = success && allocate_temp_matrices(&x_hat_new, &A_T, &C_T, &id,
                                              &temp_1, &temp_2, &temp_3, &temp_4, n, m);

  if( !success ) {
    printf("\n\nERROR allocating matrices in Target class\n\n");
    exit(1);
  }

  copy_mat(A_init, A, n * n);
  copy_mat(C_init, C, n * m);
  copy_mat(Q_init, Q, n * n);
  copy_mat(R_init, R, m * m);
  copy_mat(P_init, P, n * n);
  copy_mat(x_hat_init, x_hat, n);
}

Target::~Target() {
  destroy_matrices(A, C, Q, R, P, K);
  destroy_vectors(x, y, x_hat);
  destroy_temp_matrices(x_hat_new, A_T, C_T, id, temp_1, temp_2, temp_3, temp_4);
}


void Target::update(vector<Object> &objects, double dt) {

  bool found;
  clock_t t1,t2,t3,t4;

  t1 = clock();
  predict();
  t2 = clock();
  found = choose_next_point(objects);
  t3 = clock();
  correct(found);
  t4 = clock();

  update_time += ((t2-t1) + (t4-t3));

  num_steps++;
}

void Target::predict() {
  ::predict(x_hat, n, m, A, Q, P, x_hat_new, A_T, temp_1, temp_2);
}

void Target::correct(bool found) {
  if(found) {
    ::correct(y, x_hat, n, m, C, R, P, K, x_hat_new, C_T, id, temp_1, temp_2, temp_3, temp_4);
  } else {
    num_empty_steps++;
  }
}

bool Target::choose_next_point(vector<Object> &objects) {
  bool found = false;

  double dist, min_dist = -1;
  double dx, dy;
  Object* min_obj;
  Point2d min_center;
  Point2d prev_center(x_hat[0], x_hat[3]); //TODO this is very specific to how the kalman filter is made. please fix

  for(vector<Object>::iterator it_obj = objects.begin(); it_obj != objects.end(); it_obj++) {
    dist = it_obj->find_distance_sqd(prev_center);
    if( (dist <= max_dist_sqd) && ((min_dist < 0) || (dist < min_dist)) ) {
      min_dist = dist; 

      min_obj = &(*it_obj);
      found = true;
    } 
  }

  if (found){
    min_obj->set_is_found();
    prev_obj = Object(*min_obj);
    min_obj->get_center(min_center);
    y[0] = min_center.x;
    y[1] = min_center.y;
  }

  return found;
}





/************************ Getters and Setters *************************/


int  Target::get_num_steps() {
  return num_steps;
}

void Target::set_num_steps(int steps) {
  num_steps = steps;  
}


int  Target::get_num_empty_steps() {
  return num_empty_steps;
}

void Target::set_num_empty_steps(int steps) {
  num_empty_steps = steps;
}


double Target::get_radius() {
  return radius;
}

void Target::set_radius(double r) {
  radius = r;
}



int  Target::get_id_num(){
  return id_num;
}

void Target::set_id_num(int _id_num){
  id_num = _id_num;
}



bool Target::get_is_counted(){
  return is_counted;
}

void Target::set_is_counted(bool new_ic /* = true */){
  is_counted = new_ic;
}