```mysql
-- p55
CREATE TABLE studnt(
  id INT,
  NAME VARCHAR(20),
  gender VARCHAR(20),
  chinese INT,
  english INT,
  math INT
);
RENAME TABLE studnt TO student;
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (1,'张明','男',89,78,90);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (2,'李进','男',67,53,95);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (3,'王五','女',87,78,77);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (4,'李一','女',88,98,92);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (5,'李财','男',82,84,67);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (6,'张宝','男',55,85,45);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (7,'黄蓉','女',75,65,30);
INSERT INTO studnt (id, NAME, gender, chinese, english, math) VALUES (7,'黄蓉','女',75,65,30);



-- 查询表中所有学生信息
SELECT * FROM student;
-- 查询表中所有学生姓名和对应英语成绩
SELECT NAME,english FROM student;
-- 过滤表中重复数据
SELECT DISTINCT * FROM student;
-- 统计每个学生总分
SELECT NAME,(chinese+math+english) AS total_num FROM student;
-- 所有学生加十分
SELECT NAME,(chinese+math+english+10) AS total_num FROM student;
-- 用别名表示学生分数
SELECT NAME,chinese '语文成绩',math '数学成绩',english '英语成绩' FROM student;
-- 查询英语成绩大于90分
SELECT * FROM student WHERE english > 90;
-- 查询总成绩大于200分的
SELECT *,(chinese+math+english) total_num FROM student WHERE (chinese+math+english)>200;
-- 查询英语成绩在80-90之间的
SELECT * FROM student WHERE english>=80 AND english <=90;
SELECT * FROM student WHERE english BETWEEN 80 AND 90;
-- 查询英语成绩不在80-90的
SELECT * FROM student WHERE NOT (english BETWEEN 80 AND 90);
SELECT * FROM student WHERE english NOT BETWEEN 80 AND 90;
SELECT * FROM student WHERE english <80 || english > 90;
-- 查询数学成绩为89 90 91 的
SELECT * FROM student WHERE math IN (89,90,91);
-- 查询数学成绩不为89 90 91 的
SELECT * FROM student WHERE math NOT IN (89,90,91);
SELECT * FROM student WHERE NOT math IN (89,90,91);
-- 查询所有姓李的英语成绩
SELECT NAME,english FROM student WHERE NAME LIKE '李%';
-- 查询数学80或总分200的
SELECT * FROM student WHERE math>80 OR (english+chinese+math)>200;
-- 数学成绩降序
SELECT NAME,math FROM student ORDER BY math DESC;
-- 总分排序后输出，从高到低输出
SELECT * FROM student ORDER BY (english+chinese+math) DESC;
-- 对姓李的总成绩排名降序
SELECT * FROM student WHERE NAME LIKE '李%' ORDER BY (english+chinese+math) DESC ;
-- 查询男女分别多少人，并将人数降序排序输出
SELECT 
  gender,COUNT(*) cnt 
FROM 
  student  
GROUP BY 
  gender 
HAVING 
  cnt >=4 
ORDER BY 
  cnt 
DESC;
```



