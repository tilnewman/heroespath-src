// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_WEAPON_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPON_ENUM_HPP_INCLUDED
//
// weapon-enum.hpp
//
#include "creature/complexity-type.hpp"
#include "game/strong-types.hpp"
#include "item/category-enum.hpp"
#include "item/element-enum.hpp"
#include "item/equipment-details.hpp"
#include "item/material-enum.hpp"
#include "misc/enum-common.hpp"

#include <array>
#include <string_view>

namespace heroespath
{
namespace item
{

    // IF YOU MAKE ANY CHANGES TO ANY OF THESE: UPDATE item-template-factory.hpp

    struct Weapon : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Axe = 0,
            Bladedstaff,
            BodyPart,
            Club,
            Knife,
            Projectile,
            Staff,
            Sword,
            Whip,
            Count
        };

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            if (ENUM == BodyPart)
            {
                return "Body Part";
            }
            else
            {
                return NAMEOF_ENUM(ENUM);
            }
        }
    };

    struct Axes : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Handaxe = 0,
            Sickle,
            Battleaxe,
            Waraxe,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "one-handed chopping axe not even intended for battle",
            "short one-handed curved blade ",
            "two-handed double-edged heavy axe",
            "two-handed double-edged heavy axe used in combat and ceremony"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails( 50_score,  10_coin,  500_weight,  1_health,  3_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 80_score,  50_coin,  350_weight,  3_health,  6_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(100_score, 350_coin, 1200_weight,  8_health, 16_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(200_score, 600_coin, 2000_weight, 12_health, 18_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Sickle: return ElementList::MakeCombinationsList(Element::Shadow);

                case Battleaxe:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Frost);

                case Waraxe:
                    return ElementList::MakeCombinationsList(Element::Honor | Element::Shadow);

                case Handaxe:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Bladedstaffs : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            ShortSpear = 0,
            Spear,
            Scythe,
            Pike,
            Partisan,
            Halberd,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "short shaft with a pointed tip",
            "long shaft with a pointed tip",
            "long shaft of wood with a crossing steel blade",
            "very long thrusting spear",
            "very long thrusting spear with side flanges for blocking",
            "very long thrusting spear with an axe protruding from the side"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails( 50_score,  50_coin, 125_weight, 3_health,  7_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Tipped),
            EquipmentDetails( 70_score,  85_coin, 200_weight, 4_health,  8_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Tipped),
            EquipmentDetails( 80_score, 200_coin, 400_weight, 7_health, 10_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(100_score, 135_coin, 400_weight, 5_health,  9_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(150_score, 220_coin, 700_weight, 8_health, 10_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(200_score, 250_coin, 750_weight, 9_health, 13_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr std::string_view Name(const Bladedstaffs::Enum BSTAFF_TYPE) noexcept
        {
            if (BSTAFF_TYPE == Bladedstaffs::ShortSpear)
            {
                return "Short Spear";
            }
            else
            {
                return NAMEOF_ENUM(BSTAFF_TYPE);
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Scythe:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);

                case Pike:
                    return ElementList::MakeCombinationsList(Element::Frost | Element::Honor);

                case Partisan:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Honor);

                case Halberd:
                    return ElementList::MakeCombinationsList(
                        Element::Fire | Element::Frost | Element::Honor);

                case Spear:
                case ShortSpear:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct BodyPartWeapons : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Fists = 0,
            Claws,
            Tentacles,
            Bite,
            BreathOfFire,
            BreathOfFrost,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum ENUM) noexcept
        {
            return ((ENUM == Fists) || (ENUM == Claws) || (ENUM == Bite));
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum) noexcept
        {
            return ElementList::None;
        }

        static constexpr MaterialPair_t MaterialsPair(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Fists: return std::make_pair(Material::Bone, Material::Hide);
                case Claws: return std::make_pair(Material::Claw, Material::Count);
                case Tentacles: return std::make_pair(Material::Plant, Material::Count);
                case Bite: return std::make_pair(Material::Tooth, Material::Count);

                case BreathOfFire:
                case BreathOfFrost: return std::make_pair(Material::Gas, Material::Count);

                case Count:
                default: return std::make_pair(Material::Count, Material::Count);
            }
        }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "The clendched fists of a",           "The sharp tearing claws of a",
            "The writhing fleshy tenticles of a", "The fanged jaws of a",
            "The flaming breath of a Firebran",   "The toxic freezing breath of a Sylavin"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails(0_score, 0_coin, 190_weight, 1_health,  2_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 200_weight, 1_health,  3_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 350_weight, 1_health,  2_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 180_weight, 2_health,  4_health, Category::None,      creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin,   1_weight, 4_health,  9_health, Category::None,      creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin,   1_weight, 7_health, 10_health, Category::None,      creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart)
        };
        // clang-format on
    };

    struct Clubs : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Spiked = 0,
            Maul,
            Mace,
            Warhammer,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "spiked weight at the end of a handle",
            "long handled wedged hammer",
            "short handled bashing club",
            "large heavy ceremonial club"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails( 50_score, 200_coin, 1000_weight,  6_health, 12_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle), 
            EquipmentDetails( 60_score, 500_coin,  900_weight,  8_health, 14_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 80_score, 550_coin, 1050_weight,  9_health, 13_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(120_score, 600_coin, 2000_weight, 12_health, 18_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr std::string_view Name(const Enum CLUB_TYPE) noexcept
        {
            if (CLUB_TYPE == Spiked)
            {
                return "Spiked Club";
            }
            else
            {
                return NAMEOF_ENUM(CLUB_TYPE);
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Spiked:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);

                case Mace:
                    return ElementList::MakeCombinationsList(Element::Frost | Element::Honor);

                case Warhammer: return ElementList::All;

                case Maul:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Knifes : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Knife = 0,
            Dagger,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return true; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "short blade with a handle", "slightly curved blade with a handle"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails(22_score, 15_coin,  85_weight, 2_health, 3_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(33_score, 25_coin, 115_weight, 4_health, 5_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            if (ENUM == Knife)
            {
                return ElementList::MakeCombinationsList(
                    Element::Fire | Element::Frost | Element::Shadow);
            }
            else
            {
                return ElementList::MakeCombinationsList(
                    Element::Fire | Element::Frost | Element::Honor);
            }
        }
    };

    struct Projectiles : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Blowpipe = 0,
            Sling,
            Shortbow,
            Longbow,
            Crossbow,
            Compositebow,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "wooden pipe with a collection of poisoned tipped darts",
            "length of leather with a pouch for throwing stones",
            "smaller inward curving bow.",
            "bow that extends the full height of the archer and curves inward",
            "wooden construct with a pull string that fires metal bolts",
            "smaller bow made from animal horn with ends that curve outward"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails( 50_score, 150_coin, 200_weight,  1_health,  3_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::Handle),
            EquipmentDetails( 60_score,  25_coin,  75_weight,  2_health,  5_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::Handle),
            EquipmentDetails(100_score, 100_coin, 250_weight,  4_health,  9_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::Handle),
            EquipmentDetails(200_score, 200_coin, 450_weight,  8_health, 13_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::Handle), 
            EquipmentDetails(300_score, 300_coin, 650_weight, 10_health, 15_health, Category::TwoHanded, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::Decoration),
            EquipmentDetails(300_score, 450_coin, 650_weight,  9_health, 16_health, Category::TwoHanded, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Longbow:
                    return ElementList::MakeCombinationsList(
                        ElementList::Fire | ElementList::Frost);

                case Crossbow:
                    return ElementList::MakeCombinationsList(
                        ElementList::Honor | ElementList::Shadow);

                case Compositebow: return ElementList::All;

                case Blowpipe:
                case Sling:
                case Shortbow:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Staffs : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Staff = 0,
            Quarterstaff,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum ENUM) noexcept { return (ENUM == Staff); }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "long thin shaft", "shaft of medium length"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails(25_score, 10_coin, 300_weight, 1_health, 6_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced),
            EquipmentDetails(15_score, 10_coin, 150_weight, 1_health, 3_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            if (ENUM == Staff)
            {
                return ElementList::MakeCombinationsList(ElementList::Shadow | ElementList::Honor);
            }
            else
            {
                return ElementList::MakeCombinationsList(ElementList::Frost | ElementList::Honor);
            }
        }
    };

    struct Swords : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Shortsword = 0,
            Gladius,
            Cutlass,
            Falcata,
            Rapier,
            Saber,
            Broadsword,
            Longsword,
            Knightlysword,
            Flamberg,
            Claymore,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "sword shorter than the length of an arm",
            "broad one-handed double-edged short-bladed sword with a protective guard",
            "shorter one-handed single-edged broad sabre with a slightly curved blade and a solid "
            "cupped guard",
            "one-handed single-edged forward-sloping sword",
            "one-handed double-edged long-bladed sword with a protective guard",
            "one-handed single-edged curved blade with a large hand guard",
            "broad one-handed sword with a basket hilt",
            "large two-handed sword",
            "larger one-handed sword with a fuller and cruciform hilt",
            "longer one-handed sword with a wavy blade",
            "large two-handed broadsword with forward tilting crossguards"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails( 50_score,  50_coin,  400_weight,  3_health,  7_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 55_score,  70_coin,  450_weight,  4_health,  8_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 40_score, 100_coin,  500_weight,  4_health,  6_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle), 
            EquipmentDetails( 50_score, 100_coin,  700_weight,  7_health,  8_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 70_score, 130_coin,  450_weight,  5_health,  9_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails( 70_score, 160_coin,  725_weight,  5_health, 10_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(100_score, 180_coin,  750_weight,  6_health, 10_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(120_score, 300_coin, 1000_weight,  8_health, 14_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(110_score, 180_coin,  750_weight,  6_health, 12_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(190_score, 400_coin,  800_weight, 10_health, 15_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle),
            EquipmentDetails(200_score, 500_coin, 1200_weight, 10_health, 16_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Gladius: return ElementList::MakeCombinationsList(Element::Honor);
                case Cutlass: return ElementList::MakeCombinationsList(Element::Honor);

                case Falcata:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);

                case Rapier:
                    return ElementList::MakeCombinationsList(
                        Element::Frost | Element::Fire | Element::Shadow);

                case Saber: return ElementList::MakeCombinationsList(Element::Shadow);
                case Broadsword: return ElementList::MakeCombinationsList(Element::Honor);

                case Longsword:
                    return ElementList::MakeCombinationsList(Element::Frost | Element::Fire);

                case Knightlysword:
                    return ElementList::MakeCombinationsList(Element::Frost | Element::Honor);

                case Flamberg:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);

                case Claymore: return ElementList::All;

                case Shortsword:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Whips : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Bullwhip = 0,
            Flail,
            MaceAndChain,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "sturdy leather whip",
            "two-handed long stick handle with a shorter stick attached with soft leather",
            "short stick with a chain connecting a spiked end"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails(30_score,  60_coin, 100_weight, 1_health,  4_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Decoration),
            EquipmentDetails(60_score, 110_coin, 600_weight, 5_health, 10_health, Category::TwoHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle), 
            EquipmentDetails(90_score, 225_coin, 800_weight, 8_health, 12_health, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Handle)
        };
        // clang-format on

        static constexpr std::string_view Name(const Whips::Enum WHIP_TYPE) noexcept
        {
            if (WHIP_TYPE == Whips::MaceAndChain)
            {
                return "Mace and Chain";
            }
            else
            {
                return NAMEOF_ENUM(WHIP_TYPE);
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Flail:
                    return ElementList::MakeCombinationsList(Element::Honor | Element::Shadow);

                case MaceAndChain:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Frost);

                case Bullwhip:
                case Count:
                default: return ElementList::None;
            }
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPON_ENUM_HPP_INCLUDED
