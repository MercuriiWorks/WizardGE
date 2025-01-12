//
// Created by mecha on 08.06.2022.
//

#pragma once

#include <core/vector.h>
#include <core/core.h>
#include <scripting/Scriptable.h>
#include <ecs/Scene.h>
#include <platform/core/Window.h>

#include <string>

namespace engine::core {

    enum ProjectVersion {
        V_RELEASE, V_DEBUG, V_VISUAL, V_PROFILING
    };

    struct ENGINE_API ProjectProps {
        std::string title = "Untitled";
        std::string icon = "";
        std::string launcher = "";
        WindowProps windowProps = { "Untitled" };
        bool fullscreen = false;

        ProjectProps() = default;

        void serialize(YAML::Emitter &out) const;
        void deserialize(const YAML::Node &parent);
        void save(const char* filepath);
        static bool createFromFile(const char* filepath, ProjectProps& props);
    };

    struct ENGINE_API Project {
        std::string name;
        std::string workspacePath;
        ProjectProps props;

        Project() = default;
        Project(const std::string& name, const std::string& workspacePath)
        : name(name), workspacePath(workspacePath) {}

        [[nodiscard]] inline std::string getAssetsPath() const;
        [[nodiscard]] inline std::string getAudioPath() const;
        [[nodiscard]] inline std::string getMaterialsPath() const;
        [[nodiscard]] inline std::string getModelsPath() const;
        [[nodiscard]] inline std::string getFontsPath() const;
        [[nodiscard]] inline std::string getScenesPath() const;
        [[nodiscard]] inline std::string getScriptsPath() const;
        [[nodiscard]] inline std::string getCMakePath() const;
        [[nodiscard]] inline std::string getCoreShadersPath() const;
        [[nodiscard]] std::string getFullPath() const;
        [[nodiscard]] inline std::string getSlnPath() const;
        [[nodiscard]] inline std::string getExePath(ProjectVersion version) const;
        [[nodiscard]] inline std::string getBuildPath(ProjectVersion version) const;
        [[nodiscard]] inline std::string getZipPath(ProjectVersion version) const;
        [[nodiscard]] inline std::string getPropsPath() const;

    private:
        std::string getFullPath(const char* assetPath) const;
    };

    class ENGINE_API ProjectManager final {

    public:
        static Project* create(
                const char* projectName,
                const char* workspacePath
        );
        static bool destroy(const char* projectName);
        static void openSln(const char* projectName);
        static void openScriptsSln(const char* projectName);
        static void openScriptsSln(const Project& project);
        static void closeProject();

        inline static const Project* getCurrentProject();
        inline static void setCurrentProject(const char* projectName);
        inline static void setCurrentProject(Project& project);
        inline static const vector<Project>& getAll();

        static void cmake(const char* projectName, ProjectVersion projectVersion);
        static void cmake(const Project& project, ProjectVersion projectVersion);
        static void build(const char* projectName, ProjectVersion projectVersion);
        static void build(const Project& project, ProjectVersion projectVersion);
        static void run(const char* projectName, ProjectVersion projectVersion);
        static void run(const Project& project, ProjectVersion projectVersion);

        static void cmakeScripts(const char* projectName, ProjectVersion projectVersion);
        static void cmakeScripts(const Project& project, ProjectVersion projectVersion);

        static void buildScripts(const char* projectName, ProjectVersion projectVersion);
        static void buildScripts(const Project& project, ProjectVersion projectVersion);

        static void saveProjects();
        static void loadProjects();

        static void publish(const char* projectName, ProjectVersion projectVersion);
        static void publish(const Project& project, ProjectVersion projectVersion);

        static void loadScripts(const Project& project, ProjectVersion projectVersion);

        static void renameScript(const std::string& oldPath, const std::string& newName);

        static void newScript(const std::string& filePath, const std::string& name);

        static void saveScenes();
        static void loadScenes();

    private:
        static void postBuild(const Project& project, ProjectVersion projectVersion);
        static std::string getScriptEngineBuildPath(const Project& project, ProjectVersion projectVersion);
        static void postBuildScripts(const Project& project, ProjectVersion projectVersion);

    private:
        static vector<Project> projects;
        static Project* currentProject;
    };

}
