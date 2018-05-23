// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// adventure-stage-interact-stage.hpp
//
#include "adventure-stage-interact-stage.hpp"
#include "interact/i-interaction.hpp"
#include "interact/lock-interactions.hpp"
#include "map/map.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-util.hpp"
#include <numeric>

namespace heroespath
{
namespace stage
{

    const sf::Uint8 InteractStage::CONTEXT_IMAGE_ALPHA_{ 32 };
    const float InteractStage::SUBJECT_REGION_WIDTH_RATIO_{ 0.2f };
    const float InteractStage::SUBJECT_REGION_HEIGHT_RATIO_{ 0.5f };
    const float InteractStage::SUBJECT_IMAGE_PAD_RATIO_{ 0.9f };
    const float InteractStage::CONTEXT_IMAGE_PAD_RATIO_{ 0.8f };

    InteractStage::InteractStage(
        map::Map & map,
        const sf::FloatRect & STAGE_REGION,
        interact::InteractionManager & interactionManager)
        : Stage("AdventureInteract", STAGE_REGION, false)
        , map_(map)
        , interactionManager_(interactionManager)
        , subjectSprite_()
        , contextSprite_()
        , npcSprite_()
        , textRegionUPtr_(
              std::make_unique<sfml_util::gui::TextRegion>("AdventureStage'sInteractStage's"))
        , buttons_()
        , lockPicking_()
    {}

    InteractStage::~InteractStage() = default;

    bool InteractStage::HandleCallback(
        const sfml_util::gui::callback::TextButtonCallbackPackage_t & PACKAGE)
    {
        auto const INTERACTION_PTR_OPT{ interactionManager_.Current() };
        if (INTERACTION_PTR_OPT)
        {
            return INTERACTION_PTR_OPT->Obj().OnButtonClick(
                this,
                sfml_util::gui::TextButtonPtr_t(
                    const_cast<sfml_util::gui::TextButton *>(PACKAGE.PTR_)));
        }

        return false;
    }

    bool InteractStage::HandleCallback(const popup::PopupResponse & RESPONSE)
    {
        if (RESPONSE.Info().Name() == lockPicking_.POPUP_NAME_CHARACTER_SELECTION_)
        {
            if (lockPicking_.HandleCharacterSelectionPopupResponse(this, RESPONSE))
            {
                return false;
            }
            else
            {
                interactionManager_.Unlock();
                return true;
            }
        }
        else if (RESPONSE.Info().Name() == lockPicking_.POPUP_NAME_ATTEMPTING_)
        {
            // at this point the attempting popup has finished playing the sfx and closed
            if (lockPicking_.Attempt())
            {
                if (lockPicking_.HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(this))
                {
                    // return the opposite because we need to return false if actually opening a
                    // popup window
                    return false;
                }
                else
                {
                    if (interactionManager_.Current())
                    {
                        interactionManager_.Current()->Obj().OnSuccess(this);
                        return true;
                    }
                }
            }
            else
            {
                interactionManager_.Unlock();
            }
        }
        else if (RESPONSE.Info().Name() == lockPicking_.POPUP_NAME_TITLE_ARCHIEVED_)
        {
            if (interactionManager_.Current())
            {
                interactionManager_.Current()->Obj().OnSuccess(this);
                return true;
            }
        }

        return true;
    }

    void InteractStage::Setup() {}

    void InteractStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        DrawInteraction(target);
        Stage::Draw(target, STATES);
    }

    void InteractStage::UpdateTime(const float)
    {
        if (interactionManager_.HasCurrentChanged())
        {
            SetupInteractionForDrawing(interactionManager_.Current());
        }
    }

    bool InteractStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (interactionManager_.IsLocked() == false)
        {
            auto const INTERACTION_PTR_OPT{ interactionManager_.Current() };
            if (INTERACTION_PTR_OPT)
            {
                return INTERACTION_PTR_OPT->Obj().OnKeyRelease(this, KEY_EVENT.code);
            }
        }

