#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <locale.h>

int ShowLove(char* szFirstSentence, ...)
{

   //用来统计可变参数数量
   int num = 0;

   //第一步：
   //申明一个va_list类型对象ap，用于对参数进行遍历
   va_list ap;
   
   //第二步：
   //使用va_start对变量进行初始化
   //这里需要注意的是:
   //在传统C语言中，va_start把ap中内部指针设置为传递给函数参数的【第一个实参】；
   //而在标准的C中，va_start接受一个额外参数，也就是最后一个【固定参数】的名称，
   //并把ap中的内部指针设置为传递给函数的第一个【可变参数】.
   //所以你在VC++ 6.0和VS2008等高版本的编译器中使用va_start需要注意区别
   //这里使用标准C
   va_start(ap, szFirstSentence);


   //第三步：
   //使用va_arg宏返回实参列表中的下一个参数值，并把ap的内部指针推向下一个参数（如果有的话）
   //必须指定下一个参数的类型。
   //在调用va_start之后第一次调用va_arg将返回第一个可变参数的值
   printf(szFirstSentence);


   char* p = 0;
   while(p = va_arg(ap, char*))
   {
       printf("%s", p);
       num ++;
   }

   //第四步：
   //待所有可变参数都读取完毕以后，调用va_end宏对ap和va_list做必要的清理工作
   va_end(ap);
   return num;
}

int main(int argc, char* argv[])
{
   setlocale(LC_ALL, "");

   int z = ShowLove("123！\n");
   int y = ShowLove("123！", "321？\n");
   int l = ShowLove("123！", "321？", "456！\n");  
   printf("\n\n\n"); 
   printf("可变参数个数依次是：%d\t%d\t%d\n", z, y, l);
   return 0;
}