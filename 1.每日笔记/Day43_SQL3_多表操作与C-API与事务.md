## Day43_SQL3_多表与C-API与事务

### 多表问题

#### 1.多表设计(多表理论)

1. **一对一**

   指两个表（或多个表之间）的数据存在一一对应的关系

   > 用户和用户详情
   > 商品和商品详情

2. **一对多**

   指两个表（或多个表之间）的数据，存在表A中的一条数据对应表B中的多条数据，表B中的一条数据对应表A中的一条数

   > 用户和订单
   > 班级和学生

3. **多对多**

   存在两个表表A和表B，存在表A中的一条数据对应表B中的多条数据，表B中的一条数据对应表A中的多条数据。

   > 订单和商品
   > 一个商品可能存在于多个订单,  一个订单中可能买了多个商品 
   >
   > 剧本和演员
   > 一个演员可能出演了多个剧本,  一个剧本中可能包含多个演员

#### 2.数据库设计范式

和数据完整性不同，数据库的设计范式更偏向于表设计的维度来看待数据的存储. 其存在的目的也是为了, 在维护或者操作数据库中数据:

<span style=color:red;font-size:20px>  **1、希望在数据库中数据存储更规整 2、希望操作数据的时候效率更高**</span>

==**三大范式的核心目标**==

1. **消除重复数据——原子性**
2. **消除部分依赖——唯一性**
3. **消除传递依赖——不冗余**

- ==**第一范式：原子性**==

  - 每一列**不能再拆**（原子性）
  - 每一行**能唯一区分开**（不能有完全一样的两行）

  在设计表的时候, 应该每列保持原子性。 如果数据库中的所有字段都是<span style=color:red;background:yellow;font-size:20px>**不可分割的原子值**</span>，则说明该数据库满足第一范式，比如：地址。

  第一范式：我们在设计表的时候，应该考虑之后业务的变化，来尽量让每一列的数据保持原子性。

  **反例：**

  ```sql
  id   name    hobby
  1    张三    篮球,足球,游戏
  2    李四    看书
  -- -------------------------------
  # 满足第一范式后
  id   name    hobby
  1    张三    篮球
  1    张三    足球
  1    张三    游戏
  2    李四    看书
  # 每一行都是独立、唯一、不可再分
  ```

- ==**第二范式：唯一性**==

  - 必须先满足**第一范式**
  - **所有非主键字段，必须依赖【完整主键】，不能只依赖主键的一部分**
  - 避免：**部分依赖 → 造成数据冗余、重复、无法唯一确定**

  数据的唯一性。 要求表中数据有<span style=color:red;background:yellow;font-size:20px>**唯一标识**</span>，不存在部分依赖。非主键字段必须完全依赖整个主键，而不是部分主键，通过主键来唯一标识一个用户(满足唯一性)

  注意: 通过name+nickname+province+city+county组合标识一个用户(不满足唯一性)

  **反例：**

  ```sql
  学号  课程号  姓名   课程名
  001  101    张三   数学
  001  102    张三   语文
  主键是：(学号，课程号)
  但姓名只依赖学号，不依赖课程号 → 部分依赖,导致：张三重复出现，数据冗余，无法唯一确定一个人。
  -- ---------------------------------------------------------------
  若是要满足第二范式，则拆表：
  # 学生表：
  学号  姓名
  001  张三
  # 课程表：
  课程号  课程名
  101    数学
  102    语文
  # 成绩表：
  学号  课程号
  001  101
  001  102
  # 每一行都唯一、不重复、不冗余
  ```

  

- ==**第三范式：不冗余**==

  - 先满足前两个范式即先满足**第二范式**
  - 不能有：A → B → C 这种传递依赖
  - 所有非主键必须**直接依赖主键**，而不是依赖其他非主键
  - 保证：**通过主键能唯一确定所有字段**

  <span style=color:red;background:yellow;font-size:20px>**字段不要冗余**</span>。(消除表中非主键字段间的依赖: 即:要求每个非主键字段只依赖于主键，而不依赖于其他非主键字段)

  **反例：**

  ```sql
  学号  姓名  学院  学院电话
  001  张三  计算机  123456
  # 学院电话 依赖学院，不直接依赖学号
  # → 传递依赖：学号 → 学院 → 学院电话
  # 导致：学院电话重复存储，无法唯一确定学院信息
  -- --------------------------------------
  若要满足第三范式，则需拆表
  # 学生表
  学号  姓名  学院编号
  001  张三   01
  # 学院表
  学院编号  学院名   电话
  01      计算机  123456
  # 所有信息都只依赖主键，没有传递依赖，唯一性彻底保证
  ```

