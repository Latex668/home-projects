#include <iostream>
// S = 1-2+3-4+...+/-n
// 1-2+3-4+5-6
using namespace std;
int main(){
    long int number = 0;
    cin >> number;
    if(number%2 == 1){
        cout << number-((number-1)/2);
    }else{
        cout << (number*(-1))/2;
    }
    return 0;
}