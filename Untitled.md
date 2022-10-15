

### 重要

muduo 框架：	base()

​							  net()

读源码

框架：每天至少写150行

算法：代码随想录

开题

论文

- [x] 六级	9.12 9:00打印


### 正常

http权威指南

- [x] c++ primer


cmake

CSAPP

stl

golang

cmu15-445 https://15445.courses.cs.cmu.edu/fall2022/schedule.html **Moody-老师**

深入理解计算机系统 **yaaangmin** (2/85)

单元测试

数据库原理

MIT6.824

### 次要

协程

boost

docker

redis



```sqlite
with res(case_id,name,disposition) as (
	select case_id,attorneys.name,charges.disposition
    from charges join attorneys using(case_id)
    where attorneys.name is not ''
), res2(name,cnt) as (
	select name,count(case_id) cnt
    from res
    where disposition = 'Not Guilty'
    group by name
)
select name,count(case_id) num,cnt * 100.0 / count(case_id) per
from res natural join res2
group by name
having num > 100
order by per desc
limit 6,1;

```

