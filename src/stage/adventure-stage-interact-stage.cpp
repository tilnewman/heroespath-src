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
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/text-region.hpp"

#include <numeric>

namespace heroespath
{
namespace stage
{

    const sf::Uint8 InteractStage::CONTEXT_IMAGE_ALPHA_ { 32 };
    const sf::Uint8 InteractStage::BACKGROUND_ALPHA_ { 28 };
    const float InteractStage::SUBJECT_REGION_WIDTH_RATIO_ { 0.2f };
    const float InteractStage::SUBJECT_REGION_HEIGHT_RATIO_ { 0.5f };
    const float InteractStage::SUBJECT_IMAGE_PAD_RATIO_ { 0.9f };
    const float InteractStage::CONTEXT_IMAGE_PAD_RATIO_ { 0.8f };

    InteractStage::InteractStage(
        map::Map & map,
        const sf::FloatRect & REGION,
        interact::InteractionManager & interactionManager)
        : Stage("AdventureInteract", REGION, { sfml_util::GuiFont::DialogMedieval }, false)
        , map_(map)
        , regionPad_(sfml_util::MapByRes(18.0f, 52.0f))
        , innerRect_(
              REGION.left + regionPad_,
              REGION.top + regionPad_,
              REGION.width - (regionPad_ * 2.0f),
              REGION.height - (regionPad_ * 2.0f))
        , interactionManager_(interactionManager)
        , subjectSprite_()
        , contextSprite_()
        , npcSprite_()
        , textRegionUPtr_(
              std::make_unique<sfml_util::TextRegion>("AdventureStage'sInteractStage's"))
        , buttons_()
        , lockPicking_()
        , backgroundColoredRect_(innerRect_, sf::Color(0, 0, 0, BACKGROUND_ALPHA_))
    {}

    InteractStage::~InteractStage() = default;

    bool InteractStage::HandleCallback(
        const sfml_util::TextButton::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        auto const INTERACTION_PTR_OPT { interactionManager_.Current() };
        if (INTERACTION_PTR_OPT)
        {
            return INTERACTION_PTR_OPT.value()->OnButtonClick(
                this,
                sfml_util::TextButtonPtr_t(const_cast<sfml_util::TextButton *>(PACKET_PTR.Ptr())));
        }

        return false;
    }

    bool InteractStage::HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t & PACKET)
    {
        if (PACKET->Name() == lockPicking_.POPUP_NAME_CHARACTER_SELECTION_)
        {
            if (lockPicking_.HandleCharacterSelectionPopupResponse(this, PACKET))
            {
                return false;
            }
            else
            {
                interactionManager_.Unlock();
                return true;
            }
        }
        else if (PACKET->Name() == lockPicking_.POPUP_NAME_ATTEMPTING_)
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
                        interactionManager_.Current().value()->OnSuccess(this);
                        return true;
                    }
                }
            }
            else
            {
                interactionManager_.Unlock();
            }
        }
        else if (PACKET->Name() == lockPicking_.POPUP_NAME_TITLE_ARCHIEVED_)
        {
            if (interactionManager_.Current())
            {
                interactionManager_.Current().value()->OnSuccess(this);
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
            auto const INTERACTION_PTR_OPT { interactionManager_.Current() };
            if (INTERACTION_PTR_OPT)
            {
                return INTERACTION_PTR_OPT.value()->OnKeyRelease(this, KEY_EVENT.code);
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
            auto const INTERACTION_PTR { INTERACTION_PTR_OPT.value() };

            const sf::FloatRect SUBJECT_REGION { innerRect_.left,
                                                 innerRect_.top,
                                                 innerRect_.width * SUBJECT_REGION_WIDTH_RATIO_,
                                                 innerRect_.height * SUBJECT_REGION_HEIGHT_RATIO_ };

            auto const TEXT_REGION_TOP_MARGIN { sfml_util::MapByRes(33.0f, 99.0f) };

            const sf::FloatRect TEXT_REGION { innerRect_.left + SUBJECT_REGION.width,
                                              innerRect_.top + TEXT_REGION_TOP_MARGIN,
                                              innerRect_.width - SUBJECT_REGION.width,
                                              SUBJECT_REGION.height - TEXT_REGION_TOP_MARGIN };

            const sf::FloatRect BUTTON_REGION { innerRect_.left,
                                                innerRect_.top + SUBJECT_REGION.height,
                                                innerRect_.width,
                                                innerRect_.height - SUBJECT_REGION.height };

            const sf::FloatRect CONTEXT_REGION { TEXT_REGION.left,
                                                 TEXT_REGION.top,
                                                 TEXT_REGION.width,
                                                 TEXT_REGION.height + BUTTON_REGION.height };

            contextSprite_.setTexture(INTERACTION_PTR->ContextTexture(), true);
            contextSprite_.setColor(sf::Color(255, 255, 255, CONTEXT_IMAGE_ALPHA_));

            sfml_util::FitAndCenterTo(
                contextSprite_,
                sfml_util::ScaleAndReCenterCopy(CONTEXT_REGION, CONTEXT_IMAGE_PAD_RATIO_));

            subjectSprite_.setTexture(INTERACTION_PTR->SubjectTexture(), true);

            sfml_util::FitAndCenterTo(
                subjectSprite_,
                sfml_util::ScaleAndReCenterCopy(SUBJECT_REGION, SUBJECT_IMAGE_PAD_RATIO_));

            textRegionUPtr_->Setup(
                INTERACTION_PTR->Text(), TEXT_REGION, sfml_util::IStagePtr_t(this));

            for (auto & buttonUPtr : buttons_)
            {
                EntityRemove(buttonUPtr.get());
            }

            buttons_.clear();

            for (auto & button : INTERACTION_PTR->Buttons())
            {
                buttons_.emplace_back(
                    button.Make(sfml_util::TextButton::Callback_t::IHandlerPtrOpt_t(this)));
            }

            auto const ALL_BUTTONS_HEIGHT { std::accumulate(
                std::begin(buttons_),
                std::end(buttons_),
                0.0f,
                [](auto const SUBTOTAL, auto const & UPTR) {
                    return SUBTOTAL + UPTR->GetEntityRegion().height;
                }) };

            auto const BUTTON_REGION_HORIZ_MID { BUTTON_REGION.left
                                                 + (BUTTON_REGION.width * 0.5f) };
            auto const BUTTON_REGION_VERT_MID { BUTTON_REGION.top + (BUTTON_REGION.height * 0.5f) };
            auto const BUTTON_VERT_START { BUTTON_REGION_VERT_MID - (ALL_BUTTONS_HEIGHT * 0.5f) };

            auto vertPos { BUTTON_VERT_START };
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
                sfml_util::FitAndReCenter(npcSprite_, SUBJECT_REGION);
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
            target.draw(backgroundColoredRect_);
            target.draw(contextSprite_);
            target.draw(subjectSprite_);

            if (interactionManager_.Current().value()->Type() == interact::Interact::Conversation)
            {
                target.draw(npcSprite_);
            }

            target.draw(*textRegionUPtr_);
        }
    }

} // namespace stage
} // namespace heroespath
