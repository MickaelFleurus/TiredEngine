#pragma once

#ifdef EDITOR_MODE
#include <string>
class CStringID {
public:
    explicit CStringID(std::string name) : mName(std::move(name)) {
    }

    const std::string& GetName() const {
        return mName;
    }
    void SetName(const std::string& name) {
        mName = name;
    }

private:
    std::string mName;
};

#else
#include "engine/utils/Hashing.h"
class CStringID {
public:
    explicit CStringID(const std::string& name)
        : mHash(Utils::CreateHash(name)) {
    }

    uint64_t GetHash() const {
        return mHash;
    }

private:
    const uint64_t mHash;
};
#endif
