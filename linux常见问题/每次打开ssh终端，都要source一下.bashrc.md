> ### 为什么每次打开ssh终端，都要source一下.bashrc?

为了弄明白这个问题，首先，我们需要搞明白Linux启动时读取配置文件的顺序。
 在刚登录Linux时，首先启动`/etc/profile` 文件，然后再启动用户目录下的 `~/.bash_profile`、`~/.bash_login`或 `~/.profile`文件中的其中一个，
 **执行的顺序**为：`~/.bash_profile`、`~/.bash_login`、 `~/.profile`。

如果 `~/.bash_profile`文件存在的话，一般还会执行 `~/.bashrc`文件。
 因为在`~/.bash_profile`文件中一般会有下面的代码：



```bash
if [ -f ~/.bashrc ] ; then
        source .bashrc
fi 
```

`~/.bashrc`中，一般还会有以下代码：



```bash
if [ -f /etc/bashrc ] ; then
        source .bashrc
fi 
```

所以，`~/.bashrc`会调用`/etc/bashrc`文件。最后，在退出shell时，还会执行 `~/.bash_logout`文件。

总结一下，**这些配置文件的执行顺序为**：`/etc/profile`→ `(~/.bash_profile | ~/.bash_login | ~/.profile)`→`~/.bashrc` →`/etc/bashrc` → `~/.bash_logout`

**接下来，我们就来解释一下为什么每次打开Xshell终端，都要source一下.bashrc：**

假如在`~/.bash_profile`文件中没有下面的代码：



```bash
if [ -f ~/.bashrc ] ; then
        source .bashrc
fi 
```

那么linux就不会自动执行~/.bashrc文件，所以你每次登陆的时候都要手动输入`source ~/.bashrc`。

> ### 解决方法

接下来，为了让你看得更明白，我重新说一下如何解决每次打开Xshell终端，都要source一下.bashrc这个问题：

`vim ~/.bash_profile`在文件内部输入



```bash
   # 加载.bashrc文件
if test -f .bashrc ; then
source .bashrc 
fi
```

然后按ESC键，输入`:wq`保存并退出即可。

> ### 补充说明：关于各个配置文件的作用域
>
> ------
>
> 

#### /etc/profile：

此文件为系统的每个用户设置环境信息,当用户第一次登录时,该文件被执行. 并从/etc/profile.d目录的配置文件中搜集shell的设置。

#### /etc/bashrc:

为每一个运行bash shell的用户执行此文件.当bash shell被打开时,该文件被读取（即每次新开一个终端，都会执行bashrc）。

#### ~/.bash_profile:

每个用户都可使用该文件输入专用于自己使用的shell信息,当用户登录时,该文件仅仅执行一次。默认情况下,设置一些环境变量,执行用户的.bashrc文件。

#### ~/.bashrc:

该文件包含专用于你的bash shell的bash信息,当登录时以及每次打开新的shell时,该该文件被读取。

#### ~/.bash_logout:

当每次退出系统(退出bash shell)时,执行该文件. 另外,/etc/profile中设定的变量(全局)的可以作用于任何用户,而~/.bashrc等中设定的变量(局部)只能继承 /etc/profile中的变量,他们是"父子"关系。

###### ~/.bash_profile: 是交互式、login 方式进入 bash 运行的    ~/.bashrc 是交互式 non-login 方式进入 bash 运行的通常二者设置大致相同，所以通常前者会调用后者





### 注：zsh终端不执行`.bashrc`和`.bash_profile`,故要把对应的内容写到`.zshrc`

例如将下列放入`.zshrc`,则启动zsh终端时自动加载`.bash_profile`文件

```bash
# 自动加载.bash_profile文件
if [ -f ~/.bash_profile ] ; then
    source ~/.bash_profile
fi
```