#### 3.多表查询(连表查询)

##### 1.交叉连接

交叉连接其实就是求多个表的<span style=color:red;font-size:20px>**笛卡尔积**</span>

```sql
SELECT <字段名> FROM <表1> CROSS JOIN <表2> [WHERE子句]

select * from student cross  join  equip;
select * from student cross  join  equip  where student.id = equip.student_id;
# 两个表的笛卡尔积，返回结果数量就是两个表的数据行相乘。
# 如果每个表有1000行，那么返回结果的数量就有1000×1000=1000000行。
```

仅交叉连接的结果没有太多实际的使用意义

> 注：
>
> 交叉连接（笛卡尔积）既不是排列，也不是组合，它就是「两个集合的有序配对」，数学上叫笛卡尔积。
>
> **笛卡尔积（交叉连接）**：
>
> 左表一条 + 右表一条，**固定左在前、右在后**，不会反过来，也不会内部乱排。

##### 2.自然连接

没什么用

```sql
# 自然连接是基于两个表之间的共同列来自动匹配并组合数据。
# 自然连接将结果集限制为只包括两个表中`具有相同值`的列(并且在结果集中把重复的列去掉)。在使用自然连接时，不需要指定连接条件，而是根据两个表中具有相同名称和数据类型的列进行匹配。 (注意: 有些数据库不支持自然连接， 比如SQLServer )

select * from student natural join class;
```

##### 3.内连接——本质：交叉连接+过滤

​		INNER JOIN 就是**只保留两边都能匹配上的数据**，不匹配的，直接丢掉。
​		**两个表行数不一样，才是常态**，内连接本来就是为这种情况设计的。INNER JOIN 不关心两个表原来有多少行，它只关心：有多少行能匹配上。最终结果行数 = **能匹配上的行数**，可能比两个表都少，也可能在中间，**不可能比两个表都多**（不考虑一对多重复的情况）

==在 SQL 里，`JOIN` 就等于 `INNER JOIN`，`INNER` 是可以省略不写的==

```sql
SELECT <字段名> FROM <表1> INNER JOIN <表2> [ON子句]

#显示内连接
select * from  student inner join equip on student.id = equip.student_id;

# 隐式内连接: 不建议这样写(这是早期的sql语法中内连接的一种写法)
select * from student , equip  where  student.id = equip.student_id;
```

##### 4.左/右外连接

**不加where时**，主表的行一条都不会少，一定会全部显示

显示数量：**最少 = 主表行数**，可能更多，但绝对不会更少

主副表的问题

> 假设A和B表进行连接，AB两张表一个表示主表，另一个是副表; 查询数据的时候,   以主表中的数据为基准，匹配副表对应的数据;   当副表中的数据没有能和主表对应数据相互匹配的数据，副表匹配位置自动填充null。

```sql
SELECT <字段名> FROM <表1> LEFT OUTER JOIN <表2> <ON子句>
SELECT <字段名> FROM <表1> RIGHT OUTER JOIN <表2> <ON子句>


select * from student left outer join equip on student.id = equip.student_id;
select * from student right outer join equip on student.id = equip.student_id;
select * from equip right outer join student on student.id = equip.student_id;
select * from equip left outer join student on student.id = equip.student_id;
-- outer可省略: 工作中多数人省略了outer
select * from student left join equip on student.id = equip.student_id;
select * from student right join equip on student.id = equip.student_id;
```

##### 5.自连接(可以是内连接，也可以是外连接)

```sql
#自连接是指在同一个表中，使用不同的别名将它们连接到一起。

#查询数学成绩低于林冲的数学成绩的人的信息
select t1.* from score t1, score t2 where  t2.id = 1 and  t1.chinese < t2.chinese # 属于(隐式)内连接的自连接
```

##### 6.子查询(嵌套查询)

子查询也叫**嵌套查询**( 在某个操作中(删除/修改/查找), 用到了另外一个查询的结果) ，是指在WHERE子句或FROM子句中又嵌入SELECT查询语句

==**派生表（Derived Table）**，就是把一个**子查询放在 FROM 子句中**，当作一张**临时虚拟表**来使用，查询结束后自动消失。**一定要加别名（AS 别名）**，否则语法错误==

