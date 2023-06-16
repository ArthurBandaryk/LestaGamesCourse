#include "game.hxx"
#include "helper.hxx"

namespace arcanoid
{
    void game::on_init()
    {
        m_engine = std::unique_ptr<arci::iengine, void (*)(arci::iengine*)> {
            arci::engine_create(),
            arci::engine_destroy
        };

        m_engine->init();

        const auto [w, h] = m_engine->get_screen_resolution();
        m_screen_w = w;
        m_screen_h = h;
        m_sprite_system.screen_width = w;
        m_sprite_system.screen_height = h;

        arci::iaudio_buffer* background_sound
            = m_engine->create_audio_buffer("res/music.wav");
        arci::iaudio_buffer* hit_ball_sound
            = m_engine->create_audio_buffer("res/music.wav");
        m_sounds.insert({ "background", background_sound });
        m_sounds.insert({ "hit_ball", hit_ball_sound });

        m_sounds["background"]->play(
            arci::iaudio_buffer::running_mode::for_ever);

        init_game_objects();
    }

    void game::init_game_objects()
    {
        init_background();
        init_bricks();
        init_ball();
        init_platform();
    }

    void game::init_bricks()
    {
        arci::itexture* yellow_brick_texture
            = m_engine->create_texture("res/yellow_brick.png");
        arci::CHECK_NOTNULL(yellow_brick_texture);
        m_textures.push_back(yellow_brick_texture);

        const float brick_width { m_screen_w / 10.f };
        const float brick_height { m_screen_h / 20.f };

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                entity brick = create_entity();

                position brick_position {
                    glm::vec2 { 0.f + brick_width * j,
                                0.f + brick_height * i },
                    glm::vec2 { brick_width + brick_width * j,
                                0.f + brick_height * i },
                    glm::vec2 { brick_width + brick_width * j,
                                brick_height + brick_height * i },
                    glm::vec2 { 0.f + brick_width * j,
                                brick_height + brick_height * i },
                };
                const auto [it1, position_inserted]
                    = m_coordinator.positions.insert({ brick, brick_position });
                arci::CHECK(position_inserted);

                sprite brick_sprite { yellow_brick_texture };
                const auto [it2, sprite_inserted]
                    = m_coordinator.sprites.insert({ brick, brick_sprite });
                arci::CHECK(sprite_inserted);
            }
        }
    }

    void game::init_background()
    {
        entity background = create_entity();

        arci::itexture* background_texture
            = m_engine->create_texture("res/background1.png");
        arci::CHECK_NOTNULL(background_texture);
        m_textures.push_back(background_texture);

        position pos {
            glm::vec2 { 0.f, 0.f },
            glm::vec2 { m_screen_w, 0.f },
            glm::vec2 { m_screen_w, m_screen_h },
            glm::vec2 { 0.f, m_screen_h },
        };
        const auto [it1, pos_inserted]
            = m_coordinator.positions.insert({ background, pos });
        arci::CHECK(pos_inserted);

        sprite spr { background_texture };
        const auto [it2, sprite_inserted]
            = m_coordinator.sprites.insert({ background, spr });
        arci::CHECK(sprite_inserted);
    }

    void game::init_ball()
    {
        entity ball = create_entity();

        arci::itexture* texture
            = m_engine->create_texture("res/ball.png");
        arci::CHECK_NOTNULL(texture);
        m_textures.push_back(texture);

        const float ball_width { m_screen_w / 35.f };
        const float ball_height { m_screen_w / 35.f };

        position pos {
            glm::vec2 { m_screen_w / 2.f - ball_width / 2.f,
                        3.f * m_screen_h / 4.f - ball_height / 2.f },
            glm::vec2 { m_screen_w / 2.f + ball_width / 2.f,
                        3.f * m_screen_h / 4.f - ball_height / 2.f },
            glm::vec2 { m_screen_w / 2.f + ball_width / 2.f,
                        3.f * m_screen_h / 4.f + ball_height / 2.f },
            glm::vec2 { m_screen_w / 2.f - ball_width / 2.f,
                        3.f * m_screen_h / 4.f + ball_height / 2.f },
        };
        const auto [it1, pos_inserted]
            = m_coordinator.positions.insert({ ball, pos });
        arci::CHECK(pos_inserted);

        sprite spr { texture };
        const auto [it2, sprite_inserted]
            = m_coordinator.sprites.insert({ ball, spr });
        arci::CHECK(sprite_inserted);
    }

    void game::init_platform()
    {
        entity platform = create_entity();

        arci::itexture* texture
            = m_engine->create_texture("res/platform1.png");
        arci::CHECK_NOTNULL(texture);
        m_textures.push_back(texture);

        const float platform_width { m_screen_w / 5.f };
        const float platform_height { m_screen_w / 25.f };

        position pos {
            glm::vec2 { m_screen_w / 2.f - platform_width / 2.f,
                        m_screen_h - platform_height },
            glm::vec2 { m_screen_w / 2.f + platform_width / 2.f,
                        m_screen_h - platform_height },
            glm::vec2 { m_screen_w / 2.f + platform_width / 2.f,
                        m_screen_h },
            glm::vec2 { m_screen_w / 2.f - platform_width / 2.f,
                        m_screen_h },
        };
        const auto [it1, pos_inserted]
            = m_coordinator.positions.insert({ platform, pos });
        arci::CHECK(pos_inserted);

        sprite spr { texture };
        const auto [it2, sprite_inserted]
            = m_coordinator.sprites.insert({ platform, spr });
        arci::CHECK(sprite_inserted);
    }

    void game::on_update([[maybe_unused]] const float dt)
    {
    }

    void game::on_render()
    {
        m_sprite_system.render(m_engine.get(), m_coordinator);

        m_engine->swap_buffers();
    }
}