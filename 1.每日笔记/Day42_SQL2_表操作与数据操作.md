## Day42(Week8_4.13)——SQL2_表操作与数据操作

### 表操作

#### 1.常见的数据类型

1. **整数**

   | MySQL的整数类型 | 占用字节 |                有符号                | 无符号                 | 说明       |
   | :-------------- | -------- | :----------------------------------: | ---------------------- | ---------- |
   | TINYINT(M)      | 1        |             -128  ~  127             | 0  ~  255              | 很小的整数 |
   | INT/INTEGER(M)  | 4        | -2<sup>31</sup>  ~  2<sup>31</sup>-1 | 0  ~  2<sup>32</sup>-1 | 普通整数   |
   | BIGINT(M)       | 8        | -2<sup>63</sup>  ~  2<sup>63</sup>-1 | 0  ~  2<sup>64</sup>-1 | 大整数     |

   > 第一原则:无论给整数设置什么长度都不违背上述'有/无符号'表示的存储范围(上述范围表示是一切基本标准)。
   >
   > 第二原则: 设置长度之后, 如果存储的整数长度小于指定长度, 会默认在数字位前自动补空格, 以满足指定长度(但是补的空格这是不可见的), 也可以选择经过设置使用0填充;  设置长度之后, 如果存储的整数长度大于指定长度, 如果在表示范围内, 不做切割/不做处理(不补0, 也不补空格), 直接存储。

2. **浮点数**

   | MySQL的浮点数 | 占用字节 | 说明   |
   | ------------- | -------- | ------ |
   | FLOAT(M, D)   | 4        | 单精度 |
   | DOUBLE(M, D)  | 8        | 双精度 |

   > * M: 精度, 表示总数据位数。 取值范围为(1～255)。 
   >
   > * D: 标度, 表示小数位的位数。 取值范围为(1～30，且不能大于 M-2)。 
   >
   > * 需要注意的是: int类型设置宽度只是要不要填充空格和0的问题, 在float double中这个设置参数超过总位数会报错, 超过小数位限制舍弃
   >
   > * FLOAT 和 DOUBLE 在不指定精度时，默认会按照实际的精度(由计算机硬件和操作系统决定)。
   >
   >   ```sql
   >   # FLOAT 类型的取值范围如下：
   >   有符号的取值范围：-3.402823466E+38～-1.175494351E-38。
   >   无符号的取值范围：0 和 -1.175494351E-38～-3.402823466E+38。
   >         
   >   # DOUBLE 类型的取值范围如下：
   >   有符号的取值范围：-1.7976931348623157E+308～-2.2250738585072014E-308。
   >   无符号的取值范围：0 和 -2.2250738585072014E-308～-1.7976931348623157E+308。
   >   ```

3. **日期**

   | MySQL日期                               | 字节 | 日期格式            | 表示范围                                  |
   | --------------------------------------- | ---- | ------------------- | ----------------------------------------- |
   | YEAR                                    | 1    | YYYY                | 1901 ~ 2155                               |
   | TIME                                    | 3    | HH:MM:SS            | -838:59:59 ~ 838:59:59                    |
   | DATE                                    | 3    | YYYY-MM-DD          | 1000-01-01 ~ 9999-12-31                   |
   | <span style="color:red">DATETIME</span> | 8    | YYYY-MM-DD HH:MM:SS | 1000-01-01 00:00:00 ~ 9999-12-31 23:59:59 |
   | TIMESTAMP                               | 4    | YYYY-MM-DD HH:MM:SS | 1970-01-01 00:00:01 ~ 2038-01-19 03:14:07 |

   > * year/YEAR范围:   1901~ 2155包含255个年份, 对应一个字节表示范围。
   >
   > * time/TIME范围:   11111111111111111111111(23位剩余一位符号位)--> 8388607(十进制)
   >
   > ​		8388607:  838(小时位) xx(描述分钟) xx(描述秒)。 (计算方式不用记)
   >
   > * timestamp/TIMESTAMP范围:  (时间戳) (一般用于一些计时, 倒计时, 数据传输 ......) (秒数1)
   >
   > ​		1970年1月1日作为UNIX TIME的纪元时间(开始时间)。
   >
   > ```sql
   > create table user(
   > 	id int,
   >   name varchar(5),
   >   height float(5, 2),
   >   birthday datetime,
   >   tag_time timestamp
   > )；
   > insert into user4 values(1, 'zs', 188.01, '2000-11-12 55:30:12', '2000-01-01 00:00:00');
   > insert into user4 values(1, 'zs', 188.01, '2000-11-12 10:30:12', now());
   > ```

