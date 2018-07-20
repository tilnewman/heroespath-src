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

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageCombatOver::PopupStageCombatOver(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , bgTexture_()
        , bgSprite_()
        , titleTextRegionUPtr_()
        , descTextRegionUPtr_()
    {}

    PopupStageCombatOver::~PopupStageCombatOver() = default;

    void PopupStageCombatOver::Setup()
    {
        PopupStageBase::Setup();

        switch (popupInfo_.HowCombatEnded())
        {
            case combat::CombatEnd::Win:
            {
                sfml_util::SoundManager::Instance()
                    ->GetSoundEffectSet(sfml_util::sound_effect_set::CombatWin)
                    .PlayRandom();

                sfml_util::Loaders::Texture(
                    bgTexture_,
                    game::GameDataFile::Instance()->GetMediaPath(
                        "media-images-combat-crossswords"));

                break;
            }

            case combat::CombatEnd::Lose:
            {
                sfml_util::SoundManager::Instance()
                    ->GetSoundEffectSet(sfml_util::sound_effect_set::CombatLose)
                    .PlayRandom();

                sfml_util::Loaders::Texture(
                    bgTexture_,
                    game::GameDataFile::Instance()->GetMediaPath("media-images-combat-crossbones"));

                break;
            }

            case combat::CombatEnd::Ran:
            case combat::CombatEnd::Count:
            default:
            {
                sfml_util::SoundManager::Instance()
                    ->GetSoundEffectSet(sfml_util::sound_effect_set::CombatLose)
                    .PlayRandom();

                sfml_util::Loaders::Texture(
                    bgTexture_,
                    game::GameDataFile::Instance()->GetMediaPath("media-images-combat-run"));

                break;
            }
        }

        sfml_util::Invert(bgTexture_);
        sfml_util::Mask(bgTexture_, sf::Color::White);
        bgSprite_.setTexture(bgTexture_, true);
        bgSprite_.setColor(sf::Color(255, 255, 255, 32));

        auto const HORIZ_RESCALE { textRegion_.width / static_cast<float>(bgTexture_.getSize().x) };

        bgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (bgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE { textRegion_.height / static_cast<float>(bgTexture_.getSize().y) };

            bgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT { (textRegion_.left + (textRegion_.width * 0.5f))
                                 - (bgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP { (textRegion_.top + (textRegion_.height * 0.5f))
                                - (bgSprite_.getGlobalBounds().height * 0.5f) };

        bgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);

        auto const TITLE_TEXT { [&]() {
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

        const sfml_util::gui::TextInfo COMBAT_TITLE_TEXTINFO(
            TITLE_TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::DefaultBoldFlavor),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::Colors::GrayDarker,
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        titleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupTitle", COMBAT_TITLE_TEXTINFO, sf::FloatRect());

        auto const TITLE_POS_LEFT { (textRegion_.left + (textRegion_.width * 0.5f))
                                    - (titleTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TITLE_POS_TOP { textRegion_.top + sfml_util::MapByRes(20.0f, 60.0f) };

        titleTextRegionUPtr_->SetEntityPos(TITLE_POS_LEFT, TITLE_POS_TOP);

        auto const DESC_TEXT { [&]() {
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

        const sfml_util::gui::TextInfo COMBAT_DESC_TEXTINFO(
            DESC_TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::Colors::GrayDarker,
            sfml_util::Justified::Center);

        const sf::FloatRect COMBAT_DESC_RECT(
            textRegion_.left,
            titleTextRegionUPtr_->GetEntityRegion().top
                + titleTextRegionUPtr_->GetEntityRegion().height
                + sfml_util::MapByRes(20.0f, 60.0f),
            textRegion_.width,
            textRegion_.height
                - (titleTextRegionUPtr_->GetEntityRegion().height
                   + (sfml_util::MapByRes(20.0f, 60.0f) * 2.0f)));

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupDesc", COMBAT_DESC_TEXTINFO, COMBAT_DESC_RECT);
    }

    void PopupStageCombatOver::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        target.draw(bgSprite_, STATES);
        target.draw(*titleTextRegionUPtr_, STATES);
        target.draw(*descTextRegionUPtr_, STATES);

        Stage::Draw(target, STATES);
    }
} // namespace popup
} // namespace heroespath
