## std::forward完美转发

- #### `std::forward` 的本质也是进行强制类型转换，形参为左值时返回左值引用，形参为右值时返回右值引用

```cc
#include <iostream>
#include <utility>

template <typename _Tp>
constexpr _Tp &&
forward(typename std::remove_reference<_Tp>::type &__t) noexcept
{
    std::cout << "arg is &" << std::endl;
    return static_cast<_Tp &&>(__t);
}

template <typename _Tp>
constexpr _Tp &&
forward(typename std::remove_reference<_Tp>::type &&__t) noexcept
{
    std::cout << "arg is &&" << std::endl;

    /* static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
                                                         " substituting _Tp is an lvalue reference type"); */
    return static_cast<_Tp &&>(__t);
}
void Print(int& val) {
    std::cout << "lvalue refrence: val=" << val << std::endl;
}

void Print(int&& val) {
    std::cout << "rvalue refrence: val=" << val << std::endl;
}

template<typename T>
void TPrint(T &&t) {
    return Print(t);
}

int main() {
    int date = 1021;
    TPrint(date);
    TPrint(501);

    return 0;
}
/*
运行结果:
lvalue refrence: val=1021
lvalue refrence: val=501
*/
```

本应该是传入一个左值、一个右值，但结果都是左值，**原因在于右值引用是一个左值**

```cc
int i = 101;
int& li = i;

int&& ri = 120;
/*
这段代码中哪些是左值，哪些是右值呢？可以肯定的是 i、li 是左值， 101、120 是右值，而ri也是左值，因为它也一个可以取地址并长期有效的变量啊，只不过这个左值引用了一个右值而已。
*/
```

进行如下修改

```cc
template<typename T>
void TPrint(T&& t) {
    return Print(forward<T>(t));
}
```

当调用`TPrint(501);`时，特化为以下版本，会调用以下函数

```cc
void TPrint(int&& t) {
    return Print(std::forward<int>(t));
}
//因为t是右值引用为左值
template <typename _Tp>
constexpr _Tp &&
forward(typename std::remove_reference<_Tp>::type &__t) noexcept
{
    std::cout << "arg is &" << std::endl;
    return static_cast<_Tp &&>(__t);
}
```

#### 这里的区别就在于，使用 `std::forward` 之前的 `int&&` 是有名字的变量 `t`，它是一个左值，而使用 `std::forward` 之后的 `int&&` 是有个匿名变量，它是一个右值，真正的差距就在这里。

当调用`TPrint(date);`时，特化为以下版本，会调用以下函数

```cc
void TPrint(int& && t) {
    return Print(std::forward<int&>(t));
}

template <typename _Tp>
constexpr _Tp &&
forward(typename std::remove_reference<_Tp>::type &__t) noexcept
{
    std::cout << "arg is &" << std::endl;
    return static_cast<_Tp &&>(__t);
}
```

