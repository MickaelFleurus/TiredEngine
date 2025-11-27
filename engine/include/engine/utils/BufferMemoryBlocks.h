#pragma once

#include <list>
#include <optional>

namespace Utils {

struct SBufferRange {
    int offset;
    int size;
};

class CBufferMemoryBlocks {
public:
    CBufferMemoryBlocks();
    ~CBufferMemoryBlocks();

    void Init(int totalSize);
    std::optional<SBufferRange> Allocate(int size);
    void Free(const SBufferRange& block);
    bool Contains(const SBufferRange& block) const;

    int GetTotalSize() const {
        return mTotalSize;
    }

private:
    struct BufferBlock {
        int offset;
        int size;
        bool free;
    };

    std::list<BufferBlock> mBlocks;
    int mTotalSize;
};

} // namespace Utils