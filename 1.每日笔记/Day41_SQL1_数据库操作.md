## Day41——SQL1_数据库操作

### 基本操作

#### **1.登录数据库**

```sql
sudo mysql -u root -p      [回车]

输入密码
```

刚装完 MySQL，**只有一个超级管理员账号：root**

MySQL 默认设置：**只有系统 root 才能登 MySQL root**

#### **2.注释与分号**

> 在数据库语句中如果我们需要注释某些内容, 一般有三种方式
>
> `--` 注释符(要注意的是`--`之后要有一个空格再接着书写注释内容)
>
> `#` 注释符 (之后不需要空格)
>
> `/* */` 注释符 (一般用于多行注释)

```sql
-- <注释>;   # 注释语句
#<注释>;     # 注释语句
/*
	<注释>   # 注释语句
*/
```

SQL语句应该要以**分号**作为结束

#### **3.查看数据库**

```sql
show databases;  # 查看所有数据库 

show databases like '%数据库名%'; # 查看和期望命名相匹配的数据库

show create database 数据库名; # 查看数据库创建信息

#show databases like '%数据库名%'; 
# %代表是一个通配符: 通配0-n个字符
show databases like 'test'; -- 指明就找test
show databases like '%n';  -- 一个以n字符结束的数据库
show databases like '%n%'; --  数据库名字中, 有一个n字符 

show create database test; -- 查看之前怎么创建的test数据库(sql语句是什么)
```

#### **4.创建数据库**

```sql
CREATE DATABASE [IF NOT EXISTS] <数据库名>
[[DEFAULT] CHARACTER SET <字符集名>] 
[[DEFAULT] COLLATE <校对规则名>];

#创建一个wangdao的数据库, 有可能创建失败直接报错(假如数据库服务里面已经有一个wangdao的数据库了)
create database wangdao;

#创建一个叫test数据库, 并且要求编码格式是utf8,  还要求排序规则utf8_bin
create database test character set utf8 collate utf8_bin; 

#如果不存在名字为wangdao的数据库, 就创建wangdao, 如果已经存在了wangdao的数据库, 就不创建(也不报错)
create database if not exists wangdao; 

#创建一个指定字符编码格式的和指定排序规则的数据库
create database if not exists test character set utf8 collate utf8_bin; 

#查看数据库创建信息
show create database test;
```

#### **5.删除数据库**

在工作中不要删除数据库, 哪怕这个数据库已经没有任何用处了 ( 任何数据都是有价值的, 哪怕是错的数据)

```sql
DROP DATABASE [IF EXISTS]  <数据库名>;  # 删除数据库

drop database test; # 删除test数据库
```

#### **6.修改数据库**

<span style=color:red;font-size:18px>在工作中不要修改数据库, 因为没有任何意义.</span>

```sql
ALTER DATABASE [数据库名] 
{  [ DEFAULT ] CHARACTER SET <字符集名> | [ DEFAULT ] COLLATE <校对规则名>  }

alter database test character set utf8 collate utf8_bin;  # 把test数据库的编码改成utf8, 校对规则改为utf8_bin
```

#### **7.选择数据库与查询当前选择数据库**

```sql
USE <数据库名>;  # 选择数据库

use test; # 选择test数据库
------------------------------
SELECT DATABASE(); #查询当前正在使用的数据库
```

#### **8.数据库备份和恢复**

```sql
# 数据库备份：cmd命令下 
mysqldump -u root -p 数据库名称>文件名.sql

# 数据库恢复： 
#1. 创建数据库并选择该数据库 
create database dbName; 
use dbName;

#2. 恢复数据 
source 文件名.sql
```



