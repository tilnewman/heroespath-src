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
// treasure-factory.hpp
//
#include "treasure-factory.hpp"


namespace game
{
namespace combat
{

    int TreasureFactory::Score(const stats::TraitSet & TRAIT_SET)
    {
        using namespace stats;

        int score{ 0 };

        for (int i(1); i < Traits::Count; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_TRAIT_VALUE{ TRAIT_SET.GetCopy(NEXT_TRAIT_ENUM).Current() };
            auto traitScore{ [NEXT_TRAIT_VALUE]()
            {
                if (NEXT_TRAIT_VALUE >= 0)
                {
                    return 10 * NEXT_TRAIT_VALUE;
                }
                else
                {
                    return 5 * std::abs(NEXT_TRAIT_VALUE);
                }
            }() };

            if ((NEXT_TRAIT_ENUM == Traits::HealthGainAll) ||
                (NEXT_TRAIT_ENUM == Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if ((NEXT_TRAIT_ENUM == Traits::AnimalResist) ||
                     (NEXT_TRAIT_ENUM == Traits::ArmorRating) ||
                     (NEXT_TRAIT_ENUM == Traits::Backstab) ||
                     (NEXT_TRAIT_ENUM == Traits::CurseOnDamage) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusAll) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusMelee) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusProj) ||
                     (NEXT_TRAIT_ENUM == Traits::FindCoinsAmount) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnAll) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += traitScore;
        }

        return score;
    }


    int TreasureFactory::Score(const item::material::Enum PRI,
                               const item::material::Enum SEC)
    {
        auto const BONUS_PRI{ [PRI]()
            {
                if ((PRI == item::material::Nothing) ||
                    (PRI == item::material::Count))
                {
                    return 0;
                }
                else
                {
                    return item::material::Bonus(PRI);
                }
            }() };

        auto const BONUS_SEC{ [SEC]()
            {
                if ((SEC == item::material::Nothing) ||
                    (SEC == item::material::Count))
                {
                    return 0;
                }
                else
                {
                    return item::material::Bonus(SEC);
                }
            }() };

        return 50 + ((BONUS_PRI * BONUS_PRI) * 2) + ((BONUS_SEC * BONUS_SEC) / 2);
    }


