
#ifndef MINISTL_ALLOC_H
#define MINISTL_ALLOC_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef MINISTL_THROW_BAD_ALLOC
#include <new>
#define MINISTL_THROW_BAD_ALLOC throw std::bad_alloc()
#endif

namespace ministl {
    
/**
 * 一级内存配置器
 * 使用 malloc 直接分配内存
 * 在 SGI_STL 中还添加了模板 template <int inst>
 * 但是这个模板参数并没有用到
 * 而参数名 inst 似乎也只是意味着生成一个实例
 * 在 SGI_STL 的注释中提到：仅用于允许创建多个实例
 * 以下实现中没有加上这个模板
 */
class first_alloc_template {

private:
    // oom: out of memory
    static void * oom_malloc(size_t);
    static void * oom_realloc(void *, size_t);
    // 用于处理 oom 时的函数指针
    // 参考了 SGI_STL 中实现
    static void (* oom_handler) ();

public:
    static void * allocate(size_t n) {
        void *result = malloc(n);
        // 如果 malloc 失败了，则会调用 oom_malloc
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void * reallocate(void *p, size_t n) {
        void *result = realloc(p, n);
        if (0 == result) result = oom_realloc(p, n);
        return result;
    }

    static void deallocate(void *p, size_t) {
        free(p); 
    }

    static void (* set_malloc_handler(void (*f)() )) () {
        void (* old) () = oom_handler;
        oom_handler = f;
        return (old);
    }

};

void (* first_alloc_template::oom_handler)() = 0;

void * first_alloc_template::oom_malloc(size_t n) {
    void (* my_malloc_handler)();
    void * result;

    for (;;) {                      // 不断尝试
        my_malloc_handler = oom_handler;
        if (nullptr == my_malloc_handler) { MINISTL_THROW_BAD_ALLOC; }
        (* my_malloc_handler)();    // 调用处理函数
        result = malloc(n);         // 再次尝试申请内存
        if (result) return result;
    }
}

void * first_alloc_template::oom_realloc(void *p, size_t n) {
    void (*my_malloc_handler)();
    void *result;

    for (;;) {
        my_malloc_handler = oom_handler;
        if (nullptr == my_malloc_handler) { MINISTL_THROW_BAD_ALLOC; }
        (* my_malloc_handler)();
        result = realloc(p, n);
        if (result) return result;
    }
}

/**
 * 二级内存配置器，也是默认配置器
 * 对于分配小区块内存提高性能
 * 当分配内存大于128字节时使用一级内存配置器，小于等于128字节使用二级
 * 会先分配一大块内存
 * 用一个长度为 16 的链表维护小内存的分配和回收
 * 链表的每个节点是一个固定内存大小区块的链表
 * 8 16 24 32 40 48 56 64 72 80 88 96 104 112 120 128
 * 目前只有单线程，没有考虑多线程操作
 */
class default_alloc_template {

private:
    static const int ALIGN = 8;
    static const int MAX_BYTES = 128;
    static const int NFREELISTS = 16;

    // 将bytes上调至8的倍数
    static size_t ROUND_UP(size_t bytes) {
        // ((n + 7) & (~7))
        return ((bytes) + ALIGN-1) & ~(ALIGN-1);
    }

    union obj {                 // free_lists的节点构造
        union obj * free_list_link;
        char client_data[1];    // The client sees this
    };

    // 16 个 free_lists,
    static obj * volatile free_list[NFREELISTS];

    static size_t FREELIST_INDEX(size_t bytes) {
        // bytes应该是大于 0 的
        // ((bytes + 7)/8) - 1 的结果是从 0 开始的
        return (((bytes) + ALIGN-1)/ALIGN - 1);
    }

    static void *refill(size_t n);

    // 配置一大块空间，可容纳 nobjs 个 size 大小的区块
    // 如果无法分配 nobjs 个区块， nobjs 可能会降低，nobjs 是传递引用
    static char *chunk_alloc(size_t size, int &nobjs);

    static char *start_free;    // 内存池的起始位置，只在chunk_alloc()中变化
    static char *end_free;      // 内存池的结束位置，只在chunk_alloc()中变化
    static size_t heap_size;

public:
    // n must be > 0
    static void * allocate(size_t n) {
        obj * volatile * my_free_list;
        obj * result;
        // 如果 n 大于 128 则使用一级配置器
        if (n > MAX_BYTES) return first_alloc_template::allocate(n);
        // 寻找16个free lists中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (nullptr == result) {
            // 没有可用的 free list，准备重新填充 free list
            void *r = refill(ROUND_UP(n));
            return r;
        }
        // 调整 free list
        *my_free_list = result->free_list_link;
        return result;
    }

