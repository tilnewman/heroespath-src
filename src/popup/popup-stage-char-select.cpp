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
// popup-stage-char-select.cpp
//
#include "popup-stage-char-select.hpp"

#include "game/game.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/creature/creature.hpp"
#include "game/loop-manager.hpp"

#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"


namespace popup
{

    PopupStageCharacterSelect::PopupStageCharacterSelect(const PopupInfo & POPUP_INFO)
    :
        PopupStageImageSelect(POPUP_INFO),
        charDetailsTextRegionUPtr_()
    {}


    PopupStageCharacterSelect::~PopupStageCharacterSelect()
    {}


    void PopupStageCharacterSelect::Setup()
    {
        PopupStageBase::Setup();

        imagesRect_ = textRegion_;

        imagesRect_.top = textRegionUPtr_->GetEntityPos().y +
            sfml_util::MapByRes(70.0f, 120.0f);

        imagesRect_.height = 300.0f;

        imagePosTop_ = (imagesRect_.top + (imagesRect_.height * 0.5f));

        auto const SYM_SCALE{ sfml_util::MapByRes(0.5f, 2.0f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        auto const X_SYM_POS_TOP{ (imagesRect_.top + (imagesRect_.height * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        auto const X_SYM_POS_LEFT{ (imagesRect_.left + (imagesRect_.width * 0.5f)) -
            (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        sf::FloatRect charDetailsTextRegion(textRegion_);

        charDetailsTextRegion.top = imagesRect_.top + imagesRect_.height;

        charDetailsTextRegion.height = 0.0f;

        auto charDetailsTextInfo{ popupInfo_.TextInfo() };
        charDetailsTextInfo.text = " ";

        charDetailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupWindow's_CharacterSelection_DetailsText",
            charDetailsTextInfo,
            charDetailsTextRegion);

        //sliderbar setup
        const float SLIDERBAR_LENGTH(textRegion_.width * 0.5f);
        const float SLIDERBAR_POS_LEFT(textRegion_.left +
            ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f));

        sliderbarPosTop_ = charDetailsTextRegionUPtr_->GetEntityRegion().top +
            charDetailsTextRegionUPtr_->GetEntityRegion().height + 100.0f;

        if (sliderbarUPtr_.get() != nullptr)
        {
            EntityRemove(sliderbarUPtr_.get());
        }

        sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
            this);

        EntityAdd(sliderbarUPtr_.get());

        isChangingImageAllowed_ =
            (game::Game::Instance()->State().Party().Characters().size() != 0);

        imageMoveQueue_.push(0);
    }


    void PopupStageCharacterSelect::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageImageSelect::Draw(target, STATES);
        charDetailsTextRegionUPtr_->draw(target, STATES);
        PopupStageBase::DrawRedX(target, STATES);
    }


    std::size_t PopupStageCharacterSelect::CountMax() const
    {
        return game::Game::Instance()->State().Party().Characters().size();
    }


    void PopupStageCharacterSelect::SetupContent(const bool WILL_ERASE)
    {
        SetupCharacterSelectDetailText(WILL_ERASE);
        SetupCharacterSelectionRejectImage(WILL_ERASE);
    }


    bool PopupStageCharacterSelect::HandleNumberKeys(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::Num1) && (popupInfo_.IsNumberValid(0)))
        {
            HandleNumberKeySelection(0);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Num2) && (popupInfo_.IsNumberValid(1)))
        {
            HandleNumberKeySelection(1);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Num3) && (popupInfo_.IsNumberValid(2)))
        {
            HandleNumberKeySelection(2);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Num4) && (popupInfo_.IsNumberValid(3)))
        {
            HandleNumberKeySelection(3);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Num5) && (popupInfo_.IsNumberValid(4)))
        {
            HandleNumberKeySelection(4);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Num6) && (popupInfo_.IsNumberValid(5)))
        {
            HandleNumberKeySelection(5);
            return true;
        }
        
        return false;
    }


    void PopupStageCharacterSelect::InvalidateSelectionIfCurrentIsInvalid()
    {
        auto const TEXT{ popupInfo_.TextVec()[imageIndex_] };
        if (TEXT.empty())
        {
            selection_ = static_cast<int>(imageIndex_);
        }
        else
        {
            selection_ = -1;//any negative value will work here
        }
    }


    void PopupStageCharacterSelect::HandleNumberKeySelection(const std::size_t SELECTION) const
    {
        sfml_util::SoundManager::Instance()->
            Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();

        game::LoopManager::Instance()->PopupWaitEnd(Response::Select, SELECTION);
    }


    void PopupStageCharacterSelect::SetupCharacterSelectDetailText(const bool WILL_ERASE)
    {
        if (WILL_ERASE)
        {
            charDetailsTextRegionUPtr_->SetText(" ");
            return;
        }

        auto const CREATURE_PTR{
            game::Game::Instance()->State().Party().GetAtOrderPos(imageIndex_) };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n"
            << CREATURE_PTR->RaceName();

        if (CREATURE_PTR->IsBeast())
        {
            if (CREATURE_PTR->Race() != game::creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }
            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << " " << CREATURE_PTR->RankClassName() << " "
                << CREATURE_PTR->RoleName() << "\n";
        }

        ss << "Health:  " << CREATURE_PTR->HealthCurrent() << "/"
            << CREATURE_PTR->HealthNormal() << "\n"
            << "Condition:  " << CREATURE_PTR->ConditionNames(3) << "\n";

        charDetailsTextRegionUPtr_->SetText(ss.str());
    }


    void PopupStageCharacterSelect::SetupCharacterSelectionRejectImage(const bool WILL_ERASE)
    {
        if (imageIndex_ < popupInfo_.TextVec().size())
        {
            auto const TEXT{ popupInfo_.TextVec()[imageIndex_] };

            if (TEXT.empty())
            {
                selection_ = static_cast<int>(imageIndex_);
            }
            else
            {
                selection_ = -1;//any negative value will work here
            }

            if (TEXT.empty() || WILL_ERASE)
            {
                willShowXImage_ = false;
                SetupCharacterSelectDetailText(false);
                charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::Black);
            }
            else
            {
                willShowXImage_ = true;
                charDetailsTextRegionUPtr_->SetText(TEXT);
                charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color(100, 32, 32));
            }
        }
    }


    void PopupStageCharacterSelect::SetCurrentTexture(const std::size_t IMAGE_INDEX)
    {
        auto const CREATURE_PTR{
            game::Game::Instance()->State().Party().GetAtOrderPos(IMAGE_INDEX) };

        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
            textureCurr_,
            CREATURE_PTR->ImageFilename(),
            true);
    }

}