#include "game-system.hxx"
#include "entity.hxx"
#include "helper.hxx"

namespace arcanoid
{
    void sprite_system::update([[maybe_unused]] float dt)
    {
    }

    void sprite_system::render(arci::iengine* engine,
                               coordinator& a_coordinator)
    {
        for (entity i = 1; i <= entities_number; i++)
        {
            if (a_coordinator.sprites.count(i))
            {
                const position pos = a_coordinator.positions.at(i);
                arci::itexture* texture = a_coordinator.sprites.at(i).texture;
                arci::CHECK_NOTNULL(texture);

                std::vector<arci::vertex> vertices {};
                auto from_world_to_ndc = [](const glm::vec2& world_pos) {
                    return glm::vec2 { -1.f + world_pos[0] * 2.f / 1024.f,
                                       1.f - world_pos[1] * 2 / 768.f };
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
}
