#include <iostream>
using namespace std;
// S = 1*2 + 2*3 + ... + N*(N+1)    
int main(){
    unsigned long int number;
    cin >> number;
    unsigned long long sum = 0;
    while(number >= 1){
        sum += number*(number+1);
        number--;
    }
    cout << sum;
    return 0;
}