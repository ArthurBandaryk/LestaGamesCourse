#include "engine.hxx"

//
#include <glm/ext/matrix_float2x2_precision.hpp>
#include <glog/logging.h>

//
#include <imgui.h>

//
#include <algorithm>
#include <cmath>
#include <memory>

enum class game_status
{
    main_menu,
    game_start,
    game_over,
    exit
};

void main_menu(const std::size_t width,
               const std::size_t height,
               game_status& status)
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x,
                                   main_viewport->WorkPos.y),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    ImGui::Begin("Main menu", nullptr, window_flags);

    // Game name text label.
    const char game_name[] { "ARCANOID" };
    const float game_name_offset_y { height / 10.f };
    const float game_text_width = ImGui::CalcTextSize(game_name).x;
    const float game_text_height = ImGui::CalcTextSize(game_name).y;
    ImGui::SetCursorPosX((width - game_text_width) * 0.5f);
    ImGui::SetCursorPosY(game_name_offset_y);
    ImGui::Text(game_name);

    // Start game button.
    const char start_button_name[] { "START" };
    const float start_button_width { game_text_width * 2.f };
    const float start_button_height { game_text_height * 2.f };
    ImGui::SetCursorPosX((width - start_button_width) * 0.5f);
    ImGui::SetCursorPosY(game_name_offset_y
                         + game_text_height
                         + game_name_offset_y / 2.f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.f, 128.f, 0.f, 255.f));
    if (ImGui::Button(start_button_name,
                      ImVec2(start_button_width, start_button_height)))
    {
        status = game_status::game_start;
    }
    ImGui::PopStyleColor();

    // Exit button.
    const char exit_button_name[] { "EXIT" };
    const float exit_button_width { start_button_width };
    const float exit_button_height { start_button_height };
    ImGui::SetCursorPosX((width - exit_button_width) * 0.5f);
    ImGui::SetCursorPosY(game_name_offset_y
                         + game_text_height
                         + game_name_offset_y / 2.f
                         + start_button_height
                         + game_name_offset_y / 2.f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.f, 0.f, 0.f, 255.f));
    if (ImGui::Button(exit_button_name,
                      ImVec2(exit_button_width, exit_button_height)))
    {
        status = game_status::exit;
    }
    ImGui::PopStyleColor();

    ImGui::End();
}

void game_over_menu(const std::size_t width,
                    const std::size_t height)
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x,
                                   main_viewport->WorkPos.y),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    ImGui::Begin("Main menu", nullptr, window_flags);

    // Game over text label.
    const char title[] { "GAME OVER!" };
    const float title_offset_y { height / 2.f };
    const float title_text_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((width - title_text_width) * 0.5f);
    ImGui::SetCursorPosY(title_offset_y);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.f, 0.f, 0.f, 255.f));
    ImGui::Text(title);
    ImGui::PopStyleColor();

    ImGui::End();
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

    const auto [screen_width, screen_height]
        = engine->get_screen_resolution();

    arci::itexture* texture = engine->create_texture("worm.png");

    std::vector<arci::vertex> vertices {
        { -0.3f, -0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f },
        { 0.3f, -0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f },
        { -0.3f, 0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f },
        { 0.3f, 0.3f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f },
    };

    std::vector<uint32_t> indices { 0, 1, 2, 2, 3, 1 };

    arci::ivertex_buffer* vertex_buffer = engine->create_vertex_buffer(vertices);
    arci::i_index_buffer* index_buffer = engine->create_ebo(indices);
    arci::iaudio_buffer* background_music
        = engine->create_audio_buffer("music.wav");
    arci::iaudio_buffer* hit_sound = engine->create_audio_buffer("hit.wav");

    background_music->play(arci::iaudio_buffer::running_mode::for_ever);

    game_status status = game_status::main_menu;

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
            hit_sound->play(arci::iaudio_buffer::running_mode::once);
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

        const glm::mediump_mat3x3 rotation_matrix {
            std::cos(worm_direction), std::sin(worm_direction), 0.f,
            -std::sin(worm_direction), std::cos(worm_direction), 0.f,
            0.f, 0.f, 1.f
        };

        const glm::mediump_mat3x3 reflection_matrix {
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

        if (status == game_status::game_start)
        {
            engine->render(vertex_buffer, index_buffer, texture, result_matrix);
        }
        else if (status == game_status::exit)
        {
            loop_continue = false;
            break;
        }
        else if (status == game_status::game_over)
        {
            engine->imgui_new_frame();

            game_over_menu(screen_width, screen_height);

            engine->imgui_render();
        }
        else if (status == game_status::main_menu)
        {
            engine->imgui_new_frame();

            main_menu(screen_width, screen_height, status);

            engine->imgui_render();
        }

        engine->swap_buffers();
    }

    engine->imgui_uninit();
    engine->uninit();
    engine->destroy_texture(texture);
    engine->destroy_vertex_buffer(vertex_buffer);
    engine->destroy_ebo(index_buffer);
    engine->destroy_audio_buffer(background_music);
    engine->destroy_audio_buffer(hit_sound);

    return EXIT_SUCCESS;
}
