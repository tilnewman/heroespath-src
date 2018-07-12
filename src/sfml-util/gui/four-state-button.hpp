// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_FOURSTATEBUTTON_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_FOURSTATEBUTTON_HPP_INCLUDED
//
// four-state-button.hpp
//  Drawing and handling code for a button type that has four visual and four text states.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box

        class TextRegion;
        using TextRegionPtr_t = misc::NotNull<TextRegion *>;
        using TextRegionPtrOpt_t = boost::optional<TextRegionPtr_t>;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

        class FourStateButton;
        namespace callback
        {
            using FourStateButtonCallbackPackage_t
                = sfml_util::callback::PtrWrapper<FourStateButton>;

            using IFourStateButtonCallbackHandler_t
                = sfml_util::callback::ICallbackHandler<FourStateButtonCallbackPackage_t, bool>;

            using IFourStateButtonCallbackHandlerPtr_t
                = misc::NotNull<IFourStateButtonCallbackHandler_t *>;

            using IFourStateButtonCallbackHandlerPtrOpt_t
                = boost::optional<IFourStateButtonCallbackHandlerPtr_t>;
        } // namespace callback

        // Responsible for wrapping a path for each of the four button states
        struct ButtonStateImageKeys
        {
            ButtonStateImageKeys(
                const std::string & UP = "",
                const std::string & DOWN = "",
                const std::string & OVER = "",
                const std::string & DISABLED = "")
                : up(UP)
                , down(DOWN)
                , over(OVER)
                , disabled(DISABLED)
            {}

            std::string up;
            std::string down;
            std::string over;
            std::string disabled;
        };

        // Responsible for maintaining images and text for four possible states:
        // Mouse Not-Over (up), Mouse Over (over), Mouse Down (down), and also disabled.
        // The disabled state is used when the button cannot be pressed and will not
        // change on mouse related states.
        class FourStateButton : public GuiEntity
        {
        public:
            FourStateButton(const FourStateButton &) = delete;
            FourStateButton(FourStateButton &&) = delete;
            FourStateButton & operator=(const FourStateButton &) = delete;
            FourStateButton & operator=(FourStateButton &&) = delete;

            // if using this constructor, Setup() must be called before any other functions
            explicit FourStateButton(const std::string & NAME);

            FourStateButton(
                const std::string & NAME,
                const sf::Vector2f & POS_V = sf::Vector2f(0.0f, 0.0f),
                const ButtonStateImageKeys & STATE_IMAGE_PATH_KEYS = ButtonStateImageKeys(),
                const MouseTextInfo & STATE_TEXTINFO = MouseTextInfo(),
                const TextInfo & DISABLED_TEXTINFO = TextInfo(),
                const bool WILL_BOX = false,
                const bool IS_DISABLED = false,
                const sf::Color & IMAGE_COLOR_OVERLAY = sf::Color::White,
                const float FINAL_SCALE = 1.0f);

            virtual ~FourStateButton();

            virtual void Setup(
                const sf::Vector2f & POS_V = sf::Vector2f(0.0f, 0.0f),
                const ButtonStateImageKeys & STATE_IMAGE_PATH_KEYS = ButtonStateImageKeys(),
                const MouseTextInfo & STATE_TEXTINFO = MouseTextInfo(),
                const TextInfo & DISABLED_TEXTINFO = TextInfo(),
                const bool WILL_BOX = false,
                const bool IS_DISABLED = false,
                const sf::Color & IMAGE_COLOR_OVERLAY = sf::Color::White,
                const float FINAL_SCALE = 1.0f);

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;
            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            virtual void
                SetCallbackHandler(const callback::IFourStateButtonCallbackHandlerPtr_t PTR)
            {
                callbackHandlerPtrOpt_ = PTR;
            }

            bool IsDisabled() const { return isDisabled_; }
            void SetIsDisabled(const bool);

            void SetMouseState(const MouseState::Enum) override;

            virtual void SetScaleToRes();
            virtual void SetVertPositionToBottomOfScreenByRes(const float POS_LEFT);

            const sf::Color Color() const { return sprite_.getColor(); }

            void Color(const sf::Color & NEW_COLOR) { sprite_.setColor(NEW_COLOR); }

            virtual void SetText(
                const std::string & UP,
                const std::string & DOWN = "",
                const std::string & OVER = "",
                const std::string & DISABLED = "");

            void Scale(const float NEW_SCALE);

        protected:
            void Reset();
            void OnClick(const sf::Vector2f &) override;
            virtual void OnMousePosStateChange();
            virtual void PlayMouseStateSfx();

        protected:
            bool isDisabled_;
            sfml_util::CachedTextureOpt_t upTextureOpt_;
            sfml_util::CachedTextureOpt_t downTextureOpt_;
            sfml_util::CachedTextureOpt_t overTextureOpt_;
            sfml_util::CachedTextureOpt_t disabledTextureOpt_;
            sf::Sprite sprite_;
            TextRegionPtrOpt_t currTextRegionPtrOpt_;
            TextRegionUPtr_t upTextRegionUPtr_;
            TextRegionUPtr_t downTextRegionUPtr_;
            TextRegionUPtr_t overTextRegionUPtr_;
            TextRegionUPtr_t disabledTextRegionUPtr_;
            box::BoxUPtr_t boxUPtr_;
            callback::IFourStateButtonCallbackHandlerPtrOpt_t callbackHandlerPtrOpt_;
        };

        using FourStateButtonUPtr_t = std::unique_ptr<FourStateButton>;
        using FourStateButtonUVec_t = std::vector<FourStateButtonUPtr_t>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_FOURSTATEBUTTON_HPP_INCLUDED
