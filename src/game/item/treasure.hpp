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
#ifndef GAME_ITEM_TREASURE_HPP_INCLUDED
#define GAME_ITEM_TREASURE_HPP_INCLUDED
//
// treasure.hpp
//  Functions that determine what treasure a defeated enemy party has.
//
#include "game/stats/types.hpp"
#include "game/item/treasure-info.hpp"
#include "game/item/item-profile.hpp"
#include "game/item/treasure-image-enum.hpp"

#include <vector>


namespace game
{
namespace non_player
{
    class Character;
    using CharacterPtr_t = Character *;
    using CharacterPVec_t = std::vector<CharacterPtr_t>;
}
namespace item
{

    struct ItemCache;


    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;


    struct TreasureFactory
    {
        //Decides what treasure the given creatures have and adds it
        //to items_OutParam.  It is assumed, but not required, that
        //items_OutParam is empty when Make is called.
        static TreasureImage::Enum Make(
            const non_player::CharacterPVec_t & CHARACTER_PVEC,
            ItemCache &                         items_OutParam);

        static float TreasureRatioPer(const non_player::CharacterPVec_t &);

    private:
        static const item::TreasureInfo MakeRandTreasureInfo(
            const non_player::CharacterPVec_t &);

        static void SelectItems(
            const stats::Trait_t TREASURE_SCORE,
            const bool           IS_RELIGIOUS,
            ItemCache &          items_OutParam);

        static void RemoveTreasureScoresHigherThan(
            const stats::Trait_t,
            item::ItemProfileVec_t &,
            const bool IS_RELIGIOUS);

        static std::size_t SelectRandomWeighted(const item::ItemProfileVec_t &);

        static double TreasureScoreToWeight(const int);

        static void RemoveSetItemsAlreadyOwned(item::ItemProfileVec_t &);

        static const item::ItemProfile ItemToSetItemProfile(const item::ItemPtr_t);
    };

}
}

#endif //GAME_ITEM_TREASURE_HPP_INCLUDED