4. **字符串**

   | MySQL字符串 | 内存占用                        | 说明             |
   | ----------- | ------------------------------- | ---------------- |
   | CHAR(M)     | (M * 单个字符占用字节)          | 固定长度字符串   |
   | VARCHAR(M)  | L+1字节 or L+2字节 。           | 变长字符串       |
   | TEXT(M)     | L+2字节 。  L: 0~2<sup>16</sup> | 变长文本字符串   |
   | LONGTEXT(M) | L+4字节 。  L: 0~2<sup>32</sup> | 变长大文本字符串 |

   > L:  存储字符的实际长度。
   >
   > M:  列的指定长度。
   >
   > - CHAR (M不设置默认为1) 范围可以设置最大255 。
   > - VARCHAR(M不设置会报错) 最大长度为 65,535字节
   > - TEXT 最大长度为 65535字符
   > - LONGTEXT 最大长度长度为 4294967295 字符。
   >
   > (1)数据库中的char可以正常存储字符串: "zs", "我爱学习" ....; (但是一般我们用到的char的时候, 更多是为了存储类似性别这种具有固定长度和格式的短字符串)
   >
   > (2)创建表的时候, 如果使用char类型, 不加宽度/长度, 长度默认为1;   如果使用varchar不加长度/宽度, 创建的时候会报错, text/longtext同样建议指明长度(有些版本中不指明会报错)
   >
   > (3)在数据库中, 只要是字符串, 都有长度限制, 在字符串列存储数据的时候, 不能超出既定的长度限制 (字符串的长度限制是一个严格限定).
   >
   > (4)如果使用char类型(<span style=color:red;font-size:20px>定长字符串</span>), 它是一个固定长度字符串: 如果我们常见char的时候指明的宽度为m, 每一个这个char列的数据, 最终开辟的内存空间为: 单个字符占用空间 * M;  和实际存储了几个字符无关
   >
   > (5)我们使用varchar这种<span style=color:red;font-size:20px>变长字符串</span>存储数据的的时候, 字符串实际占用的内存空间和我们给varchar设置的宽度/长度没有任何关系, 只和这个varchar列实际存储的字符个数有关.

#### 2.查看表

```sql
SHOW TABLES;			 # 查看该数据库中所有表
SHOW CREATE TABLE <表名>;	# 查看表的创建语句
DESCRIBE <表名>;			# 查看表结构
DESC <表名>;				# 查看表结构
```

#### 3.创建表

```sql
CREATE TABLE <表名>  (<列名1> <类型1> ,[…] ,<列名n> <类型n>)   [表选项] [分区选项];

create table employee(
	id int ,
  name varchar(20),
  gender char,
  birthday date,
  job varchar(20),
  salary double(10,2)
)character set utf8 collate utf8_bin;
# 不推荐专门给一个表设置编码格式(没什么用, 只会让一个数据库编码格式不统一)(建议不设编码格式: 表的编码格式遵从数据库设置的编码格式)

#仅仅是个完善的写法示例, 一些写法完全没有必要(比如重复设置字符编码格式), 比如注释
CREATE TABLE  employee1  (
id int(11) NOT NULL AUTO_INCREMENT COMMENT 'id',
name varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '姓名',
gender varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '男' COMMENT '性别',
graduate_year year NULL DEFAULT NULL COMMENT '毕业时间',
birthday date NOT NULL COMMENT '生日',
job varchar(255) CHARACTER SET utf8 COLLATE utf8_bin NULL DEFAULT NULL COMMENT '职位名称',
salary double(10, 2) NULL DEFAULT 0.00 COMMENT '薪资',
create_time datetime NULL DEFAULT NULL COMMENT '信息创建时间',
PRIMARY KEY (id) USING BTREE
) ;
```

#### 4.主键和自增问题

##### **1.主键**

<span style=color:red;font-size:20px>**主键（PRIMARY KEY）又被称为主键约束，是 MySQL中使用非常频繁的约束。它有诸多作用(唯一标识, 查找效率...)。**</span>

==**主键的特征**==

