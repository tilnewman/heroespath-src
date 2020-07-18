// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile-factory.cpp
//
#include "item-profile-factory.hpp"

#include "creature/creature.hpp"
#include "creature/enchantment-factory.hpp"
#include "item/materials-factory.hpp"

namespace heroespath
{
namespace item
{

    const ItemProfile ItemProfileFactory::Make(
        const Armor::Enum ARMOR_TYPE,
        const Forms::Enum FORM_TYPE,
        const bool IS_PIXIE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC)
    {
        return Make(
            ItemTemplateFactory::Instance()->MakeArmor(ARMOR_TYPE, FORM_TYPE),
            IS_PIXIE,
            MAT_PRI,
            MAT_SEC,
            Named::Count,
            Set::Count,
            Element::None,
            Misc::Count);
    }

    const ItemProfile ItemProfileFactory::Make(
        const ItemTemplate & TEMPLATE_ORIG,
        const bool IS_PIXIE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const Element::Enum ELEMENT,
        const Misc::Enum MISC,
        const creature::SummonInfo & SUMMON_INFO)
    {
        const auto TEMPLATE_FINAL = SwitchToEquipmentTemplateIfNeeded(TEMPLATE_ORIG);

        const auto SCORE
            = CalcScore(TEMPLATE_FINAL, MAT_PRI, MAT_SEC, NAMED, SET, ELEMENT, MISC, SUMMON_INFO);

        const auto RELIGIOUS_SCORE = SCORE.ScaleCopy(Misc::ReligiousRatio(MISC));

        const auto CATEGORY = CalcCategory(TEMPLATE_FINAL, MISC);

        return ItemProfile(
            TEMPLATE_FINAL,
            IS_PIXIE,
            MAT_PRI,
            MAT_SEC,
            NAMED,
            SET,
            ELEMENT,
            MISC,
            SUMMON_INFO,
            SCORE,
            RELIGIOUS_SCORE,
            CATEGORY,
            CalcIsMagical(CATEGORY, NAMED, SET, ELEMENT, MISC, SUMMON_INFO),
            CalcArmorRating(
                TEMPLATE_FINAL.IsArmor(), TEMPLATE_FINAL.GetEquipmentDetails(), MAT_PRI, MAT_SEC),
            CalculatePrice(TEMPLATE_FINAL, IS_PIXIE, MAT_PRI, MAT_SEC, SCORE),
            CalculateWeight(
                TEMPLATE_FINAL.GetEquipmentDetails(), IS_PIXIE, MAT_PRI, MAT_SEC, MISC));
    }

    const ItemProfile
        ItemProfileFactory::MakeBodyPartWeapon(const creature::CreaturePtr_t CREATURE_PTR)
    {
        item::BodyPartWeapons::Enum bodypartWeapon = item::BodyPartWeapons::Count;

        const auto BODY { CREATURE_PTR->Body() };

        if (BODY.IsHumanoid() && !CREATURE_PTR->IsPixie())
        {
            bodypartWeapon = item::BodyPartWeapons::Fists;
        }
        else if (BODY.HasClaws())
        {
            bodypartWeapon = item::BodyPartWeapons::Claws;
        }
        else if (BODY.HasTentacles())
        {
            bodypartWeapon = item::BodyPartWeapons::Tentacles;
        }
        else if (BODY.HasBite())
        {
            bodypartWeapon = item::BodyPartWeapons::Bite;
        }
        else if (BODY.HasBreath())
        {
            if (CREATURE_PTR->Role() == creature::role::Sylavin)
            {
                bodypartWeapon = item::BodyPartWeapons::BreathOfFrost;
            }
            else if (CREATURE_PTR->Role() == creature::role::Firebrand)
            {
                bodypartWeapon = item::BodyPartWeapons::BreathOfFire;
            }
        }

        if (bodypartWeapon < item::BodyPartWeapons::Count)
        {
            const auto MAT_PAIR
                = item::MaterialFactory::Instance()->MakePairForBodyPartWeapon(bodypartWeapon);

            return Make(bodypartWeapon, CREATURE_PTR->IsPixie(), MAT_PAIR.first, MAT_PAIR.second);
        }
        else
        {
            return ItemProfile();
        }
    }

