#include "utils/Token.h"

CToken::~CToken() {
    for (auto* handle : m_Handles) {
        handle->m_IsValid = false;
    }
}

std::unique_ptr<CToken::CTokenHandle> CToken::GetTokenHandle() {
    auto handle = std::make_unique<CTokenHandle>(*this);
    m_Handles.insert(handle.get());
    return handle;
}

void CToken::OnHandleDestroyed(CTokenHandle& handle) {
    m_Handles.erase(&handle);
}