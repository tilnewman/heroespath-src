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
// popup-manager.cpp
//
#include "popup-manager.hpp"

#include "misc/random.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/popup-stage.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/box-info.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"

#include <sstream>
#include <exception>
#include <memory>


namespace sfml_util
{
namespace gui
{

    std::string PopupManager::windowTextureDirectoryPath_    { "" };
    std::string PopupManager::accentTextureDirectoryPath_    { "" };
    sf::Color   PopupManager::fontColor_                     { sf::Color(64, 64, 64, 255) };//set to match FontManager::Color_GrayDarker() before being set in the constructor
    std::unique_ptr<PopupManager> PopupManager::instanceUPtr_{ nullptr };


    PopupManager::PopupManager()
    :
        BACKGROUND_IMAGE_SCALE_DEFAULT_(sfml_util::MapByRes(0.05f, 6.666f)),
        popupBannerTexture_            (),
        popupRegularTexture_           (),
        popupRegularSidebarTexture_    (),
        popupLargeTexture_             (),
        popupLargeSidebarTextue_       (),
        popupSpellbookTexture_         (),
        accentPathsVec_                ()
    {
        M_HP_LOG_DBG("Singleton Construction: PopupManager");
        fontColor_ = sfml_util::FontManager::Color_GrayDarker();
    }


    PopupManager::~PopupManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: PopupManager");
    }


