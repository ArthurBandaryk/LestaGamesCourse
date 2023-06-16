#pragma once

#include "coordinator.hxx"
#include "engine.hxx"

namespace arcanoid
{
    struct sprite_system
    {
        void update(float dt);
        void render(arci::iengine* engine,
                    coordinator& a_coordinator);
    };
}
