#include <iostream>
using namespace std;

// Convert base8 to base10 using only bitshifts and addition to save precious time and memory
// that processors such as microcontrollers might not have due to the amount of clock cycles
// the multiply instruction takes, GG 

int main(){
    int base8Num;
    int sum = 0; // I got a weird bug where the sum int overflowed to max when i didnt set it = 0
    int n = 0;
    cin  >> base8Num;
    while(base8Num > 0){
        // EXPLANATION:
        // we take the first digit of the octal num, bitshift 3*n times to get the powers of 8
        // then set the octal num as itself divided by 10, so we remove the first digit basically
        // then repeat the entire algorithm  
        sum += ((base8Num%10) << (3*n));      
        base8Num = base8Num/10;
        n++;
    }
    cout << sum;
    return 0;
}