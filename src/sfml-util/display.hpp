// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_DISPLAY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_DISPLAY_HPP_INCLUDED
//
// display.hpp
//  Code that manages the display.
//  For example, the window size and color depth, etc.
//
#include "misc/not-null.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    class Fade;

    class IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;

    struct DisplayChangeResult
    {
        enum Enum
        {
            // the new video mode was achieved
            Success = 0,

            // failed to set new video mode so reverted back to the original
            FailThenRevert,

            // failed to set new video mode and SFML set the mode back to what it was automatically
            FailNoChange,

            // failed to set new video mode and SFML set the mode to something completely different
            FailChange,

            Count
        };

        static const std::string ToString(const DisplayChangeResult::Enum E);
    };

    // A Subsystem that retains details pertaining to an sfml application.
    class Display
    {
    public:
        Display(const Display &) = delete;
        Display(Display &&) = delete;
        Display & operator=(const Display &) = delete;
        Display & operator=(Display &&) = delete;

    public:
        Display(const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL);
        ~Display();

        static misc::NotNull<Display *> Instance();

        static void Acquire(
            const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL);

        static void Release();

        float GetWinWidth() const;
        float GetWinHeight() const;
        unsigned int GetWinWidthu() const;
        unsigned int GetWinHeightu() const;
        unsigned int WinColorDepth() const;
        unsigned int AntialiasLevel() const;

        const sf::FloatRect FullScreenRect() const
        {
            return sf::FloatRect(0.0f, 0.0f, GetWinWidth(), GetWinHeight());
        }

        const sf::IntRect FullScreenRecti() const
        {
            return sf::IntRect(
                0, 0, static_cast<int>(GetWinWidth()), static_cast<int>(GetWinHeight()));
        }

        void SetFrameRateLimit(const unsigned LIMIT)
        {
            frameRateLimit_ = LIMIT;
            winUPtr_->setFramerateLimit(LIMIT);
        }

        unsigned GetFrameRateLimit() const { return frameRateLimit_; }

        void SetVerticalSync(const bool WILL_SYNC)
        {
            willVerticalSync_ = WILL_SYNC;
            winUPtr_->setVerticalSyncEnabled(WILL_SYNC);
        }
        bool GetVerticalSync() const { return willVerticalSync_; }

        void ConsumeEvents();

        void DrawFader(const Fade &) const;

        void DrawStage(const IStagePtr_t &);

        bool PollEvent(sf::Event & e) { return winUPtr_->pollEvent(e); }

        void TakeScreenshot();

        void Close() { winUPtr_->close(); }

        void SetMouseCursorVisibility(const bool IS_VISIBLE)
        {
            winUPtr_->setMouseCursorVisible(IS_VISIBLE);
        }

        const sf::Vector2i GetMousePosition() { return sf::Mouse::getPosition(*winUPtr_); }

        bool IsOpen() const { return winUPtr_->isOpen(); }

        void ClearToBlack() { winUPtr_->clear(sf::Color::Black); }

        void DisplayFrameBuffer() { winUPtr_->display(); }

        static float GetWinWidthMin() { return 1280.0f; }
        static float GetWinHeightMin() { return 900.0f; }

        // These are not enfoced.  They exist only to aid in setting screen positions
        // that are relative to min/max sizes.
        //
        // WARNING:  Changing these values will throw off all relative positions returned by
        // sfml_util::MapByRes(), in other words, everything in all UIs will not be positioned
        // correctly!
        static float GetWinWidthMax() { return 7680.0f; }
        static float GetWinHeightMax() { return 4800.0f; }

        static bool IsResolutionListed(const Resolution & RES);
        static bool IsVideoModeListed(const sf::VideoMode & VM);

        static bool SetResolutionNameAndRatio(Resolution & res);

        // Decided to allow resolutions that are equal or greater than the
        // min required res.  zTn 2016-10-10
        static bool IsVideoModeSupported(const sf::VideoMode & V)
        {
            return ((V.width >= GetWinWidthMin()) && (V.height >= GetWinHeightMin()));
        }

        static bool IsResolutionSupported(const Resolution & R)
        {
            return ((R.width >= GetWinWidthMin()) && (R.height >= GetWinHeightMin()));
        }

        static bool IsCurrentDesktopResolutionSupported()
        {
            return IsVideoModeSupported(sf::VideoMode::getDesktopMode());
        }

        static void LogAllFullScreenVideoModes();

        // returns the number of supported fullscreen video modes that were supported.
        static std::size_t ComposeSupportedFullScreenVideoModesVec(ResolutionVec_t & vec);

        // returns the number of supported fullscreen video modes that were listed.
        static std::size_t LogAllSupportedFullScreenVideoModes();

        static const sf::VideoMode EstablishVideoMode();

        static const sf::VideoMode GetCurrentVideoMode();
        static const Resolution GetCurrentResolution();

        DisplayChangeResult::Enum
            ChangeVideoMode(const Resolution & RES, const unsigned ANTIALIAS_LEVEL);

        DisplayChangeResult::Enum
            ChangeVideoMode(const sf::VideoMode & VM, const unsigned ANTIALIAS_LEVEL);

        static Resolution ConvertVideoModeToReslution(const sf::VideoMode & VM);

    private:
        static std::unique_ptr<Display> instanceUPtr_;

        std::string winTitle_;
        sf::Uint32 winStyle_;
        unsigned frameRateLimit_;
        bool willVerticalSync_;
        WinUPtr_t winUPtr_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_DISPLAY_HPP_INCLUDED
