#include <iostream> // std::cout
#include <chrono>   // std::chrono::seconds, std::chrono::milliseconds
                    // std::chrono::duration_cast
#include <ctime>
#include <signal.h>
using namespace std;

class betimestr
{
public:
    betimestr() : tp(chrono::system_clock::now()), ttnow(chrono::system_clock::to_time_t(tp))
    {
        ctime_s(timestr, 50, &ttnow); //打印time字符串
        cout << "begin ctor time is " << timestr << endl;
    }

    ~betimestr()
    {
        tp = chrono::system_clock::now();
        ttnow = chrono::system_clock::to_time_t(tp);
        ctime_s(timestr, 50, &ttnow);
        cout << "end dtor time is " << timestr << endl;
    }

private:
    chrono::system_clock::time_point tp;
    char timestr[50];
    time_t ttnow;
};
bool b = true;
void test()
{
    betimestr t;

    int i = 0;
    while (b)
    {
        printf("%d ", i++);
        if (!i % 100)
            printf("\n");
    }
}
void test2()
{
    chrono::duration<int, std::ratio<60 * 60 * 24>> day(1);
    chrono::time_point<chrono::system_clock, chrono::system_clock::duration> timenow = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(timenow);
    char str[50];
    cout << ctime(&tt) << endl;
    timenow += day; // time_point.operator+=(const duration&)
    tt = chrono::system_clock::to_time_t(timenow);
    cout << ctime(&tt) << endl;
}

void test3()
{
    chrono::milliseconds foo(1000); // 1 second，1000微秒 duration
    foo *= 60;
    std::cout << "duration (in periods): ";
    std::cout << foo.count() << " milliseconds.\n";
    std::cout << "duration (in seconds): ";
    std::cout << foo.count() * chrono::milliseconds::period::num / chrono::milliseconds::period::den;
    // den表示分母，num表示分子，ratio的单位是秒
    //所以milliseconds::period::num=1000,milliseconds::period::den = 1;
    //所以num/den表示千分之一
    //同理hours::period::num = 1;hours::period::den = 3600;最好用作比例
    std::cout << " seconds.\n";
}
void test4(){
    chrono::system_clock::time_point tp = chrono::system_clock::now();
    time_t tt=chrono::system_clock::to_time_t(tp);
    cout<<ctime(&tt)<<"\n";
}
int main(int argc, char const *argv[])
{
    signal(SIGINT, [](int sig)
           { cout << "sig num is "
                  << sig << "\n";b=false; });
    // test();
    test2();
    test3();
    test4();
}
