__# BombThePlane (v1.0)
__
**due 2019/5/6**


------

## 开发环境

### client 

- C++ 
- QT

### server

- C++
- Mysql

------

# TODO

首先需要设计

- [x] 报文格式(2018.12.13)
- [x] 数据库表(2018.12.13)

## 基本要求

- [x] 用户登录
- [x] 用户改密
- [x] 重复登录，后者踢掉前者
- [x] 可以看到在线的其他用户
- [x] 可以建立/销毁双人游戏
- [x] 可以互相发送游戏猜测、断言
- [x] server需要记录log
- [x] client信息尽量存放在server端

安全

- [x] 数据库password字段需要加密





### Client 

- [x] client收到数据后，如何反馈到GUI?

### Server

------

# 协议栈

一共四种报文：

- 报道包
  - client: 发送用户密码
  - server: accept/reject和原因
- 上线/下线包
  - 只有server对client. 给所有在线用户发
  - 类型:上线/下线
  - 附加用户信息

- 文本信息发送包
  - 参考sj作业课件
  - 文件包可以在**文件信息发送包**的类型里面

- 设置包
  - 双向，用于设置GUI
  - 传配置文件(config.json)

## 报文格式

**报头**格式

|大小 | 描述 |
|------|-------|
| 2B   | 报文大类型  |
| 2B   | 细分类型/应答类型 | 
| 4B   | 长度(**不包括报头长度**) |


### 报道包 

**client -> server**

|大小 | 数值 |  描述 |
|------|-------| -------|
| 2B   | 0x10  | 类型
| 2B   | 0x00  </br> 0x01  | 登录 </br> 注册 |
| 4B   |  |  长度 | 
| 32B  |  | 用户名
| 32B  |   | 密码

|大小 | 数值 |  描述 |
|------|-------| -------|
| 2B   | 0x10  | 类型
| 2B   | 0x02  | 更改密码 |
| 4B   |  |  长度 | 
| 32B  |  | 用户名
| 32B  |   | 原密码
| 32B  |   | 现密码


**server -> client**

|大小 | 数值 |  描述 |
|------|-------|-------|
| 2B   | 0x80  | 类型
| 2B   | 0x00  </br> 0x01  </br> 0x02  </br> 0x03 </br> 0x04 </br> 0x05  </br> 0x06  </br> 0x07 </br> 0x08 </br> 0x09 </br> 0x0a  |登录/注册 成功 </br> 登录成功（但是踢了原来登陆的人） </br> 登录失败，密码错误 </br>登录/更改密码失败，用户名不存在 </br>登录失败，需要强制改密  </br>注册失败，用户名已存在 </br>注册失败，用户名不符合要求  </br>注册失败，密码不符合要求  </br> 更改密码成功 </br> 更改密码失败，原密码错误 </br> 更改密码失败，现密码不符合规范
| 4B   | | 长度|
</br>
登录成功附加数据: 

- 0x00 0x01

| 大小 | 数值 | 描述 |
|------|-------|-------|
| 19B  | |上次登录时间 |
| 4B  | |用户数量 ***n*** |
| 33B * ***n*** |  |所有用户信息  |

>注：第33B为字符'0'(ascii不为0)时代表离线，为'1'代表在线

其他无附加数据

### 上线/下线包

**server -> client**

|大小 | 数值 |  描述 |
|------|-------| -------|
| 2B   | 0x81  | 类型
| 2B   | 0x00  </br> 0x01  | 上线 </br>下线|
| 4B   | | 长度  |
| 32B   | |用户名  |
> 注：此处client若收到的下线包是自己，则代表着有人抢占该账户，client自动exit


###### 0x12 0x82 Done

### 设置包

**client -> server** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x13  | 类型
| 2B   | 0x00 | 填充 |
| 4B   | | 长度  |
| to end | | 配置文件(config) |

**server -> client**

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x83  | 类型
| 2B   | 0x00 | 填充 |
| 4B   | | 长度  |
| to end | | 配置文件(config) |

> 注：只有确认登录成功之后，server才会发此包，而client在接收到该包后才会进入真正的用户界面。</br>
> 同时，由于是登录后的操作，因而不必加上用户名。

###### 在线文件notify包 0x14
###### 文件数据包 0x15 

> 注：以下为新加入的部分

### 游戏状态包

**server -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x84  | 类型
| 2B   | 0x00 </br>0x01 | 不在游戏</br>正在游戏 |
| 4B   | | 长度  |
| 32B | | 用户名 |
> 注:发送此包说明均在线

**client -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x16  | 类型
| 2B   | 0x00 </br>0x01 </br>0x02 </br> 0x03 | 邀请对方建立对局</br>接受对局</br>拒绝对局</br>中途取消对局 |
| 4B   | | 长度  |
| 32B | | 发送方用户名 |
| 32B | | 接收方用户名 |
> 注: 附加信息只与发送方自己和接收方有关

### 准备游戏包

**client -> server** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x17  | 类型
| 2B   | 0x00  | 已准备就绪 |
| 4B   | | 长度  |


**server -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x85  | 类型
| 2B   | 0x00</br>0x01  | 先手开始游戏</br>后手开始游戏 |
| 4B   | | 长度  |

### 正在游戏包

**client -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x18  | 类型
| 2B   | 0x00</br>0x01  | 猜测位置(只有一个位置)</br>断言位置 |
| 4B   | | 长度  |
| 2B   | | 位置（小）  |
| 2B   | | 位置（大）  |
> 注: 附加信息中默认设置时，前者的位置绝对值较后者的小
> 注: 在猜测位置中只用到了位置(小)

**client -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x19  | 类型
| 2B   | 0x00</br>0x01</br>0x02</br>0x03</br>0x04  | 猜测未命中</br>猜测伤飞机</br>猜测毁飞机</br>断言未中</br>断言命中 |
| 4B   | | 长度  |

**client -> client** 

|大小 | 数值 | 描述 |
|------|-------|-------|
| 2B   | 0x1a  | 类型
| 2B   | 0x00  | 对方胜利，双方均结束游戏 |
| 4B   | | 长度  |
> 注: 被断言方若发现对方命中三次飞机后，直接发给server并自行给自己失败提示，server负责转发（并给其他client发送OffGame包）


# 数据库

## `user_info`

系统只需要一个表

|uname char(32) | pwd char(32) | last_login_time datetime | user_set mediumblob |
|------|-------|------| ------ |
|zhongyuchen| xxxx | 2018.12.13 20:12 | xxxx |
|zhaiyuchen| xxxx | 2018.12.12 14:22| xxxx |
|liuyitao| xxxx | 2018.12.13 15:32| xxxx |
> 注：在用户从未登录的状态下，其上次登录时间设置为0000.00.00 00:00:00表示下次需要强制改密



