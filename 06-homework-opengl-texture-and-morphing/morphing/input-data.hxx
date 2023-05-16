#pragma once

#include "engine.hxx"

#include <vector>

namespace arci
{
    arci::triangle triangle_left {
        { -0.4f, 0.f, 0.f, 0.f, 1.f, 0.f },
        { -0.1f, 0.1f, 0.f, 1.f, 0.f, 0.f },
        { -0.1f, -0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle triangle_up {
        { -0.1f, 0.1f, 0.f, 0.f, 1.f, 0.f },
        { 0.f, 0.4f, 0.f, 1.f, 0.f, 0.f },
        { 0.1f, 0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle triangle_right {
        { 0.1f, 0.1f, 0.f, 0.f, 1.f, 0.f },
        { 0.4f, 0.f, 0.f, 1.f, 0.f, 0.f },
        { 0.1f, -0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle triangle_down {
        { -0.1f, -0.1f, 0.f, 0.f, 1.f, 0.f },
        { 0.f, -0.4f, 0.f, 1.f, 0.f, 0.f },
        { 0.1f, -0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle triangle_middle_low {
        { -0.1f, 0.1f, 0.f, 0.f, 1.f, 0.f },
        { -0.1f, -0.1f, 0.f, 1.f, 0.f, 0.f },
        { 0.1f, -0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle triangle_middle_high {
        { -0.1f, 0.1f, 0.f, 0.f, 1.f, 0.f },
        { 0.1f, 0.1f, 0.f, 1.f, 0.f, 0.f },
        { 0.1f, -0.1f, 0.f, 0.f, 0.f, 1.f },
    };

    arci::triangle line {
        { -0.8f, 0.f, 0.f, 0.f, 1.f, 0.f },
        { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f },
        { 0.8f, 0.f, 0.f, 0.f, 0.f, 1.f },
    };

    std::vector<arci::triangle> triangles {
        triangle_left,
        triangle_up,
        triangle_down,
        triangle_right,
        triangle_middle_low,
        triangle_middle_high,
    };
}
