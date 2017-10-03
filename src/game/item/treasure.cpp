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

#include "game/game-data-file.hpp"
#include "game/item/item.hpp"
#include "game/item/item-factory.hpp"
#include "game/item/item-profile-warehouse.hpp"
#include "game/item/item-cache.hpp"
#include "game/combat/encounter.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/algorithms.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/ownership-profile.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <algorithm>


namespace game
{
namespace item
{

    TreasureImage::Enum TreasureFactory::Make(
        const non_player::CharacterPVec_t & CHARACTER_PVEC,
        ItemCache &                         itemCache_OutParam)
    {
        auto const TREASURE_SUMS{ CalculateTreasureSums(CHARACTER_PVEC) };

        itemCache_OutParam.coins = TREASURE_SUMS.Coin();
        itemCache_OutParam.gems = TREASURE_SUMS.Gem();

        SelectItems(TREASURE_SUMS.Magic(), false, itemCache_OutParam);
        SelectItems(TREASURE_SUMS.Religious(), true, itemCache_OutParam);
            
        if (itemCache_OutParam.items_pvec.empty() &&
            ((TREASURE_SUMS.Magic() > 0) || (TREASURE_SUMS.Religious() > 0)))
        {
            ForceItemSelection(itemCache_OutParam);
        }
        
        return DetermineWhichTreasureImage(TREASURE_SUMS);
    }


    TreasureInfo TreasureFactory::CalculateTreasureSums(
        const non_player::CharacterPVec_t & CHARACTER_PVEC)
    {
        TreasureInfo sums;

        for (auto const NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            using namespace non_player::ownership;
            if (complexity_type::FromCreature(NEXT_CHARACTER_PTR) != complexity_type::Animal)
            {
                sums += MakeRandTreasureInfo(CHARACTER_PVEC);
            }
        }

        return sums;
    }


    TreasureImage::Enum TreasureFactory::DetermineWhichTreasureImage(
        const TreasureInfo & TREASURE_INFO_SUM)
    {
        if (TREASURE_INFO_SUM.IsEmpty())
        {
            return TreasureImage::BonePile;
        }
        else
        {
            const stats::Trait_t LOCKBOX_COIN_SUM_MAX{
                GameDataFile::Instance()->GetCopyInt("heroespath-treasure-lockbox-coin-max") };

            return ((TREASURE_INFO_SUM.Coin() > LOCKBOX_COIN_SUM_MAX) ?
                TreasureImage::ChestClosed : TreasureImage::LockboxClosed);
        }
    }


    float TreasureFactory::TreasureRatioPer(const non_player::CharacterPVec_t & CHARACTER_PVEC)
    {
        item::TreasureInfo tScoresSum;

        for (auto const NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            tScoresSum += creature::race::TreasureScore(
                NEXT_CHARACTER_PTR->Race(), NEXT_CHARACTER_PTR->Role());
        }

        auto const COIN_AVG{
            static_cast<float>(tScoresSum.Coin()) / static_cast<float>(CHARACTER_PVEC.size()) };

        auto const GEM_AVG{
            static_cast<float>(tScoresSum.Gem()) / static_cast<float>(CHARACTER_PVEC.size()) };

        auto const MAGIC_AVG{
            static_cast<float>(tScoresSum.Magic()) / static_cast<float>(CHARACTER_PVEC.size()) };

        auto const RELIGIOUS_AVG{
          static_cast<float>(tScoresSum.Religious()) / static_cast<float>(CHARACTER_PVEC.size()) };

        auto const T_SCORES_MAX{ creature::race::TreasureScoreMax() };

        auto const COIN_RATIO{ COIN_AVG / static_cast<float>(T_SCORES_MAX.Coin()) };
        auto const GEM_RATIO{ GEM_AVG / static_cast<float>(T_SCORES_MAX.Gem()) };
        auto const MAGIC_RATIO{ MAGIC_AVG / static_cast<float>(T_SCORES_MAX.Magic()) };
        auto const RELIGIOUS_RATIO{ RELIGIOUS_AVG / static_cast<float>(T_SCORES_MAX.Religious()) };

        return (COIN_RATIO + GEM_RATIO + MAGIC_RATIO + RELIGIOUS_RATIO) / 4.0f;
    }


