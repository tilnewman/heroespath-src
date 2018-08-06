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
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem-helpers.hpp"
#include "misc/random.hpp"
#include "misc/types.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-stage-generic.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box-entity-info.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/image-loaders.hpp"
#include "sfml-util/image-options.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace popup
{

    std::string PopupManager::windowTextureDirectoryPath_ { "" };
    std::string PopupManager::accentTextureDirectoryPath_ { "" };

    // set to match sfml_util::defaults::GrayDarker before being set in the constructor
    sf::Color PopupManager::fontColor_ { sf::Color(64, 64, 64, 255) };

    std::unique_ptr<PopupManager> PopupManager::instanceUPtr_;

    PopupManager::PopupManager()
        : accentPaths_()
    {
        M_HP_LOG_DBG("Subsystem Construction: PopupManager");
        fontColor_ = sfml_util::defaults::GrayDarker;
    }

    PopupManager::~PopupManager() { M_HP_LOG_DBG("Subsystem Destruction: PopupManager"); }

    misc::NotNull<PopupManager *> PopupManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: PopupManager");
            Acquire();
        }

        return instanceUPtr_.get();
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
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "popup::PopupManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }

    void PopupManager::SetTexturesDirectoryPaths(
        const std::string & WINDOWS_PATH, const std::string & ACCENTS_PATH)
    {
        windowTextureDirectoryPath_ = WINDOWS_PATH;
        accentTextureDirectoryPath_ = ACCENTS_PATH;
    }

    bool PopupManager::Test()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "popup::PopupManager::Test() Starting Tests...");
        }

        // TODO

        game::LoopManager::Instance()->TestingStrAppend(
            "popup::PopupManager::Test()  ALL TESTS PASSED.");

        return true;
    }

    const std::string PopupManager::BackgroundImagePath(const PopupImage::Enum IMAGE) const
    {
        std::string filename { "" };
        switch (IMAGE)
        {
            case PopupImage::Banner:
            {
                filename = "paper-banner.png";
                break;
            }
            case PopupImage::Regular:
            {
                filename = "paper-regular.png";
                break;
            }
            case PopupImage::RegularSidebar:
            {
                filename = "paper-regular-bar.png";
                break;
            }
            case PopupImage::Large:
            {
                filename = "paper-large.png";
                break;
            }
            case PopupImage::LargeSidebar:
            {
                filename = "paper-large-bar.png";
                break;
            }
            case PopupImage::Spellbook:
            {
                filename = "spellbook.png";
                break;
            }
            case PopupImage::MusicSheet:
            {
                filename = "music-sheet.png";
                break;
            }
            case PopupImage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupManager::BackgroundImagePath(" << IMAGE
                   << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }

        namespace bfs = boost::filesystem;
        auto const PATH { bfs::system_complete(
            bfs::path(windowTextureDirectoryPath_) / bfs::path(filename)) };

        return PATH.string();
    }

    const sfml_util::gui::TextInfo PopupManager::TextInfoDefault(
        const std::string & TEXT,
        const sfml_util::Justified::Enum JUSTIFIED,
        const unsigned int FONT_SIZE) const
    {
        return sfml_util::gui::TextInfo(
            TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            FONT_SIZE,
            fontColor_,
            JUSTIFIED);
    }

    const PopupInfo PopupManager::CreatePopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const sfml_util::Justified::Enum JUSTIFIED,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const unsigned int FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, JUSTIFIED, FONT_SIZE),
            BUTTONS,
            IMAGE,
            SOUND_EFFECT);
    }

    const PopupInfo PopupManager::CreateImageSelectionPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const sfml_util::CachedTextureVec_t & TEXTURE_VEC,
        const std::size_t INITIAL_SELECTION,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const unsigned int FONT_SIZE) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
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
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
            THE_MIN,
            THE_MAX);
    }

    const PopupInfo PopupManager::CreateCharacterSelectPopupInfo(
        const std::string & POPUP_NAME,
        const std::string & PROMPT_TEXT,
        const std::vector<std::string> & INVALID_MSG_VEC,
        const std::size_t INITIAL_SELECTION) const
    {
        auto popupInfo { CreateImageSelectionPopupInfo(
            POPUP_NAME,
            PROMPT_TEXT,
            sfml_util::CachedTextureVec_t(),
            INITIAL_SELECTION,
            sfml_util::sound_effect::PromptGeneric,
            sfml_util::FontManager::Instance()->Size_Smallish()) };

        popupInfo.TextVec(INVALID_MSG_VEC);

        return popupInfo;
    }

    const PopupInfo PopupManager::CreateTitleFadePopupInfo(
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR) const
    {
        using namespace misc;

        std::ostringstream titleSS;
        titleSS << "New Title!";

        std::ostringstream descSS;
        descSS << "Congradulations!\n\nAfter a total of " << TO_TITLE_PTR->AchievementCount() << " "
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

        sfml_util::CachedTextureVec_t textureVec;
        if (FROM_TITLE_PTR_OPT)
        {
            textureVec.emplace_back(sfml_util::LoadAndCacheImage(FROM_TITLE_PTR_OPT.value()));
        }

        textureVec.emplace_back(sfml_util::LoadAndCacheImage(TO_TITLE_PTR));

        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Normal()),
            PopupButtons::Okay,
            PopupImage::Large,
            sfml_util::sound_effect::Achievement,
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
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::Spellbook,
            sfml_util::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::CachedTextureVec_t(),
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
            POPUP_NAME,
            TextInfoDefault(
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
            PopupButtons::None,
            PopupImage::MusicSheet,
            sfml_util::sound_effect::None,
            PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::CachedTextureVec_t(),
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
                " ",
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Large()),
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
            sfml_util::sound_effect::PromptWarn,
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
                sfml_util::Justified::Center,
                sfml_util::FontManager::Instance()->Size_Normal()),
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
        const sfml_util::sound_effect::Enum SOUND_EFFECT) const
    {
        return PopupInfo(
            POPUP_NAME,
            TextInfoDefault(PROMPT_TEXT, sfml_util::Justified::Center, FONT_SIZE),
            BUTTONS,
            IMAGE,
            SOUND_EFFECT,
            popup::PopupButtonColor::Dark,
            false,
            std::vector<std::size_t>(),
            sfml_util::CachedTextureVec_t(),
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
        const sfml_util::Justified::Enum JUSTIFIED,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const bool WILL_INCLUDE_ITEMS,
        const unsigned int FONT_SIZE) const
    {
        PopupInfo popupInfo(
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

    sfml_util::CachedTexture PopupManager::LoadRandomAccentImage() const
    {
        return sfml_util::CachedTexture(
            misc::Vector::SelectRandom(accentPaths_),
            sfml_util::ImageOptions(
                ((misc::random::Bool()) ? sfml_util::ImageOpt::Default
                                        : sfml_util::ImageOpt::FlipHoriz)));
    }

    void PopupManager::LoadAccentImagePaths()
    {
        namespace mfs = misc::filesystem;
        namespace bfs = boost::filesystem;

        auto const DIR_PATH { mfs::MakePathPretty(bfs::path(accentTextureDirectoryPath_)) };

        accentPaths_ = mfs::FindFilesInDirectory(DIR_PATH, "accent-", ".png");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (accentPaths_.empty() == false),
            "popup::PopupManager::LoadAccentImagePaths() failed to load any files from: "
                << DIR_PATH.string());
    }

} // namespace popup
} // namespace heroespath