> 主键: 如果一个表中给某一列设置了主键, 那么这一列强制不能重复, 如果存储重复的主键会自动报错
> 主键: 不允许是null, 存储null主键, 报错
> 每个表只能定义一个主键。
> 主键值必须唯一标识表中的每一行，且不能为 NULL，即表中不可能存在有相同主键值的两行数据。
>
> 如果用户没有定义主键，也没有定义索引，那么InnoDB引擎会在创建表的时候, 自动生成一个不可见的ROW_ID的列名的聚簇索引，该列是一个6字节的自增数值，随着插入而自增, 来起到主键的作用。

==**主键分类：主键分为单字段主键和多字段联合主键**==

```sql
# 单字段主键: 既将表中的一个字段设置主键; 通过 PRIMARY KEY 关键字来指定主键.
CREATE TABLE table_primary1(
id INT(11) PRIMARY KEY ,# 主键
name VARCHAR(25),
job VARCHAR(25),,
salary FLOAT
);

CREATE TABLE table_primary2(
id INT(11),
name VARCHAR(25),
job VARCHAR(25),
salary FLOAT,
PRIMARY KEY(id)
);

# 联合主键: 复合主键(不建议)。这意味着id和name的组合在table_primary3表中表示主键。
CREATE TABLE table_primary3(
   id INT(11),
   name VARCHAR(25),
   job VARCHAR(25),
   salary FLOAT,
   PRIMARY KEY(id, name)
);
```

==**增加一个主键与删除主键**==

```sql
# 如果一个表表在创建的时候没有主键, 增加主键的sql
# alter table 表名 add primary key (主键列);
alter table user add primary key (id);

# 删除主键约束
ALTER TABLE <表名> DROP PRIMARY KEY;
# 示例
ALTER TABLE user DROP PRIMARY KEY; 
```

==**注意事项**==

```sql
# 如果主键是自增列（AUTO_INCREMENT），要先去掉自增，再删主键
ALTER TABLE 表名 MODIFY 字段名 INT;  -- 去掉 AUTO_INCREMENT
ALTER TABLE 表名 DROP PRIMARY KEY;

# 一张表只能有一个主键，所以不用指定字段名

# Oracle 里主键一般有名字，删除用
ALTER TABLE 表名 DROP CONSTRAINT 主键约束名;
```

##### **2.唯一键**

<span style=color:red;font-size:20px>**唯一键(Unique Key)是数据库中的一个约束，用于确保表中的所有记录在指定的一列或列组合上是唯一的。即:组合唯一**。</span>

```sql
#与主键不同的是，唯一键允许有空值(NULL)。
#表中可以有多个唯一键约束。
#在某些情况下，唯一键也可以作为主键的一部分。
```

**==添加唯一键与删除唯一键==**

```sql
# 创建表时添加唯一键
CREATE TABLE table_primary3(
   id INT(11),
   name VARCHAR(25),
   job VARCHAR(25),
   salary FLOAT,
   primary KEY(id),
   unique key (job, salary)
);

# 已存在表添加唯一键
ALTER TABLE <表名> ADD UNIQUE(字段名);
ALTER TABLE <表名> ADD UNIQUE(字段1, 字段2);

# 指定唯一键名称（推荐）
ALTER TABLE <表名> ADD CONSTRAINT uk_名称 UNIQUE(字段名);
------------------------------------------------------
# 先查看唯一键索引名
SHOW INDEX FROM <表名>; # 一般默认就是列名
# 找到 Key_name 那一列。
# 删除唯一键
ALTER TABLE <表名> DROP INDEX <唯一键索引名-一般为列名>;
# 示例
ALTER TABLE user DROP INDEX uk_username;

简单记忆
添加：ADD UNIQUE(字段)
删除：DROP INDEX 索引名
```

##### **3.自增AUTO_INCREMENT**

```sql
CREATE TABLE table_primary1(
id INT(11) PRIMARY KEY AUTO_INCREMENT , -- 主键
name VARCHAR(25),
job VARCHAR(25),,
salary FLOAT
);
# AUTO_INCREMENT 的初始值是 1，数据增加一条，该字段值自动加 1。
# 建议: 一个表中应该只有一个字段使用 AUTO_INCREMENT 约束，且该字段一般位为作为索引/主键的id字段。
# AUTO_INCREMENT 字段应该要设置 NOT NULL 属性。
# AUTO_INCREMENT 约束的字段只能是整数类型。
# AUTO_INCREMENT 上限为所约束的类型的数值上限。

# 如果一个表表在创建的时候某些列没有自增, 设置自增
# alter table 表名 modify 列名 int auto_increment;
alter table user modify id int auto_increment;

# 如果主键是自增列（AUTO_INCREMENT），要先去掉自增，再删主键
ALTER TABLE 表名 MODIFY 字段名 INT;  -- 去掉 AUTO_INCREMENT
ALTER TABLE 表名 DROP PRIMARY KEY;
```

