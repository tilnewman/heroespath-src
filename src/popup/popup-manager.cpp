// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-manager.cpp
//
#include "popup-manager.hpp"

#include "creature/creature.hpp"
#include "creature/title.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "gui/box-entity-info.hpp"
#include "gui/box-entity.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/image-options.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/types.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-stage-generic.hpp"
#include "stage/i-stage.hpp"

namespace heroespath
{
namespace popup
{

    std::string PopupManager::windowTextureDirectoryPath_ { "" };
    std::string PopupManager::accentTextureDirectoryPath_ { "" };

    // set to match sfutil::color::GrayDarker before being set in the constructor
    sf::Color PopupManager::fontColor_ { sf::Color(64, 64, 64, 255) };

    std::unique_ptr<PopupManager> PopupManager::instanceUPtr_;

    PopupManager::PopupManager()
        : accentPaths_()
    {
        M_HP_LOG_DBG("Subsystem Construction: PopupManager");
        fontColor_ = sfutil::color::GrayDarker;
    }

    PopupManager::~PopupManager() { M_HP_LOG_DBG("Subsystem Destruction: PopupManager"); }

    misc::NotNull<PopupManager *> PopupManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: PopupManager");
            Acquire();
        }

        return instanceUPtr_;
    }

    void PopupManager::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<PopupManager>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: PopupManager");
        }
    }

    void PopupManager::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "popup::PopupManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }

    void PopupManager::SetTexturesDirectoryPaths(
        const std::string & WINDOWS_PATH, const std::string & ACCENTS_PATH)
    {
        windowTextureDirectoryPath_ = WINDOWS_PATH;
        accentTextureDirectoryPath_ = ACCENTS_PATH;
    }

    bool PopupManager::Test(stage::IStagePtr_t iStagePtr)
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            iStagePtr->TestingStrAppend("popup::PopupManager::Test() Starting Tests...");
        }

        // TODO
        iStagePtr->TestingStrAppend("popup::PopupManager::Test()  ALL TESTS PASSED.");
        return true;
    }

    const std::string PopupManager::BackgroundImageConfigFileKey(const PopupImage::Enum IMAGE) const
    {
        std::string filename { "media-image-background-paper-popup-" };
        switch (IMAGE)
        {
            case PopupImage::Banner:
            {
                filename += "banner";
                break;
            }
            case PopupImage::Regular:
            {
                filename += "medium";
                break;
            }
            case PopupImage::RegularSidebar:
            {
                filename += "medium-bar";
                break;
            }
            case PopupImage::Large:
            {
                filename += "large";
                break;
            }
            case PopupImage::LargeSidebar:
            {
                filename += "large-bar";
                break;
            }
            case PopupImage::Spellbook:
            {
                filename += "spell-book";
                break;
            }
            case PopupImage::MusicSheet:
            {
                filename += "music-sheet";
                break;
            }
            case PopupImage::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "gui::PopupManager::BackgroundImagePath(" << popup::PopupImage::ToString(
                        IMAGE) << ") but that popup::PopupImage::Enum value was invalid.");

                return "";
            }
        }

        return filename;
    }

    const gui::TextInfo PopupManager::TextInfoDefault(
        const std::string & TEXT,
        const gui::Justified::Enum JUSTIFIED,
        const unsigned int FONT_SIZE) const
    {
        return gui::TextInfo(TEXT, gui::GuiFont::System, FONT_SIZE, fontColor_, JUSTIFIED);
    }

    const PopupInfo PopupManager::CreatePopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const gui::Justified::Enum JUSTIFIED,
        const gui::sound_effect::Enum SOUND_EFFECT,
        const unsigned int FONT_SIZE) const
    {
        return PopupInfo(
            PopupStage::Generic,
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
            BUTTONS,
            IMAGE,
            SOUND_EFFECT);
    }

    const PopupInfo PopupManager::CreateImageSelectionPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const gui::CachedTextureVec_t & TEXTURE_VEC,
        const std::size_t INITIAL_SELECTION,
        const gui::sound_effect::Enum SOUND_EFFECT,
        const unsigned int FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, gui::Justified::Center, FONT_SIZE),
            TEXTURE_VEC,
            INITIAL_SELECTION,
            SOUND_EFFECT);
    }

    const PopupInfo PopupManager::CreateNumberSelectionPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const std::size_t THE_MIN,
        const std::size_t THE_MAX,
        const unsigned int FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, gui::Justified::Center, FONT_SIZE),
            THE_MIN,
            THE_MAX);
    }

    const PopupInfo PopupManager::CreateCharacterSelectPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const std::vector<std::string> & INVALID_MSG_VEC,
        const std::size_t INITIAL_SELECTION) const
    {
        gui::CachedTextureVec_t partyImages;
        for (const auto & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
        {
            partyImages.emplace_back(gui::LoadAndCacheImage(
                CREATURE_PTR, gui::ImageOptions::InvertedCharacterOptions()));
        }

        auto popupInfo { CreateImageSelectionPopupInfo(
            POPUP_NAME,
            PROMPT_TEXT,
            partyImages,
            INITIAL_SELECTION,
            gui::sound_effect::PromptGeneric,
            gui::FontManager::Instance()->Size_Smallish()) };

        popupInfo.TextVec(INVALID_MSG_VEC);

        return popupInfo;
    }

    const PopupInfo PopupManager::CreateTitleFadePopupInfo(
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR) const
    {
        std::ostringstream titleSS;
        titleSS << "New Title!";

        std::ostringstream descSS;
        descSS << "Congratulations!\n\nAfter a total of " << TO_TITLE_PTR->AchievementCount() << " "
               << creature::AchievementType::Name(TO_TITLE_PTR->GetAchievementType()) << " "
               << CREATURE_PTR->Name();

        if (FROM_TITLE_PTR_OPT)
        {
            descSS << " has transitioned from a "
                   << creature::Titles::Name(FROM_TITLE_PTR_OPT.value()->Which()) << " to a "
                   << creature::Titles::Name(TO_TITLE_PTR->Which()) << ".";
        }
        else
        {
            descSS << " has earned the title of " << creature::Titles::Name(TO_TITLE_PTR->Which())
                   << ".";
        }

        descSS << "  This title comes with the following stats bonus: "
               << TO_TITLE_PTR->StatBonus().ToString(false) << ".";

        if (TO_TITLE_PTR->ExpBonus() > 0_exp)
        {
            descSS << "  There is also an experience bonus of " << TO_TITLE_PTR->ExpBonus() << "!  "
                   << CREATURE_PTR->Name() << "'s experience is now " << CREATURE_PTR->Exp()
                   << "!!";
        }

        if (TO_TITLE_PTR->HealthBonus() > 0_health)
        {
            descSS << "  There is also a health bonus of " << TO_TITLE_PTR->HealthBonus() << "!  "
                   << CREATURE_PTR->Name() << "'s maximum health is now "
                   << CREATURE_PTR->HealthNormal() << "!!";
        }

        if (TO_TITLE_PTR->RankBonus() > 0_rank)
        {
            descSS << "  There is even a rank bonus of " << TO_TITLE_PTR->RankBonus() << "!  "
                   << CREATURE_PTR->Name() << "'s rank is now " << CREATURE_PTR->Rank() << "!!";
        }

        gui::CachedTextureVec_t textureVec;
        if (FROM_TITLE_PTR_OPT)
        {
            textureVec.emplace_back(gui::LoadAndCacheImage(FROM_TITLE_PTR_OPT.value()));
        }

        textureVec.emplace_back(gui::LoadAndCacheImage(TO_TITLE_PTR));

        return PopupInfo(
            PopupStage::ImageFade,
            POPUP_NAME,
            TextInfoDefault(
                " ", gui::Justified::Center, gui::FontManager::Instance()->Size_Normal()),
            PopupButtons::Okay,
            PopupImage::Large,
            gui::sound_effect::Achievement,
            PopupButtonColor::Dark,
            true,
            std::vector<std::size_t>(),
            textureVec,
            std::vector<std::string>(),
            100.0f,
            CREATURE_PTR,
            0,
            titleSS.str(),
            descSS.str());
    }

    const PopupInfo PopupManager::CreateSpellbookPopupInfo(
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const std::size_t INITIAL_SELECTION) const
    {
        return PopupInfo(
            PopupStage::Spellbook,
            POPUP_NAME,
            TextInfoDefault(
                " ", gui::Justified::Center, gui::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::Spellbook,
            gui::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            gui::CachedTextureVec_t(),
            std::vector<std::string>(),
            PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            CREATURE_PTR,
            INITIAL_SELECTION);
    }

    const PopupInfo PopupManager::CreateMusicPopupInfo(
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const std::size_t INITIAL_SELECTION) const
    {
        return PopupInfo(
            PopupStage::Musicsheet,
            POPUP_NAME,
            TextInfoDefault(
                " ", gui::Justified::Center, gui::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::MusicSheet,
            gui::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            gui::CachedTextureVec_t(),
            std::vector<std::string>(),
            PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            CREATURE_PTR,
            INITIAL_SELECTION);
    }

    const PopupInfo PopupManager::CreateCombatOverPopupInfo(
        const std::string & POPUP_NAME, const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ", gui::Justified::Center, gui::FontManager::Instance()->Size_Large()),
            ((HOW_COMBAT_ENDED == combat::CombatEnd::Ran) ? PopupButtons::Continue
                                                          : PopupButtons::YesNo),
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
            ss << "Congratulations, you have discovered a BUG!";
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
            PopupStage::SystemError,
            POPUP_NAME,
            TextInfoDefault(
                ss.str(), gui::Justified::Center, gui::FontManager::Instance()->Size_Smallish()),
            PopupButtons::Continue,
            PopupImage::Regular,
            gui::sound_effect::PromptWarn,
            PopupButtonColor::Dark,
            false);
    }

    const PopupInfo
        PopupManager::CreateItemProfilePleaseWaitPopupInfo(const std::string & POPUP_NAME) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                "\nPlease Wait",
                gui::Justified::Center,
                gui::FontManager::Instance()->Size_Normal()),
            PopupButtons::None,
            PopupImage::Banner);
    }

    const PopupInfo PopupManager::CreateKeepAlivePopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const float KEEP_ALIVE_SECONDS,
        const unsigned int FONT_SIZE,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const gui::sound_effect::Enum SOUND_EFFECT) const
    {
        return PopupInfo(
            PopupStage::Generic,
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, gui::Justified::Center, FONT_SIZE),
            BUTTONS,
            IMAGE,
            SOUND_EFFECT,
            popup::PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            gui::CachedTextureVec_t(),
            std::vector<std::string>(),
            popup::PopupInfo::IMAGE_FADE_SPEED_DEFAULT_,
            boost::none,
            0,
            "",
            "",
            KEEP_ALIVE_SECONDS);
    }

    const PopupInfo PopupManager::CreateInventoryPromptPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const gui::Justified::Enum JUSTIFIED,
        const gui::sound_effect::Enum SOUND_EFFECT,
        const bool WILL_INCLUDE_ITEMS,
        const unsigned int FONT_SIZE) const
    {
        PopupInfo popupInfo(
            PopupStage::InventoryPrompt,
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
            BUTTONS,
            IMAGE,
            SOUND_EFFECT);

        popupInfo.SetDoesIncludeItems(WILL_INCLUDE_ITEMS);

        return popupInfo;
    }

    const PopupInfo PopupManager::CreateTrapPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & TRAP_DESCRIPTION,
        const gui::sound_effect::Enum SOUND_EFFECT) const
    {
        return CreatePopupInfo(
            POPUP_NAME,
            TRAP_DESCRIPTION,
            popup::PopupButtons::Continue,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            SOUND_EFFECT);
    }

    const PopupInfo
        PopupManager::CreateResolutionChangePopupInfo(const gui::TextInfo & TEXT_INFO) const
    {
        return PopupInfo(
            PopupStage::Generic,
            "ResolutionChange",
            TEXT_INFO,
            popup::PopupButtons::YesNo,
            popup::PopupImage::Regular,
            gui::sound_effect::PromptQuestion);
    }

    const std::string PopupManager::RandomAccentImagePath() const
    {
        if (accentPaths_.empty())
        {
            return "";
        }
        else
        {
            return misc::Vector::SelectRandom(accentPaths_);
        }
    }

    void PopupManager::LoadAccentImagePaths()
    {
        accentPaths_ = misc::filesystem::FindFiles(false, accentTextureDirectoryPath_, "", ".png");

        if (accentPaths_.empty())
        {
            M_HP_LOG_ERR("Failed to find any images." + M_HP_VAR_STR(accentTextureDirectoryPath_));
        }
    }

} // namespace popup
} // namespace heroespath
