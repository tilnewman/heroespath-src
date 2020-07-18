// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ARMOR_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMOR_ENUM_HPP_INCLUDED
//
// armor-enum.hpp
//
#include "creature/complexity-type.hpp"
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

    struct Armor : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            // these can and must have an associated Form
            Aventail = 0,
            Boot,
            Bracer,
            Gauntlet,
            Pant,
            Shirt,

            // these can and must have an associated sub-type enum
            Skin,
            Cover,
            Helm,
            Shield,

            Count
        };
    };

    struct Skins : public EnumBaseCounting<>
    {
        // Flesh is not used because it was decided that creatures will not have a "skin" armor
        // unless it makes a real improvement on armor rating, which normal flesh does not.
        enum Enum : EnumUnderlying_t
        {
            Flesh = 0,
            Fur,
            Hide,
            Plant,
            Scale,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return true; }

        static constexpr const std::array<std::string_view, Enum::Count> descriptions {
            "skin", "fur", "hide", "tough plant", "scales"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details {
            EquipmentDetails(0_score, 0_coin, 10_weight,  4_armor, Category::None, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 10_weight, 10_armor, Category::None, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 10_weight, 20_armor, Category::None, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 10_weight, 30_armor, Category::None, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart),
            EquipmentDetails(0_score, 0_coin, 10_weight, 40_armor, Category::None, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::BodyPart)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum) noexcept
        {
            return ElementList::None;
        }
    };

    namespace detail
    {

        // this base class is only here to hold constexpr members that the derived needs to
        // build other constexpr members.  You can't build one constexpr member from other
        // static constexpr member functions.  So a base class like this is needed to hold
        // static constexpr member functions that the derived will use to make more
        // constexpr...stuff.
        struct FormsBase
        {
            enum Enum : EnumUnderlying_t
            {
                Plain = 0,
                Mail,
                Scale,
                Plate,
                Count
            };

            static constexpr Score_t TypeScore(const Armor::Enum ENUM) noexcept
            {
                switch (ENUM)
                {
                    case Armor::Aventail: return 34_score;
                    case Armor::Boot: return 46_score;
                    case Armor::Bracer: return 36_score;
                    case Armor::Gauntlet: return 30_score;
                    case Armor::Pant: return 55_score;
                    case Armor::Shirt: return 44_score;

                    case Armor::Skin:
                    case Armor::Cover:
                    case Armor::Helm:
                    case Armor::Shield:
                    case Armor::Count:
                    default: return 0_score;
                }
            }

            static constexpr Score_t SubTypeScore(const Enum ENUM) noexcept
            {
                switch (ENUM)
                {
                    case Plain: return 25_score;
                    case Mail: return 75_score;
                    case Scale: return 125_score;
                    case Plate: return 175_score;
                    case Count:
                    default: return 0_score;
                }
            }

            static constexpr Score_t Score(const Armor::Enum TYPE, const Enum SUB_TYPE) noexcept
            {
                return (TypeScore(TYPE) + SubTypeScore(SUB_TYPE));
            }
        };

    } // namespace detail

    struct Forms
        : public detail::FormsBase
        , public EnumBaseCounting<>
    {

        static constexpr bool HasPixieVersion(
            const Armor::Enum ARMOR_TYPE, const Forms::Enum FORM_TYPE = Forms::Count) noexcept
        {
            switch (ARMOR_TYPE)
            {
                case Armor::Boot:
                case Armor::Bracer:
                case Armor::Shirt:
                case Armor::Pant:
                case Armor::Gauntlet:
                case Armor::Cover: return ((FORM_TYPE < Count) && (FORM_TYPE != Plate));

                case Armor::Skin: return true;

                case Armor::Aventail:
                case Armor::Helm:
                case Armor::Shield:
                case Armor::Count:
                default: return false;
            }
        }

        static constexpr const std::array<std::string_view, (6 * Forms::Count)> descriptions {
            "neck covering attched to a helm",
            "pair of boots",
            "forearm covering",
            "pair of gloves",
            "pair of pants",
            "shirt",
            //
            "neck covering attched to a helm of interlocking rings",
            "pair of boots made of interlocking rings",
            "forearm covering of interlocking rings",
            "pair of guantlets of interlocking rings",
            "pair of pants made of interlocking rings",
            "shirt of interlocking rings",
            //
            "neck covering attched to a helm of interlocking pieces",
            "pair of boots of interlocking pieces",
            "forearm covering of interlocking pieces",
            "pair of guantlets of interlocking pieces",
            "pair of pants made of interlocking pieces",
            "shirt of interlocking pieces",
            //
            "neck covering attched to a helm of interlocking plates",
            "pair of boots that knights wear of interlocking plates",
            "forearm covering that knights wear of interlocking plates",
            "pair of gauntlets that knights wear of interlocking plates",
            "pair of pants that knights wear made of interlocking plates",
            "shirt that knights wear of interlocking plates"
        };

        static constexpr const EquipmentDetails &
            Details(const Armor::Enum TYPE, const Enum FORM) noexcept
        {
            return details[Index(TYPE, FORM)];
        }

        static constexpr const std::string_view
            Description(const Armor::Enum TYPE, const Enum FORM) noexcept
        {
            return descriptions[Index(TYPE, FORM)];
        }

        static constexpr ElementList::Enum
            ElementsAllowed(const Armor::Enum ARMOR_TYPE, const Forms::Enum FORMS_TYPE) noexcept
        {
            if ((FORMS_TYPE >= Forms::Count) || (FORMS_TYPE == Forms::Plain))
            {
                return ElementList::None;
            }

            if ((ARMOR_TYPE == Armor::Bracer) || (ARMOR_TYPE == Armor::Boot)
                || (ARMOR_TYPE == Armor::Aventail))
            {
                return ElementList::MakeCombinationsList(Element::Fire | Element::Frost);
            }

            if ((ARMOR_TYPE == Armor::Gauntlet) || (ARMOR_TYPE == Armor::Pant)
                || (ARMOR_TYPE == Armor::Shirt))
            {
                return ElementList::MakeCombinationsList(Element::Honor | Element::Shadow);
            }

            return ElementList::None;
        }

    private:
        static constexpr std::size_t Index(const Armor::Enum TYPE, const Enum FORM) noexcept
        {
            return (static_cast<std::size_t>(TYPE) * static_cast<std::size_t>(FORM));
        }

        // clang-format off
        static constexpr const std::array<EquipmentDetails, (6 * Forms::Count)> details {
            EquipmentDetails(Score(Armor::Aventail, Plain), 100_coin,  215_weight,  20_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Boot,     Plain),  15_coin,  185_weight,  10_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Bracer,   Plain),  18_coin,  125_weight,  20_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Gauntlet, Plain),   8_coin,   50_weight,  10_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ReinforcedWithBase),
            EquipmentDetails(Score(Armor::Pant,     Plain),  22_coin,  125_weight,  10_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Shirt,    Plain),  15_coin,  185_weight,  10_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,   MaterialNameStyle::ClaspedWithBase),
            //
            EquipmentDetails(Score(Armor::Aventail, Mail),  135_coin,  300_weight,  40_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Boot,     Mail),   55_coin,  300_weight,  50_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Bracer,   Mail),   50_coin,  250_weight,  40_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Gauntlet, Mail),   35_coin,  300_weight,  20_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ReinforcedWithBase),
            EquipmentDetails(Score(Armor::Pant,     Mail),  125_coin,  800_weight,  70_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Shirt,    Mail),   55_coin,  300_weight,  50_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            //
            EquipmentDetails(Score(Armor::Aventail, Scale), 200_coin,  200_weight,  50_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Boot,     Scale), 115_coin,  350_weight,  70_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Bracer,   Scale),  80_coin,  200_weight,  50_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Gauntlet, Scale),  45_coin,  125_weight,  30_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ReinforcedWithBase),
            EquipmentDetails(Score(Armor::Pant,     Scale), 205_coin, 1000_weight,  80_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Shirt,    Scale), 115_coin,  300_weight,  70_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex,  MaterialNameStyle::ClaspedWithBase),
            //
            EquipmentDetails(Score(Armor::Aventail, Plate), 300_coin,  300_weight,  60_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Boot,     Plate), 250_coin,  400_weight,  90_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Bracer,   Plate), 150_coin,  300_weight,  70_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Gauntlet, Plate),  50_coin,  220_weight,  40_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ReinforcedWithBase),
            EquipmentDetails(Score(Armor::Pant,     Plate), 400_coin, 1500_weight,  90_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ClaspedWithBase),
            EquipmentDetails(Score(Armor::Shirt,    Plate), 250_coin,  400_weight,  90_armor, Category::Wearable, creature::nonplayer::complexity_type::Moderate, MaterialNameStyle::ClaspedWithBase),
        };
        // clang-format on
    };

    struct Covers : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Cape = 0,
            Vest,
            Robe,
            Cloak,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return true; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "back covering",
            "torso cover that is cut short at the arms",
            "body covering without a hood",
            "full body cover including a hood"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details { 
            EquipmentDetails( 25_score,  16_coin,  170_weight,  10_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Clasped),
            EquipmentDetails( 50_score,  22_coin,  180_weight,  20_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Clasped),
            EquipmentDetails( 75_score,  24_coin,  240_weight,  25_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Clasped),
            EquipmentDetails(100_score,  30_coin,  300_weight,  30_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Clasped)
        };
        // clang-format on

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Cape:
                    return ElementList::MakeCombinationsList(Element::Honor | Element::Shadow);

                case Vest: return ElementList::MakeCombinationsList(Element::Fire | Element::Frost);

                case Cloak: return ElementList::All;

                case Robe:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Helms : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Leather = 0,
            MailCoif,
            Kettle,
            Archers,
            Bascinet,
            Great,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "simple top-covering single-piece helm",
            "single-piece of mail that fits over the head.",
            "top-covering brimmed single-piece helm",
            "single-piece helm that covers the top and back of the head",
            "two-piece helm with a flip front",
            "single-piece full-covering helm that knights wear with a thin visor"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details { 
            EquipmentDetails( 25_score,  15_coin,  100_weight,  20_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,  MaterialNameStyle::Reinforced),
            EquipmentDetails( 50_score,  50_coin,  250_weight,  40_armor, Category::Wearable, creature::nonplayer::complexity_type::Complex, MaterialNameStyle::Reinforced),
            EquipmentDetails( 75_score,  25_coin,  150_weight,  30_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,  MaterialNameStyle::Reinforced),
            EquipmentDetails(100_score,  75_coin,  250_weight,  50_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,  MaterialNameStyle::Reinforced),
            EquipmentDetails(125_score, 120_coin,  350_weight,  60_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,  MaterialNameStyle::Reinforced),
            EquipmentDetails(150_score, 200_coin, 1100_weight,  70_armor, Category::Wearable, creature::nonplayer::complexity_type::Simple,  MaterialNameStyle::Reinforced)
        };
        // clang-format on

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Leather: return "Leather Helm";
                case MailCoif: return "Mail Coif Helm";
                case Kettle: return "Kettle Helm";
                case Archers: return "Archers Helm";
                case Bascinet: return "Bascinet Helm";
                case Great: return "Great Helm";
                case Count: return "Count";
                default: return "item::Helms::Name(ENUM)_out_of_range";
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case MailCoif: return ElementList::MakeCombinationsList(Element::Frost);
                case Kettle: return ElementList::MakeCombinationsList(Element::Shadow);
                case Archers: return ElementList::MakeCombinationsList(Element::Honor);
                case Bascinet: return ElementList::MakeCombinationsList(Element::Fire);

                case Great: return ElementList::All;

                case Leather:
                case Count:
                default: return ElementList::None;
            }
        }
    };

    struct Shields : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Buckler = 0,
            Kite,
            Heater,
            Pavis,
            Count
        };

        static constexpr bool HasPixieVersion(const Enum) noexcept { return false; }

        static constexpr const std::array<std::string_view, Count> descriptions {
            "small circular shield",
            "shield with a round top and a pointed bottom",
            "shield with a flat top and a pointed bottom",
            "convex full-covering shield that knights use"
        };

        // clang-format off
        static constexpr const std::array<EquipmentDetails, Count> details { 
            EquipmentDetails( 50_score,  20_coin,  200_weight,   40_armor, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced),
            EquipmentDetails(100_score,  80_coin,  800_weight,   70_armor, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced),
            EquipmentDetails(150_score, 200_coin, 1100_weight,   80_armor, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced),
            EquipmentDetails(200_score, 400_coin, 2000_weight,   90_armor, Category::OneHanded, creature::nonplayer::complexity_type::Simple, MaterialNameStyle::Reinforced)
        };
        // clang-format on

        static constexpr std::string_view Name(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Buckler: return "Buckler Shield";
                case Kite: return "Kite Shield";
                case Heater: return "Heater Shield";
                case Pavis: return "Pavis Shield";
                case Count: return "Count";
                default: return "item::Shields::Name(ENUM)_out_of_range";
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Kite:
                    return ElementList::MakeCombinationsList(Element::Frost | Element::Honor);

                case Heater:
                    return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);

                case Pavis: return ElementList::All;

                case Buckler:
                case Count:
                default: return ElementList::None;
            }
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMOR_ENUM_HPP_INCLUDED