    const item::TreasureInfo TreasureFactory::MakeRandTreasureInfo(
        const non_player::CharacterPVec_t & CHARACTER_PVEC)
    {
        item::TreasureInfo tInfoSum;

        for (auto const NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            tInfoSum += creature::race::TreasureScore(
                NEXT_CHARACTER_PTR->Race(), NEXT_CHARACTER_PTR->Role());
        }

        auto const COIN_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Coin()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-coin-base")) };

        auto const COIN_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Coin()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-coin-mult")) };

        auto const COIN{ COIN_BASE + ::misc::random::Int(COIN_RAND_BASE) };

        auto const GEM_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Gem()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-gem-base")) };

        auto const GEM_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Gem()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-gem-mult")) };

        auto const GEM{ GEM_BASE + ::misc::random::Int(GEM_RAND_BASE) };

        auto const MAGIC_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Magic()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-magic-base")) };

        auto const MAGIC_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Magic()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-magic-mult")) };

        auto const MAGIC{ MAGIC_BASE + ::misc::random::Int(MAGIC_RAND_BASE) };

        auto const RELIGIOUS_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Religious()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-religious-base")) };

        auto const RELIGIOUS_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Religious()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-religious-mult")) };

        auto const RELIGIOUS{ RELIGIOUS_BASE + ::misc::random::Int(RELIGIOUS_RAND_BASE) };

        return item::TreasureInfo(COIN, GEM, MAGIC, RELIGIOUS);
    }


    std::size_t TreasureFactory::SelectItems(
        const stats::Trait_t TREASURE_SCORE,
        const bool           IS_RELIGIOUS,
        ItemCache &          itemCache_OutParam)
    {
        if (TREASURE_SCORE == 0)
        {
            return 0;
        }

        //This 'amount' variable works both as an amount of treasure still to be acquired and
        //as a measure of how rare items can possibly be
        auto amount{ TREASURE_SCORE };

        auto profiles{ item::ItemProfileWarehouse::Instance()->Get() };

        //Some items are 0% religious, and need to be removed if selecting a religious item.
        if (IS_RELIGIOUS)
        {
            profiles.erase(std::remove_if(
                profiles.begin(),
                profiles.end(),
                [](const auto & PROFILE)
                {
                    return (PROFILE.TreasureScore(true) == 0);
                }), profiles.end());
        }

        RemoveTreasureScoresHigherThan(amount, profiles, IS_RELIGIOUS);

        RemoveSetItemsAlreadyOwned(profiles);

        std::size_t count{ 0 };
        while (profiles.empty() == false)
        {
            auto const NEXT_ITEM_TO_ADD_PROFILE{ profiles[SelectRandomWeighted(profiles)] };

            profiles.erase(std::remove(
                profiles.begin(),
                profiles.end(),
                NEXT_ITEM_TO_ADD_PROFILE), profiles.end());

            itemCache_OutParam.items_pvec.push_back( item::ItemFactory::Instance()->Make(
                NEXT_ITEM_TO_ADD_PROFILE));

            amount -= NEXT_ITEM_TO_ADD_PROFILE.TreasureScore();
            RemoveTreasureScoresHigherThan(amount, profiles, IS_RELIGIOUS);
            ++count;

            // no more than five religious items will be allowed
            if (IS_RELIGIOUS && (count > 5))
            {
                break;
            }
        }

        return count;
    }


    void TreasureFactory::ForceItemSelection(ItemCache & itemCache_OutParam)
    {
        auto profiles{ item::ItemProfileWarehouse::Instance()->Get() };

        std::sort(
            profiles.begin(),
            profiles.end(),
            [](const auto & A, const auto & B)
        {
            return (A.TreasureScore() < B.TreasureScore());
        });

        auto const MAX_TREASURE_SCORE{ profiles[0].TreasureScore() * 2 };

        profiles.erase(std::remove_if(
            profiles.begin(),
            profiles.end(),
            [MAX_TREASURE_SCORE](const auto & PROFILE)
            {
                return (PROFILE.TreasureScore() > MAX_TREASURE_SCORE);
            }), profiles.end());

        auto const ITEM_PTR{ item::ItemFactory::Instance()->
            Make(::misc::Vector::SelectRandom(profiles)) };

        itemCache_OutParam.items_pvec.push_back( ITEM_PTR );
    }


