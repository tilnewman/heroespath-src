#ifndef SFMLUTIL_GUI_SLIDERBARLABELED_INCLUDED
#define SFMLUTIL_GUI_SLIDERBARLABELED_INCLUDED
//
// sliderbar-labeled.hpp
//  A SliderBar that also displays the current value.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-region.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    //Encapsulates a gui sliderbar with a label
    class SliderBarLabeled : public SliderBar
    {
        //prevent copy construction and copy assignment
        SliderBarLabeled(const SliderBarLabeled &);
        SliderBarLabeled & operator=(const SliderBarLabeled &);

    public:
        SliderBarLabeled(const std::string &   NAME,
                         const float           POS_LEFT,
                         const float           POS_TOP,
                         const float           LENGTH,
                         const SliderStyle &   STYLE,
                         const MouseTextInfo & THREE_TEXT_INFOS,
                         const float           INITIAL_VALUE         = 0.0f,
                         const float           LABEL_POS_OFFSET_LEFT = 0.0f,
                         const float           LABEL_POS_OFFSET_TOP  = 0.0f);//must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline const sf::Vector2f GetLabelPos() const { return textRegion_.GetEntityPos(); }
        void SetLabelPos(const float POS_LEFT, const float POS_TOP);
        void SetLabelPosRelative(const float POS_REL_LEFT, const float POS_REL_TOP);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        inline virtual void SetLabelOffset(const float X, const float Y) { labelOffsetX_ = X; labelOffsetY_ = Y; }
        inline virtual sf::Vector2f GetLabelOffset() const { return sf::Vector2f(labelOffsetX_, labelOffsetY_); }

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {} //TODO sound effect?

        virtual void OnChange(const float NEW_VALUE);

    protected:
        MouseTextInfo threeTextInfos_;
        TextRegion    textRegion_;
        float         labelOffsetX_;
        float         labelOffsetY_;
    };

    using SliderBarLabeledSPtr_t = std::shared_ptr<SliderBarLabeled>;
    using SliderBarLabeledSVec_t = std::vector<SliderBarLabeledSPtr_t>;

}
}

#endif //SFMLUTIL_GUI_SLIDERBARLABELED_INCLUDED




