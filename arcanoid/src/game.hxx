#pragma once

#include "component.hxx"
#include "coordinator.hxx"
#include "engine.hxx"
#include "entity.hxx"
#include "game-system.hxx"

#include "FrameTimer.hxx"

#include <map>
#include <memory>
#include <string>

namespace arcanoid
{
    class game final
    {
    public:
        void main_loop();
        ~game();

    private:
        enum class game_status
        {
            main_menu,
            game,
            game_over,
            exit
        };

        void on_init();
        void on_event();
        void on_update(const float dt);
        void on_render();

        void init_world();
        void init_bricks();
        void init_ball();
        void init_platform();
        void init_background();

        std::vector<arci::itexture*> m_textures {};
        std::map<std::string, arci::iaudio_buffer*> m_sounds {};

        coordinator m_coordinator {};
        input_system m_input_system {};
        sprite_system m_sprite_system {};
        transform_system m_transform_system {};
        collision_system m_collision_system {};

        std::unique_ptr<arci::iengine,
                        void (*)(arci::iengine*)>
            m_engine { nullptr, nullptr };
        std::size_t m_screen_w {};
        std::size_t m_screen_h {};
        game_status m_status { game_status::game };

        cFrameTimer m_frame_timer;
    };
}