#### 5.==修改表==

表的修改操作：不允许在工作直接修改表

```sql
ALTER TABLE <表名> ADD COLUMN <列名> <类型>; # 添加列
ALTER TABLE <表名> ADD <新字段名> <数据类型> FIRST; # 头位置添加列
ALTER TABLE <表名> ADD <新字段名> <数据类型> AFTER <已经存在的字段名>; # 指定位置添加列

ALTER TABLE <表名> MODIFY COLUMN <列名> <类型>;  # 修改某列类型
ALTER TABLE <表名> CHANGE COLUMN <旧列名> <新列名> <新列类型>; # 修改列及类型（column可以省略）
ALTER TABLE <表名> ALTER COLUMN <列名> SET DEFAULT <默认值>; # 修改某列默认值

ALTER TABLE <表名> ALTER COLUMN <列名> DROP DEFAULT; # 删除某列默认值
ALTER TABLE <表名> DROP COLUMN <列名>; # 删除某列

ALTER TABLE <表名> RENAME TO <新表名>; # 修改表名
ALTER TABLE <表名> RENAME AS <新表名>; # 修改表名
ALTER TABLE <表名> RENAME <新表名>; # 修改表名
RENAME TABLE <表名> TO <新表名>; # 修改表名

ALTER TABLE <表名> CHARACTER SET <字符集名>; # 修改表字符集
ALTER TABLE <表名> COLLATE <校对规则名>; # 修改表排序规则
ALTER TABLE <表名> [DEFAULT] CHARACTER SET <字符集名> [DEFAULT] COLLATE <校对规则名>;

#例如
alter table employee  add column height  float(5,2);
alter table employee  add column height  float(5,2) first;
alter table employee  add column height  float(5,2) after name;
alter table employee modify  column age float(5, 0);
alter table employee  change column age age1 float(5, 0);
alter table employee alter column age set default 20;
alter table employee alter column age drop default;
alter table employee drop column height;
alter table employee rename to aaa;
alter table aaa rename as employee;
alter table employee rename  aaa;
rename table aaa to employee;
alter table employee character set utf8mb4;
alter table employee collate utf8mb4_unicode_ci;
alter table employee character set gbk collate gbk_bin;
```

#### 6.删除表

不允许在工作直接删除表，更不建议: 任何数据都是有意义的

```sql
DROP TABLE [IF EXISTS] 表名1 [ ,表名2, 表名3 ...]

drop table if exists table_a, table_b, table_c;
```

### 数据操作

#### 1.添加数据

```sql
INSERT INTO <表名> [ (<列名1>, … <列名n> )]  VALUES (值1, … 值n), … (值1, … 值n);
INSERT INTO <表名> SET <列名1>=<值1>, … <列名n>=<值n>;


insert into employee1 (id, name, gender, graduate_year, birthday, job, salary, create_time) values(1, 'zs', '男', 2022, '1999-01-01', '程序员',  100.2, '2022-09-09 16:51:49');

insert into employee1 (id, name, gender, graduate_year, birthday, job, salary, create_time) values(1, 'zs', '男', 2022, '1999-01-01', '程序员',  100.2, '2022-09-09 16:51:49'),(2, 'ls', '男', 2020, '1997-01-01', '程序员',  10000.2, '2022-09-09 16:51:50');

insert into employee1  set id=4, name='ls', gender='男', graduate_year=2022, birthday='1999-01-01', job='程序员', salary=220.05,  create_time='2022-09-09 16:55:49';
```

> * 如果values中包含数据和表列数据一一对应(无省略),  那么在插入语句中可以省略表名之后表列的一一列举。eg: insert   into  <表名>  values (值1, … 值n);
> * values中的内容应该要与对应插入字段对应。
> * 数据中字符串和日期应该包含在引号中。

#### 2.查询数据

