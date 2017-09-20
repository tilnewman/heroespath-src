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
// popup-stage.cpp
//
#include "popup-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-rendering.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"

#include "game/game.hpp"
#include "game/stats/trait-enum.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/loop-manager.hpp"
#include "game/log-macros.hpp"
#include "game/creature/name-info.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/song/song.hpp"
#include "game/game-data-file.hpp"
#include "game/item/item-profile-warehouse.hpp"

#include "popup/popup-manager.hpp"

#include "misc/random.hpp"
#include "misc/boost-string-includes.hpp"

#include <sstream>
#include <random>
#include <algorithm>


namespace popup
{

    

    PopupStage::PopupStage(const PopupInfo & POPUP_INFO)
    :
        PopupStageBase              (POPUP_INFO),
        elapsedTimeCounter_         (0.0f),
        secondCounter_              (10),
        combatBgTexture_            (),
        combatBgSprite_             (),
        titleUPtr_                  (),
        descUPtr_                   (),
        drawCountdown_              (3)
    {}


    PopupStage::~PopupStage()
    {}


    void PopupStage::Setup()
    {
        PopupStageBase::Setup();
        
        if (popupInfo_.Type() == Popup::CombatOver)
        {
            SetupCombatOverPopup();
        }
        else if (popupInfo_.Type() == Popup::SystemError)
        {
            SetupSystemErrorPopup();
        }
    }


    void PopupStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        if (popupInfo_.Type() == Popup::ItemProfilePleaseWait)
        {
            if (drawCountdown_ > 0)
            {
                if (0 == --drawCountdown_)
                {
                    ItemProfileSetup();
                }
            }
        }

