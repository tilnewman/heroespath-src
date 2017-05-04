//
// sliderbar-labeled.cpp
//
#include "sliderbar-labeled.hpp"
#include "game/log-macros.hpp"
#include "sfml-util/sound-manager.hpp"
#include <sstream>


namespace sfml_util
{
namespace gui
{

    SliderBarLabeled::SliderBarLabeled(const std::string &   NAME,
                                       const float           POS_LEFT,
                                       const float           POS_TOP,
                                       const float           LENGTH,
                                       const SliderStyle &   STYLE,
                                       const MouseTextInfo & THREE_TEXT_INFOS,
                                       const float           INITIAL_VALUE,
                                       const float           LABEL_POS_OFFSET_LEFT,
                                       const float           LABEL_POS_OFFSET_TOP)
    :
        SliderBar      (std::string(NAME).append("_SliderBarLabeled"), POS_LEFT, POS_TOP, LENGTH, STYLE, nullptr, INITIAL_VALUE),
        threeTextInfos_(THREE_TEXT_INFOS),
        textRegion_    ("SliderBarLabeled's"),
        labelOffsetX_  (LABEL_POS_OFFSET_LEFT),
        labelOffsetY_  (LABEL_POS_OFFSET_TOP)
    {
        std::ostringstream ss;
        ss << static_cast<int>( GetCurrentValue() );
        TextInfo textInfo(THREE_TEXT_INFOS.up);
        textInfo.text = ss.str();

        const sf::FloatRect R(POS_LEFT + LABEL_POS_OFFSET_LEFT, POS_TOP + LABEL_POS_OFFSET_TOP, 0.0f, 0.0f);

        textRegion_.Setup(textInfo, R);
    }


    SliderBarLabeled::~SliderBarLabeled()
    {}


    void SliderBarLabeled::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        SliderBar::draw(target, states);
        textRegion_.draw(target, states);
    }


    void SliderBarLabeled::SetLabelPos(const float POS_LEFT, const float POS_TOP)
    {
        textRegion_.SetEntityPos(POS_LEFT, POS_TOP);
    }


    void SliderBarLabeled::SetLabelPosRelative(const float POS_REL_LEFT, const float POS_REL_TOP)
    {
        textRegion_.SetEntityPos(textRegion_.GetEntityPos().x + POS_REL_LEFT, textRegion_.GetEntityPos().y + POS_REL_TOP);
    }


    void SliderBarLabeled::OnChange(const float NEW_VALUE)
    {
        const int NEW_VALUE_INT( static_cast<int>(NEW_VALUE * 100.0f) );

        TextInfo textInfo(threeTextInfos_.up);

        if (0 == NEW_VALUE_INT)
            textInfo = threeTextInfos_.down;
        else
            if (100 == NEW_VALUE_INT)
                textInfo = threeTextInfos_.over;

        std::ostringstream ss;
        ss << NEW_VALUE_INT;
        textInfo.text = ss.str();

        sf::FloatRect r( textRegion_.GetEntityRegion() );
        r.width = 0.0f;
        r.height = 0.0f;
        textRegion_.Setup(textInfo, r);
    }


    void SliderBarLabeled::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        SliderBar::SetEntityPos(POS_LEFT, POS_TOP);
        textRegion_.SetEntityPos(POS_LEFT + labelOffsetX_, POS_TOP + labelOffsetY_);
    }


    void SliderBarLabeled::MoveEntityPos(const float HORIZ, const float VERT)
    {
        SliderBar::MoveEntityPos(HORIZ, VERT);
        textRegion_.MoveEntityPos(HORIZ, VERT);
    }

}
}