```sql
SELECT <字段名> FROM <表|子查询> WHERE <IN| NOT IN | EXISTS | NOT EXISTS > <子查询>

select * from  student where  id in (select student_id from equip);
select * from  student where  id  not in (select student_id from equip where student_id !=  "");

select * from  student where exists (select * from equip where student_id = 11);
select * from  student where not exists (select * from equip where student_id = 11);
select * from  student where exists (select * from equip where student_id = 11)  and id = 5;
# 在MySQL每次查询数据的结果集都是一个新的临时表。
```

##### 7.联合查询

联合查询**合并**两条查询语句的查询结果.

联合查询**去掉**两条查询语句中的**重复数据行**，然后返回合并后没有重复数据行的查询结果。

```sql
SELECT <字段名> FROM <表> UNION  SELECT <字段名> FROM <表>

select * from  score  where chinese >= 90 union select * from  score  where math >= 90;
```

### C的API

#### 相关函数

1. **mysql_init——初始化连接**

   ```c
   MYSQL *mysql_init(
       MYSQL *mysql // 一个指向MYSQL结构的指针,通常传入NULL，函数会为你分配一个新的MYSQL结构。
   			    // 如果你已经有一个MYSQL结构的空间，也可以传入该结构的指针，函数将在该结构上进行初始化。
   );
   // 返回值: 成功时，返回一个指向MYSQL结构的指针。如果内存分配失败，返回NULL。
   ```

   > mysql_init函数线程不安全: 所以当在一个进程中的多个线程中准备同时创建多个数据库连接的时候, 建议先加锁(connect之后再解锁)

2. **mysql_real_connect——建立到Mysql服务器的连接**

   ```c
   MYSQL *mysql_real_connect(
       MYSQL *mysql,        // 指向MYSQL结构的指针
       const char *host,    // 数据库服务器的主机名或IP地址(传递NULL或"localhost"将尝试连接到本地机器上的服务器。)
       const char *user,    // 用于登录数据库的用户名
       const char *passwd,  // 与用户名相对应的密码 
       const char *db,      // 要连接的数据库名
       unsigned int port,   // 数据库服务器的端口号。如果指定为0，将使用默认端口（通常是3306）
       const char *unix_socket, // 连接本地MySQL服务器时Unix套接字文件路径。可以填NULL
       unsigned long client_flag // 用于控制连接行为的标志。填0即可。
   );
   // 成功时，返回MYSQL结构的指针。失败时，返回NULL。
   ```

   **int mysql_set_character_set(MYSQL *mysql, const char *csname);——设置当前连接字符集**

   ```sql
   # 示例
   mysql_set_character_set(conn, "utf8mb4");
   ```

3. **mysql_query——执行增删改查的SQL语句**

   ```c
   int mysql_query(
      MYSQL *mysql, // 建立连接的MYSQL结构的指针
      const char *query // 要执行的SQL语句的字符串(这个SQL字符串不应以分号结尾，并且必须是以空字符结束)
   );
   // 返回值:成功返回0。失败返回非0值
   ```

4. **处理结果**

   1. **mysql_store_result——获取返回结果**

      ```c
      // 获取由mysql_query()函数返回的结果
      MYSQL_RES *mysql_store_result(
      	MYSQL *mysql // 建立连接的MYSQL结构的指针
      );
      // 返回值:
      // 成功：返回一个指向MYSQL_RES结构的指针，该结构代表了从服务器返回的结果集。通过这个结构，可以进一步获取查询结果
      // 失败：如果SQL没有返回数据(eg:INSERT/UPDATE/DELETE操作,或者SELECT没有找到结果,或者其它错误)，则返回NULL
      /*
      typedef struct st_mysql_res {
          my_ulonglong     row_count;       // 结果集总行数
          unsigned int     field_count;     // 总列数（字段数）
          MYSQL_FIELD     *fields;          // 列元数据数组（字段名、类型、长度）
          MYSQL_DATA      *data;            // 实际数据存储区
          MYSQL_ROWS      *data_cursor;     // 行遍历光标
          unsigned long   *lengths;         // 当前行各列数据长度
          MYSQL_ROW       current_row;      // 当前读取行（MYSQL_ROW）
          MYSQL           *handle;          // 所属连接句柄
          my_bool          eof;             // 是否读到末尾
          // 其他内部内存管理字段...
      } MYSQL_RES; 
      */
      ```

   2. **mysql_fetch_row——从结果中取出一行**

      ```c
      // 从结果当中取出一行
      MYSQL_ROW mysql_fetch_row( // typedef char ** MYSQL_ROW
          MYSQL_RES *result // 指向MYSQL_RES结构的指针
      );
      // 返回值: 返回MYSQL_ROW类型(一个字符串数组，每个元素对应结果集中一行的一个字段的值)。
      // 如果字段值为NULL，相应的数组元素也将是NULL。
      // 如果没有更多行了,或结果集为空，函数返回NULL。
      ```

   3. **mysql_num_rows——获取结果行数**

      ```c
      // SQL语句结果的行数
      my_ulonglong mysql_num_rows(
          MYSQL_RES *result // 指向MYSQL_RES结构的指针
      );
      // 返回值:结果集的行数(my_ulonglong: 是一个无符号的64位整数); 如果result是NULL函数返回0。
      ```

   4. **mysql_num_fields——获取结果列数**

      ```c
      // SQL语句结果的列数
      unsigned int mysql_num_fields(
          MYSQL_RES *result // 指向MYSQL_RES结构的指针
      );
      // 返回值:返回结果集中的列数; 如果result是NULL函数返回0
      ```

   5. **mysql_free_result——释放数据结构MYSQL_RES占据的内存空间**

      ```c
      // 释放数据结构MYSQL_RES占据的内存空间
      void mysql_free_result(
          MYSQL_RES *result // 指向MYSQL_RES结构的指针
      );
      ```

