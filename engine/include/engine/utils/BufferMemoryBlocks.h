#pragma once

#include "engine/utils/BufferTypes.h"

#include <list>
#include <optional>

namespace Utils {

class CBufferMemoryBlocks {
public:
    CBufferMemoryBlocks();
    ~CBufferMemoryBlocks();

    void Init(std::size_t totalSize);
    std::optional<SBufferRange> Allocate(std::size_t size);
    void Free(const SBufferRange& block);
    bool Contains(const SBufferRange& block) const;
    void Reset();

    int GetTotalSize() const;

private:
    struct BufferBlock {
        std::size_t offset;
        std::size_t size;
        bool free;
    };

    std::list<BufferBlock> mBlocks;
    std::size_t mTotalSize;
};

} // namespace Utils