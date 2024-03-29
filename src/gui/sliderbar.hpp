// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDERBAR_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDERBAR_HPP_INCLUDED
//
// sliderbar.hpp
//
#include "gui/cached-texture.hpp"
#include "gui/entity.hpp"
#include "gui/sliderbar-style.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Encapsulates a gui sliderbar with mouse clickable increment arrows and a slider pad.
    class SliderBar : public Entity
    {
    public:
        using Callback_t = misc::Callback<SliderBar>;

        SliderBar(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const SliderStyle & STYLE,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT = boost::none,
            const float INITIAL_POS_RATIO = 0.0f,
            const bool WILL_INVERT_POSITION = false,
            const float MIN_MOVE_RATIO = 0.0f); // zero means there is no min move ratio

        virtual ~SliderBar() = default;

        SliderBar(const SliderBar &) = delete;
        SliderBar(SliderBar &&) = delete;
        SliderBar & operator=(const SliderBar &) = delete;
        SliderBar & operator=(SliderBar &&) = delete;

        float PositionRatio() const { return currentPosRatio_; }

        // calls the callback
        void PositionRatio(const float NEW_VAL);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

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

        bool WillPlaySfx() const { return willPlaySfx_; }
        void WillPlaySfx(const bool WILL_PLAY_SFX) { willPlaySfx_ = WILL_PLAY_SFX; }

        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    protected:
        virtual void OnChange(const float NEW_VALUE);

    private:
        void SetupInitialSpritePositions(const sf::Vector2f POS_V);
        float CalcPositionRatioChangeOnMouseUp(const sf::Vector2f & MOUSE_POS_V) const;
        float CalcPositionRatioFromScreenPos(const sf::Vector2f & SCREEN_POS_V) const;

        const sf::IntRect TextureCoordsBaseOnStyle_TopOrLeft(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_TopOrLeft_Horizontal(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_TopOrLeft_Vertical(const SliderStyle STYLE) const;

        const sf::IntRect TextureCoordsBaseOnStyle_BottomOrRight(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_BottomOrRight_Horizontal(const SliderStyle STYLE) const;

        const sf::IntRect
            TextureCoordsBaseOnStyle_BottomOrRight_Vertical(const SliderStyle STYLE) const;

        const sf::IntRect TextureCoordsBasedOnStyle_Bar(const SliderStyle STYLE) const;
        const sf::IntRect TextureCoordsBasedOnStyle_Pad(const SliderStyle STYLE) const;

        // calls the callback
        void UpdatePadPosition();

        const sf::Vector2f CalcPadPosition() const;

        void ResetMouseDownStatus();

        float CalcMoveAmountRatio() const;

        float CalcPositionRatioChangeOnMouseUp_Impl(const sf::Vector2f & MOUSE_POS_V) const;
        float CalcPositionRatioFromScreenPos_Impl(const sf::Vector2f & SCREEN_POS_V) const;

        float CalcPadLength(
            const float SIZE_ORIG, const float RANGE, const float MIN_MOVE_RATIO) const;

        const sf::FloatRect CalcPadSpriteRegion() const;

        void SetPadPosition(const sf::Vector2f & POS_V);

    public:
        static const float POS_OFFSET_HORIZ_;
        static const float POS_OFFSET_VERT_;

    private:
        const float length_;
        const SliderStyle style_;
        const float minMoveRatio_;
        const int PAD_TEXTURE_END_LENGTH_;
        float currentPosRatio_;
        gui::CachedTexture guiElementsCachedTexture_;
        sf::Sprite topOrLeftSprite_;
        sf::Sprite botOrRightSprite_;
        sf::Sprite barSprite_;
        sf::Sprite topOrLeftPadSprite_;
        sf::Sprite middlePadSprite_;
        sf::Sprite botOrRightPadSprite_;
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
        bool topOrLeftIsMouseDown_;
        bool botOrRightIsMouseDown_;
        bool barIsMouseDown_;
        bool padIsMouseDown_;
        bool willPlaySfx_;
        bool willInvertPosition_;
        sf::Vector2f padIsMouseDownPosV_;
    };

    using SliderBarPtr_t = misc::NotNull<SliderBar *>;
    using SliderBarPtrOpt_t = boost::optional<SliderBarPtr_t>;
    using SliderBarUPtr_t = std::unique_ptr<SliderBar>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDERBAR_HPP_INCLUDED
