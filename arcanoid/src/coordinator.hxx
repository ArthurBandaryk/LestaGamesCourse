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
        std::map<entity, transform2d> transformations {};
        std::map<entity, key_inputs> inputs {};
    };
}
