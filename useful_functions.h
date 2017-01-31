/* useful_functions.cpp
 * various functions that are useful to the project but don't belong anywhere in particular
 
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

#ifndef USEFUL_FUNCTIONS_H
#define USEFUL_FUNCTIONS_H

#include <opencv/cv.hpp>
//#include <opencv/highgui.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cv;

int char_to_int(char* c);
int str_to_int(string s);
string int_to_str(int i);
string char_cat(char* first, char* second);
void copy_char_list(char** in, int in_count, char** &out);
void delete_char_list(char** &list, int len);
void display_window(string name, Mat& img, bool wait = false);
void get_background(string vid_name, Mat& avg_frame);

#endif