```sql
SELECT  *  FROM <表名字>  [ WHERE  <条件> ];
SELECT  <列名1>, …<列名n>  FROM <表名字>  [ WHERE  <条件> ];

select * from employee1;
select * from employee1 where id<20;
select name from employee1 where id>1;
select name, job, salary from employee1 where salary> 200;
```

> * 查询的结果是一个新的临时表。
> * 在MySQL中 select * from 表名 where 1; 表示查询所有数据。

#### 3.修改数据

```sql
UPDATE <表名> SET 列1=值1 [, 列2=值2 … ] [WHERE <条件> ]

update employee1 set job='老程序员' where salary >10000;
```

> * 注意如果没有where子句指明条件, 那么修改就是对所有行的修改。
> * 修改一行数据的多个列值时，SET 子句的每个值用逗号分开即可。

#### 4.删除数据

```sql
# 在工作中删除数据, 一般是逻辑删除
DELETE FROM <表名> [WHERE <条件>]

delete from employee;
delete from employee where id=4;
```

> * 如果没有where以及条件, 默认删除是表中所有数据。
> * delete不能单独只删除某一列数据, delete删除数据的最小单元为行。
> * delete语句仅删除数据记录, 删除的不是表, 如果要删除表需要使用drop table语句。

#### 5.特殊关键字

##### 1.**where**

```sql
SELECT <查询内容|列等> FROM  <表名字> WHERE <查询条件|表达式>

select id, name from students where id > 10;
```

<span style=color:red;font-size:20px>**使用 WHERE 关键字并指定查询/删除/修改的条件, 让操作在满足条件的情况下执行数据操作**</span>.

在构建Where的条件的过程中, 我们可能需要了解到一些重要的SQL运算符

###### **1.算术运算符:**

**可以用在条件中, 也可以用在结果集中**

| 运算符 | 作用 |
| ------ | ---- |
| +      | 加   |
| -      | 减   |
| *      | 乘   |
| /      | 除   |
| %      | 取余 |

```sql
select * from students where (chinese + english + math) < 180; 
select * from students where (chinese - math) > 30; 
select *, (chinese*0.5 + english*0.1 + math *0.4) from students; 
select *, (chinese*0.5 + english*0.1 + math *0.4)  from students where (chinese*0.5 + english*0.1 + math *0.4) <= 60 ;  
select *, (chinese + english + math) /180 from students ;  
select *, (chinese + english + math) /180 from students  where (chinese + english + math) /180  < 1.2; 
```

###### **2.比较和逻辑运算符**

| 运算符      | 作用       | 运算符      | 作用                      |
| ----------- | ---------- | ----------- | ------------------------- |
| =           | 等于       | <=>         | 等于(可比较null)          |
| !=          | 不等于     | <>          | 不等于(可比较null)        |
| <           | 小于       | >           | 大于                      |
| <=          | 小于等于   | >=          | 大于等于                  |
| is null     | 是否为null | is not null | 是否不为null              |
| between and | 在闭区间内 | in          | 是否在列表内              |
| not in      | 不在列表内 | like        | 通配符匹配(%:通配, _占位) |
| and         | 与         | &&          | 与                        |
| or          | 或         | \|\|        | 或                        |

```sql
select * from students where chinese = 60;
select * from students where chinese <=> 60;
select * from students where chinese != 60;
select * from students where chinese <=> 60;
select * from students where  chinese < 60;
select * from students where  chinese > 90;
select * from students where  chinese <= 60;
select * from students where  chinese >= 90;
select * from students where  chinese is null;
select * from students where  chinese is not null;
select * from students where  chinese between 60 and 90;
select * from students where  chinese in (60 , 90);
select * from students where  chinese not in (60 , 90);
select * from students where  name = '曹操';
select * from students where  name like '曹操';
select * from students where  name like '曹%';
select * from students where  name like '曹%'  and  chinese = 90;
select * from students where  name like '曹%'  &&  chinese = 90;
select * from students where  name like '曹%'  or  chinese = 90;
select * from students where  name like '曹%'  ||  chinese = 90;
```

##### 2.**distinct**

使用DISTINCT对数据表中一个或多个字段重复的数据进行<span style=color:red;font-size:20px>**过滤**</span>，重复的数据只返回其<span style=color:red;font-size:20px>**一条数据**</span>给用户

```sql
SELECT DISTINCT <字段名> FROM <表名>;

select distinct chinese from students;
select distinct chinese, english from students;
```

