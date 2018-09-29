// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
//  entity-slider.cpp
//
#include "entity-slider.hpp"

#include "sfml-util/gui/i-entity.hpp"

namespace heroespath
{
namespace sfml_util
{

    EntitySlider::EntitySlider(
        const IEntityPtrOpt_t & ENTITY_PTR_OPT,
        const sf::Vector2f & FROM_POS_V,
        const sf::Vector2f & TO_POS_V,
        const float SLIDER_SPEED)
        : PosSlider(FROM_POS_V, TO_POS_V, SLIDER_SPEED)
        , guiEntityPtrOpt_(ENTITY_PTR_OPT)
    {
        Setup(guiEntityPtrOpt_, FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }

    EntitySlider::~EntitySlider() = default;

    void EntitySlider::Setup(
        const IEntityPtrOpt_t & ENTITY_PTR_OPT,
        const sf::Vector2f & FROM_POS_V,
        const sf::Vector2f & TO_POS_V,
        const float SLIDER_SPEED)
    {
        guiEntityPtrOpt_ = ENTITY_PTR_OPT;
        PosSlider::Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }

    bool EntitySlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const bool RESULT { PosSlider::UpdateTime(ELAPSED_TIME_SECONDS) };

        if (RESULT && guiEntityPtrOpt_)
        {
            guiEntityPtrOpt_.value()->SetEntityPos(Position());
        }

        return RESULT;
    }

} // namespace sfml_util
} // namespace heroespath
