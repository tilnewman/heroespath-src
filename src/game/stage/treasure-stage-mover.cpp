// This is an open treasure non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-treasure, non-commercial, simple, game in the RPG style.
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
//  2. Altered treasure versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any treasure distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// treasure-stage-listbox-mover.hpp
//
#include "treasure-stage-mover.hpp"

#include "sfml-util/gui/gui-entity.hpp"


namespace game
{
namespace stage
{
namespace treasure
{

    const float StageMover::SLIDE_SPEED_{ 4.0f };


    StageMover::StageMover(const Type TREASURE_TYPE)
    :
        inventoryCharIndex_(0),
        treasureType_(TREASURE_TYPE),
        treasureSliders_(),
        inventorySliders_()
    {}


    void StageMover::StartAll()
    {
        for (auto & slider : treasureSliders_)
        {
            slider.Start();
        }

        for (auto & slider : inventorySliders_)
        {
            slider.Start();
        }
    }


    void StageMover::StopAll()
    {
        for (auto & slider : treasureSliders_)
        {
            slider.Stop();
        }

        for (auto & slider : inventorySliders_)
        {
            slider.Stop();
        }
    }


    bool StageMover::AreTreasureObjectsMoving() const
    {
        for (auto const & SLIDER : treasureSliders_)
        {
            if (SLIDER.IsMoving())
            {
                return true;
            }
        }

        return false;
    }


    bool StageMover::AreInventoryObjectsMoving() const
    {
        for (auto const & SLIDER : inventorySliders_)
        {
            if (SLIDER.IsMoving())
            {
                return true;
            }
        }

        return false;
    }


    void StageMover::AddTreasureObject(
        const sfml_util::gui::IGuiEntityPtr_t IGUI_ENTITY_PTR,
        const sf::Vector2f & ONSCREEN_POS,
        const sf::Vector2f & OFFSCREEN_POS)
    {
        treasureSliders_.push_back( sfml_util::gui::GuiEntitySlider(
            IGUI_ENTITY_PTR,
            OFFSCREEN_POS,
            ONSCREEN_POS,
            SLIDE_SPEED_) );
    }


    void StageMover::AddInventoryObject(
        const sfml_util::gui::IGuiEntityPtr_t IGUI_ENTITY_PTR,
        const sf::Vector2f & ONSCREEN_POS,
        const sf::Vector2f & OFFSCREEN_POS)
    {
        inventorySliders_.push_back(sfml_util::gui::GuiEntitySlider(
            IGUI_ENTITY_PTR,
            OFFSCREEN_POS,
            ONSCREEN_POS,
            SLIDE_SPEED_));
    }


    Type StageMover::TreasureSwitch()
    {
        for (auto & slider : treasureSliders_)
        {
            slider.ChangeDirection();
            slider.Start();
        }

        if (Type::Held == treasureType_)
        {
            treasureType_ = Type::Container;
        }
        else
        {
            treasureType_ = Type::Held;
        }

        return treasureType_;
    }


    void StageMover::InventoryIndexSet(const std::size_t NEW_INDEX)
    {
        if (inventoryCharIndex_ != NEW_INDEX)
        {
            inventoryCharIndex_ = NEW_INDEX;

            for (auto & slider : inventorySliders_)
            {
                if (slider.Direction() == sfml_util::Moving::Toward)
                {
                    slider.ChangeDirection();
                }

                slider.Start();
            }
        }
    }


    bool StageMover::UpdateTimeTreasure(const float ELAPSED_TIME_SECONDS)
    {
        auto areAnyFinished{ false };
        for (auto & slider : treasureSliders_)
        {
            if (UpdateTime(slider, ELAPSED_TIME_SECONDS))
            {
                areAnyFinished = true;
            }
        }

        if (areAnyFinished)
        {
            for (auto & slider : treasureSliders_)
            {
                slider.ChangeDirection();
                slider.Start();
            }
        }

        return areAnyFinished;
    }


    bool StageMover::UpdateTimeInventory(const float ELAPSED_TIME_SECONDS)
    {
        auto areAnyFinished{ false };
        for (auto & slider : inventorySliders_)
        {
            if (UpdateTime(slider, ELAPSED_TIME_SECONDS))
            {
                areAnyFinished = true;
            }
        }

        if (areAnyFinished)
        {
            for (auto & slider : inventorySliders_)
            {
                slider.ChangeDirection();
                slider.Start();
            }
        }

        return areAnyFinished;
    }


    void StageMover::ReplaceEntity(
        const sfml_util::gui::IGuiEntityPtr_t FROM,
        const sfml_util::gui::IGuiEntityPtr_t TO)
    {
        for (auto & slider : treasureSliders_)
        {
            if (slider.GetEntity() == FROM)
            {
                slider.SetEntity(TO);
            }
        }

        for (auto & slider : inventorySliders_)
        {
            if (slider.GetEntity() == FROM)
             {
                slider.SetEntity(TO);
            }
        }
    }


    bool StageMover::UpdateTime(
        sfml_util::gui::GuiEntitySlider & slider,
        const float ELAPSED_TIME_SECONDS)
    {
        auto const DID_FINISH_MOVING{ slider.UpdateTime(ELAPSED_TIME_SECONDS) };
        return DID_FINISH_MOVING && (slider.Direction() == sfml_util::Moving::Away);
    }

}
}
}
