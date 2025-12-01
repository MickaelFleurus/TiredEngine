#pragma once

#include "engine/core/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandle.h"
#include "engine/vulkan/IBufferHandleWrapper.h"

#include <optional>
#include <vector>

namespace Vulkan {

template <typename T>
class CBufferHandleWrapper : public IBufferHandleWrapper {
public:
    CBufferHandleWrapper(CBufferHandle* bufferHandle)
        : mBufferHandle(bufferHandle) {
    }

    CBufferHandle* GetBuffer() const override {
        return mBufferHandle;
    }

    std::optional<Utils::SBufferIndexRange>
    Prepare(const std::vector<T>& data) {
        if (mIsUploaded) {
            LOG_ERROR(
                "Data already uploaded, clear before preparing new ones.");
            return std::nullopt;
        }
        int initialOffset = mCurrentRange.has_value()
                                ? mCurrentRange->size + mCurrentRange->offset
                                : 0;
        int addedSize = static_cast<int>(data.size() * sizeof(T));
        if (mCurrentRange.has_value()) {
            if (!mBufferHandle->Resize(mCurrentRange.value(),
                                       addedSize + initialOffset)) {
                LOG_ERROR("Too many data to fit in current data "
                          "buffer! This is not good.");
                return std::nullopt;
            }
        } else {
            mCurrentRange = mBufferHandle->Reserve(addedSize + initialOffset);
            if (!mCurrentRange.has_value()) {
                LOG_ERROR("Not enough space to reserve data in data "
                          "buffer! This is not good.");
                return std::nullopt;
            }
        }

        std::size_t firstIndex = mStorage.size();

        mStorage.insert(mStorage.end(), data.begin(), data.end());

        return Utils::SBufferIndexRange{.first = firstIndex,
                                        .count = data.size()};
    }

    bool Upload() override {
        if (mStorage.empty() || !mCurrentRange.has_value()) {
            LOG_INFO("No data to upload or range not prepared!");
            return true;
        }
        if (mIsUploaded) {
            LOG_ERROR("Data already uploaded, clear before uploading again.");
            return false;
        }
        if (!mBufferHandle->Fill(mStorage.data(),
                                 static_cast<int>(mStorage.size() * sizeof(T)),
                                 0, mCurrentRange.value())) {
            LOG_ERROR("Failed to upload data to buffer!");
            return false;
        }

#ifndef DEBUG_BUILD
        mStorage.clear();
#endif
        mIsUploaded = true;
        return true;
    }

    bool Clear() override {
        if (mCurrentRange.has_value()) {
            mBufferHandle->FreeRange(mCurrentRange.value());
            mCurrentRange = std::nullopt;
        }
        mStorage.clear();
        mIsUploaded = false;
        return true;
    }

    const std::vector<T>& GetData() const {
        return mStorage;
    }

private:
    CBufferHandle* mBufferHandle{nullptr};

    std::vector<T> mStorage;

    std::optional<Utils::SBufferRange> mCurrentRange{std::nullopt};

    bool mIsUploaded{false};
};
} // namespace Vulkan