```mysql
-- p56
USE mydb2;
CREATE TABLE emp(
	empno INT, -- 员工编号
	ename VARCHAR(20), -- 员工名字
	job VARCHAR(20), -- 工作名字
	mgr INT,	-- 上级领导编号
	hiredate DATE, -- 入职日期
	sal INT,   -- 薪资
	comm INT,  -- 奖金
	deptno INT -- 部门编号
);

INSERT INTO emp VALUES(7369,'SMITH','CLERK',7902,'1980-12-17',800,NULL,20),
(7499,'ALEN','SALESMAN',7698,'1981-02-20',1600,300,30),
(7521,'WARD','SALESMAN',7698,'1981-02-22',1250,500,30),
(7566,'JONES','MANAGER',7839,'1981-04-02',2975,NULL,20),
(7654,'MARTIN','SALESMAN',7698,'1981-09-28',1250,1400,30),
(7698,'BLAKE','MANAGER',7839,'1981-05-01',2850,NULL,30),
(7782,'CLARK','MANAGER',7839,'1981-06-09',2450,NULL,10),
(7788,'SCOTT','ANALYST',7566,'1987-04-19',3000,NULL,20),
(7839,'KING','PRESIDENT',NULL,'1981-11-17',5000,NULL,10),
(7844,'TURNER','SALESMAN',7698,'1981-09-08',1500,0,30),
(7876,'ADAMS','CLERK',7788,'1987-05-23',1100,NULL,20),
(7900,'JAMES','CLERK',7698,'1981-12-03',950,NULL,30),
(7902,'FORD','ANALYST',7566,'1981-12-03',3000,NULL,20),
(7934,'MILLER','CLERK',7782,'1982-01-23',1300,NULL,10);

TRUNCATE emp;
SELECT * FROM emp;


-- 按员工编号升序排列不在10号部门工作的员工信息
SELECT * FROM emp WHERE deptno != 10 ORDER BY empno ASC;
-- 查询姓名第二个字母不是A且薪资大于1000的员工信息，按年薪降序排列
-- ifnull(sal,0) 如果sal为null，则当作0，不为null则还是原来的值
SELECT *,(12*sal+IFNULL(comm,0)) yearsal FROM emp WHERE ename NOT LIKE '_A%' && sal > 1000 ORDER BY (12*sal+IFNULL(comm,0)) DESC;

-- 求每个部门平均薪水
SELECT deptno,AVG(sal) AS avg_sal FROM emp GROUP BY deptno ORDER BY avg_sal DESC;

-- 求各个部门最高薪水
SELECT deptno,MAX(sal) AS max_sal FROM emp GROUP BY deptno ORDER BY max_sal DESC;

-- 求每个部门每个岗位最高薪水
SELECT deptno,job,MAX(sal) FROM emp GROUP BY deptno,job; 

-- 求平均薪水大于2000的部门编号
SELECT deptno,AVG(sal) AS avg_sal FROM emp GROUP BY deptno HAVING avg_sal >=2000;

-- 将部门平均薪水大于1500的部门编号列出，按平均薪水降序排除
SELECT deptno,AVG(sal) AS avg_sal FROM emp GROUP BY deptno HAVING avg_sal>=1500 ORDER BY avg_sal DESC; 

-- 选择公司中有奖金的员工姓名 工资
SELECT ename,sal FROM emp WHERE comm IS NOT NULL; 

-- 查询员工最高和最低薪资的差距
SELECT MAX(sal)-MIN(sal) FROM emp;
```





