#include "string"
#include "iostream"
using namespace std;

extern "C" {
  void out(){
    cout<<"extern C out test!"<<endl;  
  }

  int add(int x, int y){
    int sum = x+y;
    cout<<"sum:"<<sum<<endl;
    return sum;
  }
}
