#include "engine.hxx"
#include "glad/glad.h"
#include "opengl-debug.hxx"
#include "opengl-shader-programm.hxx"
#include "picopng.hxx"

//
#include <SDL3/SDL.h>
//#include <SDL3/SDL_opengl.h>

//
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

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

    struct bind_key
    {
        std::string_view key_name {};
        SDL_KeyCode key_code;
        key_event button_pressed;
        key_event button_released;
        arci::keys key;
    };

    ///////////////////////////////////////////////////////////////////////////////

    const std::array<bind_key, 8> keys {
        bind_key {
            "left",
            SDLK_LEFT,
            key_event::left_button_pressed,
            key_event::left_button_released,
            keys::left,
        },
        bind_key {
            "right",
            SDLK_RIGHT,
            key_event::right_button_pressed,
            key_event::right_button_released,
            keys::right,
        },
        bind_key {
            "up",
            SDLK_UP,
            key_event::up_button_pressed,
            key_event::up_button_released,
            keys::up,
        },
        bind_key {
            "down",
            SDLK_DOWN,
            key_event::down_button_pressed,
            key_event::down_button_released,
            keys::down,
        },
        bind_key {
            "space",
            SDLK_SPACE,
            key_event::button1_pressed,
            key_event::button1_released,
            keys::button1,
        },
        bind_key {
            "+",
            SDLK_KP_PLUS,
            key_event::button2_pressed,
            key_event::button2_released,
            keys::magnify,
        },
        bind_key {
            "-",
            SDLK_KP_MINUS,
            key_event::button3_pressed,
            key_event::button3_released,
            keys::reduce,
        },
        bind_key {
            "escape",
            SDLK_ESCAPE,
            key_event::escape_button_pressed,
            key_event::escape_button_released,
            keys::exit,
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

    class vertex_buffer final : public ivertex_buffer
    {
    public:
        vertex_buffer(const std::vector<triangle>& triangles)
        {
            m_num_vertices = triangles.size() * 3;

            glGenVertexArrays(1, &m_vao_id);
            opengl_check();
            glGenBuffers(1, &m_vbo_id);
            opengl_check();

            bind();

            glBufferData(
                GL_ARRAY_BUFFER,
                triangles.size() * 3 * sizeof(vertex),
                triangles.data()->vertices.data(),
                GL_STATIC_DRAW);
            opengl_check();
        }

        vertex_buffer(const std::vector<vertex>& vertices)
        {
            m_num_vertices = vertices.size();

            glGenVertexArrays(1, &m_vao_id);
            opengl_check();
            glGenBuffers(1, &m_vbo_id);
            opengl_check();

            bind();

            glBufferData(GL_ARRAY_BUFFER,
                         m_num_vertices * sizeof(vertex),
                         vertices.data(),
                         GL_STATIC_DRAW);
            opengl_check();
        }

        ~vertex_buffer()
        {
            glBindVertexArray(0);
            opengl_check();
            glDeleteVertexArrays(1, &m_vao_id);
            opengl_check();
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            opengl_check();
            glDeleteBuffers(1, &m_vbo_id);
            opengl_check();
        }

        void bind() override
        {
            glBindVertexArray(m_vao_id);
            opengl_check();
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
            opengl_check();
        }

        std::size_t get_vertices_number() const override
        {
            return m_num_vertices;
        }

    private:
        GLuint m_vbo_id {};
        GLuint m_vao_id {};
        std::size_t m_num_vertices {};
    };

    class index_buffer : public i_index_buffer
    {
    public:
        index_buffer(const std::vector<uint32_t>& indices)
        {
            m_num_indices = indices.size();

            m_indices.resize(m_num_indices);
            std::copy(indices.begin(), indices.end(), m_indices.begin());

            glGenBuffers(1, &m_ebo_id);
            opengl_check();

            bind();

            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         m_num_indices * sizeof(uint32_t),
                         indices.data(),
                         GL_STATIC_DRAW);
            opengl_check();
        }

        ~index_buffer()
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            opengl_check();
            glDeleteBuffers(1, &m_ebo_id);
            opengl_check();
        }

        void bind() override
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);
            opengl_check();
        }

        uint32_t* data() override
        {
            return m_indices.data();
        }

        std::size_t get_indices_number() const override
        {
            return m_num_indices;
        }

    private:
        std::vector<uint32_t> m_indices {};
        GLuint m_ebo_id {};
        std::size_t m_num_indices {};
    };

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
        void init_imgui() override;
        bool process_input(event& event) override;
        bool key_down(const enum keys key) override;
        void imgui_new_frame() override;
        void imgui_render() override;
        void render(const triangle& triangle) override;
        void render(const triangle& triangle,
                    itexture* const texture) override;
        void render(const triangle& triangle,
                    itexture* const texture,
                    const glm::mediump_mat3& matrix) override;
        void render(ivertex_buffer* vertex_buffer,
                    itexture* const texture,
                    const glm::mediump_mat3& matrix) override;
        void render(ivertex_buffer* vertex_buffer,
                    i_index_buffer* ebo,
                    itexture* const texture,
                    const glm::mediump_mat3& matrix) override;
        itexture* create_texture(const std::string_view path) override;
        void destroy_texture(const itexture* const texture) override;
        ivertex_buffer* create_vertex_buffer(
            const std::vector<triangle>& triangles) override;
        ivertex_buffer* create_vertex_buffer(
            const std::vector<vertex>& vertices) override;
        void destroy_vertex_buffer(ivertex_buffer* buffer) override;
        i_index_buffer* create_ebo(
            const std::vector<uint32_t>& indices) override;
        void destroy_ebo(i_index_buffer* buffer) override;
        void swap_buffers() override;
        void uninit() override;
        void imgui_uninit() override;
        std::pair<size_t, size_t>
        get_screen_resolution() const noexcept override;

        std::uint64_t get_time_since_epoch() const;

    private:
        std::optional<bind_key> get_key_for_event(
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

        m_screen_width = 1024u;
        m_screen_height = 768u;

        // Window setup.
        m_window = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(
            SDL_CreateWindow(
                "Adding imgui",
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

    void engine_using_sdl::init_imgui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

        io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 50.f);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        CHECK(m_window);
        CHECK(m_opengl_context);
        ImGui_ImplSDL3_InitForOpenGL(m_window.get(), m_opengl_context.get());
        ImGui_ImplOpenGL3_Init("#version 100");
    }

    bool engine_using_sdl::process_input(event& event)
    {
        SDL_Event sdl_event {};

        std::optional<bind_key> key_for_event {};

        if (SDL_PollEvent(&sdl_event))
        {
            ImGui_ImplSDL3_ProcessEvent(&sdl_event);

            switch (sdl_event.type)
            {
            case SDL_EVENT_QUIT:
                event.is_quitting = true;
                break;

            case SDL_EVENT_KEY_UP:
                event.device = event_from_device::keyboard;
                key_for_event = get_key_for_event(sdl_event);
                if (key_for_event)
                {
                    event.key_info = key_for_event->button_released;
                }
                break;

            case SDL_EVENT_KEY_DOWN:
                event.device = event_from_device::keyboard;
                key_for_event = get_key_for_event(sdl_event);
                if (key_for_event)
                {
                    event.key_info = key_for_event->button_pressed;
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event.device = event_from_device::mouse;
                event.mouse_info = mouse_event { sdl_event.button.x, sdl_event.button.y };
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                break;

            default:
                break;
            }

            return true;
        }

        return false;
    }

    bool engine_using_sdl::key_down(const enum keys key)
    {
        const auto iter = std::find_if(
            keys.begin(),
            keys.end(),
            [key](const bind_key& bind) {
                return bind.key == key;
            });

        if (iter != keys.end())
        {
            const Uint8* state = SDL_GetKeyboardState(nullptr);
            const SDL_Scancode sdl_scan_code
                = SDL_GetScancodeFromKey(iter->key_code);
            return state[sdl_scan_code];
        }

        return false;
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

    ivertex_buffer* engine_using_sdl::create_vertex_buffer(
        const std::vector<triangle>& triangles)
    {
        return new vertex_buffer { triangles };
    }

    ivertex_buffer* engine_using_sdl::create_vertex_buffer(
        const std::vector<vertex>& vertices)
    {
        return new vertex_buffer { vertices };
    }

    void engine_using_sdl::destroy_vertex_buffer(ivertex_buffer* buffer)
    {
        CHECK_NOTNULL(buffer);
        delete buffer;
    }

    i_index_buffer* engine_using_sdl::create_ebo(const std::vector<uint32_t>& indices)
    {
        return new index_buffer { indices };
    }

    void engine_using_sdl::destroy_ebo(i_index_buffer* buffer)
    {
        CHECK_NOTNULL(buffer);
        delete buffer;
    }

    void engine_using_sdl::imgui_new_frame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void engine_using_sdl::imgui_render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
        glEnableVertexAttribArray(0);
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(1);
        opengl_check();

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(6 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(2);
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

    void engine_using_sdl::render(const triangle& triangle,
                                  itexture* const texture,
                                  const glm::mediump_mat3& matrix)
    {
        m_textured_triangle_program.apply_shader_program();

        m_textured_triangle_program.set_uniform("u_matrix", matrix);
        m_textured_triangle_program.set_uniform("s_texture");

        texture->bind();

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(triangle.vertices),
            triangle.vertices.data(),
            GL_STATIC_DRAW);
        opengl_check();

        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(0));
        opengl_check();
        glEnableVertexAttribArray(0);
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(1);
        opengl_check();

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(6 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(2);
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

    void engine_using_sdl::render(ivertex_buffer* vertex_buffer,
                                  itexture* const texture,
                                  const glm::mediump_mat3& matrix)
    {
        m_textured_triangle_program.apply_shader_program();

        m_textured_triangle_program.set_uniform("u_matrix", matrix);
        m_textured_triangle_program.set_uniform("s_texture");

        texture->bind();
        vertex_buffer->bind();

        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(0));
        opengl_check();
        glEnableVertexAttribArray(0);
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(1);
        opengl_check();

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(6 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(2);
        opengl_check();

        glDrawArrays(GL_TRIANGLES, 0, vertex_buffer->get_vertices_number());
        opengl_check();

        glDisableVertexAttribArray(0);
        opengl_check();

        glDisableVertexAttribArray(1);
        opengl_check();

        glDisableVertexAttribArray(2);
        opengl_check();
    }

    void engine_using_sdl::render(ivertex_buffer* vertex_buffer,
                                  i_index_buffer* ebo,
                                  itexture* const texture,
                                  const glm::mediump_mat3& matrix)
    {
        m_textured_triangle_program.apply_shader_program();

        m_textured_triangle_program.set_uniform("u_matrix", matrix);
        m_textured_triangle_program.set_uniform("s_texture");

        texture->bind();
        vertex_buffer->bind();
        ebo->bind();

        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(0));
        opengl_check();
        glEnableVertexAttribArray(0);
        opengl_check();

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(3 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(1);
        opengl_check();

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex),
            reinterpret_cast<void*>(6 * sizeof(float)));
        opengl_check();
        glEnableVertexAttribArray(2);
        opengl_check();

        glDrawElements(GL_TRIANGLES,
                       ebo->get_indices_number(),
                       GL_UNSIGNED_INT,
                       0);
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

    void engine_using_sdl::imgui_uninit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
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

    std::optional<bind_key> engine_using_sdl::get_key_for_event(
        const SDL_Event& sdl_event)
    {
        const auto iter = std::find_if(
            keys.begin(),
            keys.end(),
            [&sdl_event](const bind_key& k) {
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
