#include "engine.hxx"
// #include "glm-math.hxx"

//
#include <glm/ext/matrix_float2x2_precision.hpp>
#include <glog/logging.h>

//
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>

arci::triangle get_transformed_triangle(
    const arci::triangle& t,
    const glm::mediump_mat3x2& aspect_matrix,
    const glm::mediump_mat3x3& scale_matrix,
    const glm::mediump_mat3x3& rotation_matrix,
    const glm::mediump_mat3x3& move_matrix,
    const glm::vec3& worm_pos,
    const float worm_direction)
{
    arci::triangle result { t };

    std::for_each(result.vertices.begin(),
                  result.vertices.end(),
                  [&](arci::vertex& v) {
                      glm::vec3 v_pos_source { v.x, v.y, 1.f };

                      const glm::mediump_mat3x3 move_to_origin {
                          1.f, 0.f, 0.f,
                          0.f, 1.f, 0.f,
                          -v.x, -v.y, 1.f
                      };

                      const glm::mediump_mat3x3 move_back {
                          1.f, 0.f, 0.f,
                          0.f, 1.f, 0.f,
                          v.x, v.y, 1.f
                      };

                      glm::mediump_mat3 result_matrix
                          = aspect_matrix * scale_matrix
                          * move_to_origin * rotation_matrix
                          * move_back * move_matrix;

                      glm::vec3 v_pos_result = result_matrix * v_pos_source;

                      v.x = v_pos_result[0];
                      v.y = v_pos_result[1];

                      LOG(INFO) << v.x << " " << v.y;
                  });

    LOG(INFO) << "=======================================";

    return result;
}

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

    constexpr float pi { 3.14159 };
    float worm_direction { 0.f };
    float speed_x { 0.01f }, speed_y { 0.01f };
    const auto start_time = std::chrono::steady_clock::now();
    auto cur_time = std::chrono::steady_clock::now();

    bool loop_continue { true };

    while (loop_continue)
    {
        arci::event event {};
        cur_time = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed_seconds = cur_time - start_time;
        const float dt { elapsed_seconds.count() };

        while (engine->process_input(event))
        {
            if (event.is_quitting)
            {
                loop_continue = false;
                break;
            }
            else
            {
                if (event.keyboard_info)
                {
                    if (*event.keyboard_info
                        == arci::keyboard_event::plus_button_pressed)
                    {
                        worm_scale[0] += 0.1;
                        worm_scale[1] += 0.1;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::minus_button_pressed)
                    {
                        worm_scale[0] -= 0.1;
                        worm_scale[1] -= 0.1;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::left_button_pressed)
                    {
                        worm_direction = 0.f;
                        worm_pos[0] -= speed_x * dt;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::right_button_pressed)
                    {
                        worm_pos[0] += speed_x * dt;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::up_button_pressed)
                    {
                        worm_pos[1] += speed_y * dt;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::down_button_pressed)
                    {
                        worm_pos[1] -= speed_y * dt;
                    }
                    else if (*event.keyboard_info
                             == arci::keyboard_event::space_button_pressed)
                    {
                        worm_direction += 0.1f;
                    }
                }
            }
        }

        const glm::mediump_mat3x3 aspect_matrix {
            1.f,
            0.f,
            0.f,
            0.f,
            static_cast<float>(screen_width) / screen_height,
            0.f,
            0.f,
            0.f,
            1.f
        };

        const glm::mediump_mat3x3 scale_matrix {
            worm_scale[0],
            0.f,
            0.f,
            0.f,
            worm_scale[1],
            0.f,
            0.f,
            0.f,
            1.f
        };

        const glm::mediump_mat3 rotation_matrix {
            std::cos(worm_direction),
            std::sin(worm_direction),
            0.f,
            -std::sin(worm_direction),
            std::cos(worm_direction),
            0.f,
            0.f,
            0.f,
            1.f
        };

        const glm::mediump_mat3x3 move_matrix {
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            worm_pos[0], worm_pos[1], 1.f
        };

        arci::triangle triangle_low_transformed
            = get_transformed_triangle(
                triangle_low,
                aspect_matrix,
                scale_matrix,
                rotation_matrix,
                move_matrix,
                worm_pos,
                worm_direction);
        arci::triangle triangle_high_transformed
            = get_transformed_triangle(
                triangle_high,
                aspect_matrix,
                scale_matrix,
                rotation_matrix,
                move_matrix,
                worm_pos,
                worm_direction);

        engine->render(triangle_low_transformed, texture.get());
        engine->render(triangle_high_transformed, texture.get());

        engine->swap_buffers();
    }

    engine->uninit();

    return EXIT_SUCCESS;
}
