#pragma once

#ifdef EDITOR_MODE
#include <string>

class CStringId {
public:
    explicit CStringId() = default;
    explicit CStringId(std::string name) : mName(std::move(name)) {
    }

    const std::string& GetName() const {
        return mName;
    }

    void SetName(const std::string& name) {
        mName = name;
    }

    CStringId operator+(const std::string& append) {
        return CStringId{mName + append};
    }

    CStringId operator+(const CStringId& append) {
        return CStringId{mName + append.mName};
    }

    bool operator==(const CStringId& other) const {
        return mName == other.mName;
    }

private:
    std::string mName;
};

#else
#include "engine/utils/Hashing.h"
class CStringId {
public:
    explicit CStringId() = default;
    explicit CStringId(const std::string& name)
        : mHash(Utils::CreateHash(name)) {
    }

    uint64_t GetHash() const {
        return mHash;
    }

    CStringId operator+(const std::string& append) {
        const auto hashAppend = Utils::CreateHash(append);
        mHash += hashAppend;
        return CStringId{Utils::CreateHash(mHash)};
    }

    CStringId operator+(const CStringId& append) {
        return CStringId{Utils::CreateHash(mHash + append.mHash)};
    }

    bool operator==(const CStringId& other) const {
        return mHash == other.mHash;
    }

private:
    const uint64_t mHash;
};
#endif