5. **mysql_close——关闭与MySQL的连接**

   ```c
   void mysql_close(
       MYSQL *mysql // 建立连接的MYSQL结构的指针
   );
   ```

6. **mysql_error——检测函数出错，诊断了解为什么一个特定的MySQL操作（如连接、查询等）失败**

   ```c
   const char *mysql_error(
       MYSQL *mysql // MYSQL结构的指针
   );
   // 返回值: 描述最近一次API调用失败原因
   ```

#### 代码示例

1. **select查询**

   ```c
   
   ```

2. **update修改**

   ```c
   
   ```

### 事务

<span style=color:red;background:yellow;font-size:20px>**事务是组成逻辑的一组SQL操作，这个操作的各个单元或者语句，要么都成功执行，要么都不成功执行**</span>

<span style=color:red;font-size:20px>**构成事务的三个操作：开启事务、回滚(非必要)、提交**</span>

```sql
# 开启事务
# start transaction   #也可以使用begin开启事务
begin;    # 开启事务

#执行sql操作
update account set money = money+200 where name like '%黄四郎%';

update account set money = money+200 where name like '%家族2%';
update account set money = money+200 where name like '%家族3%';
update account set money = money+200 where name like '%家族4%';

#事务回滚
rollback; -- 回滚事务之后，之前(到begin)所有的操作都会失效。

update account set money = money-800 where name like '张麻子';

#提交事务
commit; # 提交事务之后，从开启事务到提交事务之间所有的操作才会生效。
```

<span style=color:red;font-size:20px>**注意: 开启事务之后, 要注意提交(要么rollback, 要么commit，即rollback和commit是完全对立的但但核心功能相同-即结束事务)**</span>

#### 事务特性ACID

==**做了一堆SQL操作：这些sql操作我们希望是不可拆分的**==

<span style=color:red;background:yellow;font-size:20px>**(1)原子性atomicity**</span>

一个事务, **这个事务整体的所有sql操作都要必须要被视为一个不可分割的最小单元**, **整个事务中的操作要么全部提交成功, 要么全部失败回滚**, 对于一个事务来说, 不可能只执行其中的一部分操作, 这就是事务的原子性.(这是从操作角度来谈失误: 操作要么都成功, 要么都失败, 不可切割)

<span style=color:red;background:yellow;font-size:20px>**(2)一致性consistency**</span>

**数据库总是从一个一致性的状态转换到另一个一致性的状态**. 如前面的例子, 所谓一致性, 就是要么扣款和增加都成功, 要么未成功发生扣款的同时也未发生金额增加, 数据(金钱的总量是要前后一致的)是一致性变化的. (从事务的结果, 数据角度来谈:  **数据是从一个状态, 通过事务, 要么不变, 要么一致性的演变到另外一种状态**)

<span style=color:red;background:yellow;font-size:20px>**(3)隔离性isolation**</span>

就是**事务和事务之间的关联性**。注意事务之间的关联, 有可能强, 也有可能关联性弱 (这是可以设置的)

<span style=color:red;background:yellow;font-size:20px>**(4)持久性durability**</span>

事务的持久性是指**一旦事务提交后或者回滚，事务对数据库的改变应该是永久性的**。 (因为: 提交和回滚,  都会结束事务)

#### 并发可能产生的问题

##### 1.脏写

**后一次的写将前一次的写覆盖掉**

