IniFile 将指定ini文件读取，每个变量放入map<string,map<string,value>>中



#### SocketHandler 管理socket

   错误则关闭对应socket，读事件则产生WorkTask，传入socket指针，WorkTask内部进行信息recv，任务首先接受消息头部，确认成功后再继续recv数据流，将函数名、参数读取，再将其读入数据流中

单例模式保证server是同一个，每个任务内部通过单例模式创建一个server指针，call调用指定函数，并将返回数据放入datastream out中，再send给client



#### 序列化

datastream 将常用内置类型 vector string等常用容器进行了write read方法重载

​	自定义类型则需要将class继承serializable类，通过重写虚函数 来实现读写



serializable 给自定义类型 class进行序列化，需要继承该类，并在内部使用宏将需要序列化的成员变量写入

#### TaskDispatcher

TaskDispatcher负责将任务分发给线程池，内部list负责暂存，条件变量等待

#### WorkerThread

线程池中的线程



#### FunctionHandler

std::map<string, std::function<void(DataStream &, DataStream &)>> m_handlers;



->bind 调用std::bind 绑定func函数，并使用占位符_1 _2留给datasream in out

```cc
template <typename Func>
void FunctionHandler::wrap(Func func, DataStream & in, DataStream & out)
{
    wrap_impl(func, in, out);
}

template <typename R, typename ...Args>
void FunctionHandler::wrap_impl(R(*func)(Args...), DataStream & in, DataStream & out)
{
    wrap_impl(std::function<R(Args...)>(func), in, out);
}
```

std::function<R(Args...)>(func)可以自动获得参数列表



将所有函数都wrap为一个std::function<void(DataStream &, DataStream &)，将返回值都放入out中，如果函数返回值是void，将选择其特化版本



using args_type = std::tuple<typename std::decay\<Args>::type...>;将得到参数的tuple

datasream  in向tuple中写入参数值



->call 从map中找到，并输入参数in out，out中获得结果