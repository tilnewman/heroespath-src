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
namespace combat
{

    ItemCache::ItemCache()
    :
        coins        (0),
        gems         (0),
        items_pvec   ()
    {}


    game::stats::Trait_t ItemCache::Weight() const
    {
        game::stats::Trait_t sum{ 0 };

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

            auto const TREASURE_INFO{ MakeRandTreasureInfo(CHARACTER_PVEC) };

            items_OutParam.coins = TREASURE_INFO.Coin();
            items_OutParam.gems = TREASURE_INFO.Gem();

            SelectItems(TREASURE_INFO.Magic(), false, items_OutParam);
            SelectItems(TREASURE_INFO.Religious(), true, items_OutParam);
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

        auto const COIN{ COIN_BASE + misc::random::Int(COIN_RAND_BASE) };

        auto const GEM_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Gem()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-gem-base")) };

        auto const GEM_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Gem()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-gem-mult")) };

        auto const GEM{ GEM_BASE + misc::random::Int(GEM_RAND_BASE) };

        auto const MAGIC_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Magic()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-magic-base")) };

        auto const MAGIC_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Magic()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-magic-mult")) };

        auto const MAGIC{ MAGIC_BASE + misc::random::Int(MAGIC_RAND_BASE) };

        auto const RELIGIOUS_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Religious()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-religious-base")) };

        auto const RELIGIOUS_RAND_BASE{ static_cast<stats::Trait_t>(
            static_cast<float>(tInfoSum.Religious()) *
                GameDataFile::Instance()->GetCopyFloat("heroespath-treasure-religious-mult")) };

        auto const RELIGIOUS{ RELIGIOUS_BASE + misc::random::Int(RELIGIOUS_RAND_BASE) };

        return item::TreasureInfo(COIN, GEM, MAGIC, RELIGIOUS);
    }


    void TreasureFactory::SelectItems(const stats::Trait_t TREASURE_SCORE,
                                      const bool           IS_RELIGIOUS,
                                      ItemCache &          items_OutParam)
    {
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

        while (profiles.empty() == false)
        {
            auto const NEXT_ITEM_TO_ADD_PROFILE{ profiles[SelectRandomWeighted(profiles)] };
            
            profiles.erase(std::remove(profiles.begin(),
                                       profiles.end(),
                                       NEXT_ITEM_TO_ADD_PROFILE), profiles.end());

            items_OutParam.items_pvec.push_back( item::ItemFactory::Instance()->Make(
                NEXT_ITEM_TO_ADD_PROFILE));

            amount -= NEXT_ITEM_TO_ADD_PROFILE.TreasureScore();
            RemoveTreasureScoresHigherThan(amount, profiles, IS_RELIGIOUS);
        }
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

        std::sort(profiles.begin(),
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

        auto const RAND{ misc::random::Double(weightSum) };

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