    const ItemProfile ItemProfileFactory::MakeSkinArmor(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto MAT_PAIR
            = item::MaterialFactory::Instance()->MakePairForSkinArmor(CREATURE_PTR->Race());

        Skins::Enum skinType = Skins::Count;

        if (MAT_PAIR.first == Material::Fur)
        {
            skinType = Skins::Fur;
        }
        else if (MAT_PAIR.first == Material::Hide)
        {
            skinType = Skins::Hide;
        }
        else if (MAT_PAIR.first == Material::Plant)
        {
            skinType = Skins::Plant;
        }
        else if (MAT_PAIR.first == Material::Scales)
        {
            skinType = Skins::Scale;
        }

        if (skinType < Skins::Count)
        {
            return Make(Skins::Hide, CREATURE_PTR->IsPixie(), MAT_PAIR.first, MAT_PAIR.second);
        }
        else
        {
            return ItemProfile();
        }
    }

    Score_t ItemProfileFactory::CalcScore(
        const ItemTemplate & TEMPLATE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const Element::Enum ELEMENT,
        const Misc::Enum MISC,
        const creature::SummonInfo & SUMMON_INFO)
    {
        Score_t result = 0_score;

        const auto IS_WEAPON = TEMPLATE.IsWeapon();

        if (MISC < Misc::Count)
        {
            result += Material::Score(MAT_PRI, MAT_SEC);

            result += creature::EnchantmentFactory::Score(SET);

            // TODO this one appears to be broken and not doing anything
            // result += creature::EnchantmentFactory::Score(MISC, MAT_PRI, MAT_SEC);

            if (Misc::IsUnique(MISC))
            {
                result += 200_score;
            }

            if (ELEMENT != Element::None)
            {
                result += creature::EnchantmentFactory::Score(ELEMENT, IS_WEAPON, MAT_PRI, MAT_SEC);
            }
        }
        else
        {
            // This check for an invalid primary material is really a check if this call is
            // based on a template. If this is for a thin profile then the score is ignored
            // and the time consuming calls to EnchantmentFactory can be skipped.
            if (MAT_PRI < Material::Count)
            {
                result += TEMPLATE.GetEquipmentDetails().base_score;

                result += Material::Score(MAT_PRI, MAT_SEC);

                result += creature::EnchantmentFactory::Score(
                    NAMED, MAT_PRI, MAT_SEC, IS_WEAPON, !IS_WEAPON);

                result += creature::EnchantmentFactory::Score(SET);
                result += creature::EnchantmentFactory::Score(ELEMENT, IS_WEAPON, MAT_PRI, MAT_SEC);
            }
        }

        if (SUMMON_INFO.IsValid())
        {
            result += SUMMON_INFO.Score();
        }

        return result;
    }

    bool ItemProfileFactory::CalcIsMagical(
        const Category::Enum CATEGORY,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const Element::Enum ELEMENT,
        const Misc::Enum MISC,
        const creature::SummonInfo & SUMMON_INFO)
    {
        return (
            (NAMED < Named::Count) || (SET < Set::Count) || (ELEMENT != Element::None)
            || Misc::IsAlwaysMagical(MISC) || SUMMON_INFO.IsValid()
            || (CATEGORY & Category::ShowsEnemyInfo));
    }

    Category::Enum
        ItemProfileFactory::CalcCategory(const ItemTemplate & TEMPLATE, const Misc::Enum MISC)
    {
        Category::Enum result = Misc::CategoryAdditions(MISC);

        if (TEMPLATE.IsWeapon() || TEMPLATE.IsArmor())
        {
            result |= Category::Equipable;
            result |= TEMPLATE.GetEquipmentDetails().category;
        }

        return result;
    }