    int TreasureFactory::Score(const item::weapon::axe_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case axe_type::Handaxe:   { return 50; }
            case axe_type::Sickle:    { return 80; }
            case axe_type::Battleaxe: { return 100; }
            case axe_type::Waraxe:    { return 200; }
            case axe_type::Count:
            default:                  { return 0; }
        }
    }


    int TreasureFactory::Score(const item::weapon::bladedstaff_type::Enum E)
    {
        using namespace item::weapon;

        switch (E)
        {
            case bladedstaff_type::Spear:      { return 70; }
            case bladedstaff_type::ShortSpear: { return 50; }
            case bladedstaff_type::Scythe:     { return 80; }
            case bladedstaff_type::Pike:       { return 100; }
            case bladedstaff_type::Partisan:   { return 150; }
            case bladedstaff_type::Halberd:    { return 200; }
            case bladedstaff_type::Count:
            default:                           { return 0; }
        }
    }


    int TreasureFactory::Score(const item::weapon::club_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case club_type::Spiked:    { return 50; }
            case club_type::Maul:      { return 60; }
            case club_type::Mace:      { return 80; }
            case club_type::Warhammer: { return 120; }
            case club_type::Count:
            default:                   { return 0; }
        }
    }


    int TreasureFactory::Score(const item::weapon::projectile_type::Enum E)
    {
        using namespace item::weapon;

        switch (E)
        {
            case projectile_type::Blowpipe:     { return 50; }
            case projectile_type::Sling:        { return 60; }
            case projectile_type::Shortbow:     { return 100; }
            case projectile_type::Longbow:      { return 200; }
            case projectile_type::CompositeBow: { return 300; }
            case projectile_type::Crossbow:     { return 300; }
            case projectile_type::Count:
            default:                            { return 0; }
        }
    }


    int TreasureFactory::Score(const item::weapon::sword_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case sword_type::Claymore:      { return 200; }
            case sword_type::Longsword:     { return 120; }
            case sword_type::Flamberg:      { return 190; }
            case sword_type::Knightlysword: { return 110; }
            case sword_type::Broadsword:    { return 100; }
            case sword_type::Falcata:       { return 50; }
            case sword_type::Saber:         { return 70; }
            case sword_type::Cutlass:       { return 40; }
            case sword_type::Rapier:        { return 70; }
            case sword_type::Gladius:       { return 55; }
            case sword_type::Shortsword:    { return 50; }
            case sword_type::Count:
            default:                        { return 0; }
        }
    }


    int TreasureFactory::Score(const item::weapon::whip_type::Enum E)
    {
        using namespace item::weapon;

        switch(E)
        {
            case whip_type::Bullwhip:     { return 30; }
            case whip_type::Flail:        { return 60; }
            case whip_type::MaceAndChain: { return 90; }
            case whip_type::Count:
            default:                      { return 0; }
        }
    }


    int TreasureFactory::ScoreKnife(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 20;
        }

        return 25;
    }


    int TreasureFactory::ScoreDagger(const sfml_util::Size::Enum E)
    {
        if (E == sfml_util::Size::Small)
        {
            return 30;
        }

        return 35;
    }


    int TreasureFactory::ScoreStaff()
    {
        return 25;
    }


    int TreasureFactory::ScoreQuarterStaff()
    {
        return 15;
    }


    int TreasureFactory::Score(const item::armor::base_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case base_type::Mail:  { return 75; }
            case base_type::Scale: { return 125; }
            case base_type::Plate: { return 175; }
            case base_type::Plain:
            case base_type::Count:
            default:               { return 25; }
        }
    }


    int TreasureFactory::Score(const item::armor::cover_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case cover_type::Cape:  { return 25; }
            case cover_type::Vest:  { return 50; }
            case cover_type::Robe:  { return 75; }
            case cover_type::Cloak: { return 100; }
            case cover_type::Count:
            default:                { return 0; }
        }
    }


    int TreasureFactory::Score(const item::armor::helm_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case helm_type::Leather:  { return 25; }
            case helm_type::MailCoif: { return 50; }
            case helm_type::Kettle:   { return 75; }
            case helm_type::Archers:  { return 100; }
            case helm_type::Bascinet: { return 125; }
            case helm_type::Great:    { return 150; }
            case helm_type::Count:
            default:                  { return 0; }
        }
    }


    int TreasureFactory::Score(const item::armor::shield_type::Enum E)
    {
        using namespace item::armor;

        switch(E)
        {
            case shield_type::Buckler: { return 50; }
            case shield_type::Kite:    { return 100; }
            case shield_type::Heater:  { return 150; }
            case shield_type::Pavis:   { return 200; }
            case shield_type::Count:
            default:                   { return 0; }
        }
    }


    int TreasureFactory::ScoreAventail(item::armor::base_type::Enum E)
    {
        return 35 + Score(E);
    }


    int TreasureFactory::ScoreBracer(item::armor::base_type::Enum E)
    {
        return 35 + Score(E);
    }


    int TreasureFactory::ScoreShirt(item::armor::base_type::Enum E)
    {
        return 45 + Score(E);
    }


    int TreasureFactory::ScoreBoots(item::armor::base_type::Enum E)
    {
        return 45 + Score(E);
    }


    int TreasureFactory::ScorePants(item::armor::base_type::Enum E)
    {
        return 55 + Score(E);
    }


    int TreasureFactory::ScoreGauntlets(item::armor::base_type::Enum E)
    {
        return 30 + Score(E);
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(const item::weapon::axe_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(
        const item::weapon::bladedstaff_type::Enum)
    {
        return item::MaterialVecPair_t({ item::material::Wood }, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(const item::weapon::club_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(
        const item::weapon::projectile_type::Enum E)
    {
        using namespace item;
        using namespace item::weapon;

        switch (E)
        {
            case projectile_type::Blowpipe:
            {
                return MaterialVecPair_t({ material::Wood,
                                           material::Stone,
                                           material::Bone,
                                           material::Obsidian },
                                         material::CoreSecondary());
            }

            case projectile_type::Sling:
            {
                return MaterialVecPair_t({ material::Rope }, material::CoreSecondary());
            }
            case projectile_type::Shortbow:
            case projectile_type::Longbow:
            {
                return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
            }
            case projectile_type::CompositeBow:
            {
                return MaterialVecPair_t({ material::Horn }, material::CoreSecondary());
            }
            case projectile_type::Crossbow:
            {
                return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
            }
            case projectile_type::Count:
            default:
            {
                return MaterialVecPair_t();
            }
        }
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(
        const item::weapon::sword_type::Enum)
    {
        return item::material::CoreMetalAndCoreSecondary();
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(
        const item::weapon::whip_type::Enum E)
    {
        using namespace item;
        using namespace item::weapon;

        if (E == whip_type::Bullwhip)
        {
            return MaterialVecPair_t({ material::SoftLeather }, material::CoreSecondary());
        }
        else
        {
            return material::CoreMetalAndCoreSecondary();
        }
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsKnife()
    {
        return item::material::CorePrimaryNoPearlAndSecondary();
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsDagger()
    {
        return MaterialsKnife();
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsStaff()
    {
        return item::MaterialVecPair_t({ item::material::Wood }, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsQuarterStaff()
    {
        return MaterialsStaff();
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(const item::armor::cover_type::Enum)
    {
        return item::MaterialVecPair_t({ item::material::Cloth,
                                         item::material::SoftLeather,
                                         item::material::HardLeather},
                                       item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(const item::armor::helm_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        if (E == helm_type::Leather)
        {
            return item::MaterialVecPair_t({ material::HardLeather }, material::CoreSecondary());
        }
        else
        {
            return item::MaterialVecPair_t(material::CoreMetal(), material::CoreSecondary());
        }
    }


    const item::MaterialVecPair_t TreasureFactory::Materials(
        const item::armor::shield_type::Enum)
    {
        item::MaterialVec_t v{ item::material::CoreMetal() };
        v.push_back(item::material::Wood);
        return item::MaterialVecPair_t(v, item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsAventail(
        item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsBracer(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsShirt(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsBoots(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseTypeNoCloth(E);
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsPants(item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsGauntlets(
        item::armor::base_type::Enum E)
    {
        return MaterialsFromBaseType(E);
    }


    const item::MaterialVec_t TreasureFactory::MaterialsPrimaryFromArmorBaseType(
        const item::armor::base_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        switch(E)
        {
            case base_type::Scale: { return { material::Scale }; }
            case base_type::Mail:
            case base_type::Plate: { return { material::CoreMetal() }; }
            case base_type::Plain: { return { material::Cloth,
                                              material::SoftLeather,
                                              material::HardLeather }; }
            case base_type::Count:
            default:               { return {}; }
        }
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsFromBaseType(
        const item::armor::base_type::Enum E)
    {
        return item::MaterialVecPair_t(MaterialsPrimaryFromArmorBaseType(E),
            item::material::CoreSecondary());
    }


    const item::MaterialVecPair_t TreasureFactory::MaterialsFromBaseTypeNoCloth(
        const item::armor::base_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;

        item::MaterialVec_t v;

        if (E == base_type::Plain)
        {
            v.push_back(material::SoftLeather);
            v.push_back(material::HardLeather);
        }
        else
        {
            v = MaterialsPrimaryFromArmorBaseType(E);
        }

        return item::MaterialVecPair_t(v, item::material::CoreSecondary());
    }

}
}
