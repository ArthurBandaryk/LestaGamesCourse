#include "game-system.hxx"
#include "entity.hxx"
#include "helper.hxx"

#include <cmath>

namespace arcanoid
{
    void sprite_system::render(arci::iengine* engine,
                               coordinator& a_coordinator)
    {
        for (entity i = 1; i <= entities_number; i++)
        {
            if (a_coordinator.sprites.count(i)
                && a_coordinator.positions.count(i))
            {
                const position pos = a_coordinator.positions.at(i);
                arci::itexture* texture = a_coordinator.sprites.at(i).texture;
                arci::CHECK_NOTNULL(texture);

                std::vector<arci::vertex> vertices {};

                auto from_world_to_ndc = [this](const glm::vec2& world_pos) {
                    return glm::vec2 { -1.f + world_pos[0] * 2.f / screen_width,
                                       1.f - world_pos[1] * 2 / screen_height };
                };

                glm::vec2 tex_coordinates[4] {
                    glm::vec2 { 0.f, 1.f }, // Top left.
                    glm::vec2 { 1.f, 1.f }, // Top right.
                    glm::vec2 { 1.f, 0.f }, // Bottom right.
                    glm::vec2 { 0.f, 0.f }, // Bottom left.
                };

                for (std::size_t i = 0; i < pos.vertices.size(); i++)
                {
                    glm::vec2 ndc_pos = from_world_to_ndc(pos.vertices[i]);
                    arci::vertex v {
                        ndc_pos[0],
                        ndc_pos[1],
                        1.f,
                        0.f,
                        0.f,
                        0.f,
                        1.f,
                        tex_coordinates[i].x,
                        tex_coordinates[i].y
                    };

                    vertices.push_back(v);
                }

                std::vector<std::uint32_t> indices { 0, 1, 2, 0, 3, 2 };

                arci::ivertex_buffer* vbo = engine->create_vertex_buffer(vertices);
                arci::i_index_buffer* ebo = engine->create_ebo(indices);
                arci::CHECK_NOTNULL(vbo);
                arci::CHECK_NOTNULL(ebo);

                engine->render(vbo, ebo, texture);
                engine->destroy_vertex_buffer(vbo);
                engine->destroy_ebo(ebo);
            }
        }
    }

    void transform_system::update(coordinator& a_coordinator, const float dt)
    {
        for (entity i = 1; i <= entities_number; i++)
        {
            if (a_coordinator.transformations.count(i)
                && a_coordinator.positions.count(i))
            {
                position& pos = a_coordinator.positions.at(i);
                for (auto& vertex : pos.vertices)
                {
                    vertex.x
                        += a_coordinator.transformations.at(i).speed_x * dt;
                    vertex.y
                        += a_coordinator.transformations.at(i).speed_y * dt;
                }
            }
        }
    }

    void input_system::update(coordinator& a_coordinator,
                              arci::iengine* engine,
                              const float dt)
    {
        const float speed { 20.f / dt };

        for (entity i = 1; i <= entities_number; i++)
        {
            if (a_coordinator.inputs.count(i)
                && a_coordinator.transformations.count(i))
            {
                if (engine->key_down(arci::keys::left))
                {
                    a_coordinator.transformations.at(i).speed_x = -speed;
                }
                if (engine->key_down(arci::keys::right))
                {
                    a_coordinator.transformations.at(i).speed_x = speed;
                }
                if (!engine->key_down(arci::keys::right)
                    && !engine->key_down(arci::keys::left))
                {
                    a_coordinator.transformations.at(i).speed_x = 0.f;
                }
            }
        }
    }

    void collision_system::update(coordinator& a_coordinator,
                                  const float dt,
                                  const std::size_t screen_width,
                                  const std::size_t screen_height)
    {
        for (auto& collidable : a_coordinator.collidable_entities)
        {
            if (collidable.first == a_coordinator.collidable_ids.at("ball"))
            {
                resolve_collision_for_ball(collidable.first,
                                           a_coordinator,
                                           dt,
                                           screen_width,
                                           screen_height);
            }

            if (collidable.first == a_coordinator.collidable_ids.at("platform"))
            {
                resolve_collision_for_platform(collidable.first,
                                               a_coordinator,
                                               dt,
                                               screen_width);
            }
        }
    }

    void collision_system::resolve_collision_for_platform(
        const entity id,
        coordinator& a_coordinator,
        const float dt,
        const std::size_t screen_width)
    {
        position& pos = a_coordinator.positions.at(id);

        for (const auto& vertex : pos.vertices)
        {
            const float new_x
                = vertex.x + a_coordinator.transformations.at(id).speed_x * dt;
            if (new_x < 0.f || new_x > screen_width)
            {
                a_coordinator.transformations.at(id).speed_x = 0.f;
                break;
            }
        }
    }

