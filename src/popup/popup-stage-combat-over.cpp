// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-combat-over.cpp
//
#include "popup-stage-combat-over.hpp"

#include "gui/font-manager.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <memory>

namespace heroespath
{
namespace popup
{

    PopupStageCombatOver::PopupStageCombatOver(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , bgCachedTexture_(
              PickImagePathKey(popupInfo_.HowCombatEnded()),
              gui::ImageOptions(gui::ImageOpt::Default | gui::ImageOpt::Invert, sf::Color::White))
        , bgSprite_(bgCachedTexture_.Get())
        , titleTextRegionUPtr_()
        , descTextRegionUPtr_()
    {}

    PopupStageCombatOver::~PopupStageCombatOver() = default;

    void PopupStageCombatOver::Setup()
    {
        PopupStageBase::Setup();

        PlaySfxForHowCombatEnded(popupInfo_.HowCombatEnded());

        bgSprite_.setColor(sf::Color(255, 255, 255, 32));

        sfutil::FitAndCenterTo(bgSprite_, textRegion_);

        const auto TITLE_TEXT { [&]() {
            switch (popupInfo_.HowCombatEnded())
            {
                case combat::CombatEnd::Win:
                {
                    return "Victory!";
                }
                case combat::CombatEnd::Lose:
                {
                    return "Death Strikes!";
                }
                case combat::CombatEnd::Ran:
                case combat::CombatEnd::Count:
                default:
                {
                    return "Defeat!";
                }
            }
        }() };

        const gui::TextInfo COMBAT_TITLE_TEXTINFO(
            TITLE_TEXT,
            gui::GuiFont::DefaultBoldFlavor,
            gui::FontManager::Instance()->Size_Large(),
            sfutil::color::GrayDarker,
            gui::Justified::Center,
            sf::Text::Bold);

        titleTextRegionUPtr_
            = std::make_unique<gui::TextRegion>("CombatOverPopupTitle", COMBAT_TITLE_TEXTINFO);

        const auto TITLE_POS_LEFT { (textRegion_.left + (textRegion_.width * 0.5f))
                                    - (titleTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        const auto VERT_SPACER { sfutil::ScreenRatioToPixelsVert(0.0222f) };

        const auto TITLE_POS_TOP { textRegion_.top + VERT_SPACER };

        titleTextRegionUPtr_->SetEntityPos(TITLE_POS_LEFT, TITLE_POS_TOP);

        const auto DESC_TEXT { [&]() {
            std::ostringstream descSS;
            switch (popupInfo_.HowCombatEnded())
            {
                case combat::CombatEnd::Win:
                {
                    descSS << "Congratulations, your party has beaten all enemies on the "
                           << "field of battle and emerged victorious.\n\nWill you search "
                           << "for loot?";
                    break;
                }
                case combat::CombatEnd::Lose:
                {
                    descSS << "The dangers of Etan have claimed another party of "
                           << "adventurers.  All of your characters have been "
                           << "incapacitated or killed, but all is not lost.  Your "
                           << "saved games remain and can be loaded at any time.\n\n"
                           << "Click YES to restart from your last save, or click NO "
                           << "to quit.";
                    break;
                }
                case combat::CombatEnd::Ran:
                case combat::CombatEnd::Count:
                default:
                {
                    descSS << "You have run from battle, and as a consequence, "
                           << "you will earn no experience and loot no treasure.";
                    break;
                }
            }
            return descSS.str();
        }() };

        const gui::TextInfo COMBAT_DESC_TEXTINFO(
            DESC_TEXT,
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Normal(),
            sfutil::color::GrayDarker,
            gui::Justified::Center);

        const sf::FloatRect COMBAT_DESC_RECT(
            textRegion_.left,
            titleTextRegionUPtr_->GetEntityRegion().top
                + titleTextRegionUPtr_->GetEntityRegion().height + VERT_SPACER,
            textRegion_.width,
            textRegion_.height
                - (titleTextRegionUPtr_->GetEntityRegion().height + (VERT_SPACER * 2.0f)));

        descTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "CombatOverPopupDesc", COMBAT_DESC_TEXTINFO, COMBAT_DESC_RECT);
    }

    void PopupStageCombatOver::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);

        target.draw(bgSprite_, states);
        target.draw(*titleTextRegionUPtr_, states);
        target.draw(*descTextRegionUPtr_, states);

        StageBase::draw(target, states);
    }

    const std::string
        PopupStageCombatOver::PickImagePathKey(const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const
    {
        switch (HOW_COMBAT_ENDED)
        {
            case combat::CombatEnd::Win:
            {
                return "media-images-combat-crossswords";
            }

            case combat::CombatEnd::Lose:
            {
                return "media-images-combat-crossbones";
            }

            case combat::CombatEnd::Ran:
            case combat::CombatEnd::Count:
            default:
            {
                return "media-images-combat-run";
            }
        }
    }

    void PopupStageCombatOver::PlaySfxForHowCombatEnded(
        const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const
    {
        switch (HOW_COMBAT_ENDED)
        {
            case combat::CombatEnd::Win:
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::CombatWin)
                    .PlayRandom();
                break;
            }

            case combat::CombatEnd::Lose:
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::CombatLose)
                    .PlayRandom();
                break;
            }

            case combat::CombatEnd::Ran:
            case combat::CombatEnd::Count:
            default:
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::CombatLose)
                    .PlayRandom();
                break;
            }
        }
    }

} // namespace popup
} // namespace heroespath
