// Count number of 1s and 0s in n bytes, by using modulo 2
#include <iostream>
using namespace std;
int main(){
    unsigned int Number;
    int countOnes=0;
    int countZeroes=0;
    cout << "Insert: ";
    cin >> Number;
    while(Number!=0){
        if(Number%2==1){
            countOnes++;
        }else{
            countZeroes++;
        }
        Number = Number/2;
    }
    cout << 32-countOnes << "\n" << countOnes;
    return 0;
}