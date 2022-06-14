# linux c 练习代码

## 构建方式
* cmake
* vscode 自带任务运行，支持调试


## 程序如下
* dllist 双向链表基本操作
* linux_kenel_list 同上，linux内核双向链表测试
* log 基本日志封装 
* time 时间戳与基本时间结构体转换
* time_str c中基本时间操作
* eloop 单线程eloop封装
* httpclient http客户端 (克隆源自:https://gitee.com/topsoup/httpclient/tree/master)
* parson json解析
* uci_c c语言操作uci 增删查改 注意必须使用sudo执行，必须传入section name
* mips_test mips gcc编译器测试
    * io_interrupt_test mips应用层配置gpio输入中断实现
    * io_test_use_xxx.c mips使用c、shell分别控制gpio翻转电平，测试应用层控制io速率
    * endianness.c mips大小端测试
* zip minizip-ng测试 c语言压缩解压缩zip文件
* websocket c语言实现websocket 客户端
* file 文件目录操作
* pthread 线程调度策略测试
* mq posix 消息队列测试
* exe_shell c调用控制台执行脚本测试
* mongoose_user mongoose网络库测试

