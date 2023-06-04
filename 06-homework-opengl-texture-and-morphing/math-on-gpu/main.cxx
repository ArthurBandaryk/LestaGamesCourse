#include "engine.hxx"

//
#include <glm/ext/matrix_float2x2_precision.hpp>
#include <glog/logging.h>

//
#include <algorithm>
#include <cmath>
#include <memory>

int main(int, char** argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    // Create engine.
    std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> engine {
        arci::engine_create(),
        arci::engine_destroy
    };

    engine->init();

    std::unique_ptr<arci::itexture> texture {
        engine->create_texture("worm.png")
    };

    const auto [screen_width, screen_height]
        = engine->get_screen_resolution();

    arci::triangle triangle_low {
        { -0.3f, -0.3f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f },
        { 0.3f, -0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f },
        { -0.3f, 0.3f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
    };

    arci::triangle triangle_high {
        { -0.3f, 0.3f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
        { 0.3f, 0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f },
        { 0.3f, -0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f },
    };

    glm::vec3 worm_pos { 0.f, 0.f, 1.f };
    glm::vec2 worm_scale { 1.f, 1.f };

    constexpr float pi { 3.14159f };
    float worm_direction { 0.f };
    float reflection_y { 1.f };
    float speed_x { 0.01f }, speed_y { 0.01f };

    bool loop_continue { true };

    while (loop_continue)
    {
        arci::event event {};

        while (engine->process_input(event))
        {
            if (event.is_quitting)
            {
                loop_continue = false;
                break;
            }
        }

        if (engine->key_down(arci::keys::magnify))
        {
            worm_scale[0] += 0.1;
            worm_scale[1] += 0.1;
        }
        else if (engine->key_down(arci::keys::reduce))
        {
            worm_scale[0] -= 0.1;
            worm_scale[1] -= 0.1;
        }
        else if (engine->key_down(arci::keys::left))
        {
            worm_direction = 0.f;
            reflection_y = 1.f;
            worm_pos[0] -= speed_x;
        }
        else if (engine->key_down(arci::keys::right))
        {
            worm_direction = 0.f;
            reflection_y = -1.f;
            worm_pos[0] += speed_x;
        }
        else if (engine->key_down(arci::keys::up))
        {
            worm_direction = 0.f;
            worm_pos[1] += speed_y;
        }
        else if (engine->key_down(arci::keys::down))
        {
            worm_direction = pi / 2.f;
            reflection_y = 1.f;
            worm_pos[1] -= speed_y;
        }
        else if (engine->key_down(arci::keys::button1))
        {
            reflection_y = 1.f;
            worm_direction += 0.1f;
        }

        const glm::mediump_mat3x3 aspect_matrix {
            1.f, 0.f, 0.f,
            0.f, static_cast<float>(screen_width) / screen_height, 0.f,
            0.f, 0.f, 1.f
        };

        const glm::mediump_mat3x3 scale_matrix {
            worm_scale[0], 0.f, 0.f,
            0.f, worm_scale[1], 0.f,
            0.f, 0.f, 1.f
        };

        const glm::mediump_mat3 rotation_matrix {
            std::cos(worm_direction), std::sin(worm_direction), 0.f,
            -std::sin(worm_direction), std::cos(worm_direction), 0.f,
            0.f, 0.f, 1.f
        };

        const glm::mediump_mat3 reflection_matrix {
            reflection_y, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f
        };

        const glm::mediump_mat3x3 move_matrix {
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            worm_pos[0], worm_pos[1], 1.f
        };

        const glm::mediump_mat3 result_matrix
            = aspect_matrix * move_matrix * scale_matrix
            * rotation_matrix * reflection_matrix;

        engine->render(triangle_low, texture.get(), result_matrix);
        engine->render(triangle_high, texture.get(), result_matrix);

        engine->swap_buffers();
    }

    engine->uninit();

    return EXIT_SUCCESS;
}
