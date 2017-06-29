#include <iostream>
#include <unistd.h>
using namespace std;

int main(int argc, char** argv) {
  // for(int i = 0; i < argc; i++) {
  //   cout << argv[i] << endl;
  // }

  for(int i = 0; i < 10; i++) {
    sleep(2);
    cout << "HELLO" << endl;
  };
}
