// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// slider-entity.cpp
//
#include "slider-entity.hpp"

#include "sfml-util/i-entity.hpp"

namespace heroespath
{
namespace sfml_util
{

    EntitySlider::EntitySlider()
        : PosSlider()
        , guiEntityPtrOpt_(boost::none)
    {}

    EntitySlider::EntitySlider(
        const IEntityPtrOpt_t & ENTITY_PTR_OPT,
        const sf::Vector2f & FROM_POS_V,
        const sf::Vector2f & TO_POS_V,
        const float SLIDER_SPEED,
        const WillOscillate WILL_OSCILLATE,
        const WillAutoStart WILL_AUTO_START,
        const std::size_t CYCLE_COUNT_LIMIT)
        : PosSlider(
              FROM_POS_V,
              TO_POS_V,
              SLIDER_SPEED,
              WILL_OSCILLATE,
              WILL_AUTO_START,
              CYCLE_COUNT_LIMIT)
        , guiEntityPtrOpt_(ENTITY_PTR_OPT)
    {
        OnUpdateChange();
    }

    EntitySlider::~EntitySlider() = default;

    void EntitySlider::SetEntity(const IEntityPtrOpt_t PTR_OPT)
    {
        guiEntityPtrOpt_ = PTR_OPT;
        OnUpdateChange();
    }

    void EntitySlider::OnUpdateChange()
    {
        if (guiEntityPtrOpt_)
        {
            guiEntityPtrOpt_.value()->SetEntityPos(Value());
        }
    }

} // namespace sfml_util
} // namespace heroespath
