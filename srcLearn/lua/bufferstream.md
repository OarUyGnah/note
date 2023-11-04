## buffer stream

#### lzio.h

```C
struct Zio {
  size_t n;          // 未读长度
  const char *p;     // 当前位置
  lua_Reader reader; // reader函数指针 typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
  void *data;        // 额外数据
  lua_State *L;      // reader的Lua state
};

#define zgetc(z) (((z)->n--) > 0 ? cast_uchar(*(z)->p++) : luaZ_fill(z))
// 获取当前z的p位置char*指针，如果没有则luaZ_fill
#define luaZ_initbuffer(L, buff) ((buff)->buffer = NULL, (buff)->buffsize = 0)

#define luaZ_buffer(buff) ((buff)->buffer)
#define luaZ_sizebuffer(buff) ((buff)->buffsize)
#define luaZ_bufflen(buff) ((buff)->n)

#define luaZ_buffremove(buff, i) ((buff)->n -= (i))
#define luaZ_resetbuffer(buff) ((buff)->n = 0)

#define luaZ_resizebuffer(L, buff, size)                                       \
  ((buff)->buffer =                                                            \
       luaM_reallocvchar(L, (buff)->buffer, (buff)->buffsize, size),           \
   (buff)->buffsize = size)

#define luaZ_freebuffer(L, buff) luaZ_resizebuffer(L, buff, 0)

LUAI_FUNC int luaZ_fill(ZIO *z);
LUAI_FUNC void luaZ_init(lua_State *L, ZIO *z, lua_Reader reader, void *data);
LUAI_FUNC size_t luaZ_read(ZIO *z, void *b, size_t n); // read n bytes
```

#### lzio.c

```c
int luaZ_fill (ZIO *z) {
  size_t size;
  lua_State *L = z->L;
  const char *buff;
  lua_unlock(L);
  buff = z->reader(L, z->data, &size);
  lua_lock(L);
  if (buff == NULL || size == 0)
    return EOZ;
  z->n = size - 1;  /* discount char being returned */
  z->p = buff;
  return cast_uchar(*(z->p++));
}

void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
}

/* ----read n bytes from z->p to b --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (z->n == 0) {  // 没有可读字节则fill
      if (luaZ_fill(z) == EOZ)  // 尝试调用reader func填充，如果没有则返回
        return n;
      else {
        z->n++;
        z->p--;
      }
    }
    m = (n <= z->n) ? n : z->n;  // min(n,z->n)
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}
```

