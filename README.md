# myfs
系统软件综合设计题目，基于磁盘存储设备的 UNIX 文件系统。

## 题目要求

磁盘文件系统:建立基于磁盘存储设备的 UNIX 文件系统。

- 首先分配一定容量的磁盘存储空间,作为文件存储空间；
- 建立相应的文件系统,使用 UNIX 文件系统,使用索引结点、混合分配方式、成组连接方法管理存储空间；
- 解决文件的重名、共享和安全控制；
- 支持文件的按名存取；
- 为该文件系统设计相应的数据结构来管理目录、磁盘空闲空间、已分配空间等；
- 提供文件的创建、删除、移位、改名等功能；
- 提供良好的界面,可以显示磁盘文件系统的状态和空间的使用情况；
- 提供虚拟磁盘转储功能,可将信息存入磁盘,还可从磁盘读入内存。

## 目录说明

- buid: 用于构建项目的文件以及构建生成的文件
- myfs: 文件系统核心内容
- myshell: 用于演示的mysh程序
- virtualdisk: 虚拟磁盘
- tests: 单元测试
- docs: 相关的文档

## 编译

```bash
mkdir build
cd build
cmake ..
make
```

## 运行

```bash
./mysh
createdisk myfs.fs 16
format myfs.fs 4096
mount myfs.fs
```

其中，`createdisk myfs.fs 16` 命令是创建一个 `16MB` 大小的虚拟磁盘文件，`format myfs.fs 4096` 是将磁盘文件格式化成 myfs 文件系统支持的格式，`mount myfs.fs` 是将格式化后的磁盘文件挂载到 myfs 文件系统中。

随后，可以输入 `help` 命令查看 mysh 支持的命令，输入对应的命令查看使用提示，目前支持的命令有：`createdisk`、`format`、`mount`、`unmount`、`pwd`、`df`、`ls`、`chdisk`、`mkdir`、`touch`、`rm`、`stat`、`stat`、`ln`、`chmod`、`mv`，它们的功能和行为与实际的 Linux 操作系统中对应的命令基本相同。

## 测试

编译后，在 `build` 目录中执行 `./testall` 即可运行全部测试，详细的测试编写和运行方法见 `docs` 中的测试说明。