    void TreasureFactory::RemoveTreasureScoresHigherThan(
        const stats::Trait_t REMOVE_IF_HIGHER,
        item::ItemProfileVec_t & profiles,
        const bool IS_RELIGIOUS)
    {
        if ((REMOVE_IF_HIGHER <= 0) || profiles.empty())
        {
            return;
        }

        profiles.erase(std::remove_if(
            profiles.begin(),
            profiles.end(),
            [REMOVE_IF_HIGHER, IS_RELIGIOUS](const auto & PROFILE)
            {
                return (PROFILE.TreasureScore(IS_RELIGIOUS) > REMOVE_IF_HIGHER);
            }), profiles.end());
    }


    std::size_t TreasureFactory::SelectRandomWeighted(const item::ItemProfileVec_t & PROFILES_ORIG)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((PROFILES_ORIG.empty() == false),
            "game::combat::TreasureFactory::SelectRandomWeighted() was given an empty vector.");

        auto profiles{ PROFILES_ORIG };

        std::sort(
            profiles.begin(),
            profiles.end(),
            [](const auto & A, const auto & B)
            {
                return (A.TreasureScore() < B.TreasureScore());
            });

        double weightSum{ 0.0 };

        for (auto const & NEXT_PROFILE : profiles)
        {
            weightSum += TreasureScoreToWeight(NEXT_PROFILE.TreasureScore());
        }

        auto const RAND{ ::misc::random::Double(weightSum) };

        double runningWeight{ 0.0 };
        for (std::size_t i(0); i<profiles.size(); ++i)
        {
            runningWeight += TreasureScoreToWeight(profiles[i].TreasureScore());
            if (RAND < runningWeight)
            {
                return i;
            }
        }

        return profiles.size() - 1;
    }


    double TreasureFactory::TreasureScoreToWeight(const int TREASURE_SCORE)
    {
        return 1.0 / (static_cast<double>(TREASURE_SCORE) * 0.1);
    }


    void TreasureFactory::RemoveSetItemsAlreadyOwned(item::ItemProfileVec_t & profiles)
    {
        item::ItemProfileVec_t setItemsOwnedProfiles;

        //populate setItemsOwnedProfiles with thin profiles of equipped and unequipped items
        auto const CREATURE_PVEC{ game::creature::Algorithms::Players(
            creature::Algorithms::Runaway) };

        for (auto const CREATURE_PTR : CREATURE_PVEC)
        {
            auto const UNEQUIPPED_ITEM_PTRS{ CREATURE_PTR->Inventory().Items() };

            for (auto const UNEQUIPPED_ITEM_PTR : UNEQUIPPED_ITEM_PTRS)
            {
                auto const SET_ITEM_PROFILE{ ItemToSetItemProfile(UNEQUIPPED_ITEM_PTR) };
                if (SET_ITEM_PROFILE.IsSet())
                {
                    setItemsOwnedProfiles.push_back(SET_ITEM_PROFILE);
                }
            }

            auto const EQUIPPED_ITEM_PTRS{ CREATURE_PTR->Inventory().ItemsEquipped() };

            for (auto const EQUIPPED_ITEM_PTR : EQUIPPED_ITEM_PTRS)
            {
                auto const SET_ITEM_PROFILE{ ItemToSetItemProfile(EQUIPPED_ITEM_PTR) };
                if (SET_ITEM_PROFILE.IsSet())
                {
                    setItemsOwnedProfiles.push_back(SET_ITEM_PROFILE);
                }
            }
        }

        //eliminate profiles that match the any in setItemsOwnedProfiles
        for(auto const & PROFILE : setItemsOwnedProfiles)
        {
            profiles.erase(std::remove_if(
                profiles.begin(),
                profiles.end(),
                [&PROFILE](const auto & P)
                {
                    return ((PROFILE.SetType() == P.SetType()) &&
                            (PROFILE.WeaponType() == P.WeaponType()) &&
                            (PROFILE.ArmorType() == P.ArmorType()) &&
                            (PROFILE.MiscType() == P.MiscType()));
                }), profiles.end());
        }
    }


    const item::ItemProfile TreasureFactory::ItemToSetItemProfile(const item::ItemPtr_t ITEM_PTR)
    {
        if ((ITEM_PTR->SetType() == item::set_type::Count) ||
            (ITEM_PTR->SetType() == item::set_type::NotASet))
        {
            return item::ItemProfile();
        }
        else
        {
            return item::ItemProfile(
                "",
                ITEM_PTR->Category(),
                ITEM_PTR->ArmorType(),
                ITEM_PTR->WeaponType(),
                item::unique_type::NotUnique,
                ITEM_PTR->MiscType(),
                ITEM_PTR->SetType(),
                item::named_type::NotNamed);
        }
    }

}
}
