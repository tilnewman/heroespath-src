// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_RADIO_OR_CHECK_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_RADIO_OR_CHECK_ENTITY_HPP_INCLUDED
//
// radio-or-check-entity.hpp
//
#include "gui/brightness-enum.hpp"
#include "gui/image-text-entity.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class CachedTextre;
    using CachedTextureUPtr_t = std::unique_ptr<CachedTexture>;

    // Responsible for being an ImageTextEntity that works like a radio button or check box,
    // with text just to the right of the image.  Mouse states have new meaning: not-selected
    // (up), selected (down), invalid (unabe to be selected) (disabled).  The over state is only
    // used to set overlay colors. If invalid, the state cannot be changed except by calling
    // Invalid(false).  If you only call Invalid() and Select() you will never need to call
    // Sync(), it will be called automatically.  Use the MouseTextInfo.disabled.color to store
    // the disabled color overlay.
    class RadioOrCheckEntity : public ImageTextEntity
    {
    public:
        RadioOrCheckEntity(const RadioOrCheckEntity &) = delete;
        RadioOrCheckEntity(RadioOrCheckEntity &&) = delete;
        RadioOrCheckEntity & operator=(const RadioOrCheckEntity &) = delete;
        RadioOrCheckEntity & operator=(RadioOrCheckEntity &&) = delete;

        // if ColorOpt_t is initialized then the initial state is invalid
        RadioOrCheckEntity(
            const std::string & NAME,
            const bool IS_INVALID,
            const bool IS_RADIO_BUTTON, // if not then check-box is assumed
            const Brightness::Enum BRIGHTNESS,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float IMAGE_POS_LEFT = 0.0f,
            const float TEXT_POS_TOP = 0.0f,
            const float BETWEEN_SPACER = 0.0f);

        virtual ~RadioOrCheckEntity() = default;

        // does nothing, it makes no sense for the owning entity to change state, use Select()
        // or Invalid() to change the state
        bool SetMouseState(const MouseState::Enum) override { return false; }

        // returns true if IsSelected() changed
        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        // returns true if an image was clicked, never changes mouse state
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        // returns true if the mouse entered or exited the entity region (implies text overlay
        // color was changed)
        bool UpdateMousePos(const sf::Vector2f &) override;

        bool IsInvalid() const { return (GetMouseState() == MouseState::Disabled); }

        // returns true if the state changed
        bool Invalid(const bool WILL_INVALIDATE);

        bool IsSelected() const;

        // if IsInvalid() then nothing happens and false is returned, otherwise returns true if
        // the state changed
        bool Select(const bool WILL_SELECT);

        bool IsRadioButton() const { return isRadioButton_; }

    private:
        const sf::IntRect GuiElementsTextureRect(
            const bool IS_RADIO_BUTTON,
            const bool IS_FOR_UP_STATE,
            const Brightness::Enum BRIGHTNESS) const;

        bool isRadioButton_;
        bool isMouseDownOnImage_;
        bool isMouseOverRegion_;
    };

    using RadioOrCheckEntityUPtr_t = std::unique_ptr<RadioOrCheckEntity>;
    using RadioOrCheckEntityUVec_t = std::vector<RadioOrCheckEntityUPtr_t>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_RADIO_OR_CHECK_ENTITY_HPP_INCLUDED
