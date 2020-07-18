// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_PROFILE_HPP_INCLUDE
//
// item-profile.hpp
//
#include "creature/summon-info.hpp"
#include "game/strong-types.hpp"
#include "item/item-template-factory.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace item
{

    struct ItemProfileFactory;

    // Responsible for wrapping all the information required to create a new Item.  Basically
    // everything about an item that never changes, including the treasure score.
    //
    // This is where a Template, which can only be one of either Weapon/Armor/Misc, is merged
    // with all other item details and can become any combination of Weapon/Armor/Misc, which is why
    // you see functions here that pass both a Template and a Misc::Enum together.  This is also
    // why this class has a Misc::Enum member when the Template already has one.
    //
    // A complete set of these ItemProfiles (sorted by treasure score)is kept in the
    // ItemProfileWarehouse, which allows the game to randomly generate/select which items the
    // player finds after combat.
    class ItemProfile
    {
        // only the factory can use the valid constructors and make valid ItemProfiles
        friend ItemProfileFactory;

        // this constructor is used by all the others and is always the last to execute
        ItemProfile(
            const ItemTemplate & TEMPLATE,
            const bool IS_PIXIE,
            const Material::Enum MAT_PRI,
            const Material::Enum MAT_SEC,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const Element::Enum ELEMENT,
            const Misc::Enum MISC,
            const creature::SummonInfo & SUMMON_INFO,
            const Score_t SCORE,
            const Score_t RELIGIOUS_SCORE,
            const Category::Enum CATEGORY,
            const bool IS_MAGICAL,
            const Armor_t ARMOR_RATING,
            const Coin_t PRICE,
            const Weight_t WEIGHT)
            : template_(TEMPLATE)
            , isPixie_(IS_PIXIE)
            , matPri_(MAT_PRI)
            , matSec_(MAT_SEC)
            , misc_(MISC)
            , set_(SET)
            , named_(NAMED)
            , element_(ELEMENT)
            , summonInfo_(SUMMON_INFO)
            , score_(SCORE)
            , religiousScore_(RELIGIOUS_SCORE)
            , category_(CATEGORY)
            , isMagical_(IS_MAGICAL)
            , armorRating_(ARMOR_RATING)
            , price_(PRICE)
            , weight_(WEIGHT)
        {}

    public:
        // creates an invalid ItemProfile, this only exists for boost::serialization
        ItemProfile() = default;

        constexpr bool IsPixie() const noexcept { return isPixie_; }
        constexpr Set::Enum SetType() const noexcept { return set_; }
        constexpr Named::Enum NamedType() const noexcept { return named_; }
        constexpr Element::Enum ElementType() const noexcept { return element_; }
        constexpr Misc::Enum MiscType() const noexcept { return misc_; }
        constexpr Category::Enum Category() const noexcept { return category_; }
        constexpr bool IsQuest() const noexcept { return Misc::IsQuest(misc_); }
        constexpr Material::Enum MaterialPrimary() const noexcept { return matPri_; }
        constexpr Material::Enum MaterialSecondary() const noexcept { return matSec_; }
        constexpr Score_t Score() const noexcept { return score_; }
        constexpr const creature::SummonInfo SummoningInfo() const noexcept { return summonInfo_; }
        
        constexpr bool IsWeapon() const noexcept { return (template_.IsOnlyWeapon() || (IsMisc() && (Misc::))); }
        constexpr bool IsArmor() const noexcept { return template_.IsArmor(); }

        constexpr bool IsMisc() const noexcept { return (misc_ < Misc::Count); }
        constexpr bool IsUnique() const noexcept { return Misc::IsUnique(misc_); }
        constexpr bool IsNamed() const noexcept { return (named_ < Named::Count); }
        constexpr bool IsSet() const noexcept { return (set_ < Set::Count); }
        constexpr bool IsBodyPart() const noexcept { return template_.IsBodyPartEquipment(); }
        constexpr bool IsMagical() const noexcept { return isMagical_; }
        constexpr Armor_t ArmorRating() const noexcept { return armorRating_; }
        constexpr Coin_t Price() const noexcept { return price_; }
        constexpr Weight_t Weight() const noexcept { return weight_; }
        constexpr Score_t ReligiousScore() const noexcept { return religiousScore_; }
        constexpr bool IsReligious() const noexcept { return (religiousScore_.IsZeroOrMore()); }

        constexpr bool HasElementType(const Element::Enum ENUM) const noexcept
        {
            return (element_ & ENUM);
        }

        constexpr bool IsElemental() const noexcept { return (element_ != Element::None); }

        constexpr bool CanSummon() const noexcept { return summonInfo_.IsValid(); }

        constexpr Health_t DamageMin() const noexcept
        {
            return template_.GetEquipmentDetails().damage_min;
        }

        constexpr Health_t DamageMax() const noexcept
        {
            return template_.GetEquipmentDetails().damage_max;
        }

        constexpr creature::nonplayer::complexity_type::Enum Complexity() const noexcept
        {
            return template_.GetEquipmentDetails().complexity;
        }

        constexpr const std::string_view BaseDescription() const noexcept
        {
            return template_.EquipmentDescription();
        }

        constexpr Armor::Enum ArmorType() const noexcept { return template_.ArmorInfo().Type(); }

        constexpr Weapon::Enum WeaponType() const noexcept
        {
            return template_.WeaponInfo().Type();
        }

        constexpr MaterialNameStyle::Enum GetMaterialNameStyle() const noexcept
        {
            return template_.GetMaterialNameStyle();
        }

        constexpr const WeaponTemplate & WeaponInfo() const noexcept
        {
            return template_.WeaponInfo();
        }

        constexpr const ArmorTemplate & ArmorInfo() const noexcept
        {
            return template_.ArmorInfo();
        }

        constexpr bool MustBePixieVersionForPixiesToEquip() const noexcept
        {
            if (IsWeapon() || IsArmor())
            {
                return true;
            }
            else
            {
                return Misc::MustBePixieVersionForPixiesToEquip(misc_);
            }
        }

        const std::string ToString() const;

        const std::string SystemName() const
        {
            if (misc_ < Misc::Count)
            {
                return std::string(NAMEOF_ENUM(misc_));
            }
            else
            {
                return template_.SystemNameBase();
            }
        }

        const std::string ReadableName() const
        {
            if (misc_ < Misc::Count)
            {
                return std::string(Misc::Name(misc_));
            }
            else
            {
                return template_.ReadableNameBase();
            }
        }

        // intended to only be used during unit tests
        constexpr bool IsValid() const noexcept
        {
            if (!template_.IsValid() || (category_ == Category::None)
                || (matPri_ >= Material::Count) || (matPri_ == matSec_) || (IsWeapon() && IsArmor())
                || (CanSummon() && !IsMisc()) || (score_ <= 0_score))
            {
                return false;
            }

            if (IsSet())
            {
                if (IsElemental() || !IsMagical() || IsUnique())
                {
                    return false;
                }
            }

            if (IsNamed())
            {
                if (IsSet() || IsMisc() || summonInfo_.IsValid() || (!IsWeapon() && !IsArmor())
                    || IsUnique())
                {
                    return false;
                }
            }

            if (!isMagical_)
            {
                if (IsElemental() || IsSet() || IsNamed() || CanSummon()
                    || Misc::IsAlwaysMagical(misc_))
                {
                    return false;
                }
            }

            return true;
        }

        friend constexpr bool operator==(const ItemProfile & L, const ItemProfile & R) noexcept;
        friend constexpr bool operator<(const ItemProfile & L, const ItemProfile & R) noexcept;

    private:
        // these members are all that is needed to uniquely identify an ItemProfile
        // template_ will be weapon or armor if this is a misc item that is also equipment
        ItemTemplate template_ {};
        bool isPixie_ = false;
        Material::Enum matPri_ = Material::Count;
        Material::Enum matSec_ = Material::Count;
        Misc::Enum misc_ = Misc::Count;
        Set::Enum set_ = Set::Count;
        Named::Enum named_ = Named::Count;
        Element::Enum element_ = Element::None;
        creature::SummonInfo summonInfo_ = creature::SummonInfoEmpty;

        // score is completely generated from the members above but it IS STILL used when
        // comparing because the whole point of having ItemProfiles is to have a complete set
        // that is sorted (first) by score
        Score_t score_ = 0_score;

        // these members are completely generated from the members above
        // so there is no need to check these when comparing
        Score_t religiousScore_ = 0_score;
        Category::Enum category_ = Category::None;
        bool isMagical_ = false;
        Armor_t armorRating_ = 0_armor;
        Coin_t price_ = 0_coin;
        Weight_t weight_ = 0_weight;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & template_;
            ar & isPixie_;
            ar & matPri_;
            ar & matSec_;
            ar & misc_;
            ar & set_;
            ar & named_;
            ar & element_;
            ar & summonInfo_;
            ar & score_;
            ar & religiousScore_;
            ar & category_;
            ar & isMagical_;
            ar & armorRating_;
            ar & price_;
            ar & weight_;
        }
    };

    using ItemProfileVec_t = std::vector<ItemProfile>;

    // only compares non-generated members except for score
    constexpr bool operator==(const ItemProfile & L, const ItemProfile & R) noexcept
    {
        // shortcut to member that most often varies
        if (L.score_ != R.score_)
        {
            return false;
        }

        return std::tie(
                   L.template_,
                   L.isPixie_,
                   L.matPri_,
                   L.matSec_,
                   L.misc_,
                   L.set_,
                   L.named_,
                   L.element_,
                   L.summonInfo_)
            == std::tie(
                   R.template_,
                   R.isPixie_,
                   R.matPri_,
                   R.matSec_,
                   R.misc_,
                   R.set_,
                   R.named_,
                   R.element_,
                   R.summonInfo_);
    }

    // only compares non-generated members except for score
    constexpr bool operator!=(const ItemProfile & L, const ItemProfile & R) noexcept
    {
        return !(L == R);
    }

    // only compares non-generated members except for score which is sorted by first
    constexpr bool operator<(const ItemProfile & L, const ItemProfile & R) noexcept
    {
        // sort order must always consider score first
        // and also a handy shortcut to the member that most often varies
        if (L.score_ != R.score_)
        {
            return (L.score_ < R.score_);
        }

        return std::tie(
                   L.template_,
                   L.misc_,
                   L.summonInfo_,
                   L.named_,
                   L.set_,
                   L.element_,
                   L.matPri_,
                   L.matSec_,
                   L.isPixie_)
            < std::tie(
                   R.template_,
                   R.misc_,
                   R.summonInfo_,
                   R.named_,
                   R.set_,
                   R.element_,
                   R.matPri_,
                   R.matSec_,
                   R.isPixie_);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_HPP_INCLUDE
