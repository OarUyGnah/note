### strpbrk

返回accept中任一字符第一次出现在s的位置

```c
#include <string.h>
char *strpbrk(const char *s, const char *accept);
```

### strspn	strcspn

strspn：返回s的**初始段**长度，该初始段中任意字符都在accept中

strcspn：返回s的**初始段**长度，该初始段中任意字符都不在reject中

```c
#include <string.h>
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);

char * str = "   1231241";
//跳过str开头的空格或\t
*str+strspn(str," \t") = '1';
```

### strchr	strrchr

strchr：返回s中第一次出现c的位置

strrchr：返回s中最后一次出现c的位置

```c
#include <string.h>
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
```

### strcasecmp	strncasecmp

忽略大小比较s1、s2

```c
#include <strings.h>
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
```

