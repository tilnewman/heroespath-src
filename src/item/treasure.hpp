// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_TREASURE_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASURE_HPP_INCLUDED
//
// treasure.hpp
//
#include "game/strong-types.hpp"
#include "item/armor-enum.hpp"
#include "item/misc-enum.hpp"
#include "item/set-enum.hpp"
#include "item/treasure-image-enum.hpp"
#include "item/treasure-score-set.hpp"
#include "item/weapon-enum.hpp"
#include "misc/not-null.hpp"

#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace item
{

    class Item;
    class ItemProfile;
    struct ItemCache;

    using ItemProfileVec_t = std::vector<ItemProfile>;

    // Responsible for wrapping all the information about an Item's Set type, and for providing
    // functions to compare that type against an Item or an ItemProfile.
    class SetTypeProfile
    {
    public:
        SetTypeProfile() = default;
        explicit SetTypeProfile(const Item & ITEM);
        explicit SetTypeProfile(const ItemProfile & ITEM_PROFILE);

        bool DoesSetMatch(const Item & ITEM) const;
        bool DoesSetMatch(const ItemProfile & ITEM_PROFILE) const;
        inline bool DoesBelongToASet() const { return (set < Set::Count); }

    private:
        Set::Enum set = Set::Count;
        Weapon::Enum weapon = Weapon::Count;
        Armor::Enum armor = Armor::Count;
        Misc::Enum misc = Misc::Count;
    };

    using SetTypeProfileVec_t = std::vector<SetTypeProfile>;

    // Responsible for wrapping all the details about an ItemProfile in consideration for random
    // selection
    struct RandomSelection
    {
        explicit RandomSelection(const std::size_t INDEX = 0, const Score_t & SCORE = 0_score)
            : score(SCORE)
            , index(INDEX)
        {}

        Score_t score;
        std::size_t index;
    };

    using RandomSelectionVec_t = std::vector<RandomSelection>;

    // Responsible for deciding what treasure a given creatures has.
    class TreasureFactory
    {
    public:
        TreasureFactory() = default;

        TreasureFactory(const TreasureFactory &) = delete;
        TreasureFactory(TreasureFactory &&) = delete;
        TreasureFactory & operator=(const TreasureFactory &) = delete;
        TreasureFactory & operator=(TreasureFactory &&) = delete;

        TreasureImage::Enum
            Make(const creature::CreaturePVec_t & CHARACTER_PVEC, ItemCache & items_OutParam) const;

        float TreasureRatioPer(const creature::CreaturePVec_t &) const;

    private:
        TreasureImage::Enum DetermineWhichTreasureImage(const TreasureScoreSet &) const;

        // uses a TreasureInfo object to hold/return sum values
        const TreasureScoreSet CalculateTreasureSums(const creature::CreaturePVec_t &) const;

        const TreasureScoreSet MakeRandTreasureInfo(const creature::CreaturePVec_t &) const;

        std::size_t SelectItems(
            const Score_t & TREASURE_SCORE,
            const bool IS_RELIGIOUS,
            ItemCache & items_OutParam) const;

        void SelectRandomCheapFallbackItem(ItemCache & items_OutParam) const;

        std::size_t SelectRandomWeighted(
            const double WEIGHT_SUM,
            const RandomSelectionVec_t &,
            const std::size_t INDEX_LIMIT) const;

        double TreasureScoreSetToWeight(const Score_t & SCORE) const
        {
            return 1.0 / (SCORE.GetAs<double>() * 0.1);
        }

        const SetTypeProfileVec_t SetItemsAlreadyOwned() const;

        double PopuplatePossibleVectorAndReturnWeightSum(
            RandomSelectionVec_t & possibleVec,
            const ItemProfileVec_t & PROFILES,
            const Score_t & MAX_SCORE,
            const bool IS_RELIGIOUS,
            const SetTypeProfileVec_t & OWNED_SET_PROFILES) const;

        void FindNewPossibleVectorLimitAndUpdateWeightSum(
            RandomSelectionVec_t & possibleVec,
            const Score_t & MAX_SCORE,
            std::size_t & possibleIndexLimit,
            double & weightSum) const;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASURE_HPP_INCLUDED
