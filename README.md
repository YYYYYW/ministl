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



## 迭代器、萃取器、vector、部分算法更新

- 增加了一个测试allocator的小程序

  - 测试中遇到的问题：

    - 不要将 #include 放在 namespace ministl { } 的大括号中间，否则会出现奇怪的错误，感觉和 #include 的展开顺序有关

    - 在 allocate 中的 construct 函数中，增加一个参数为右值引用的函数

    - ```C++
      template <typename T, typename Alloc>
      void allocator<T, Alloc>::construct(pointer p, T&& value);
      ```

    - 这是因为测试使用的std::vector，在添加元素的时候是需要使用参数为右值引用的 construct 函数

  - 修改后使用 ministl::allocator 作为 std::vector 的分配器

- 新增 util.h 文件

  - _MINISTL_DEBUG 宏函数用于 debug 打印信息
  - move函数 对应 std::move

- 新增 type_traits.h 文件

  - true_type 和 false_type
  - type_traits

- 新增iterator.h文件

  - ```C++
    struct input_iterator_tag { };
    struct output_iterator_tag { };
    struct forward_iterator_tag : public input_iterator_tag { };
    struct bidirectional_iterator_tag : public forward_iterator_tag { };
    struct random_access_iterator_tag : public bidirectional_iterator_tag { };
    
    
    template <typename I>
    struct iterator_traits {
        typedef typename I::iterator_category   iterator_category;
        typedef typename I::value_type          value_type;
        typedef typename I::difference_type     difference_type;
        typedef typename I::pointer             pointer;
        typedef typename I::reference           refernece;
    };
    ```

  - 用于迭代器萃取

- 新增 algo.h 文件

  - 新增copy函数，将两个迭代器范围内的值复制到另一个迭代器处
  - 新增copy_backward函数，与copy类似，不过是从尾部向前复制
  - 新增fill函数，将两个迭代器返回内的值使用给定值填充
  - 新增fill_n函数，将一个迭代器开始后n个值使用给定值填充

- 新增 vector.h 文件

  - 初步实现基础的插入删除功能。
  - 还缺少一个反向迭代器，缺少使用初始化列表进行初始化
  - 比较操作符还需要完善
  - 还未测试

wyj 2022.07.05
