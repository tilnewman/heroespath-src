#ifndef SFMLUTIL_DISPLAY_INCLUDED
#define SFMLUTIL_DISPLAY_INCLUDED
//
// display.hpp
//  Code that manages the display.
//  For example, the window size and color depth, etc.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include <memory>


namespace sfml_util
{

    struct DisplayChangeResult
    {
        enum Enum
        {
            Success = 0,     //the new video mode was achieved
            FailThenRevert, //failed to set new video mode so reverted back to the original
            FailNoChange,  //failed to set new video mode and SFML set the mode back to what it was automatically
            FailChange,   //failed to set new video mode and SFML set the mode to something completely different
            Count
        };

        static const std::string ToString(const DisplayChangeResult::Enum E);
    };


    //types required by the singleton implementation
    class Display;
    using DisplaySPtr_t = std::shared_ptr<Display>;


    //A singleton that retains details pertaining to an sfml application.
    class Display
    {
        //prevent copy construction
        Display(const Display &);

        //prevent copy assignment
        Display & operator=(const Display &);

        //prevent non-singleton construction
        Display();

    public:
        virtual ~Display();
        static DisplaySPtr_t Instance();

        inline WinSPtr_t GetWindow() const         { return winSPtr_; }
        inline void SetWindow(WinSPtr_t & winSPtr) { winSPtr_ = winSPtr; }

        float        GetWinWidth() const;
        float        GetWinHeight() const;
        unsigned int GetWinWidthu() const;
        unsigned int GetWinHeightu() const;
        unsigned int WinColorDepth() const;
        unsigned int AntialiasLevel() const;

        inline const sf::FloatRect FullScreenRect() const   { return sf::FloatRect(0.0f, 0.0f, GetWinWidth(), GetWinHeight()); }
        inline const sf::IntRect FullScreenRecti() const    { return sf::IntRect(0, 0, static_cast<int>(GetWinWidth()), static_cast<int>(GetWinHeight())); }

        inline void SetWindowTitle(const std::string & TITLE_STR)   { winTitle_ = TITLE_STR; }
        inline const std::string GetWindowTitle() const             { return winTitle_; }

        inline void SetWindowStyle(const sf::Uint32 STYLE)  { winStyle_ = STYLE; }
        inline sf::Uint32 GetWindowStyle() const            { return winStyle_;  }

        void SetFrameRateLimit(const int LIMIT);
        inline int GetFrameRateLimit() const    { return frameRateLimit_; }

        void SetVerticalSync(const bool WILL_SYNC);
        inline bool GetVerticalSync() const     { return willVerticalSync_; }

        inline static float GetWinWidthMin()    { return 1280.0f; }
        inline static float GetWinHeightMin()   { return 900.0f;  }

        //These are not enfoced.  They exist only to aid in setting screen positions that are relative to min/max sizes.
        inline static float GetWinWidthMax()    { return 7680.0f; }
        inline static float GetWinHeightMax()   { return 4800.0f; }

        static bool IsResolutionListed(const Resolution & RES);
        static bool IsVideoModeListed(const sf::VideoMode & VM);

        static bool SetResolutionNameAndRatio(Resolution & res);

        //Decided to allow resolutions that are equal or greater than the min required res.  zTn 2016-10-10
        inline static bool IsVideoModeSupported(const sf::VideoMode & V)    { return ((V.width >= GetWinWidthMin()) && (V.height >= GetWinHeightMin())); }
        inline static bool IsResolutionSupported(const Resolution & R)      { return ((R.width >= GetWinWidthMin()) && (R.height >= GetWinHeightMin())); }
        inline static bool IsCurrentDesktopResolutionSupported()            { return IsVideoModeSupported(sf::VideoMode::getDesktopMode()); }

        static void LogAllFullScreenVideoModes();

        //returns the number of supported fullscreen video modes that were supported.
        static std::size_t ComposeSupportedFullScreenVideoModesVec(ResolutionVec_t & vec);

        //returns the number of supported fullscreen video modes that were listed.
        static std::size_t LogAllSupportedFullScreenVideoModes();

        static const sf::VideoMode EstablishVideoMode();

        static sfml_util::WinSPtr_t OpenRenderWindow(const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL = 0);

        static const sf::VideoMode GetCurrentVideoMode();
        static const Resolution    GetCurrentResolution();

        static DisplayChangeResult::Enum ChangeVideoMode(const Resolution & RES, const unsigned ANTIALIAS_LEVEL);

        static DisplayChangeResult::Enum ChangeVideoMode(const sf::VideoMode & VM, const unsigned ANTIALIAS_LEVEL);

        static Resolution ConvertVideoModeToReslution(const sf::VideoMode & VM);

    private:
        static DisplaySPtr_t instanceSPtr_;
        //
        WinSPtr_t        winSPtr_;
        PopupImage::Enum whichPopup_;
        std::string      winTitle_;
        sf::Uint32       winStyle_;
        int              frameRateLimit_;
        bool             willVerticalSync_;
    };

}
#endif //SFMLUTIL_DISPLAY_INCLUDED
