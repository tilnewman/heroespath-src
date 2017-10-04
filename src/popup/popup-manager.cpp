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

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/box-info.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"
#include "game/creature/title.hpp"
#include "game/creature/creature.hpp"

#include "popup/popup-stage-generic.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>
#include <memory>



namespace popup
{

    std::string PopupManager::windowTextureDirectoryPath_    { "" };
    std::string PopupManager::accentTextureDirectoryPath_    { "" };

    //set to match sfml_util::FontManager::Color_GrayDarker() before being set in the constructor
    sf::Color   PopupManager::fontColor_                     { sf::Color(64, 64, 64, 255) };

    std::unique_ptr<PopupManager> PopupManager::instanceUPtr_{ nullptr };


    PopupManager::PopupManager()
    :
        BACKGROUND_IMAGE_SCALE_DEFAULT_(sfml_util::MapByRes(0.05f, 6.666f)),//found by experiment
        accentPathsVec_()
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
            instanceUPtr_ = std::make_unique<PopupManager>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: PopupManager");
        }
    }


    void PopupManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "popup::PopupManager::Release() found instanceUPtr that was null.");
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
            game::LoopManager::Instance()->TestingStrAppend(
                "popup::PopupManager::Test() Starting Tests...");
        }

        //TODO

        game::LoopManager::Instance()->TestingStrAppend(
            "sfml_util::gui::SpellImageManager::Test()  ALL TESTS PASSED.");

        return true;
    }


    const std::string PopupManager::BackgroundImagePath(
        const PopupImage::Enum IMAGE) const
    {
        std::string filename{ "" };
        switch (IMAGE)
        {
            case PopupImage::Banner:         { filename = "paper-banner.png"; break; }
            case PopupImage::Regular:        { filename = "paper-regular.png"; break; }
            case PopupImage::RegularSidebar: { filename = "paper-regular-bar.png"; break; }
            case PopupImage::Large:          { filename = "paper-large.png"; break; }
            case PopupImage::LargeSidebar:   { filename = "paper-large-bar.png"; break; }
            case PopupImage::Spellbook:      { filename = "spellbook.png"; break; }
            case PopupImage::MusicSheet:     { filename = "music-sheet.png"; break; }
            case PopupImage::Custom:
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupManager::BackgroundImagePath("
                    << IMAGE << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }

        namespace bfs = boost::filesystem;
        auto const PATH{ bfs::system_complete(bfs::path(windowTextureDirectoryPath_) /
            bfs::path(filename)) };

        return PATH.string();
    }


    const sfml_util::gui::TextInfo PopupManager::TextInfoDefault(
        const std::string &              TEXT,
        const sfml_util::Justified::Enum JUSTIFIED,
        const unsigned int               FONT_SIZE) const
    {
        return sfml_util::gui::TextInfo(
            TEXT,
            sfml_util::FontManager::Instance()->Font_Default2(),
            FONT_SIZE,
            fontColor_,
            JUSTIFIED);
    }


    const PopupInfo PopupManager::CreatePopupInfo(
        const std::string &                 POPUP_NAME,
        const std::string &                 PROMPT_TEXT,
        const PopupButtons::Enum            BUTTONS,
        const PopupImage::Enum              IMAGE,
        const sfml_util::Justified::Enum    JUSTIFIED,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const unsigned int                  FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
            BUTTONS,
            IMAGE,
            GetScaleForImage(IMAGE),
            SOUND_EFFECT);
    }


    const PopupInfo PopupManager::CreateBoxedPopupInfo(
        const std::string &                 POPUP_NAME,
        const std::string &                 PROMPT_TEXT,
        const sf::Color &                   TEXT_COLOR,
        const sfml_util::gui::box::Info &   BOX_INFO,
        const PopupButtons::Enum            BUTTONS,
        const sfml_util::Justified::Enum    JUSTIFIED,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const unsigned int                  FONT_SIZE) const
    {
        sfml_util::gui::TextInfo ti(TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE));
        ti.color = TEXT_COLOR;

        return PopupInfo(
            POPUP_NAME,
            ti,
            BUTTONS,
            BOX_INFO,
            0.25f,
            0.5f,
            SOUND_EFFECT);
    }


    const PopupInfo PopupManager::CreateImageSelectionPopupInfo(
        const std::string &                 POPUP_NAME,
        const std::string &                 PROMPT_TEXT,
        const sfml_util::TextureVec_t &     TEXTURE_VEC,
        const bool                          ARE_IMAGES_CREATURES,
        const std::size_t                   INITIAL_SELECTION,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const unsigned int                  FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
            TEXTURE_VEC,
            ARE_IMAGES_CREATURES,
            INITIAL_SELECTION,
            GetScaleForImage(popup::PopupImage::Large),
            SOUND_EFFECT);
    }


    const PopupInfo PopupManager::CreateNumberSelectionPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const std::size_t   THE_MIN,
        const std::size_t   THE_MAX,
        const unsigned int  FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
            THE_MIN,
            THE_MAX,
            GetScaleForImage(popup::PopupImage::Large));
    }


    const PopupInfo PopupManager::CreateCharacterSelectPopupInfo(
        const std::string &              POPUP_NAME,
        const std::string &              PROMPT_TEXT,
        const std::vector<std::string> & INVALID_MSG_VEC,
        const std::size_t                INITIAL_SELECTION) const
    {
        auto popupInfo{ CreateImageSelectionPopupInfo(
            POPUP_NAME,
            PROMPT_TEXT,
            sfml_util::TextureVec_t(),
            true,
            INITIAL_SELECTION,
            sfml_util::sound_effect::PromptGeneric,
            sfml_util::FontManager::Instance()->Size_Smallish()) };

        popupInfo.TextVec(INVALID_MSG_VEC);

        return popupInfo;
    }


    const PopupInfo PopupManager::CreateImageFadePopupInfo(
        const std::string &                 POPUP_NAME,
        const game::creature::CreaturePtr_t CREATURE_PTR,
        const game::creature::TitlePtr_t    FROM_TITLE_PTR,
        const game::creature::TitlePtr_t    TO_TITLE_PTR,
        const sf::Texture * const           FROM_IMAGE_PTR,
        const sf::Texture * const           TO_IMAGE_PTR) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((TO_IMAGE_PTR != nullptr),
            "popup::PopupManager::CreateImageFadePopupInfo(\""
            << POPUP_NAME << "\")  TO_IMAGE_PTR was null.");

        std::ostringstream titleSS;
        titleSS << "New Title!";

        std::ostringstream descSS;
        descSS << "Congradulations!\n\nAfter a total of " << TO_TITLE_PTR->AchievementCount()
            << " " << game::creature::AchievementType::Name(TO_TITLE_PTR->GetAchievementType())
            << " " << CREATURE_PTR->Name();

        if (FROM_TITLE_PTR == nullptr)
        {
            descSS << " has earned the title of "
                << game::creature::Titles::Name(TO_TITLE_PTR->Which()) << ".";
        }
        else
        {
            descSS << " has transitioned from a "
                << game::creature::Titles::Name(FROM_TITLE_PTR->Which())
                << " to a "
                << game::creature::Titles::Name(TO_TITLE_PTR->Which()) << ".";
        }

        descSS << "  This title comes with the following stats bonus: "
            << TO_TITLE_PTR->StatBonus().ToString(false) << ".";

        if (TO_TITLE_PTR->ExpBonus() > 0)
        {
            descSS << "  There is also an experience bonus of " << TO_TITLE_PTR->ExpBonus()
                << "!  " << CREATURE_PTR->Name() << "'s experience is now " << CREATURE_PTR->Exp()
                << "!!";
        }

        if (TO_TITLE_PTR->HealthBonus() > 0)
        {
            descSS << "  There is also a health bonus of " << TO_TITLE_PTR->HealthBonus() << "!  "
                << CREATURE_PTR->Name() << "'s maximum health is now "
                << CREATURE_PTR->HealthNormal() << "!!";
        }

        if (TO_TITLE_PTR->RankBonus() > 0)
        {
            descSS << "  There is even a rank bonus of " << TO_TITLE_PTR->RankBonus() << "!  "
                << CREATURE_PTR->Name() << "'s rank is now " << CREATURE_PTR->Rank() << "!!";
        }

        sfml_util::TextureVec_t textureVec;
        if ((FROM_TITLE_PTR != nullptr) && (FROM_IMAGE_PTR != nullptr))
        {
            textureVec.push_back( * FROM_IMAGE_PTR);
        }

        textureVec.push_back( * TO_IMAGE_PTR);

        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Normal()),
            PopupButtons::Okay,
            PopupImage::Large,
            sfml_util::MapByRes(1.5f, 4.5f),
            sfml_util::sound_effect::Achievement,
            PopupButtonColor::Dark,
            true,
            std::vector<std::size_t>(),
            textureVec,
            std::vector<std::string>(),
            100.0f,
            CREATURE_PTR,
            0,
            false,
            titleSS.str(),
            descSS.str(),
            FROM_TITLE_PTR,
            TO_TITLE_PTR);
    }


    const PopupInfo PopupManager::CreateSpellbookPopupInfo(
        const std::string &                 POPUP_NAME,
        const game::creature::CreaturePtr_t CREATURE_CPTR,
        const std::size_t                   INITIAL_SELECTION) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::Spellbook,
            1.0f,
            sfml_util::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::TextureVec_t(),
            std::vector<std::string>(),
            PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            CREATURE_CPTR,
            INITIAL_SELECTION);
    }


    const PopupInfo PopupManager::CreateMusicPopupInfo(
        const std::string &                 POPUP_NAME,
        const game::creature::CreaturePtr_t CREATURE_CPTR,
        const std::size_t                   INITIAL_SELECTION) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::MusicSheet,
            1.0f,
            sfml_util::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::TextureVec_t(),
            std::vector<std::string>(),
            PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            CREATURE_CPTR,
            INITIAL_SELECTION);
    }


    const PopupInfo PopupManager::CreateCombatOverPopupInfo(
        const std::string &                 POPUP_NAME,
        const game::combat::CombatEnd::Enum HOW_COMBAT_ENDED) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
            sfml_util::MapByRes(1.0f, 5.0f),
            ((HOW_COMBAT_ENDED == game::combat::CombatEnd::Ran) ?
                PopupButtons::Continue : PopupButtons::YesNo),
            HOW_COMBAT_ENDED);
    }


    const PopupInfo PopupManager::CreateSystemErrorPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & GENERAL_ERROR_MSG,
        const std::string & TECH_ERROR_MSG,
        const std::string & TITLE_MSG) const
    {
        std::ostringstream ss;

        if (TITLE_MSG.empty())
        {
            ss << "Congradulations, you have discovered a BUG!";
        }
        else
        {
            ss << TITLE_MSG;
        }

        ss << "\n\n";

        if (GENERAL_ERROR_MSG.empty() == false)
        {
            ss << GENERAL_ERROR_MSG << "\n\n";
        }

        if (TECH_ERROR_MSG.empty() == false)
        {
            ss << TECH_ERROR_MSG;
        }

        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                ss.str(),
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Smallish()),
            PopupButtons::Continue,
            PopupImage::Regular,
            GetScaleForImage(PopupImage::Regular),
            sfml_util::sound_effect::PromptWarn,
            PopupButtonColor::Dark,
            false);
    }


    const PopupInfo PopupManager::CreateItemProfilePleaseWaitPopupInfo(
        const std::string & POPUP_NAME) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                "\nPlease Wait",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Normal()),
            PopupButtons::None);
    }


    const PopupInfo PopupManager::CreateKeepAlivePopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const float KEEP_ALIVE_SECONDS,
        const unsigned int FONT_SIZE,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const sfml_util::sound_effect::Enum SOUND_EFFECT) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
            BUTTONS,
            IMAGE,
            1.0f,
            SOUND_EFFECT,
            popup::PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::TextureVec_t(),
            std::vector<std::string>(),
            popup::PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            nullptr,
            0,
            false,
            "",
            "",
            nullptr,
            nullptr,
            KEEP_ALIVE_SECONDS);
    }


    const PopupInfo PopupManager::CreateInventoryPromptPopupInfo(
        const std::string &                 POPUP_NAME,
        const std::string &                 PROMPT_TEXT,
        const PopupButtons::Enum            BUTTONS,
        const PopupImage::Enum              IMAGE,
        const sfml_util::Justified::Enum    JUSTIFIED,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const bool                          WILL_INCLUDE_ITEMS,
        const unsigned int                  FONT_SIZE) const
    {
        PopupInfo popupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
            BUTTONS,
            IMAGE,
            GetScaleForImage(IMAGE),
            SOUND_EFFECT);

        popupInfo.SetDoesIncludeItems(WILL_INCLUDE_ITEMS);

        return popupInfo;
    }


    const PopupInfo PopupManager::CreateTrapPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & TRAP_DESCRIPTION,
        const sfml_util::sound_effect::Enum SOUND_EFFECT) const
    {
        return CreatePopupInfo(
            POPUP_NAME,
            TRAP_DESCRIPTION,
            popup::PopupButtons::Continue,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            SOUND_EFFECT);
    }



    void PopupManager::LoadRandomAccentImage(sf::Texture & texture) const
    {
        sfml_util::LoadTexture(texture, accentPathsVec_.at(static_cast<std::size_t>(
            misc::random::Int(static_cast<int>(accentPathsVec_.size()) - 1))).string());

        if (misc::random::Bool())
        {
            sfml_util::FlipHoriz(texture);
        }
    }


    void PopupManager::LoadAccentImagePaths()
    {
        namespace bfs = boost::filesystem;

        const bfs::path   DIR_OBJ(bfs::system_complete(accentTextureDirectoryPath_));

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::exists(DIR_OBJ)),
            "popup::PopupManager::LoadAssets() accents dir path not found \""
            << DIR_OBJ.string() << "\".");

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::is_directory(DIR_OBJ)),
            "sfml_util::PopupManager::LoadAssets() accents dir path found but it is not a dir \""
            << DIR_OBJ.string() << "\".");

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

}

