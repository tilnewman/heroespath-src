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
// treasure-stage-listbox-mover.hpp
//
#include "treasure-stage-listbox-mover.hpp"

#include "sfml-util/gui/gui-entity.hpp"


namespace game
{
namespace stage
{
namespace TreasureStage
{

    const float ListboxMover::SLIDE_SPEED_{ 50.0f };


    ListboxMover::ListboxMover(
        const std::size_t NUM_PLAYERS,
        sfml_util::gui::IGuiEntityPtr_t sourceListboxPtr,
        const sf::Vector2f & SOURCE_ONSCREEN_POS_V,
        const sf::Vector2f & SOURCE_OFFSCREEN_POS_V,
        sfml_util::gui::IGuiEntityPtr_t targetListboxPtr,
        const sf::Vector2f & TARGET_ONSCREEN_POS_V,
        const sf::Vector2f & TARGET_OFFSCREEN_POS_V)
    :
        targetNumber_(0),
        targetNumberMax_(NUM_PLAYERS),
        sourceType_(SourceType::Worn),
        sourceSlider_(
            sourceListboxPtr,
            SOURCE_OFFSCREEN_POS_V,
            SOURCE_ONSCREEN_POS_V,
            SLIDE_SPEED_),
        targetSlider_(
            targetListboxPtr,
            TARGET_OFFSCREEN_POS_V,
            TARGET_ONSCREEN_POS_V,
            SLIDE_SPEED_)
    {
        sourceSlider_.Start();
        targetSlider_.Start();
    }


    TreasureStage::SourceType ListboxMover::SourceChange()
    {
        sourceSlider_.ChangeDirection();
        sourceSlider_.Start();
        if (SourceType::Worn == sourceType_)
        {
            sourceType_ = SourceType::Container;
        }
        else
        {
            sourceType_ = SourceType::Worn;
        }

        return sourceType_;
    }


    std::size_t ListboxMover::TargetChangeInc()
    {
        if (++targetNumber_ > targetNumberMax_)
        {
            targetNumber_ = 0;
        }

        TargetChange();
        return targetNumber_;
    }


    std::size_t ListboxMover::TargetChangeDec()
    {
        if (0 == targetNumber_)
        {
            targetNumber_ = targetNumberMax_;
        }
        else
        {
            --targetNumber_;
        }

        TargetChange();
        return targetNumber_;
    }


    bool ListboxMover::UpdateTimeSource(const float ELAPSED_TIME_SECONDS)
    {
        return UpdateTime(sourceSlider_, ELAPSED_TIME_SECONDS);
    }


    bool ListboxMover::UpdateTimeTarget(const float ELAPSED_TIME_SECONDS)
    {
        return UpdateTime(targetSlider_, ELAPSED_TIME_SECONDS);
    }


    void ListboxMover::TargetChange()
    {
        if (targetSlider_.Direction() == sfml_util::Moving::Toward)
        {
            targetSlider_.ChangeDirection();
        }

        targetSlider_.Start();
    }


    bool ListboxMover::UpdateTime(
        sfml_util::gui::GuiEntitySlider & slider,
        const float ELAPSED_TIME_SECONDS)
    {
        auto const DID_FINISH_MOVING{ slider.UpdateTime(ELAPSED_TIME_SECONDS) };

        auto const DOES_CONTENT_NEED_UPDATING{
            DID_FINISH_MOVING && (slider.Direction() == sfml_util::Moving::Away) };

        if (DOES_CONTENT_NEED_UPDATING)
        {
            slider.ChangeDirection();
            slider.Start();
        }

        return DOES_CONTENT_NEED_UPDATING;
    }

}
}
}
