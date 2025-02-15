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

	size_t Allocator::alloc(size_t size) //size_t alloc(size_t size); in allocator.h
    {
        size = getAlignedSize(size);

        // 1. Try to find a free block that fits
        for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it) {
			if (it->second >= size) { // it为循坏返回的迭代器，it->second为迭代器指向的值
                size_t addr = it->first;
                it->second -= size;
                if (it->second == 0) {
                    free_blocks.erase(it);
                }
                used += size;
                return addr;
            }
        }

        // 2. No suitable free block found, allocate from the end
        size_t addr = used;
        used += size;
        if (used > peak) {
            size_t newPeak = std::max(peak * 2, used); // Or a more sophisticated strategy
			// Below is the actual memory allocation
            //void* newPtr = runtime->alloc(newPeak);
            //if (newPtr == nullptr) {
            //    throw std::runtime_error("Memory allocation failed!");
            //}
            //if (ptr != nullptr) {
            //    memcpy(newPtr, ptr, peak); // Copy old data
            //    runtime->dealloc(ptr);
            //}
            //ptr = newPtr;
            peak = newPeak;
            //printf("Allocator really alloc: %p %lu bytes\n", this->ptr, peak);
        }
        return addr;
    }
        // =================================== 作业 ===================================
        // TODO: 设计一个算法来分配内存，返回起始地址偏移量
        // =================================== 作业 ===================================

    void Allocator::free(size_t addr, size_t size)
    {
        size = getAlignedSize(size);
        IT_ASSERT(ptr != nullptr);
        IT_ASSERT(addr + size <= used);

        used -= size;

        // 1. Insert the free block into the free_blocks map
        free_blocks[addr] = size;

        // 2. Try to merge with adjacent free blocks
        auto it = free_blocks.find(addr);

        // Check for merge with the block before
        if (it != free_blocks.begin()) {
            auto prev_it = std::prev(it);
            if (prev_it->first + prev_it->second == addr) {
                prev_it->second += size;
                free_blocks.erase(it);
                it = prev_it; // Update iterator for next merge check
            }
        }

        // Check for merge with the block after
        auto next_it = std::next(it);
        if (next_it != free_blocks.end() && it->first + it->second == next_it->first) {
            it->second += next_it->second;
            free_blocks.erase(next_it);
        }
    }
    // =================================== 作业 ===================================
    // TODO: 设计一个算法来回收内存
    // =================================== 作业 ===================================


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
