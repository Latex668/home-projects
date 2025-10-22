#include <iostream>
using namespace std;
int main(){
    unsigned int number = 0;
    unsigned long long int sum = 0;
    cin >> number;
    // cout << sizeof(unsigned long long int) << " bytes";
    for(unsigned int i=0; i<number; i++){
        sum+= i;
    }
    cout << sum;
    return 0;
}