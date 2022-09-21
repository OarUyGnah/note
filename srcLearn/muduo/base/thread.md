### 线程安全分析

线程安全分析的工作原理非常类似于多线程程序的类型系统。除了声明数据类型(例如int、float等)之外，程序员还可以(选择性地)声明如何在多线程环境中控制对数据的访问。例如，如果foo是由互斥锁mu保护的，那么当一段代码在没有首先锁定mu的情况下对foo进行读写时，分析就会发出警告。类似地，如果有一些特定的例程应该只由GUI线程调用，那么分析将在其他线程调用这些例程时发出警告。

#### **因此设计了一系列宏定义用于通过代码注解（annotations）告诉编译器哪些成员变量和成员函数受mutex保护，若忘记加锁，编译器给出警告。**

#### **目的：**

**他人后续维护代码时，特别容易遗漏线程安全的假设。此工具能将原作者设计意图以代码注解清楚地写出来并让编译器【自动检查】！**

- `clang-Wthead-safety`编译代码
- `clang -c -Wthread-safety example.cc`

#### 线程安全分析见下链接，只在clang编译下有效

[CSDN Thread Safety Analysis](https://blog.csdn.net/weixin_42157432/article/details/115939656?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166150882316781790741800%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166150882316781790741800&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~baidu_landing_v2~default-1-115939656-null-null.142^v42^pc_ran_alice,185^v2^control&utm_term=SCOPED_CAPABILITY%20&spm=1018.2226.3001.4187)

[ThreadSafetyAnalysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)

## CurrentThread

```cc
namespace detail {
    pid_t gettid()
    {
      return static_cast<pid_t>(::syscall(SYS_gettid));
    }
}

namespace CurrentThread
{
  // internal
  // __thread是GCC内置的线程局部存储设施，存取效率可以和全局变量相比
  // __thread变量每一个线程有一份独立实体，各个线程的值互不干扰
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  void cacheTid() {
      if (t_cachedTid == 0)//为0说明是新线程
      {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
      }
  }

  inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }

  inline const char* tidString() // for logging
  {
    return t_tidString;
  }

  inline int tidStringLength() // for logging
  {
    return t_tidStringLength;
  }

  inline const char* name()
  {
    return t_threadName;
  }

  bool isMainThread() {
      return tid() == ::getpid();
  }

  void sleepUsec(int64_t usec) {
      struct timespec ts = { 0, 0 };//秒+纳秒
      //s,ms,us,ns都是1:1000
      ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);// us/1000000
      ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);//%取秒之后的数，再转化成us
      ::nanosleep(&ts, NULL);
  }

  string stackTrace(bool demangle);
}  // namespace CurrentThread
```

## CountDownLatch(倒计时锁存器)

- CountDownLatch最多调用wait() count次

- 参考Java的一个同步辅助类，在完成一组正在其他线程中执行的操作之前，它允许一个或多个线程一直等待。既可以用于所有子线程等待主线程发起 “起跑”，也可以用于主线程等待子线程初始化完毕才开始工作
-  **` Condition condition_ GUARDED_BY(mutex_);`表示为数据成员condition_ 提供mutex_保护，读共享，写排他（即写前锁）**
- 所有人都要调用同一个CountDownLatch实例（即统一condition、mutex）
- 发号通知-报数的人，不断调用CountDownLatch::countDown( ) 减数，减到0，就通知其他等待的人 
  等待命令的人，调用CountDownLatch::wait()等待
- **注意mutext_是mutable类型，因为 getCount成员函数是const类型，不能修改此实例，但是getCount用到了锁，要修改mutex_，为了可以只修改锁，而不修改其他变量，所以把锁表明mutable类型**

```cc
class CountDownLatch : noncopyable
{
 public:

  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable MutexLock mutex_;
  //
  Condition condition_ GUARDED_BY(mutex_);
  int count_ GUARDED_BY(mutex_);
};

CountDownLatch::CountDownLatch(int count)
  : mutex_(),
    condition_(mutex_),
    count_(count)
{
}

void CountDownLatch::wait()
{
  MutexLockGuard lock(mutex_);
  while (count_ > 0)
  {
    condition_.wait();
  }
}

void CountDownLatch::countDown()
{
  MutexLockGuard lock(mutex_);
  --count_;
  if (count_ == 0)
  {
    condition_.notifyAll();
  }
}

int CountDownLatch::getCount() const
{
  MutexLockGuard lock(mutex_);
  return count_;
}
```

#### 例子

```cc
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>
#include <boost/bind.hpp>
 
#include <vector>
#include <stdio.h>
using namespace muduo;
CountDownLatch latch_(1);
void Function()
{
    latch_.wait();//wait for latch_ countDown
    printf("Thread ID=%d, Name=%s\n", CurrentThread::tid(), CurrentThread::name());
}
int main()
{
    Thread t1(boost::bind(Function), "Thread 1");
    Thread t2(boost::bind(Function), "Thread 2");
    t1.start();
    t2.start();
 
    printf("main thread running, before countDown\n");
    latch_.countDown();
    sleep(3);//wait for thread t1, t2
    printf("main thread running, after countDown\n");
 
    t1.join();
    t2.join();
 
    return 0;
}
/*
运行结果:
main thread running, before countDown
Thread ID=6994, Name=Thread 2
Thread ID=6993, Name=Thread 1
main thread running, after countDown
*/
```



## Condition

```cc
class Condition : noncopyable
{
 public:
  explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
  {
    MCHECK(pthread_cond_init(&pcond_, NULL));
  }

  ~Condition()
  {
    MCHECK(pthread_cond_destroy(&pcond_));
  }

  void wait()
  {
    MutexLock::UnassignGuard ug(mutex_);
    MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
  }

  // returns true if time out, false otherwise.
  bool waitForSeconds(double seconds);

  void notify()
  {
    MCHECK(pthread_cond_signal(&pcond_));
  }

  void notifyAll()
  {
    MCHECK(pthread_cond_broadcast(&pcond_));
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t pcond_;
};

bool muduo::Condition::waitForSeconds(double seconds)
{
  struct timespec abstime;
  // FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
  clock_gettime(CLOCK_REALTIME, &abstime);

  const int64_t kNanoSecondsPerSecond = 1000000000;
  int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

  abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
  abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

  MutexLock::UnassignGuard ug(mutex_);
  return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}
```

## Mutex

#### MutexLock

下面内容与在clang编译下有效，请先阅读**线程安全分析**相关内容，这里只简要介绍涉及的部分

内部包含一个`pthread_mutex_t mutex`

`CAPABILITY("mutex")`是类的一个属性，声明这个类的对象可作为**能力**使用“mutex”指定错误消息中的功能类型

`assertLocked() `的`ASSERT_CAPABILITY(this)`是函数或方法上的属性，它断言调用线程已经拥有给定的能力，即调用该函数时已拥有mutex

`lock()`的` ACQUIRE()`声明函数**获得**能力，但不释放它。该**能力进入前不持有，但在退出后持有**，即mutex lock以后拥有该mutex

`unlock()` 的`RELEASE()`声明函数释放能力，即**能力在进入前持有，退出前释放**，即mutex unlock后失去该mutex



```cc
class CAPABILITY("mutex") MutexLock : noncopyable
{
 public:
  MutexLock()
    : holder_(0)
  {
    MCHECK(pthread_mutex_init(&mutex_, NULL));
  }

  ~MutexLock()
  {
    assert(holder_ == 0);
    MCHECK(pthread_mutex_destroy(&mutex_));
  }

  // must be called when locked, i.e. for assertion
  bool isLockedByThisThread() const
  {
    return holder_ == CurrentThread::tid();
  }

  void assertLocked() const ASSERT_CAPABILITY(this)
  {
    assert(isLockedByThisThread());
  }

  // internal usage

  void lock() ACQUIRE()
  {
    MCHECK(pthread_mutex_lock(&mutex_));
    assignHolder();
  }

  void unlock() RELEASE()
  {
    unassignHolder();
    MCHECK(pthread_mutex_unlock(&mutex_));
  }

  pthread_mutex_t* getPthreadMutex() /* non-const */
  {
    return &mutex_;
  }

 private:
  friend class Condition;

  class UnassignGuard : noncopyable
  {
   public:
    explicit UnassignGuard(MutexLock& owner)
      : owner_(owner)
    {
      owner_.unassignHolder();
    }

    ~UnassignGuard()
    {
      owner_.assignHolder();
    }

   private:
    MutexLock& owner_;
  };

  void unassignHolder()
  {
    holder_ = 0;
  }

  void assignHolder()
  {
    holder_ = CurrentThread::tid();
  }

  pthread_mutex_t mutex_;
  pid_t holder_;
};

```

#### MutexLockGuard

`SCOPED_CAPABILITY`是实现`RAII-style`锁定的类的一个属性，在这些类中，功能在构造函数中获得，在析构函数中释放。

```cc
class SCOPED_CAPABILITY MutexLockGuard : noncopyable
{
 public:
  explicit MutexLockGuard(MutexLock& mutex) ACQUIRE(mutex)
    : mutex_(mutex)
  {
    mutex_.lock();
  }

  ~MutexLockGuard() RELEASE()
  {
    mutex_.unlock();
  }

 private:

  MutexLock& mutex_;
};
```



## Thread

thread内部的start通过pthread_create传入startThread作为子线程工作函数，并传入参数ThreadData指针data  `pthread_create(&pthreadId_, NULL, &detail::startThread, data)`

startThread内部调用参数data的runInThread

```cc
class Thread : noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(ThreadFunc, const string& name = string());
  // FIXME: make it movable in C++11
  ~Thread();

  void start();
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }

  static int numCreated() { return numCreated_.get(); }

 private:
  void setDefaultName();

  bool       started_;
  bool       joined_;
  pthread_t  pthreadId_;
  pid_t      tid_;
  ThreadFunc func_;
  string     name_;
  CountDownLatch latch_;
  // 静态原子int变量，用于记录产生的线程数
  static AtomicInt32 numCreated_;
};
Thread::Thread(ThreadFunc func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
  setDefaultName();
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}

void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();// num = ++numCreated_;
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;
  // FIXME: move(func_)
  detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    started_ = false;
    delete data; // or no delete?
    LOG_SYSFATAL << "Failed in pthread_create";
  }
  else
  {
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}
```

```cc
struct ThreadData
{
  // typedef std::function<void ()> ThreadFunc;
  typedef muduo::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  string name_;
  pid_t* tid_;
  CountDownLatch* latch_;

  ThreadData(ThreadFunc func,
             const string& name,
             pid_t* tid,
             CountDownLatch* latch)
    : func_(std::move(func)),
      name_(name),
      tid_(tid),
      latch_(latch)
  { }

  void runInThread()
  {
    *tid_ = muduo::CurrentThread::tid();
    tid_ = NULL;
    // 无论接下来func_()调用成功与否，都latch_->countDown()表示此线程已完成任务
    latch_->countDown();
    latch_ = NULL;

    muduo::CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
    ::prctl(PR_SET_NAME, muduo::CurrentThread::t_threadName);
    try
    {
      func_();
      muduo::CurrentThread::t_threadName = "finished";
    }
    catch (const Exception& ex)
    {
      muduo::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
      abort();
    }
    catch (const std::exception& ex)
    {
      muduo::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      abort();
    }
    catch (...)
    {
      muduo::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
      throw; // rethrow
    }
  }
};
```



## ThreadPool

内部存放一个任务deque和一个unique_ptr指针指向的thread的vector

start()初始化线程数组

```cc
class ThreadPool : noncopyable
{
 public:
  typedef std::function<void ()> Task;

  explicit ThreadPool(const string& nameArg = string("ThreadPool"));
  ~ThreadPool();

  // Must be called before start().
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
  void setThreadInitCallback(const Task& cb)
  { threadInitCallback_ = cb; }

  void start(int numThreads);
  void stop();

  const string& name() const
  { return name_; }

  size_t queueSize() const;

  // Could block if maxQueueSize > 0
  // Call after stop() will return immediately.
  // There is no move-only version of std::function in C++ as of C++14.
  // So we don't need to overload a const& and an && versions
  // as we do in (Bounded)BlockingQueue.
  // https://stackoverflow.com/a/25408989
  void run(Task f);

 private:
  bool isFull() const REQUIRES(mutex_);
  void runInThread();
  Task take();

  mutable MutexLock mutex_;
  Condition notEmpty_ GUARDED_BY(mutex_);
  Condition notFull_ GUARDED_BY(mutex_);
  string name_;
  Task threadInitCallback_;
  std::vector<std::unique_ptr<muduo::Thread>> threads_;
  std::deque<Task> queue_ GUARDED_BY(mutex_);
  size_t maxQueueSize_;
  bool running_;
};

ThreadPool::ThreadPool(const string& nameArg)
  : mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(nameArg),
    maxQueueSize_(0),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
  if (running_)
  {
    stop();
  }
}

void ThreadPool::start(int numThreads)
{
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i)
  {
    char id[32];
    snprintf(id, sizeof id, "%d", i+1);
    threads_.emplace_back(new muduo::Thread(
          std::bind(&ThreadPool::runInThread, this), name_+id));
    threads_[i]->start();
  }
  if (numThreads == 0 && threadInitCallback_)
  {
    threadInitCallback_();
  }
}

void ThreadPool::stop()
{
  {
  MutexLockGuard lock(mutex_);
  running_ = false;
  notEmpty_.notifyAll();
  notFull_.notifyAll();
  }
  for (auto& thr : threads_)
  {
    thr->join();
  }
}

size_t ThreadPool::queueSize() const
{
  MutexLockGuard lock(mutex_);
  return queue_.size();
}

void ThreadPool::run(Task task)
{
  if (threads_.empty())
  {
    task();
  }
  else
  {
    MutexLockGuard lock(mutex_);
    while (isFull() && running_)
    {
      notFull_.wait();
    }
    if (!running_) return;
    assert(!isFull());

    queue_.push_back(std::move(task));
    notEmpty_.notify();
  }
}

ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mutex_);
  // always use a while-loop, due to spurious wakeup
  while (queue_.empty() && running_)
  {
    notEmpty_.wait();
  }
  Task task;
  if (!queue_.empty())
  {
    task = queue_.front();
    queue_.pop_front();
    if (maxQueueSize_ > 0)
    {
      notFull_.notify();
    }
  }
  return task;
}

bool ThreadPool::isFull() const
{
  mutex_.assertLocked();
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
  try
  {
    if (threadInitCallback_)
    {
      threadInitCallback_();
    }
    while (running_)
    {
      Task task(take());
      if (task)
      {
        task();
      }
    }
  }
  catch (const Exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
    abort();
  }
  catch (const std::exception& ex)
  {
    fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
    fprintf(stderr, "reason: %s\n", ex.what());
    abort();
  }
  catch (...)
  {
    fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
    throw; // rethrow
  }
}
```



## ThreadLocal

```cc
/*
   在多线程程序中，所有线程共享进程中的变量。
   现在有一全局变量，所有线程都可以使用它，改变它的值。
   而如果每个线程希望能单独拥有它，那么就需要使用线程存储了。
   表面上看起来这是一个全局变量，所有线程都可以使用它，而它的值在每一个线程中又是单独存储的。这就是线程存储的意义。
*/
template<typename T>
class ThreadLocal : noncopyable
{
 public:
  ThreadLocal()
  {
    // 初始化并设置清理函数
    // 当每个线程结束时，系统将调用这个函数来释放绑定在这个键上的内存块。
    MCHECK(pthread_key_create(&pkey_, &ThreadLocal::destructor));
  }

  ~ThreadLocal()
  {
    MCHECK(pthread_key_delete(pkey_));
  }

  T& value()
  {
    T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
    if (!perThreadValue)
    {
      T* newObj = new T();
      MCHECK(pthread_setspecific(pkey_, newObj));
      perThreadValue = newObj;
    }
    return *perThreadValue;
  }

 private:
  // 参数x为pthread_setspecific的那块内存，进行delete
  static void destructor(void *x)
  {
    T* obj = static_cast<T*>(x);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;
    delete obj;
  }

 private:
  pthread_key_t pkey_;
};
```



## namespace ProcessInfo

```cc
namespace ProcessInfo
{
  pid_t pid();
  string pidString();
  uid_t uid();
  string username();
  uid_t euid();
  Timestamp startTime();
  int clockTicksPerSecond();
  int pageSize();
  bool isDebugBuild();  // constexpr

  string hostname();
  string procname();
  StringPiece procname(const string& stat);

  /// read /proc/self/status
  string procStatus();

  /// read /proc/self/stat
  string procStat();

  /// read /proc/self/task/tid/stat
  string threadStat();

  /// readlink /proc/self/exe
  string exePath();

  int openedFiles();
  int maxOpenFiles();

  struct CpuTime
  {
    double userSeconds;
    double systemSeconds;

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }

    double total() const { return userSeconds + systemSeconds; }
  };
  CpuTime cpuTime();

  int numThreads();
  std::vector<pid_t> threads();
}  // namespace ProcessInfo

#include "muduo/base/ProcessInfo.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/FileUtil.h"

#include <algorithm>

#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h> // snprintf
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>

namespace muduo
{
namespace detail
{
  // 每个线程一个 当前打开的文件数
__thread int t_numOpenedFiles = 0;

int fdDirFilter(const struct dirent* d)
{
  if (::isdigit(d->d_name[0]))
  {
    ++t_numOpenedFiles;
  }
  // 返回0的过滤，因为只是查看d->d_name无需将其保留，故返回0
  return 0;
}

__thread std::vector<pid_t>* t_pids = NULL;
int taskDirFilter(const struct dirent* d)
{
  // 开头为数字则为进程
  if (::isdigit(d->d_name[0]))
  {
    t_pids->push_back(atoi(d->d_name));
  }
  return 0;
}

int scanDir(const char *dirpath, int (*filter)(const struct dirent *))
{
  struct dirent** namelist = NULL;
  // 通过调用filter，将返回值为非0的每个实体信息存储在namelist中
  int result = ::scandir(dirpath, &namelist, filter, alphasort);
  // 扫描dir目标只是为了判断每个文件的名字，因此无需在namelist存储内容
  assert(namelist == NULL);
  return result;
}
// 进程开始时设置
Timestamp g_startTime = Timestamp::now();
  
// sysconf 函数用来获取系统执行的配置信息。例如页大小、最大页数、cpu个数、打开句柄的最大个数等等。
// assume those won't change during the life time of a process.
// _SC_CLK_TCK：每秒对应的时钟tick数
int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
// _SC_PAGESIZE：一个page的大小，单位byte
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));
}  // namespace detail
}  // namespace muduo

using namespace muduo;
using namespace muduo::detail;

pid_t ProcessInfo::pid()
{
  return ::getpid();
}

string ProcessInfo::pidString()
{
  char buf[32];
  snprintf(buf, sizeof buf, "%d", pid());
  return buf;
}

uid_t ProcessInfo::uid()
{
  return ::getuid();
}

/*
            struct passwd {
                 char   *pw_name;       // username
                 char   *pw_passwd;     // user password
                 uid_t   pw_uid;        // user ID
                 gid_t   pw_gid;        // group ID
                 char   *pw_gecos;      // user information
                 char   *pw_dir;        // home directory
                 char   *pw_shell;      // shell program
             };
       The getpwuid() function returns a pointer to a structure containing the
       broken-out  fields  of the record in the password database that matches
       the user ID uid.
*/
string ProcessInfo::username()
{
  struct passwd pwd;
  struct passwd* result = NULL;
  char buf[8192];
  const char* name = "unknownuser";

  getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
  if (result)
  {
    name = pwd.pw_name;
  }
  return name;
}
// returns the effective user ID of the calling process.
uid_t ProcessInfo::euid()
{
  return ::geteuid();
}

Timestamp ProcessInfo::startTime()
{
  return g_startTime;
}

int ProcessInfo::clockTicksPerSecond()
{
  return g_clockTicks;
}

int ProcessInfo::pageSize()
{
  return g_pageSize;
}

bool ProcessInfo::isDebugBuild()
{
#ifdef NDEBUG
  return false;
#else
  return true;
#endif
}

string ProcessInfo::hostname()
{
  // HOST_NAME_MAX 64
  // _POSIX_HOST_NAME_MAX 255
  char buf[256];
  if (::gethostname(buf, sizeof buf) == 0)
  {
    buf[sizeof(buf)-1] = '\0';
    return buf;
  }
  else
  {
    return "unknownhost";
  }
}

string ProcessInfo::procname()
{
  return procname(procStat()).as_string();
}

StringPiece ProcessInfo::procname(const string& stat)
{
  StringPiece name;
  size_t lp = stat.find('(');
  size_t rp = stat.rfind(')');
  if (lp != string::npos && rp != string::npos && lp < rp)
  {
    name.set(stat.data()+lp+1, static_cast<int>(rp-lp-1));
  }
  return name;
}

string ProcessInfo::procStatus()
{
  string result;
  FileUtil::readFile("/proc/self/status", 65536, &result);
  return result;
}

string ProcessInfo::procStat()
{
  string result;
  FileUtil::readFile("/proc/self/stat", 65536, &result);
  return result;
}

string ProcessInfo::threadStat()
{
  char buf[64];
  snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", CurrentThread::tid());
  string result;
  FileUtil::readFile(buf, 65536, &result);
  return result;
}

string ProcessInfo::exePath()
{
  string result;
  char buf[1024];
  // https://blog.csdn.net/JK198310/article/details/41596775
  // readlink()会将参数path的符号链接内容存储到参数buf所指的内存空间
  ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
  if (n > 0)
  {
    result.assign(buf, n);
  }
  return result;
}

int ProcessInfo::openedFiles()
{
  t_numOpenedFiles = 0;
  scanDir("/proc/self/fd", fdDirFilter);
  return t_numOpenedFiles;
}

int ProcessInfo::maxOpenFiles()
{
  struct rlimit rl;
  if (::getrlimit(RLIMIT_NOFILE, &rl))
  {
    return openedFiles();
  }
  else
  {
    return static_cast<int>(rl.rlim_cur);
  }
}

/*
      times()  stores  the  current  process times in the struct tms that buf
      points to.  The struct tms is as defined in <sys/times.h>:

           struct tms {
               clock_t tms_utime;  // user time 
               clock_t tms_stime;  // system time
               clock_t tms_cutime; // user time of children
               clock_t tms_cstime; // system time of children
           };
*/
ProcessInfo::CpuTime ProcessInfo::cpuTime()
{
  ProcessInfo::CpuTime t;
  struct tms tms;
  if (::times(&tms) >= 0)
  {
    const double hz = static_cast<double>(clockTicksPerSecond());
    t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
    t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
  }
  return t;
}

int ProcessInfo::numThreads()
{
  int result = 0;
  string status = procStatus();
  // Threads: 1
  size_t pos = status.find("Threads:");
  if (pos != string::npos)
  {
    result = ::atoi(status.c_str() + pos + 8); // or +9 ?
  }
  return result;
}

std::vector<pid_t> ProcessInfo::threads()
{
  std::vector<pid_t> result;
  t_pids = &result;
  scanDir("/proc/self/task", taskDirFilter);
  t_pids = NULL;
  std::sort(result.begin(), result.end());
  return result;
}

```

