#include <iostream>
using namespace std;

int main() {
    int* a = new int(3);
    int* b;
    b = new int(*a);
    cout << *b << endl;
    cout << a << endl;
    cout << b << endl;
}