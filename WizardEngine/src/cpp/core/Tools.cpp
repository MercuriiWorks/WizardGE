//
// Created by mecha on 09.10.2021.
//

#include <tools/Tools.h>
#include <sstream>

namespace engine {

    void Tools::openVSCodeTask(const std::string &filePath) {
        EDITOR_INFO("openVSCodeTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "VSCode_Task",
                "VSCode_Thread",
                openVSCodeTask
        };
        task.run(filePath);
    }

    void Tools::openVSCode(const std::string &filePath) {
        EDITOR_INFO("openVSCode({0})", filePath);
        std::string command = "code " + filePath;
        system(command.c_str());
    }

    void Tools::openNotepadTask(const std::string &filePath) {
        EDITOR_INFO("openNotepadTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "VSCode_Task",
                "VSCode_Thread",
                openVSCodeTask
        };
        task.run(filePath);
    }

    void Tools::openNotepad(const std::string &filePath) {
        EDITOR_INFO("openNotepad({0})", filePath);
        std::string command = "notepad " + filePath;
        system(command.c_str());
    }

    void Tools::openVisualStudioTask(const std::string &filePath) {
        EDITOR_INFO("openVisualStudioTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "VSCode_Task",
                "VSCode_Thread",
                openVSCodeTask
        };
        task.run(filePath);
    }

    void Tools::openVisualStudio(const std::string &filePath) {
        EDITOR_INFO("openVisualStudio({0})", filePath);
        std::string command = "start devenv " + filePath;
        system(command.c_str());
    }

    void Tools::openPhotoshopTask(const std::string &filePath) {
        EDITOR_INFO("openPhotoshopTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "Photoshop_Task",
                "Photoshop_Thread",
                openPhotoshopTask
        };
        task.run(filePath);
    }

    void Tools::openBlenderTask(const std::string &filePath) {
        EDITOR_INFO("openBlenderTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "Blender_Task",
                "Blender_Thread",
                openBlenderTask
        };
        task.run(filePath);
    }

    void Tools::openZBrushTask(const std::string &filePath) {
        EDITOR_INFO("openZBrushTask({0})", filePath);
        VoidTask<const std::string&> task = {
                "ZBrush_Task",
                "ZBrush_Thread",
                openZBrushTask
        };
        task.run(filePath);
    }

    void Tools::msBuildTask(const std::string &slnPath) {
        EDITOR_INFO("msBuildTask({0})", slnPath);
        VoidTask<const std::string&> task = {
                "MSBuild_Task",
                "MSBuild_Thread",
                msBuildTask
        };
        task.run(slnPath);
    }

    void Tools::cmakeTask(const std::string &cmakePath) {
        EDITOR_INFO("cmakeTask({0})", cmakePath);
        VoidTask<const std::string&> task = {
                "CMake_Task",
                "CMake_Thread",
                cmakeTask
        };
        task.run(cmakePath);
    }

    void Tools::openPhotoshop(const std::string &filePath) {
        EDITOR_INFO("openPhotoshop({0})", filePath);
        std::string cmd = "start photoshop " + filePath;
        system(cmd.c_str());
    }

    void Tools::openBlender(const std::string &filePath) {
        EDITOR_INFO("openBlender({0})", filePath);
        std::string cmd = "blender " + filePath;
        system(cmd.c_str());
    }

    void Tools::openZBrush(const std::string &filePath) {
        EDITOR_INFO("openZBrush({0})", filePath);
        std::string cmd = "start zbrush " + filePath;
        system(cmd.c_str());
    }

    void Tools::msBuild(const std::string &slnPath) {
        EDITOR_INFO("msBuild({0})", slnPath);
        std::string cmd = "msbuild " + slnPath;
        system(cmd.c_str());
    }

    void Tools::cmake(const std::string &cmakePath) {
        EDITOR_INFO("cmake({0})", cmakePath);
        std::string cmd = "cmake " + cmakePath;
        system(cmd.c_str());
    }

    void Tools::cmake(const std::string &cmakePath, const std::vector<const char *>& definitions) {
        EDITOR_INFO("cmakeTask({0})", cmakePath);
        std::stringstream ss;
        ss << "cmake ";
        for (const char* d : definitions) {
            ss << "-D" << d << " ";
        }
        ss << cmakePath;
        system(ss.str().c_str());
    }

}