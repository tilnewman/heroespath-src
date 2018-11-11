// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_DISPLAY_HPP_INCLUDED
#define HEROESPATH_GUI_DISPLAY_HPP_INCLUDED
//
// display.hpp
//  Code that manages the display.
//  For example, the window size and color depth, etc.
//
#include "gui/display-change-result.hpp"
#include "gui/resolution.hpp"
#include "misc/not-null.hpp"
#include "stage/i-stage.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>
#include <string>
#include <vector>

namespace sf
{
class RenderWindow;
}

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;

} // namespace stage

namespace gui
{

    class ColoredRectSlider;
    using ColoredRectSliderUPtr_t = std::unique_ptr<ColoredRectSlider>;

    // A Subsystem that retains details pertaining to an sfml application.
    class Display
    {
    public:
        Display(const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL);
        ~Display();

        Display(const Display &) = delete;
        Display(Display &&) = delete;
        Display & operator=(const Display &) = delete;
        Display & operator=(Display &&) = delete;

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

        const sf::FloatRect FullScreenRect() const { return sf::FloatRect(FullScreenRecti()); }

        const sf::IntRect FullScreenRecti() const
        {
            return sf::IntRect(
                0, 0, static_cast<int>(GetWinWidthu()), static_cast<int>(GetWinHeightu()));
        }

        void SetFrameRateLimit(const unsigned LIMIT);

        unsigned GetFrameRateLimit() const { return frameRateLimit_; }

        void SetVerticalSync(const bool WILL_SYNC);

        bool GetVerticalSync() const { return willVerticalSync_; }

        void DrawStage(stage::IStagePtr_t &);

        void TestDraw(const sf::Drawable &);

        const std::vector<sf::Event> PollEvents();

        void TakeScreenshot();

        void Close();

        void SetMouseCursorVisibility(const bool IS_VISIBLE);

        const sf::Vector2i GetMousePosition();

        bool IsOpen() const;

        void ClearToBlack();

        void DisplayFrameBuffer();

        static float GetWinWidthMin() { return 1280.0f; }
        static float GetWinHeightMin() { return 900.0f; }

        // These are not enforced.  They exist only to aid in setting screen positions
        // that are relative to min/max sizes.
        //
        // WARNING:  Changing these values will throw off all relative positions returned by
        // sfutil::MapByRes(), in other words, everything in all UIs will not be positioned
        // correctly!
        //
        // When I finish eliminating all calls to MapByRes() then this crap can be removed.
        static float GetWinWidthMax() { return 7680.0f; }
        static float GetWinHeightMax() { return 4800.0f; }

        static bool IsResolutionListed(const Resolution & RES);
        static bool IsVideoModeListed(const sf::VideoMode & VM);

        static void LogAllFullScreenVideoModes();

        // returns the number of supported fullscreen video modes that were listed.
        static std::size_t LogAllSupportedFullScreenVideoModes();

        static const sf::VideoMode GetCurrentVideoMode();
        static const Resolution GetCurrentResolution();

        const DisplayChangeResult
            ChangeVideoMode(const Resolution & RES, const unsigned ANTIALIAS_LEVEL);

        const DisplayChangeResult
            ChangeVideoMode(const sf::VideoMode & VM, const unsigned ANTIALIAS_LEVEL);

        void FadeOutStart(const sf::Color & COLOR, const float SPEED);
        void FadeInStart(const float SPEED);

        const sf::Color UpdateFadeColor(const float FRAME_TIME_SEC);

        bool IsFadeFinished() const;

        void DrawFade();

    private:
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

        // returns the number of supported fullscreen video modes that were supported.
        static std::size_t ComposeSupportedFullScreenVideoModesVec(ResolutionVec_t & vec);

        static const sf::VideoMode EstablishVideoMode();

        static Resolution ConvertVideoModeToResolution(const sf::VideoMode & VM);

        void UpdateFaderRegion();

    private:
        static std::unique_ptr<Display> instanceUPtr_;

        std::string winTitle_;
        sf::Uint32 winStyle_;
        unsigned frameRateLimit_;
        bool willVerticalSync_;
        std::unique_ptr<sf::RenderWindow> winUPtr_;
        ColoredRectSliderUPtr_t fadeColoredRectSliderUPtr_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_DISPLAY_HPP_INCLUDED
