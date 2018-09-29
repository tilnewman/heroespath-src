// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-char-select.cpp
//
#include "popup-stage-char-select.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "sfml-util/image-loaders.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageCharacterSelect::PopupStageCharacterSelect(const PopupInfo & POPUP_INFO)
        : PopupStageImageSelect(POPUP_INFO)
        , charDetailsTextRegionUPtr_()
        , creatureToTextureMap_()
    {}

    PopupStageCharacterSelect::~PopupStageCharacterSelect() = default;

    void PopupStageCharacterSelect::Setup()
    {
        PopupStageBase::Setup();

        imagesRect_ = textRegion_;

        imagesRect_.top = textRegionUPtr_->GetEntityPos().y + sfml_util::MapByRes(70.0f, 120.0f);

        imagesRect_.height = sfml_util::MapByRes(125.0f, 625.0f);

        imagePosTop_ = (imagesRect_.top + (imagesRect_.height * 0.5f));

        auto const SYM_SCALE { sfml_util::MapByRes(0.5f, 2.0f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        sfml_util::CenterTo(xSymbolSprite_, imagesRect_);

        sf::FloatRect charDetailsTextRegion(textRegion_);

        charDetailsTextRegion.top = imagesRect_.top + imagesRect_.height;

        charDetailsTextRegion.height = 0.0f;

        auto charDetailsTextInfo { popupInfo_.TextInfo() };
        charDetailsTextInfo.text = " ";

        charDetailsTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "PopupWindow's_CharacterSelection_DetailsText",
            charDetailsTextInfo,
            charDetailsTextRegion);

        // sliderbar setup
        const float SLIDERBAR_LENGTH(textRegion_.width * 0.5f);
        const float SLIDERBAR_POS_LEFT(
            textRegion_.left + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f));

        sliderbarPosTop_ = charDetailsTextRegionUPtr_->GetEntityRegion().top
            + charDetailsTextRegionUPtr_->GetEntityRegion().height
            + sfml_util::MapByRes(60.0f, 300.0f);

        if (sliderbarUPtr_)
        {
            EntityRemove(sliderbarUPtr_.get());
        }

        sliderbarUPtr_ = std::make_unique<sfml_util::SliderBar>(
            "PopupStage's",
            SLIDERBAR_POS_LEFT,
            sliderbarPosTop_,
            SLIDERBAR_LENGTH,
            sfml_util::SliderStyle(sfml_util::Orientation::Horiz),
            sfml_util::SliderBar::Callback_t::IHandlerPtr_t(this));

        EntityAdd(sliderbarUPtr_.get());

        isChangingImageAllowed_
            = (game::Game::Instance()->State().Party().Characters().size() != 0);

        imageMoveQueue_.push(0);
        EnqueueImagesFromCurrentToTarget(imageIndex_, popupInfo_.InitialSelection());
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

    bool PopupStageCharacterSelect::HandleSelect()
    {
        // attempt to select the value most recently enqueued
        if (imageMoveQueue_.empty() == false)
        {
            if (popupInfo_.TextVec()[imageMoveQueue_.back()].empty() == false)
            {
                PlayInvalidKeypressSoundEffect();
                return false;
            }
            else
            {
                selection_ = static_cast<int>(imageMoveQueue_.back());
            }
        }
        else
        {
            selection_ = static_cast<int>(imageIndex_);
        }

        if ((selection_ < 0) || (selection_ >= static_cast<int>(popupInfo_.TextVec().size())))
        {
            PlayInvalidKeypressSoundEffect();
            return false;
        }

        if (popupInfo_.TextVec()[static_cast<std::size_t>(selection_)].empty() == false)
        {
            PlayInvalidKeypressSoundEffect();
            return false;
        }

        return PopupStageBase::HandleSelect();
    }

    void PopupStageCharacterSelect::SetupContent(const bool WILL_ERASE)
    {
        SetupCharacterSelectDetailText(WILL_ERASE);
        SetupCharacterSelectionRejectImage(WILL_ERASE);
    }

    void PopupStageCharacterSelect::SetupCharacterSelectDetailText(const bool WILL_ERASE)
    {
        if (WILL_ERASE)
        {
            charDetailsTextRegionUPtr_->SetText(" ");
            return;
        }

        auto const CREATURE_PTR { game::Game::Instance()->State().Party().GetAtOrderPos(
            imageIndex_) };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n" << CREATURE_PTR->RaceName();

        if (CREATURE_PTR->IsBeast())
        {
            if (CREATURE_PTR->Race() != creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }
            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << " " << CREATURE_PTR->RankClassName() << " " << CREATURE_PTR->RoleName() << "\n";
        }

        ss << "Health:  " << CREATURE_PTR->HealthCurrent() << "/" << CREATURE_PTR->HealthNormal()
           << "\n"
           << "Condition:  " << CREATURE_PTR->ConditionNames(3) << "\n";

        charDetailsTextRegionUPtr_->SetText(ss.str());
    }

    void PopupStageCharacterSelect::SetupCharacterSelectionRejectImage(const bool WILL_ERASE)
    {
        if (imageIndex_ < popupInfo_.TextVec().size())
        {
            auto const TEXT { popupInfo_.TextVec()[imageIndex_] };

            if (TEXT.empty())
            {
                selection_ = static_cast<int>(imageIndex_);
            }
            else
            {
                selection_ = -1; // any negative value will work here
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

    const sfml_util::CachedTexture &
        PopupStageCharacterSelect::GetCurrentCachedTexture(const std::size_t IMAGE_INDEX)
    {
        auto const CREATURE_PTR { game::Game::Instance()->State().Party().GetAtOrderPos(
            IMAGE_INDEX) };

        auto foundIter { creatureToTextureMap_.Find(CREATURE_PTR) };

        if (foundIter == std::end(creatureToTextureMap_))
        {
            creatureToTextureMap_.Append(
                CREATURE_PTR,
                sfml_util::LoadAndCacheImage(
                    CREATURE_PTR, sfml_util::ImageOptions::InvertedCharacterOptions()));

            foundIter = creatureToTextureMap_.Find(CREATURE_PTR);
        }

        return foundIter->second;
    }

} // namespace popup
} // namespace heroespath
