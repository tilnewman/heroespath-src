// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// nonplayer-inventory-types.cpp
//
#include "nonplayer-inventory-types.hpp"

#include "creature/creature.hpp"
#include "misc/config-file.hpp"

namespace heroespath
{
namespace creature
{
    namespace nonplayer
    {

        ItemChances::ItemChances(
            const CountChanceMap_t & NUM_OWNED_MAP,
            const float CHANCE_EQUIPPED,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : chance_equipped(CHANCE_EQUIPPED)
            , num_owned_map(NUM_OWNED_MAP)
            , mat_map_pri(MAT_CH_MAP_PRI)
            , mat_map_sec(MAT_CH_MAP_SEC)
        {
            if (num_owned_map.Empty())
            {
                num_owned_map[0] = 1.0f;
            }
        }

        ItemChances::ItemChances(
            const float CHANCE_OWNED,
            const float CHANCE_EQUIPPED,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : chance_equipped(CHANCE_EQUIPPED)
            , num_owned_map()
            , mat_map_pri(MAT_CH_MAP_PRI)
            , mat_map_sec(MAT_CH_MAP_SEC)
        {
            SetCountChanceSingle(CHANCE_OWNED);
        }

        ItemChances::ItemChances(
            const float CHANCE_OWNED,
            const float CHANCE_EQUIPPED,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY)
            : chance_equipped(CHANCE_EQUIPPED)
            , num_owned_map()
            , mat_map_pri()
            , mat_map_sec()
        {
            SetCountChanceSingle(CHANCE_OWNED);
            mat_map_pri[MATERIAL_PRIMARY] = 1.0f;
            mat_map_sec[MATERIAL_SECONDARY] = 1.0f;
        }

        std::size_t ItemChances::CountOwned() const
        {
            if (num_owned_map.Empty())
            {
                return 0;
            }
            else
            {
                return MappedRandomFloatChance<std::size_t>(num_owned_map);
            }
        }

        item::material::Enum ItemChances::RandomMaterialPri() const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (mat_map_pri.Empty() == false),
                "creature::nonplayer::ItemChances::RandomMaterialPri() "
                    << "called when mat_map_pri is empty.");

            return MappedRandomFloatChance<item::material::Enum>(mat_map_pri);
        }

        item::material::Enum ItemChances::RandomMaterialSec() const
        {
            if (mat_map_sec.Empty())
            {
                return item::material::Nothing;
            }
            else
            {
                return MappedRandomFloatChance<item::material::Enum>(mat_map_sec);
            }
        }

        void ItemChances::SetCountChanceSingleCertain()
        {
            num_owned_map.Clear();
            SetCountChanceSingle(1.0f);
        }

        void ItemChances::SetCountChanceSingle(const float CHANCE)
        {
            num_owned_map.Clear();
            SetCountChance(1, CHANCE);
            SetCountChance(0, 1.0f - CHANCE);
        }

        void ItemChances::SetCountChanceIncrement(const float CHANCE)
        {
            const std::size_t MAX_ITERATIONS(1000);
            std::size_t i(0);
            while (i < MAX_ITERATIONS)
            {
                if (misc::IsRealClose(num_owned_map[++i], 0.0f))
                {
                    num_owned_map[i] = 1.0f;
                    break;
                }
            };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (i < MAX_ITERATIONS),
                "creature::nonplayer::ItemChances::SetCountChanceIncrement("
                    << CHANCE << ") reached the sentinel of " << MAX_ITERATIONS
                    << " iterations.  Something is very wrong...");
        }

