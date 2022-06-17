# ministl
## first

新建了这个仓库，希望能完成这个目标 ministl

初始，我把 C++primer 第五版 十三章 四十四题写了出来，照葫芦画瓢，实现一个简单的string，但是从这个string开始可以确定一些暂时性的目标

1. 空间配置器，allocator。
   - 第一版的这个string的alloc使用的是std中的。
   - 一个大致的方向为实现的allocator需要有**allocate**和**deallocate**两个基本的函数
   - 并且allocator是支持泛型
   - 具体的细节得边学习边实现了
2. 迭代器，iterator。
   - 在第一版的string中begin和end函数直接返回的char*，很显然需要一个迭代器来支持这样的操作
   - 迭代器相对来说比较独立感觉
   - 应该不难实现（狗头）
3. 工具函数
   - forward
   - move
   - DEBUG宏函数（总不能一直写#ifndef MINISTL_DEBUG）
   - ministl 的 pair（感觉是一个支持泛型的结构体，包含first和second两个成员）
   - 。。。（后续应该还能想要需要的一些工具函数）

wyj 2022.06.14



## 空间配置器更新

初步实现了空间配置器，参考了SGI_STL的实现

- 新增了constructor.h文件
  - 主要实现两个函数 construct() 和 destroy()
  - construct 使用 placement new 构造对象
  - destroy 调用对象的 析构函数
  - 实现了 destroy 的参数为迭代器的版本, 以及特化版本
- 新增了alloc.h 文件
  - 实现了一级分配器和二级分配器
  - 一级分配器简单的使用 malloc 分配内存，实现自定义了 malloc 内存不够的 处理函数配置
  - 二级分配器使用内存池，以及 16 个链表来分配回收内存
  - 当需要分配的内存大于 128 B(Byte) 时，使用一级分配器，小于128 B 时，使用二级分配器
- 新增了allocator.h 文件
  - 实现了空间分配器暴露的接口 allocator
- 新增了uninitialized.h 文件
  - 包含三个函数
  - uninitialized_fill_n()
  - uninitialized_copy()
  - uninitialized_fill()
  - 三个函数的目的都是在一个已经分配好的内存区域中，构造对象即初始化对象
- 待完成
  - allocator 类中还有几个接口没有实现完，例如参数为 模板参数包 时
  - uninitialized.h 的函数实现中，使用到了迭代器相关内容，如 true_type, false_type, value_type() 等
  - uninitialized.h 的函数实现中，使用到了部分算法文件中函数，fill(), fill_n(), copy() 。需要在算法文件中实现这些函数
  - 编写空间配置器的相关测试

wyj 2022.06.17

