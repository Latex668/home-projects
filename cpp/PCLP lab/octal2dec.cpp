#include <iostream>
using namespace std;
int main(){
    int OctalN = 0;
    int octalDigit=0;
    cout << "Insert a num";
    cin  >> OctalN; // forced octal
    while(OctalN != 0){
        octalDigit = OctalN%8;
        OctalN = OctalN-octalDigit;
        cout << OctalN << "-" << octalDigit << "\n";
    }
    return 0;
}