        ClothingChances::ClothingChances(
            const float SHIRT,
            const float GLOVES,
            const float PANTS,
            const float BOOTS,
            const float VEST,
            const CoverChanceMap_t & COVER_CHANCE_MAP,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : shirt(SHIRT, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , gloves(GLOVES, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , pants(PANTS, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , boots(BOOTS, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , vest(VEST, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , cover_map(COVER_CHANCE_MAP)
        {}

        item::armor::cover_type::Enum ClothingChances::RandomCoverType() const
        {
            misc::VectorMap<item::armor::cover_type::Enum, float> coverChanceMap;

            for (const auto & NEXT_CHANCE_PAIR : cover_map)
            {
                if (NEXT_CHANCE_PAIR.second.IsOwned())
                {
                    coverChanceMap[NEXT_CHANCE_PAIR.first] = misc::random::Float();
                }
            }

            if (coverChanceMap.Size() == 1)
            {
                return coverChanceMap.begin()->first;
            }
            else if (coverChanceMap.Size() > 1)
            {
                auto highestChance { 0.0f };
                auto highestEnum { coverChanceMap.begin()->first };
                for (const auto & NEXT_CHANCE_PAIR : coverChanceMap)
                {
                    if (NEXT_CHANCE_PAIR.second > highestChance)
                    {
                        highestEnum = NEXT_CHANCE_PAIR.first;
                    }
                }

                return highestEnum;
            }
            else
            {
                return item::armor::cover_type::Count;
            }
        }

        ArmorItemChances::ArmorItemChances(
            const float CHANCE_OWNED,
            const ArmorTypeChanceMap_t & ARMOR_TYPE_CH_MAP,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : ItemChances(CHANCE_OWNED, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , type_map(ARMOR_TYPE_CH_MAP)
        {}

        ArmorItemChances::ArmorItemChances(
            const float CHANCE_OWNED,
            const item::armor::base_type::Enum ARMOR_BASE_TYPE,
            const item::material::Enum MATERIAL_PRIMARY,
            const item::material::Enum MATERIAL_SECONDARY)
            : ItemChances(CHANCE_OWNED, 1.0f, MATERIAL_PRIMARY, MATERIAL_SECONDARY)
            , type_map()
        {
            type_map[ARMOR_BASE_TYPE] = 1.0f;
        }

        ArmorChances::ArmorChances(
            const ArmorItemChances & AVENTAIL,
            const ArmorItemChances & SHIRT,
            const ArmorItemChances & BRACERS,
            const ArmorItemChances & GAUNTLETS,
            const ArmorItemChances & PANTS,
            const ArmorItemChances & BOOTS,
            const HelmChanceMap_t & HELM_MAP,
            const CoverChanceMap_t & COVER_MAP,
            const ShieldChanceMap_t & SHIELD_MAP)
            : aventail(AVENTAIL)
            , shirt(SHIRT)
            , bracers(BRACERS)
            , gauntlets(GAUNTLETS)
            , pants(PANTS)
            , boots(BOOTS)
            , helm_map(HELM_MAP)
            , cover_map(COVER_MAP)
            , shield_map(SHIELD_MAP)
        {}

        KnifeItemChances::KnifeItemChances(
            const float CHANCE_OWNED,
            const float IS_EQUIPPED,
            const float IS_DAGGER,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : ItemChances(CHANCE_OWNED, IS_EQUIPPED, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , is_dagger(IS_DAGGER)
        {}

        StaffItemChances::StaffItemChances(
            const float CHANCE_OWNED,
            const float IS_EQUIPPED,
            const float IS_QUARTERSTAFF,
            const MaterialChanceMap_t & MAT_CH_MAP_PRI,
            const MaterialChanceMap_t & MAT_CH_MAP_SEC)
            : ItemChances(CHANCE_OWNED, IS_EQUIPPED, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC)
            , is_quarterstaff(IS_QUARTERSTAFF)
        {}

        InventoryChances::InventoryChances(
            const Coin_t & COINS_MIN,
            const Coin_t & COINS_MAX,
            const ClothingChances & CLOTHES_CHANCES,
            const WeaponChances & WEAPON_CHANCES,
            const ArmorChances & ARMOR_CHANCES,
            const ItemChanceMap_t & MISC_ITEM_CHANCES)
            : coins_min(COINS_MIN)
            , coins_max(COINS_MAX)
            , armor(ARMOR_CHANCES)
            , weapon(WEAPON_CHANCES)
            , clothes(CLOTHES_CHANCES)
            , misc_items(MISC_ITEM_CHANCES)
        {}

        WeaponChances::WeaponChances(
            const bool HAS_CLAWS,
            const bool HAS_BITE,
            const bool HAS_FISTS,
            const bool HAS_TENTACLES,
            const bool HAS_BREATH,
            const KnifeItemChances & KNIFE,
            const StaffItemChances & STAFF,
            const AxeChanceMap_t & AXE_MAP,
            const ClubChanceMap_t & CLUB_MAP,
            const WhipChanceMap_t & WHIP_MAP,
            const SwordChanceMap_t & SWORD_MAP,
            const ProjectileChanceMap_t & PROJECTILE_MAP,
            const BladedStaffChanceMap_t & BLADEDSTAFF_MAP)
            : has_claws(HAS_CLAWS)
            , has_bite(HAS_BITE)
            , has_fists(HAS_FISTS)
            , has_tentacles(HAS_TENTACLES)
            , has_breath(HAS_BREATH)
            , knife(KNIFE)
            , staff(STAFF)
            , axe_map(AXE_MAP)
            , club_map(CLUB_MAP)
            , whip_map(WHIP_MAP)
            , sword_map(SWORD_MAP)
            , projectile_map(PROJECTILE_MAP)
            , bladedstaff_map(BLADEDSTAFF_MAP)
        {}

        const std::string wealth_type::ToString(const wealth_type::Enum WEALTH_TYPE)
        {
            switch (WEALTH_TYPE)
            {
                case Destitute:
                {
                    return "Destitute";
                }
                case Poor:
                {
                    return "Poor";
                }
                case LowerMiddle:
                {
                    return "LowerMiddle";
                }
                case UpperMiddle:
                {
                    return "UpperMiddle";
                }
                case Rich:
                {
                    return "Rich";
                }
                case Lavish:
                {
                    return "Lavish";
                }
                case Royal:
                {
                    return "Royal";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(WEALTH_TYPE));
                    return "";
                }
            }
        }

        wealth_type::Enum wealth_type::FromRankType(const rank_class::Enum RANK_CLASS)
        {
            const auto RANK_CLASS_STR { rank_class::ToString(RANK_CLASS) };

            misc::VectorMap<wealth_type::Enum, float> wealthChanceMap;

            auto wealthTypeChanceRemaining { wealth_type::Count };

            for (std::size_t i(0); i < wealth_type::Count; ++i)
            {
                const auto NEXT_WEALTH_TYPE { static_cast<wealth_type::Enum>(i) };
                const auto NEXT_WEALTH_TYPE_NAME { wealth_type::ToString(NEXT_WEALTH_TYPE) };

                std::ostringstream ss;
                ss << "heroespath-wealthtype-chance-" << RANK_CLASS_STR << "-"
                   << NEXT_WEALTH_TYPE_NAME << "-one-in";

                const auto NEXT_VALUE_STR { misc::ConfigFile::Instance()->Value(ss.str()) };

                if (NEXT_VALUE_STR == "remaining")
                {
                    wealthTypeChanceRemaining = NEXT_WEALTH_TYPE;
                }

                auto nextChanceValue { 0.0f };
                try
                {
                    nextChanceValue = boost::lexical_cast<float>(NEXT_VALUE_STR);
                }
                catch (...)
                {
                    nextChanceValue = -1.0f;
                }

                wealthChanceMap[NEXT_WEALTH_TYPE]
                    = ((nextChanceValue > 0.0f) ? (1.0f / nextChanceValue) : (0.0f));
            }

            auto cumulative { 0.0f };
            const auto RAND { misc::random::Float() };
            for (const auto & NEXT_TYPECHANCE_PAIR : wealthChanceMap)
            {
                if (NEXT_TYPECHANCE_PAIR.first != wealthTypeChanceRemaining)
                {
                    cumulative += NEXT_TYPECHANCE_PAIR.second;

                    if (RAND < cumulative)
                    {
                        return NEXT_TYPECHANCE_PAIR.first;
                    }
                }
            }

            return wealthTypeChanceRemaining;
        }

        wealth_type::Enum wealth_type::FromRank(const Rank_t & RANK)
        {
            return FromRankType(rank_class::FromRank(RANK));
        }

        wealth_type::Enum wealth_type::FromCreature(const CreaturePtr_t CHARACTER_PTR)
        {
            return FromRank(CHARACTER_PTR->Rank());
        }

        const std::string collector_type::ToStringPopulate(
            const misc::EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
        {
            std::string str;

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, collector_type::Minimalist, "Minimalist", SEPARATOR);

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, collector_type::Practical, "Practical", SEPARATOR);

            AppendNameIfBitIsSet(
                str, ENUM_VALUE, collector_type::Collector, "Collector", SEPARATOR);

            AppendNameIfBitIsSet(str, ENUM_VALUE, collector_type::Hoarder, "Hoarder", SEPARATOR);
            return str;
        }

        collector_type::Enum collector_type::FromCreature(const CreaturePtr_t CHARACTER_PTR)
        {
            const auto CHANCE_BASE(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-base"));

            // adjust for race
            const auto RACE_STR { race::ToString(CHARACTER_PTR->Race()) };

            const auto RACE_KEY {
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-race-"
                + RACE_STR
            };

            const auto RACE_COLLECTOR_PARTS_STR { misc::ConfigFile::Instance()->Value(RACE_KEY) };

            const std::vector<std::string> RACE_PARTS_STR_VEC { misc::SplitByChars(
                RACE_COLLECTOR_PARTS_STR) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (RACE_PARTS_STR_VEC.size() == 4),
                "creature::nonplayer::collector_type::FromCreature("
                    << CHARACTER_PTR->NameAndRaceAndRole()
                    << ") failed to read four values from the key=" << RACE_KEY);

            float chanceMinimalist(
                CHANCE_BASE + ConvertStringToFloat(RACE_KEY, RACE_PARTS_STR_VEC[0]));

            float chancePractical(
                CHANCE_BASE + ConvertStringToFloat(RACE_KEY, RACE_PARTS_STR_VEC[1]));

            float chanceCollector(
                CHANCE_BASE + ConvertStringToFloat(RACE_KEY, RACE_PARTS_STR_VEC[2]));

            float chanceHoarder(
                CHANCE_BASE + ConvertStringToFloat(RACE_KEY, RACE_PARTS_STR_VEC[3]));

            // adjust for roles
            const auto ROLE_STR { role::ToString(CHARACTER_PTR->Role()) };

            const auto ROLE_KEY {
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-role-"
                + ROLE_STR
            };

            const auto ROLE_COLLECTOR_PARTS_STR { misc::ConfigFile::Instance()->Value(ROLE_KEY) };

            const std::vector<std::string> ROLE_PARTS_STR_VEC { misc::SplitByChars(
                ROLE_COLLECTOR_PARTS_STR) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ROLE_PARTS_STR_VEC.size() == 4),
                "creature::nonplayer::collector_type::FromCreature("
                    << CHARACTER_PTR->NameAndRaceAndRole()
                    << ") failed to read four values from the key=" << ROLE_KEY);

            chanceMinimalist += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[0]);
            chancePractical += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[1]);
            chanceCollector += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[2]);
            chanceHoarder += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[3]);

            // enforce min
            {
                const float CHANCE_MIN(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "heroespath-nonplayer-ownershipprofile-collectortype-chance-minimum"));

                if (chanceMinimalist < CHANCE_MIN)
                {
                    chanceMinimalist = CHANCE_MIN;
                }

                if (chancePractical < CHANCE_MIN)
                {
                    chancePractical = CHANCE_MIN;
                }

                if (chanceCollector < CHANCE_MIN)
                {
                    chanceCollector = CHANCE_MIN;
                }

                if (chanceHoarder < CHANCE_MIN)
                {
                    chanceHoarder = CHANCE_MIN;
                }
            }

            // enforce max
            {
                const float CHANCE_MAX(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "heroespath-nonplayer-ownershipprofile-collectortype-chance-maximum"));

                if (chanceMinimalist > CHANCE_MAX)
                {
                    chanceMinimalist = CHANCE_MAX;
                }

                if (chancePractical > CHANCE_MAX)
                {
                    chancePractical = CHANCE_MAX;
                }

                if (chanceCollector > CHANCE_MAX)
                {
                    chanceCollector = CHANCE_MAX;
                }

                if (chanceHoarder > CHANCE_MAX)
                {
                    chanceHoarder = CHANCE_MAX;
                }
            }

            // determine
            collector_type::Enum collectorType { None };
            if (misc::random::Float() < chancePractical)
            {
                collectorType = static_cast<collector_type::Enum>(collectorType | Practical);
            }

            if (misc::random::Float() < chanceCollector)
            {
                collectorType = static_cast<collector_type::Enum>(collectorType | Collector);
            }

            if (misc::random::Float() < chanceMinimalist)
            {
                collectorType = static_cast<collector_type::Enum>(collectorType | Minimalist);
            }

            if ((misc::random::Float() < chanceHoarder) && (0 == (collectorType & Minimalist)))
            {
                collectorType = static_cast<collector_type::Enum>(collectorType | Hoarder);
            }

            return collectorType;
        }

        const std::string owns_magic_type::ToString(const owns_magic_type::Enum OWNS_MAGIC_TYPE)
        {
            if (OWNS_MAGIC_TYPE == Rarely)
            {
                return "Rarely";
            }
            else if (OWNS_MAGIC_TYPE == Religious)
            {
                return "Religious";
            }
            else if (OWNS_MAGIC_TYPE == Magical)
            {
                return "Magical";
            }
            else if (OWNS_MAGIC_TYPE == Count)
            {
                return "(Count)";
            }
            else
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(OWNS_MAGIC_TYPE));
                return "";
            }
        }

