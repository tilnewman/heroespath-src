// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// radio-or-check-entity.cpp
//
#include "radio-or-check-entity.hpp"

#include "gui/cached-texture.hpp"
#include "gui/gui-images.hpp"
#include "gui/image-entity.hpp"
#include "gui/text-entity.hpp"
#include "sfutil/center-of.hpp"
#include "sfutil/size-and-scale.hpp"

namespace heroespath
{
namespace gui
{

    RadioOrCheckEntity::RadioOrCheckEntity(
        const std::string & NAME,
        const bool IS_INVALID,
        const bool IS_RADIO_BUTTON,
        const Brightness::Enum BRIGHTNESS,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const float IMAGE_POS_LEFT,
        const float TEXT_POS_TOP,
        const float BETWEEN_SPACER)
        : ImageTextEntity(
            NAME + "_RadioOrCheckEntity",
            MouseImageInfo(),
            MouseTextInfo(),
            boost::none,
            ImageTextEntity::MouseStateSync::Image,
            true)
        , isRadioButton_(IS_RADIO_BUTTON)
        , isMouseDownOnImage_(false)
        , isMouseOverRegion_(false)
    {
        const CachedTexture GUI_ELEMENTS_CACHED_TEXTURE(GuiImages::PathKey());

        const auto TEXTURE_RECT_UP { GuiElementsTextureRect(IS_RADIO_BUTTON, true, BRIGHTNESS) };

        const auto TEXTURE_RECT_DOWN { GuiElementsTextureRect(IS_RADIO_BUTTON, false, BRIGHTNESS) };

        // both texture rects above have the same size
        const auto IMAGE_SIZE_V { sfutil::Size(sf::FloatRect(TEXTURE_RECT_UP)) };

        const auto TEXT_POS_LEFT { IMAGE_POS_LEFT + IMAGE_SIZE_V.x + BETWEEN_SPACER };

        textEntityUPtr_ = std::make_unique<gui::TextEntity>(
            "TextEntity_Of_" + GetEntityName(), TEXT_POS_LEFT, TEXT_POS_TOP, MOUSE_TEXT_INFO);

        const auto IMAGE_POS_TOP { sfutil::CenterOfVert(textEntityUPtr_->GetEntityRegion())
                                   - (IMAGE_SIZE_V.y * 0.5f) };

        const sf::Vector2f IMAGE_POS_V(IMAGE_POS_LEFT, IMAGE_POS_TOP);

        const EntityImageInfo IMAGE_INFO_UP(
            GUI_ELEMENTS_CACHED_TEXTURE, IMAGE_POS_V, sf::Vector2f(1.0f, 1.0f), TEXTURE_RECT_UP);

        const EntityImageInfo IMAGE_INFO_DOWN(
            GUI_ELEMENTS_CACHED_TEXTURE, IMAGE_POS_V, sf::Vector2f(1.0f, 1.0f), TEXTURE_RECT_DOWN);

        const EntityImageInfo IMAGE_INFO_DISABLED(
            GUI_ELEMENTS_CACHED_TEXTURE,
            IMAGE_POS_V,
            sf::Vector2f(1.0f, 1.0f),
            TEXTURE_RECT_UP,
            MOUSE_TEXT_INFO.disabled.color);

        const MouseImageInfo MOUSE_IMAGE_INFO(
            false, IMAGE_INFO_UP, IMAGE_INFO_DOWN, EntityImageInfo(), IMAGE_INFO_DISABLED);

        imageEntityUPtr_ = std::make_unique<gui::ImageEntity>(
            "ImageEntity_Of_" + GetEntityName(), MOUSE_IMAGE_INFO);

        if (IS_INVALID)
        {
            imageEntityUPtr_->SetMouseState(MouseState::Disabled);
        }
    }

    bool RadioOrCheckEntity::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (!IsInvalid() && isMouseDownOnImage_
            && imageEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
        {
            // radio buttons cannot be un-selected
            if (!isRadioButton_ || !IsSelected())
            {
                Select(!IsSelected());
                return true;
            }
        }

        return false;
    }

    bool RadioOrCheckEntity::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (!IsInvalid() && imageEntityUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
        {
            isMouseDownOnImage_ = true;
            return true;
        }

        return false;
    }

    bool RadioOrCheckEntity::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (GetEntityRegion().contains(MOUSE_POS_V))
        {
            if (!isMouseOverRegion_)
            {
                textEntityUPtr_->OverlayColor(textEntityUPtr_->Disabled().color);
                isMouseOverRegion_ = true;
                return true;
            }
        }
        else if (isMouseOverRegion_)
        {
            textEntityUPtr_->OverlayColor(textEntityUPtr_->FromMouseState().color);
            isMouseOverRegion_ = false;
            return true;
        }

        return false;
    }

    bool RadioOrCheckEntity::Invalid(const bool WILL_INVALIDATE)
    {
        if (WILL_INVALIDATE == IsInvalid())
        {
            return false;
        }
        else
        {
            if (WILL_INVALIDATE)
            {
                imageEntityUPtr_->SetMouseState(MouseState::Disabled);
            }
            else
            {
                imageEntityUPtr_->SetMouseState(MouseState::Up);
            }

            Sync();
            return true;
        }
    }

    bool RadioOrCheckEntity::IsSelected() const
    {
        return (imageEntityUPtr_->GetMouseState() == MouseState::Down);
    }

    bool RadioOrCheckEntity::Select(const bool WILL_SELECT)
    {
        if (IsInvalid())
        {
            return false;
        }
        else
        {
            if (WILL_SELECT == IsSelected())
            {
                return false;
            }
            else
            {
                if (WILL_SELECT)
                {
                    imageEntityUPtr_->SetMouseState(MouseState::Down);
                }
                else
                {
                    imageEntityUPtr_->SetMouseState(MouseState::Up);
                }

                Sync();
                return true;
            }
        }
    }

    const sf::IntRect RadioOrCheckEntity::GuiElementsTextureRect(
        const bool IS_RADIO_BUTTON,
        const bool IS_FOR_UP_STATE,
        const Brightness::Enum BRIGHTNESS) const
    {
        switch (BRIGHTNESS)
        {
            case Brightness::Dark:
            {
                if (IS_RADIO_BUTTON)
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::RadioButtonDarkOffI();
                    }
                    else
                    {
                        return GuiImages::RadioButtonDarkOnI();
                    }
                }
                else
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::CheckBoxDarkOffI();
                    }
                    else
                    {
                        return GuiImages::CheckBoxDarkOnI();
                    }
                }
            }
            case Brightness::Medium:
            {
                if (IS_RADIO_BUTTON)
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::RadioButtonMedOffI();
                    }
                    else
                    {
                        return GuiImages::RadioButtonMedOnI();
                    }
                }
                else
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::CheckBoxMedOffI();
                    }
                    else
                    {
                        return GuiImages::CheckBoxMedOnI();
                    }
                }
            }
            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                if (IS_RADIO_BUTTON)
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::RadioButtonBrightOffI();
                    }
                    else
                    {
                        return GuiImages::RadioButtonBrightOnI();
                    }
                }
                else
                {
                    if (IS_FOR_UP_STATE)
                    {
                        return GuiImages::CheckBoxBrightOffI();
                    }
                    else
                    {
                        return GuiImages::CheckBoxBrightOnI();
                    }
                }
            }
        }
    }

} // namespace gui
} // namespace heroespath