    Armor_t ItemProfileFactory::CalcArmorRating(
        const bool IS_ARMROR,
        const EquipmentDetails & EQUIPMENT_DETAILS,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC)
    {
        Armor_t armorRating = 0_armor;

        if (!IS_ARMROR)
        {
            return armorRating;
        }

        const auto NAME_TYPE = EQUIPMENT_DETAILS.material_name_type;

        if (NAME_TYPE == MaterialNameStyle::BodyPart)
        {
            // The armor rating of bodyparts (Skin) cannot be completely calculated without
            // knowing the creature's rank that it comes from.  So calc everything except for
            // that here.  The Creature class will add the extra armor rating determined by
            // rank.
            armorRating += Material::ArmorRatingBonus(MAT_PRI, MAT_SEC);
        }
        else
        {
            armorRating += EQUIPMENT_DETAILS.base_armor_rating;

            Material::Enum secondaryMaterialToUse = MAT_SEC;

            const bool WILL_EXCLUDE_SECONDARY_MATERIAL
                = ((NAME_TYPE == MaterialNameStyle::Clasped)
                   || (NAME_TYPE == MaterialNameStyle::ClaspedWithBase)
                   || (NAME_TYPE == MaterialNameStyle::Decoration));

            if (WILL_EXCLUDE_SECONDARY_MATERIAL)
            {
                secondaryMaterialToUse = Material::Count;
            }

            armorRating += Material::ArmorRatingBonus(MAT_PRI, secondaryMaterialToUse);
        }

        return armorRating;
    }

    Coin_t ItemProfileFactory::CalculatePrice(
        const ItemTemplate & TEMPLATE,
        const bool IS_PIXIE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC,
        const Score_t SCORE)
    {
        Coin_t price = 0_coin;

        if (TEMPLATE.IsWeapon())
        {
            price += TEMPLATE.GetEquipmentDetails().base_price;
        }

        if (TEMPLATE.IsArmor())
        {
            price += TEMPLATE.GetEquipmentDetails().base_price;
        }

        if (price <= 0_coin)
        {
            price = SCORE.GetAs<Coin_t>();
        }

        price += Material::PriceAdj(MAT_PRI, MAT_SEC);

        if (IS_PIXIE)
        {
            price.Scale(1.5f);
        }

        return price;
    }

    Weight_t ItemProfileFactory::CalculateWeight(
        const EquipmentDetails & EQUIPMENT_DETAILS,
        const bool IS_PIXIE,
        const Material::Enum MAT_PRI,
        const Material::Enum MAT_SEC,
        const Misc::Enum MISC)
    {
        Weight_t weight = EQUIPMENT_DETAILS.base_weight;

        if (weight <= 0_weight)
        {
            weight += Misc::Weight(MISC);
        }

        weight.Scale(Material::WeightMult(MAT_PRI, MAT_SEC));

        if (IS_PIXIE)
        {
            weight.Scale(0.004f); // divide by 250
        }

        if (weight < 1_weight)
        {
            weight = 1_weight;
        }

        return weight;
    }

    const ItemTemplate
        ItemProfileFactory::SwitchToEquipmentTemplateIfNeeded(const ItemTemplate & TEMPLATE)
    {
        if (TEMPLATE.IsMisc())
        {
            const auto MISC = TEMPLATE.MiscTypeBase();

            if ((MISC == Misc::CapeCommanders) || (MISC == Misc::CapeGenerals)
                || (MISC == Misc::CapeKings))
            {
                return ItemTemplateFactory::Instance()->MakeFromSubType(Covers::Cape);
            }

            if ((MISC == Misc::ShadeCloak) || (MISC == Misc::GhostSheet)
                || (MISC == Misc::BurialShroud))
            {
                return ItemTemplateFactory::Instance()->MakeFromSubType(Covers::Cloak);
            }

            if (MISC == Misc::SpecterRobe)
            {
                return ItemTemplateFactory::Instance()->MakeFromSubType(Covers::Robe);
            }

            if (MISC == Misc::ReaperScythe)
            {
                return ItemTemplateFactory::Instance()->MakeFromSubType(Bladedstaffs::Scythe);
            }
        }

        return TEMPLATE;
    }

} // namespace item
} // namespace heroespath
