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
#ifndef HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
#define HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
//
// misc-item-factory.hpp
//  Responsible for making individual item objects.
//
#include "item/item-factory-base.hpp"
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{

    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;

    class ItemProfile;

    // A singleton class responsible for making misc item objects.
    class MiscItemFactory : public FactoryBase

    {
    public:
        MiscItemFactory(const MiscItemFactory &) = delete;
        MiscItemFactory(MiscItemFactory &&) = delete;
        MiscItemFactory & operator=(const MiscItemFactory &) = delete;
        MiscItemFactory & operator=(MiscItemFactory &&) = delete;

    public:
        MiscItemFactory();
        virtual ~MiscItemFactory();

        static MiscItemFactory * Instance();
        static void Acquire();
        static void Release();

        static const ItemPtr_t Make(const ItemProfile &);

        static const ItemPtr_t Make_Amulet(const ItemProfile &);
        static const ItemPtr_t Make_Armband(const ItemProfile &);
        static const ItemPtr_t Make_BalmPot(const ItemProfile &);
        static const ItemPtr_t Make_Braid(const ItemProfile &);
        static const ItemPtr_t Make_Cape(const ItemProfile &);
        static const ItemPtr_t Make_DevilHorn(const ItemProfile &);
        static const ItemPtr_t Make_Doll(const ItemProfile &);
        static const ItemPtr_t Make_DrumLute(const ItemProfile &);
        static const ItemPtr_t Make_Egg(const ItemProfile &);
        static const ItemPtr_t Make_Embryo(const ItemProfile &);
        static const ItemPtr_t Make_Finger(const ItemProfile &);
        static const ItemPtr_t Make_Fingerclaw(const ItemProfile &);
        static const ItemPtr_t Make_Goblet(const ItemProfile &);
        static const ItemPtr_t Make_HurdyGurdy(const ItemProfile &);
        static const ItemPtr_t Make_Icicle(const ItemProfile &);
        static const ItemPtr_t Make_Litchhand(const ItemProfile &);
        static const ItemPtr_t Make_LockPicks(const ItemProfile &);
        static const ItemPtr_t Make_Lyre(const ItemProfile &);
        static const ItemPtr_t Make_MummyHand(const ItemProfile &);
        static const ItemPtr_t Make_Orb(const ItemProfile &);
        static const ItemPtr_t Make_PetrifiedSnake(const ItemProfile &);
        static const ItemPtr_t Make_PipeAndTabor(const ItemProfile &);
        static const ItemPtr_t Make_Recorder(const ItemProfile &);
        static const ItemPtr_t Make_Ring(const ItemProfile &);
        static const ItemPtr_t Make_Scepter(const ItemProfile &);
        static const ItemPtr_t Make_Seeds(const ItemProfile &);
        static const ItemPtr_t Make_Shard(const ItemProfile &);
        static const ItemPtr_t Make_SpiderEggs(const ItemProfile &);
        static const ItemPtr_t Make_Staff(const ItemProfile &);
        static const ItemPtr_t Make_SummoningStatue(const ItemProfile &);
        static const ItemPtr_t Make_UnicornHorn(const ItemProfile &);
        static const ItemPtr_t Make_Veil(const ItemProfile &);
        static const ItemPtr_t Make_Viol(const ItemProfile &);
        static const ItemPtr_t Make_Wand(const ItemProfile &);

        static const ItemPtr_t Make_MiscBlessedOrCursed(const ItemProfile &);

        // to support the older InventoryFactory
        static const ItemPtr_t Make_Ring(const material::Enum, const material::Enum);
        static const ItemPtr_t Make_Wand(const material::Enum, const material::Enum);
        static const ItemPtr_t Make_DrumLute(const bool IS_PIXIE_ITEM);

    private:
        static const ItemPtr_t Make_Helper(
            const ItemProfile & PROFILE,
            const Coin_t & BASE_PRICE,
            const Weight_t & BASE_WEIGHT,
            const std::string & BASE_DESC = "");

    private:
        static std::unique_ptr<MiscItemFactory> instanceUPtr_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MISCITEMFACTORY_HPP_INCLUDED
