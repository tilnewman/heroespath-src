// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
#define HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
//
// treasure-stage-listbox-mover.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/entity.hpp"
#include "sfml-util/slider-entity.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace gui
{
    class IEntity;
    using IEntityPtr_t = misc::NotNull<IEntity *>;
    using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;
} // namespace gui

namespace stage
{
    namespace treasure
    {

        enum class Type
        {
            Held = 0,
            Container
        };

        using EntitySliderVec_t = std::vector<gui::EntitySlider>;

        // Responsible for managing the positions of Entity objects that slide on and off screen.
        // Entities on the left side of the screen are called Treasure objects because they
        // correspond to the source of Treasure.  Entities on the right side of the screen are
        // called Inventory objects because they correspond to a character's inventory.  This class
        // is responsible for managing the state of each object and for presenting an interface for
        // sliding them on and off screen.
        class StageMover
        {
        public:
            StageMover() = delete;

            StageMover(
                const treasure::Type INITIAL_TREASURE_SOURCE,
                const std::size_t INITIAL_CHARACTER_INDEX);

            void StartAll();
            void StopAll();

            bool AreTreasureObjectsMoving() const;
            bool AreInventoryObjectsMoving() const;

            void AddTreasureObject(
                const gui::IEntityPtrOpt_t & IGUI_ENTITY_PTR_OPT,
                const sf::Vector2f & ONSCREEN_POS,
                const sf::Vector2f & OFFSCREEN_POS);

            void AddInventoryObject(
                const gui::IEntityPtrOpt_t & IGUI_ENTITY_PTR_OPT,
                const sf::Vector2f & ONSCREEN_POS,
                const sf::Vector2f & OFFSCREEN_POS);

            bool IsEitherMoving() const
            {
                return AreTreasureObjectsMoving() || AreInventoryObjectsMoving();
            }

            treasure::Type Source() const { return treasureType_; }
            std::size_t InventoryCharacterIndex() const { return inventoryCharIndex_; }

            treasure::Type TreasureSwitch();
            void InventoryIndexSet(const std::size_t);

            // These functions return true if the listbox has just finished
            // moving offscreen and its contents need to be updated.
            bool UpdateTimeTreasure(const float ELAPSED_TIME_SECONDS);
            bool UpdateTimeInventory(const float ELAPSED_TIME_SECONDS);

            void ReplaceEntity(
                const gui::IEntityPtr_t FROM_ENTITY_PTR, const gui::IEntityPtr_t TO_ENTITY_PTR);

        private:
            bool UpdateTime(gui::EntitySlider & slider, const float ELAPSED_TIME_SECONDS);

        private:
            static const float SLIDE_SPEED_;
            //
            std::size_t inventoryCharIndex_;
            treasure::Type treasureType_;
            EntitySliderVec_t treasureSliders_;
            EntitySliderVec_t inventorySliders_;
        };

        using StageMoverUPtr_t = std::unique_ptr<StageMover>;

    } // namespace treasure
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TREASURESTAGEMOVER_HPP_INCLUDED
