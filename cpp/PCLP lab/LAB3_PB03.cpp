#include <iostream>
using namespace std;

// int convertB8toB10(int num){
//     int sum = 0; // I got a weird bug where the sum int overflowed to max when i didnt set it = 0
//     int n = 0;
//     while(num != 0){
//         // EXPLANATION:
//         // we take the first digit of the octal num, bitshift 3*n times to get the powers of 8
//         // then set the octal num as itself divided by 10, so we remove the first digit basically
//         // then repeat the entire algorithm  
//         sum += ((num%10) << (3*n));      
//         num = num/10;
//         n++;
//     }
//     return sum;
// }
    
    int convertB10toB8(int num){
        int sum = 0;
        int pwr = 0;
        int carry = 0;
        while(num > 0){
            carry = 
            sum += ((num%8) * pwr *10);
            num /= 8;
        }
        return sum;
    }
int main(){
    int num1, num2;
    long long int sum=0;
    cin >> num1 >> num2;
    // num1 = convertB8toB10(num1);
    // num2 = convertB8toB10(num2);
    // sum = num1 + num2;
    cout << "n1 " << num1 << " n2 " << num2 << " b8 " << convertB10toB8(num1);
    return 0;
}