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
//  gui-entity-slider.cpp
//
#include "gui-entity-slider.hpp"

#include "sfml-util/gui/i-gui-entity.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        GuiEntitySlider::GuiEntitySlider(
            const IGuiEntityPtrOpt_t ENTITY_PTR_OPT,
            const sf::Vector2f & FROM_POS_V,
            const sf::Vector2f & TO_POS_V,
            const float SLIDER_SPEED)
            : PosSlider(FROM_POS_V, TO_POS_V, SLIDER_SPEED)
            , guiEntityPtrOpt_(ENTITY_PTR_OPT)
        {
            Setup(guiEntityPtrOpt_, FROM_POS_V, TO_POS_V, SLIDER_SPEED);
        }

        GuiEntitySlider::~GuiEntitySlider() = default;

        void GuiEntitySlider::Setup(
            const IGuiEntityPtrOpt_t ENTITY_PTR_OPT,
            const sf::Vector2f & FROM_POS_V,
            const sf::Vector2f & TO_POS_V,
            const float SLIDER_SPEED)
        {
            guiEntityPtrOpt_ = ENTITY_PTR_OPT;
            PosSlider::Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
        }

        bool GuiEntitySlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
        {
            const bool RESULT{ PosSlider::UpdateTime(ELAPSED_TIME_SECONDS) };

            if (RESULT && guiEntityPtrOpt_)
            {
                guiEntityPtrOpt_->Obj().SetEntityPos(Position());
            }

            return RESULT;
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
