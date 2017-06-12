// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// display.cpp
//
#include "display.hpp"

#include "sfml-util/sfml-util.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>
#include <string>
#include <vector>


namespace sfml_util
{

    const std::string DisplayChangeResult::ToString(const DisplayChangeResult::Enum E)
    {
        switch (E)
        {
            case Success:           { return "Success"; }
            case FailThenRevert:    { return "FailThenRevert"; }
            case FailNoChange:      { return "FailNoChange"; }
            case FailChange:        { return "FailChange"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::DisplayChangeResult::Enum::ToString(" << E << ")_UnknownValue";
                throw std::range_error( ss.str() );
            }
        }
    }


    std::unique_ptr<Display> Display::instanceUPtr_{ nullptr };


    Display::Display()
    :
        winUPtr_         (),
        winTitle_        (""),
        winStyle_        (0),
        frameRateLimit_  (0),
        willVerticalSync_(false)
    {
        M_HP_LOG_DBG("Singleton Construction: Display");
    }


    Display::~Display()
    {
        //M_HP_LOG_DBG("Singleton Destruction: Display");
    }


    Display * Display::Instance()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "sfml_util::Display::Instance() found instanceUPtr that was null.");

        return instanceUPtr_.get();
    }


    void Display::Acquire(const std::string & TITLE,
                          const sf::Uint32    STYLE,
                          const unsigned      ANTIALIAS_LEVEL)
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new Display);
            instanceUPtr_->OpenRenderWindow(TITLE, STYLE, ANTIALIAS_LEVEL);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: Display");
        }
    }


    void Display::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "sfml_util::Display::Release() found instanceUPtr that was null.");

        Display::Instance()->CloseRenderWindow();
        instanceUPtr_.reset();
    }


    float Display::GetWinWidth() const
    {
        return static_cast<float>(winUPtr_->getSize().x);
    }


    float Display::GetWinHeight() const
    {
        return static_cast<float>(winUPtr_->getSize().y);
    }


    unsigned int Display::GetWinWidthu() const
    {
        return winUPtr_->getSize().x;
    }


    unsigned int Display::GetWinHeightu() const
    {
        return winUPtr_->getSize().y;
    }


    unsigned int Display::WinColorDepth() const
    {
        return winUPtr_->getSettings().depthBits;
    }


    unsigned int Display::AntialiasLevel() const
    {
        return winUPtr_->getSettings().antialiasingLevel;
    }


    void Display::SetFrameRateLimit(const int LIMIT)
    {
        frameRateLimit_ = LIMIT;
        winUPtr_->setFramerateLimit(static_cast<unsigned int>(LIMIT));
    }


    void Display::SetVerticalSync(const bool WILL_SYNC)
    {
        willVerticalSync_ = WILL_SYNC;
        winUPtr_->setVerticalSyncEnabled(WILL_SYNC);
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
        if      ((640 == res.width)  && (480  == res.height)) { res.name = "VGA"     ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((800 == res.width)  && (600  == res.height)) { res.name = "SVGA"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1024 == res.width) && (768  == res.height)) { res.name = "XGA"     ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1152 == res.width) && (768  == res.height)) { res.name = "WXGA"    ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((1152 == res.width) && (864  == res.height)) { res.name = "XGA+"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1280 == res.width) && (768  == res.height)) { res.name = "WXGA"    ; res.aspect_ratio = "5:3"     ; return true; }
        else if ((1280 == res.width) && (800  == res.height)) { res.name = "WXGA"    ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1280 == res.width) && (960  == res.height)) { res.name = "SXVGA"   ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1280 == res.width) && (1024 == res.height)) { res.name = "SXGA"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1360 == res.width) && (768  == res.height)) { res.name = "WXGA"    ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((1366 == res.width) && (768  == res.height)) { res.name = "WXGAHD"  ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((1400 == res.width) && (1050 == res.height)) { res.name = "SXGA+"   ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1440 == res.width) && (900  == res.height)) { res.name = "WXGA+"   ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1440 == res.width) && (960  == res.height)) { res.name = "WXGA+"   ; res.aspect_ratio = "15:10"   ; return true; }
        else if ((1440 == res.width) && (1050 == res.height)) { res.name = "?"       ; res.aspect_ratio = "1.371"   ; return true; }
        else if ((1440 == res.width) && (1080 == res.height)) { res.name = "HDV"     ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1600 == res.width) && (900  == res.height)) { res.name = "HD+"     ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((1600 == res.width) && (1024 == res.height)) { res.name = "1600SW"  ; res.aspect_ratio = "25:16"   ; return true; }
        else if ((1600 == res.width) && (1050 == res.height)) { res.name = "WSXGA"   ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1600 == res.width) && (1200 == res.height)) { res.name = "UXGA"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1600 == res.width) && (1280 == res.height)) { res.name = "Sun3"    ; res.aspect_ratio = "5:4"     ; return true; }
        else if ((1680 == res.width) && (1050 == res.height)) { res.name = "WSXGA+"  ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1792 == res.width) && (1344 == res.height)) { res.name = "?"       ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1800 == res.width) && (1440 == res.height)) { res.name = "?"       ; res.aspect_ratio = "5:4"     ; return true; }
        else if ((1856 == res.width) && (1392 == res.height)) { res.name = "?"       ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((1920 == res.width) && (1080 == res.height)) { res.name = "1K"      ; res.aspect_ratio = "9:4"     ; return true; }
        else if ((1920 == res.width) && (1200 == res.height)) { res.name = "WUXGA"   ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1920 == res.width) && (1280 == res.height)) { res.name = "FullHD+" ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((1920 == res.width) && (1400 == res.height)) { res.name = "TXGA"    ; res.aspect_ratio = "7:5"     ; return true; }
        else if ((1920 == res.width) && (1440 == res.height)) { res.name = "?"       ; res.aspect_ratio = "21:9"    ; return true; }
        else if ((2000 == res.width) && (1280 == res.height)) { res.name = "QXGA"    ; res.aspect_ratio = "1.56"    ; return true; }
        else if ((2048 == res.width) && (1080 == res.height)) { res.name = "DCI2K"   ; res.aspect_ratio = "19:10"   ; return true; }
        else if ((2048 == res.width) && (1152 == res.height)) { res.name = "QXGA"    ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((2048 == res.width) && (1536 == res.height)) { res.name = "2K"      ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((2160 == res.width) && (1440 == res.height)) { res.name = "MSSP3"   ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((2304 == res.width) && (1440 == res.height)) { res.name = "..."     ; res.aspect_ratio = "8:5"     ; return true; }
        else if ((2560 == res.width) && (1080 == res.height)) { res.name = "..."     ; res.aspect_ratio = "21:9"    ; return true; }
        else if ((2560 == res.width) && (1440 == res.height)) { res.name = "WQHD"    ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((2560 == res.width) && (1600 == res.height)) { res.name = "WQXGA"   ; res.aspect_ratio = "8:5"     ; return true; }
        else if ((2560 == res.width) && (1700 == res.height)) { res.name = "CHB_P"   ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((2560 == res.width) && (1800 == res.height)) { res.name = "PIXELC"  ; res.aspect_ratio = "64:45"   ; return true; }
        else if ((2560 == res.width) && (1920 == res.height)) { res.name = "P225F"   ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((2560 == res.width) && (2048 == res.height)) { res.name = "QSXGA"   ; res.aspect_ratio = "5:4"     ; return true; }
        else if ((2736 == res.width) && (1824 == res.height)) { res.name = "MSPRO4"  ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((2732 == res.width) && (2048 == res.height)) { res.name = "iPadPro" ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((2840 == res.width) && (1536 == res.height)) { res.name = "QXGA"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((2880 == res.width) && (900  == res.height)) { res.name = "CWSXGA"  ; res.aspect_ratio = "16:5"    ; return true; }
        else if ((2880 == res.width) && (1800 == res.height)) { res.name = "Retina"  ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((2880 == res.width) && (2100 == res.height)) { res.name = "QSXGA+"  ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((3000 == res.width) && (2000 == res.height)) { res.name = "MsBook"  ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((3200 == res.width) && (1800 == res.height)) { res.name = "WQXGA+"  ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((3200 == res.width) && (2048 == res.height)) { res.name = "WQSXGA"  ; res.aspect_ratio = "25:16"   ; return true; }
        else if ((3200 == res.width) && (2400 == res.height)) { res.name = "QUXGA"   ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((3840 == res.width) && (2160 == res.height)) { res.name = "4K"      ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((3840 == res.width) && (2400 == res.height)) { res.name = "WQUXGA"  ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((4096 == res.width) && (2160 == res.height)) { res.name = "4KHD"    ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((4096 == res.width) && (2304 == res.height)) { res.name = "4K"      ; res.aspect_ratio = "21:9"    ; return true; }
        else if ((4096 == res.width) && (3072 == res.height)) { res.name = "HXGA"    ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((4500 == res.width) && (3000 == res.height)) { res.name = "SurStd"  ; res.aspect_ratio = "3:2"     ; return true; }
        else if ((5120 == res.width) && (2160 == res.height)) { res.name = "5K"      ; res.aspect_ratio = "21:9"    ; return true; }
        else if ((5120 == res.width) && (2700 == res.height)) { res.name = "5K"      ; res.aspect_ratio = "19:10"   ; return true; }
        else if ((5120 == res.width) && (2880 == res.height)) { res.name = "UHD+"    ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((5120 == res.width) && (3200 == res.height)) { res.name = "WHXGA"   ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((5120 == res.width) && (3840 == res.height)) { res.name = "5K"      ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((5120 == res.width) && (4096 == res.height)) { res.name = "HSXGA"   ; res.aspect_ratio = "5:4"     ; return true; }
        else if ((6400 == res.width) && (4096 == res.height)) { res.name = "WHSXGA"  ; res.aspect_ratio = "25:16"   ; return true; }
        else if ((6400 == res.width) && (4800 == res.height)) { res.name = "HUXGA"   ; res.aspect_ratio = "4:3"     ; return true; }
        else if ((7680 == res.width) && (4320 == res.height)) { res.name = "8K_UHD"  ; res.aspect_ratio = "16:9"    ; return true; }
        else if ((7680 == res.width) && (4800 == res.height)) { res.name = "WHUXGA"  ; res.aspect_ratio = "16:10"   ; return true; }
        else if ((8192 == res.width) && (4608 == res.height)) { res.name = "8K"      ; res.aspect_ratio = "16:9"    ; return true; }
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

        for (auto const & NEXT_VIDEO_MODE : fullScreenVideoModes)
            M_HP_LOG("\t" << ConvertVideoModeToReslution(NEXT_VIDEO_MODE).ToString());
    }


    std::size_t Display::ComposeSupportedFullScreenVideoModesVec(ResolutionVec_t & vec)
    {
        bool isCurrentVideoModeListed(false);
        const sf::VideoMode CURRENT_VIDEO_MODE( GetCurrentVideoMode() );

        std::vector<sf::VideoMode> fullScreenSupportedModesVec(
            sf::VideoMode::getFullscreenModes() );

        for (auto const & NEXT_VIDEO_MODE : fullScreenSupportedModesVec)
        {
            const Resolution R( ConvertVideoModeToReslution(NEXT_VIDEO_MODE) );
            if (IsResolutionSupported(R))
                vec.push_back(R);

            if ((CURRENT_VIDEO_MODE.width == R.width) &&
                (CURRENT_VIDEO_MODE.height == R.height))
            {
                isCurrentVideoModeListed = true;
            }
        }

        //add the current video mode if supported and not already listed
        if (IsCurrentDesktopResolutionSupported() && (false == isCurrentVideoModeListed))
            vec.push_back(ConvertVideoModeToReslution(sf::VideoMode::getDesktopMode()));

        return vec.size();
    }


    std::size_t Display::LogAllSupportedFullScreenVideoModes()
    {
        M_HP_LOG("Listing all SUPPORTED full-screen video modes this display is capable of:");

        ResolutionVec_t supportedFullScreenResolutionsVec;
        const std::size_t NUM_SUPPORTED_RESOLUTIONS(
            ComposeSupportedFullScreenVideoModesVec(supportedFullScreenResolutionsVec) );

        if (0 == NUM_SUPPORTED_RESOLUTIONS)
        {
            M_HP_LOG("THERE ARE NO SUPPORTED MODES!  Minimum resolution required is "
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
            M_HP_LOG("Current video mode "
                << ConvertVideoModeToReslution(CURR_VIDEO_MODE).ToString()
                << " is supported, and will be used.");

            videoMode = CURR_VIDEO_MODE;
        }
        else
        {
            M_HP_LOG("Current video mode "
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
                M_HP_LOG("No valid video modes found that are supported by this system.  "
                    << "See above for a list of supported video modes.  Attempt to increase "
                    << "your resolution to one of these.");

                throw std::runtime_error(
                    "No valid video modes found that are supported by this system.");
            }
            else
            {
                M_HP_LOG("Anoter supported video mode " << supportedResolutionsVec[0].ToString()
                    << " was found to be compatible with your hardware, and will be used.");

                videoMode.width        = supportedResolutionsVec[0].width;
                videoMode.height       = supportedResolutionsVec[0].height;
                videoMode.bitsPerPixel = CURR_VIDEO_MODE.bitsPerPixel;
            }
        }

        return videoMode;
    }


    void Display::OpenRenderWindow(const std::string & TITLE,
                                   const sf::Uint32    STYLE,
                                   const unsigned      ANTIALIAS_LEVEL)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((winUPtr_.get() == nullptr),
            "sfml_util::Display::OpenRenderWindow() called twice.");

        const sf::VideoMode VIDEO_MODE(EstablishVideoMode());

        sf::ContextSettings contextSettings;
        contextSettings.antialiasingLevel = ANTIALIAS_LEVEL;

        winUPtr_ = std::make_unique<sf::RenderWindow>(VIDEO_MODE, TITLE, STYLE, contextSettings);

        M_ASSERT_OR_LOGANDTHROW_SS(winUPtr_->isOpen(),
            "Unable to open render window.  Check console for explanation.  Bail.");

        M_HP_LOG("Window open " << winUPtr_->getSize().x << "x" << winUPtr_->getSize().y
            << " with color depth reported as " << winUPtr_->getSettings().depthBits
            << ((0 == winUPtr_->getSettings().depthBits) ? "(which is really 32...), " : ", " )
            << "AA=" << winUPtr_->getSettings().antialiasingLevel
            << ".");

        if (winUPtr_->getSettings().antialiasingLevel != ANTIALIAS_LEVEL)
        {
            M_HP_LOG("Window antialias level changed automatically from requested "
                << ANTIALIAS_LEVEL << " to " << winUPtr_->getSettings().antialiasingLevel << ".");
        }

        SetWindowTitle(TITLE);
        SetWindowStyle(STYLE);
    }


    void Display::CloseRenderWindow()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((winUPtr_.get() != nullptr),
            "sfml_util::Display::CloseRenderWindow() called before OpenRenderWindow().");

        winUPtr_->close();
        winUPtr_.reset();
    }


    const sf::VideoMode Display::GetCurrentVideoMode()
    {
        sf::VideoMode currentVideoMode(sf::VideoMode::getDesktopMode());

        //for some reason SFML seems to have the bpp set to zero
        //when it is really 32... zTn 2016-10-10
        if (0 == currentVideoMode.bitsPerPixel)
        {
            currentVideoMode.bitsPerPixel = 32;
        }

        return currentVideoMode;
    }


    const Resolution Display::GetCurrentResolution()
    {
        return ConvertVideoModeToReslution( GetCurrentVideoMode() );
    }


    DisplayChangeResult::Enum Display::ChangeVideoMode(const Resolution & RES_PARAM,
                                                       const unsigned ANTIALIAS_LEVEL)
    {
        Resolution resToUse(RES_PARAM);
        SetResolutionNameAndRatio(resToUse);
        auto winPtr{ Instance()->GetWindow() };
        return ChangeVideoMode(
            sf::VideoMode(
                resToUse.width,
                resToUse.height,
                ((0 == winPtr->getSettings().depthBits) ? 32 : winPtr->getSettings().depthBits)),
                ANTIALIAS_LEVEL);
    }


    DisplayChangeResult::Enum Display::ChangeVideoMode(const sf::VideoMode & VM_PARAM,
                                                       const unsigned ANTIALIAS_LEVEL_PARAM)
    {
        //verify window pointer
        auto winPtr{ Instance()->GetWindow() };
        M_ASSERT_OR_LOGANDTHROW_SS((winPtr != nullptr),
            "sfml_util::Display::ChangeVideoMode(to=\""
            << ConvertVideoModeToReslution(VM_PARAM).ToString() << "\", AA="
            << ANTIALIAS_LEVEL_PARAM << ") was called while there was a null window pointer.");

        //construct the intended video mode with 32bpp if the bpp was originally zero
        const sf::VideoMode INTENDED_VIDEO_MODE(
            VM_PARAM.width,
            VM_PARAM.height,
            ((0 == VM_PARAM.bitsPerPixel) ? 32 : VM_PARAM.bitsPerPixel));

        const unsigned INTENDED_ANTIALIAS_LEVEL(ANTIALIAS_LEVEL_PARAM);

        //save original video mode for later comparrison
        const sf::VideoMode ORIG_VIDEO_MODE( GetCurrentVideoMode() );
        const unsigned ORIG_ANTIALIAS_LEVEL(winPtr->getSettings().antialiasingLevel );

        if ((INTENDED_VIDEO_MODE == ORIG_VIDEO_MODE) &&
            (ORIG_ANTIALIAS_LEVEL == ANTIALIAS_LEVEL_PARAM))
        {
            M_HP_LOG("Asked to change to a resolution and AA that matches the current"
                << "resolution and AA.  Ignoring...");

            return DisplayChangeResult::Success;
        }

        M_HP_LOG("Changing resolution from "
            << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString()
            << " AA=" << ORIG_ANTIALIAS_LEVEL << " to "
            << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
            << " AA=" << INTENDED_ANTIALIAS_LEVEL << ".");

        winPtr->close();

        //re-open
        sf::ContextSettings contextSettings(0, 0, ANTIALIAS_LEVEL_PARAM);
        winPtr->create(INTENDED_VIDEO_MODE,
                       Instance()->GetWindowTitle(),
                       Instance()->GetWindowStyle(),
                       contextSettings);

        const bool WAS_SUCCESS(winPtr->isOpen());
        if (WAS_SUCCESS)
        {
            const sf::VideoMode NEW_VIDEO_MODE( GetCurrentVideoMode() );
            const unsigned NEW_ANTIALIAS_LEVEL( winPtr->getSettings().antialiasingLevel );

            //for some reason sfml is finiky on some hardware and has a habit of switching
            //back to the original video mode without warning or error
            if ((NEW_VIDEO_MODE == ORIG_VIDEO_MODE) &&
                (NEW_ANTIALIAS_LEVEL == ORIG_ANTIALIAS_LEVEL))
            {
                M_HP_LOG("Failed to change video mode to the intended "
                    << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
                    << " AA=" << INTENDED_ANTIALIAS_LEVEL
                    << ", and for some reason SFML switched back to the original video mode of "
                    << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString() << " AA="
                    << ORIG_ANTIALIAS_LEVEL << ".");

                return DisplayChangeResult::FailNoChange;
            }
            else
            {
                if ((NEW_VIDEO_MODE == INTENDED_VIDEO_MODE) &&
                    (NEW_ANTIALIAS_LEVEL == INTENDED_ANTIALIAS_LEVEL))
                {
                    M_HP_LOG("Changed video mode to "
                        << ConvertVideoModeToReslution(NEW_VIDEO_MODE).ToString()
                        << " AA=" << NEW_ANTIALIAS_LEVEL);

                    return DisplayChangeResult::Success;
                }
                else
                {
                    M_HP_LOG("Failed to change video mode to "
                        << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
                        << " AA=" << NEW_ANTIALIAS_LEVEL
                        << ". For some unknown reason (ahem) SFML switched to a new video mode of "
                        << ConvertVideoModeToReslution(NEW_VIDEO_MODE).ToString() << " AA="
                        << NEW_ANTIALIAS_LEVEL << ".");

                    return DisplayChangeResult::FailChange;
                }
            }
        }
        else
        {
            M_HP_LOG("Failed to change video mode to "
                << ConvertVideoModeToReslution(INTENDED_VIDEO_MODE).ToString()
                << " AA=" << INTENDED_ANTIALIAS_LEVEL
                << ".  Reverting back to original video mode of "
                << ConvertVideoModeToReslution(ORIG_VIDEO_MODE).ToString()
                << " AA=" << ORIG_ANTIALIAS_LEVEL << " instead.");

            contextSettings.antialiasingLevel = ORIG_ANTIALIAS_LEVEL;
            winPtr->create(ORIG_VIDEO_MODE,
                           Instance()->GetWindowTitle(),
                           Instance()->GetWindowStyle(),
                           contextSettings);

            return DisplayChangeResult::FailThenRevert;
        }
    }


    Resolution Display::ConvertVideoModeToReslution(const sf::VideoMode & VM)
    {
        Resolution res(VM);
        SetResolutionNameAndRatio(res);
        return res;
    }

}
