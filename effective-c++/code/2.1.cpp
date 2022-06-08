#include <iostream>
using namespace std;
class C
{
public:
    C(int i) : num(i) { cout << num << endl; };
    inline int getnumTurn() { return numTurns; }
    static const int numTurns = 5;
    enum // enum无法被取地址
    {
        errorNum = 10
    };

private:
    int num;
};
//放在类上面会报错
//不定义下行也会报错undefined reference to `C::numTurns'
const int C::numTurns;
int main(int argc, char **argv)
{
    C(100);
    C c(100);
    // int *enumptr = &(C::errorNum);
    const int *p = &(C::numTurns);
    cout << *p << endl;
    cout << C::numTurns << endl; // cout<<c.getnumTurn();

    return 0;
}