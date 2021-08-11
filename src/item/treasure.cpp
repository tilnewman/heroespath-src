// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure.cpp
//
#include "treasure.hpp"

#include "combat/encounter.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature.hpp"
#include "creature/nonplayer-inventory-types.hpp"
#include "item/item-cache.hpp"
#include "item/item-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <algorithm>

namespace heroespath
{
namespace item
{

    SetTypeProfile::SetTypeProfile(const Item & ITEM)
        : set(ITEM.SetType())
        , weapon(ITEM.WeaponType())
        , armor(ITEM.ArmorType())
        , misc(ITEM.MiscType())
    {}

    bool SetTypeProfile::DoesSetMatch(const Item & ITEM) const
    {
        return (
            (ITEM.SetType() == set) && (ITEM.WeaponType() == weapon) && (ITEM.ArmorType() == armor)
            && (ITEM.MiscType() == misc));
    }

    TreasureImage::Enum TreasureFactory::Make(
        const creature::CreaturePVec_t & CHARACTER_PVEC, ItemCache & itemCache_OutParam) const
    {
        const auto TREASURE_SCORES { CalculateTreasureSums(CHARACTER_PVEC) };

        itemCache_OutParam.coins = Coin_t::Make(TREASURE_SCORES.Coin());
        itemCache_OutParam.gems = Gem_t::Make(TREASURE_SCORES.Gem());

        SelectItems(TREASURE_SCORES.Magic(), false, itemCache_OutParam);
        SelectItems(TREASURE_SCORES.Religious(), true, itemCache_OutParam);

        if (itemCache_OutParam.items_pvec.empty()
            && ((TREASURE_SCORES.Magic() > 0_score) || (TREASURE_SCORES.Religious() > 0_score)))
        {
            ForceItemSelection(itemCache_OutParam);
        }

        return DetermineWhichTreasureImage(TREASURE_SCORES);
    }

    const TreasureScores TreasureFactory::CalculateTreasureSums(
        const creature::CreaturePVec_t & CHARACTER_PVEC) const
    {
        TreasureScores scores;

        for (const auto & NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            if (creature::nonplayer::complexity_type::FromCreature(NEXT_CHARACTER_PTR)
                != creature::nonplayer::complexity_type::Animal)
            {
                scores += MakeRandTreasureInfo(CHARACTER_PVEC);
            }
        }

        return scores;
    }

    TreasureImage::Enum
        TreasureFactory::DetermineWhichTreasureImage(const TreasureScores & TREASURE_SCORES) const
    {
        if (TREASURE_SCORES.IsEmpty())
        {
            return TreasureImage::BonePile;
        }
        else
        {
            const auto LOCKBOX_COIN_SUM_MAX { Score_t::Make(
                misc::ConfigFile::Instance()->ValueOrDefault<int>(
                    "treasure-lockbox-coin-max")) };

            return (
                (TREASURE_SCORES.Coin() > LOCKBOX_COIN_SUM_MAX) ? TreasureImage::ChestClosed
                                                                : TreasureImage::LockboxClosed);
        }
    }

    float TreasureFactory::TreasureRatioPer(const creature::CreaturePVec_t & CHARACTER_PVEC) const
    {
        TreasureScores scores;

        for (const auto & NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            scores += creature::race::TreasureScore(
                NEXT_CHARACTER_PTR->Race(), NEXT_CHARACTER_PTR->Role());
        }

        const auto COIN_AVG { scores.Coin().As<float>()
                              / static_cast<float>(CHARACTER_PVEC.size()) };

        const auto GEM_AVG { scores.Gem().As<float>() / static_cast<float>(CHARACTER_PVEC.size()) };

        const auto MAGIC_AVG { scores.Magic().As<float>()
                               / static_cast<float>(CHARACTER_PVEC.size()) };

        const auto RELIGIOUS_AVG { scores.Religious().As<float>()
                                   / static_cast<float>(CHARACTER_PVEC.size()) };

        const auto T_SCORES_MAX { creature::race::TreasureScoreMax() };

        const auto COIN_RATIO { COIN_AVG / T_SCORES_MAX.Coin().As<float>() };
        const auto GEM_RATIO { GEM_AVG / T_SCORES_MAX.Gem().As<float>() };
        const auto MAGIC_RATIO { MAGIC_AVG / T_SCORES_MAX.Magic().As<float>() };
        const auto RELIGIOUS_RATIO { RELIGIOUS_AVG / T_SCORES_MAX.Religious().As<float>() };

        return (COIN_RATIO + GEM_RATIO + MAGIC_RATIO + RELIGIOUS_RATIO) / 4.0f;
    }