```mysql
DROP DATABASE test1;
CREATE DATABASE test1;
USE test1;
CREATE TABLE dept(
	deptno INT PRIMARY KEY,
	dname VARCHAR(14),
	loc VARCHAR(13)
);

INSERT INTO dept VALUES(10,'accounting','new york'),
			(20,'research','dallas'),
			(30,'sales','chicago'),
			(40,'operations','boston');
			
CREATE TABLE emp(
	empno INT PRIMARY KEY COMMENT '员工编号',
    ename VARCHAR(10) COMMENT '员工名',
    job VARCHAR(9) COMMENT '员工工作',
    mgr INT COMMENT '员工直属领导编号',
    hiredate DATE COMMENT '入职时间',
    sal DOUBLE COMMENT '工资',
    comm DOUBLE COMMENT '奖金',
    deptno INT COMMENT '对应dept表的外键'
);
-- 添加部门和员工之间的外键关系
ALTER TABLE emp ADD CONSTRAINT FOREIGN KEY emp(deptno) REFERENCES dept (deptno);

CREATE TABLE salgrade(
	grand INT COMMENT '等级',
    losal DOUBLE COMMENT '最低工资',
    hisal DOUBLE COMMENT '最高工资'
);
INSERT INTO salgrade VALUES (1,700,1200),(2,1201,1400),(3,1401,2000),(4,2001,3000),(5,3001,9999);

SELECT * FROM salgrade;

INSERT INTO emp VALUES
(7369,'smith','clerk',7902,'1980-12-17',800,NULL,20),
(7499,'allen','salesman',7698,'1981-02-20',1600,300,30),
(7521,'ward','salesman',7698,'1981-02-22',1250,500,30),
(7566,'JONES','MANAGER',7839,'1981-04-02',2975,NULL,20),
(7654,'MARTIN','SALESMAN',7698,'1981-09-28',1250,1400,30),
(7698,'BLAKE','MANAGER',7839,'1981-05-01',2850,NULL,30),
(7782,'CLARK','MANAGER',7839,'1981-06-09',2450,NULL,10),
(7788,'SCOTT','ANALYST',7566,'1987-04-19',3000,NULL,20),
(7839,'KING','PRESIDENT',NULL,'1981-11-17',5000,NULL,10),
(7844,'TURNER','SALESMAN',7698,'1981-09-08',1500,0,30),
(7876,'ADAMS','CLERK',7788,'1987-05-23',1100,NULL,20),
(7900,'JAMES','CLERK',7698,'1981-12-03',950,NULL,30),
(7902,'FORD','ANALYST',7566,'1981-12-03',3000,NULL,20),
(7934,'MILLER','CLERK',7782,'1982-01-23',1300,NULL,10);
SELECT * FROM emp;

-- 1 返回拥有员工的部门名、部门号
SELECT  DISTINCT d.dname,d.deptno FROM dept d JOIN emp e ON d.deptno = e.deptno;

-- 2 工资水平多于smith的员工信息
SELECT * FROM emp WHERE sal > (SELECT sal FROM emp WHERE ename='smith');

-- 3返回员工和所属经理的姓名
SELECT e1.`ename`, e2.`ename` FROM emp e1 JOIN emp e2 ON e1.mgr = e2.empno;
SELECT e1.ename,e2.ename FROM emp e1, emp e2 WHERE e1.mgr = e2.empno; 

-- 4 返回雇员的雇佣日期早于其经理雇佣日期的员工及其经理姓名
SELECT e1.ename,e1.hiredate,e2.ename,e2.hiredate FROM emp e1 JOIN emp e2 
ON e1.hiredate < e2.hiredate AND e1.mgr = e2.empno; 

-- 5 返回员工姓名及其所在部门名称
SELECT emp.ename,dept.dname FROM emp,dept WHERE dept.deptno = emp.deptno;
SELECT emp.ename,dept.dname FROM emp JOIN dept ON dept.deptno = emp.deptno;

-- 6 返回从事clerk工作的员工姓名和所在部门名称
SELECT e.ename,d.dname FROM emp e JOIN dept d ON e.job='clerk' AND e.deptno = d.deptno;

-- 7 返回部门号及其本部门最低工资
SELECT deptno,MIN(sal) FROM emp GROUP BY emp.deptno;

-- 8 返回销售部(sales)所有员工姓名
SELECT ename FROM emp INNER JOIN dept ON dept.dname='sales' AND emp.deptno = dept.deptno;

-- 9 返回工资水平多于平均工资的员工
SELECT * FROM emp WHERE sal > (SELECT AVG(sal) FROM emp);

-- 10 返回与scott从事相同工作的员工
SELECT * FROM emp WHERE emp.job = (SELECT job FROM emp WHERE ename = 'scott') AND ename NOT IN('scott'); 

-- 11 返回工资高于30部门所有员工的员工姓名与工资
SELECT ename,sal FROM emp WHERE sal > ALL(SELECT sal FROM emp WHERE deptno = 30);

-- 12 返回员工工作及其从事此工作的最低工资
SELECT job,MIN(sal) FROM emp GROUP BY job;

-- 13 计算员工年薪，并排序
SELECT ename,(sal*12+IFNULL(comm,0)) AS total FROM emp  ORDER BY  total DESC;
SELECT * FROM salgrade;
SELECT * FROM dept;
-- 14 返回工资处于第四级的员工和姓名
SELECT ename,sal FROM emp WHERE sal BETWEEN (SELECT losal FROM salgrade WHERE grand = 4) AND (SELECT hisal FROM salgrade WHERE grand = 4);

SELECT * FROM emp JOIN salgrade ON sal >= (SELECT losal FROM salgrade WHERE grand = 4) AND sal <= (SELECT hisal FROM salgrade WHERE grand = 4) AND grand = 4;
SELECT * FROM emp;
-- 15 返回工资为二等级的职员名字、部门所在地
SELECT * 
FROM emp e
JOIN dept d ON d.`deptno` = e.`deptno`
JOIN salgrade sg ON sg.`grand` = 2 AND e.`sal` BETWEEN sg.`losal` AND sg.`hisal`;
```





