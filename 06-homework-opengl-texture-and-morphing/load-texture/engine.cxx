#include "engine.hxx"
#include "glad/glad.h"
#include "opengl-debug.hxx"
#include "opengl-shader-programm.hxx"
#include "picopng.hxx"

//
#include <SDL3/SDL.h>

//
#include <glog/logging.h>

//
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace arci
{

    ///////////////////////////////////////////////////////////////////////////////

    struct keyboard_key
    {
        SDL_KeyCode key_code;
        keyboard_event button_pressed;
        keyboard_event button_released;
        std::string_view key_name {};
    };

    ///////////////////////////////////////////////////////////////////////////////

    const std::array<keyboard_key, 6> keys {
        keyboard_key {
            SDLK_LEFT,
            keyboard_event::left_button_pressed,
            keyboard_event::left_button_released,
            "left",
        },
        keyboard_key {
            SDLK_RIGHT,
            keyboard_event::right_button_pressed,
            keyboard_event::right_button_released,
            "right",
        },
        keyboard_key {
            SDLK_UP,
            keyboard_event::up_button_pressed,
            keyboard_event::up_button_released,
            "up",
        },
        keyboard_key {
            SDLK_DOWN,
            keyboard_event::down_button_pressed,
            keyboard_event::down_button_released,
            "down",
        },
        keyboard_key {
            SDLK_SPACE,
            keyboard_event::space_button_pressed,
            keyboard_event::space_button_released,
            "space",
        },
        keyboard_key {
            SDLK_ESCAPE,
            keyboard_event::escape_button_pressed,
            keyboard_event::escape_button_released,
            "escape",
        },
    };

    ///////////////////////////////////////////////////////////////////////////////

    triangle::triangle(const vertex& v0, const vertex& v1, const vertex& v2)
    {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }

    ///////////////////////////////////////////////////////////////////////////////

    class opengl_texture : public itexture
    {
    public:
        void bind() override
        {
            CHECK(m_texture_id);
            glBindTexture(GL_TEXTURE_2D, m_texture_id);
            opengl_check();
        }

        void load(const std::string_view path) override;

        std::pair<unsigned long, unsigned long> get_texture_size() const
        {
            return { m_texture_width, m_texture_height };
        }

    private:
        GLuint m_texture_id {};
        unsigned long m_texture_width {};
        unsigned long m_texture_height {};
    };

    ///////////////////////////////////////////////////////////////////////////////

    class engine_using_sdl final : public iengine
    {
    public:
        engine_using_sdl() = default;
        engine_using_sdl(const engine_using_sdl&) = delete;
        engine_using_sdl(engine_using_sdl&&) = delete;
        engine_using_sdl& operator=(const engine_using_sdl&) = delete;
        engine_using_sdl& operator=(engine_using_sdl&&) = delete;

        void init() override;
        bool process_input(event& event) override;
        void render(const triangle& triangle) override;
        void render(const triangle& triangle,
                    itexture* const texture) override;
        itexture* create_texture(const std::string_view path) override;
        void destroy_texture(const itexture* const texture) override;
        void swap_buffers() override;
        void uninit() override;

        std::pair<size_t, size_t> get_screen_resolution() const noexcept;
        std::uint64_t get_time_since_epoch() const;

    private:
        std::optional<keyboard_key> get_keyboard_key_for_event(
            const SDL_Event& sdl_event);

        std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>
            m_window { nullptr, nullptr };

        std::unique_ptr<void, int (*)(SDL_GLContext)>
            m_opengl_context { nullptr, nullptr };

        opengl_shader_program m_colored_triangle_program {};
        opengl_shader_program m_textured_triangle_program {};

        std::size_t m_screen_width {};
        std::size_t m_screen_height {};
        GLuint m_vbo {};
        GLuint m_vao {};
    };

    void opengl_texture::load(const std::string_view path)
    {
        std::vector<unsigned char> raw_png_image {};

        std::ifstream file { path.data(), std::ios::binary };

        CHECK(file.is_open());

        const std::filesystem::path fs_path { path.data() };

        const std::size_t bytes_to_read {
            std::filesystem::file_size(fs_path)
        };

        CHECK(bytes_to_read) << "Nothing to read";

        raw_png_image.resize(bytes_to_read);

        file.read(reinterpret_cast<char*>(raw_png_image.data()), bytes_to_read);
        CHECK(file.good());

        file.close();
        CHECK(file.good());

        std::vector<unsigned char> raw_pixels_after_decoding {};

        const int decoding_status = decodePNG(raw_pixels_after_decoding,
                                              m_texture_width,
                                              m_texture_height,
                                              raw_png_image.data(),
                                              raw_png_image.size());

        CHECK(decoding_status == 0)
            << "Error on decoding " << path
            << ". Error code: " << decoding_status;
        CHECK(raw_pixels_after_decoding.size());

        glGenTextures(1, &m_texture_id);
        opengl_check();

        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        opengl_check();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        opengl_check();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        opengl_check();

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     m_texture_width,
                     m_texture_height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     raw_pixels_after_decoding.data());
        opengl_check();
        glGenerateMipmap(GL_TEXTURE_2D);
        opengl_check();
    }

    void engine_using_sdl::init()
    {
        // SDL initialization.
        CHECK_EQ(SDL_Init(SDL_INIT_EVERYTHING), 0)
            << "`SDL_Init()` failed with the following error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG),
            0)
            << "`SDL_GL_SetAttribute()` for debug failed with the error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_PROFILE_MASK,
                SDL_GL_CONTEXT_PROFILE_CORE),
            0)
            << "`SDL_GL_SetAttribute()` for profile failed with the error: "
            << SDL_GetError();

        int opengl_major_version { 3 }, opengl_minor_version { 2 };

        CHECK_EQ(
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_MAJOR_VERSION,
                opengl_major_version),
            0)
            << "`SDL_GL_SetAttribute()` for opengl major"
               " version failed with the error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_MINOR_VERSION,
                opengl_minor_version),
            0)
            << "`SDL_GL_SetAttribute()` for opengl minor"
               " version failed with the error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_GL_GetAttribute(
                SDL_GL_CONTEXT_MAJOR_VERSION,
                &opengl_major_version),
            0)
            << "`SDL_GL_GetAttribute()` for opengl major"
               " version failed with the error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_GL_GetAttribute(
                SDL_GL_CONTEXT_MINOR_VERSION,
                &opengl_minor_version),
            0)
            << "`SDL_GL_GetAttribute()` for opengl minor"
               " version failed with the error: "
            << SDL_GetError();

        CHECK(opengl_major_version == 3) << "Invalid opengl major version";
        CHECK(opengl_minor_version == 2) << "Invalid opengl minor version";

        m_screen_width = 800u;
        m_screen_height = 600u;

        // Window setup.
        m_window = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(
            SDL_CreateWindow(
                "Basic opengl loading texture",
                m_screen_width,
                m_screen_height,
                SDL_WINDOW_OPENGL),
            SDL_DestroyWindow);

        CHECK(m_window)
            << "`SDL_CreateWindow()` failed with the following error: "
            << SDL_GetError();

        CHECK_EQ(
            SDL_SetWindowPosition(
                m_window.get(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED),
            0)
            << "`SDL_SetWindowPosition()` failed with the following error: "
            << SDL_GetError();

        m_opengl_context = std::unique_ptr<void, int (*)(SDL_GLContext)>(
            SDL_GL_CreateContext(m_window.get()),
            SDL_GL_DeleteContext);
        CHECK_NOTNULL(m_opengl_context);

        auto load_opengl_func_pointer = [](const char* func_name) -> void* {
            SDL_FunctionPointer sdl_func_pointer
                = SDL_GL_GetProcAddress(func_name);
            CHECK_NOTNULL(sdl_func_pointer);
            return reinterpret_cast<void*>(sdl_func_pointer);
        };

        CHECK(gladLoadGLES2Loader(load_opengl_func_pointer));

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_message_callback, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_TRUE);

        m_colored_triangle_program.load_shader(GL_VERTEX_SHADER,
                                               "triangle-vertex.shader");
        m_colored_triangle_program.load_shader(GL_FRAGMENT_SHADER,
                                               "triangle-fragment.shader");
        m_colored_triangle_program.prepare_program();

        m_textured_triangle_program.load_shader(GL_VERTEX_SHADER,
                                                "texture-vertex.shader");
        m_textured_triangle_program.load_shader(GL_FRAGMENT_SHADER,
                                                "texture-fragment.shader");
        m_textured_triangle_program.prepare_program();

        glGenBuffers(1, &m_vbo);
        opengl_check();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        opengl_check();

        glGenVertexArrays(1, &m_vao);
        opengl_check();
        glBindVertexArray(m_vao);
        opengl_check();

        glEnable(GL_BLEND);
        opengl_check();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        opengl_check();
    }

    bool engine_using_sdl::process_input(event& event)
    {
        bool continue_event_processing { true };
        SDL_Event sdl_event {};
        std::optional<keyboard_key> keyboard_key_for_event {};

        if (SDL_PollEvent(&sdl_event))
        {
            switch (sdl_event.type)
            {
            case SDL_EVENT_QUIT:
                event.is_quitting = true;
                break;

            case SDL_EVENT_KEY_UP:
                event.device = event_from_device::keyboard;
                keyboard_key_for_event = get_keyboard_key_for_event(sdl_event);
                if (keyboard_key_for_event)
                {
                    event.keyboard_info = keyboard_key_for_event->button_released;
                }
                break;

            case SDL_EVENT_KEY_DOWN:
                event.device = event_from_device::keyboard;
                keyboard_key_for_event = get_keyboard_key_for_event(sdl_event);
                if (keyboard_key_for_event)
                {
                    event.keyboard_info = keyboard_key_for_event->button_pressed;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event.device = event_from_device::mouse;
                event.mouse_info = mouse_event { sdl_event.button.x, sdl_event.button.y };
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                // For now do nothing.
                break;
            }
        }
        else
        {
            continue_event_processing = false;
        }

        return continue_event_processing;
    }

    itexture* engine_using_sdl::create_texture(const std::string_view path)
    {
        itexture* texture = new opengl_texture {};
        texture->load(path);
        return texture;
    }

    void engine_using_sdl::destroy_texture(const itexture* const texture)
    {
        CHECK_NOTNULL(texture);
        delete texture;
    }

    void engine_using_sdl::render(const triangle& triangle)
    {
        glEnableVertexAttribArray(0);
        opengl_check();

        glEnableVertexAttribArray(1);
        opengl_check();

        m_colored_triangle_program.apply_shader_program();

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(triangle.vertices),
            triangle.vertices.data(),
            GL_STATIC_DRAW);
        opengl_check();

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(0));
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        opengl_check();

        glDisableVertexAttribArray(0);
        opengl_check();

        glDisableVertexAttribArray(1);
        opengl_check();
    }

    void engine_using_sdl::render(const triangle& triangle,
                                  itexture* const texture)
    {
        glEnableVertexAttribArray(0);
        opengl_check();

        glEnableVertexAttribArray(1);
        opengl_check();

        glEnableVertexAttribArray(2);
        opengl_check();

        m_textured_triangle_program.apply_shader_program();
        texture->bind();

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(triangle.vertices),
            triangle.vertices.data(),
            GL_STATIC_DRAW);
        opengl_check();

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(0));
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(6 * sizeof(float)));
        opengl_check();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        opengl_check();

        glDisableVertexAttribArray(0);
        opengl_check();

        glDisableVertexAttribArray(1);
        opengl_check();

        glDisableVertexAttribArray(2);
        opengl_check();
    }

    void engine_using_sdl::swap_buffers()
    {
        CHECK(!SDL_GL_SwapWindow(m_window.get()));

        glClearColor(0.f, 1.f, 1.f, 1.f);
        opengl_check();
        glClear(GL_COLOR_BUFFER_BIT);
        opengl_check();
    }

    void engine_using_sdl::uninit()
    {
        SDL_Quit();
    }

    std::pair<size_t, size_t>
    engine_using_sdl::get_screen_resolution() const noexcept
    {
        return { m_screen_width, m_screen_height };
    }

    std::uint64_t engine_using_sdl::get_time_since_epoch() const
    {
        return std::chrono::system_clock::now().time_since_epoch().count();
    }

    std::optional<keyboard_key> engine_using_sdl::get_keyboard_key_for_event(
        const SDL_Event& sdl_event)
    {
        const auto iter = std::find_if(
            keys.begin(),
            keys.end(),
            [&sdl_event](const keyboard_key& k) {
                return sdl_event.key.keysym.sym == k.key_code;
            });

        if (iter != keys.end())
        {
            return std::make_optional(*iter);
        }

        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////

    class engine_instance final
    {
    public:
        engine_instance() = delete;
        engine_instance(const engine_instance&) = delete;
        engine_instance(engine_instance&&) = delete;

        static iengine* get_instance()
        {
            CHECK(!m_is_existing) << "Engine has been already created";
            m_is_existing = true;
            return m_engine;
        }

    private:
        static iengine* m_engine;
        static bool m_is_existing;
    };

    iengine* engine_instance::m_engine = new engine_using_sdl {};
    bool engine_instance::m_is_existing = false;

    ///////////////////////////////////////////////////////////////////////////////

    iengine* engine_create()
    {
        return engine_instance::get_instance();
    }

    ///////////////////////////////////////////////////////////////////////////////

    void engine_destroy(iengine* engine)
    {
        CHECK(engine) << "Invalid engine state on engine destroy";
        delete engine;
    }

    ///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
