#include "engine/debug/Overlord.h"

#include <chrono>

#include <SDL3/SDL_events.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/debug/IOverlordItem.h"
#include "engine/vulkan/VulkanContext.h"
#include "engine/vulkan/VulkanRendering.h"

namespace Debug {
CGuardedContainer<IOverlordItem> COverlord::mWidgets;
CGuardedContainer<IOverlordItem> COverlord::mMenus;

COverlord::COverlord(const Vulkan::CVulkanContext& context,
                     const Vulkan::CVulkanRendering& rendering)
    : mContext(context), mRendering(rendering) {
}

void COverlord::Initialize(SDL_Window* window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    vkCreateDescriptorPool(mContext.GetDevice(), &pool_info, nullptr,
                           &mImguiPool);

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForVulkan(window);
    ImGui_ImplVulkan_InitInfo info{};
    info.Device = mContext.GetDevice();
    info.Instance = mContext.GetInstance();
    info.PhysicalDevice = mContext.GetPhysicalDevice();
    info.Queue = mContext.GetGraphicsQueue();
    info.QueueFamily = mContext.GetGraphicsQueueFamilyIndex();
    info.DescriptorPool = mImguiPool;
    info.MinImageCount = 2;
    info.ImageCount = static_cast<uint32_t>(mContext.GetImageCount());
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.RenderPass = mContext.GetRenderPass();
    info.Allocator = nullptr;
    info.CheckVkResultFn = nullptr;
    info.PipelineCache = VK_NULL_HANDLE;
    info.Subpass = 0;

    ImGui_ImplVulkan_Init(&info);
}

COverlord::~COverlord() {

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(mContext.GetDevice(), mImguiPool, nullptr);
}

bool COverlord::PrepareRender(SDL_Window* window) {
    ImGuiIO& io = ImGui::GetIO();
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    if (width <= 0 || height <= 0) {
        return false;
    }
    io.DisplaySize =
        ImVec2(static_cast<float>(width), static_cast<float>(height));

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    // Update FPS counter
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    mFrameTime =
        std::chrono::duration<float, std::chrono::milliseconds::period>(
            now - lastTime)
            .count();
    lastTime = now;

    mFPSUpdateTime += mFrameTime;
    if (mFPSUpdateTime >= 1000.0f) { // Update every second
        mFPS = static_cast<int>(1000.0f / mFrameTime);
        mFPSUpdateTime = 0.0f;
    }

    RenderMenuBar();
    RenderWidgets();
    ImGui::Render();
    return true;
}

void COverlord::Render(VkCommandBuffer cmd, VkPipeline pipeline) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd, pipeline);
}

void COverlord::AddWidget(IOverlordItem& item, CToken& token) {
    mWidgets.Add(item, token);
}

void COverlord::AddMenu(IOverlordItem& item, CToken& token) {
    mMenus.Add(item, token);
}

void COverlord::HandleEvent(const SDL_Event* e) {
    ImGui_ImplSDL3_ProcessEvent(e);
}

void COverlord::RenderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        for (auto& menu : mMenus) {
            if (menu.IsVisible()) {
                menu.Render();
            }
        }

        if (ImGui::BeginMenu("Widgets")) {
            for (auto& widget : mWidgets) {
                if (ImGui::MenuItem(widget.GetName(), nullptr,
                                    widget.IsVisible())) {
                    widget.ToggleVisible();
                }
            }
            ImGui::EndMenu();
        }
        // Add FPS display
        ImGui::SameLine(ImGui::GetWindowWidth() - 120.0f);
        ImGui::Text("FPS: %d (%.2f ms)", mFPS, mFrameTime);

        ImGui::EndMainMenuBar();
    }
}

void COverlord::RenderWidgets() {
    for (auto& item : mWidgets) {
        if (item.IsVisible()) {
            item.Render();
        }
    }
}

} // namespace Debug
