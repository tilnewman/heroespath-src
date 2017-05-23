//
// platform.cpp
//
#include "platform.hpp"
#include "game/log-macros.hpp"
#include <exception>
#include <sstream>


namespace utilz
{

    const std::string platform_type::ToString(const platform_type::Enum E)
    {
        switch (E)
        {
            case Unknown:     { return "Unknown"; }
            case Windows:     { return "Windows"; }
            case Linux:       { return "Linux"; }
            case Apple:       { return "Apple"; }
            case Unsupported: { return "Unsupported"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "utilz::Platform::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }


    PlatformSPtr_t Platform::instance_(nullptr);


    Platform::Platform()
    :
        platform_(platform_type::Unknown)
    {}


    Platform::~Platform()
    {}


    PlatformSPtr_t Platform::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new Platform );

        return instance_;
    }


    void Platform::DetectAndLog()
    {
        std::ostringstream ss;
        platform_ = platform_type::Unknown;

#ifdef PLATFORM_DETECTED_IS_WINDOWS
        ss << "  Windows";
        platform_ = platform_type::Windows;
#endif

#ifdef PLATFORM_DETECTED_IS_APPLE9
        ss << "  APPLE (OS9)";
        platform_ = platform_type::Unsupported;
#endif

#ifdef PLATFORM_DETECTED_IS_APPLE
        ss << "  APPLE (OS)";
        platform_ = platform_type::Apple;
#endif

#ifdef PLATFORM_DETECTED_IS_LINUX
        ss << "  Linux";
        platform_ = platform_type::Linux;
#endif

#ifdef PLATFORM_DETECTED_IS_UNIX
        ss << "  Unix";
#endif

#ifdef PLATFORM_DETECTED_IS_POSIX
        ss << "  POSIX";
#endif

#ifdef PLATFORM_DETECTED_IS_CYGWIN
        ss << "  CYGWIN";
#endif

#ifdef PLATFORM_DETECTED_IS_LINUX_GNU
        ss << "  GNU/Linux";
#endif

        if (ss.str().empty())
            ss << "(None?  This platform is probably unsupported...)";

        M_HP_LOG("Platform(s) Detected:  " << ss.str() << "  The platform_type has been set to " << GetName() << ".");
    }

}