> (1)DISTINCT 只能在SELECT语句中使用。 (对结果进行去重)
> (2)当对一个或多个字段去重时，DISTINCT 要写在所有字段的最前面。
> (3)如果 DISTINCT 对多个字段去重时，只有多个字段组合起来完全是一样的情况下才会被去重。

##### **3.limit**

使用LIMIT对数据表查询<span style=color:red;font-size:20px>**结果集大小进行限定**</span>

```sql
SELECT <查询内容|列等> FROM  <表名字> LIMIT 记录数目
SELECT <查询内容|列等> FROM  <表名字> LIMIT 初始位置，记录数目;
SELECT <查询内容|列等> FROM  <表名字> LIMIT 记录数目 OFFSET 初始位置;

select * from students limit 3;
select * from students limit 4, 3;
select * from students limit 3 offset 4;
```

> LIMIT 记录数目: 从第一条开始, 限定记录数目
>
> LIMIT 初始位置，记录数目: 从起始位置开始, 限定记录数目
>
> LIMIT 记录数目 OFFSET 初始位置: 从起始位置开始, 限定记录数目
>
> 注意: 数据(默认下标从0开始)

##### **4.as**

AS关键字用来为<span style=color:red;font-size:20px>**指定表和字段的别名**</span>

```sql
<内容> AS <别名>

select  name from students;
select  * from students as s where s.chinese < 60;
select  s.name from students as s ;
select  s.name from students as s where s.chinese < 60;
select  name as username from students;
select  * from students;
select  *, (chinese + english + math) from students;
select  *, (chinese + english + math) as sum from students;
```

##### 5.order by

ORDER BY对查询数据结果集进行<span style=color:red;font-size:20px>**排序**</span>

```sql
SELECT <查询内容|列等> FROM  <表名字> ORDER BY <字段名> [ASC|DESC];

eg: 
select * from students order by chinese;
select * from students order by chinese asc;
select * from students order by chinese desc;
select * from students order by chinese, english;
select * from students order by chinese desc, english desc, math desc;
```

> 不加排序模式: 升序排序
>
> ASC: 升序排序
>
> DESC: 降序排序
>
> 注意:  如上查询, 当我们进行==**多字段排序的时候**==, 会先满足第一个列的排序要求, ==**如果第一列一致的话, 再按照第二列进行排序, 以此类推**==

##### 6.group by

使用 GROUP BY 关键字,对数据进行<span style=color:red;font-size:20px>**分组**</span>

```sql
SELECT <查询内容|列等> FROM  <表名字> GROUP BY  <字段名...>

select class, group_concat(name), group_concat(chinese) from students group by class;
select class, group_concat(name) from students where chinese > 90 group by class;

select class, group_concat(name), avg(chinese) from students group by class;
select class, group_concat(name) from students group by class having count(*) > 3;
select class, group_concat(name), avg(chinese) from students group by class having avg(chinese) > 60;	
```

> GROUP_CONCAT()函数会把每个分组的字段值都显示出来.
>
> HAVING 可以让我们对分组后的各组数据过滤。(一般和分组+聚合函数配合使用)

##### ==**7.聚合函数**==

聚合函数一般用来计算列相关的指定值。**通常**和分组一起使用

==但是**这 6 个全部都可以在不分组（不用 GROUP BY）的前提下单独使用**。==

> 1. group_concat
> 2. count：用于计数；
> 3. sum：求和；
> 4. avg：求平均值；
> 5. max：最大值；
> 6. min：最小值；

1. **group_concat：把同一组里的多行文本，拼接成一行字符串**

   ```sql
   select 院系编号, GROUP_CONCAT(name) from S where age < 20 group by 院系编号 having 院系编号 = 计算机科学系编号;
   ```

2. **count：用于计数**

   ```sql
   SELECT <查询内容|列等> , COUNT <列|*> FROM  <表名字> GROUP BY HAVING COUNT <表达式|条件>
   ```

   <span style=color:red;background:yellow;font-size:20px>**注意：面试常考点，count(1)与count(*)结果与性能基本等价，都是显示表中的所有条数（行数），包含空行；但是count(列名)只会显示非空行，如果是空的就不统计。**</span>

3. **sum：求和**

   ```sql
   SELECT <查询内容|列等> , SUM<列> FROM  <表名字> GROUP BY HAVING SUM<表达式|条件>
   ```

