// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-text-entity.cpp
//
#include "image-text-entity.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/gui/image-entity.hpp"
#include "sfml-util/gui/text-entity.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        const std::string
            ImageTextEntity::MouseStateSync::ToString(const MouseStateSync::Enum FOLLOW_TYPE)
        {
            switch (FOLLOW_TYPE)
            {
                case None:
                {
                    return "None";
                }
                case Image:
                {
                    return "Image";
                }
                case Text:
                {
                    return "Text";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(FOLLOW_TYPE, "ToString");
                }
            }
        }

        ImageTextEntity::ImageTextEntity(
            const std::string & NAME,
            ImageEntityUPtr_t && IMAGE_ENTITY_UPTR,
            TextEntityUPtr_t && TEXT_ENTITY_UPTR,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const MouseStateSync::Enum MOUSE_STATE_SYNC,
            const bool WILL_SYNC_MOUSESTATES)
            : Entity(NAME + "'s_ImageTextEntity_", 0.0f, 0.0f)
            , imageEntityUPtr_(std::move(IMAGE_ENTITY_UPTR))
            , textEntityUPtr_(std::move(TEXT_ENTITY_UPTR))
            , mouseStateSync_(MOUSE_STATE_SYNC)
            , isMouseDown_(false)
            , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
            , willSyncImageAndTextMouseState_(WILL_SYNC_MOUSESTATES)
        {
            Sync();
        }

        ImageTextEntity::ImageTextEntity(
            const std::string & NAME,
            const MouseImageInfo & MOUSE_IMAGE_INFO,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const MouseStateSync::Enum MOUSE_STATE_SYNC,
            const bool WILL_SYNC_MOUSESTATES)
            : ImageTextEntity(
                  NAME + "_Constructor2_",
                  std::make_unique<sfml_util::gui::ImageEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor2_'s_", MOUSE_IMAGE_INFO),
                  std::make_unique<sfml_util::gui::TextEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor2_'s_",
                      0.0f,
                      0.0f,
                      MOUSE_TEXT_INFO),
                  CALLBACK_HANDLER_PTR_OPT,
                  MOUSE_STATE_SYNC,
                  WILL_SYNC_MOUSESTATES)
        {}

        ImageTextEntity::ImageTextEntity(
            const std::string & NAME,
            const MouseImageInfo & MOUSE_IMAGE_INFO,
            const TextInfo & TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const MouseStateSync::Enum MOUSE_STATE_SYNC,
            const bool WILL_SYNC_MOUSESTATES)
            : ImageTextEntity(
                  NAME + "_Constructor3_",
                  std::make_unique<sfml_util::gui::ImageEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor3_'s_", MOUSE_IMAGE_INFO),
                  std::make_unique<sfml_util::gui::TextEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor3_'s_",
                      0.0f,
                      0.0f,
                      MouseTextInfo(TEXT_INFO)),
                  CALLBACK_HANDLER_PTR_OPT,
                  MOUSE_STATE_SYNC,
                  WILL_SYNC_MOUSESTATES)
        {}

        ImageTextEntity::ImageTextEntity(
            const std::string & NAME,
            const EntityImageInfo & ENTITY_IMAGE_INFO,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const MouseStateSync::Enum MOUSE_STATE_SYNC,
            const bool WILL_SYNC_MOUSESTATES)
            : ImageTextEntity(
                  NAME + "_Constructor4_",
                  std::make_unique<sfml_util::gui::ImageEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor4_'s_",
                      sfml_util::gui::MouseImageInfo(true, ENTITY_IMAGE_INFO)),
                  std::make_unique<sfml_util::gui::TextEntity>(
                      NAME + "'s_ImageTextEntity_FromConstructor4_'s_",
                      0.0f,
                      0.0f,
                      MOUSE_TEXT_INFO),
                  CALLBACK_HANDLER_PTR_OPT,
                  MOUSE_STATE_SYNC,
                  WILL_SYNC_MOUSESTATES)
        {}

        ImageTextEntity::~ImageTextEntity() = default;

        void ImageTextEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (imageEntityUPtr_)
            {
                imageEntityUPtr_->draw(target, states);
            }

            if (textEntityUPtr_)
            {
                textEntityUPtr_->draw(target, states);
            }
        }

        bool ImageTextEntity::SetMouseState(const MouseState::Enum NEW_MOUSE_STATE)
        {
            if (NEW_MOUSE_STATE != GetMouseState())
            {
                Entity::SetMouseState(NEW_MOUSE_STATE);

                bool couldRegionHaveChanged { false };

                if (imageEntityUPtr_
                    && ((MouseStateSync::Image == mouseStateSync_)
                        || (willSyncImageAndTextMouseState_
                            && (MouseStateSync::Text == mouseStateSync_) && textEntityUPtr_)))
                {
                    if (imageEntityUPtr_->SetMouseState(NEW_MOUSE_STATE))
                    {
                        couldRegionHaveChanged = true;
                    }
                }

                if (textEntityUPtr_
                    && ((MouseStateSync::Text == mouseStateSync_)
                        || (willSyncImageAndTextMouseState_
                            && (MouseStateSync::Image == mouseStateSync_) && imageEntityUPtr_)))
                {
                    if (textEntityUPtr_->SetMouseState(NEW_MOUSE_STATE))
                    {
                        couldRegionHaveChanged = true;
                    }
                }

                if (couldRegionHaveChanged)
                {
                    SyncRegion();
                }
            }

            return false;
        }

        void ImageTextEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - GetEntityPos() };
            MoveEntityPos(DIFF_V.x, DIFF_V.y);
        }

        void ImageTextEntity::MoveEntityPos(const float HORIZ, const float VERT)
        {
            Entity::MoveEntityPos(HORIZ, VERT);

            if (imageEntityUPtr_)
            {
                imageEntityUPtr_->MoveEntityPos(HORIZ, VERT);
            }

            if (textEntityUPtr_)
            {
                textEntityUPtr_->MoveEntityPos(HORIZ, VERT);
            }

            SyncRegion();
        }

        void ImageTextEntity::SetEntityRegion(const sf::FloatRect & R)
        {
            Entity::SetEntityRegion(R);

            if (imageEntityUPtr_)
            {
                imageEntityUPtr_->SetEntityRegion(R);
            }

            if (textEntityUPtr_)
            {
                textEntityUPtr_->SetEntityRegion(R);
            }

            SyncRegion();
        }

        bool ImageTextEntity::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            bool didSomethingHappend { false };

            if (imageEntityUPtr_ && imageEntityUPtr_->MouseUp(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(imageEntityUPtr_.get());
                didSomethingHappend = true;
            }

            if (textEntityUPtr_ && textEntityUPtr_->MouseUp(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(textEntityUPtr_.get());
                didSomethingHappend = true;
            }

            if (isMouseDown_ && GetEntityRegion().contains(MOUSE_POS_V))
            {
                bool didDoubleClickAlready(false);

                if (hasClockStarted_)
                {
                    hasClockStarted_ = false;

                    if (entityClock_.getElapsedTime().asMilliseconds() < DOUBLE_CLICK_TIME_MS_)
                    {
                        OnDoubleClick(MOUSE_POS_V);
                        didDoubleClickAlready = true;
                    }
                }
                else
                {
                    hasClockStarted_ = true;
                    entityClock_.restart();
                }

                if (false == didDoubleClickAlready)
                {
                    OnClick(MOUSE_POS_V);
                    didSomethingHappend = true;
                }

                isMouseDown_ = false;
            }
            else
            {
                hasClockStarted_ = false;
            }

            return didSomethingHappend;
        }

        bool ImageTextEntity::MouseDown(const sf::Vector2f & MOUSE_POS_V)
        {
            if (GetEntityRegion().contains(MOUSE_POS_V))
            {
                isMouseDown_ = true;
            }

            if (imageEntityUPtr_ && imageEntityUPtr_->MouseDown(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(imageEntityUPtr_.get());
                isMouseDown_ = true;
            }

            if (textEntityUPtr_ && textEntityUPtr_->MouseDown(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(textEntityUPtr_.get());
                isMouseDown_ = true;
            }

            return isMouseDown_;
        }

        bool ImageTextEntity::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
        {
            bool didSomethingHappend { false };

            if (imageEntityUPtr_ && imageEntityUPtr_->UpdateMousePos(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(imageEntityUPtr_.get());
                didSomethingHappend = true;
            }

            if (textEntityUPtr_ && textEntityUPtr_->UpdateMousePos(MOUSE_POS_V))
            {
                SyncAfterImageOrTextMouseStateChange(textEntityUPtr_.get());
                didSomethingHappend = true;
            }

            return didSomethingHappend;
        }

        const std::string ImageTextEntity::Text() const
        {
            if (textEntityUPtr_)
            {
                return textEntityUPtr_->GetText();
            }
            else
            {
                return "";
            }
        }

        const sf::Sprite ImageTextEntity::Image() const
        {
            if (imageEntityUPtr_)
            {
                return imageEntityUPtr_->FromMouseState().sprite;
            }
            else
            {
                return sf::Sprite();
            }
        }

        void ImageTextEntity::Sync()
        {
            // first, set the image and text mouse states equal if needed
            if (willSyncImageAndTextMouseState_ && imageEntityUPtr_ && textEntityUPtr_
                && (imageEntityUPtr_->GetMouseState() != textEntityUPtr_->GetMouseState()))
            {
                // if the image and text are supposed to be the same but they are not, then you have
                // to pick which one is slaved to the other, so use the follow state to decide, if
                // that fails then set text to image
                if (MouseStateSync::Text == mouseStateSync_)
                {
                    imageEntityUPtr_->SetMouseState(textEntityUPtr_->GetMouseState());
                }
                else
                {
                    textEntityUPtr_->SetMouseState(imageEntityUPtr_->GetMouseState());
                }
            }

            // second, set the mouse state to either image or text if sync'd
            if (imageEntityUPtr_ && (MouseStateSync::Image == mouseStateSync_)
                && (imageEntityUPtr_->GetMouseState() != GetMouseState()))
            {
                Entity::SetMouseState(imageEntityUPtr_->GetMouseState());
            }
            else if (
                textEntityUPtr_ && (MouseStateSync::Text == mouseStateSync_)
                && (textEntityUPtr_->GetMouseState() != GetMouseState()))
            {
                Entity::SetMouseState(textEntityUPtr_->GetMouseState());
            }

            // third, make sure the region is correct
            SyncRegion();
        }

        void ImageTextEntity::OnClick(const sf::Vector2f & MOUSE_POS_V)
        {
            if (callbackHandlerPtrOpt_)
            {
                const auto IS_MOUSE_OVER_IMAGE {
                    (imageEntityUPtr_ && imageEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
                };

                const auto IS_MOUSE_OVER_TEXT { (
                    textEntityUPtr_ && textEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V)) };

                EventPacket eventPackage(
                    this, GuiEvent::Click, MOUSE_POS_V, IS_MOUSE_OVER_IMAGE, IS_MOUSE_OVER_TEXT);

                callbackHandlerPtrOpt_.value()->HandleCallback(&eventPackage);
            }
        }

        void ImageTextEntity::OnDoubleClick(const sf::Vector2f & MOUSE_POS_V)
        {
            if (callbackHandlerPtrOpt_)
            {
                const auto IS_MOUSE_OVER_IMAGE {
                    (imageEntityUPtr_ && imageEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
                };

                const auto IS_MOUSE_OVER_TEXT { (
                    textEntityUPtr_ && textEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V)) };

                EventPacket eventPackage(
                    this, GuiEvent::Click, MOUSE_POS_V, IS_MOUSE_OVER_IMAGE, IS_MOUSE_OVER_TEXT);

                callbackHandlerPtrOpt_.value()->HandleCallback(
                    Callback_t::PacketPtr_t(&eventPackage));
            }
        }

        void ImageTextEntity::SyncAfterImageOrTextMouseStateChange(
            const IEntityPtr_t & IENTITY_CHANGED_PTR)
        {
            const auto NEW_MOUSE_STATE { IENTITY_CHANGED_PTR->GetMouseState() };
            const auto DID_IMAGE_CHANGE { IENTITY_CHANGED_PTR.Ptr() == imageEntityUPtr_.get() };

            const auto DOES_OTHER_ENTITY_EXIST { (
                (DID_IMAGE_CHANGE) ? !!textEntityUPtr_ : !!imageEntityUPtr_) };

            if (willSyncImageAndTextMouseState_ && DOES_OTHER_ENTITY_EXIST)
            {
                IEntityPtr_t otherEntityPtr { (
                    (DID_IMAGE_CHANGE) ? IEntityPtr_t(textEntityUPtr_.get())
                                       : IEntityPtr_t(imageEntityUPtr_.get())) };

                if (otherEntityPtr->SetMouseState(NEW_MOUSE_STATE))
                {
                    SyncRegion();
                }
            }

            if ((MouseStateSync::Image == mouseStateSync_)
                || (MouseStateSync::Text == mouseStateSync_))
            {
                Entity::SetMouseState(NEW_MOUSE_STATE);
            }
        }

        void ImageTextEntity::SyncRegion()
        {
            if (imageEntityUPtr_ && textEntityUPtr_)
            {
                Entity::SetEntityRegion(sfml_util::MininallyEnclosing(
                    imageEntityUPtr_->GetEntityRegion(), textEntityUPtr_->GetEntityRegion()));
            }
            else if (imageEntityUPtr_)
            {
                Entity::SetEntityRegion(imageEntityUPtr_->GetEntityRegion());
            }
            else if (textEntityUPtr_)
            {
                Entity::SetEntityRegion(textEntityUPtr_->GetEntityRegion());
            }
            else
            {
                Entity::SetEntityRegion(
                    sf::FloatRect(Position(GetEntityRegion()), sf::Vector2f(0.0f, 0.0f)));
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
