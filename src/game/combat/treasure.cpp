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
// treasure.cpp
//
#include "treasure.hpp"

#include "game/item/item.hpp"
#include "game/combat/encounter.hpp"
#include "game/creature/creature.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/ownership-profile.hpp"


namespace game
{
namespace combat
{

    ItemCache::ItemCache()
    :
        coins        (0),
        meteor_shards(0),
        gems         (0),
        items_pvec   ()
    {}


    game::item::Weight_t ItemCache::Weight() const
    {
        game::item::Weight_t sum{ 0 };

        for (auto const NEXT_ITEM_PTR : items_pvec)
        {
            sum += NEXT_ITEM_PTR->Weight();
        }

        return sum;
    }


    TreasureImage::Enum TreasureFactory::Make(const non_player::CharacterPVec_t & CHARACTER_PVEC,
                                              ItemCache &                         items_OutParam)
    {
        auto areAllDeadEnemiesNonCarriers{ true };

        for (auto const NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            auto const OWNERSHIP_COMPLEXITY{
                non_player::ownership::complexity_type::FromCreature(NEXT_CHARACTER_PTR) };

            if (OWNERSHIP_COMPLEXITY == non_player::ownership::complexity_type::Animal)
            {
                continue;
            }
            else
            {
                areAllDeadEnemiesNonCarriers = false;
            }

            items_OutParam.coins += MakeCoinsForCreature(NEXT_CHARACTER_PTR);
            items_OutParam.gems += MakeGemsForCreature(NEXT_CHARACTER_PTR);
            items_OutParam.meteor_shards += MakeShardsForCreature(NEXT_CHARACTER_PTR);

            MakeItemsForCreature(NEXT_CHARACTER_PTR, items_OutParam.items_pvec);
        }

        if (areAllDeadEnemiesNonCarriers)
        {
            return TreasureImage::BonePile;
        }
        else
        {
            return TreasureImage::LockboxClosed;
        }
    }


    game::item::Coin_t TreasureFactory::MakeCoinsForCreature(
        const creature::CreaturePtr_t)
    {
        //TODO
        return 0;
    }


    item::Gem_t TreasureFactory::MakeGemsForCreature(
        const creature::CreaturePtr_t)
    {
        //TODO
        return 0;
    }


    item::Meteor_t TreasureFactory::MakeShardsForCreature(
        const creature::CreaturePtr_t)
    {
        //TODO
        return 0;
    }


    std::size_t TreasureFactory::MakeItemsForCreature(
        const creature::CreaturePtr_t,
        item::ItemPVec_t &)
    {
        //TODO
        return 0;
    }

}
}