同一事务里，后一次写覆盖前一次写 → **正常的事务内修改**；
脏写是**两个不同事务之间**的严重并发问题，和单事务无关。

##### 2.脏读(Dirty read)

<span style=color:red;font-size:20px>一个事务读取到了另外一个事务还没提交的数据:即脏读(读到了脏数据)</span>——**即一个事务能看到==另一个==事务的修改数据(未提交)**

##### 3.不可重复读(nonrepeatable read)

<span style=color:red;font-size:20px>在同一个事务内，针对同一个数据，前后读取的数据不一样: 即不可重复读</span>——**即一个事务能看到==另一个==事务的修改数据(提交/未提交)**

##### 4.虚幻读/幻读/幻影读(phantom  read)

<span style=color:red;font-size:20px>在一个事务内，任何一条数据的内容前后读取一致, 但是数据条数前后不一定一致。</span>——**即一个事务能看到==另一个==事务的添加(删除)数据(已提交-因为未提交时也属于脏读范畴)**

##### 区别

> 1. **脏读、不可重复读**：都是针对**已存在的行**——**行内容问题**
> 2. **幻读**：是针对**行的数量变化**——**行数量问题**
>
> - 幻读若是未提交时属于脏读范畴：只要是 “读到了别人没提交的数据”，不管是 update、insert 还是 delete，统统都叫脏读
> - 可重复读一定—>不可脏读，若是存在脏读则一定—>不可重复读

#### 查看和修改隔离级别

```sql
# 查看隔离级别
SELECT @@GLOBAL.TX_ISOLATION; -- 整个数据的设置
SELECT @@SESSION.TX_ISOLATION; -- 一个连接的隔离级别
# 设置全局（所有新会话）
SET GLOBAL TRANSACTION ISOLATION LEVEL [SERIALIZABLE | REPEATABLE READ| ...];
# 设置当前会话（仅本连接）
SET SESSION TRANSACTION ISOLATION LEVEL [SERIALIZABLE | REPEATABLE READ| ...];

# 示例
select @@global.tx_isolation;  #查看全局隔离级别
select @@session.tx_isolation; #查看会话隔离级别
#设置全局隔离级别
set global transaction isolation level read uncommitted;
set global transaction isolation level repeatable read;
```

#### 隔离级别

|               隔离级别               |               隔离效果               |
| :----------------------------------: | :----------------------------------: |
|     READ UNCOMMITTED（读未提交）     |   别人能看到你**未提交**的临时数据   |
| READ COMMITTED（读已提交，**默认**） | 别人只能看到你**commit 之后**的数据  |
|     REPEATABLE READ（可重复读）      | 别人完全看不到你中间修改，直到你提交 |
|        Serializable（串行化）        |         直接锁表，互相看不见         |

| ==隔离级别（均无脏写）==         | ==脏读== | ==不可重复读== | ==虚幻读== |
| :------------------------------- | :------: | :------------: | :--------: |
| **读未提交（read uncommitted）** |    ✅️     |       ✅️        |     ✅️      |
| **读已提交（read committed）**   |  **X**   |       ✅️        |     ✅️      |
| **可重复读（repeatable read）**  |  **X**   |     **X**      |  **✅️-×**   |
| **串行化（serializable）**       |  **X**   |     **X**      |   **X**    |

### ==数据库题目==

1. **请找出两科成绩在90分以上的学生名称**

   ```sql
   create table student(
   id int,
   name varchar(20),
   chinese float,
   english float,
   math float
   );
   
   # 一般解
   select name from student where (chinese >= 90 && english >= 90) || (chinese >= 90 && math >= 90) || (english >= 90 && math >= 90);
   # 最优解
   select name from student where (chinese >= 90) + (english >= 90) + (math >= 90) >= 2;
   ```

