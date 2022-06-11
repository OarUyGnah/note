#include <vector>
#include <iostream>
//=======================
/* namespace WidgetStuff
{
    class WidgetImpl;
    class Widget;

    class WidgetImpl
    {
    public:
        WidgetImpl(int aa, int bb, int cc, std::vector<double> v)
            : a(aa), b(bb), c(cc), v(v) {}

    private:
        //数据多意味着复制时间很长
        int a,
            b,
            c;
        std::vector<double> v;
    };
    class Widget
    {
    public:
        Widget(WidgetImpl *p) : pImpl(p) { std::cout << "widget ctor" << std::endl; }
        Widget(const Widget &rhs)
        {
            std::cout << "copy constructor" << std::endl;
            this->pImpl = rhs.pImpl;
        }
        Widget &operator=(const Widget &rhs)
        {
            std::cout << "widget operator=" << std::endl;
            *pImpl = *(rhs.pImpl);
            return *this;
        }
        void swap(Widget &rhs)
        {
            using std::swap;
            std::swap(pImpl, rhs.pImpl);
        }
        void printPtr()
        {
            std::cout << pImpl << std::endl;
        }

    private:
        WidgetImpl *pImpl;
    };

    WidgetImpl *initWidgetImpl()
    {
        WidgetImpl *res = new WidgetStuff::WidgetImpl(0, 1, 2, {1, 2, 3});
        return res;
    }
}
namespace std
{
    template <>
    void swap<WidgetStuff::Widget>(WidgetStuff::Widget &lhs, WidgetStuff::Widget &rhs)
    {
        lhs.swap(rhs);
    }
} */

//=======================

//模板class
namespace WidgetStuff
{
    template <typename T>
    class WidgetImpl
    {
    public:
        WidgetImpl(T aa, T bb, T cc, std::vector<T> v)
            : a(aa), b(bb), c(cc), v(v) {}

    private:
        //数据多意味着复制时间很长
        T a,
            b,
            c;
        std::vector<T> v;
    };
    template <typename T>
    class Widget
    {
    public:
        Widget(WidgetImpl<T> *p) : pImpl(p) { std::cout << "widget ctor" << std::endl; }
        Widget(const Widget<T> &rhs)
        {
            std::cout << "copy constructor" << std::endl;
            this->pImpl = rhs.pImpl;
        }
        Widget<T> &operator=(const Widget<T> &rhs)
        {
            std::cout << "widget operator=" << std::endl;
            *pImpl = *(rhs.pImpl);
            return *this;
        }
        void swap(Widget<T> &rhs)
        {
            using std::swap; // using 声明式
            swap(pImpl, rhs.pImpl);
        }
        void printPtr()
        {
            std::cout << pImpl << std::endl;
        }

    private:
        WidgetImpl<T> *pImpl;
    };

    template <typename T>
    void swap(WidgetStuff::Widget<T> &lhs, WidgetStuff::Widget<T> &rhs)
    {
        std::cout << "other swap" << std::endl;
        using std::swap;
        lhs.swap(rhs);
    }

    template <typename T>
    WidgetImpl<T> *initWidgetImpl()
    {
        WidgetImpl<T> *res = new WidgetStuff::WidgetImpl<T>(0, 1, 2, {1, 2, 3});
        return res;
    }
}

int main()
{
    /* WidgetStuff::Widget w1(WidgetStuff::initWidgetImpl());
    WidgetStuff::Widget w2(WidgetStuff::initWidgetImpl());
    w1.printPtr();
    w2.printPtr();
    w1.swap(w2);
    std::swap(w1, w2);
    w1.printPtr();
    w2.printPtr(); */

    //模板类
    WidgetStuff::Widget<int> w1(WidgetStuff::initWidgetImpl<int>());
    WidgetStuff::Widget<int> w2(WidgetStuff::initWidgetImpl<int>());
    w1.printPtr();
    w2.printPtr();
    w1.swap(w2);
    WidgetStuff::swap(w1, w2);
    using namespace std;
    //会优先使用WidgeStuff的swap，没有则使用std::swap
    swap(w1, w2);

    w1.printPtr();
    w2.printPtr();
}