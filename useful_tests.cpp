/* useful_tests.cpp
 * testing for the useful functions
 * very much a wrok in progress
 
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

#include "useful_functions.h"

using namespace std;
using namespace cv;

void test_char_cat(){
  char* f = (char*)"hello";
  char* s = (char*)" world";
  string o;

  cout << "* * * * * testing char cat * * * * *" << endl;
  
  o = char_cat(f, s);
  cout << "Concatinating '" << f << "' with '" << s << endl;
  cout << "Expected: '" << f << s << "'" << endl;
  cout << "Actual:   '" << o << "'" << endl;  
}

void test_char_copy(){
  char** one_list    = new char*[2];
  char** two_list    = NULL;
  one_list[0] = (char*)"tracking_video";
  one_list[1] = (char*)"debugging_video";

  cout << "* * * * * testing copy char list * * * * *" << endl;

  copy_char_list(one_list, 2, two_list);

  if(two_list == NULL) {
    cout << "ERROR second list is NULL" << endl;

  } else if(two_list[0] == NULL) {
    cout << "ERROR second list first string is NULL" << endl;

  } else if(two_list[1] == NULL) {
    cout << "ERROR second list second string is NULL" << endl;

  } else {
    cout << "Expected:" << endl;
    cout << one_list[0] << endl;
    cout << one_list[1] << endl;
    cout << endl;
    delete[] one_list;

    cout << "Actual:" << endl;
    cout << two_list[0] << endl;
    cout << two_list[1] << endl;
  }
  
  delete_char_list(two_list, 2);
}

int main(int argc, char** argv){

	cout << endl << endl << "*********** TESTING USEFUL FUNCTIONS ***********" << endl << endl;

  // test_char_cat();
  test_char_copy();

	cout << endl << endl << "***************** DONE TESTING *****************" << endl << endl;

}
