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
#ifndef GAME_COMBAT_TREASURE_HPP_INCLUDED
#define GAME_COMBAT_TREASURE_HPP_INCLUDED
//
// treasure.hpp
//  Functions that determine what treasure a defeated enemy party has.
//
#include "game/stats/types.hpp"
#include "game/item/treasure-info.hpp"
#include "game/item/item-profile.hpp"
#include "game/combat/treasure-image-enum.hpp"

#include <vector>


namespace game
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;
}
namespace non_player
{
    class Character;
    using CharacterPtr_t = Character *;
    using CharacterPVec_t = std::vector<CharacterPtr_t>;
}
namespace combat
{

    //Handy container of items used by Encounter to store treasure
    //both on enemy creatures and cached in the chest/lockbox.
    struct ItemCache
    {
        ItemCache();

        stats::Trait_t coins {0};
        stats::Trait_t gems {0};
        item::ItemPVec_t items_pvec;

        stats::Trait_t Weight() const;
    };


    struct TreasureFactory
    {
        //Decides what treasure the given creatures have and adds it
        //to items_OutParam.  It is assumed, but not required, that
        //items_OutParam is empty when Make is called.
        static TreasureImage::Enum Make(const non_player::CharacterPVec_t & CHARACTER_PVEC,
                                        ItemCache &                         items_OutParam);

    private:
        static const item::TreasureInfo MakeRandTreasureInfo(const non_player::CharacterPVec_t &);

        static void SelectItems(const stats::Trait_t TREASURE_SCORE,
                                const bool           IS_RELIGIOUS,
                                ItemCache &          items_OutParam);

        static void RemoveTreasureScoresHigherThan(const stats::Trait_t,
                                                   item::ItemProfileVec_t &,
                                                   const bool IS_RELIGIOUS);

        static std::size_t SelectRandomWeighted(const item::ItemProfileVec_t &);

        static double TreasureScoreToWeight(const int);
    };

}
}

#endif //GAME_COMBAT_TREASURE_HPP_INCLUDED
