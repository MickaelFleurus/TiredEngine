#include "engine/debug/FileExplorer.h"

#include <cstdint>
#include <expected>
#include <limits>
#include <optional>

#include <SDL3/SDL.h>

namespace {
struct SDialogResult {
    std::vector<std::filesystem::path> paths;
    std::optional<std::string> error;
};

Uint32 gEventId{std::numeric_limits<uint32_t>::max()};

void SDLCALL OnDialogDone(void* /*userdata*/, const char* const* filelist,
                          int filter) {
    if (gEventId == std::numeric_limits<Uint32>::max()) {
        gEventId = SDL_RegisterEvents(1);
    }

    auto* res = new SDialogResult();

    if (!filelist) {
        res->error = SDL_GetError();
    } else if (*filelist) {
        for (const char* const* p = filelist; *p; ++p)
            res->paths.emplace_back(*p);
    }

    SDL_Event e;
    SDL_zero(e);
    e.type = gEventId;
    e.user.data1 = res;
    SDL_PushEvent(&e); // thread-safe
}

Debug::ExpectedFile WaitForDialogFile() {
    bool quitRequested = false;
    SDL_Event e;

    while (SDL_WaitEvent(&e)) { // blocks without burning CPU
        if (e.type == gEventId) {
            std::unique_ptr<SDialogResult> res(
                static_cast<SDialogResult*>(e.user.data1));

            if (quitRequested) { // don't swallow a quit that came in meanwhile
                SDL_Event q;
                SDL_zero(q);
                q.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&q);
            }
            if (res->error.has_value()) {
                return std::unexpected<std::string>(*res->error);
            }
            if (res->paths.empty()) {
                return std::unexpected<std::string>("No file selected");
            }
            return res->paths.front();
        }
        if (e.type == SDL_EVENT_QUIT)
            quitRequested = true;
    }
    return std::unexpected<std::string>("Not sure how that would happen");
}

Debug::ExpectedFiles WaitForDialogFiles() {
    bool quitRequested = false;
    SDL_Event e;

    while (SDL_WaitEvent(&e)) { // blocks without burning CPU
        if (e.type == gEventId) {
            std::unique_ptr<SDialogResult> res(
                static_cast<SDialogResult*>(e.user.data1));

            if (quitRequested) { // don't swallow a quit that came in meanwhile
                SDL_Event q;
                SDL_zero(q);
                q.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&q);
            }
            if (res->error.has_value()) {
                return std::unexpected<std::string>(*res->error);
            }
            if (res->paths.empty()) {
                return std::unexpected<std::string>("No file selected");
            }
            return res->paths;
        }
        if (e.type == SDL_EVENT_QUIT)
            quitRequested = true;
    }
    return std::unexpected<std::string>("Not sure how that would happen");
}

SDL_DialogFileFilter GetTypeFilter(Debug::EFileType type) {
    switch (type) {
    case Debug::EFileType::Image:
        return {"Images", "png;jpg;bmp"};

    case Debug::EFileType::Scenes:
        return {"Scene file", "scene"};

    case Debug::EFileType::Sound:
        return {"Sound", "mp3;ogg"};

    case Debug::EFileType::SpriteAnimations:
        return {"Sprite Animation", "spriteanim"};
    }
    return {};
}

} // namespace

namespace Debug {
ExpectedFile SelectFile(std::span<EFileType> types, SDL_Window* window) {
    std::vector<SDL_DialogFileFilter> filters;
    for (const auto& type : types) {
        filters.push_back(GetTypeFilter(type));
    }
    SDL_ShowOpenFileDialog(OnDialogDone, nullptr, window, filters.data(),
                           types.size(), nullptr, false);
    return WaitForDialogFile();
}

ExpectedFiles SelectFiles(std::span<EFileType> types, SDL_Window* window) {
    std::vector<SDL_DialogFileFilter> filters;
    for (const auto& type : types) {
        filters.push_back(GetTypeFilter(type));
    }
    SDL_ShowOpenFileDialog(OnDialogDone, nullptr, window, filters.data(),
                           types.size(), nullptr, true);
    return WaitForDialogFiles();
}

ExpectedFile SaveFile(SDL_Window* window) {
    SDL_ShowSaveFileDialog(OnDialogDone, nullptr, window, nullptr, 0, nullptr);
    return WaitForDialogFile();
}
} // namespace Debug
