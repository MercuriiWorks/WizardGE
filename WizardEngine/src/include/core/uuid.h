//
// Created by mecha on 31.05.2022.
//

#pragma once

#include <xhash>
#include <core/primitives.h>
#include <core/core.h>

namespace engine {

    using namespace core;
    // "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
    // used to "uniquely" identify information.
    class ENGINE_API uuid {

    public:
        uuid();
        uuid(int uuid);
        uuid(const uuid& other);

    public:
        operator int() { return _uuid; }
        operator const int() const { return _uuid; }
        bool operator ==(int other) const { return _uuid == other; }

    private:
        int _uuid;
    };
}

namespace std {

    template <>
    struct hash<engine::uuid> {
        std::size_t operator()(const engine::uuid& uuid) const {
            return hash<engine::core::u64>()((engine::core::u64)uuid);
        }
    };

}
