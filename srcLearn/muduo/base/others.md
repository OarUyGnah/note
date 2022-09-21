### nocopyable.h copyable.h

基类，用于被其他类继承，表名继承的类是否可以被拷贝

```cc
class noncopyable
{
 public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

class copyable
{
 protected:
  copyable() = default;
  ~copyable() = default;
};
```

### Atomic.h

class AtomicIntegerT 模板类，使用了一系列原子操作函数，封装了原子级别的操作

原子操作的C API见[C/C++ linux无锁化编程](https://blog.csdn.net/LU_ZHAO/article/details/104331164?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166133920916781685367613%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166133920916781685367613&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-3-104331164-null-null.142^v42^pc_ran_alice,185^v2^control&utm_term=__sync_lock_test_and_set&spm=1018.2226.3001.4187)

```cc
#include "muduo/base/noncopyable.h"

#include <stdint.h>

namespace muduo
{

namespace detail
{
template<typename T>
class AtomicIntegerT : noncopyable
{
 public:
  AtomicIntegerT()
    : value_(0)
  {
  }

  // uncomment if you need copying and assignment
  //
  // AtomicIntegerT(const AtomicIntegerT& that)
  //   : value_(that.get())
  // {}
  //
  // AtomicIntegerT& operator=(const AtomicIntegerT& that)
  // {
  //   getAndSet(that.get());
  //   return *this;
  // }

  T get()
  {
    // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
    // __sync_val_compare_and_swap(&val,old,new);
    //写入new之前,读出old,当且仅当old与val中的当前值一致时,才把new写入存储
    //成功写入返回1 失败0
    return __sync_val_compare_and_swap(&value_, 0, 0);
  }

  T getAndAdd(T x)
  {
    // in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
    // value_ += x 并返回原来的值
    return __sync_fetch_and_add(&value_, x);
  }

  T addAndGet(T x)
  {
    return getAndAdd(x) + x;
  }

  T incrementAndGet()
  {
    return addAndGet(1);
  }

  T decrementAndGet()
  {
    return addAndGet(-1);
  }

  void add(T x)
  {
    getAndAdd(x);
  }

  void increment()
  {
    incrementAndGet();
  }

  void decrement()
  {
    decrementAndGet();
  }

  T getAndSet(T newValue)
  {
    // in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
    // 将value_设为newValue并返回value_操作之前的值
    return __sync_lock_test_and_set(&value_, newValue);
  }

 private:
  volatile T value_;
};
}  // namespace detail

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;

}  // namespace muduo
```



### Exception.h

what()返回const char* 字符串

stackTrace()返回CurrentThread::stackTrace(false)

```cc
/*====================Exception.h====================*/
#ifndef MUDUO_BASE_EXCEPTION_H
#define MUDUO_BASE_EXCEPTION_H

#include "muduo/base/Types.h"
#include <exception>

namespace muduo
{

class Exception : public std::exception
{
 public:
  Exception(string what);
  ~Exception() noexcept override = default;

  // default copy-ctor and operator= are okay.

  const char* what() const noexcept override
  {
    return message_.c_str();
  }

  const char* stackTrace() const noexcept
  {
    return stack_.c_str();
  }

 private:
  string message_;
  string stack_;
};

}  // namespace muduo

#endif  // MUDUO_BASE_EXCEPTION_H
/*====================Exception.h====================*/

/*====================Exception.cc====================*/
#include "muduo/base/Exception.h"
#include "muduo/base/CurrentThread.h"

namespace muduo
{

Exception::Exception(string msg)
  : message_(std::move(msg)),
    stack_(CurrentThread::stackTrace(/*demangle=*/false))
{
}

}  // namespace muduo
/*====================Exception.cc====================*/
```

### Types.h

```cc
#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#include <string.h>  // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

///
/// The most common stuffs.
///
namespace muduo
{

using std::string;

inline void memZero(void* p, size_t n)
{
  memset(p, 0, n);
}
  
// 使用implicit_cast 作为static_cast 或const_cast 的安全版本
// 用于在类型层次结构中向上转换
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

// 下行转换，使用高效的static_cast,先用dynamic_cast确定能否下行转换,能的话再用static_cast
template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

  // assert依赖一个NDEBUG的宏，如果NDEBUG已定义，assert啥也不干，如果没有定义，assert才会发挥作用
  // dynamic_cast失败返回NULL
#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}

}  // namespace muduo

#endif  // MUDUO_BASE_TYPES_H

```

