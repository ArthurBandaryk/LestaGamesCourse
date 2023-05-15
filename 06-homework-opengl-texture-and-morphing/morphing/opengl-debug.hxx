#pragma once

#include "glad/glad.h"

//
#include <glog/logging.h>

//
#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace arci
{

    ///////////////////////////////////////////////////////////////////////////////

    static std::string source_msg_enum_to_string(const GLenum source_msg);
    static std::string type_msg_enum_to_string(const GLenum type_msg);
    static std::string severity_msg_enum_to_string(const GLenum severity_msg);

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDebugMessageCallback.xhtml
    [[maybe_unused]] static void APIENTRY opengl_message_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        [[maybe_unused]] const void* userParam)
    {
        CHECK(length < GL_MAX_DEBUG_MESSAGE_LENGTH)
            << "Debug opengl message must be less"
            << " than `GL_MAX_DEBUG_MESSAGE_LENGTH`";

        std::string debug_msg_to_stderr {};
        debug_msg_to_stderr.resize(GL_MAX_DEBUG_MESSAGE_LENGTH);
        std::copy(message, message + length, debug_msg_to_stderr.begin());

        LOG(ERROR) << "Message id: " << id;
        LOG(ERROR) << source_msg_enum_to_string(source);
        LOG(ERROR) << type_msg_enum_to_string(type);
        LOG(ERROR) << severity_msg_enum_to_string(severity);
        LOG(ERROR) << debug_msg_to_stderr;
    }

    static std::string source_msg_enum_to_string(const GLenum source_msg)
    {
        std::string result { "Message source: " };

        switch (source_msg)
        {
        case GL_DEBUG_SOURCE_API:
            result += "calls to the OpenGL API\n";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            result += "calls to a window-system API\n";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            result += "a compiler for a shading language\n";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            result += "a third party application associated with OpenGL\n";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            result += "a source application associated with OpenGL\n";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            result += "some other source\n";
            break;
        default:
            result += "unknown source\n";
            break;
        }

        return result;
    }

    static std::string type_msg_enum_to_string(const GLenum type_msg)
    {
        std::string result { "Message type: " };

        switch (type_msg)
        {
        case GL_DEBUG_TYPE_ERROR:
            result += "an error, typically from the API\n";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            result += "some behavior marked deprecated has been used\n";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            result += "something has invoked undefined behavior\n";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            result
                += "some functionality the user relies upon is not portable\n";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            result += "code has triggered possible performance issues\n";
            break;
        case GL_DEBUG_TYPE_MARKER:
            result += "command stream annotation\n";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            result += "group pushing\n";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            result += "group popping\n";
            break;
        case GL_DEBUG_TYPE_OTHER:
            result += "some other type\n";
            break;
        default:
            result += "unknown type\n";
            break;
        }

        return result;
    }

    static std::string severity_msg_enum_to_string(const GLenum severity_msg)
    {
        std::string result { "Message severity: " };

        switch (severity_msg)
        {
        case GL_DEBUG_SEVERITY_HIGH:
            result += "HIGH. All OpenGL Errors, shader compilation/linking errors,"
                      " or highly-dangerous undefined behavior\n";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            result += "MEDIUM. Major performance warnings, "
                      "shader compilation/linking warnings, "
                      "or the use of deprecated functionality\n";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            result += "LOW. Redundant state change performance warning, "
                      "or unimportant undefined behavior\n";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            result += "NOTIFICATION. Anything that isn't an error or performance issue\n";
            break;
        default:
            result += "UNKNOWN\n";
            break;
        }

        return result;
    }

    inline static void opengl_check()
    {
        const GLenum error = glGetError();

        if (error == GL_NO_ERROR)
        {
            return;
        }

        switch (error)
        {
        case GL_INVALID_ENUM:
            LOG(ERROR) << "An unacceptable value is specified"
                       << " for an enumerated argument";
            break;
        case GL_INVALID_VALUE:
            LOG(ERROR) << "A numeric argument is out of range";
            break;
        case GL_INVALID_OPERATION:
            LOG(ERROR) << "The specified operation is not"
                       << " allowed in the current state";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            LOG(ERROR) << "The framebuffer object is not complete";
            break;
        case GL_OUT_OF_MEMORY:
            LOG(ERROR) << "There is not enough memory left to execute the command";
            break;
        case GL_STACK_UNDERFLOW:
            LOG(ERROR) << "An attempt has been made to perform"
                       << " an operation that would cause an internal"
                       << " stack to underflow";
            break;
        case GL_STACK_OVERFLOW:
            LOG(ERROR) << "An attempt has been made to perform"
                       << " an operation that would cause an internal"
                       << " stack to overflow";
            break;
        default:
            LOG(ERROR) << "Undefined opengl error type";
            break;
        }

        std::exit(1);
    }

    ///////////////////////////////////////////////////////////////////////////////

} // namespace arci

///////////////////////////////////////////////////////////////////////////////
