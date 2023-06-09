#include <glog/logging.h>

#include <SDL3/SDL.h>

#include <iostream>

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

struct audio_buffer
{
    Uint8* start { nullptr };
    std::size_t size {};
    std::size_t current_position {};
};

void sdl_audio_callback(void* userdata,
                        Uint8* stream,
                        int len)
{
    CHECK_NOTNULL(userdata);
    audio_buffer* audio_buffer_struct = reinterpret_cast<audio_buffer*>(userdata);

    std::memset(stream, 0, len);

    std::size_t stream_len { static_cast<std::size_t>(len) };

    while (stream_len > 0)
    {
        const Uint8* start_buffer
            = audio_buffer_struct->start
            + audio_buffer_struct->current_position;

        const std::size_t bytes_left_in_buffer
            = audio_buffer_struct->size
            - audio_buffer_struct->current_position;

        if (bytes_left_in_buffer > stream_len)
        {
            SDL_MixAudioFormat(stream, start_buffer, AUDIO_S16LSB, len, 128);
            audio_buffer_struct->current_position += stream_len;
            break;
        }
        else
        {
            SDL_MixAudioFormat(stream, start_buffer, AUDIO_S16LSB, bytes_left_in_buffer, 128);
            audio_buffer_struct->current_position = 0;
            stream_len -= bytes_left_in_buffer;
        }
    }
}

int main(int, char** argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    CHECK(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) == 0);

    std::string music_file_path { "music.wav" };

    SDL_RWops* music_rwop_ptr_file = SDL_RWFromFile(music_file_path.c_str(),
                                                    "rb");

    CHECK(music_rwop_ptr_file) << SDL_GetError();

    SDL_AudioSpec spec_for_music_file {};
    Uint8* music_audio_buffer { nullptr };
    Uint32 music_audio_length {};

    // Load the audio data of a WAVE file into memory.
    SDL_AudioSpec* music_spec = SDL_LoadWAV_RW(music_rwop_ptr_file,
                                               1,
                                               &spec_for_music_file,
                                               &music_audio_buffer,
                                               &music_audio_length);

    CHECK(music_spec) << SDL_GetError();

    audio_buffer music_buffer_in_memory {
        music_audio_buffer, music_audio_length, 0
    };

    LOG(INFO) << "Loaded music file audio spec:";
    LOG(INFO) << spec_for_music_file;

    SDL_AudioSpec desired_spec {
        48000,
        AUDIO_S16LSB,
        1,
        0,
        4096,
        0,
        0,
        sdl_audio_callback,
        &music_buffer_in_memory
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

    // Just play music for a long time.
    SDL_Delay(1990000);

    SDL_PauseAudioDevice(audio_device_id);
    SDL_CloseAudioDevice(audio_device_id);

    SDL_free(music_audio_buffer);
    SDL_Quit();

    return EXIT_SUCCESS;
}
