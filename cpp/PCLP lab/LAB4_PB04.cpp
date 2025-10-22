#include <iostream>
using namespace std;
int main(){
    long int number;
    int i = 1, digits = 0;
    bool isEqual = 1;
    while(isEqual){
        cin >> number;
        while(number != 0){
            number = number / (10*i); 
            digits++;
            i++;
        }

        cout << digits;
        }
    return 0;
}