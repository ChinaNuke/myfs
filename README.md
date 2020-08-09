# unix-filesystem
OS课程设计，基于磁盘存储设备的UNIX文件系统。

## 题目要求

磁盘文件系统:建立基于磁盘存储设备的 UNIX 文件系统。

- 首先分配一定容量的磁盘存储空间,作为文件存储空间；
- 建立相应的文件系统,使用 UNIX 文件系统,使用索引结点、混合分配方式、成组连接方法管理存储空间；
- 解决文件的重名、共享和安全控制；
- 支持文件的`按名存取`；
- 为该文件系统设计相应的数据结构来管理目录、磁盘空闲空间、已分配空间等；
- 提供文件的创建、删除、移位、改名等功能；
- 提供良好的界面,可以显示磁盘文件系统的状态和空间的使用情况；
- 提供虚拟磁盘转储功能,可将信息存入磁盘,还可从磁盘读入内存。

## 目录说明

- buid: 用于构建项目的文件以及构建生成的文件
- core: 所设计文件系统的源文件
- demo: GUI 演示程序
- docs: 相关的文档

## 分支说明

- master: 最终确定能够运行的的稳定版，不轻易修改，不直接在上面改动；
- dev: 开发分支，在某个时间点允许不能运行，不直接在上面改动；
- 其他本地分支：开发时基于最新的dev在本地建立自己的分支，叫什么随意（比如feature1, bug1），怎么折腾随意，最终merge到dev分支，这些本地分支不push到远程仓库。

## 项目开发计划

- [x] 学习并测试C的库怎么写，怎么调用；复习C语言知识；
- [x] 学习Qt Creator使用，正式构建项目结构；
- [ ] 根据项目结构进行分工安排，各自进行自己的开发，并根据情况merge到dev分支；
- [ ] 两人一起对dev分支进行测试，确保没有问题后merge到master分支，core部分开发结束。
- [ ] 进行GUI部分开发。

## 编译

```bash
mkdir build
cd build
cmake ..
make
```

目前尚无可以运行的程序，可以运行 main 看是否正常编译，会输出一行 `HelloWorld!` 。