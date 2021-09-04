//
// Created by mecha on 29.08.2021.
//

#include "Logger.h"

namespace engine {

    Ref<spdlog::logger> Logger::_clientLogger;
    Ref<spdlog::logger> Logger::_engineLogger;

    void Logger::createEngineLogger(const std::string &name) {
        _engineLogger = createRef<spdlog::logger>("Engine");
        spdlog::register_logger(_engineLogger);
        _engineLogger->set_level(spdlog::level::trace);
        _engineLogger->flush_on(spdlog::level::trace);
    }

    void Logger::createClientLogger(const std::string &name) {
        _clientLogger = createRef<spdlog::logger>("Client");
        spdlog::register_logger(_clientLogger);
        _clientLogger->set_level(spdlog::level::trace);
        _clientLogger->flush_on(spdlog::level::trace);
    }

    void Logger::setPattern(const std::string& pattern) {
        spdlog::set_pattern(pattern);
    }

}