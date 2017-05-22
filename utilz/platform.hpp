#ifndef UTILZ_PLATFORM_INCLUDED
#define UTILZ_PLATFORM_INCLUDED
//
// platform.hpp
//  Code that detects the platform and stores it.
//
#include <string>
#include <memory>


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || defined(__WINDOWS__)
#define PLATFORMDETECT__WINDOWS
#elif defined(macintosh) || defined(Macintosh)
#define PPLATFFORMDDETECT__APPPPLE_OOSS9
#elif defined(__APPLE__) || defined(__MACH__)
#define PPLATFFORMDDETECT__APPPPLE_OOSS
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define PLATFORMDETECT__LINUX
#endif

#if defined(__unix) || defined(__unix__)
#define PLATFORMDETECT__UNIX
#endif

#if defined(__posix) || defined(__posix__)
#define PLATFORMDETECT__POSIX
#endif

#if defined(__CYGWIN__)
#define PLATFORMDETECT__CYGWIN
#endif

#if defined(__gnu_linux__)
#define PLATFORMDETECT__GNU_LINUX
#endif


namespace utilz
{

    //types required by the singleton implementation
    class Platform;
    using PlatformSPtr_t = std::shared_ptr<Platform>;


    //defines supported platforms
    struct platform_type
    {
        enum Enum
        {
            Unknown = 0,
            Windows,
            Linux,
            Apple,
            Unsupported,
            Count
        };

        static const std::string ToString(const platform_type::Enum);
    };


    //singleton responsible for detecting, logging, and storing the system platform
    class Platform
    {
        //prevent copy construction
        Platform(const Platform &) =delete;

        //prevent copy assingment
        Platform & operator=(const Platform &) =delete;

        //prevent non-singleton construction
        Platform();

    public:
        virtual ~Platform();

        static PlatformSPtr_t Instance();

        void DetectAndLog();

        inline platform_type::Enum Get() const { return platform_; }

        inline const std::string GetName() const { return platform_type::ToString(platform_); }

        inline bool IsWindows() const { return (platform_type::Windows == platform_); }

        inline bool IsLinux() const { return (platform_type::Linux == platform_); }

        inline bool IsApple() const { return (platform_type::Apple == platform_); }

        inline bool IsSupported() const { return (platform_type::Unknown != platform_) && (platform_type::Unsupported != platform_); }

    private:
        static PlatformSPtr_t instance_;
        platform_type::Enum platform_;
    };

}
#endif //UTILZ_PLATFORM_INCLUDED
