#pragma once

#include <engine.hxx>

#include <glm/ext/vector_float2.hpp>

#include <array>

namespace arcanoid
{
    struct position
    {
        std::array<glm::vec2, 4u> vertices;
    };

    struct sprite
    {
        arci::itexture* texture { nullptr };
    };

    struct transform2D
    {
        glm::vec2 scale {};
    };

    struct motion2D
    {
        float speed { 0.f };
    };

    struct life
    {
        std::uint32_t lives_number {};
        std::uint32_t lives_left {};
    };
}
