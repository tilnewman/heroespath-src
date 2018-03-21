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
#ifndef HEROESPATH_SFMLUTIL_GUI_SLIDERBARLABELED_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_SLIDERBARLABELED_HPP_INCLUDED
//
// sliderbar-labeled.hpp
//  A SliderBar that also displays the current value.
//
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-region.hpp"
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

        // Encapsulates a gui sliderbar with a label
        class SliderBarLabeled : public SliderBar
        {
        public:
            SliderBarLabeled(const SliderBarLabeled &) = delete;
            SliderBarLabeled(SliderBarLabeled &&) = delete;
            SliderBarLabeled & operator=(const SliderBarLabeled &) = delete;
            SliderBarLabeled & operator=(SliderBarLabeled &&) = delete;

        public:
            SliderBarLabeled(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const float LENGTH,
                const SliderStyle & STYLE,
                const MouseTextInfo & THREE_TEXT_INFOS_HOLDER,
                const float INITIAL_VALUE = 0.0f,
                const float LABEL_POS_OFFSET_LEFT = 0.0f,
                const float LABEL_POS_OFFSET_TOP = 0.0f); // must be [0.0f, 1.0f]

            virtual ~SliderBarLabeled();

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            const sf::Vector2f GetLabelPos() const { return textRegion_.GetEntityPos(); }
            void SetLabelPos(const float POS_LEFT, const float POS_TOP);
            void SetLabelPosRelative(const float POS_REL_LEFT, const float POS_REL_TOP);

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            virtual void SetLabelOffset(const float X, const float Y)
            {
                labelOffsetX_ = X;
                labelOffsetY_ = Y;
            }
            virtual sf::Vector2f GetLabelOffset() const { return { labelOffsetX_, labelOffsetY_ }; }

            void ChangeTextInfo(const MouseTextInfo &);

        protected:
            void OnClick(const sf::Vector2f &) override {}

            void OnChange(const float NEW_VALUE) override;

            const TextInfo GetTextInfoFromSliderValue(const int SLIDER_VAL) const;

        protected:
            // This var is not really about mouse text info but about the three TextInfo
            // objects required to display the current slider number.  A MouseTextInfo object
            // is just being used to hold three TextInfo objects.
            // Down==when zero, Over==when 100, and Up==when all other values.
            MouseTextInfo threeTextInfosHolder_;

            TextRegion textRegion_;
            float labelOffsetX_;
            float labelOffsetY_;
        };

        using SliderBarLabeledSPtr_t = std::shared_ptr<SliderBarLabeled>;
        using SliderBarLabeledSVec_t = std::vector<SliderBarLabeledSPtr_t>;
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_SLIDERBARLABELED_HPP_INCLUDED