4. **avg：求平均值**

   ```sql
   SELECT <查询内容|列等> , AVG<列> FROM  <表名字> GROUP BY HAVING AVG<表达式|条件>
   ```

5. **max：最大值**

   ```sql
   SELECT <查询内容|列等> , MAX<列> FROM  <表名字> GROUP BY HAVING MAX<表达式|条件>
   ```

6. **min：最小值**

   ```sql
   SELECT <查询内容|列等> , MIN<列> FROM  <表名字> GROUP BY HAVING MIN<表达式|条件>
   ```

###### ==**6.SQL执行顺序**==

**在整个 SQL 执行流程里，真正做 “分组” 这件事的，只有两个：**

1. **GROUP BY** —— 负责**把行按字段分成一组一组**
2. **HAVING** —— 负责**对分好的组进行过滤**
3. **ORDER BY——==可以按 SELECT 里没出现的字段排序==，不是只能按 SELECT 映射的列**

```sql
(5) SELECT column_name, ...
(1) FROM table_name, ...
(2) [WHERE ...]
(3) [GROUP BY ...]    #在group by之后就可以用聚集函数了(select、having后面都可以使用聚集函数)
(4) [HAVING ...]
(6) [ORDER BY ...];
(7) [Limit ...]

(1)(2)在拿到表，处理原始数据-----------------------------> 原始行
(3)(4)(5)就是将原始数据合并得到组的概念  -----------------> 组
(6)就是对分组之后的结果进行排序，但是排序有可能会有多个结果；如果没有分组就只能对原始数据排序；
(7)对排序之后的显示结果进行选择性的显示；

# 计算每个班级中语文成绩大于60的同学的平均数学成绩, 获得平均数学成绩大于等于40的班级中平均语文成绩最高的那个班级的平均语文和数学成绩
select class, group_concat(name), avg(chinese) as chineseA, avg(math) 
from students 
where chinese >= 60 
group by class 
having avg(math)>=40 
order by chineseA desc 
limit 0, 1;
```

> - 小括号中的数字代表执行顺序
>
> - having和select的执行顺序收到优化器的影响，可能会改变执行顺序

##### 8.having

也是过滤，但是是对<span style=color:red;background:yellow;font-size:20px>**分组之后的结果进行过滤**</span>，执行顺序在group by之后

```sql
mysql> select sum(english) from person  group by person_id having sum(english) >= 200;
+--------------+
| sum(english) |
+--------------+
|          278 |
|          200 |
+--------------+
2 rows in set (0.00 sec)

mysql>
```

#### 6.数据完整性

为了让设计的表中的数据是正确的，并且可以更加高效的获取

1. **实体完整性**

   **保证表中的每一行数据都是表中唯一的实体**。(即:一个表中每一条数据都应该是唯一的，就是为了保证表中的每一条数据是唯一的，可以通过主键进行体现）

2. **域完整性**

   **表示保证表中数据的字段的取值在有效范围之内或者符合特定的数据类型约束**

3. **参照完整性**

   **用于确保相关联的表间的数据应该要保持一致**，一般来讲, 参照完整性是通过==外键和主键==来维护的

   ==**作为外键的那一列，本身不需要唯一；**被外键引用的目标字段**才必须唯一**==

   外键的设置和取消: 
   
   ```sql
   # 方式一: 在创建表的时候设置外键
   CREATE TABLE `class`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL
   );
   CREATE TABLE `student`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL,
   `class_id` int ,
   CONSTRAINT `foreign_key_name` FOREIGN KEY (class_id) REFERENCES `class`(`id`)
   );
   
   #方式二: 表创建完毕, 对表设置外键
   CREATE TABLE `class`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL
   );
   CREATE TABLE `student`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL,
   `class_id` int 
   );
   
   ALTER TABLE  `student` ADD CONSTRAINT `foreign_key_name` FOREIGN KEY (`class_id`) REFERENCES  `class` (`id`);
   
   #方式三: 表创建完毕, 添加新的字段列, 并且设置为外键
   CREATE TABLE `class`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL
   );
   CREATE TABLE `student`  (
   `id` int NOT NULL PRIMARY KEY,
   `name` varchar(20) NULL
   );
   alter table `student` add column `class_id` int null after `name`,
   add constraint `foreign_key_name` foreign key (`class_id`) references `class` (`id`);
   
   # 删除外键
   ALTER TABLE `student` DROP FOREIGN KEY `foreign_key_name`;
   ```
