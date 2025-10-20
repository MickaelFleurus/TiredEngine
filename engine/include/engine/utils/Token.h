#pragma once
#include <memory>
#include <unordered_set>

class CToken {
public:
    class CTokenHandle {
    public:
        CTokenHandle(CToken& token) : mToken(token) {
        }

        ~CTokenHandle() {
            if (mIsValid) {
                mToken.OnHandleDestroyed(*this);
            }
        }

        bool IsValid() const {
            return mIsValid;
        }

    private:
        bool mIsValid = true;
        CToken& mToken;

        friend class CToken;
    };

    CToken() = default;
    ~CToken();
    CToken(const CToken&) = delete;
    CToken& operator=(const CToken&) = delete;
    CToken(CToken&&) = delete;
    CToken& operator=(CToken&&) = delete;

    void Reset();
    std::unique_ptr<CTokenHandle> GetTokenHandle();

private:
    void OnHandleDestroyed(CTokenHandle& handle);
    std::unordered_set<CTokenHandle*> mHandles;
};