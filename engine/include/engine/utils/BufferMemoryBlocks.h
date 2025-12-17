#pragma once

#include "engine/utils/BufferTypes.h"

#include <cstdint>
#include <list>
#include <optional>

namespace Utils {

class CBufferMemoryBlocks {
public:
    CBufferMemoryBlocks();
    ~CBufferMemoryBlocks();

    void Init(uint64_t totalSize);
    std::optional<SBufferRange> Allocate(uint64_t size);
    void Free(const SBufferRange& block);
    bool Contains(const SBufferRange& block) const;
    std::optional<SBufferRange> TryResize(const SBufferRange& block,
                                          uint64_t newSize);
    void Reset();

    uint64_t GetTotalSize() const;

private:
    struct BufferBlock {
        uint64_t offset;
        uint64_t size;
        bool free;
    };

    std::list<BufferBlock> mBlocks;
    uint64_t mTotalSize;
};

} // namespace Utils