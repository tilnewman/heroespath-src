// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_PLAYER_INITIAL_SETUP_HPP_INCLUDED
#define HEROESPATH_CREATURE_PLAYER_INITIAL_SETUP_HPP_INCLUDED
//
// player-initial-setup.hpp
//
#include "item/item-factory.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace creature
{

    // Responsible for the initial setup of player characters. (items/spells/starting health/etc.)
    class PlayerInitialSetup
    {
    public:
        PlayerInitialSetup(const PlayerInitialSetup &) = delete;
        PlayerInitialSetup(PlayerInitialSetup &&) = delete;
        PlayerInitialSetup & operator=(const PlayerInitialSetup &) = delete;
        PlayerInitialSetup & operator=(PlayerInitialSetup &&) = delete;

        PlayerInitialSetup() = default;

        void Setup(const creature::CreaturePtr_t) const;

    private:
        void SetupInventory(const creature::CreaturePtr_t) const;
        void SetupSpellsAndSongs(const creature::CreaturePtr_t) const;
        void EquipBodyParts(const creature::CreaturePtr_t) const;
        Health_t GetStartingHealth(const creature::CreaturePtr_t) const;
        void SetStartingHealth(const creature::CreaturePtr_t) const;
        void SetStartingMana(const creature::CreaturePtr_t) const;

        void EnsureItemAddedAndEquipped(const creature::CreaturePtr_t, const item::ItemPtr_t) const;

        const item::ItemPtr_t MakePlainBoots(const bool IS_PIXIE) const;
        const item::ItemPtr_t MakePlainShirt(const bool IS_PIXIE) const;
        const item::ItemPtr_t MakePlainPants(const bool IS_PIXIE) const;
        const item::ItemPtr_t MakePlainWand(const bool IS_PIXIE) const;

        item::ItemFactory itemFactory_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_PLAYER_INITIAL_SETUP_HPP_INCLUDED