        owns_magic_type::Enum owns_magic_type::FromCreature(const CreaturePtr_t CHARACTER_PTR)
        {
            float chanceRarely(0.0f);
            float chanceReligious(0.0f);
            float chanceMagical(0.0f);

            // adjust for race
            {
                const auto RACE_STR(race::ToString(CHARACTER_PTR->Race()));

                const auto RACE_KEY {
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-race-" + RACE_STR
                };

                const auto RACE_OWNSMAGIC_PARTS_STR { misc::ConfigFile::Instance()->Value(
                    RACE_KEY) };

                const std::vector<std::string> RACE_PARTS_STR_VEC { misc::SplitByChars(
                    RACE_OWNSMAGIC_PARTS_STR) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (RACE_PARTS_STR_VEC.size() == 3),
                    "creature::nonplayer::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole()
                        << ") failed to read three values from the key=" << RACE_KEY);

                const auto RARELY_RACE_ADJ { ConvertStringToFloat(
                    RACE_KEY, RACE_PARTS_STR_VEC[0]) };

                const auto RELIGIOUS_RACE_ADJ { ConvertStringToFloat(
                    RACE_KEY, RACE_PARTS_STR_VEC[1]) };

                const auto MAGICAL_RACE_ADJ { ConvertStringToFloat(
                    RACE_KEY, RACE_PARTS_STR_VEC[2]) };

                // Why do these values have to add up to one?
                // After the role adjustments below the values most definitely don't sum to 1.0...?
                // zTn -2017-7-14
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (misc::IsRealOne(RARELY_RACE_ADJ + RELIGIOUS_RACE_ADJ + MAGICAL_RACE_ADJ)),
                    "creature::nonplayer::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole() << ") found key " << RACE_KEY << "="
                        << RACE_OWNSMAGIC_PARTS_STR << " -but those values do not add up to one.");

