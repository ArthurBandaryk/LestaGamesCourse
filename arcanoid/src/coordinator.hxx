#pragma once

#include "component.hxx"
#include "entity.hxx"

#include <map>

namespace arcanoid
{
    struct coordinator
    {
        std::map<entity, position> positions {};
        std::map<entity, sprite> sprites {};
    };
}