        if (popupInfo_.Type() == Popup::CombatOver)
        {
            target.draw(combatBgSprite_, STATES);
            target.draw( * titleUPtr_, STATES);
            target.draw( * descUPtr_, STATES);
        }
        else if (popupInfo_.Type() == Popup::SystemError)
        {
            //The SystemError popup uses CombatOver popup's texture and sprite.
            target.draw(combatBgSprite_, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void PopupStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if ((popupInfo_.Type() == Popup::ResolutionChange) &&
            (secondCounter_ > 0))
        {
            elapsedTimeCounter_ += ELAPSED_TIME_SECONDS;

            if (elapsedTimeCounter_ > 1.0f)
            {
                elapsedTimeCounter_ = 0.0f;
                --secondCounter_;

                if (secondCounter_ > 0)
                {
                    sfml_util::gui::TextInfo textInfo(popupInfo_.TextInfo());
                    std::ostringstream ss;
                    ss << textInfo.text << "\n" << secondCounter_;
                    textInfo.text = ss.str();

                    textRegionUPtr_->Setup(textInfo,
                        textRegion_,
                        this);
                }
                else
                {
                    game::LoopManager::Instance()->PopupWaitEnd(Response::No);
                }
            }
        }
    }


    bool PopupStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((popupInfo_.Type() == Popup::ContentSelectionWithItem) ||
            (popupInfo_.Type() == Popup::ContentSelectionWithoutItem))
        {
            if ((KEY_EVENT.code == sf::Keyboard::I) &&
                (popupInfo_.Type() == Popup::ContentSelectionWithItem))
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Item());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::C)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Coins());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::G)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_Gems());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::M)
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::Thock).PlayRandom();

                game::LoopManager::Instance()->PopupWaitEnd(
                    Response::Select, PopupInfo::ContentNum_MeteorShards());

                return true;
            }
        }

        if ((popupInfo_.Buttons() & PopupButtons::Select) &&
            (KEY_EVENT.code == sf::Keyboard::S))
        {
            return HandleSelect();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStage::ItemProfileSetup()
    {
        game::item::ItemProfileWarehouse::Instance()->Setup();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
    }


    void PopupStage::SetupCombatOverPopup()
    {   
        switch (popupInfo_.HowCombatEnded())
        {
            case game::combat::CombatEnd::Win:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatWin).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-crossswords"));

                break;
            }

            case game::combat::CombatEnd::Lose:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatLose).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-crossbones"));

                break;
            }

            case game::combat::CombatEnd::Ran:
            case game::combat::CombatEnd::Count:
            default:
            {
                sfml_util::SoundManager::Instance()->Getsound_effect_set(
                    sfml_util::sound_effect_set::CombatLose).PlayRandom();

                sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-combat-run"));

                break;
            }
        }

        sfml_util::Invert(combatBgTexture_);
        sfml_util::Mask(combatBgTexture_, sf::Color::White);
        combatBgSprite_.setTexture(combatBgTexture_, true);
        combatBgSprite_.setColor( sf::Color(255, 255, 255, 32) );

        auto const HORIZ_RESCALE{ textRegion_.width /
            static_cast<float>(combatBgTexture_.getSize().x) };

        combatBgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (combatBgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height /
                static_cast<float>(combatBgTexture_.getSize().y) };

            combatBgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().height * 0.5f) };

        combatBgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);

        auto const TITLE_TEXT{ [&]()
            {
                switch (popupInfo_.HowCombatEnded())
                {
                    case game::combat::CombatEnd::Win:  { return "Victory!";}
                    case game::combat::CombatEnd::Lose: { return "Death Strikes!"; }
                    case game::combat::CombatEnd::Ran:
                    case game::combat::CombatEnd::Count:
                    default:                            { return "Defeat!"; }
                }
            }() };

        const sfml_util::gui::TextInfo COMBAT_TITLE_TEXTINFO(
            TITLE_TEXT,
            sfml_util::FontManager::Instance()->Font_BigFlavor1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        titleUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupTitle",
            COMBAT_TITLE_TEXTINFO,
            sf::FloatRect());

        auto const TITLE_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (titleUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TITLE_POS_TOP{ textRegion_.top + sfml_util::MapByRes(20.0f, 60.0f) };

        titleUPtr_->SetEntityPos(TITLE_POS_LEFT, TITLE_POS_TOP);

        auto const DESC_TEXT{ [&]()
            {
                std::ostringstream descSS;
                switch (popupInfo_.HowCombatEnded())
                {
                    case game::combat::CombatEnd::Win:
                    {
                        descSS << "Congratulations, your party has beaten all enemies on the "
                            << "field of battle and emerged victorious.\n\nWill you search "
                            << "for loot?";
                        break;
                    }
                    case game::combat::CombatEnd::Lose:
                    {
                        descSS << "The dangers of Etan have claimed another party of "
                            << "adventurers.  All of your characters have been "
                            << "incapacitated or killed, but all is not lost.  Your "
                            << "saved games remain and can be loaded at any time.\n\n"
                            << "Click YES to restart from your last save, or click NO "
                            << "to quit.";
                        break;
                    }
                    case game::combat::CombatEnd::Ran:
                    case game::combat::CombatEnd::Count:
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
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        const sf::FloatRect COMBAT_DESC_RECT(
            textRegion_.left,
            titleUPtr_->GetEntityRegion().top +
                titleUPtr_->GetEntityRegion().height +
                sfml_util::MapByRes(20.0f, 60.0f),
            textRegion_.width,
            textRegion_.height -
                (titleUPtr_->GetEntityRegion().height +
                    (sfml_util::MapByRes(20.0f, 60.0f) * 2.0f)));

        descUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CombatOverPopupDesc",
            COMBAT_DESC_TEXTINFO,
            COMBAT_DESC_RECT);
    }


    void PopupStage::SetupSystemErrorPopup()
    {
        sfml_util::LoadTexture(combatBgTexture_, game::GameDataFile::Instance()->
                    GetMediaPath("media-images-misc-error"));

        sfml_util::Mask(combatBgTexture_, sf::Color::White);
        combatBgSprite_.setTexture(combatBgTexture_, true);
        combatBgSprite_.setColor( sf::Color(255, 255, 255, 32) );

        auto const HORIZ_RESCALE{ textRegion_.width /
            static_cast<float>(combatBgTexture_.getSize().x) };

        combatBgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (combatBgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height /
                static_cast<float>(combatBgTexture_.getSize().y) };

            combatBgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f)) -
            (combatBgSprite_.getGlobalBounds().height * 0.5f) };

        combatBgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);
    }

}