```mysql
-- p104
-- 1 查询部门平均薪水最高的部门名称
-- 1.1创建根据deptno分组的平均薪水作为视图test_view1
CREATE VIEW test1_view1
AS
(SELECT 
        deptno,
        AVG(sal) avg_sal 
      FROM
        emp 
      GROUP BY deptno);
SELECT * FROM test1_view1;
-- 1.2根据test_view1创建根据平均sal排序的test_view2;
CREATE VIEW test1_view2
AS
(SELECT 
      *,
      rank () over (ORDER BY avg_sal DESC) rn 
    FROM
       test_view1);
SELECT * FROM test1_view2;
-- 1.3根据test_view2创建平均薪水排序第一的test_view3
CREATE VIEW test1_view3
AS
(SELECT 
    * 
  FROM
    test_view2 tt 
  WHERE rn = 1);
SELECT * FROM test1_view3;
-- 1.4根据test_view3查询平均薪水最高的部门名称
SELECT 
  d.deptno,
  d.dname,
  d.loc,
  avg_sal 
FROM
  dept d,
  test1_view3 ttt 
WHERE d.deptno = ttt.deptno;


  SELECT * FROM emp;
-- 2 查询员工比所属领导薪资高的部门名、员工名、员工领导编号
-- 2.1 查询员工比领导工资高的部门号
CREATE OR REPLACE VIEW test2_view1
AS
SELECT 
  a.ename ename,
  a.sal esal,
  b.ename mname,
  b.sal msal,
  a.deptno
FROM 
  emp a,
  emp b
WHERE
  a.mgr = b.empno
  AND a.sal > b.sal;
-- 2.2 和部门表联合查询
SELECT * FROM dept d JOIN test2_view1 t ON d.deptno = t.deptno;

-- 3 查询工资等级为4 2000年以后入职的工作地点为上海的员工编号、姓名、工资，并查询薪资在前三的员工信息
CREATE OR REPLACE VIEW test3_view1
AS
SELECT d.dname,d.loc,e.*,sg.grade
FROM 
dept d 
JOIN emp e 
JOIN salgrade sg 
ON d.deptno = e.deptno 
   AND sg.grade = 4 
   AND e.sal BETWEEN sg.losal 
   AND sg.hisal 
   AND YEAR(hiredate) > '2000' 
   AND d.loc = '上海';
   
SELECT * FROM test3_view1;

SELECT *,
rank() over(ORDER BY sal DESC) rn
FROM test3_view1;
```



```mysql
-- p123
-- 每天对应一个表，产生下个月所有的表
CREATE DATABASE dbuser;
USE dbuser;

DELIMITER $$
CREATE PROCEDURE create_month_table()
BEGIN
  DECLARE next_year INT;
  DECLARE next_month INT;
  DECLARE next_month_day INT;
  DECLARE next_month_str VARCHAR(2);
  DECLARE next_month_day_str VARCHAR(2);
  
  DECLARE table_name_str VARCHAR(20);
  DECLARE t_index INT DEFAULT 1;
  
  SET next_year = YEAR(DATE_ADD(NOW(),INTERVAL 1 MONTH));
  SET next_month = MONTH(DATE_ADD(NOW(),INTERVAL 1 MONTH));
  -- next_month_day下个月有多少天 
  -- DATE_ADD(CURDATE()-DAY(CURDATE())+1,INTERVAL 2 MONTH) 两个月后的第一天
  SET next_month_day = DATEDIFF(DATE_ADD(CURDATE()-DAY(CURDATE())+1,INTERVAL 2 MONTH),DATE_ADD(CURDATE()-DAY(CURDATE())+1,INTERVAL 1 MONTH));

  IF next_month > 10
    THEN SET next_month_str = CONCAT('0',next_month);
  ELSE
    SET next_month_str = CONCAT('',next_month);
  END IF;
  
  WHILE t_index <= next_month_day DO
    IF t_index < 10
      THEN SET next_month_day_str = CONCAT(0,t_index);
    ELSE
      SET next_month_day_str = CONCAT('',t_index);
    END IF;
    
    SET table_name_str = CONCAT(next_year,'_',next_month_str,'_',next_month_day_str);
    
    SET @create_table_sql = CONCAT(
	'create table if not exists user_',
	table_name_str,
	'(`uid` int,`ename` varchar(20),`information` varchar(20)) collate=\'utf8_general_ci\'
	engine=INNODB'
    );
    -- 
    PREPARE create_table_stmt FROM @create_table_sql;
    EXECUTE create_table_stmt;
    DEALLOCATE PREPARE create_table_stmt;
    SET t_index = t_index + 1;
    
  END WHILE;
END $$
DELIMITER;

CALL create_month_table();
```

