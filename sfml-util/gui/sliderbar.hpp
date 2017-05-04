#ifndef SFMLUTIL_SLIDERBAR_INCLUDED
#define SFMLUTIL_SLIDERBAR_INCLUDED
//
// sliderbar.hpp
//  Code that draws a sliderbar with buttons at either end and a carrot in the center that can be dragged.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/sliderbar-style.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/gui-entity-image.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    class SliderBar;
    namespace callback
    {
        using SliderBarCallbackPackage_t = sfml_util::callback::PtrWrapper<SliderBar>;
        using ISliderBarCallbackHandler_t = sfml_util::callback::ICallbackHandler<SliderBarCallbackPackage_t, bool>;
    }


    //Encapsulates a gui sliderbar with mouse clickable increment arrows and a slider pad.
    class SliderBar : public GuiEntity
    {
        //prevent copy construction
        SliderBar(const SliderBar &);

        //prevent copy assignment
        SliderBar & operator=(const SliderBar &);

    public:
        SliderBar(const std::string & NAME,
                  const float         POS_LEFT,
                  const float         POS_TOP,
                  const float         LENGTH,
                  const SliderStyle & STYLE,
                  callback::ISliderBarCallbackHandler_t * const CHANGE_HANDLER_PTR = nullptr,
                  const float         INITIAL_VALUE = 0.0f);//must be [0.0f, 1.0f]

        virtual ~SliderBar() {}

        inline float GetCurrentValue() const { return currentVal_; }
        void SetCurrentValue(const float NEW_VAL);
        virtual void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        //Overrides from GuiEntity
        virtual bool MouseWheel     (const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION);
        virtual bool MouseDown      (const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseUp        (const sf::Vector2f & MOUSE_POS_V);
        virtual bool UpdateMousePos (const sf::Vector2f & MOUSE_POS_V);
        virtual void SetEntityPos   (const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos  (const float HORIZ, const float VERT);

        inline virtual void SetOnChangeHandler(callback::ISliderBarCallbackHandler_t * const CHANGE_HANDLER_PTR) { changeHandlerPtr_ = CHANGE_HANDLER_PTR; }

        inline float GetLength() const { return LENGTH_; }
    protected:
        inline virtual void OnClick(const sf::Vector2f &) {} //TODO sound effect?

        virtual void OnChange(const float NEW_VALUE);

        void Setup();
        virtual void SetPadPosition();
        void SetupAllPositions();
        void SetupEntityRegion();
        void SetEntityRegions();

    protected:
        float             currentVal_; //always [0.0f, 1.0f]
        const float       LENGTH_;
        const SliderStyle STYLE_;
        GuiImage          botOrLeftImage_;
        GuiImage          topOrRightImage_;
        GuiImage          barImage_;
        GuiImage          padImage_;
        callback::ISliderBarCallbackHandler_t * changeHandlerPtr_;
    };


    using SliderBarSPtr_t = std::shared_ptr<SliderBar>;
    using SliderBarSVec_t = std::vector<SliderBarSPtr_t>;

}
}

#endif //SFMLUTIL_SLIDERBAR_INCLUDED
