// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// display.cpp
//
#include "display.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "sfml-util/date-time.hpp"
#include "sfml-util/i-stage.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    const std::string DisplayChangeResult::ToString(const DisplayChangeResult::Enum E)
    {
        switch (E)
        {
            case Success: { return "Success";
            }
            case FailThenRevert: { return "FailThenRevert";
            }
            case FailNoChange: { return "FailNoChange";
            }
            case FailChange: { return "FailChange";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::DisplayChangeResult::Enum::ToString(" << E << ")_UnknownValue";
                throw std::range_error(ss.str());
            }
        }
    }

    std::unique_ptr<Display> Display::instanceUPtr_;

    Display::Display(
        const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL)
        : winTitle_(TITLE)
        , winStyle_(STYLE)
        , frameRateLimit_(0)
        , willVerticalSync_(false)
        , winUPtr_(std::make_unique<sf::RenderWindow>(
              EstablishVideoMode(), TITLE, STYLE, sf::ContextSettings(0, 0, ANTIALIAS_LEVEL)))
    {
        M_HP_LOG_DBG("Subsystem Construction: Display");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            winUPtr_->isOpen(),
            "Unable to open render window.  Check console for explanation.  Bail.");

        M_HP_LOG(
            "Window open " << winUPtr_->getSize().x << "x" << winUPtr_->getSize().y
                           << " with color depth reported as " << winUPtr_->getSettings().depthBits
                           << ((0 == winUPtr_->getSettings().depthBits)
                                   ? "(which is really 32...), "
                                   : ", ")
                           << "AA=" << winUPtr_->getSettings().antialiasingLevel << ".");

        if (winUPtr_->getSettings().antialiasingLevel != ANTIALIAS_LEVEL)
        {
            M_HP_LOG(
                "Window antialias level changed automatically from requested "
                << ANTIALIAS_LEVEL << " to " << winUPtr_->getSettings().antialiasingLevel << ".");
        }
    }

    Display::~Display()
    {
        if (winUPtr_->isOpen())
        {
            winUPtr_->close();
        }
    }

    misc::NotNull<Display *> Display::Instance()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "sfml_util::Display::Instance() found instanceUPtr that was null.");

        return instanceUPtr_.get();
    }

    void Display::Acquire(
        const std::string & TITLE, const sf::Uint32 STYLE, const unsigned ANTIALIAS_LEVEL)
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<Display>(TITLE, STYLE, ANTIALIAS_LEVEL);
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: Display");
        }
    }

    void Display::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "sfml_util::Display::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    float Display::GetWinWidth() const { return static_cast<float>(winUPtr_->getSize().x); }

    float Display::GetWinHeight() const { return static_cast<float>(winUPtr_->getSize().y); }

    unsigned int Display::GetWinWidthu() const { return winUPtr_->getSize().x; }

    unsigned int Display::GetWinHeightu() const { return winUPtr_->getSize().y; }

    unsigned int Display::WinColorDepth() const { return winUPtr_->getSettings().depthBits; }

    unsigned int Display::AntialiasLevel() const
    {
        return winUPtr_->getSettings().antialiasingLevel;
    }

    void Display::SetFrameRateLimit(const unsigned LIMIT)
    {
        frameRateLimit_ = LIMIT;
        winUPtr_->setFramerateLimit(LIMIT);
    }

    void Display::SetVerticalSync(const bool WILL_SYNC)
    {
        willVerticalSync_ = WILL_SYNC;
        winUPtr_->setVerticalSyncEnabled(WILL_SYNC);
    }

    void Display::ConsumeEvents()
    {
        sf::Event ignored;
        while (winUPtr_->pollEvent(ignored))
        {
        }
    }

    void Display::DrawStage(const IStagePtr_t & ISTAGE_PTR)
    {
        ISTAGE_PTR->Draw(*winUPtr_, sf::RenderStates());
    }

    void Display::TakeScreenshot()
    {
        // acquire screenshot
        const auto WINDOW_SIZE { winUPtr_->getSize() };
        sf::Texture texture;
        if (texture.create(WINDOW_SIZE.x, WINDOW_SIZE.y) == false)
        {
            M_HP_LOG_ERR(
                "Display::TakeScreenshot()'s texture.create("
                << WINDOW_SIZE.x << "x" << WINDOW_SIZE.y << ") "
                << "returned false.  Unable to take screenshot.");

            return;
        }

        texture.update(*winUPtr_);

        const auto SCREENSHOT_IMAGE { texture.copyToImage() };

        // establish the path
        const auto DIR_PATH_STR { misc::filesystem::AppendPathsToCurrentThenClean("screenshots") };

        // create directory if missing
        if (misc::filesystem::ExistsAndIsDirectory(DIR_PATH_STR) == false)
        {
            misc::filesystem::CreateDirectory(DIR_PATH_STR);
        }

        // compose the filename
        const auto DATE_TIME { sfml_util::DateTime::CurrentDateTime() };
        std::ostringstream ssFileName;
        ssFileName << "heroespath_screenshot_" << DATE_TIME.date.year << "-" << DATE_TIME.date.month
                   << "-" << DATE_TIME.date.day << "_" << DATE_TIME.time.hours << ":"
                   << DATE_TIME.time.minutes << ":" << DATE_TIME.time.seconds;

        const auto FILE_NAME_BASE { ssFileName.str() };
        const auto FILE_NAME_EXT { ".png" };

        const auto FILE_PATH_STR { misc::filesystem::FindFirstAvailableNumberedFilenamePath(
            DIR_PATH_STR, FILE_NAME_BASE, FILE_NAME_EXT, "_") };

        if (SCREENSHOT_IMAGE.saveToFile(FILE_PATH_STR) == false)
        {
            M_HP_LOG_ERR(
                "Display::TakeScreenshot() failed screenshot sf::Image::saveToFile(\""
                << FILE_PATH_STR << "\") returned false.  Check the console for a reason...maybe.");
        }
    }

    bool Display::IsResolutionListed(const Resolution & RES)
    {
        Resolution r(RES);
        return SetResolutionNameAndRatio(r);
    }

    bool Display::IsVideoModeListed(const sf::VideoMode & VM)
    {
        Resolution r(VM);
        return SetResolutionNameAndRatio(r);
    }

    bool Display::SetResolutionNameAndRatio(Resolution & res)
    {
        if ((640 == res.width) && (480 == res.height))
        {
            res.name = "VGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((800 == res.width) && (600 == res.height))
        {
            res.name = "SVGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1024 == res.width) && (768 == res.height))
        {
            res.name = "XGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1152 == res.width) && (768 == res.height))
        {
            res.name = "WXGA";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((1152 == res.width) && (864 == res.height))
        {
            res.name = "XGA+";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1280 == res.width) && (768 == res.height))
        {
            res.name = "WXGA";
            res.aspect_ratio = "5:3";
            return true;
        }
        else if ((1280 == res.width) && (800 == res.height))
        {
            res.name = "WXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1280 == res.width) && (960 == res.height))
        {
            res.name = "SXVGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1280 == res.width) && (1024 == res.height))
        {
            res.name = "SXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1360 == res.width) && (768 == res.height))
        {
            res.name = "WXGA";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((1366 == res.width) && (768 == res.height))
        {
            res.name = "WXGAHD";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((1400 == res.width) && (1050 == res.height))
        {
            res.name = "SXGA+";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1440 == res.width) && (900 == res.height))
        {
            res.name = "WXGA+";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1440 == res.width) && (960 == res.height))
        {
            res.name = "WXGA+";
            res.aspect_ratio = "15:10";
            return true;
        }
        else if ((1440 == res.width) && (1050 == res.height))
        {
            res.name = "?";
            res.aspect_ratio = "1.371";
            return true;
        }
        else if ((1440 == res.width) && (1080 == res.height))
        {
            res.name = "HDV";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1600 == res.width) && (900 == res.height))
        {
            res.name = "HD+";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((1600 == res.width) && (1024 == res.height))
        {
            res.name = "1600SW";
            res.aspect_ratio = "25:16";
            return true;
        }
        else if ((1600 == res.width) && (1050 == res.height))
        {
            res.name = "WSXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1600 == res.width) && (1200 == res.height))
        {
            res.name = "UXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1600 == res.width) && (1280 == res.height))
        {
            res.name = "Sun3";
            res.aspect_ratio = "5:4";
            return true;
        }
        else if ((1680 == res.width) && (1050 == res.height))
        {
            res.name = "WSXGA+";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1792 == res.width) && (1344 == res.height))
        {
            res.name = "?";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1800 == res.width) && (1440 == res.height))
        {
            res.name = "?";
            res.aspect_ratio = "5:4";
            return true;
        }
        else if ((1856 == res.width) && (1392 == res.height))
        {
            res.name = "?";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((1920 == res.width) && (1080 == res.height))
        {
            res.name = "1K";
            res.aspect_ratio = "9:4";
            return true;
        }
        else if ((1920 == res.width) && (1200 == res.height))
        {
            res.name = "WUXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1920 == res.width) && (1280 == res.height))
        {
            res.name = "FullHD+";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((1920 == res.width) && (1400 == res.height))
        {
            res.name = "TXGA";
            res.aspect_ratio = "7:5";
            return true;
        }
        else if ((1920 == res.width) && (1440 == res.height))
        {
            res.name = "?";
            res.aspect_ratio = "21:9";
            return true;
        }
        else if ((2000 == res.width) && (1280 == res.height))
        {
            res.name = "QXGA";
            res.aspect_ratio = "1.56";
            return true;
        }
        else if ((2048 == res.width) && (1080 == res.height))
        {
            res.name = "DCI2K";
            res.aspect_ratio = "19:10";
            return true;
        }
        else if ((2048 == res.width) && (1152 == res.height))
        {
            res.name = "QXGA";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((2048 == res.width) && (1536 == res.height))
        {
            res.name = "2K";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((2160 == res.width) && (1440 == res.height))
        {
            res.name = "MSSP3";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((2304 == res.width) && (1440 == res.height))
        {
            res.name = "...";
            res.aspect_ratio = "8:5";
            return true;
        }
        else if ((2560 == res.width) && (1080 == res.height))
        {
            res.name = "...";
            res.aspect_ratio = "21:9";
            return true;
        }
        else if ((2560 == res.width) && (1440 == res.height))
        {
            res.name = "WQHD";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((2560 == res.width) && (1600 == res.height))
        {
            res.name = "WQXGA";
            res.aspect_ratio = "8:5";
            return true;
        }
        else if ((2560 == res.width) && (1700 == res.height))
        {
            res.name = "CHB_P";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((2560 == res.width) && (1800 == res.height))
        {
            res.name = "PIXELC";
            res.aspect_ratio = "64:45";
            return true;
        }
        else if ((2560 == res.width) && (1920 == res.height))
        {
            res.name = "P225F";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((2560 == res.width) && (2048 == res.height))
        {
            res.name = "QSXGA";
            res.aspect_ratio = "5:4";
            return true;
        }
        else if ((2736 == res.width) && (1824 == res.height))
        {
            res.name = "MSPRO4";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((2732 == res.width) && (2048 == res.height))
        {
            res.name = "iPadPro";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((2840 == res.width) && (1536 == res.height))
        {
            res.name = "QXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((2880 == res.width) && (900 == res.height))
        {
            res.name = "CWSXGA";
            res.aspect_ratio = "16:5";
            return true;
        }
        else if ((2880 == res.width) && (1800 == res.height))
        {
            res.name = "Retina";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((2880 == res.width) && (2100 == res.height))
        {
            res.name = "QSXGA+";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((3000 == res.width) && (2000 == res.height))
        {
            res.name = "MsBook";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((3200 == res.width) && (1800 == res.height))
        {
            res.name = "WQXGA+";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((3200 == res.width) && (2048 == res.height))
        {
            res.name = "WQSXGA";
            res.aspect_ratio = "25:16";
            return true;
        }
        else if ((3200 == res.width) && (2400 == res.height))
        {
            res.name = "QUXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((3840 == res.width) && (2160 == res.height))
        {
            res.name = "4K";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((3840 == res.width) && (2400 == res.height))
        {
            res.name = "WQUXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((4096 == res.width) && (2160 == res.height))
        {
            res.name = "4KHD";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((4096 == res.width) && (2304 == res.height))
        {
            res.name = "4K";
            res.aspect_ratio = "21:9";
            return true;
        }
        else if ((4096 == res.width) && (3072 == res.height))
        {
            res.name = "HXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((4500 == res.width) && (3000 == res.height))
        {
            res.name = "SurStd";
            res.aspect_ratio = "3:2";
            return true;
        }
        else if ((5120 == res.width) && (2160 == res.height))
        {
            res.name = "5K";
            res.aspect_ratio = "21:9";
            return true;
        }
        else if ((5120 == res.width) && (2700 == res.height))
        {
            res.name = "5K";
            res.aspect_ratio = "19:10";
            return true;
        }
        else if ((5120 == res.width) && (2880 == res.height))
        {
            res.name = "UHD+";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((5120 == res.width) && (3200 == res.height))
        {
            res.name = "WHXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((5120 == res.width) && (3840 == res.height))
        {
            res.name = "5K";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((5120 == res.width) && (4096 == res.height))
        {
            res.name = "HSXGA";
            res.aspect_ratio = "5:4";
            return true;
        }
        else if ((6400 == res.width) && (4096 == res.height))
        {
            res.name = "WHSXGA";
            res.aspect_ratio = "25:16";
            return true;
        }
        else if ((6400 == res.width) && (4800 == res.height))
        {
            res.name = "HUXGA";
            res.aspect_ratio = "4:3";
            return true;
        }
        else if ((7680 == res.width) && (4320 == res.height))
        {
            res.name = "8K_UHD";
            res.aspect_ratio = "16:9";
            return true;
        }
        else if ((7680 == res.width) && (4800 == res.height))
        {
            res.name = "WHUXGA";
            res.aspect_ratio = "16:10";
            return true;
        }
        else if ((8192 == res.width) && (4608 == res.height))
        {
            res.name = "8K";
            res.aspect_ratio = "16:9";
            return true;
        }
        {
            res.name = "?";
            res.aspect_ratio = "?";
            return false;
        }
    }

    void Display::LogAllFullScreenVideoModes()
    {
        M_HP_LOG("Listing ALL full-screen video modes this display is capable of:");

        std::vector<sf::VideoMode> fullScreenVideoModes(sf::VideoMode::getFullscreenModes());

        for (const auto & NEXT_VIDEO_MODE : fullScreenVideoModes)
            M_HP_LOG("\t" << ConvertVideoModeToReslution(NEXT_VIDEO_MODE).ToString());
    }

    std::size_t Display::ComposeSupportedFullScreenVideoModesVec(ResolutionVec_t & vec)
    {
        bool isCurrentVideoModeListed(false);
        const sf::VideoMode CURRENT_VIDEO_MODE(GetCurrentVideoMode());

        std::vector<sf::VideoMode> fullScreenSupportedModesVec(sf::VideoMode::getFullscreenModes());

        for (const auto & NEXT_VIDEO_MODE : fullScreenSupportedModesVec)
        {
            const Resolution R(ConvertVideoModeToReslution(NEXT_VIDEO_MODE));
            if (IsResolutionSupported(R))
                vec.emplace_back(R);

            if ((CURRENT_VIDEO_MODE.width == R.width) && (CURRENT_VIDEO_MODE.height == R.height))
            {
                isCurrentVideoModeListed = true;
            }
        }

        // add the current video mode if supported and not already listed
        if (IsCurrentDesktopResolutionSupported() && (false == isCurrentVideoModeListed))
            vec.emplace_back(ConvertVideoModeToReslution(sf::VideoMode::getDesktopMode()));

        return vec.size();
    }

    std::size_t Display::LogAllSupportedFullScreenVideoModes()
    {
        M_HP_LOG("Listing all SUPPORTED full-screen video modes this display is capable of:");

        ResolutionVec_t supportedFullScreenResolutionsVec;
        const std::size_t NUM_SUPPORTED_RESOLUTIONS(
            ComposeSupportedFullScreenVideoModesVec(supportedFullScreenResolutionsVec));

        if (0 == NUM_SUPPORTED_RESOLUTIONS)
        {
            M_HP_LOG(
                "THERE ARE NO SUPPORTED MODES!  Minimum resolution required is "
                << GetWinWidthMin() << "x" << GetWinHeightMin() << ".");
        }
        else
        {
            for (std::size_t i(0); i < NUM_SUPPORTED_RESOLUTIONS; ++i)
            {
                M_HP_LOG("\t" << supportedFullScreenResolutionsVec[i].ToString());
            }
        }

        return NUM_SUPPORTED_RESOLUTIONS;
    }

    const sf::VideoMode Display::EstablishVideoMode()
    {
        sf::VideoMode videoMode;

        const sf::VideoMode DESKTOP_REPORTED_MODE(sf::VideoMode::getDesktopMode());

        const sf::VideoMode CURR_VIDEO_MODE(
            DESKTOP_REPORTED_MODE.width,
            DESKTOP_REPORTED_MODE.height,
            ((0 == DESKTOP_REPORTED_MODE.bitsPerPixel) ? 32 : DESKTOP_REPORTED_MODE.bitsPerPixel));

        if (IsCurrentDesktopResolutionSupported())
        {
            M_HP_LOG(
                "Current video mode " << ConvertVideoModeToReslution(CURR_VIDEO_MODE).ToString()
                                      << " is supported, and will be used.");

            videoMode = CURR_VIDEO_MODE;
        }
        else
        {
            M_HP_LOG(
                "Current video mode "
                << ConvertVideoModeToReslution(CURR_VIDEO_MODE).ToString()
                << " is NOT supported.  The resolution must be greater, at least "
                << GetWinWidthMin() << "x" << GetWinHeightMin()
                << ".  Below is a list of supported modes you can use instead.");

            LogAllSupportedFullScreenVideoModes();

            ResolutionVec_t supportedResolutionsVec;
            const std::size_t NUM_SUPPORTED(
                ComposeSupportedFullScreenVideoModesVec(supportedResolutionsVec));

            if (0 == NUM_SUPPORTED)
            {
                M_HP_LOG(
                    "No valid video modes found that are supported by this system.  "
                    << "See above for a list of supported video modes.  Attempt to increase "
                    << "your resolution to one of these.");

                throw std::runtime_error(
                    "No valid video modes found that are supported by this system.");
            }
            else
            {
                M_HP_LOG(
                    "Anoter supported video mode "
                    << supportedResolutionsVec[0].ToString()
                    << " was found to be compatible with your hardware, and will be used.");

                videoMode.width = supportedResolutionsVec[0].width;
                videoMode.height = supportedResolutionsVec[0].height;
                videoMode.bitsPerPixel = CURR_VIDEO_MODE.bitsPerPixel;
            }
        }

        return videoMode;
    }

    const sf::VideoMode Display::GetCurrentVideoMode()
    {
        sf::VideoMode currentVideoMode(sf::VideoMode::getDesktopMode());

        // for some reason SFML seems to have the bpp set to zero
        // when it is really 32... zTn 2016-10-10
        if (0 == currentVideoMode.bitsPerPixel)
        {
            currentVideoMode.bitsPerPixel = 32;
        }

        return currentVideoMode;
    }

    const Resolution Display::GetCurrentResolution()
    {
        return ConvertVideoModeToReslution(GetCurrentVideoMode());
    }

    DisplayChangeResult::Enum
        Display::ChangeVideoMode(const Resolution & RES_PARAM, const unsigned ANTIALIAS_LEVEL)
    {
        Resolution resToUse(RES_PARAM);
        SetResolutionNameAndRatio(resToUse);

        return ChangeVideoMode(
            sf::VideoMode(
                resToUse.width,
                resToUse.height,
                ((0 == winUPtr_->getSettings().depthBits) ? 32
                                                          : winUPtr_->getSettings().depthBits)),
            ANTIALIAS_LEVEL);
    }

    DisplayChangeResult::Enum Display::ChangeVideoMode(
        const sf::VideoMode & VM_PARAM, const unsigned ANTIALIAS_LEVEL_PARAM)
    {
        // construct the intended video mode with 32bpp if the bpp was originally zero
        const sf::VideoMode INTENDED_VIDEO_MODE(
            VM_PARAM.width,
            VM_PARAM.height,
            ((0 == VM_PARAM.bitsPerPixel) ? 32 : VM_PARAM.bitsPerPixel));

        const auto INTENDED_ANTIALIAS_LEVEL { ANTIALIAS_LEVEL_PARAM };

        // save original video mode for later comparrison
        const auto ORIG_VIDEO_MODE { GetCurrentVideoMode() };
        const auto ORIG_ANTIALIAS_LEVEL { winUPtr_->getSettings().antialiasingLevel };

        if ((INTENDED_VIDEO_MODE == ORIG_VIDEO_MODE)
            && (ORIG_ANTIALIAS_LEVEL == ANTIALIAS_LEVEL_PARAM))
        {
            M_HP_LOG_WRN(
                "Asked to change to a resolution and AA that matches the current"
                << "resolution and AA.  Ignoring...");

            return DisplayChangeResult::Success;
        }

        M_HP_LOG(
            "Changing resolution from "
            << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString()
            << " AA=" << ORIG_ANTIALIAS_LEVEL << " to "
            << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
            << " AA=" << INTENDED_ANTIALIAS_LEVEL << ".");

        winUPtr_->close();

        // re-open
        sf::ContextSettings contextSettings(0, 0, ANTIALIAS_LEVEL_PARAM);

        winUPtr_->create(INTENDED_VIDEO_MODE, winTitle_, winStyle_, contextSettings);

        const bool WAS_SUCCESS(winUPtr_->isOpen());
        if (WAS_SUCCESS)
        {
            const auto NEW_VIDEO_MODE { GetCurrentVideoMode() };
            const auto NEW_ANTIALIAS_LEVEL { winUPtr_->getSettings().antialiasingLevel };

            // for some reason sfml is finiky on some hardware and has a habit of switching
            // back to the original video mode without warning or error
            if ((NEW_VIDEO_MODE == ORIG_VIDEO_MODE)
                && (NEW_ANTIALIAS_LEVEL == ORIG_ANTIALIAS_LEVEL))
            {
                M_HP_LOG_ERR(
                    "Failed to change video mode to the intended "
                    << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
                    << " AA=" << INTENDED_ANTIALIAS_LEVEL
                    << ", and for some reason SFML switched back to the original video mode of "
                    << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString()
                    << " AA=" << ORIG_ANTIALIAS_LEVEL << ".");

                return DisplayChangeResult::FailNoChange;
            }
            else
            {
                if ((NEW_VIDEO_MODE == INTENDED_VIDEO_MODE)
                    && (NEW_ANTIALIAS_LEVEL == INTENDED_ANTIALIAS_LEVEL))
                {
                    M_HP_LOG(
                        "Changed video mode to "
                        << ConvertVideoModeToReslution(NEW_VIDEO_MODE).ToString()
                        << " AA=" << NEW_ANTIALIAS_LEVEL);

                    return DisplayChangeResult::Success;
                }
                else
                {
                    M_HP_LOG_ERR(
                        "Failed to change video mode to "
                        << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
                        << " AA=" << NEW_ANTIALIAS_LEVEL
                        << ". For some unknown reason (ahem) SFML switched to a new video mode of "
                        << ConvertVideoModeToReslution(NEW_VIDEO_MODE).ToString()
                        << " AA=" << NEW_ANTIALIAS_LEVEL << ".");

                    return DisplayChangeResult::FailChange;
                }
            }
        }
        else
        {
            M_HP_LOG_ERR(
                "Failed to change video mode to "
                << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString() << " AA="
                << INTENDED_ANTIALIAS_LEVEL << ".  Reverting back to original video mode of "
                << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString()
                << " AA=" << ORIG_ANTIALIAS_LEVEL << " instead.");

            contextSettings.antialiasingLevel = ORIG_ANTIALIAS_LEVEL;

            winUPtr_->create(ORIG_VIDEO_MODE, winTitle_, winStyle_, contextSettings);

            return DisplayChangeResult::FailThenRevert;
        }
    }

    Resolution Display::ConvertVideoModeToReslution(const sf::VideoMode & VM)
    {
        Resolution res(VM);
        SetResolutionNameAndRatio(res);
        return res;
    }

} // namespace sfml_util
} // namespace heroespath
