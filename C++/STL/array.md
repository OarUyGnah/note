## `std::array` 的实现

`std::array` 是封装**固定大小数组的容器**。

此容器是一个**聚合类型**，其语义等同于**保有一个 C 风格数组 T[N] 作为其唯一非静态数据成员**的结构体。

实现 `std::array` 只需要按照规定设置别名，以及提供访问自己保有的数组的成员函数。它的构造函数和析构函数都是隐式声明的。

但是，事实上标准库实现它，还**需要处理一个特殊情况**

### array长度为0 编译可以通过

```cc
#include <array>

int main(){
    std::array<int, 0>arr;
}
```

上面的代码可以通过，但下面这段代码编译会提示使用了非标准扩展 **结构体中零长的数组**

```cc
#include <array>

template<typename T,std::size_t N>
struct array{
    T arr[N];
};

int main(){
    array<int, 0>arr;
}
```

因为c++不允许（极少数情况除外，反正当前情况不允许）零长数组

**真正实现会分情况处理，如偏特化(msvc)**

#### gcc实现

当_Nm==0，则特化为一个空类，空类有两个成员函数，调用了 `operator []` ，那么就会让程序异常终止，如果是 `operator *` 则返回空指针。

下面的代码

`__attribute__((__always_inline__,__noreturn__))` 

> 就是强制内联和让函数不返回。 注意这里的内联和inline关键字没什么关系

`__builtin_trap()` 是什么？

> 简单的说这个东西会**导致程序异常终止**，以及有些时候有助于编译器的优化，具体可[参见](https://link.zhihu.com/?target=https%3A//stackoverflow.com/questions/35734759/builtin-trap-when-to-use-it)。在这里的用途是禁止对 `0` 长度的`std::array` 进行下标访问。

```cc
typename __array_traits<_Tp, _Nm>::_Type        _M_elems;
template<typename _Tp, size_t _Nm>
  struct __array_traits
  {
    using _Type = _Tp[_Nm];
    using _Is_swappable = __is_swappable<_Tp>;
    using _Is_nothrow_swappable = __is_nothrow_swappable<_Tp>;
  };
template<typename _Tp>
 struct __array_traits<_Tp, 0>
 {
   // Empty type used instead of _Tp[0] for std::array<_Tp, 0>.
   struct _Type
   {
     // Indexing is undefined.
     __attribute__((__always_inline__,__noreturn__))
     _Tp& operator[](size_t) const noexcept { __builtin_trap(); }
     // Conversion to a pointer produces a null pointer.
     __attribute__((__always_inline__))
     constexpr operator _Tp*() const noexcept { return nullptr; }
   };
   using _Is_swappable = true_type;
   using _Is_nothrow_swappable = true_type;
 };
```



```cc
#include <array>
#include <iostream>

int main(){
    std::cout << sizeof(std::array<int, 0>);//gcc clang是1 msvc 是4
}
```



#### msvc

单纯的保有一个普通的数组，只是它加了一个**偏特化**，类似重写了一个类，将所有函数全部重写

```cc
template<typename T,std::size_t N>
struct array{
    T arr[N];
};

template <class _Ty>
class array<_Ty, 0>;
```

