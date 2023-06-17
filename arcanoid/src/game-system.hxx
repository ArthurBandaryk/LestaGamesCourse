#pragma once

#include "coordinator.hxx"
#include "engine.hxx"

namespace arcanoid
{
    struct sprite_system
    {
        void update(const float dt);
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
        void update(coordinator& a_coordinator, arci::iengine* engine);
    };
}
