#include <iostream>

using namespace std;

int main(){
    int x = 4;
    int y = 6;
    int &a = x;
    cout << a << endl;
    &a = y;
    cout << a << endl;
    cout << x << endl;
}   