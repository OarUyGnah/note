#include <string>
#include <iostream>
using namespace std;
template <class T>
class A
{
public:
    A(std::string &name, const T &val) : name(name), obj(val) {}
    void print()
    {
        cout << "name:" << name << "\nT:" << obj << endl;
    }
    A &operator=(const A a)
    {
        this->name = a.name;
        return *this;
    }

private:
    std::string &name;
    const T obj;
};

int main()
{
    std::string a("123");
    std::string b("abc");
    A<int> p(a, 2);
    A<int> s(b, 3);
    p.print();
    s.print();
    (p = s).print();
    p.print();
    s.print();
}