#pragma once

#include <cstdint>
#include <list>
#include <optional>

#include "engine/utils/BufferTypes.h"


namespace Utils {

class CBufferMemoryBlocks {
public:
    CBufferMemoryBlocks();
    ~CBufferMemoryBlocks();
    CBufferMemoryBlocks(CBufferMemoryBlocks&&) noexcept = default;
    CBufferMemoryBlocks& operator=(CBufferMemoryBlocks&&) noexcept = default;

    CBufferMemoryBlocks(const CBufferMemoryBlocks&) = default;
    CBufferMemoryBlocks& operator=(const CBufferMemoryBlocks&) = default;

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
