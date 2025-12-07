#include "engine/utils/BufferMemoryBlocks.h"

#include "engine/utils/Logger.h"

namespace Utils {

CBufferMemoryBlocks::CBufferMemoryBlocks() : mTotalSize(0ULL) {
}

CBufferMemoryBlocks::~CBufferMemoryBlocks() {
    mBlocks.clear();
}

void CBufferMemoryBlocks::Init(uint64_t totalSize) {
    mTotalSize = totalSize;
    mBlocks.clear();
    mBlocks.push_back(BufferBlock{0, totalSize, true});
}

std::optional<SBufferRange> CBufferMemoryBlocks::Allocate(uint64_t size) {
    BufferBlock* bestFit = nullptr;
    for (auto& block : mBlocks) {
        if (block.free && block.size >= size) {
            if (!bestFit || block.size < bestFit->size) {
                bestFit = &block;
            }
        }
    }
    if (!bestFit) {
        return std::nullopt;
    }

    SBufferRange range{bestFit->offset, size};
    if (bestFit->size > size) {
        auto bestFitIt = std::find_if(
            mBlocks.begin(), mBlocks.end(), [&](const BufferBlock& b) {
                return b.offset == bestFit->offset && b.size == bestFit->size &&
                       b.free;
            });
        auto nextIt = std::next(bestFitIt);
        mBlocks.insert(nextIt, BufferBlock{bestFit->offset + size,
                                           bestFit->size - size, true});
        bestFit->size = size;
    }
    bestFit->free = false;
    return range;
}

void CBufferMemoryBlocks::Free(const SBufferRange& block) {
    auto itFreeing =
        std::find_if(mBlocks.begin(), mBlocks.end(), [&](const BufferBlock& b) {
            return b.offset == block.offset && b.size == block.size && !b.free;
        });
    if (itFreeing == mBlocks.end()) {
        LOG_WARNING("Trying to free a non-existing block!");
        return;
    }

    if (itFreeing != mBlocks.begin()) {
        auto prevIt = std::prev(itFreeing);
        if (prevIt->free) {
            prevIt->size += itFreeing->size;
            mBlocks.erase(itFreeing);
            itFreeing = prevIt;
        }
    }
    // Merge with next block if free
    if (itFreeing != mBlocks.end()) {
        auto nextIt = std::next(itFreeing);
        if (nextIt != mBlocks.end() && nextIt->free) {
            itFreeing->size += nextIt->size;
            mBlocks.erase(nextIt);
            itFreeing->free = true;
        }
    }
}

bool CBufferMemoryBlocks::Contains(const SBufferRange& block) const {
    auto it =
        std::find_if(mBlocks.begin(), mBlocks.end(), [&](const BufferBlock& b) {
            return b.offset == block.offset && b.size == block.size && !b.free;
        });
    return it != mBlocks.cend();
}

std::optional<SBufferRange>
CBufferMemoryBlocks::TryResize(const SBufferRange& block, std::size_t newSize) {
    auto it =
        std::find_if(mBlocks.begin(), mBlocks.end(), [&](const BufferBlock& b) {
            return b.offset == block.offset && b.size == block.size && !b.free;
        });
    if (it == mBlocks.end()) {
        return std::nullopt;
    }

    if (newSize == block.size) {
        return block;
    } else if (newSize < block.size) {
        std::size_t sizeDifference = block.size - newSize;
        it->size = newSize;
        auto nextIt = std::next(it);
        mBlocks.insert(nextIt,
                       BufferBlock{it->offset + newSize, sizeDifference, true});
        return SBufferRange{it->offset, newSize};
    } else {
        std::size_t additionalSizeNeeded = newSize - block.size;
        auto nextIt = std::next(it);
        if (nextIt != mBlocks.end() && nextIt->free &&
            nextIt->size >= additionalSizeNeeded) {
            it->size = newSize;
            if (nextIt->size > additionalSizeNeeded) {
                nextIt->offset += additionalSizeNeeded;
                nextIt->size -= additionalSizeNeeded;
            } else {
                mBlocks.erase(nextIt);
            }
            return SBufferRange{it->offset, newSize};
        }
    }
    return std::nullopt;
}

void CBufferMemoryBlocks::Reset() {
    mBlocks.clear();
    mBlocks.push_back(BufferBlock{0, mTotalSize, true});
}

uint64_t CBufferMemoryBlocks::GetTotalSize() const {
    return mTotalSize;
}

} // namespace Utils