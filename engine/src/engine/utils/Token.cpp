#include "engine/utils/Token.h"

CToken::~CToken() {
    for (auto* handle : mHandles) {
        handle->mIsValid = false;
    }
}

std::unique_ptr<CToken::CTokenHandle> CToken::GetTokenHandle() {
    auto handle = std::make_unique<CTokenHandle>(*this);
    mHandles.insert(handle.get());
    return handle;
}

void CToken::OnHandleDestroyed(CTokenHandle& handle) {
    mHandles.erase(&handle);
}

void CToken::Reset() {
    for (auto* handle : mHandles) {
        handle->mIsValid = false;
    }
    mHandles.clear();
}