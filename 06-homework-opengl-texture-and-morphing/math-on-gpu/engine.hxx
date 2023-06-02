#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include <glm/ext/matrix_float2x2_precision.hpp>

///////////////////////////////////////////////////////////////////////////////

namespace arci
{

    ///////////////////////////////////////////////////////////////////////////////

    class iengine;

    iengine* engine_create();

    ///////////////////////////////////////////////////////////////////////////////

    void engine_destroy(iengine* engine);

    ///////////////////////////////////////////////////////////////////////////////

    enum class event_from_device
    {
        keyboard,
        mouse,
    };

    ///////////////////////////////////////////////////////////////////////////////

    struct mouse_event
    {
        float x;
        float y;
    };

    ///////////////////////////////////////////////////////////////////////////////

    enum class keyboard_event
    {
        left_button_pressed,
        left_button_released,
        right_button_pressed,
        right_button_released,
        up_button_pressed,
        up_button_released,
        down_button_pressed,
        down_button_released,
        space_button_pressed,
        space_button_released,
        plus_button_pressed,
        plus_button_released,
        minus_button_pressed,
        minus_button_released,
        escape_button_pressed,
        escape_button_released,
    };

    ///////////////////////////////////////////////////////////////////////////////

    struct event
    {
        event_from_device device;
        std::optional<mouse_event> mouse_info;
        std::optional<keyboard_event> keyboard_info;
        bool is_quitting { false };
    };

    ///////////////////////////////////////////////////////////////////////////////

    struct vertex
    {
        float x {};
        float y {};
        float z {};
        float r {};
        float g {};
        float b {};
        float tx {};
        float ty {};
    };

    struct triangle
    {
        triangle() = default;
        triangle(const vertex& v0, const vertex& v1, const vertex& v2);

        std::array<vertex, 3> vertices {};
    };

    ///////////////////////////////////////////////////////////////////////////////

    class itexture
    {
    public:
        virtual ~itexture() = default;
        virtual void load(const std::string_view path) = 0;
        virtual void bind() = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////

    class iengine
    {
    public:
        virtual ~iengine() = default;
        virtual void init() = 0;
        virtual bool process_input(event& event) = 0;

        // Render simple colored triangle.
        virtual void render(const triangle& triangle) = 0;

        // Render textured triangle. All math is calculated on cpu.
        /* clang-format off */
        virtual void render(const triangle& triangle,
                            itexture* const texture) = 0;
        /* clang-format on */

        // Render textured triangle. Math is calculated on gpu.
        /* clang-format off */
        virtual void render(const triangle& triangle,
                            itexture* const texture, 
                            const glm::mediump_mat3& matrix) = 0;
        /* clang-format on */

        virtual itexture* create_texture(const std::string_view path) = 0;
        virtual void destroy_texture(const itexture* const texture) = 0;
        virtual void uninit() = 0;
        virtual void swap_buffers() = 0;
        virtual std::pair<size_t, size_t> get_screen_resolution() const noexcept = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
