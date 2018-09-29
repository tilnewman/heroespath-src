// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDERBAR_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDERBAR_HPP_INCLUDED
//
// sliderbar.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/entity.hpp"
#include "sfml-util/sliderbar-style.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Encapsulates a gui sliderbar with mouse clickable increment arrows and a slider pad.
    class SliderBar : public Entity
    {
    public:
        using Callback_t = Callback<SliderBar>;

        SliderBar(const SliderBar &) = delete;
        SliderBar(SliderBar &&) = delete;
        SliderBar & operator=(const SliderBar &) = delete;
        SliderBar & operator=(SliderBar &&) = delete;

        SliderBar(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const SliderStyle & STYLE,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT = boost::none,
            const float INITIAL_POS_RATIO = 0.0f,
            const bool WILL_INVERT_POSITION = false); // must be [0.0f, 1.0f]

        virtual ~SliderBar() = default;

        float PositionRatio() const { return currentPosRatio_; }

        // calls the callback
        void PositionRatio(const float NEW_VAL);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        bool UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) override;

        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
        bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        float GetLength() const { return length_; }

        void SetCallbackHandler(const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT)
        {
            callbackHandlerPtrOpt_ = CALLBACK_HANDLER_PTR_OPT;
        }

    protected:
        float CalcPositionRatioChangeOnMouseUp(const sf::Vector2f & MOUSE_POS_V) const;
        float CalcPositionRatioFromScreenPos(const sf::Vector2f & SCREEN_POS_V) const;

        virtual void OnChange(const float NEW_VALUE);

        const sf::IntRect TextureCoordsBaseOnStyle_BottomOrLeft(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_BottomOrLeft_Horizontal(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_BottomOrLeft_Vertical(const SliderStyle STYLE) const;

        const sf::IntRect TextureCoordsBaseOnStyle_TopOrRight(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_TopOrRight_Horizontal(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_TopOrRight_Vertical(const SliderStyle STYLE) const;

        const sf::IntRect TextureCoordsBasedOnStyle_Bar(const SliderStyle STYLE) const;
        const sf::IntRect TextureCoordsBasedOnStyle_Pad(const SliderStyle STYLE) const;

        // calls the callback
        void UpdatePadPosition();

        const sf::Vector2f CalcPadPosition() const;

        void ResetMouseDownStatus();

        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        float CalcPositionRatioChangeOnMouseUp_Inner(const sf::Vector2f & MOUSE_POS_V) const;
        float CalcPositionRatioFromScreenPos_Inner(const sf::Vector2f & SCREEN_POS_V) const;

    public:
        static const float POS_OFFSET_HORIZ_;
        static const float POS_OFFSET_VERT_;

    protected:
        static const float MOVE_AMOUNT_RATIO_;

        float currentPosRatio_;
        float length_;
        SliderStyle style_;
        sfml_util::CachedTexture guiElementsCachedTexture_;
        sf::Sprite botOrLeftSprite_;
        sf::Sprite topOrRightSprite_;
        sf::Sprite barSprite_;
        sf::Sprite padSprite_;
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
        bool botOrLeftIsMouseDown_;
        bool topOrRightIsMouseDown_;
        bool barIsMouseDown_;
        bool padIsMouseDown_;
        bool willInvertPosition_;
    };

    using SliderBarPtr_t = misc::NotNull<SliderBar *>;
    using SliderBarPtrOpt_t = boost::optional<SliderBarPtr_t>;
    using SliderBarUPtr_t = std::unique_ptr<SliderBar>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDERBAR_HPP_INCLUDED
