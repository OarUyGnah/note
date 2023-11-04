#### 获取命令行各参数

```cc
int numargvs;
PWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &numargvs);
wprintf(L"%s\n", argv[1]);
```

