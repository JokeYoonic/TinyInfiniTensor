#include "core/allocator.h"
#include <utility>

namespace infini
{
    Allocator::Allocator(Runtime runtime) : runtime(runtime)
    {
        used = 0;
        peak = 0;
        ptr = nullptr;

        // 'alignment' defaults to sizeof(uint64_t), because it is the length of
        // the longest data type currently supported by the DataType field of
        // the tensor
        alignment = sizeof(uint64_t);
    }

    Allocator::~Allocator()
    {
        if (this->ptr != nullptr)
        {
            runtime->dealloc(this->ptr);
        }
    }

    size_t Allocator::alloc(size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        // pad the size to the multiple of alignment
        size = this->getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来分配内存，返回起始地址偏移量
        // =================================== 作业 ===================================
        // 检查是否有足够的空闲块
        for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it) {
            if (it->second >= size) {
                // 找到足够大的空闲块
                size_t addr = it->first;
                size_t remaining_size = it->second - size;

                // 移除当前空闲块
                free_blocks.erase(it);

                // 如果有剩余空间，重新插入空闲块列表
                if (remaining_size > 0) {
                    free_blocks[addr + size] = remaining_size;
                }

                used += size;
                if (used > peak) {
                    peak = used;
                }
                return addr;
            }
        }

        // 如果没有找到合适的空闲块，从 used 指针分配
        size_t addr = used;
        used += size;
        if (used > peak) {
            peak = used;
        }
        return addr;

    }
    

    void Allocator::free(size_t addr, size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        size = getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来回收内存
        // =================================== 作业 ===================================
		        // 将释放的内存块插入空闲块列表
        free_blocks[addr] = size;

        // 尝试合并相邻的空闲块
        auto it = free_blocks.find(addr);
        if (it != free_blocks.begin()) {
            auto prev_it = std::prev(it);
            if (prev_it->first + prev_it->second == addr) {
                // 合并前一个空闲块
                prev_it->second += size;
                free_blocks.erase(it);
                it = prev_it;
            }
        }

        auto next_it = std::next(it);
        if (next_it != free_blocks.end() && addr + size == next_it->first) {
            // 合并后一个空闲块
            it->second += next_it->second;
            free_blocks.erase(next_it);
        }

        used -= size;
    }

    void *Allocator::getPtr()
    {
        if (this->ptr == nullptr)
        {
            this->ptr = runtime->alloc(this->peak);
            printf("Allocator really alloc: %p %lu bytes\n", this->ptr, peak);
        }
        return this->ptr;
    }

    size_t Allocator::getAlignedSize(size_t size)
    {
        return ((size - 1) / this->alignment + 1) * this->alignment;
    }

    void Allocator::info()
    {
        std::cout << "Used memory: " << this->used
                  << ", peak memory: " << this->peak << std::endl;
    }
}
