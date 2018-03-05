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
#ifndef HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
#define HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
//
// treasure-stage-listbox-mover.hpp
//
#include "sfml-util/gui/gui-entity-slider.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class IGuiEntity;
        using IGuiEntityPtr_t = IGuiEntity *;
    } // namespace gui
} // namespace sfml_util

namespace stage
{
    namespace treasure
    {

        enum class Type
        {
            Held = 0,
            Container
        };

        using GuiEntitySliderVec_t = std::vector<sfml_util::gui::GuiEntitySlider>;

        // Responsible for managing the positions of GuiEntity objects that slide on and off screen.
        // Entities on the left side of the screen are called Treasure objects because they
        // correspond to the source of Treasure.  Entities on the right side of the screen are
        // called Inventory objects because they correspond to a character's inventory.  This class
        // is responsible for managing the state of each object and for presenting an interface for
        // sliding them on and off screen.
        class StageMover
        {
            StageMover() = delete;

        public:
            StageMover(
                const treasure::Type INITIAL_TREASURE_SOURCE,
                const std::size_t INITIAL_CHARACTER_INDEX);

            void StartAll();
            void StopAll();

            bool AreTreasureObjectsMoving() const;
            bool AreInventoryObjectsMoving() const;

            void AddTreasureObject(
                const sfml_util::gui::IGuiEntityPtr_t IGUI_ENTITY_PTR,
                const sf::Vector2f & ONSCREEN_POS,
                const sf::Vector2f & OFFSCREEN_POS);

            void AddInventoryObject(
                const sfml_util::gui::IGuiEntityPtr_t IGUI_ENTITY_PTR,
                const sf::Vector2f & ONSCREEN_POS,
                const sf::Vector2f & OFFSCREEN_POS);

            inline bool IsEitherMoving() const
            {
                return AreTreasureObjectsMoving() || AreInventoryObjectsMoving();
            }

            inline treasure::Type Source() const { return treasureType_; }
            inline std::size_t InventoryCharacterIndex() const { return inventoryCharIndex_; }

            treasure::Type TreasureSwitch();
            void InventoryIndexSet(const std::size_t);

            // These functions return true if the listbox has just finished
            // moving offscreen and its contents need to be updated.
            bool UpdateTimeTreasure(const float ELAPSED_TIME_SECONDS);
            bool UpdateTimeInventory(const float ELAPSED_TIME_SECONDS);

            void ReplaceEntity(
                const sfml_util::gui::IGuiEntityPtr_t FROM,
                const sfml_util::gui::IGuiEntityPtr_t TO);

        private:
            bool UpdateTime(
                sfml_util::gui::GuiEntitySlider & slider, const float ELAPSED_TIME_SECONDS);

        private:
            static const float SLIDE_SPEED_;
            //
            std::size_t inventoryCharIndex_;
            treasure::Type treasureType_;
            GuiEntitySliderVec_t treasureSliders_;
            GuiEntitySliderVec_t inventorySliders_;
        };

        using StageMoverUPtr_t = std::unique_ptr<StageMover>;
    } // namespace treasure
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
