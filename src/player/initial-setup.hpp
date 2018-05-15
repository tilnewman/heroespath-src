// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PLAYER_INITIAL_SETUP_HPP_INCLUDED
#define HEROESPATH_PLAYER_INITIAL_SETUP_HPP_INCLUDED
//
// initial-setup.hpp
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
namespace player
{

    // Responsible for the initial setup of player characters. (items/spells/starting health/etc.)
    class InitialSetup
    {
    public:
        InitialSetup(const InitialSetup &) = delete;
        InitialSetup(InitialSetup &&) = delete;
        InitialSetup & operator=(const InitialSetup &) = delete;
        InitialSetup & operator=(InitialSetup &&) = delete;

        InitialSetup() = default;

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

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_INITIAL_SETUP_HPP_INCLUDED