    void collision_system::resolve_ball_vs_walls(
        const entity id,
        coordinator& a_coordinator,
        [[maybe_unused]] const float dt,
        const std::size_t screen_width,
        const std::size_t screen_height)
    {
        const position& pos = a_coordinator.positions.at(id);

        for (const auto& vertex : pos.vertices)
        {
            const float new_x = vertex.x
                + a_coordinator.transformations.at(id).speed_x * dt;
            const float new_y = vertex.y
                + a_coordinator.transformations.at(id).speed_y * dt;
            if (new_x < 0.f || new_x > screen_width)
            {
                a_coordinator.transformations.at(id).speed_x *= -1.f;
            }

            if (new_y < 0.f || new_y > screen_height)
            {
                a_coordinator.transformations.at(id).speed_y *= -1.f;
                break;
            }
        }
    }

    void collision_system::resolve_collision_for_ball(
        const entity id,
        coordinator& a_coordinator,
        const float dt,
        const std::size_t screen_width,
        const std::size_t screen_height)
    {
        resolve_ball_vs_walls(id,
                              a_coordinator,
                              dt,
                              screen_width,
                              screen_height);

        // If the ball is collidable with some brick.
        // We're using this flag just to know that we should
        // not change the direction cause we've already
        // done it per this frame.
        bool is_collidable { false };

        for (entity ent = 1; ent <= entities_number; ent++)
        {
            // Entity is not collidable. So just continue.
            if (!a_coordinator.collidable_entities.count(ent))
            {
                continue;
            }

            // There is no any need to check collision to itself.
            if (ent == id)
            {
                continue;
            }

            if (ent == a_coordinator.collidable_ids.at("platform"))
            {
                // Do nothing for now.
                continue;
            }

            resolve_ball_vs_brick(id, ent, a_coordinator, dt, is_collidable);
        }
    }

    void collision_system::resolve_ball_vs_brick(
        const entity ball_id,
        const entity brick_id,
        coordinator& a_coordinator,
        const float dt,
        [[maybe_unused]] bool& is_collidable)
    {
        position& ball_pos = a_coordinator.positions.at(ball_id);
        position& brick_pos = a_coordinator.positions.at(brick_id);

        // Calculate ball new position for a next frame.
        glm::vec2 ball_new_pos[4] {};

        for (std::size_t i = 0; i < ball_pos.vertices.size(); i++)
        {
            ball_new_pos[i].x = ball_pos.vertices[i].x
                + a_coordinator.transformations.at(ball_id).speed_x * dt;
            ball_new_pos[i].y = ball_pos.vertices[i].y
                + a_coordinator.transformations.at(ball_id).speed_y * dt;
        }

        const float ball_x_left { ball_new_pos[0].x };
        const float ball_x_right { ball_new_pos[1].x };
        const float ball_y_top { ball_new_pos[0].y };
        const float ball_y_bottom { ball_new_pos[2].y };

        const float brick_x_left { brick_pos.vertices[0].x };
        const float brick_x_right { brick_pos.vertices[1].x };
        const float brick_y_top { brick_pos.vertices[0].y };
        const float brick_y_bottom { brick_pos.vertices[2].y };

        // Check if ball is collidable with brick.
        if ((brick_x_right <= ball_x_right && brick_x_right >= ball_x_left)
            || (brick_x_left <= ball_x_right && brick_x_left >= ball_x_left)
            || (ball_x_left >= brick_x_left && ball_x_left <= brick_x_right))
        {
            if ((brick_y_top <= ball_y_bottom && brick_y_top >= ball_y_top)
                || (ball_y_top <= brick_y_bottom && ball_y_top >= brick_y_top)
                || (brick_y_bottom <= ball_y_bottom && brick_y_bottom >= ball_y_top))
            {
                // Ball collides with brick. Just remove the brick
                // from all data.
                a_coordinator.destroy_entity(brick_id);
            }
            // Ball and brick do not collide for Y axis.
            else
            {
                return;
            }
        }
        // Ball and brick do not collide for X axis.
        else
        {
            return;
        }

        // Calculate reflection. Basically I'm looking which side
        // of brick the ball collides.
        if (!is_collidable)
        {
            is_collidable = true;

            // First case. Ball intersects only horizontal line of brick.
            if (ball_x_left >= brick_x_left && ball_x_right <= brick_x_right)
            {
                a_coordinator.transformations.at(ball_id).speed_y *= -1.f;
                return;
            }

            // Second case. Ball intersects only vertical line of brick.
            if ((ball_x_left <= brick_x_left && ball_y_top > brick_y_top
                 && ball_y_bottom <= brick_y_bottom)
                || (ball_x_right >= brick_x_right && ball_y_top > brick_y_top
                    && ball_y_bottom <= brick_y_bottom))
            {
                a_coordinator.transformations.at(ball_id).speed_x *= -1.f;
                return;
            }

            // Third case. Ball intersects brick's edge.
            if ((ball_x_left < brick_x_left && brick_y_top < ball_y_top)
                || (ball_x_right > brick_x_right && brick_y_top < ball_y_top)
                || (ball_x_left < brick_x_left && brick_y_bottom < ball_y_bottom)
                || (ball_x_right > brick_x_right && brick_y_bottom < ball_y_bottom))
            {
                // TODO(arci): needs to be more specific here (find max length for
                // collidable side).
                a_coordinator.transformations.at(ball_id).speed_x *= -1.f;
                return;
            }
        }
    }
}
