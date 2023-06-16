#pragma once

#include "component.hxx"
#include "coordinator.hxx"
#include "engine.hxx"
#include "entity.hxx"
#include "game-system.hxx"

#include <map>
#include <memory>
#include <string>

namespace arcanoid
{
    class game final
    {
    public:
        void on_init();
        void on_event(arci::event& event);
        void on_update(const float dt);
        void on_render();

        enum class game_status
        {
            main_menu,
            game_start,
            game_over,
            exit
        };

    private:
        void init_game_objects();
        void init_bricks();
        void init_ball();
        void init_platform();
        void init_background();

        std::vector<arci::itexture*> m_textures {};
        std::map<std::string, arci::iaudio_buffer*> m_sounds {};

        coordinator m_coordinator {};
        sprite_system m_sprite_system {};

        std::unique_ptr<arci::iengine,
                        void (*)(arci::iengine*)>
            m_engine { nullptr, nullptr };
        std::size_t m_screen_w {};
        std::size_t m_screen_h {};
        game_status status { game_status::game_start };
    };
}