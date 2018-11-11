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

#include "gui/text-region.hpp"
#include "interact/i-interaction.hpp"
#include "interact/lock-interactions.hpp"
#include "map/map.hpp"
#include "popup/popup-response.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

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

    InteractStage::InteractStage(interact::InteractionManager & interactionManager)
        : StageBase(
            "AdventureInteract",
            { gui::GuiFont::DialogMedieval },
            false) // Stage Region set in PreSetup() below
        , mapPtrOpt_(boost::none)
        , regionPad_(sfutil::MapByRes(18.0f, 52.0f))
        , innerRect_() // set in PreSetup() below
        , interactionManager_(interactionManager)
        , subjectSprite_()
        , contextSprite_()
        , npcSprite_()
        , textRegionUPtr_(std::make_unique<gui::TextRegion>("AdventureStage'sInteractStage's"))
        , buttons_()
        , lockPicking_()
        , backgroundColoredRect_() // set in PreSetup() below
    {}

    InteractStage::~InteractStage() = default;

    const std::string InteractStage::HandleCallback(
        const gui::TextButton::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        const auto INTERACTION_PTR_OPT { interactionManager_.Current() };
        if (INTERACTION_PTR_OPT)
        {
            return INTERACTION_PTR_OPT.value()->OnButtonClick(
                stage::InteractStagePtr_t(this), PACKET, PACKET_DESCRIPTION);
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "text button callback NOT HANDLED");
    }

    const std::string InteractStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_CHARACTER_SELECTION_)
        {
            if (lockPicking_.HandleCharacterSelectionPopupResponse(
                    stage::InteractStagePtr_t(this), PACKET, stage::IStagePtr_t(this)))
            {
                interactionManager_.Unlock();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw user decided which character will try to pick the lock and started all "
                    "the unlocking attempt stuff");
            }
            else
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw user failed to choose a valid character to try and pick the lock");
            }
        }
        else if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_ATTEMPTING_)
        {
            // at this point the attempting popup has finished playing the sfx and closed
            if (lockPicking_.Attempt())
            {
                if (lockPicking_.HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
                        stage::InteractStagePtr_t(this), stage::IStagePtr_t(this)))
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "the lock was picked and that popup has finished playing the picking sfx "
                        "and closed but that character earned a title for lockpicking so that "
                        "popup was spawned");
                }
                else
                {
                    if (interactionManager_.Current())
                    {
                        interactionManager_.Current().value()->OnSuccess(
                            stage::InteractStagePtr_t(this));

                        return MakeCallbackHandlerMessage(
                            PACKET_DESCRIPTION, "the lock was picked");
                    }
                }
            }
            else
            {
                // this unlocks the interaction NOT the door/chest/etc.
                interactionManager_.Unlock();
                return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "the lock was NOT picked");
            }
        }
        else if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_TITLE_ARCHIEVED_)
        {
            if (interactionManager_.Current())
            {
                interactionManager_.Current().value()->OnSuccess(stage::InteractStagePtr_t(this));

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "the lock was picked and the title gained popup has closed so moving on now");
            }
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "popup callback NOT HANDLED");
    }

    void InteractStage::PreSetup(const sf::FloatRect & STAGE_REGION, map::MapPtr_t mapPtr)
    {
        StageRegion(STAGE_REGION);
        mapPtrOpt_ = mapPtr;
    }

    void InteractStage::Setup()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (mapPtrOpt_),
            "stage::InteractStage::Setup() called but mapPtrOpt_ was invalid.  It "
            "should be set before this function is called by stage::StageFactory::Make(), which "
            "sets it by calling InteractStage::PreSetup().");

        innerRect_ = sf::FloatRect(
            StageRegion().left + regionPad_,
            StageRegion().top + regionPad_,
            StageRegion().width - (regionPad_ * 2.0f),
            StageRegion().height - (regionPad_ * 2.0f));

        backgroundColoredRect_.Setup(innerRect_, sf::Color(0, 0, 0, BACKGROUND_ALPHA_));
    }

    void InteractStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        DrawInteraction(target);
        StageBase::draw(target, states);
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
            const auto INTERACTION_PTR_OPT { interactionManager_.Current() };
            if (INTERACTION_PTR_OPT)
            {
                return INTERACTION_PTR_OPT.value()->OnKeyRelease(
                    stage::InteractStagePtr_t(this), KEY_EVENT.code);
            }
        }

        return false;
    }

    void InteractStage::MapTransition(const map::Transition & TRANSITION)
    {
        if (mapPtrOpt_)
        {
            mapPtrOpt_.value()->TransitionLevel(TRANSITION);
        }
    }

    void InteractStage::SetupInteractionForDrawing(
        const interact::IInteractionPtrOpt_t INTERACTION_PTR_OPT)
    {
        if (INTERACTION_PTR_OPT)
        {
            const auto INTERACTION_PTR { INTERACTION_PTR_OPT.value() };

            const sf::FloatRect SUBJECT_REGION { innerRect_.left,
                                                 innerRect_.top,
                                                 innerRect_.width * SUBJECT_REGION_WIDTH_RATIO_,
                                                 innerRect_.height * SUBJECT_REGION_HEIGHT_RATIO_ };

            const auto TEXT_REGION_TOP_MARGIN { sfutil::MapByRes(33.0f, 99.0f) };

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

            sfutil::FitAndCenterTo(
                contextSprite_,
                sfutil::ScaleAndReCenterCopy(CONTEXT_REGION, CONTEXT_IMAGE_PAD_RATIO_));

            subjectSprite_.setTexture(INTERACTION_PTR->SubjectTexture(), true);

            sfutil::FitAndCenterTo(
                subjectSprite_,
                sfutil::ScaleAndReCenterCopy(SUBJECT_REGION, SUBJECT_IMAGE_PAD_RATIO_));

            textRegionUPtr_->Setup(INTERACTION_PTR->Text(), TEXT_REGION, stage::IStagePtr_t(this));

            for (auto & buttonUPtr : buttons_)
            {
                EntityRemove(buttonUPtr);
            }

            buttons_.clear();

            for (auto & button : INTERACTION_PTR->Buttons())
            {
                buttons_.emplace_back(
                    button.Make(gui::TextButton::Callback_t::IHandlerPtrOpt_t(this)));
            }

            const auto ALL_BUTTONS_HEIGHT { std::accumulate(
                std::begin(buttons_),
                std::end(buttons_),
                0.0f,
                [](const auto SUBTOTAL, const auto & UPTR) {
                    return SUBTOTAL + UPTR->GetEntityRegion().height;
                }) };

            const auto BUTTON_REGION_HORIZ_MID { BUTTON_REGION.left
                                                 + (BUTTON_REGION.width * 0.5f) };
            const auto BUTTON_REGION_VERT_MID { BUTTON_REGION.top + (BUTTON_REGION.height * 0.5f) };
            const auto BUTTON_VERT_START { BUTTON_REGION_VERT_MID - (ALL_BUTTONS_HEIGHT * 0.5f) };

            auto vertPos { BUTTON_VERT_START };
            for (auto & buttonUPtr : buttons_)
            {
                buttonUPtr->SetEntityPos(
                    BUTTON_REGION_HORIZ_MID - (buttonUPtr->GetEntityRegion().width * 0.5f),
                    vertPos);

                vertPos += buttonUPtr->GetEntityRegion().height;

                EntityAdd(buttonUPtr);
            }

            if (INTERACTION_PTR->Type() == interact::Interact::Conversation)
            {
                npcSprite_ = INTERACTION_PTR->NpcSprite();
                sfutil::FitAndReCenter(npcSprite_, SUBJECT_REGION);
            }
        }
        else
        {
            for (auto & buttonUPtr : buttons_)
            {
                EntityRemove(buttonUPtr);
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
