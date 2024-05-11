#include "salloc/predefined_block_allocator.h"

namespace salloc
{

PredefinedBlockAllocator::PredefinedBlockAllocator(size_t initialChunkSize, std::span<size_t> blockSizes)
    : blockCount{ 0 }
    , chunkCount{ 0 }
    , chunkSize{ initialChunkSize }
    , chunks{ nullptr }
    , sizeMap(std::move(blockSizes))
{
    freeList = (Block**)salloc::Alloc(sizeMap.sizes.size() * sizeof(Block*));
    memset(freeList, 0, sizeMap.sizes.size() * sizeof(Block*));
}

PredefinedBlockAllocator::~PredefinedBlockAllocator()
{
    Clear();
}

void* PredefinedBlockAllocator::Allocate(size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }
    if (size > sizeMap.MaxBlockSize())
    {
        return salloc::Alloc(size);
    }

    size_t index = sizeMap.values[size];
    assert(0 <= index && index <= blockSizeCount);

    if (freeList[index] == nullptr)
    {
        chunkSize += chunkSize / 2;

        Block* blocks = (Block*)salloc::Alloc(chunkSize);
        size_t blockSize = sizeMap.sizes[index];
        size_t blockCapacity = chunkSize / blockSize;

        // Build a linked list for the free list.
        for (size_t i = 0; i < blockCapacity - 1; ++i)
        {
            Block* block = (Block*)((char*)blocks + blockSize * i);
            Block* next = (Block*)((char*)blocks + blockSize * (i + 1));
            block->next = next;
        }
        Block* last = (Block*)((char*)blocks + blockSize * (blockCapacity - 1));
        last->next = nullptr;

        Chunk* newChunk = (Chunk*)salloc::Alloc(sizeof(Chunk));
        newChunk->capacity = blockCapacity;
        newChunk->blockSize = blockSize;
        newChunk->blocks = blocks;
        newChunk->next = chunks;
        chunks = newChunk;
        ++chunkCount;

        freeList[index] = newChunk->blocks;
    }

    Block* block = freeList[index];
    freeList[index] = block->next;
    ++blockCount;

    return block;
}

void PredefinedBlockAllocator::Free(void* p, size_t size)
{
    if (size == 0)
    {
        return;
    }

    if (size > sizeMap.MaxBlockSize())
    {
        salloc::Free(p);
        return;
    }

    assert(0 < size && size <= sizeMap.maxBlockSize);

    size_t index = sizeMap.values[size];
    assert(0 <= index && index <= blockSizeCount);

#if defined(_DEBUG)
    // Verify the memory address and size is valid.
    size_t blockSize = blockSizes[index];
    bool found = false;

    Chunk* chunk = chunks;
    while (chunk)
    {
        size_t currentChunkSize = chunk->blockSize * chunk->capacity;
        if (chunk->blockSize != blockSize)
        {
            assert((char*)p + blockSize <= (char*)chunk->blocks || (char*)chunk->blocks + currentChunkSize <= (char*)p);
        }
        else
        {
            if (((char*)chunk->blocks <= (char*)p && (char*)p + blockSize <= (char*)chunk->blocks + currentChunkSize))
            {
                found = true;
                break;
            }
        }

        chunk = chunk->next;
    }

    assert(found);
#endif

    Block* block = (Block*)p;
    block->next = freeList[index];
    freeList[index] = block;
    --blockCount;
}

void PredefinedBlockAllocator::Clear()
{
    Chunk* chunk = chunks;
    while (chunk)
    {
        Chunk* c0 = chunk;
        chunk = c0->next;
        salloc::Free(c0->blocks);
        salloc::Free(c0);
    }

    blockCount = 0;
    chunkCount = 0;
    chunks = nullptr;
    memset(freeList, 0, sizeMap.sizes.size() * sizeof(Block*));
}

} // namespace salloc
