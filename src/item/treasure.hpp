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
//  Functions that determine what treasure a defeated enemy party has.
//
#include "item/item-profile.hpp"
#include "item/treasure-image-enum.hpp"
#include "item/treasure-scores.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

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
    struct ItemCache;

    // Responsible for wrapping all the information about an Item's Set type, and for providing
    // functions to compare that type against an Item or an ItemProfile.
    class SetTypeProfile
    {
    public:
        explicit SetTypeProfile(const ItemProfile & ITEM_PROFILE = ItemProfile())
            : set(ITEM_PROFILE.SetType())
            , weapon(ITEM_PROFILE.WeaponType())
            , armor(ITEM_PROFILE.ArmorType())
            , misc(ITEM_PROFILE.MiscType())
        {}

        explicit SetTypeProfile(const Item & ITEM);

        bool DoesBelongToASet() const
        {
            return ((set_type::Count != set) && (set_type::Not != set));
        }

        bool DoesSetMatch(const ItemProfile & ITEM_PROFILE) const
        {
            return (
                (ITEM_PROFILE.SetType() == set) && (ITEM_PROFILE.WeaponType() == weapon)
                && (ITEM_PROFILE.ArmorType() == armor) && (ITEM_PROFILE.MiscType() == misc));
        }

        bool DoesSetMatch(const Item & ITEM) const;

    private:
        set_type::Enum set;
        weapon_type::Enum weapon;
        armor_type::Enum armor;
        misc_type::Enum misc;
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

    struct TreasureFactory
    {
        // Decides what treasure the given creatures have and adds it
        // to items_OutParam.  It is assumed, but not required, that
        // items_OutParam is empty when Make is called.
        static TreasureImage::Enum
            Make(const creature::CreaturePVec_t & CHARACTER_PVEC, ItemCache & items_OutParam);

        static float TreasureRatioPer(const creature::CreaturePVec_t &);

    private:
        static TreasureImage::Enum DetermineWhichTreasureImage(const TreasureScores &);

        // uses a TreasureInfo object to hold/return sum values
        static const TreasureScores CalculateTreasureSums(const creature::CreaturePVec_t &);

        static const TreasureScores MakeRandTreasureInfo(const creature::CreaturePVec_t &);

        static std::size_t SelectItems(
            const Score_t & TREASURE_SCORE, const bool IS_RELIGIOUS, ItemCache & items_OutParam);

        static void ForceItemSelection(ItemCache & items_OutParam);

        static std::size_t SelectRandomWeighted(
            const double WEIGHT_SUM, const RandomSelectionVec_t &, const std::size_t INDEX_LIMIT);

        static double TreasureScoreToWeight(const Score_t & SCORE)
        {
            return 1.0 / (SCORE.As<double>() * 0.1);
        }

        static const SetTypeProfileVec_t SetItemsAlreadyOwned();

        static double PopuplatePossibleVectorAndReturnWeightSum(
            RandomSelectionVec_t & possibleVec,
            const ItemProfileVec_t & PROFILES,
            const Score_t & MAX_SCORE,
            const bool IS_RELIGIOUS,
            const SetTypeProfileVec_t & OWNED_SET_PROFILES);

        static void FindNewPossibleVectorLimitAndUpdateWeightSum(
            RandomSelectionVec_t & possibleVec,
            const Score_t & MAX_SCORE,
            std::size_t & possibleIndexLimit,
            double & weightSum);

        static void PopulateFallbackItemProfiles();

        static ItemProfileVec_t fallbackItemProfiles_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASURE_HPP_INCLUDED
