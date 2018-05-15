// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
#define HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
//
// initial.hpp
//  A collection of functions that performs initial setup of a player character
//
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "stats/trait.hpp"

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

    class Initial
    {
    public:
        static void Setup(const creature::CreaturePtr_t);

    private:
        static void SetupInventory(const creature::CreaturePtr_t);
        static void SetupSpellsAndSongs(const creature::CreaturePtr_t);
        static void EquipBodyParts(const creature::CreaturePtr_t);
        static Health_t GetStartingHealth(const creature::CreaturePtr_t);
        static void SetStartingHealth(const creature::CreaturePtr_t);
        static void SetStartingMana(const creature::CreaturePtr_t);

        static void
            EnsureItemAddedAndEquipped(const creature::CreaturePtr_t, const item::ItemPtr_t);

        static const item::ItemPtr_t MakePlainBoots(const bool IS_PIXIE);
        static const item::ItemPtr_t MakePlainShirt(const bool IS_PIXIE);
        static const item::ItemPtr_t MakePlainPants(const bool IS_PIXIE);
        static const item::ItemPtr_t MakePlainWand(const bool IS_PIXIE);
    };

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