2. **新建一个学生表S，有包含如下信息, 并插入10条数据。**

   ```sql
   -- 学号 id，
   -- 学生姓名 name，
   -- 性别 gender，
   -- 年龄 age，
   -- 专业 dept，
   -- 所在系编号 …等
   -- 学号格式为 201801 201802 201803...
   -- 性别只有 'male' & 'female'
   -- 院系包含（信息系、数学系，计算机科学系等）
   
   # 简单题
   -- 1.查询全体学生的学号与姓名。
   -- 3.查询全体学生的详细记录。
   -- 4.查询全体学生的姓名、出生年份和所属部门 使用列别名改变查询结果的列标题
   -- 7.查询所有年龄在20岁以下的学生姓名及其年龄。
   -- 8.查询年龄在20~23岁（包括20岁和23岁）之间的学生的姓名、系别和年龄。
   -- 9.查询年龄不在20~23岁之间的学生姓名、系别和年龄。
   -- 10.查询信息系、数学系和计算机系学生的姓名和性别。
   -- 11.查询既不是信息系、数学系，也不是计算机科学系的学生的姓名和性别。
   -- 12.查询学号为200518的学生的详细情况。
   -- 13.查询所有姓刘学生的姓名、学号和性别。
   -- 14.查询姓“李”且全名为两个汉字的学生的姓名。
   -- 15.查询名字中第2个字为“立"字的学生的姓名和学号。
   -- 16.查询所有不姓刘的学生姓名。
   -- 17.查询学号在201801~201809之间的学生姓名。
   -- -----------------------------------------------------------------
   # 提高题
   -- 18.查询不同院系学生的人数。
   -- 19.查询不同院系学生的平均年龄，并以降序排序。
   -- 20.查询计算机系年龄在20岁以下的学生姓名。
   -- 22.查询全体学生情况，查询结果按所在系的系号升序排列，同一系中的学生按年龄降序排列。
   
   
   # 答案
   1.select id, name from S;
   3.select * from S;
   4.select name 姓名, 2026-age 出生年份, dept 所属部门 from S;
   7.select name, age from S where age < 20;
   8.select name, dept, age from S where age between 20 and 23;
   9.select name, dept, age from S where age not in (20,21,22,23);
   10.select name, gender from S where dept in ('信息系','数学系','计算机科学系');
   11.select name, gender from S where dept not in ('信息系','数学系','计算机科学系');
   12.select * from S where id = 200518;
   13.select name, id, gender from S where name like '刘%';
   14.select name from S where name like '李_';
   15.select name, id from S where name like '_立%';
   16.select name from S where name not like '刘%';
   17.select name from S where id between 201801 and 201809;
   -- -----------------------------------------------------------------
   18.select 院系编号, count(id) from S group by 院系编号;
   19.select 院系编号, avg(age) from S group by 院系编号 order by avg(age) desc;
   20.select 院系编号, group_concat(name) from S where age < 20 group by 院系编号 having 院系编号 = 计算机科学系编号;
    或 select name from S where age < 20 && 院系编号 = 计算机系编号;
   22.select * from S order by 院系编号 asc, age desc;
   ```

3. **创建一个城市表，字段有id、名字、所属国家名字、人口、所属省份, 并插入若干数据**

   ```sql
   # 简单题
   -- 1. 查询所有城市名及人口信息
   -- 2. 查询city表中，所有中国的城市信息
   -- 3. 查询人口数小于100w人城市信息
   -- 4. 查询中国,人口数超过500w的所有城市信息
   -- 5. 查询中国或美国的城市信息
   -- 6. 查询人口数为100-200w（包括两头）城市信息
   -- 7. 查询中国或美国，人口数大于500w的城市
   -- 8. 查询城市名为qing开头的城市信息
   -- ---------------------------------------------------------------------------
   # 提高题
   -- 9. 统计city表的行数
   -- 10.统计各国城市的个数
   -- 11.统计每个国家的城市总人口数
   -- 12.统计中国每个省的城市个数及城市名列表
   -- 13.统计每个国家的城市个数,并且只显示超过5个城市的国家
   -- 14.统计每个国家的城市个数,并且只显示超过5个城市的国家并按照从大到小排序
   -- 15.统计每个国家的城市个数,并且只显示超过5个城市的国家并按照从大到小排序,并且只显示排名前三
   
   
   # 答案
   1.select 城市名, 人口 from 城市表;
   2.select 城市名, 人口 from 城市表 where 所属国家 = '中国';
   3.select * from 城市表 where 人口 < 100;
   4.select * from 城市表 where 所属国家 = '中国' && 人口 > 500;
   5.select * from 城市表 where 所属国家 in ('中国','美国');
   6.select * from 城市表 where 人口 between 100 and 200;
   7.select * from 城市表 where (所属国家 in ('中国','美国')) && 人口 > 500;
   8.select * from 城市表 where 城市名 like 'qing%';
   -- ---------------------------------------------------------------------------
   9.select count(1) from city;
   10.select 所属国家名字, count(*) from city group by 所属国家名字; # *代表当前所有行数也可用名字代替，只要城市名不为空
   11.select 所属国家名字, sum(人口) from city group by 所属国家名字;
   12.select 所属省份,count(名字) as 城市个数, group_concat(名字) as 城市名列表  from city where 所属国家名字 = '中国' group by 所属省份;  # 中国是字符串，需要' '
   13.select 所属国家名字, count(*) as 城市个数 from city group by 所属国家名字 having count(*) > 5;
   14.select 所属国家名字, count(*) as 城市个数 from city group by 所属国家名字 having count(*) > 5 order by 城市个数 desc;
   15.select 所属国家名字, count(*) as 城市个数 from city group by 所属国家名字 having count(*) > 5 order by 城市个数 desc limit 0, 3;
   ```

