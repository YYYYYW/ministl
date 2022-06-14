<<<<<<< HEAD
# ministl
========
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
>>>>>>> 48d1f6f (first)
