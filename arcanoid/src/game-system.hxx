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
    };
}