4. **某大学研究生院**

   ```sql
   有若干研究生导师，包括职工编号、姓名、职称、研究方向，其中每个导师的职工编号是唯一的。
   若干研究生，包括学号、姓名、性别、入学日期，其中每个研究生的学号是唯一的。
   每个导师可以带若干研究生，但每个研究生只能有一个导师。
   # 提高题
   -- 请设计一个数据库，要求可以正确体现导师和研究生之间的关系。
   -- 设计完毕之后，请插入一定量的数据，并验证你设计的数据库是否满足要求。
   -- 1.请查出每个导师所带研究生的姓名。
   -- 2.清查出特定姓名的导师所带研究生的姓名。
   -- 3.请查出每个导师所带研究生的数量。
   -- 4.请查出每个导师所带的男研究生的数量。
   -- 5.请找出选择哪个研究方向的导师最多。
   -- 6.请找统计不同职称的导师的个数。
   
   -- 建表
   create database if not exists academy;
   -- 导师表
   create table teachers (
   teacher_id int primary key,
   t_name varchar(20) not null,
   title varchar(20),
   research varchar(50)
   );
   -- 研究生表
   create table students (
   student_id int primary key,
   s_name varchar(20) not null,
   gender char(1),
   enroll_date date,
   teacher_id int,
   foreign key(teacher_id) references teachers(teacher_id)
   );
   # 答案
   -- 一般解
   1.select 导师, group_concat(学生) 学生名单 from (select t.t_name 导师, s.s_name 学生 from students s left join teachers t on t.teacher_id = s.teacher_id where s.teacher_id is not NULL order by t.teacher_id) temp group by 导师; # 每个派生表都要有自己的别名，即使用不到
   2.select 学生 from (select t.t_name 导师, s.s_name 学生 from students s left join teachers t on t.teacher_id = s.teacher_id where s.teacher_id is not NULL order by t.teacher_id) temp where 导师 = '张教授';
   3.select 导师, count(学生) 学生数量 from (select t.t_name 导师, s.s_name 学生 from students s left join teachers t on t.teacher_id = s.teacher_id where s.teacher_id is not NULL order by t.teacher_id) temp group by 导师;
   4.select 导师, count(学生) 男学生数量 from (select t.t_name 导师, s.s_name 学生 from students s left join teachers t on t.teacher_id = s.teacher_id where s.teacher_id is not NULL && s.gender = '男' order by t.teacher_id) temp group by 导师;
   5.select group_concat(研究方向) 研究方向清单 from (select research 研究方向, count(teacher_id) 人数 from teachers group by research) temp group by 人数 order by 人数 desc limit 0, 1; # ！子查询里order by无效，会被MySQL自动忽略，分组后select可映射的一定是分组后的结果中可存在的列，像研究方向已经不是新的列了，但是研究方向清单确实新的列，所以可以单独映射，并非一定要映射结果中存在分组的那个列
   6.select title 职称, count(teacher_id) 导师个数 from teachers group by title;
   -- 最优解
   1.每个导师所带研究生姓名：JOIN+GROUP BY+GROUP_CONCAT
   select t.t_name 导师, group_concat(s.s_name) 学生名单 from teachers t join students s on t.teacher_id = s.teacher_id group by t.teacher_id, t.t_name;
   2.特定导师带的研究生：JOIN+WHERE 过滤导师名
   select s.s_name 学生 from teachers t join students s on t.teacher_id = s.teacher_id where t.t_name = '张教授';
   3.每个导师带研究生数量：JOIN+GROUP BY+COUNT (*)
   select t.t_name 导师, count(*) 学生数量 from teachers t join students s on t.teacher_id = s.teacher_id group by t.teacher_id, t.t_name;
   4.每个导师带的男研究生数量：JOIN+WHERE 性别 + GROUP BY+COUNT (*)
   select t.t_name 导师, count(*) 男学生数量 from teachers t join students s on t.teacher_id = s.teacher_id where s.gender = '男' group by t.teacher_id, t.t_name;
   5.哪个研究方向导师最多：单表 GROUP BY+ORDER BY DESC+LIMIT 1
   select research 研究方向, count(*) 人数 from teachers group by research order by 人数 desc limit 1;
   6.统计不同职称导师数量：单表 GROUP BY+COUNT (*)（原写法已是最优）
   ```

