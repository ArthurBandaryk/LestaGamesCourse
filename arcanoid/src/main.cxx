#include "game.hxx"

#include <imgui.h>

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

int main(int, char**)
{
    arcanoid::game arcanoid {};
    arcanoid.main_loop();
    return EXIT_SUCCESS;
}
