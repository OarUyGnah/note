## inserter	插入迭代器

```cc
#include <iterator>
list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
list<int> lst2 ={10}, lst3={10},lst4={10};
//back_inserter在尾部插入
copy(lst.cbegin(), lst.cend(), back_inserter(lst2));
//lst2包含10,1,2,3,4,5,6,7,8,9

//inserter第二个参数必须是一个给定容器的迭代器，元素将被插入到给定迭代器之前，一次全部插入
copy(lst.cbegin(), lst.cend(), inserter(lst3, lst3.begin()));
//lst3包含1,2,3,4,5,6,7,8,9,10

//front_inserter 元素总是插在第一个元素以前，一个一个插入
copy(lst.cbegin(), lst.cend(), front_inserter(lst4));
//lst4包含9,8,7,6,5,4,3,2,1,10
```
