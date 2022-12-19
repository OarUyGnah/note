在正式讨论虚函数前，我们需要明确 c++ 的设计思想——**零成本抽象**

对于下面的这个类

```
class A {
public:
    int x;
};


```

这个类的大小为 4，也就是一个 int 的大小。

我们在跑这个类，等同于在跑一个单独的 int

```
class A {
public:
    int x;
};

int main()
{
    cout << sizeof(A) << endl;
    A a;
    int* p = (int*)&a;
    *p = 23333;
    cout << a.x << endl;
    return 0;
}


```

输出

> 4  
> 23333

实际上，在汇编的角度上，更能看出来

![](https://pic4.zhimg.com/v2-15d7ba315b33748af5abe0127aeb7977_r.jpg)

所以，类这个概念，只存在于编译时期。

也就是，我们可以写出修改类中的私有变量的代码（因为，私有这个东西，只在编译时期中存在）

```
class A {
private:
    int x;
public:
    int getx() { return x; }
};
int main()
{
    cout << sizeof(A) << endl;
    A a;
    int* p = (int*)&a;
    *p = 114514;
    cout << a.getx() << endl;
    return 0;
}


```

输出

> 4  
> 114514

这个时候我们发现，函数是不占空间的。

我们写出一个继承

```
class A {
public:
    int x, y;
    void show() { cout << "show" << endl; }
};
class B :public A {
public:
    int z;
};
int main(){
    cout << sizeof(A) << endl;
    cout << sizeof(B) << endl;
    return 0;
}


```

输出

> 8  
> 12

内存模型为

![](https://pic4.zhimg.com/v2-fbff03bbcbbff858dbd5cf74ea35ba9f_r.jpg)

两个类共享一个 show，这个 show 不会占用类的空间（放在别的地方了

输出下 show 的位置

```
printf("%p\n", &A::show);
printf("%p\n", &B::show);


```

输出

> 00007FF75D8A152D  
> 00007FF75D8A152D

好我们来看看虚函数吧！如果你不会虚函数的话可以看下 [c++ 虚函数的作用是什么？](https://www.zhihu.com/question/23971699/answer/26263735)

我们整个带虚函数的类

```
class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
};


```

输出下大小发现是 8。

很怪？我们多给 A 放点东西

```
class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
    int x, y;
};


```

大小为 16

也就是，只要有虚函数，无论多少个，都会增加 8 的大小。

8 就是 64 位，显然我的电脑是 64 位系统（

也就是，这个 8 应该是个指针。

实际上，A 的内存模型为

![](https://pic2.zhimg.com/v2-9fafa60a5aa32ff2092242a4a6909e0d_b.jpg)

开头 8 的空间放了一个指针。

我们就直接放出内存模型

![](https://pic1.zhimg.com/v2-42d989b759f13ec41c771b0ceb50346c_r.jpg)

我们来一步步的解析啊。

```
typedef long long u64;
typedef void(*func)();
A a;
u64* p = (u64*)&a;


```

![](https://pic3.zhimg.com/v2-d68e9f51070a0877a7eae219a4031c3e_r.jpg)

然后我们再

```
u64* arr = (u64*)*p;


```

![](https://pic1.zhimg.com/v2-645f66a9447ab6bf5550aef7059f1644_r.jpg)

我们用函数指针接着

```
func fa = (func)arr[0];
func fb = (func)arr[1];
func fc = (func)arr[2];
fa(); fb(); fc();


```

此时我们就指向了虚函数

```
class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
    int x, y;
};

int main(){
    typedef long long u64;
    typedef void(*func)();
    
    A a;
    u64* p = (u64*)&a;
    u64* arr = (u64*)*p;
   
    func fa = (func)arr[0];
    func fb = (func)arr[1];
    func fc = (func)arr[2];
    fa(); fb(); fc();
    return 0;
}


```

输出

> A a()  
> A b()  
> A c()

对于 A 的实例化，那个指针都是指向同一块

![](https://pic1.zhimg.com/v2-88ba4fd720230946c33e52d7a6a41574_r.jpg)

```
A a1, a2;
u64* p = (u64*)&a1;
cout << *p << endl;
p = (u64*)&a2;
cout << *p << endl;


```

输出

> 140695023172728  
> 140695023172728

现在我们来个 A 的派生

```
class B :public A {
public:
    virtual void b() { cout << "B b()" << endl; }
};


```

按照上面的代码跑一下

```
B b;
u64* p = (u64*)&b;
u64* arr = (u64*)*p;
func fa = (func)arr[0];
func fb = (func)arr[1];
func fc = (func)arr[2];
fa(); fb(); fc();


```

输出

> A a()  
> B b()  
> A c()

我们来对比下二者的虚函数的指向

```
A a;
u64* pa = (u64*)&a;
u64* arra = (u64*)*pa;
B b;
u64* pb = (u64*)&b;
u64* arrb = (u64*)*pb;
for (int i = 0; i < 3; i++) {
    cout << hex << arra[i] << " " << arrb[i] << endl;
}


```

输出

> 7ff6889a159b 7ff6889a159b  
> 7ff6889a1596 7ff6889a15c3  
> 7ff6889a155f 7ff6889a155f

也就是说，内存模型是这样的

![](https://pic4.zhimg.com/v2-19cf253e039ffe7e5d2d47109c4330d3_r.jpg)

这个时候我们看下任何虚函数教程都有的

```
A *a = new B;


```

我们来对比下指向的那个数组

```
A* a1 = new A;
A* a2 = new A;
A* a3 = new B;
A* b = new B;
cout << hex << *(u64*)a1 << endl;
cout << hex << *(u64*)a2 << endl;
cout << hex << *(u64*)a3 << endl;
cout << hex << *(u64*)b << endl;


```

输出

> 7ff626e6bc78  
> 7ff626e6bc78  
> 7ff626e6bc18  
> 7ff626e6bc18

内存模型为

![](https://pic4.zhimg.com/v2-f98d23cb79f8907aaa7dd8cd778bde3f_r.jpg)

如果我们的 B，多放些数据

```
class B :public A {
public:
    int z;
    virtual void b() { cout << "B b()" << endl; }
};


```

那内存模型为

![](https://pic4.zhimg.com/v2-80a9df919bea217726fec02706e81c27_r.jpg)

那么我们可以整一个究极花活

我们先定义个 C

```
class C {
public:
    virtual void d() { cout << "C d()" << endl; }
    virtual void e() { cout << "C e()" << endl; }
    virtual void f() { cout << "C f()" << endl; }
};


```

长成这个样子

![](https://pic4.zhimg.com/v2-5c26c0901a21252b6b56ded3f9b83a27_r.jpg)

那么我们移花接木一下 A

![](https://pic3.zhimg.com/v2-3967b544045bd99fbba6d5553180aafe_r.jpg)

代码

```cc
#include <iostream>
using namespace std;

class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
    int x, y;
};

class B :public A {
public:
    int z;
    virtual void b() { cout << "B b()" << endl; }
};

class C {
public:
    virtual void d() { cout << "C d()" << endl; }
    virtual void e() { cout << "C e()" << endl; }
    virtual void f() { cout << "C f()" << endl; }
};

int main(){
    typedef long long u64;
    typedef void(*func)();
    int i = 1;
    A* a = new A;
    C* c = new C;
    *(u64*)a = *(u64*)c;
    a->a(); a->b(); a->c();
    return 0;
}
```

输出

> C d()  
> C e()  
> C f()

因为编译器只知道，函数 a() 去找 arr[0],b() 去找 arr[1],c() 去找 arr[2]。

但是到底 arr 变成了什么呢，就由不得编译器了

好了，相信看到这里，大家应该都知道虚函数在哪里了吧。

剩下的一些分配策略什么的，去看看别人的就可以了。

如果你觉得自己懂了的话，可以尝试用 C 语言模拟一遍。

[严格鸽：C 语言实现虚函数 / 继承 / 封装](https://zhuanlan.zhihu.com/p/566782733)

经人提醒，实际上数组前面还有一块

![](https://pic1.zhimg.com/v2-987aa3c593751c05e7985322fe30534c_r.jpg)

不过太细节的地方大家还是自己去看吧。

评论区有人提了个问题

如果我们 B 中有个新的虚函数，然后我们 $\rm A *a = new B$\rm A *a = new B 是否可以访问到

![](https://pic3.zhimg.com/v2-bbe559273d191f395878afbe1dcf88ee_r.jpg)

```
class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
    int x = 3, y = 5;
};
class B :public A {
public:
    virtual void d() { cout << "B d()" << endl; }
};
int main(){   
    typedef unsigned long long u64;
    typedef  void(*func)();
    A* a = new B;
    u64* arr = (u64*)*(u64*)a;
    func f = (func)arr[3];
    f();
    return 0;
}


```

输出

> B d()

实际上就是

![](https://pic2.zhimg.com/v2-38ff8f3e77fd62ceeca247af4880bbe5_r.jpg)

当然是可以的

但是吧， 不要继续深究这个了，越来越 UB 了。

评论区又提问了

> b 多放些数据那里 a3 是不是也应该有 z 呢

答案是可以的

```
class B :public A {
public:
    int z;
    B(int _x, int _y, int _z) { x = _x, y = _y, z = _z; }
    virtual void d() { cout << "B d()" << endl; }
};


```

这个时候我们

```
 A* a = new B(1,3,5);


```

实际上这个 a 是指向了

![](https://pic4.zhimg.com/v2-d32874c89b7a411878f42a02102bac1f_r.jpg)

而 z 的位置，处于 y 的下面

所以我们写出这样的代码

```
class A {
public:
    virtual void a() { cout << "A a()" << endl; }
    virtual void b() { cout << "A b()" << endl; }
    virtual void c() { cout << "A c()" << endl; }
    int x = 3, y = 5;
};
class B :public A {
public:
    int z;
    B(int _x, int _y, int _z) { x = _x, y = _y, z = _z; }
    virtual void d() { cout << "B d()" << endl; }
};
int main(){  
    A* a = new B(1,3,5);
    cout << *(&(a->y) + 1) << endl;
    return 0;
}


```

输出

> 5