    const TreasureScores
        TreasureFactory::MakeRandTreasureInfo(const creature::CreaturePVec_t & CHARACTER_PVEC) const
    {
        TreasureScores scores;

        for (const auto & NEXT_CHARACTER_PTR : CHARACTER_PVEC)
        {
            scores += creature::race::TreasureScore(
                NEXT_CHARACTER_PTR->Race(), NEXT_CHARACTER_PTR->Role());
        }

        const auto COIN_BASE { static_cast<int>(
            scores.Coin().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-coin-base")) };

        const auto COIN_RAND_BASE { static_cast<int>(
            scores.Coin().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-coin-mult")) };

        const auto COIN { Score_t::Make(COIN_BASE + misc::Random(COIN_RAND_BASE)) };

        const auto GEM_BASE { static_cast<int>(
            scores.Gem().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-gem-base")) };

        const auto GEM_RAND_BASE { static_cast<int>(
            scores.Gem().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-gem-mult")) };

        const auto GEM { Score_t::Make(GEM_BASE + misc::Random(GEM_RAND_BASE)) };

        const auto MAGIC_BASE { static_cast<creature::Trait_t>(
            scores.Magic().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-magic-base")) };

        const auto MAGIC_RAND_BASE { static_cast<creature::Trait_t>(
            scores.Magic().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-magic-mult")) };

        const auto MAGIC { Score_t::Make(MAGIC_BASE + misc::Random(MAGIC_RAND_BASE)) };

        const auto RELIGIOUS_BASE { static_cast<creature::Trait_t>(
            scores.Religious().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-religious-base")) };

        const auto RELIGIOUS_RAND_BASE { static_cast<creature::Trait_t>(
            scores.Religious().As<float>()
            * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "treasure-religious-mult")) };

        const auto RELIGIOUS { Score_t::Make(
            RELIGIOUS_BASE + misc::Random(RELIGIOUS_RAND_BASE)) };

        return TreasureScores(COIN, GEM, MAGIC, RELIGIOUS);
    }

    std::size_t TreasureFactory::SelectItems(
        const Score_t & TREASURE_SCORE,
        const bool IS_RELIGIOUS,
        ItemCache & itemCache_OutParam) const
    {
        // this function assumes that ItemProfileWarehouse::Instance()->Get() returns a vector
        // sorted by TreasureScore()

        if (TREASURE_SCORE.IsZero())
        {
            return 0;
        }

        // This 'amount' variable works both as an amount of treasure still to be acquired and
        // as a measure of how rare items can possibly be
        auto amount { TREASURE_SCORE };

        // this is the vector of ALL ItemProfiles that can be selected
        const auto & PROFILES { (
            (IS_RELIGIOUS) ? ItemProfileWarehouse::Instance()->GetReligiousProfiles()
                           : ItemProfileWarehouse::Instance()->GetNormalProfiles()) };

        const auto OWNED_SET_PROFILES { SetItemsAlreadyOwned() };

        // this temp vector contains the indexes into PROFILES along with that profile's
        // corresponding treasure score weight.  This vector defines which items are
        // possible/available for random selection.
        RandomSelectionVec_t possibleVec;
        possibleVec.reserve(
            PROFILES.size() / 4); // this value found by experiment to be a good guess

        auto weightSum { PopuplatePossibleVectorAndReturnWeightSum(
            possibleVec, PROFILES, amount, IS_RELIGIOUS, OWNED_SET_PROFILES) };

        if (possibleVec.empty())
        {
            return 0;
        }

        // This is an index of possibleVec that defines its useable size.  Rather than repeatedly
        // populating possibleVec, this index allows it to be shrunk instead.  This also means that
        // weightSum is not the sum of all in possibleVec but instead the sum from possibleVec[0] to
        // possibleVec[possibleVecLimitIndex].
        std::size_t possibleVecLimitIndex { possibleVec.size() - 1 };

        // this vector keeps track of which PROFILES have already been randomly selected
        std::vector<std::size_t> selectedIndexVec;
        selectedIndexVec.reserve(64); // found by experiment to be a good upper bound

        while ((possibleVec.empty() == false) && (possibleVecLimitIndex > 0))
        {
            auto & randomSelection {
                possibleVec[SelectRandomWeighted(weightSum, possibleVec, possibleVecLimitIndex)]
            };

            if (randomSelection.score > 0_score)
            {
                amount -= randomSelection.score;
                weightSum -= TreasureScoreToWeight(randomSelection.score);

                // this is how we mark possibilities as already selected so they are not
                // selected multiple times (prevents duplicates)
                randomSelection.score = 0_score;

                selectedIndexVec.emplace_back(randomSelection.index);
            }

            // no more than five religious items will be allowed
            if (IS_RELIGIOUS && (selectedIndexVec.size() > 5))
            {
                break;
            }
            else
            {
                FindNewPossibleVectorLimitAndUpdateWeightSum(
                    possibleVec, amount, possibleVecLimitIndex, weightSum);
            }
        }

        ItemFactory itemFactory;
        for (const auto PROFILE_INDEX : selectedIndexVec)
        {
            itemCache_OutParam.items_pvec.emplace_back(itemFactory.Make(PROFILES[PROFILE_INDEX]));
        }

        return selectedIndexVec.size();
    }

    void TreasureFactory::ForceItemSelection(ItemCache & itemCache_OutParam) const
    {
        if (fallbackItemProfiles_.empty())
        {
            PopulateFallbackItemProfiles();
        }

        ItemFactory itemFactory;

        const auto ITEM_PTR { itemFactory.Make(misc::RandomSelect(fallbackItemProfiles_)) };

        itemCache_OutParam.items_pvec.emplace_back(ITEM_PTR);
    }

    std::size_t TreasureFactory::SelectRandomWeighted(
        const double WEIGHT_SUM,
        const RandomSelectionVec_t & POSSIBLE_SELECTIONS,
        const std::size_t INDEX_LIMIT) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (POSSIBLE_SELECTIONS.empty() == false),
            "combat::TreasureFactory::SelectRandomWeighted() was given an empty vector.");

        const auto RAND_WEIGHT { misc::Random(WEIGHT_SUM) };

        double runningWeightSum { 0.0 };
        for (std::size_t i(0); i <= INDEX_LIMIT; ++i)
        {
            const auto SELECTION_SCORE { POSSIBLE_SELECTIONS[i].score };

            // skip possibilities with a score of zero, because that is how possibilities are marked
            // as already selected (how duplicates are prevented)
            if (0_score != SELECTION_SCORE)
            {
                runningWeightSum += TreasureScoreToWeight(SELECTION_SCORE);
                if (RAND_WEIGHT < runningWeightSum)
                {
                    return i;
                }
            }
        }

        return INDEX_LIMIT;
    }

    const SetTypeProfileVec_t TreasureFactory::SetItemsAlreadyOwned() const
    {
        SetTypeProfileVec_t ownedSetProfiles;
        ownedSetProfiles.reserve(128); // found by experiment to be a good upper bound

        // populate setItemsOwnedProfiles with thin profiles of equipped and unequipped items
        const auto CREATURE_PVEC { creature::Algorithms::Players(creature::Algorithms::Runaway) };

        for (const auto & CREATURE_PTR : CREATURE_PVEC)
        {
            for (const auto & UNEQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().Items())
            {
                const auto SET_TYPE_POFILE { SetTypeProfile(*UNEQUIPPED_ITEM_PTR) };
                if (SET_TYPE_POFILE.DoesBelongToASet())
                {
                    ownedSetProfiles.emplace_back(SET_TYPE_POFILE);
                }
            }

            for (const auto & EQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().ItemsEquipped())
            {
                const auto SET_TYPE_POFILE { SetTypeProfile(*EQUIPPED_ITEM_PTR) };
                if (SET_TYPE_POFILE.DoesBelongToASet())
                {
                    ownedSetProfiles.emplace_back(SET_TYPE_POFILE);
                }
            }
        }

        return ownedSetProfiles;
    }

    double TreasureFactory::PopuplatePossibleVectorAndReturnWeightSum(
        RandomSelectionVec_t & possibleVec,
        const ItemProfileVec_t & PROFILES,
        const Score_t & MAX_SCORE,
        const bool IS_RELIGIOUS,
        const SetTypeProfileVec_t & OWNED_SET_PROFILES) const
    {
        possibleVec.clear();

        auto weightSum { 0.0 };

        const auto NUM_PROFILES { PROFILES.size() };
        for (std::size_t i(0); i < NUM_PROFILES; ++i)
        {
            const auto & PROFILE { PROFILES[i] };

            const auto SCORE { (
                (IS_RELIGIOUS) ? PROFILE.ReligiousScore() : PROFILE.TreasureScore()) };

            if (SCORE > MAX_SCORE)
            {
                break;
            }

            if (PROFILE.IsQuestItem())
            {
                continue;
            }

            // skip if the party already owns this item in this set
            if (std::find_if(
                    std::begin(OWNED_SET_PROFILES),
                    std::end(OWNED_SET_PROFILES),
                    [&](const auto & SET_TYPE_PROFILE) {
                        return SET_TYPE_PROFILE.DoesSetMatch(PROFILE);
                    })
                != std::end(OWNED_SET_PROFILES))
            {
                continue;
            }

            if (SCORE > 0_score)
            {
                possibleVec.emplace_back(RandomSelection(i, SCORE));
                weightSum += TreasureScoreToWeight(SCORE);
            }
        }

        return weightSum;
    }

    void TreasureFactory::FindNewPossibleVectorLimitAndUpdateWeightSum(
        RandomSelectionVec_t & possibleVec,
        const Score_t & MAX_SCORE,
        std::size_t & possibleIndexLimit,
        double & weightSum) const
    {
        for (;; --possibleIndexLimit)
        {
            const auto POSSIBLE_SELECTION_SCORE { possibleVec[possibleIndexLimit].score };

            // selections with a score of zero have already been randomly selected and the weightSum
            // already adjusted so skip them
            if (POSSIBLE_SELECTION_SCORE != 0_score)
            {
                if (POSSIBLE_SELECTION_SCORE < MAX_SCORE)
                {
                    break;
                }
                else
                {
                    weightSum -= TreasureScoreToWeight(POSSIBLE_SELECTION_SCORE);
                }
            }

            if (0 == possibleIndexLimit)
            {
                break;
            }
        }
    }

    void TreasureFactory::PopulateFallbackItemProfiles() const
    {
        fallbackItemProfiles_.reserve(745); // this was the size measured on 2018-3-19

        const auto & PROFILES { ItemProfileWarehouse::Instance()->GetNormalProfiles() };

        const auto MAX_SCORE { PROFILES[0].TreasureScore() * 2_score };

        for (const auto & PROFILE : PROFILES)
        {
            if (PROFILE.TreasureScore() > MAX_SCORE)
            {
                break;
            }
            else
            {
                fallbackItemProfiles_.emplace_back(PROFILE);
            }
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (fallbackItemProfiles_.empty() == false),
            "item::TreasureFactory::PopulateFallbackItemProfiles() failed to create any fallback "
            "profiles.");
    }

} // namespace item
} // namespace heroespath
