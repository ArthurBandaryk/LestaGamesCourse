#pragma once

#include "coordinator.hxx"
#include "engine.hxx"

namespace arcanoid
{
    struct sprite_system
    {
        void render(arci::iengine* engine,
                    coordinator& a_coordinator);

        std::size_t screen_width {};
        std::size_t screen_height {};
    };

    struct transform_system
    {
        void update(coordinator& a_coordinator, const float dt);
    };

    struct input_system
    {
        void update(coordinator& a_coordinator, arci::iengine* engine, const float dt);
    };

    struct collision_system
    {
        void update(coordinator& a_coordinator,
                    const float dt,
                    const std::size_t screen_width,
                    const std::size_t screen_height);

    private:
        void resolve_collision_for_ball(const entity id,
                                        coordinator& a_coordinator,
                                        const float dt,
                                        const std::size_t screen_width,
                                        const std::size_t screen_height);
        void resolve_collision_for_platform(const entity id,
                                            coordinator& a_coordinator,
                                            const float dt,
                                            const std::size_t screen_width);
        void resolve_ball_vs_walls(const entity id,
                                   coordinator& a_coordinator,
                                   const float dt,
                                   const std::size_t screen_width);

        void resolve_ball_vs_brick(const entity ball_id,
                                   const entity brick_id,
                                   coordinator& a_coordinator,
                                   const float dt,
                                   bool& is_collidable);

        void resolve_ball_vs_platform(const entity ball_id,
                                      const entity platform_id,
                                      coordinator& a_coordinator,
                                      const float dt);
    };

    enum class game_status
    {
        main_menu,
        game,
        game_over,
        exit
    };

    struct game_over_system
    {
        void update(coordinator& a_coordinator,
                    game_status& status,
                    const std::size_t screen_height);

        void render(arci::iengine* engine,
                    std::size_t width,
                    const std::size_t height);
    };

    struct menu_system
    {
        void render(arci::iengine* engine,
                    game_status& status,
                    std::size_t width,
                    const std::size_t height);
    };
}
