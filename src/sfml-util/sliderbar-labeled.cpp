// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sliderbar-labeled.cpp
//
#include "sliderbar-labeled.hpp"

#include "misc/log-macros.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-region.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <sstream>

namespace heroespath
{
namespace gui
{

    SliderBarLabeled::SliderBarLabeled(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        const SliderStyle & STYLE,
        const MouseTextInfo & THREE_TEXT_INFOS_HOLDER,
        const float INITIAL_VALUE,
        const float LABEL_POS_OFFSET_LEFT,
        const float LABEL_POS_OFFSET_TOP)
        : SliderBar(
            std::string(NAME).append("_SliderBarLabeled"),
            POS_LEFT,
            POS_TOP,
            LENGTH,
            STYLE,
            boost::none,
            INITIAL_VALUE,
            true)
        , threeTextInfosHolder_(THREE_TEXT_INFOS_HOLDER)
        , textRegionUPtr_(std::make_unique<TextRegion>("SliderBarLabeled's"))
        , labelOffsetX_(LABEL_POS_OFFSET_LEFT)
        , labelOffsetY_(LABEL_POS_OFFSET_TOP)
    {
        std::ostringstream ss;
        ss << static_cast<int>(PositionRatio());

        TextInfo textInfo(THREE_TEXT_INFOS_HOLDER.up);
        textInfo.text = ss.str();

        const sf::FloatRect R(
            POS_LEFT + LABEL_POS_OFFSET_LEFT, POS_TOP + LABEL_POS_OFFSET_TOP, 0.0f, 0.0f);

        textRegionUPtr_->Setup(textInfo, R);

        SetEntityRegion(
            sfutil::MinimallyEnclosing(GetEntityRegion(), textRegionUPtr_->GetEntityRegion()));
    }

    SliderBarLabeled::~SliderBarLabeled() = default;

    void SliderBarLabeled::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        SliderBar::draw(target, states);
        target.draw(*textRegionUPtr_, states);
    }

    void SliderBarLabeled::MoveEntityPos(const float HORIZ, const float VERT)
    {
        SliderBar::MoveEntityPos(HORIZ, VERT);
        textRegionUPtr_->MoveEntityPos(HORIZ, VERT);
    }

    void SliderBarLabeled::OnChange(const float NEW_VALUE_FLOAT)
    {
        sf::FloatRect textRect(textRegionUPtr_->GetEntityRegion());
        textRect.width = 0.0f;
        textRect.height = 0.0f;

        textRegionUPtr_->Setup(CreateTextToDisplay(NEW_VALUE_FLOAT), textRect);

        SliderBar::OnChange(NEW_VALUE_FLOAT);
    }

    const TextInfo SliderBarLabeled::TextInfoFromCurrentPositionPercent(const int PERCENT) const
    {
        if (PERCENT <= 0)
        {
            return threeTextInfosHolder_.down;
        }
        else if (PERCENT >= 100)
        {
            return threeTextInfosHolder_.over;
        }
        else
        {
            return threeTextInfosHolder_.up;
        }
    }

    const TextInfo SliderBarLabeled::CreateTextToDisplay(const float CURRENT_POS_RATIO)
    {
        const auto CURRENT_POS_PERCENT { static_cast<int>(CURRENT_POS_RATIO * 100.0f) };

        TextInfo textInfo { TextInfoFromCurrentPositionPercent(CURRENT_POS_PERCENT) };

        std::ostringstream newValueIntSS;
        newValueIntSS << CURRENT_POS_PERCENT;
        textInfo.text = newValueIntSS.str();

        return textInfo;
    }

} // namespace gui
} // namespace heroespath
