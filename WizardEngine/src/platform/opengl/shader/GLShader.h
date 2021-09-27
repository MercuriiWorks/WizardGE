//
// Created by mecha on 05.09.2021.
//

#pragma once

#include "../../../graphics/shader/Shader.h"
#include "../../../core/Assert.h"

#include "unordered_map"

#include "glad/glad.h"

namespace engine {

    class GLShader : public Shader {

    public:
        GLShader(const ShaderProps& shaderProps, VertexFormat* vertexFormat) : Shader(shaderProps, vertexFormat) {
            onCreate();
        }

        ~GLShader() override {
            onDestroy();
        }

    public:
        void start() override;
        void stop() override;

        void bindAttributes() override;

        void setUniform(FloatUniform &uniform) override;
        void setUniform(BoolUniform &uniform) override;
        void setUniform(IntUniform &uniform) override;
        void setUniform(DoubleUniform &uniform) override;

        void setUniform(Vec2fUniform &uniform) override;
        void setUniform(Vec3fUniform &uniform) override;
        void setUniform(Vec4fUniform &uniform) override;

        void setUniform(Mat2fUniform &uniform) override;
        void setUniform(Mat3fUniform &uniform) override;
        void setUniform(Mat4fUniform &uniform) override;

    private:
        static std::string toStringShaderType(GLenum type);

    private:
        void onCreate();
        void onDestroy();
        void compile();
        void detachShaders();
        void deleteShaders();

    protected:
        const char* getExtensionName() const override;

    private:
        std::unordered_map<GLenum, std::string> _typeSources;
        std::vector<GLenum> _shaderIds;

    };

}