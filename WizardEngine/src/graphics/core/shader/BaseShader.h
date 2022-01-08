//
// Created by mecha on 17.11.2021.
//

#pragma once

#include "ShaderData.h"
#include "../io/ShaderFile.h"
#include "../../../ecs/Entity.h"

#include "../../../platform/includes/graphics/ubo.h"
#include "../../../platform/includes/graphics/shader.h"

namespace engine::shader {

    class BaseShader;

    struct ShaderScript {
        static uint16_t IDS;
        uint16_t id = IDS++;
        std::function<void(const BaseShader&, entt::registry&)> updateRegistry;
        std::function<void(const BaseShader&, const Entity&)> updateEntity;

        ShaderScript() = default;
        ShaderScript(const ShaderScript&) = default;

        bool operator==(const ShaderScript& other) const {
            return id == other.id;
        }
    };

    class BaseShader final : public Shader {

    public:
        BaseShader() : Shader() {}
        BaseShader(const std::initializer_list<ShaderScript> &scripts): scripts(scripts) {}
        BaseShader(const char* src) : Shader(src) {}
        BaseShader(const char* src, const std::initializer_list<ShaderScript> &scripts)
        : Shader(src), scripts(scripts) {}
        ~BaseShader() = default;

    public:
        [[nodiscard]] inline const std::string& getSrc() const {
            return src;
        }

        inline void setUniformBlockFormat(const UniformBlockFormat& uniformBlockFormat) {
            uniformBuffer.setUniformBlockFormat(uniformBlockFormat);
        }

        inline void bindUniformBlockFormat() {
            const auto& uniformBlockFormat = uniformBuffer.getUniformBlockFormat();
            bindUbf(uniformBlockFormat.getName().data(), uniformBlockFormat.getId());
        }

        void applyUbf(const UniformBlockFormat& uniformBlockFormat);

        void updateUniformBuffer(const UniformData &uniformData) const;
        void updateUniformBuffer(IntUniform &uniform, const uint32_t &index = 0) const;
        void updateUniformBuffer(FloatUniform &uniform, const uint32_t &index = 0) const;
        void updateUniformBuffer(BoolUniform &uniform, const uint32_t &index = 0) const;
        void updateUniformBuffer(Vec3fUniform &uniform, const uint32_t &index = 0) const;
        void updateUniformBuffer(Vec4fUniform &uniform, const uint32_t &index = 0) const;
        void updateUniformBuffer(Mat4fUniform &uniform, const uint32_t &index = 0) const;

        void updateScripts(const Entity &entity) const;
        void updateScripts(entt::registry &registry) const;
        void addScript(const ShaderScript& script);
        void removeScript(const ShaderScript& script);
        void clearScripts();
        void release();

    protected:
        std::vector<ShaderScript> scripts;
        UniformBuffer uniformBuffer;

    };

    class BaseShaderProgram final : public ShaderProgram {

    public:
        // with using default constructor, you should call construct() function manually!
        BaseShaderProgram() = default;

        BaseShaderProgram(const ShaderProps &props) {
            construct(props);
        }

        BaseShaderProgram(
                const ShaderProps &props,
                const BaseShader &vShader,
                const BaseShader &fShader
        ) : _vShader(vShader), _fShader(fShader) {
            construct(props);
        }

        ~BaseShaderProgram() = default;

    public:
        [[nodiscard]] inline const VertexFormat& getVertexFormat() const {
            return vertexFormat;
        }

        [[nodiscard]] inline const BaseShader& getVShader() const {
            return _vShader;
        }

        [[nodiscard]] inline const BaseShader& getFShader() const {
            return _fShader;
        }

    public:
        void construct(const ShaderProps& props);
        void detachShaders();
        void releaseShaders();
        bool invalidate();
        void bindVertexFormat();
        void parseVertexFormat();
        void parseUniformBlockFormat(BaseShader& shader);
        void update(entt::registry& registry);
        void update(const Entity& entity);
        void release();

    private:
        static ElementCount toElementCount(const std::string &elementCountStr);

    private:
        BaseShader _vShader; // Vertex Shader
        BaseShader _fShader; // Fragment/Pixel Shader
        VertexFormat vertexFormat;
        uint32_t uniformBlockSlots = 0;

    };

}