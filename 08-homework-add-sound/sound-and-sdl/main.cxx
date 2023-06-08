#include <glog/logging.h>

#include <SDL3/SDL.h>

int main(int, char** argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    return EXIT_SUCCESS;
}
