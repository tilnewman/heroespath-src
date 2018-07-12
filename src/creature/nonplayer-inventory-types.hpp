// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_NONPLAYER_INVENTORY_TYPES_HPP_INCLUDED
#define HEROESPATH_CREATURE_NONPLAYER_INVENTORY_TYPES_HPP_INCLUDED
//
// nonplayer-inventory-types.hpp
//  A collection of types that define chances to items that might be
//  owned/carried/worn by non-player characters.
//
#include "creature/trait.hpp"
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"

#include <boost/type_index.hpp>

#include <cstddef> //for std::size_t
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    namespace nonplayer
    {
        // all chances are float ratio values over [0.0f, 1.0f]

        // custom types
        using CountChanceMap_t = misc::VectorMap<std::size_t, float>;
        using MaterialChanceMap_t = misc::VectorMap<item::material::Enum, float>;
        using ArmorTypeChanceMap_t = misc::VectorMap<item::armor::base_type::Enum, float>;

        // helper function that chooses a random value from the map types defined above
        template <typename T>
        T MappedRandomFloatChance(const misc::VectorMap<T, float> & MAP)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (MAP.Empty() == false),
                "creature::nonplayer::MappedRandomFloatChance(T=\""
                    << boost::typeindex::type_id<T>().pretty_name()
                    << "\") called when the map was empty.");

            auto chanceSubTotal{ 0.0f };
            for (auto const & NEXT_MAP_PAIR : MAP)
            {
                chanceSubTotal += NEXT_MAP_PAIR.second;
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((misc::IsRealZero(chanceSubTotal) == false) && (chanceSubTotal > 0.0f)),
                "creature::nonplayer::MappedRandomFloatChance(T=\""
                    << boost::typeindex::type_id<T>().pretty_name() << "\", size=" << MAP.Size()
                    << ") called when the map's chance total is zero or less.");

            auto const RAND{ misc::random::Float(0.0f, chanceSubTotal) };

            auto cumulativeChance{ 0.0f };
            for (auto const & NEXT_MAP_PAIR : MAP)
            {
                cumulativeChance += NEXT_MAP_PAIR.second;
                if (RAND < cumulativeChance)
                {
                    return NEXT_MAP_PAIR.first;
                }
            }

            // it is rare but possible to get here if RAND == chanceSubTotal
            if (misc::IsRealClose(RAND, chanceSubTotal))
            {
                // in these rare cases just pick the first non-zero chance
                for (auto const & NEXT_MAP_PAIR : MAP)
                {
                    if (NEXT_MAP_PAIR.second > 0.0f)
                    {
                        return NEXT_MAP_PAIR.first;
                    }
                }
            }

            // if we get here something is wrong, so log everything

            std::ostringstream ss;
            ss << "creature::nonplayer::MappedRandomFloatChance(T=\""
               << boost::typeindex::type_id<T>().pretty_name()
               << "\") failed to random select.  chanceSubTotal=" << chanceSubTotal
               << ", RAND=" << RAND << ", MAP={";

            for (auto const & NEXT_MAP_PAIR : MAP)
            {
                ss << NEXT_MAP_PAIR.second << ", ";
            }

            ss << "}  ";

            std::size_t i{ 0 };
            for (auto const & NEXT_MAP_PAIR : MAP)
            {
                if (NEXT_MAP_PAIR.second > 0.0f)
                {
                    ss << "picking " << static_cast<int>(NEXT_MAP_PAIR.first)
                       << " with chance=" << NEXT_MAP_PAIR.second << " at index=" << i;

                    M_HP_LOG_WRN(ss.str());
                    return NEXT_MAP_PAIR.first;
                }
                else
                {
                    ++i;
                }
            }

            ss << "-MORE problems: Could not find any chance greater-than zero so raising this "
                  "to an exception and throwing.";

            throw std::runtime_error(ss.str());
        }

        // A wrapper for all information needed to determine if an item is owned, and what
        // composes it.
        struct ItemChances
        {
            explicit ItemChances(
                const CountChanceMap_t & NUM_OWNED_MAP = CountChanceMap_t(),
                const float CHANCE_EQUIPPED = 0.0f,
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            explicit ItemChances(
                const float CHANCE_OWNED,
                const float CHANCE_EQUIPPED = 0.0f,
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            ItemChances(
                const float CHANCE_OWNED,
                const float CHANCE_EQUIPPED,
                const item::material::Enum MATERIAL_PRIMARY,
                const item::material::Enum MATERIAL_SECONDARY);

            static ItemChances NoChance() { return ItemChances(); }

            bool IsEquipped() const { return (misc::random::Float() < chance_equipped); }

            std::size_t CountOwned() const;

            bool IsOwned() const { return (CountOwned() > 0); }

            item::material::Enum RandomMaterialPri() const;
            item::material::Enum RandomMaterialSec() const;

            // make it so there is no chance this item will be owned
            void SetCountChanceSingleNoChance()
            {
                num_owned_map.Clear();
                num_owned_map[0] = 1.0f;
            }

            // set the likelyhood of a certain (1.0f) number of this items owned
            void SetCountChance(const std::size_t COUNT = 1, const float CHANCE = 1.0f)
            {
                num_owned_map[COUNT] = CHANCE;
            }

            // clear all chances except for single and make that single chance certain (1.0f)
            void SetCountChanceSingleCertain();

            // clear all chances except for a single item to be owned,
            // and set that chance to the value provided
            void SetCountChanceSingle(const float CHANCE);

            // find the highest number owned with a non-zero chance,
            // then set the number greater than that to the value provided
            void SetCountChanceIncrement(const float CHANCE = 1.0f);

            // same as SetCountChanceIncrement() but the CHANCE value set is 1.0f
            void SetCountChanceIncrementCertain() { SetCountChanceIncrement(1.0f); }

            // calls SetCountChanceIncrement() and sets chance_equipped
            void SetCountChanceIncrementAndEquip(
                const float CHANCE = 1.0f, const float EQUIP_CHANCE = 1.0f)
            {
                SetCountChanceIncrement(CHANCE);
                chance_equipped = EQUIP_CHANCE;
            }

            // The chance that this item will be equipped.
            // ChanceFactory sets this value for InventoryFactory to determine if the item will
            // be equipped or not, see IsEquipped().
            float chance_equipped;

            // What is the chance that a certain number of this item is owned.
            //(num_owned_map[0]=1.0f is valid)
            // ChanceFactory sets these values so that the InventoryFactory knows
            // how many to give a non-player_character.
            CountChanceMap_t num_owned_map;

            // the primary material composing this item
            MaterialChanceMap_t mat_map_pri;

            // material::Nothing can be added to this map so that there can be a chance of no
            // secondary material
            MaterialChanceMap_t mat_map_sec;
        };

        using ItemChanceMap_t = misc::VectorMap<item::misc_type::Enum, ItemChances>;
        using HelmChanceMap_t = misc::VectorMap<item::armor::helm_type::Enum, ItemChances>;
        using CoverChanceMap_t = misc::VectorMap<item::armor::cover_type::Enum, ItemChances>;
        using ShieldChanceMap_t = misc::VectorMap<item::armor::shield_type::Enum, ItemChances>;
        //
        using AxeChanceMap_t = misc::VectorMap<item::weapon::axe_type::Enum, ItemChances>;
        using ClubChanceMap_t = misc::VectorMap<item::weapon::club_type::Enum, ItemChances>;
        using WhipChanceMap_t = misc::VectorMap<item::weapon::whip_type::Enum, ItemChances>;
        using SwordChanceMap_t = misc::VectorMap<item::weapon::sword_type::Enum, ItemChances>;

        using ProjectileChanceMap_t
            = misc::VectorMap<item::weapon::projectile_type::Enum, ItemChances>;

        using BladedStaffChanceMap_t
            = misc::VectorMap<item::weapon::bladedstaff_type::Enum, ItemChances>;

        // returns the type and count of the item selected in a pair
        // It is possible and valid for the MAP to be empty, or to have only chances for zero
        // counts. It is the caller's responsibility to check for a returned count of zero.
        template <typename T>
        std::pair<T, std::size_t>
            MappedRandomItemChance(const misc::VectorMap<T, ItemChances> & MAP)
        {
            if (MAP.Empty())
            {
                return std::make_pair(T(), 0);
            }

            float chanceSubTotal(0.0f);
            for (auto const & NEXT_MAP_PAIR_OUTER : MAP)
            {
                for (auto const & NEXT_MAP_PAIR_INNER : NEXT_MAP_PAIR_OUTER.second.num_owned_map)
                {
                    if (NEXT_MAP_PAIR_INNER.first > 0)
                    {
                        chanceSubTotal += NEXT_MAP_PAIR_INNER.second;
                    }
                }
            }

            if (misc::IsRealClose(chanceSubTotal, 0.0f))
            {
                return std::make_pair(MAP.begin()->first, 0);
            }

            const float RAND(misc::random::Float(0.0f, chanceSubTotal));

            float cumulativeChance(0.0f);
            for (auto const & NEXT_MAP_PAIR_OUTER : MAP)
            {
                for (auto const & NEXT_MAP_PAIR_INNER : NEXT_MAP_PAIR_OUTER.second.num_owned_map)
                {
                    cumulativeChance += NEXT_MAP_PAIR_INNER.second;
                    if (RAND < cumulativeChance)
                    {
                        return std::make_pair(NEXT_MAP_PAIR_OUTER.first, NEXT_MAP_PAIR_INNER.first);
                    }
                }
            }

            M_HP_LOG(
                "WARNING:  creature::nonplayer::MappedRandomItemChance(T=\""
                << boost::typeindex::type_id<T>().pretty_name()
                << "\") failed random selection.  Choosing first with a count of one by "
                   "default.");

            return std::make_pair(MAP.begin()->first, 1);
        }

        // A wrapper for all the info needed to create a set of clothes for a
        // non-player-character.
        struct ClothingChances
        {
            explicit ClothingChances(
                const float SHIRT = 0.0f,
                const float GLOVES = 0.0f,
                const float PANTS = 0.0f,
                const float BOOTS = 0.0f,
                const float VEST = 0.0f,
                const CoverChanceMap_t & COVER_CHANCE_MAP = CoverChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            static ClothingChances NoClothes() { return ClothingChances(); }

            item::armor::cover_type::Enum RandomCoverType() const;

            ItemChances shirt;
            ItemChances gloves;
            ItemChances pants;
            ItemChances boots;
            ItemChances vest;
            CoverChanceMap_t cover_map; // For robe, cloak, and cape only.  Vests are separate.
        };

        // A wrapper for (almost) all the information needed to determine if a
        // piece of armor is owned and its construction
        struct ArmorItemChances : public ItemChances
        {
            explicit ArmorItemChances(
                const float CHANCE_OWNED = 0.0f,
                const ArmorTypeChanceMap_t & ARMOR_TYPE_CH_MAP = ArmorTypeChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            ArmorItemChances(
                const float CHANCE_OWNED,
                const item::armor::base_type::Enum ARMOR_BASE_TYPE,
                const item::material::Enum MATERIAL_PRIMARY,
                const item::material::Enum MATERIAL_SECONDARY);

            static ArmorItemChances NoArmorChance() { return ArmorItemChances(); }

            item::armor::base_type::Enum RandomArmorBaseType() const
            {
                return MappedRandomFloatChance<item::armor::base_type::Enum>(type_map);
            }

            ArmorTypeChanceMap_t type_map;
        };

        // A wrapper holding all the information needed to determine what
        // armor a non-player-character is wearing
        struct ArmorChances
        {
            explicit ArmorChances(
                const ArmorItemChances & AVENTAIL = ArmorItemChances::NoArmorChance(),
                const ArmorItemChances & SHIRT = ArmorItemChances::NoArmorChance(),
                const ArmorItemChances & BRACERS = ArmorItemChances::NoArmorChance(),
                const ArmorItemChances & GAUNTLETS = ArmorItemChances::NoArmorChance(),
                const ArmorItemChances & PANTS = ArmorItemChances::NoArmorChance(),
                const ArmorItemChances & BOOTS = ArmorItemChances::NoArmorChance(),
                const HelmChanceMap_t & HELM_MAP = HelmChanceMap_t(),
                const CoverChanceMap_t & COVER_MAP = CoverChanceMap_t(),
                const ShieldChanceMap_t & SHIELD_MAP = ShieldChanceMap_t());

            static ArmorChances NoArmor() { return ArmorChances(); }

            const std::pair<item::armor::helm_type::Enum, std::size_t> RandomHelm() const
            {
                return MappedRandomItemChance<item::armor::helm_type::Enum>(helm_map);
            }

            const std::pair<item::armor::cover_type::Enum, std::size_t> RandomCover() const
            {
                return MappedRandomItemChance<item::armor::cover_type::Enum>(cover_map);
            }

            const std::pair<item::armor::shield_type::Enum, std::size_t> RandomShield() const
            {
                return MappedRandomItemChance<item::armor::shield_type::Enum>(shield_map);
            }

            ArmorItemChances aventail;
            ArmorItemChances shirt;
            ArmorItemChances bracers;
            ArmorItemChances gauntlets;
            ArmorItemChances pants;
            ArmorItemChances boots;

            HelmChanceMap_t helm_map;
            CoverChanceMap_t cover_map;
            ShieldChanceMap_t shield_map;
        };

        // A wrapper holding all the information needed to determine what kind of knife a
        // non-player-character has.
        struct KnifeItemChances : public ItemChances
        {
            explicit KnifeItemChances(
                const float CHANCE_OWNED = 0.0f,
                const float CHANCE_EQUIPPED = 0.0f,
                const float IS_DAGGER = 0.0f,
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            static KnifeItemChances NoKnife() { return KnifeItemChances(); }

            float is_dagger;
        };

        // A wrapper holding all the information needed to determine what kind of staff a
        // non-player-character has.
        struct StaffItemChances : public ItemChances
        {
            explicit StaffItemChances(
                const float CHANCE_OWNED = 0.0f,
                const float IS_EQUIPPED = 0.0f,
                const float IS_QUARTERSTAFF = 0.0f,
                const MaterialChanceMap_t & MAT_CH_MAP_PRI = MaterialChanceMap_t(),
                const MaterialChanceMap_t & MAT_CH_MAP_SEC = MaterialChanceMap_t());

            float is_quarterstaff;
        };

        // A wrapper holding all the information needed to determine what weapons a
        // non-player-character owns.
        struct WeaponChances
        {
            explicit WeaponChances(
                const bool HAS_CLAWS = false,
                const bool HAS_BITE = false,
                const bool HAS_FISTS = false,
                const bool HAS_TENTACLES = false,
                const bool HAS_BREATH = false,
                const KnifeItemChances & KNIFE = KnifeItemChances(),
                const StaffItemChances & STAFF = StaffItemChances(),
                const AxeChanceMap_t & AXE_MAP = AxeChanceMap_t(),
                const ClubChanceMap_t & CLUB_MAP = ClubChanceMap_t(),
                const WhipChanceMap_t & WHIP_MAP = WhipChanceMap_t(),
                const SwordChanceMap_t & SWORD_MAP = SwordChanceMap_t(),
                const ProjectileChanceMap_t & PROJECTILE_MAP = ProjectileChanceMap_t(),
                const BladedStaffChanceMap_t & BLADEDSTAFF_MAP = BladedStaffChanceMap_t());

            static WeaponChances NoWeapon() { return WeaponChances(); }

            bool has_claws;
            bool has_bite;
            bool has_fists;
            bool has_tentacles;
            bool has_breath;

            KnifeItemChances knife;
            StaffItemChances staff;

            AxeChanceMap_t axe_map;
            ClubChanceMap_t club_map;
            WhipChanceMap_t whip_map;
            SwordChanceMap_t sword_map;
            ProjectileChanceMap_t projectile_map;
            BladedStaffChanceMap_t bladedstaff_map;
        };

        // A wrapper holding all the information needed to randomly choose a complete
        // inventory set for a non-player-character.
        struct InventoryChances
        {
            explicit InventoryChances(
                const Coin_t & COINS_MIN = 0_coin,
                const Coin_t & COINS_MAX = 0_coin,
                const ClothingChances & CLOTHES_CHANCES = ClothingChances::NoClothes(),
                const WeaponChances & WEAPON_CHANCES = WeaponChances::NoWeapon(),
                const ArmorChances & ARMOR_CHANCES = ArmorChances::NoArmor(),
                const ItemChanceMap_t & MISC_ITEM_CHANCES = ItemChanceMap_t());

            Coin_t RandomCoins() const
            {
                return (
                    (coins_min < coins_max)
                        ? Coin_t(misc::random::Int(coins_min.As<int>(), coins_max.As<int>()))
                        : coins_min);
            }

            Coin_t coins_min;
            Coin_t coins_max;

            ArmorChances armor;
            WeaponChances weapon;
            ClothingChances clothes;
            ItemChanceMap_t misc_items;
        };

        // defines the value of items owned by a creature
        struct wealth_type : public misc::EnumBaseCounting<wealth_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Destitute = 0,
                Poor,
                LowerMiddle,
                UpperMiddle,
                Rich,
                Lavish,
                Royal,
                Count
            };

            static const std::string ToString(const wealth_type::Enum);
            static wealth_type::Enum FromRankType(const rank_class::Enum RANK_CLASS);
            static wealth_type::Enum FromRank(const Rank_t & RANK);
            static wealth_type::Enum FromCreature(const CreaturePtr_t CHARACTER_PTR);
        };

        // defines the number of items a creature owns
        struct collector_type : misc::EnumBaseBitField<collector_type>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                // pure random chance to carry items, should not be used
                None = 0,

                // rarely carries duplicates, usually only carries essential items
                Minimalist = 1 << 0,

                // often carries items that can be used (torche/lantern/healingherbs/etc.)
                Practical = 1 << 1,

                // often carries rare items or items of unique/cool materials
                Collector = 1 << 2,

                // often carries duplicates, especially items that cannot be used
                Hoarder = 1 << 3,

                Last = Hoarder
            };

            static void ToStringPopulate(
                std::ostringstream & ss,
                const misc::EnumUnderlying_t ENUM_VALUE,
                const std::string & SEPARATOR);

            static collector_type::Enum FromCreature(const CreaturePtr_t CHARACTER_PTR);
        };

        // defines the frequency/power of magical items owned by the creature
        struct owns_magic_type
            : public misc::EnumBaseCounting<owns_magic_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Rarely = 0, // rarely carries magic items
                Religious, // often carries religous items and only occasionally magic items
                Magical, // often carries religous and magic items
                Count
            };

            static const std::string ToString(const owns_magic_type::Enum);
            static owns_magic_type::Enum FromCreature(const CreaturePtr_t CHARACTER_PTR);
        };

        // define the complexity of items owned by the creature
        struct complexity_type
            : public misc::EnumBaseCounting<complexity_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                //...nothing
                Animal = 0,

                // shortbow/longbow/blowpipe, rings,
                // leather/stone/bone/obsidian/lapis/horn/hide/tooth materials only
                Simple,

                // composite bow, medallion, scale material, iron/steel/tin metals only
                Moderate,

                // crossbow, lantern, necklas, work with all metals
                Complex,

                Count
            };

            static const std::string ToString(const complexity_type::Enum);
            static complexity_type::Enum FromCreature(const CreaturePtr_t CHARACTER_PTR);
        };

        // wrapper for info that describes what a creature will own
        struct Profile
        {
            Profile(
                const wealth_type::Enum WEALTH_TYPE,
                const collector_type::Enum COLLECTOR_TYPE,
                const owns_magic_type::Enum MAGIC_OWN_TYPE,
                const complexity_type::Enum COMPLEXITY_TYPE);

            static const Profile Make_FromCreature(const CreaturePtr_t CHARACTER_PTR);

            wealth_type::Enum wealthType;
            collector_type::Enum collectorType;
            owns_magic_type::Enum magicType;
            complexity_type::Enum complexityType;
        };

        float ConvertStringToFloat(const std::string & KEY, const std::string & STR_FLOAT);

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_NONPLAYER_CHANCE_TYPES_HPP_INCLUDED
