#include <iostream>
using namespace std;
class A
{
public:
    A()
    {
        cout << "A ctor" << endl;
        // log();
    }

    virtual void log() const = 0; // const 一致才能保证多态
};
/* void A::log()
{
    cout << "A log" << endl;
} */
class B : public A
{
public:
    B()
    {
        cout << "B ctor" << endl;
        log();
    }
    virtual void log() const override
    {
        cout << "B log" << endl;
    }
};

class C : public A
{
public:
    C()
    {
        cout << "C ctor" << endl;
        log();
    }
    virtual void log() const override
    {
        cout << "C log" << endl;
    }
};
int main()
{
    B b;
    C c;
}