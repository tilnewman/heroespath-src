// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-stage-listbox-mover.hpp
//
#include "treasure-stage-mover.hpp"

#include "misc/log-macros.hpp"

#include "gui/entity.hpp"

namespace heroespath
{
namespace stage
{
    namespace treasure
    {

        const float StageMover::SLIDE_SPEED_ { 4.0f };

        StageMover::StageMover(
            const Type INITIAL_TREASURE_TYPE, const std::size_t INITIAL_CHARACTER_INDEX)
            : inventoryCharIndex_(INITIAL_CHARACTER_INDEX)
            , treasureType_(INITIAL_TREASURE_TYPE)
            , treasureSliders_()
            , inventorySliders_()
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
            for (const auto & SLIDER : treasureSliders_)
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
            for (const auto & SLIDER : inventorySliders_)
            {
                if (SLIDER.IsMoving())
                {
                    return true;
                }
            }

            return false;
        }

        void StageMover::AddTreasureObject(
            const gui::IEntityPtrOpt_t & IGUI_ENTITY_PTR_OPT,
            const sf::Vector2f & ONSCREEN_POS,
            const sf::Vector2f & OFFSCREEN_POS)
        {
            treasureSliders_.emplace_back(
                gui::EntitySlider(IGUI_ENTITY_PTR_OPT, OFFSCREEN_POS, ONSCREEN_POS, SLIDE_SPEED_));
        }

        void StageMover::AddInventoryObject(
            const gui::IEntityPtrOpt_t & IGUI_ENTITY_PTR_OPT,
            const sf::Vector2f & ONSCREEN_POS,
            const sf::Vector2f & OFFSCREEN_POS)
        {
            inventorySliders_.emplace_back(
                gui::EntitySlider(IGUI_ENTITY_PTR_OPT, OFFSCREEN_POS, ONSCREEN_POS, SLIDE_SPEED_));
        }

        Type StageMover::TreasureSwitch()
        {
            for (auto & slider : treasureSliders_)
            {
                slider.ReverseDirection();
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
                    if (slider.Direction() == gui::Moving::Toward)
                    {
                        slider.ReverseDirection();
                    }

                    slider.Start();
                }
            }
        }

        bool StageMover::UpdateTimeTreasure(const float ELAPSED_TIME_SECONDS)
        {
            auto areAnyFinished { false };
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
                    slider.ReverseDirection();
                    slider.Start();
                }
            }

            return areAnyFinished;
        }

        bool StageMover::UpdateTimeInventory(const float ELAPSED_TIME_SECONDS)
        {
            auto areAnyFinished { false };
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
                    slider.ReverseDirection();
                    slider.Start();
                }
            }

            return areAnyFinished;
        }

        void StageMover::ReplaceEntity(
            const gui::IEntityPtr_t FROM_ENTITY_PTR, const gui::IEntityPtr_t TO_ENTITY_PTR)
        {
            for (auto & slider : treasureSliders_)
            {
                if (slider.GetEntity() == FROM_ENTITY_PTR)
                {
                    slider.SetEntity(TO_ENTITY_PTR);
                    TO_ENTITY_PTR->SetEntityPos(slider.Value());
                }
            }

            for (auto & slider : inventorySliders_)
            {
                if (slider.GetEntity() == FROM_ENTITY_PTR)
                {
                    slider.SetEntity(TO_ENTITY_PTR);
                    TO_ENTITY_PTR->SetEntityPos(slider.Value());
                }
            }
        }

        bool StageMover::UpdateTime(gui::EntitySlider & slider, const float ELAPSED_TIME_SECONDS)
        {
            const auto IS_STOPPED { slider.UpdateAndReturnIsStopped(ELAPSED_TIME_SECONDS) };
            return (IS_STOPPED && (slider.IsMovingToward() == false));
        }

    } // namespace treasure
} // namespace stage
} // namespace heroespath
