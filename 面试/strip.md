```c
#include <iostream>
#include <string.h>

char* strip(char* s) {
    char* p = s;
    char* q = s + strlen(s) - 1;
    while(*p == ' ' || *p == '\n' || *p == '\t') {
        p++;
    }
    while(*q == ' ' || *q == '\n' || *q == '\t') {
        q--;
    }
    *(q+1) = '\0';
    return p;
}
int main()
{
    char buf[] = " test string \n\t";
    std::cout << "[" << strip(buf) << "]"<< std::endl;
   
   return 0;
}
```

