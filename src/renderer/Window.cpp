#include "renderer/Window.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_init.h"
#include <functional>

namespace {
constexpr int kWindowWidth = 1920;
constexpr int kWindowHeight = 1080;

constexpr auto kSDLWindowDeleter = [](SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
};

constexpr auto kSDLRendererDeleter = [](SDL_Renderer* renderer) {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
};
} // namespace

namespace Renderer {

class CWindow::CImpl {
public:
    CImpl()
        : m_SDLWindow(std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)>(
              SDL_CreateWindow("SDL3 window", kWindowWidth, kWindowHeight, 0),
              kSDLWindowDeleter))
        , m_SDLRenderer(
              std::unique_ptr<SDL_Renderer, decltype(kSDLRendererDeleter)>(
                  SDL_CreateRenderer(m_SDLWindow.get(), nullptr),
                  kSDLRendererDeleter)) {

        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    ~CImpl() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Render() {
        SDL_SetRenderDrawColor(m_SDLRenderer.get(), 0, 124, 0, 255);
        SDL_RenderClear(m_SDLRenderer.get());
        SDL_RenderPresent(m_SDLRenderer.get());
    }

private:
    std::unique_ptr<SDL_Window, decltype(kSDLWindowDeleter)> m_SDLWindow;
    std::unique_ptr<SDL_Renderer, decltype(kSDLRendererDeleter)> m_SDLRenderer;
};

CWindow::CWindow() : m_Impl(std::make_unique<CWindow::CImpl>()) {
}

CWindow::~CWindow() = default;

void CWindow::Render() {
    m_Impl->Render();
}

} // namespace Renderer