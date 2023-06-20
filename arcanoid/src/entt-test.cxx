#include <entt/entt.hpp>

#include <iostream>

struct position
{
    float x;
    float y;
};

struct velocity
{
    float dx;
    float dy;
};

void update(entt::registry& registry)
{
    auto view = registry.view<const position, velocity>();

    // use a callback
    view.each([](const auto& pos, auto& vel) {
        std::cout << "Current pos: " << pos.x << ", " << pos.y << std::endl;
        std::cout << "Current speed: " << vel.dx << ", "
                  << vel.dy << std::endl;
    });

    // use an extended callback
    view.each([]([[maybe_unused]] const entt::entity entity,
                 const auto& pos,
                 auto& vel) {

        std::cout << "Current pos: " << pos.x << ", " << pos.y << std::endl;
        std::cout << "Current speed: " << vel.dx << ", "
                  << vel.dy << std::endl; });

    // use forward iterators and get only the components of interest
    for (auto entity : view)
    {
        auto& vel = view.get<velocity>(entity);
        vel.dx = -vel.dx;
        vel.dx = -vel.dy;
    }
}

int main()
{
    entt::registry registry;

    for (auto i = 0u; i < 10u; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i * 1.f, i * 1.f);
        if (i % 2 == 0)
        {
            registry.emplace<velocity>(entity, i * .1f, i * .1f);
        }
    }

    update(registry);
}
