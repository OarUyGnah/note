## dynamic_cast

```cc
dynamic_cast<type*>(e)
dynamic_cast<type&>(e)
dynamic_cast<type&&>(e)
```

type必须为类类型

e的类型必须满足以下其中一个条件

- e的类型是目标type的公有派生类
- e的类型是目标type的公有基类
- e的类型就是目标type的类型

如果**失败则抛出一个`std::bad_cast`异常**

可以对空指针执行dynamic_cast，结果是所需类型的空指针



### 引用类型的dynamic_cast

不存在空引用，因此当类型转换失败时，程序抛出一个名为`std::bad_cast`异常

```cc
try {
	const Derived& d = dynamic_cast<const Derived&>(b);
}
catch(std::bad_cast) {
	
}
```



```cc
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <typeinfo>
using namespace std;
 
class A
{
public:
	A()
	{
		cout<<"A()"<<endl;
	}
	virtual ~A()
	{
		cout<<"~A()"<<endl;
	}
};
 
class B:public virtual A
{
public:
	B()
	{
		cout<<"B()"<<endl;
	}
	virtual ~B()
	{
		cout<<"~B()"<<endl;
	}
};
class C:public B
{
public:
	C()
	{
		cout<<"C()"<<endl;
	}
	virtual ~C()
	{
		cout<<"~C()"<<endl;
	}
};
class D:public B,public virtual A
{
public:
	D()
	{
		cout<<"D()"<<endl;
	}
	virtual ~D()
	{
		cout<<"~D()"<<endl;
	}
};
int main(int argc,char** argv)
{
	A *pa = new C;
	if (B *pb = dynamic_cast<B*>(pa))
	{
		cout<<"True"<<endl;
	}
	else
	{
		cout<<"False"<<endl;
	}//因为指针类型的转换失败返回为0可以使用条件中赋值判断
 
	try
	{
		C &cp = dynamic_cast<C&>(*pa);//正确，*pa的类型是C  
		cout << "cp" << endl;  
	}  
	catch (std::bad_cast e) 
	{  
		cout << e.what() << endl;  
	}//引用类型失败返回的是bad_cast  
 
	B *pbb = new B;
	if (C *pc = dynamic_cast<C*>(pbb))
	{
		cout<<"True"<<endl;
	}
	else
	{
		if(A *paa=dynamic_cast<A*>(pbb)){cout<<"paa"<<endl;}
		cout<<"False"<<endl;
	}
 	
	A *paa = new D;//错误，必须要虚继承才可以
	if (B *pc = dynamic_cast<B*>(paa))
	{
		cout<<"True"<<endl;
	}
	else
	{
		cout<<"False"<<endl;
	}
 
	cin.get();
	return 0;
}

```

## typeid运算符

- typeid作用于数组或函数时，并不会执行向指针的标准类型转换，如果对数组执行typeid，所得结果为数组类型而非指针类型
- 运算对象不属于类类型或者不包含虚函数的类时，typeid指示的是运算对象的静态类型。而运算对象是定义了虚函数的类的左值时，typeid结果直到运行时才会求得
- typeid作用于指针时（而非指针所指的对象），返回结果是该指针的静态编译时类型

### type_info类

没有默认构造，拷贝和移动构造以及赋值运算符都为delete，因此获得type_info对象的途径只有typeid