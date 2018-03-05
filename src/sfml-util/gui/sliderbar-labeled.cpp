// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// sliderbar-labeled.cpp
//
#include "sliderbar-labeled.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/sound-manager.hpp"
#include <sstream>

namespace heroespath
{
namespace sfml_util
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
                  nullptr,
                  INITIAL_VALUE)
            , threeTextInfosHolder_(THREE_TEXT_INFOS_HOLDER)
            , textRegion_("SliderBarLabeled's")
            , labelOffsetX_(LABEL_POS_OFFSET_LEFT)
            , labelOffsetY_(LABEL_POS_OFFSET_TOP)
        {
            std::ostringstream ss;
            ss << static_cast<int>(GetCurrentValue());
            TextInfo textInfo(THREE_TEXT_INFOS_HOLDER.up);
            textInfo.text = ss.str();

            const sf::FloatRect R(
                POS_LEFT + LABEL_POS_OFFSET_LEFT, POS_TOP + LABEL_POS_OFFSET_TOP, 0.0f, 0.0f);

            textRegion_.Setup(textInfo, R);
        }

        SliderBarLabeled::~SliderBarLabeled() {}

        void SliderBarLabeled::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            SliderBar::draw(target, states);
            textRegion_.draw(target, states);
        }

        void SliderBarLabeled::SetLabelPos(const float POS_LEFT, const float POS_TOP)
        {
            textRegion_.SetEntityPos(POS_LEFT, POS_TOP);
        }

        void
            SliderBarLabeled::SetLabelPosRelative(const float POS_REL_LEFT, const float POS_REL_TOP)
        {
            textRegion_.SetEntityPos(
                textRegion_.GetEntityPos().x + POS_REL_LEFT,
                textRegion_.GetEntityPos().y + POS_REL_TOP);
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

        void SliderBarLabeled::OnChange(const float NEW_VALUE_FLOAT)
        {
            const int NEW_VALUE_INT(static_cast<int>(NEW_VALUE_FLOAT * 100.0f));

            TextInfo textInfo{ GetTextInfoFromSliderValue(NEW_VALUE_INT) };

            std::ostringstream newValueIntSS;
            newValueIntSS << NEW_VALUE_INT;
            textInfo.text = newValueIntSS.str();

            sf::FloatRect textInfoRect(textRegion_.GetEntityRegion());
            textInfoRect.width = 0.0f;
            textInfoRect.height = 0.0f;

            textRegion_.Setup(textInfo, textInfoRect);
        }

        const TextInfo SliderBarLabeled::GetTextInfoFromSliderValue(const int SLIDER_VAL) const
        {
            if (SLIDER_VAL <= 0)
            {
                return threeTextInfosHolder_.down;
            }
            else if (SLIDER_VAL >= 100)
            {
                return threeTextInfosHolder_.over;
            }
            else
            {
                return threeTextInfosHolder_.up;
            }
        }

        void SliderBarLabeled::ChangeTextInfo(const MouseTextInfo & TEXT_INFO_SET)
        {
            threeTextInfosHolder_ = TEXT_INFO_SET;
            OnChange(currentVal_);
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