5. **在数据库中创建如下两张表:部门dept、雇员emp**

   ```sql
   -- 部门
   create table dept (
   deptno varchar(10) primary key,
   dname varchar(10)
   );
   -- 雇员
   create table emp (
   empno varchar(10) primary key,
   ename varchar(10),
   job varchar(10),
   mgr varchar(10),
   sal float,
   deptno varchar(10) references dept(deptno)
   );
   -- 添加数据
   insert into dept values ('1','事业部');
   insert into dept values ('2','销售部');
   insert into dept values ('3','技术部');
   insert into emp values ('01','jacky','clerk','tom','1000','1');
   insert into emp values ('02','tom','clerk','','2000','1');
   insert into emp values ('07','biddy','clerk','','2000','1');
   insert into emp values ('03','jenny','sales','pretty','600','2');
   insert into emp values ('04','pretty','sales','','800','2');
   insert into emp values ('05','buddy','jishu','canndy','1000','3');
   insert into emp values ('06','canndy','jishu','','1500','3');
   # 提高题
   -- 1.列出emp表中各部门的部门号，最高工资，最低工资
   -- 2.列出emp表中各部门job为'CLERK'的员工的最低工资，最高工资
   -- 3.emp中最低工资小于2000的部门，列出job为'CLERK'的员工的部门号，最低工资，最高工资
   -- 4.根据部门号由高而低，工资有低而高列出每个员工的姓名，部门号，工资
   -- 5.列出'buddy'所在部门中每个员工的姓名与部门号
   -- 6.列出每个员工的姓名，工作，部门号，部门名
   -- 7.列出emp中工作为'CLERK'的员工的姓名，工作，部门号，部门名
   -- 8.对于emp中有管理者的员工，列出姓名，管理者姓名（管理者外键为mgr）
   -- 9.对于dept表中，列出所有部门名，部门号，同时列出各部门工作为'CLERK'的员工名与工作
   -- 10.对于工资高于本部门平均水平的员工，列出部门号，姓名，工资，按部门号排序
   -- 11.对于emp，列出各个部门中工资高于本部门平均工资的员工数和部门号，按部门号排序
   
   # 答案
   1.select deptno 部门号, max(sal) 最高工资, min(sal) 最低工资 from emp group by deptno;
   2.select deptno 部门号, min(sal) 最低工资, max(sal) 最高工资 from emp where job = 'clerk' group by deptno;
   3.select deptno 部门号, min(sal) 最低工资, max(sal) 最高工资 from emp where job = 'clerk' group by deptno having min(sal) < 2000;
   4.select ename 姓名, deptno 部门号, sal 工资 from emp order by deptno desc, sal asc;
   5.select e1.ename 姓名, e1.deptno 部门号 from emp e1 inner join emp e2 on e1.deptno = e2.deptno where e2.ename = 'buddy'; # 与自身的内连接 
   或 select ename 姓名, deptno 部门号 from emp where deptno = (select deptno from emp where ename = 'buddy'); # 当查询结果是一行一列时就代表一个值
   6.select e.ename 姓名, e.job 工作, e.deptno 部门号, d.dname 部门名 from emp e left outer join dept d on e.deptno = d.deptno;
   7.select e.ename 姓名, e.job 工作, e.deptno 部门号, d.dname 部门名 from emp e left outer join dept d on e.deptno = d.deptno where e.job = 'clerk';
   8.select ename 姓名, mgr 管理员姓名 from emp where mgr != '';
   9.select d.deptno 部门号, d.dname 部门名, e.ename 员工名, e.job 工作 from dept d left join emp e on d.deptno = e.deptno && e.job = 'clerk'; # 内连接不行的原因：只会保留有clerk的部门，题目前提是先列出所有的部门名、部门号，所以只能用外连接
   10.select e.deptno 部门号, e.ename 姓名, e.sal 工资 from emp e left outer join (select deptno no, avg(sal) avgs from emp group by deptno) a on e.deptno = a.no where e.sal > a.avgs order by e.deptno;
   11.select 部门号, count(姓名) 员工数 from (select e.deptno 部门号, e.ename 姓名, e.sal 工资 from emp e left outer join (select deptno no, avg(sal) avgs from emp group by deptno) a on e.deptno = a.no where e.sal > a.avgs) temp group by 部门号 order by 部门号; 
   ```

   