                chanceRarely += RARELY_RACE_ADJ;
                chanceReligious += RELIGIOUS_RACE_ADJ;
                chanceMagical += MAGICAL_RACE_ADJ;
            }

            // adjust for role
            {
                const auto ROLE_STR { role::ToString(CHARACTER_PTR->Role()) };

                std::ostringstream ss;
                ss << "heroespath-nonplayer-ownershipprofile-"
                   << "ownsmagictype-chance-adjustment-Rarely-role-" << ROLE_STR;

                const auto ROLE_KEY {
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-adjustment-role-"
                    + ROLE_STR
                };

                const auto ROLE_OWNSMAGIC_PARTS_STR { misc::ConfigFile::Instance()->Value(
                    ROLE_KEY) };

                const std::vector<std::string> ROLE_PARTS_STR_VEC { misc::SplitByChars(
                    ROLE_OWNSMAGIC_PARTS_STR) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (ROLE_PARTS_STR_VEC.size() == 3),
                    "creature::nonplayer::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole()
                        << ") failed to read three values from the key=" << ROLE_KEY);

                chanceRarely += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[0]);
                chanceReligious += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[1]);
                chanceMagical += ConvertStringToFloat(ROLE_KEY, ROLE_PARTS_STR_VEC[2]);
            }

            // enforce min
            {
                const float MIN(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-min"));

                if (chanceRarely < MIN)
                {
                    chanceRarely = MIN;
                }

                if (chanceReligious < MIN)
                {
                    chanceReligious = MIN;
                }

                if (chanceMagical < MIN)
                {
                    chanceMagical = MIN;
                }
            }

            // enforce max
            {
                const float MAX(misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-max"));

                if (chanceRarely > MAX)
                {
                    chanceRarely = MAX;
                }

                if (chanceReligious > MAX)
                {
                    chanceReligious = MAX;
                }

                if (chanceMagical > MAX)
                {
                    chanceMagical = MAX;
                }
            }

            // determine
            const auto RAND(
                misc::random::Float(0.0f, (chanceRarely + chanceReligious + chanceRarely)));

            if (RAND < chanceMagical)
            {
                return Magical;
            }
            else
            {
                if (RAND < (chanceMagical + chanceReligious))
                {
                    return Religious;
                }
                else
                {
                    return Rarely;
                }
            }
        }

        const std::string complexity_type::ToString(const complexity_type::Enum COMPLEXITY_TYPE)
        {
            switch (COMPLEXITY_TYPE)
            {
                case Animal:
                {
                    return "Animal";
                }
                case Simple:
                {
                    return "Simple";
                }
                case Moderate:
                {
                    return "Moderate";
                }
                case Complex:
                {
                    return "Complex";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(COMPLEXITY_TYPE));
                    return "";
                }
            }
        }

        complexity_type::Enum complexity_type::FromCreature(const CreaturePtr_t CHARACTER_PTR)
        {
            const auto RACE_COMPLEXITY_STR { misc::ConfigFile::Instance()->Value(
                "heroespath-nonplayer-ownershipprofile-complexitytype-race-"
                + race::ToString(CHARACTER_PTR->Race())) };

            if (RACE_COMPLEXITY_STR == "based-on-role")
            {
                return static_cast<complexity_type::Enum>(
                    FromString(misc::ConfigFile::Instance()->Value(
                        "heroespath-nonplayer-ownershipprofile-complexitytype-role-"
                        + role::ToString(CHARACTER_PTR->Role()))));
            }
            else
            {
                return static_cast<complexity_type::Enum>(FromString(RACE_COMPLEXITY_STR));
            }
        }

        Profile::Profile(
            const wealth_type::Enum WEALTH_TYPE,
            const collector_type::Enum COLLECTOR_TYPE,
            const owns_magic_type::Enum MAGIC_OWN_TYPE,
            const complexity_type::Enum COMPLEXITY_TYPE)
            : wealthType(WEALTH_TYPE)
            , collectorType(COLLECTOR_TYPE)
            , magicType(MAGIC_OWN_TYPE)
            , complexityType(COMPLEXITY_TYPE)
        {}

        const Profile Profile::Make_FromCreature(const CreaturePtr_t CHARACTER_PTR)
        {
            return Profile(
                wealth_type::FromCreature(CHARACTER_PTR),
                collector_type::FromCreature(CHARACTER_PTR),
                owns_magic_type::FromCreature(CHARACTER_PTR),
                complexity_type::FromCreature(CHARACTER_PTR));
        }

        float ConvertStringToFloat(const std::string & KEY, const std::string & STR_FLOAT)
        {
            try
            {
                return boost::lexical_cast<float>(STR_FLOAT);
            }
            catch (...)
            {
                std::ostringstream ss;
                ss << "creature::nonplayer::ConvertStringToFloat(key=" << KEY
                   << ", STR_FLOAT=" << STR_FLOAT << " unable to convert that str to a float.";

                throw std::runtime_error(ss.str());
            }
        }

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath
