#include <glog/logging.h>

#include <SDL3/SDL.h>

#include <iostream>
#include <mutex>
#include <string_view>
#include <vector>

std::ostream& operator<<(std::ostream& os, const SDL_AudioSpec& spec);

std::mutex audio_mutex {};

struct audio_buffer
{
    enum class running_mode
    {
        once,
        for_ever
    };

    Uint8* buffer { nullptr };
    Uint32 size {};
    std::size_t current_position {};
    running_mode mode { running_mode::once };
    bool is_running { false };

    audio_buffer(const std::string_view audio_file,
                 const SDL_AudioSpec& desired_audio_spec)
    {
        SDL_RWops* rwop_ptr_file = SDL_RWFromFile(audio_file.data(),
                                                  "rb");
        CHECK(rwop_ptr_file) << SDL_GetError();

        SDL_AudioSpec audio_spec {};

        // Load the audio data of a WAVE file into memory.
        SDL_AudioSpec* music_spec = SDL_LoadWAV_RW(rwop_ptr_file,
                                                   1,
                                                   &audio_spec,
                                                   &buffer,
                                                   &size);

        CHECK(music_spec) << SDL_GetError();

        LOG(INFO) << "Log for " << audio_file << ":";
        LOG(INFO) << audio_spec;

        if (audio_spec.freq != desired_audio_spec.freq
            || audio_spec.channels != desired_audio_spec.channels
            || audio_spec.format != desired_audio_spec.format)
        {
            Uint8* new_converted_buffer { nullptr };
            int new_length {};
            const int status = SDL_ConvertAudioSamples(audio_spec.format,
                                                       audio_spec.channels,
                                                       audio_spec.freq,
                                                       buffer,
                                                       size,
                                                       desired_audio_spec.format,
                                                       desired_audio_spec.channels,
                                                       desired_audio_spec.freq,
                                                       &new_converted_buffer,
                                                       &new_length);

            CHECK(status == 0) << SDL_GetError();
            CHECK_NOTNULL(new_converted_buffer);

            SDL_free(buffer);
            buffer = new_converted_buffer;
            size = new_length;
        }
    }

    ~audio_buffer()
    {
        SDL_free(buffer);
    }

    void play(const running_mode mode)
    {
        std::lock_guard<std::mutex> lock { audio_mutex };
        current_position = 0;
        is_running = true;
        this->mode = mode;
    }
};

std::vector<audio_buffer*> sounds {};

audio_buffer* create_audio_buffer(const std::string_view audio_file,
                                  const SDL_AudioSpec& desired_audio_spec)
{
    audio_buffer* buffer = new audio_buffer { audio_file, desired_audio_spec };

    {
        std::lock_guard<std::mutex> lock { audio_mutex };
        sounds.push_back(buffer);
    }

    return buffer;
}

void sdl_audio_callback([[maybe_unused]] void* userdata,
                        Uint8* stream,
                        int len)
{
    std::lock_guard<std::mutex> lock { audio_mutex };

    std::memset(stream, 0, len);

    for (audio_buffer* sound : sounds)
    {
        if (sound->is_running)
        {
            std::size_t stream_len { static_cast<std::size_t>(len) };

            const Uint8* start_buffer
                = sound->buffer
                + sound->current_position;

            const std::size_t bytes_left_in_buffer
                = sound->size
                - sound->current_position;

            if (bytes_left_in_buffer <= stream_len)
            {
                SDL_MixAudioFormat(stream,
                                   start_buffer,
                                   AUDIO_S16LSB,
                                   bytes_left_in_buffer,
                                   SDL_MIX_MAXVOLUME);
                sound->current_position += bytes_left_in_buffer;
            }
            else
            {
                SDL_MixAudioFormat(stream,
                                   start_buffer,
                                   AUDIO_S16LSB,
                                   stream_len,
                                   SDL_MIX_MAXVOLUME);
                sound->current_position += stream_len;
            }

            if (sound->current_position == sound->size)
            {
                if (sound->mode == audio_buffer::running_mode::for_ever)
                {
                    sound->current_position = 0;
                }
                else
                {
                    sound->is_running = false;
                }
            }
        }
    }
}

int main(int, char** argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    CHECK(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO) == 0);

    SDL_Window* window = SDL_CreateWindow("test-sound", 1024, 768, 0);

    CHECK(window) << SDL_GetError();

    SDL_AudioSpec desired_spec {
        48000,
        AUDIO_S16LSB,
        1,
        0,
        4096,
        0,
        0,
        sdl_audio_callback,
        nullptr
    };

    const char* default_audio_device { nullptr };

    SDL_AudioSpec returned_from_open_audio_device {};

    SDL_AudioDeviceID audio_device_id
        = SDL_OpenAudioDevice(default_audio_device,
                              0,
                              &desired_spec,
                              &returned_from_open_audio_device,
                              0);

    CHECK(audio_device_id != 0) << SDL_GetError();

    LOG(INFO) << "Returned music audio spec: " << returned_from_open_audio_device;

    CHECK(desired_spec.freq == returned_from_open_audio_device.freq)
        << "Desired frequency differs from returned frequency"
        << " from SDL_OpenAudioDevice()";

    CHECK(desired_spec.channels == returned_from_open_audio_device.channels)
        << "Desired number of channels differs from returned number"
        << " from SDL_OpenAudioDevice()";

    CHECK(desired_spec.freq == returned_from_open_audio_device.freq)
        << "Desired frequency differs from returned frequency"
        << " from SDL_OpenAudioDevice()";

    CHECK(desired_spec.format == returned_from_open_audio_device.format)
        << "Desired format differs from returned format"
        << " from SDL_OpenAudioDevice()";

    SDL_PlayAudioDevice(audio_device_id);

    audio_buffer* background_music = create_audio_buffer("music.wav", desired_spec);
    audio_buffer* hit_sound = create_audio_buffer("hit.wav", desired_spec);

    background_music->play(audio_buffer::running_mode::for_ever);

    bool loop_continue { true };

    while (loop_continue)
    {
        SDL_Event sdl_event {};
        if (SDL_PollEvent(&sdl_event))
        {
            switch (sdl_event.type)
            {
            case SDL_EVENT_QUIT:
                loop_continue = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (sdl_event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                {
                    LOG(INFO) << "enter was pressed";
                    hit_sound->play(audio_buffer::running_mode::once);
                }
                break;

            default:
                break;
            }
        }
    }

    SDL_PauseAudioDevice(audio_device_id);
    SDL_CloseAudioDevice(audio_device_id);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

std::ostream& operator<<(std::ostream& os, const SDL_AudioSpec& spec)
{
    os << "frequency: " << spec.freq << "\n";
    os << "format: " << std::hex << spec.format << "\n";
    os << "channels: " << std::dec << static_cast<int>(spec.channels) << "\n";
    os << "silence: " << static_cast<int>(spec.silence) << "\n";
    os << "samples: " << spec.samples << "\n";
    os << "audio buffer size in bytes: " << spec.size << "\n";
    return os;
}