    PopupManager * PopupManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): PopupManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void PopupManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new PopupManager);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: PopupManager");
        }
    }


    void PopupManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::PopupManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void PopupManager::SetTexturesDirectoryPaths(const std::string & WINDOWS_PATH,
                                                 const std::string & ACCENTS_PATH)
    {
        windowTextureDirectoryPath_ = WINDOWS_PATH;
        accentTextureDirectoryPath_ = ACCENTS_PATH;
    }


    bool PopupManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::PopupManager::Test() Starting Tests...");
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::SpellImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }

    void PopupManager::LoadAssets()
    {
        LoadPopup("paper-banner.png", popupBannerTexture_);
        LoadPopup("paper-regular.png", popupRegularTexture_);
        LoadPopup("paper-regular-bar.png", popupRegularSidebarTexture_);
        LoadPopup("paper-large.png", popupLargeTexture_);
        LoadPopup("paper-large-bar.png", popupLargeSidebarTextue_);
        LoadPopup("spellbook.png", popupSpellbookTexture_);
        LoadPopup("music-sheet.png", popupMusicSheetTexture_);

        LoadAccentImagePaths();
    }


    void PopupManager::LoadPopup(const std::string & WINDOW_FILE_NAME,
                                 sf::Texture &       texture) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(windowTextureDirectoryPath_) / bfs::path(WINDOW_FILE_NAME)));
        sfml_util::LoadImageOrTexture(texture, PATH_OBJ.string());
    }


    const sf::IntRect PopupManager::Rect(const PopupImage::Enum PI, const float SCALE) const
    {
        switch (PI)
        {
            case PopupImage::Banner:         { return sfml_util::ScaleRectCopy(Rect_Banner(), SCALE); }
            case PopupImage::Regular:        { return sfml_util::ScaleRectCopy(Rect_Regular(), SCALE); }
            case PopupImage::RegularSidebar: { return sfml_util::ScaleRectCopy(Rect_RegularSidebar(), SCALE); }
            case PopupImage::Large:          { return sfml_util::ScaleRectCopy(Rect_Large(), SCALE); }
            case PopupImage::LargeSidebar:   { return sfml_util::ScaleRectCopy(Rect_LargeSidebar(), SCALE); }
            case PopupImage::Spellbook:
            case PopupImage::MusicSheet:
            case PopupImage::Custom:
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "PopupManager::Rect(" << PI << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const sfml_util::gui::TextInfo PopupManager::TextInfoDefault(const std::string &              TEXT,
                                                                 const sfml_util::Justified::Enum JUSTIFIED,
                                                                 const unsigned int               FONT_SIZE) const
    {
        return sfml_util::gui::TextInfo(TEXT,
                                        sfml_util::FontManager::Instance()->Font_Default2(),
                                        FONT_SIZE,
                                        fontColor_,
                                        JUSTIFIED);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(const std::string &           POPUP_NAME,
                                                        const std::string &           PROMPT_TEXT,
                                                        const PopupButtons::Enum      BUTTONS,
                                                        const PopupImage::Enum        IMAGE,
                                                        const Justified::Enum         JUSTIFIED,
                                                        const sound_effect::Enum      SOUND_EFFECT,
                                                        const game::Popup::Enum WHICH_POPUP,
                                                        const unsigned int            FONT_SIZE)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
                               BUTTONS,
                               IMAGE,
                               GetScaleForImage(IMAGE),
                               WHICH_POPUP,
                               SOUND_EFFECT);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(const std::string &      POPUP_NAME,
                                                        const std::string &      PROMPT_TEXT,
                                                        const sf::Color &        TEXT_COLOR,
                                                        const gui::box::Info &   BOX_INFO,
                                                        const PopupButtons::Enum BUTTONS,
                                                        const Justified::Enum    JUSTIFIED,
                                                        const sound_effect::Enum SOUND_EFFECT,
                                                        const unsigned int       FONT_SIZE)
    {
        sfml_util::gui::TextInfo ti(TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE));
        ti.color = TEXT_COLOR;
        return game::PopupInfo(POPUP_NAME,
                               ti,
                               BUTTONS,
                               BOX_INFO,
                               0.25f,
                               0.5f,
                               game::Popup::Generic,
                               SOUND_EFFECT);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(
        const std::string &             POPUP_NAME,
        const std::string &             PROMPT_TEXT,
        const sfml_util::TextureVec_t & TEXTURE_VEC,
        const bool                      ARE_IMAGES_CREATURES,
        const sound_effect::Enum        SOUND_EFFECT,
        const unsigned int              FONT_SIZE)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(PROMPT_TEXT, Justified::Center, FONT_SIZE),
                               TEXTURE_VEC,
                               ARE_IMAGES_CREATURES,
                               BACKGROUND_IMAGE_SCALE_DEFAULT_,
                               SOUND_EFFECT);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(const std::string & POPUP_NAME,
                                                        const std::string & PROMPT_TEXT,
                                                        const std::size_t   THE_MIN,
                                                        const std::size_t   THE_MAX,
                                                        const unsigned int  FONT_SIZE)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(PROMPT_TEXT, Justified::Center, FONT_SIZE),
                               THE_MIN,
                               THE_MAX,
                               BACKGROUND_IMAGE_SCALE_DEFAULT_);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(
        const std::string &              POPUP_NAME,
        const std::string &              PROMPT_TEXT,
        const std::vector<std::string> & INVALID_TEXT_VEC,
        const unsigned int               FONT_SIZE)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(PROMPT_TEXT,
                                               sfml_util::Justified::Center,
                                               FONT_SIZE),
                               sfml_util::PopupButtons::SelectCancel,
                               sfml_util::PopupImage::Large,
                               GetScaleForImage(PopupImage::Large),
                               game::Popup::CharacterSelection,
                               sfml_util::sound_effect::PromptQuestion,
                               sfml_util::PopupButtonColor::Dark,
                               true,
                               std::vector<std::size_t>(),
                               TextureVec_t(),
                               INVALID_TEXT_VEC,
                               0.0f,
                               nullptr,
                               0,
                               true);
    }


    const game::PopupInfo PopupManager::CreatePopupInfo(const std::string &      POPUP_NAME,
                                                        const std::string &      PROMPT_TEXT,
                                                        const sf::Texture &      FROM_IMAGE,
                                                        const sf::Texture &      TO_IMAGE,
                                                        const PopupButtons::Enum BUTTONS,
                                                        const unsigned int       FONT_SIZE,
                                                        const sound_effect::Enum SOUND_EFFECT)
    {
        sfml_util::TextureVec_t textureVec;
        textureVec.push_back(FROM_IMAGE);
        textureVec.push_back(TO_IMAGE);

        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(PROMPT_TEXT,
                                               sfml_util::Justified::Center,
                                               FONT_SIZE),
                               BUTTONS,
                               PopupImage::Regular,
                               GetScaleForImage(PopupImage::Regular),
                               game::Popup::ImageFade,
                               SOUND_EFFECT,
                               sfml_util::PopupButtonColor::Dark,
                               true,
                               std::vector<std::size_t>(),
                               textureVec,
                               std::vector<std::string>(),
                               100.0f);
    }


    const game::PopupInfo PopupManager::CreateSpellbookPopupInfo(
        const std::string &                 POPUP_NAME,
        const game::creature::CreaturePtr_t CREATURE_CPTR,
        const std::size_t                   INITIAL_SELECTION)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(" ",
                                               sfml_util::Justified::Center,
                                               sfml_util::FontManager::Instance()->Size_Large()),
                               sfml_util::PopupButtons::None,
                               PopupImage::Spellbook,
                               1.0f,
                               game::Popup::Spellbook,
                               sfml_util::sound_effect::None,
                               sfml_util::PopupButtonColor::Dark,
                               false,
                               std::vector<std::size_t>(),
                               sfml_util::TextureVec_t(),
                               std::vector<std::string>(),
                               game::PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
                               CREATURE_CPTR,
                               INITIAL_SELECTION);
    }


    const game::PopupInfo PopupManager::CreateMusicPopupInfo(
        const std::string &                 POPUP_NAME,
        const game::creature::CreaturePtr_t CREATURE_CPTR,
        const std::size_t                   INITIAL_SELECTION)
    {
        return game::PopupInfo(POPUP_NAME,
                               TextInfoDefault(" ",
                                               sfml_util::Justified::Center,
                                               sfml_util::FontManager::Instance()->Size_Large()),
                               sfml_util::PopupButtons::None,
                               PopupImage::MusicSheet,
                               1.0f,
                               game::Popup::Spellbook,
                               sfml_util::sound_effect::None,
                               sfml_util::PopupButtonColor::Dark,
                               false,
                               std::vector<std::size_t>(),
                               sfml_util::TextureVec_t(),
                               std::vector<std::string>(),
                               game::PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
                               CREATURE_CPTR,
                               INITIAL_SELECTION);
    }


    void PopupManager::Texture(const PopupImage::Enum PI, sf::Texture & texture) const
    {
        switch (PI)
        {
            case PopupImage::Banner:         { texture = popupBannerTexture_; break; }
            case PopupImage::Regular:        { texture = popupRegularTexture_; break; }
            case PopupImage::RegularSidebar: { texture = popupRegularSidebarTexture_; break; }
            case PopupImage::Large:          { texture = popupLargeTexture_; break; }
            case PopupImage::LargeSidebar:   { texture = popupLargeSidebarTextue_; break; }
            case PopupImage::Spellbook:      { texture = popupSpellbookTexture_; break; }
            case PopupImage::MusicSheet:     { texture = popupMusicSheetTexture_; break; }
            case PopupImage::Custom:
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "PopupManager::Texture(" << PI << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    PopupStagePtr_t PopupManager::CreatePopupStage(const game::PopupInfo & POPUP_INFO)
    {
        if (POPUP_INFO.Image() == sfml_util::PopupImage::Custom)
        {
            //establish popup window region
            const float SCREEN_WIDTH(sfml_util::Display::Instance()->GetWinWidth());
            const float SCREEN_HEIGHT(sfml_util::Display::Instance()->GetWinHeight());
            const float POPUP_WIDTH(SCREEN_WIDTH * POPUP_INFO.SizeX());
            const float POPUP_HEIGHT(SCREEN_HEIGHT * POPUP_INFO.SizeY());

            const sf::FloatRect REGION((SCREEN_WIDTH * 0.5f) - (POPUP_WIDTH * 0.5f),
                                       (SCREEN_HEIGHT * 0.5f) - (POPUP_HEIGHT * 0.5f),
                                       POPUP_WIDTH,
                                       POPUP_HEIGHT);

            //re-construct the info objects based on the REGION
            sfml_util::gui::box::Info newBoxInfo(POPUP_INFO.BoxInfo());
            newBoxInfo.SetBoxAndBackgroundRegion(REGION);
            const game::PopupInfo NEW_POPUP_INFO(POPUP_INFO.Name(),
                                                       POPUP_INFO.TextInfo(),
                                                       POPUP_INFO.Buttons(),
                                                       newBoxInfo,
                                                       POPUP_INFO.SizeX(),
                                                       POPUP_INFO.SizeY(),
                                                       POPUP_INFO.Type(),
                                                       POPUP_INFO.SoundEffect(),
                                                       POPUP_INFO.ButtonColor(),
                                                       POPUP_INFO.WillAddRandImage());

            //establish inner rect
            const float PAD(20.0f);
            sf::FloatRect innerRegion(REGION);
            innerRegion.left   = PAD;
            innerRegion.width -= PAD;
            innerRegion.top    = PAD;
            innerRegion.top   -= PAD;

            auto popupStagePtr( new PopupStage(NEW_POPUP_INFO,
                                               REGION,
                                               innerRegion) );
            popupStagePtr->Setup();
            return popupStagePtr;
        }
        else if (POPUP_INFO.Image() == sfml_util::PopupImage::Spellbook)
        {
            sf::Texture backgroundTexture;
            Texture(POPUP_INFO.Image(), backgroundTexture);

            //define the outer limits of the stage
            auto const SPELLBOOK_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() * PopupStage::SPELLBOOK_POPUP_BACKGROUND_WIDTH_RATIO_ };
            auto const SPELLBOOK_HEIGHT{ (static_cast<float>(backgroundTexture.getSize().y) * SPELLBOOK_WIDTH) / static_cast<float>(backgroundTexture.getSize().x) };

            sf::FloatRect rect;
            rect.left = ((sfml_util::Display::Instance()->GetWinWidth() - SPELLBOOK_WIDTH) * 0.5f);
            rect.top = ((sfml_util::Display::Instance()->GetWinHeight() - SPELLBOOK_HEIGHT) * 0.5f);
            rect.width = SPELLBOOK_WIDTH;
            rect.height = SPELLBOOK_HEIGHT;

            auto const INNER_RECT{ rect };

            auto popupStagePtr( new PopupStage(POPUP_INFO,
                                               rect,
                                               INNER_RECT,
                                               backgroundTexture) );
            popupStagePtr->Setup();
            return popupStagePtr;
        }
        else
        {
            sf::Texture backgroundTexture;
            Texture(POPUP_INFO.Image(), backgroundTexture);

            //define the outer limits of the stage
            const float TEXTURE_WIDTH (static_cast<float>(backgroundTexture.getSize().x));
            const float TEXTURE_HEIGHT(static_cast<float>(backgroundTexture.getSize().y));

            sf::FloatRect rect;
            rect.left = (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (TEXTURE_WIDTH * 0.5f);
            rect.top = (sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (TEXTURE_HEIGHT * 0.5f);
            rect.width = TEXTURE_WIDTH;
            rect.height = TEXTURE_HEIGHT;

            const sf::FloatRect INNER_RECT(sfml_util::ConvertRect<int, float>(Rect(POPUP_INFO.Image(), POPUP_INFO.ImageScale())));

            auto popupStagePtr( new PopupStage(POPUP_INFO,
                                               rect,
                                               INNER_RECT,
                                               backgroundTexture) );
            popupStagePtr->Setup();
            return popupStagePtr;
        }
    }


    void PopupManager::LoadRandomAccentImage(sf::Texture & texture) const
    {
        sfml_util::LoadImageOrTexture(texture, accentPathsVec_.at(static_cast<std::size_t>(misc::random::Int(static_cast<int>(accentPathsVec_.size()) - 1))).string());

        if (misc::random::Bool())
        {
            sfml_util::FlipHoriz(texture);
        }

        texture.setSmooth(true);
    }


    float PopupManager::GetScaleForImage(const PopupImage::Enum E) const
    {
        if (E == PopupImage::Banner)
        {
            return sfml_util::MapByRes(1.0f, 3.0f);
        }
        else
        {
            return BACKGROUND_IMAGE_SCALE_DEFAULT_;
        }
    }


    void PopupManager::LoadAccentImagePaths()
    {
        namespace bfs = boost::filesystem;

        const bfs::path   DIR_OBJ(bfs::system_complete(accentTextureDirectoryPath_));

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::exists(DIR_OBJ)), "sfml_util::gui::PopupManager::LoadAssets() accents dir path not found \"" << DIR_OBJ.string() << "\".");
        M_ASSERT_OR_LOGANDTHROW_SS((bfs::is_directory(DIR_OBJ)), "sfml_util::PopupManager::LoadAssets() accents dir path found but it is not a dir \"" << DIR_OBJ.string() << "\".");

        //create a vector of paths to saved games
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(DIR_OBJ); itr != end_itr; ++itr)
        {
            if ((bfs::is_regular_file(itr->path())) &&
                (boost::algorithm::starts_with(itr->path().leaf().string(), "accent-")) &&
                (boost::algorithm::ends_with(itr->path().leaf().string(), ".png")))
            {
                accentPathsVec_.push_back(itr->path());
            }
        }
    }

}
}