        return false;
    }

    void InteractStage::MapTransition(const map::Transition & TRANSITION)
    {
        map_.TransitionLevel(TRANSITION);
    }

    void InteractStage::SetupInteractionForDrawing(
        const interact::IInteractionPtrOpt_t INTERACTION_PTR_OPT)
    {
        if (INTERACTION_PTR_OPT)
        {
            auto const INTERACTION_PTR{ INTERACTION_PTR_OPT.value() };

            const sf::FloatRect SUBJECT_REGION{ StageRegionLeft(),
                                                StageRegionTop(),
                                                StageRegionWidth() * SUBJECT_REGION_WIDTH_RATIO_,
                                                StageRegionHeight()
                                                    * SUBJECT_REGION_HEIGHT_RATIO_ };

            const sf::FloatRect TEXT_REGION{ StageRegionLeft() + SUBJECT_REGION.width,
                                             StageRegionTop(),
                                             StageRegionWidth() - SUBJECT_REGION.width,
                                             SUBJECT_REGION.height };

            const sf::FloatRect BUTTON_REGION{ StageRegionLeft(),
                                               StageRegionTop() + SUBJECT_REGION.height,
                                               StageRegionWidth(),
                                               StageRegionHeight() - SUBJECT_REGION.height };

            const sf::FloatRect CONTEXT_REGION{ TEXT_REGION.left,
                                                TEXT_REGION.top,
                                                TEXT_REGION.width,
                                                TEXT_REGION.height + BUTTON_REGION.height };

            contextSprite_.setTexture(INTERACTION_PTR->ContextTexture(), true);
            contextSprite_.setColor(sf::Color(255, 255, 255, CONTEXT_IMAGE_ALPHA_));

            sfml_util::CenterAndScaleSpriteToFit(
                contextSprite_, CONTEXT_REGION, CONTEXT_IMAGE_PAD_RATIO_);

            subjectSprite_.setTexture(INTERACTION_PTR->SubjectTexture(), true);

            sfml_util::CenterAndScaleSpriteToFit(
                subjectSprite_, SUBJECT_REGION, SUBJECT_IMAGE_PAD_RATIO_);

            textRegionUPtr_->Setup(
                INTERACTION_PTR->Text(), TEXT_REGION, sfml_util::IStagePtr_t(this));

            for (auto & buttonUPtr : buttons_)
            {
                EntityRemove(buttonUPtr.get());
            }

            buttons_.clear();

            for (auto & button : INTERACTION_PTR->Buttons())
            {
                buttons_.emplace_back(button.Make(this));
            }

            auto const ALL_BUTTONS_HEIGHT{ std::accumulate(
                std::begin(buttons_),
                std::end(buttons_),
                0.0f,
                [](auto const SUBTOTAL, auto const & UPTR) {
                    return SUBTOTAL + UPTR->GetEntityRegion().height;
                }) };

            auto const BUTTON_REGION_HORIZ_MID{ BUTTON_REGION.left + (BUTTON_REGION.width * 0.5f) };
            auto const BUTTON_REGION_VERT_MID{ BUTTON_REGION.top + (BUTTON_REGION.height * 0.5f) };
            auto const BUTTON_VERT_START{ BUTTON_REGION_VERT_MID - (ALL_BUTTONS_HEIGHT * 0.5f) };

            auto vertPos{ BUTTON_VERT_START };
            for (auto & buttonUPtr : buttons_)
            {
                buttonUPtr->SetEntityPos(
                    BUTTON_REGION_HORIZ_MID - (buttonUPtr->GetEntityRegion().width * 0.5f),
                    vertPos);

                vertPos += buttonUPtr->GetEntityRegion().height;

                EntityAdd(buttonUPtr.get());
            }

            if (INTERACTION_PTR->Type() == interact::Interact::Conversation)
            {
                npcSprite_ = INTERACTION_PTR->NpcSprite();
                sfml_util::CenterAndScaleSpriteToFit(npcSprite_, SUBJECT_REGION);
            }
        }
        else
        {
            for (auto & buttonUPtr : buttons_)
            {
                EntityRemove(buttonUPtr.get());
            }

            buttons_.clear();
        }
    }

    void InteractStage::DrawInteraction(sf::RenderTarget & target) const
    {
        if (interactionManager_.Current())
        {
            target.draw(contextSprite_);
            target.draw(subjectSprite_);

            if (interactionManager_.Current().value().Obj().Type()
                == interact::Interact::Conversation)
            {
                target.draw(npcSprite_);
            }

            target.draw(*textRegionUPtr_);
        }
    }

} // namespace stage
} // namespace heroespath