    // p 不是 nullptr
    static void deallocate(void *p, size_t n) {
        // 如果 n 大于 128 则使用一级配置器
        if (n > MAX_BYTES) {
            first_alloc_template::deallocate(p, n);
            return;
        }
        // 这里将 p 转为 obj* 给 q。
        obj * q = (obj*) p;
        obj * volatile * my_free_list;
        // 找到合适的位置
        my_free_list = free_list + FREELIST_INDEX(n);
        // 将 q 的这个区块插入到 free list 头中
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    static void * reallocate(void *p, size_t old_size, size_t new_size);

};

// 初始化静态变量
char * default_alloc_template::start_free = nullptr;
char * default_alloc_template::end_free = nullptr;
size_t default_alloc_template::heap_size = 0;

default_alloc_template::obj * volatile 
default_alloc_template::free_list[default_alloc_template::NFREELISTS] = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

/**
 * 当 free list 为空时，需要调用 refill 来从内存池中获取区块填充到 free list 中
 * 返回一个大小为 n 的对象，并且有时候会为适当的 free list 增加节点
 * 假设 n 已经上调至 8 的倍数
 */
void * default_alloc_template::refill(size_t n) {
    int nobjs = 20;
    // 调用 chunk_alloc 尝试从内存池中获取 nobjs 个区块
    char * chunk = chunk_alloc(n, nobjs);
    // 如果只获取一个区块，则直接返回，将这个区块给调用者
    if (1 == nobjs) return chunk;
    // 否则调整 free list，加入新节点
    obj * volatile * my_free_list;
    my_free_list = free_list + FREELIST_INDEX(n);
    obj * current_obj;
    obj * next_obj;
    obj * result;
    result = (obj*) chunk;      // 第一块将会返回给调用者使用
    *my_free_list = next_obj = (obj*)(chunk + n);
    for (int i = 1; ; ++i) {    // 从 1 开始，第 0 个作为 result 返回
        current_obj = next_obj;
        next_obj = (obj*)((char*)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj->free_list_link = nullptr;
            break;
        }
        current_obj->free_list_link = next_obj;
    }
    return result;
}

// size 已经上调至 8 的倍数
// nobjs 是传递的引用
char* default_alloc_template::chunk_alloc(size_t size, int& nobjs) {
    char *result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free; // 内存池剩余空间

    if (bytes_left >= total_bytes) {
        // 内存池剩余空间完全满足需求量
        result = start_free;
        start_free += total_bytes;
        return (result);
    } else if (bytes_left >= size) {
        // 内存池剩余空间不能完全满足需求量，但足够供应一个及以上的区块
        nobjs = bytes_left/size;
        total_bytes = nobjs * size;
        result = start_free;
        start_free += total_bytes;
        return (result);
    } else {
        // 内存池剩余空间连一个区块的大小都无法满足
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        // 以下试着让内存池中的残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池内还有一些零头，先分配给适当的 free list
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整 free list, 将内存池中的残余空间加入
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*) start_free;
        }

        // 配置 heap 空间，用来补充内存池
        start_free = (char *)malloc(bytes_to_get);
        if (nullptr == start_free) {
            // heap 空间不足，malloc 失败
            int i;
            obj * volatile * my_free_list;
            obj * p;
            // 使用已经拥有的东西，而不是去尝试分配更小的区块
            // 因为那样会在多进程机器上容易导致灾难
            // 在 free list 找到一块未被使用并且足够大的区块
            for (i = size; i <= MAX_BYTES; i += ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (p != nullptr) {     // free list 中还有区块
                    // 调整 free list 释放出未用的区块
                    *my_free_list = p->free_list_link;
                    start_free = (char*) p;
                    end_free = start_free + i;
                    // 递归调用自己，为了修正 nobjs 
                    return (chunk_alloc(size, nobjs));
                    // 任何残余零头都会被编入适当的 free list 中
                }
            }
            end_free = nullptr;     // 没有内存可以用了
            // 调用一级配置器，看看oom机制能否尽点力
            start_free = (char *)first_alloc_template::allocate(bytes_to_get);
            // 这会导致抛出异常
        }

        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        // 递归调用自己，为了修正 nobjs
        return (chunk_alloc(size, nobjs));
    }
}

void * default_alloc_template::reallocate(void *p, size_t old_size, size_t new_size) {
    void *result;
    size_t copy_size;

    if (old_size > MAX_BYTES && new_size > MAX_BYTES) {
        return realloc(p, new_size);
    }
    if (ROUND_UP(old_size) == ROUND_UP(new_size)) return p;
    result = allocate(new_size);
    copy_size = new_size > old_size ? old_size : new_size;
    memcpy(result, p, copy_size);
    deallocate(p, old_size);
    return result;
}

}

#endif // MINISTL_ALLOC_H