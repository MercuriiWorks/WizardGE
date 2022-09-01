//
// Created by mecha on 01.09.2022.
//

#include <visual/Visual.h>
#include <visual/Theme.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <platform/graphics/tools/ShaderPath.h>
#include <imgui_internal.h>

#define IO ImGui::GetIO()
#define STYLE ImGui::GetStyle()
#define COLORS STYLE.Colors

namespace engine::visual {

    VisualProps Visual::props;
    vector<ImFont*> Visual::fonts;
    bool Visual::fullScreen;
    bool Visual::openDockspace;
    int Visual::windowFlags;
    int Visual::dockspaceFlags;

    void Visual::init(void* nativeWindow, const VisualProps& props) {
        Visual::props = props;
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            STYLE.WindowRounding = 0.0f;
            COLORS[ImGuiCol_WindowBg].w = 1.0f;
        }
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*) nativeWindow, true);
        ImGui_ImplOpenGL3_Init(GRAPHICS_CORE_VERSION);
    }

    u32 Visual::addFont(const char *filepath, f32 fontSize) {
        fonts.emplace_back(IO.Fonts->AddFontFromFileTTF(filepath, fontSize));
        return fonts.size() - 1;
    }

    void Visual::setDefaultFont(u32 fontIndex) {
        if (fontIndex >= fonts.size()) {
            ENGINE_ERR("Visual::setDefaultFont: invalid condition fontIndex > fonts.size()");
            return;
        }
        IO.FontDefault = fonts.at(fontIndex);
    }

    void Visual::setDefaultFont(const char *filepath, f32 fontSize) {
        setDefaultFont(addFont(filepath, fontSize));
    }

    void Visual::setTheme() {
        auto& style = ImGui::GetStyle();
        auto& colors = ImGui::GetStyle().Colors;

        //========================================================
        /// Colours

        // Headers
        colors[ImGuiCol_Header]				= ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
        colors[ImGuiCol_HeaderHovered]		= ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
        colors[ImGuiCol_HeaderActive]		= ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);

        // Buttons
        colors[ImGuiCol_Button]				= ImColor(56, 56, 56, 200);
        colors[ImGuiCol_ButtonHovered]		= ImColor(70, 70, 70, 255);
        colors[ImGuiCol_ButtonActive]		= ImColor(56, 56, 56, 150);

        // Frame BG
        colors[ImGuiCol_FrameBg]			= ImGui::ColorConvertU32ToFloat4(Theme::propertyField);
        colors[ImGuiCol_FrameBgHovered]		= ImGui::ColorConvertU32ToFloat4(Theme::propertyField);
        colors[ImGuiCol_FrameBgActive]		= ImGui::ColorConvertU32ToFloat4(Theme::propertyField);

        // Tabs
        colors[ImGuiCol_Tab]				= ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
        colors[ImGuiCol_TabHovered]			= ImColor(255, 225, 135, 30);
        colors[ImGuiCol_TabActive]			= ImColor(255, 225, 135, 60);
        colors[ImGuiCol_TabUnfocused]		= ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
        colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabHovered];

        // Title
        colors[ImGuiCol_TitleBg]			= ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
        colors[ImGuiCol_TitleBgActive]		= ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
        colors[ImGuiCol_TitleBgCollapsed]	= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Resize Grip
        colors[ImGuiCol_ResizeGrip]			= ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]	= ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]	= ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg]		= ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]		= ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

        // Check Mark
        colors[ImGuiCol_CheckMark]			= ImColor(200, 200, 200, 255);

        // Slider
        colors[ImGuiCol_SliderGrab]			= ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
        colors[ImGuiCol_SliderGrabActive]	= ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

        // Text
        colors[ImGuiCol_Text]				= ImGui::ColorConvertU32ToFloat4(Theme::text);

        // Checkbox
        colors[ImGuiCol_CheckMark]			= ImGui::ColorConvertU32ToFloat4(Theme::text);

        // Separator
        colors[ImGuiCol_Separator]			= ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);
        colors[ImGuiCol_SeparatorActive]	= ImGui::ColorConvertU32ToFloat4(Theme::highlight);
        colors[ImGuiCol_SeparatorHovered]	= ImColor(39, 185, 242, 150);

        // Window Background
        colors[ImGuiCol_WindowBg]			= ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
        colors[ImGuiCol_ChildBg]			= ImGui::ColorConvertU32ToFloat4(Theme::background);
        colors[ImGuiCol_PopupBg]			= ImGui::ColorConvertU32ToFloat4(Theme::backgroundPopup);
        colors[ImGuiCol_Border]				= ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);

        // Tables
        colors[ImGuiCol_TableHeaderBg]		= ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
        colors[ImGuiCol_TableBorderLight]	= ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);

        // Menubar
        colors[ImGuiCol_MenuBarBg]			= ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };

        //========================================================
        /// Style
        style.FrameRounding = 2.5f;
        style.FrameBorderSize = 1.0f;
        style.IndentSpacing = 11.0f;
    }

    void Visual::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Visual::end() {
        ENGINE_INFO("Visual::end(): width={0}, height={1}", props.width, props.height);
        IO.DisplaySize = ImVec2(props.width, props.height);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void Visual::release() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Visual::beginDockspace(const char* name) {
        if (!openDockspace) return;

        setDockspace();
        static bool open = openDockspace;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(name, &open, windowFlags);
        ImGui::PopStyleVar();

        if (fullScreen) {
            ImGui::PopStyleVar(2);
        }

        if (IO.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID(name);
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }
    }

    void Visual::setDockspace() {
        dockspaceFlags = ImGuiDockNodeFlags_None;
        windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        if (fullScreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        } else {
            dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
            windowFlags |= ImGuiWindowFlags_NoBackground;
        }
    }

    void Visual::endDockspace() {
        if (!openDockspace) return;
        ImGui::End();
    }

    void Visual::onKeyPressed(event::KeyCode keyCode) {
//        IO.KeysDown[keyCode] = true;
    }

    void Visual::onKeyHold(event::KeyCode keyCode) {
//        IO.KeysDown[keyCode] = true;
    }

    void Visual::onKeyReleased(event::KeyCode keyCode) {
//        IO.KeysDown[keyCode] = false;
    }

    void Visual::onKeyTyped(event::KeyCode keyCode) {
//        IO.AddInputCharacter(keyCode);
    }

    void Visual::onMouseScrolled(double xOffset, double yOffset) {
        IO.MouseWheelH += xOffset;
        IO.MouseWheel += yOffset;
    }

    void Visual::onMousePressed(event::MouseCode mouseCode) {
        IO.MouseDown[mouseCode] = true;
    }

    void Visual::onMouseRelease(event::MouseCode mouseCode) {
        IO.MouseDown[mouseCode] = false;
    }

    void Visual::onCursorMoved(double xPos, double yPos) {
        IO.MousePos = {(float) xPos, (float) yPos};
    }

    void Visual::onWindowClosed() {
        ENGINE_INFO("onWindowsClosed()");
    }

    void Visual::onWindowResized(const uint32_t &width, const uint32_t &height) {
        ENGINE_INFO("onWindowResized(width = {0}, height = {1})", width, height);
    }

    void Panel::begin(const char* title, const vec2f& size) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0 , 0});
        ImGui::SetNextWindowSize({ size.x(), size.y() });
        static bool open = true;
        if (!ImGui::Begin(title, &open)) {
            end();
            return;
        }
    }

    void Panel::end() {
        ImGui::End();
        ImGui::PopStyleVar();
    }

    bool Panel::isFocused(ImGuiFocusedFlags flags) {
        return ImGui::IsWindowFocused(flags);
    }
}