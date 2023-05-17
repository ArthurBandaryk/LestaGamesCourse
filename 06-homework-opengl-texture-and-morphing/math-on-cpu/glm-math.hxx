#pragma once

#include "engine.hxx"

//
#include <glm/ext/matrix_float2x2_precision.hpp>

//
#include <utility>

namespace arci
{
    static glm::mediump_mat2x2 get_aspect_matrix(const iengine& engine)
    {
        const std::pair<std::size_t, std::size_t> window_size
            = engine.get_screen_resolution();

        return glm::mediump_mat2(window_size.second / window_size.first, 0, 0, 1);
    }

    static arci::triangle
    get_scaled_triangle(const arci::triangle& triangle,
                        const arci::vector2d& scale)
    {
        arci::triangle result {};

        // glm::mat2x2 scale =

        for (const arci::vertex& v : triangle.vertices)
        {
        }
    }
}
