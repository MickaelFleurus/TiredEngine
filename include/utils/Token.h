#pragma once
#include <memory>
#include <unordered_set>

class CToken {
public:
    class CTokenHandle {
    public:
        CTokenHandle(CToken& token) : m_Token(token) {
        }

        ~CTokenHandle() {
            if (m_IsValid) {
                m_Token.OnHandleDestroyed(*this);
            }
        }

        bool IsValid() const {
            return m_IsValid;
        }

    private:
        bool m_IsValid = true;
        CToken& m_Token;

        friend class CToken;
    };

    CToken() = default;
    ~CToken();
    CToken(const CToken&) = delete;
    CToken& operator=(const CToken&) = delete;
    CToken(CToken&&) = delete;
    CToken& operator=(CToken&&) = delete;

    std::unique_ptr<CTokenHandle> GetTokenHandle();

private:
    void OnHandleDestroyed(CTokenHandle& handle);
    std::unordered_set<CTokenHandle*> m_Handles;
};