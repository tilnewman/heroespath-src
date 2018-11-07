// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MOUSESTATE_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_MOUSESTATE_ENTITY_HPP_INCLUDED
//
// image-text-entity.hpp
//
#include "gui/entity.hpp"
#include "gui/gui-event-enum.hpp"
#include "gui/mouse-image-info.hpp"
#include "gui/mouse-text-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    class ImageEntity;
    using ImageEntityPtr_t = misc::NotNull<ImageEntity *>;
    using ImageEntityUPtr_t = std::unique_ptr<ImageEntity>;

    class TextEntity;
    using TextEntityPtr_t = misc::NotNull<TextEntity *>;
    using TextEntityUPtr_t = std::unique_ptr<TextEntity>;

    // Responsible for being a Entity that can have a unique image and text style
    // for whatever MouseState::Enum it is in. (up/down/over)
    class ImageTextEntity : public Entity
    {
    public:
        struct EventPacket
        {
            EventPacket(
                const misc::NotNull<ImageTextEntity *> & LISTBOX_PTR,
                const gui::GuiEvent::Enum GUI_EVENT,
                const sf::Vector2f & MOUSE_POS_V = sf::Vector2f(0.0f, 0.0f),
                const bool IS_MOUSE_OVER_IMAGE = false,
                const bool IS_MOUSE_OVER_TEXT = false)
                : entity_ptr(LISTBOX_PTR)
                , gui_event(GUI_EVENT)
                , mouse_pos_v(MOUSE_POS_V)
                , is_mouse_over_image(IS_MOUSE_OVER_IMAGE)
                , is_mouse_over_text(IS_MOUSE_OVER_TEXT)
            {}

            EventPacket(const EventPacket &) = default;
            EventPacket(EventPacket &&) = default;
            EventPacket & operator=(const EventPacket &) = default;
            EventPacket & operator=(EventPacket &&) = default;

            misc::NotNull<ImageTextEntity *> entity_ptr;
            gui::GuiEvent::Enum gui_event;
            sf::Vector2f mouse_pos_v;
            bool is_mouse_over_image;
            bool is_mouse_over_text;
        };

        using Callback_t = misc::Callback<EventPacket>;

        // Defines the relationship between an ImageTextEntity's mouse state and the mouse
        // states of it's image and text entity.  willSyncImageAndTextMouseState_ defines the
        // relationship between the image and text entities, see  below.
        struct MouseStateSync
            : public misc::EnumBaseCounting<MouseStateSync, misc::EnumFirstValue::None>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                // the mouse state does not change until SetMouseState() is called
                None = 0,

                // the mouse state is always ImageEntity.GetMouseState(), SetMouseState() will
                // change both this entity and the ImageEntity and also TextEntity if
                // willSyncImageAndTextMouseState_
                Image,

                // the mouse state is always TextEntity.GetMouseState(), SetMouseState() will
                // change both this entity and the TextEntity and also ImageEntity if
                // willSyncImageAndTextMouseState_
                Text,

                Count
            };

            static const std::string ToString(const Enum);
        };

        ImageTextEntity(const ImageTextEntity &) = delete;
        ImageTextEntity(ImageTextEntity &&) = delete;
        ImageTextEntity & operator=(const ImageTextEntity &) = delete;
        ImageTextEntity & operator=(ImageTextEntity &&) = delete;

    private:
        ImageTextEntity(
            const std::string & NAME,
            ImageEntityUPtr_t && IMAGE_ENTITY_UPTR,
            TextEntityUPtr_t && TEXT_ENTITY_UPTR,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const MouseStateSync::Enum MOUSE_STATE_FOLLOW,
            const bool WILL_SYNC_MOUSESTATES);

    public:
        explicit ImageTextEntity(
            const std::string & NAME,
            const MouseImageInfo & MOUSE_IMAGE_INFO = MouseImageInfo(),
            const MouseTextInfo & MOUSE_TEXT_INFO = MouseTextInfo(),
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT = boost::none,
            const MouseStateSync::Enum MOUSE_STATE_FOLLOW = MouseStateSync::None,
            const bool WILL_SYNC_MOUSESTATES = false);

        ImageTextEntity(
            const std::string & NAME,
            const MouseImageInfo & MOUSE_IMAGE_INFO,
            const TextInfo & TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT = boost::none,
            const MouseStateSync::Enum MOUSE_STATE_FOLLOW = MouseStateSync::None,
            const bool WILL_SYNC_MOUSESTATES = false);

        ImageTextEntity(
            const std::string & NAME,
            const EntityImageInfo & ENTITY_IMAGE_INFO,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT = boost::none,
            const MouseStateSync::Enum MOUSE_STATE_FOLLOW = MouseStateSync::None,
            const bool WILL_SYNC_MOUSESTATES = false);

        virtual ~ImageTextEntity();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        // if returns true then mouse state changed, mouseStateSync_ was enforced, and Sync()
        bool SetMouseState(const MouseState::Enum NEW_MOUSE_STATE) override;

        // does not actually set the position, instead it moves relative to GetEntityPos()
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        using Entity::SetEntityPos;

        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void SetEntityRegion(const sf::FloatRect & R) override;

        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
        bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

        bool WillSyncMouseStates() const { return willSyncImageAndTextMouseState_; }

        MouseStateSync::Enum MouseStateFollows() const { return mouseStateSync_; }

        bool HasImage() const { return !!imageEntityUPtr_; }

        // returns a default constructed sf::Sprite if !HasImage()
        const sf::Sprite Image() const;

        ImageEntityUPtr_t & ImageEntity() { return imageEntityUPtr_; }
        const ImageEntityUPtr_t & ImageEntity() const { return imageEntityUPtr_; }

        bool HasText() const { return !!textEntityUPtr_; }

        // returns an empty string if !HasText()
        const std::string Text() const;

        TextEntityUPtr_t & TextEntity() { return textEntityUPtr_; }
        const TextEntityUPtr_t & TextEntity() const { return textEntityUPtr_; }

        // call this if you make any changes to the image or text
        virtual void Sync();

    protected:
        void OnClick(const sf::Vector2f &) override;
        void OnDoubleClick(const sf::Vector2f &) override;

    private:
        void SyncAfterImageOrTextMouseStateChange(const IEntityPtr_t & IENTITY_CHANGED_PTR);
        void SyncRegion();

    protected:
        ImageEntityUPtr_t imageEntityUPtr_;
        TextEntityUPtr_t textEntityUPtr_;
        MouseStateSync::Enum mouseStateSync_;
        bool isMouseDown_;
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
        bool willSyncImageAndTextMouseState_;
    };

    using ImageTextEntityPtr_t = misc::NotNull<ImageTextEntity *>;
    using ImageTextEntityUPtr_t = std::unique_ptr<ImageTextEntity>;
    using ImageTextEntityUVec_t = std::vector<ImageTextEntityUPtr_t>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MOUSESTATE_ENTITY_HPP_INCLUDED
