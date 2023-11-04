### 哈希表

```c
typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode;  /* log2 of size of 'node' array */
  unsigned int alimit;  /* "limit" of 'array' array */
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  struct Table *metatable;
  GCObject *gclist;
} Table;


typedef union Node {
  struct NodeKey {
    TValuefields;  /* fields for value */
    lu_byte key_tt;  /* key type  (unsigned char) */
    int next;  /* for chaining */
    Value key_val;  /* key value */
  } u;
  TValue i_val;  /* direct access to node's value as a proper 'TValue' */
} Node;

typedef union Value {
  struct GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  lua_CFunction f; /* light C functions typedef int (*lua_CFunction) (lua_State *L); */  
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
  /* not used, but may avoid warnings for uninitialized value */
  lu_byte ub;
} Value;

typedef struct GCObject {
  struct GCObject *next; lu_byte tt; lu_byte marked;
} GCObject;
```

