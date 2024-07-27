/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "memory.h"
#include <coreinit/memexpheap.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memheap.h>
#include <malloc.h>

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Memory functions
//! This is the only place where those are needed so lets keep them more or less private
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static MEMHeapHandle mem1_heap   = NULL;
static MEMHeapHandle bucket_heap = NULL;

void libgui_memoryInitialize(void) {
    if (!mem1_heap) {
        MEMHeapHandle mem1_heap_handle = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
        uint32_t mem1_allocatable_size = MEMGetAllocatableSizeForFrmHeapEx(mem1_heap_handle, 4);
        void *mem1_memory              = MEMAllocFromFrmHeapEx(mem1_heap_handle, mem1_allocatable_size, 4);
        if (mem1_memory)
            mem1_heap = MEMCreateExpHeapEx(mem1_memory, mem1_allocatable_size, 0);
    }

    if (!bucket_heap) {
        MEMHeapHandle bucket_heap_handle = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
        uint32_t bucket_allocatable_size = MEMGetAllocatableSizeForFrmHeapEx(bucket_heap_handle, 4);
        void *bucket_memory              = MEMAllocFromFrmHeapEx(bucket_heap_handle, bucket_allocatable_size, 4);
        if (bucket_memory)
            bucket_heap = MEMCreateExpHeapEx(bucket_memory, bucket_allocatable_size, 0);
    }
}

void libgui_memoryRelease(void) {
    if (mem1_heap) {
        MEMDestroyExpHeap(mem1_heap);
        MEMFreeToFrmHeap(MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1), MEM_FRM_HEAP_FREE_ALL);
        mem1_heap = NULL;
    }
    if (bucket_heap) {
        MEMDestroyExpHeap(bucket_heap);
        MEMFreeToFrmHeap(MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG), MEM_FRM_HEAP_FREE_ALL);
        bucket_heap = NULL;
    }
}

//!-------------------------------------------------------------------------------------------
//! some wrappers
//!-------------------------------------------------------------------------------------------
void *MEM2_alloc(uint32_t size, uint32_t align) {
    return memalign(align, size);
}

void MEM2_free(void *ptr) {
    free(ptr);
}

void *MEM1_alloc(uint32_t size, uint32_t align) {
    if (align < 4)
        align = 4;
    return MEMAllocFromExpHeapEx(mem1_heap, size, align);
}

void MEM1_free(void *ptr) {
    MEMFreeToExpHeap(mem1_heap, ptr);
}

void *MEMBucket_alloc(uint32_t size, uint32_t align) {
    if (align < 4)
        align = 4;
    return MEMAllocFromExpHeapEx(bucket_heap, size, align);
}

void MEMBucket_free(void *ptr) {
    MEMFreeToExpHeap(bucket_heap, ptr);
}
