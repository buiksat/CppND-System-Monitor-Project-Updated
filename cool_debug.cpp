#include <iostream>
#include <string>

#define printVariableNameAndValue(x) cout<<"The name of variable **"<<(#x)<<"** and the value of variable is => "<<x<<"\n"
// I am talking about this piece of code ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

using namespace std;


int main(){
    int n = 25;
    string myFirstStringEver = "Hello World";

    printVariableNameAndValue(n);
    printVariableNameAndValue(myFirstStringEver);

}
