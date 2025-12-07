#pragma once

#include "engine/core/DataTypes.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/BufferMemoryBlocks.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/IBufferHandleWrapper.h"
#include "engine/vulkan/VulkanContext.h"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {

template <typename T>
class CBufferHandleWrapper : public IBufferHandleWrapper {
public:
    CBufferHandleWrapper(const CVulkanContext& vulkanContext,
                         Renderer::CMemoryAllocator& memoryAllocator)
        : mMemoryAllocator(memoryAllocator)
        , mVulkanContext(vulkanContext)
        , mDataSize(memoryAllocator.AlignSize(sizeof(T))) {
    }

    ~CBufferHandleWrapper() override {
        Destroy();
        mBuffer = VK_NULL_HANDLE;
        mMemory = VK_NULL_HANDLE;
    }

    bool Init(uint64_t bufferSize, VkBufferUsageFlags usage) override {
        if (mBuffer != VK_NULL_HANDLE || mMemory != VK_NULL_HANDLE) {
            LOG_WARNING("Reinitializing an already initialized buffer!");
            Destroy();
        }
        mUsage = usage;
        mMemoryBlocks.Init(bufferSize);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.usage = usage;
        bufferInfo.size = bufferSize;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkDevice device = mVulkanContext.GetDevice();
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &mBuffer) !=
            VK_SUCCESS) {
            LOG_FATAL("Failed to create buffer!");
            return false;
        }

        mMemory = mMemoryAllocator.AllocateMemory(
            mBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(device, mBuffer, mMemory, 0);
        return true;
    }

    VkBuffer GetBuffer() const {
        return mBuffer;
    }
    bool PrepareData(Utils::SBufferIndexRange& previousIndexRange,
                     const T& data) {
        return PrepareData(previousIndexRange, std::vector<T>{data});
    }

    bool PrepareData(Utils::SBufferIndexRange& previousIndexRange,
                     const std::vector<T>& data) {

        const auto newDataSize = data.size() * mDataSize;
        const auto previousDataRange =
            Utils::SBufferRange{.offset = previousIndexRange.first * mDataSize,
                                .size = previousIndexRange.count * mDataSize};

        if (previousIndexRange.first + previousIndexRange.count == 0) {
            LOG_INFO("Data range not uploaded yet, reserving a space for "
                     "it and uploading it on next upload call.");

            auto reservedRange = Reserve(data.size() * mDataSize);
            if (!reservedRange.has_value()) {
                LOG_FATAL("Not enough space to reserve data in data "
                          "buffer! Buffer needs to be resized.");
                // TODO: Fatal for now, implement buffer resizing later
                return false;
            }
            previousIndexRange = Utils::SBufferIndexRange{
                .first = reservedRange->offset / mDataSize,
                .count = data.size()};
        } else if ((previousIndexRange.first + previousIndexRange.count) *
                       mDataSize <
                   newDataSize) {
            LOG_INFO("Data size has changed, resizing and uploading new data "
                     "on next upload call.");
            auto resizedRange =
                mMemoryBlocks.TryResize(previousDataRange, newDataSize);
            if (!resizedRange.has_value()) {
                mMemoryBlocks.Free(previousDataRange);
                resizedRange = Reserve(newDataSize);
                if (!resizedRange.has_value()) {
                    LOG_FATAL("Not enough space to resize data in data "
                              "buffer! Buffer needs to be resized.");
                    return false;
                }
            }
            previousIndexRange = Utils::SBufferIndexRange{
                .first = resizedRange->offset / mDataSize,
                .count = data.size()};
        }
        mToUpdate.push_back(std::make_pair(
            (Utils::SBufferRange{.offset = previousIndexRange.first * mDataSize,
                                 .size = previousIndexRange.count * mDataSize}),
            data));
        return true;
    }

    bool Upload() override {
        if (mToUpdate.empty()) {
            LOG_INFO("No data to upload or range not prepared!");
            return true;
        }
        VkDeviceSize minOffset = UINT64_MAX;
        VkDeviceSize maxOffset = 0;

        for (const auto& [range, data] : mToUpdate) {
            minOffset = std::min(minOffset, range.offset);
            maxOffset = std::max(maxOffset, range.offset + range.size);
        }

        if (PrepareUpdate(minOffset, maxOffset - minOffset)) {
            for (auto& [range, data] : mToUpdate) {
                range.offset -= minOffset;
                Update(&data, range);
            }
        } else {
            LOG_ERROR("Failed to prepare buffer for update!");
            return false;
        }
        FinalizeUpdate();

        mToUpdate.clear();
        return true;
    }

    bool Clear() override {
        // if (mCurrentRange.has_value()) {
        //     mBufferHandle->FreeRange(mCurrentRange.value());
        //     mCurrentRange = std::nullopt;
        // }
        // mStorage.clear();
        // mIsUploaded = false;
        return true;
    }

    // const std::vector<T>& GetData() const {
    //     return mStorage;
    // }

private:
    void Destroy() {
        if (mBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(mVulkanContext.GetDevice(), mBuffer, nullptr);
            mBuffer = VK_NULL_HANDLE;
        }
        if (mMemory != VK_NULL_HANDLE) {
            vkFreeMemory(mVulkanContext.GetDevice(), mMemory, nullptr);
            mMemory = VK_NULL_HANDLE;
        }
        mMemoryBlocks.Reset();
    }

    std::optional<Utils::SBufferRange> Reserve(VkDeviceSize size) {
        return mMemoryBlocks.Allocate(static_cast<uint64_t>(size));
    }

    void* PrepareUpdate(VkDeviceSize offset, VkDeviceSize size) {
        void* mappedData{nullptr};
        auto result = vkMapMemory(mVulkanContext.GetDevice(), mMemory, offset,
                                  size, 0, &mappedData);
        return mappedData;
    }

    void Update(void* data, const Utils::SBufferRange& range) {
        memcpy(static_cast<uint8_t*>(data) + range.offset, data, range.size);
    }

    bool FinalizeUpdate() {
        vkUnmapMemory(mVulkanContext.GetDevice(), mMemory);
        return true;
    }

    const CVulkanContext& mVulkanContext;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    Utils::CBufferMemoryBlocks mMemoryBlocks;

    VkBufferUsageFlags mUsage{0};
    VkBuffer mBuffer{VK_NULL_HANDLE};
    VkDeviceMemory mMemory{VK_NULL_HANDLE};
    VkDeviceSize mDataSize{0};

    std::vector<std::pair<Utils::SBufferRange, std::vector<T>>> mToUpdate;
};
} // namespace Vulkan