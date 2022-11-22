//
// Created by mecha on 04.10.2022.
//

#include <visual/ProjectsPanel.h>
#include <visual/Visual.h>
#include <visual/Theme.h>
#include <visual/FontAwesome4.h>
#include <visual/AssetBrowser.h>

#include <core/Application.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

namespace engine::visual {

    ProjectOptionTab ProjectsPanel::tab = TAB_NEW_PROJECT;
    std::string ProjectsPanel::newProjectName = "Untitled";
    std::string ProjectsPanel::newWorkspace = "projects";
    Project ProjectsPanel::selectedProject;
    bool ProjectsPanel::isProjectSelected = false;

    void ProjectsPanel::draw(const char *title, const vec2f &size, const std::function<void(const Project&)>& onProjectOpen) {
        static bool open = true;
        if (!ImGui::Begin(ICON_FA_MAGIC" Projects", &open)) {
            ImGui::End();
            return;
        }

        ImGui::Columns(2);
        ImGui::SetColumnOffset(1, 230);

        // Left side
        {

            static ImVec4 active = { 0 / 255.0, 0 / 255.0, 0 / 255.0, 255.0 / 255.0 };
            static ImVec4 inactive = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 255 / 255.0 };

            ImGui::PushStyleColor(ImGuiCol_Text, tab == TAB_NEW_PROJECT ? inactive : active);
            ImGui::PushStyleColor(ImGuiCol_Button, tab == TAB_NEW_PROJECT ? active : inactive);
            if (Button::textButton(ICON_FA_MAGIC" New Project", { 230 - 15, 41 }, 4)) {
                tab = TAB_NEW_PROJECT;
            }

            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, tab == TAB_EXISTING_PROJECTS ? inactive : active);
            ImGui::PushStyleColor(ImGuiCol_Button, tab == TAB_EXISTING_PROJECTS ? active : inactive);
            if (Button::textButton(ICON_FA_LIST" Projects", { 230 - 15, 41 })) {
                tab = TAB_EXISTING_PROJECTS;
            }

            ImGui::PopStyleColor(4);

            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30);

            Text::centered(ICON_FA_QUESTION_CIRCLE" Version 1.0.0", 230, 1, false);
        }

        ImGui::NextColumn();

        // Right side
        {
            // New Project Tab
            if (tab == TAB_NEW_PROJECT) {
                isProjectSelected = false;
                ImGui::InvisibleButton("##padding-title-top", { 240, 4 });
                Text::field("##project_name", "Title " ICON_FA_STAR, &newProjectName, { 2, 2 }, false);
                Text::field("##project_workspace", "Workspace " ICON_FA_STAR, &newWorkspace, { 2, 2 }, false);
                ImGui::InvisibleButton("##padding-workspace-bottom", { 240, 2 });

                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
                ImVec2 contentRegion = ImGui::GetContentRegionAvail();
                contentRegion.y -= 35;
                if (ImGui::ListBoxHeader("##NewProject_ProjectList", contentRegion)) {
                    for (const Project& project : ProjectManager::getAll()) {
                        if (ImGui::TreeNode(project.name.c_str())) {
                            ImGui::TreePop();
                        }
                    }
                    ImGui::ListBoxFooter();
                }
                ImGui::PopStyleColor();

                if (Button::textButton(ICON_FA_MAGIC" Create", { 205, 34 }, 2, 2)) {
                    ProjectManager::create(newProjectName.c_str(), newWorkspace.c_str());
                    ProjectManager::saveProjects();
                }
                ImGui::Spacing();
            }
            // Existing Projects Tab
            if (tab == TAB_EXISTING_PROJECTS) {
                static ImGuiTextFilter filter;
                ImGui::InvisibleButton("##padding-search-top", { 240, 4 });
                filter.Draw(ICON_FA_SEARCH" Search", 240);
                ImGui::InvisibleButton("##padding-search-bottom", { 240, 2 });

                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
                ImVec2 contentRegion = ImGui::GetContentRegionAvail();
                contentRegion.y -= 35;
                if (ImGui::ListBoxHeader("##Projects_ProjectList", contentRegion)) {
                    for (const Project& project : ProjectManager::getAll()) {
                        if (filter.PassFilter(project.name.c_str())) {
                            if (ImGui::TreeNode(project.name.c_str())) {
                                selectedProject = project;
                                isProjectSelected = true;
                                ImGui::TreePop();
                            }
                        }
                    }
                    ImGui::ListBoxFooter();
                }
                ImGui::PopStyleColor();

                if (isProjectSelected) {
                    ImGui::InvisibleButton("padding-open-top", { 205, 2 });
                    ImGui::SameLine();
                    ImGui::InvisibleButton("padding-delete-top", { 205, 2 });
                    ImGui::SameLine();
                    ImGui::InvisibleButton("padding-publish-top", { 205, 2 });

                    if (ImGui::Button(ICON_FA_FOLDER_OPEN" Open", { 205, 34 })) {
                        selectedProject.loadScenes();
                        Application::get().pushScenes(selectedProject.scenes);
                        ProjectManager::setCurrentProject(selectedProject);
                        AssetBrowser::setProject(selectedProject);
                        onProjectOpen(selectedProject);
                    }

                    Visual::sameLine();
                    if (ImGui::Button(ICON_FA_XING" Delete", { 205, 34 })) {
                        ProjectManager::destroy(selectedProject.name.c_str());
                        ProjectManager::saveProjects();
                    }

                    Visual::sameLine();
                    if (ImGui::Button(ICON_FA_NEWSPAPER_O" Publish", { 205, 34 })) {
                        ProjectManager::build(selectedProject, ProjectVersion::V_DEBUG);
                        ProjectManager::publish(selectedProject, ProjectVersion::V_DEBUG);
                    }

                    ImGui::InvisibleButton("padding-open-bottom", { 205, 2 });
                    ImGui::SameLine();
                    ImGui::InvisibleButton("padding-delete-bottom", { 205, 2 });
                    ImGui::SameLine();
                    ImGui::InvisibleButton("padding-publish-bottom", { 205, 2 });
                }
            }
        }

        ImGui::End();
    }

    void ProjectsPanel::setTheme() {
//        ImGuiStyle* style = &ImGui::GetStyle();
//
//        style->WindowBorderSize = 0;
//        style->WindowTitleAlign = ImVec2(0.5, 0.5);
//        style->WindowMinSize = ImVec2(20, 20);
//
//        style->FramePadding = ImVec2(8, 2);
//
//        static ImVec4 active = { 0 / 255.0, 0 / 255.0, 0 / 255.0, 255.0 / 255.0 };
//        static ImVec4 inactive = { 255 / 255.0, 255 / 255.0, 255 / 255.0, 255 / 255.0 };
//
//        style->Colors[ImGuiCol_TitleBg] = active;
//        style->Colors[ImGuiCol_TitleBgActive] = active;
//        style->Colors[ImGuiCol_TitleBgCollapsed] = inactive;
//        style->Colors[ImGuiCol_TextSelectedBg] = inactive;
//
//        style->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
//        style->Colors[ImGuiCol_ButtonActive] = ImColor(41, 40, 41, 255);
//        style->Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);
//
//        style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
//        style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
//        style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);
//
//        style->Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
//        style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
//        style->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);
//
//        style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
//        style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
//        style->Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);
    }

    void ProjectsPanel::init() {
        ProjectManager::loadProjects();
        setTheme();
    }

    void ProjectsPanel::destroy() {
        ProjectManager::saveProjects();
    }

}