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
//  Code that draws a sliderbar with buttons at either end and a carrot in the center that can be
//  dragged.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/gui-entity-image.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/sliderbar-style.hpp"
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

        class SliderBar;
        namespace callback
        {
            using SliderBarCallbackPackage_t = sfml_util::callback::PtrWrapper<SliderBar>;

            using ISliderBarCallbackHandler_t
                = sfml_util::callback::ICallbackHandler<SliderBarCallbackPackage_t, bool>;

            using ISliderBarCallbackHandlerPtr_t = misc::NotNull<ISliderBarCallbackHandler_t *>;

            using ISliderBarCallbackHandlerPtrOpt_t
                = boost::optional<ISliderBarCallbackHandlerPtr_t>;
        } // namespace callback

        // Encapsulates a gui sliderbar with mouse clickable increment arrows and a slider pad.
        class SliderBar : public GuiEntity
        {
        public:
            SliderBar(const SliderBar &) = delete;
            SliderBar(SliderBar &&) = delete;
            SliderBar & operator=(const SliderBar &) = delete;
            SliderBar & operator=(SliderBar &&) = delete;

        public:
            SliderBar(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const float LENGTH,
                const SliderStyle & STYLE,
                const callback::ISliderBarCallbackHandlerPtrOpt_t CHANGE_HANDLER_PTR_OPT
                = boost::none,
                const float INITIAL_VALUE = 0.0f); // must be [0.0f, 1.0f]

            virtual ~SliderBar() = default;

            float GetCurrentValue() const { return currentVal_; }
            void SetCurrentValue(const float NEW_VAL);
            virtual void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V);

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            // Overrides from GuiEntity
            bool UpdateMouseWheel(
                const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) override;

            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;
            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            virtual void SetOnChangeHandler(
                const callback::ISliderBarCallbackHandlerPtr_t CHANGE_HANDLER_PTR)
            {
                changeHandlerPtrOpt_ = CHANGE_HANDLER_PTR;
            }

            float GetLength() const { return LENGTH_; }

        protected:
            void OnClick(const sf::Vector2f &) override {}
            virtual void OnChange(const float NEW_VALUE);
            void Setup();
            virtual void SetPadPosition();
            void SetupAllPositions();
            void SetupEntityRegion();
            void SetEntityRegions();

        protected:
            float currentVal_; // always [0.0f, 1.0f]
            const float LENGTH_;
            const SliderStyle STYLE_;
            GuiImage botOrLeftImage_;
            GuiImage topOrRightImage_;
            GuiImage barImage_;
            GuiImage padImage_;
            callback::ISliderBarCallbackHandlerPtrOpt_t changeHandlerPtrOpt_;
        };

        using SliderBarPtr_t = misc::NotNull<SliderBar *>;
        using SliderBarPtrOpt_t = boost::optional<SliderBarPtr_t>;
        using SliderBarUPtr_t = std::unique_ptr<SliderBar>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDERBAR_HPP_INCLUDED
