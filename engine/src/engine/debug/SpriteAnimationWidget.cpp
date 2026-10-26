#include "engine/debug/SpriteAnimationWidget.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include <imgui.h>

#include "engine/debug/FileExplorer.h"
#include "engine/utils/Logger.h"

namespace {

struct SSpriteRect {
    ImVec2 uv0, uv1;
}; // one sprite in the sheet

struct SClipFrame {
    int sprite = 0;
    float duration = 0.1f;
}; // duration in seconds

enum class ELoopMode { Once, Loop, PingPong };

struct SAnimClip {
    std::string name;
    std::vector<SClipFrame> frames;
    ELoopMode loop = ELoopMode::Loop;
};

struct SState {
    int selected = -1;
};

// Wrapped grid of sprite thumbnails. Click returns the sprite index (else -1).
// Each thumbnail is also a drag source ("SPRITE" payload) for the clip strip.
int SpritePalette(const std::vector<SSpriteRect>& sprites, ImTextureID tex,
                  float thumb = 48.f) {
    int clicked = -1;
    const ImGuiStyle& style = ImGui::GetStyle();
    const float xLimit =
        ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;

    for (int i = 0; i < (int)sprites.size(); ++i) {
        ImGui::PushID(i);
        if (ImGui::ImageButton("##s", tex, ImVec2(thumb, thumb), sprites[i].uv0,
                               sprites[i].uv1))
            clicked = i;
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("SPRITE", &i, sizeof(int));
            ImGui::Image(tex, ImVec2(thumb * 0.5f, thumb * 0.5f),
                         sprites[i].uv0, sprites[i].uv1);
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();

        const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x +
                            ImGui::GetItemRectSize().x;
        if (i + 1 < (int)sprites.size() && nextX < xLimit)
            ImGui::SameLine();
    }
    return clicked;
}

// Returns true when the clip was modified.
bool ClipEditor(SAnimClip& clip, const std::vector<SSpriteRect>& sprites,
                ImTextureID tex, SState& st, float thumb = 64.f) {
    bool changed = false;
    int removeIdx = -1, moveFrom = -1, moveTo = -1, insertAt = -1,
        insertSprite = -1;
    const ImVec2 thumbSize(thumb, thumb);
    const int count = (int)clip.frames.size();

    ImGui::BeginChild("##strip", ImVec2(0, thumb + 84.f),
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_HorizontalScrollbar);

    for (int i = 0; i < count; ++i) {
        SClipFrame& f = clip.frames[i];
        const SSpriteRect& s = sprites[f.sprite];

        ImGui::PushID(i);
        ImGui::BeginGroup();

        const bool sel = (st.selected == i);
        if (sel)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(
                                                       ImGuiCol_ButtonActive));
        if (ImGui::ImageButton("##f", tex, thumbSize, s.uv0, s.uv1))
            st.selected = i;
        if (sel)
            ImGui::PopStyleColor();
        const float cellW = ImGui::GetItemRectSize().x;

        // Drag source: reorder
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("CLIP_FRAME", &i, sizeof(int));
            ImGui::Image(tex, ImVec2(thumb * 0.5f, thumb * 0.5f), s.uv0, s.uv1);
            ImGui::EndDragDropSource();
        }
        // Drop target: reorder (frame takes this slot) or insert a sprite
        // before this frame
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* p =
                    ImGui::AcceptDragDropPayload("CLIP_FRAME")) {
                moveFrom = *(const int*)p->Data;
                moveTo = i;
            }
            if (const ImGuiPayload* p =
                    ImGui::AcceptDragDropPayload("SPRITE")) {
                insertSprite = *(const int*)p->Data;
                insertAt = i;
            }
            ImGui::EndDragDropTarget();
        }

        // Duration (seconds)
        ImGui::SetNextItemWidth(cellW);
        if (ImGui::DragFloat("##d", &f.duration, 0.001f, 0.005f, 10.f, "%.3fs",
                             ImGuiSliderFlags_AlwaysClamp))
            changed = true;

        ImGui::Text("#%d", i);
        ImGui::SameLine();
        if (ImGui::SmallButton("x"))
            removeIdx = i;

        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SameLine();
    }

    // Trailing "+" slot: click for a sprite picker, or drop a sprite on it to
    // append
    ImGui::PushID("add");
    if (ImGui::Button("+", ImVec2(thumb, thumb)))
        ImGui::OpenPopup("pick_sprite");
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("SPRITE")) {
            insertSprite = *(const int*)p->Data;
            insertAt = count;
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::BeginPopup("pick_sprite")) {
        const int c = SpritePalette(sprites, tex);
        if (c >= 0) {
            insertSprite = c;
            insertAt = count;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();

    // Delete key removes the selected frame
    if (st.selected >= 0 && st.selected < count && ImGui::IsWindowFocused() &&
        ImGui::IsKeyPressed(ImGuiKey_Delete))
        removeIdx = st.selected;

    ImGui::EndChild();

    // Apply deferred edits (never mutate the vector while iterating it)
    auto& fr = clip.frames;
    if (removeIdx >= 0) {
        fr.erase(fr.begin() + removeIdx);
        st.selected = std::min(st.selected, (int)fr.size() - 1);
        changed = true;
    } else if (moveFrom >= 0 && moveFrom != moveTo) {
        // Dragged frame ends up at the target's index
        SClipFrame tmp = fr[moveFrom];
        fr.erase(fr.begin() + moveFrom);
        fr.insert(fr.begin() + moveTo, tmp);
        st.selected = moveTo;
        changed = true;
    } else if (insertSprite >= 0) {
        SClipFrame nf;
        nf.sprite = insertSprite;
        nf.duration = fr.empty()
                          ? 0.1f
                          : fr.back().duration; // inherit neighbour's timing
        fr.insert(fr.begin() + insertAt, nf);
        st.selected = insertAt;
        changed = true;
    }
    return changed;
}

// Which frame index is showing at time t (seconds since playback start). -1 if
// empty.
int SampleClip(const SAnimClip& c, float t) {
    if (c.frames.empty())
        return -1;
    float total = 0.f;
    for (const SClipFrame& f : c.frames)
        total += f.duration;
    if (total <= 0.f)
        return 0;

    switch (c.loop) {
    case ELoopMode::Once:
        if (t >= total)
            return (int)c.frames.size() - 1;
        break;
    case ELoopMode::Loop:
        t = std::fmod(t, total);
        break;
    case ELoopMode::PingPong: {
        const float period = total * 2.f;
        t = std::fmod(t, period);
        if (t > total)
            t = period - t;
        break;
    }
    }
    for (int i = 0; i < (int)c.frames.size(); ++i) {
        if (t < c.frames[i].duration)
            return i;
        t -= c.frames[i].duration;
    }
    return (int)c.frames.size() - 1;
}

// Draws the clip at time t. Call with an accumulating timer; reset it to 0 to
// restart.
void Preview(const SAnimClip& c, const std::vector<SSpriteRect>& sprites,
             ImTextureID tex, float t, float size = 128.f) {
    const int idx = SampleClip(c, t);
    if (idx < 0) {
        ImGui::Dummy(ImVec2(size, size));
        return;
    }
    const SSpriteRect& s = sprites[c.frames[idx].sprite];
    ImGui::Image(tex, ImVec2(size, size), s.uv0, s.uv1);
}

} // namespace

namespace Debug {
CSpriteAnimationWidget::CSpriteAnimationWidget(
    Renderer::CSpriteManager& spriteManager,
    Renderer::CTextureManager& textureManager, SDL_Window* window)
    : mSpriteManager(spriteManager)
    , mTextureManager(textureManager)
    , mWindow(window) {
}

CSpriteAnimationWidget::~CSpriteAnimationWidget() {
}

void CSpriteAnimationWidget::Render() {
    if (!ImGui::Begin("Sprite Animation", &mVisible)) {
        ImGui::End();
        return;
    }
    if (ImGui::Button("Create")) {
        auto blip = Debug::SaveFile(mWindow);
        LOG_TRACE("Creating file {}...", blip->string());
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
    }
    // previewTime += ImGui::GetIO().DeltaTime;

    // if (ClipEditor(clip, sprites, sheetTex, clipSState)) {
    //     // clip was modified: mark it dirty / save it
    // }

    // Preview(clip, sprites, sheetTex, previewTime, 128.f);

    // ImGui::Separator();
    // SpritePalette(sprites, sheetTex); //
    ImGui::End();
}

const char* CSpriteAnimationWidget::GetName() const {
    return "SpriteAnimation";
}
} // namespace Debug
