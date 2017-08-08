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


    const item::ItemProfileVec_t TreasureFactory::ThinProfilesWeaponsSwords()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        for (int i(0); i < sword_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetSword( static_cast<sword_type::Enum>(i) );
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t TreasureFactory::ThinProfilesWeaponsProjectiles()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        for (int i(0); i < projectile_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetProjectile( static_cast<projectile_type::Enum>(i) );
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t TreasureFactory::ThinProfilesWeaponsAll()
    {
        using namespace item;
        using namespace item::weapon;

        ItemProfileVec_t v;

        auto const SWORD_THIN_PROFILES_VEC{ ThinProfilesWeaponsSwords() };

        std::copy(SWORD_THIN_PROFILES_VEC.begin(),
                  SWORD_THIN_PROFILES_VEC.end(),
                  std::back_inserter(v));

        auto const PROJ_THIN_PROFILES_VEC{ ThinProfilesWeaponsProjectiles() };

        std::copy(PROJ_THIN_PROFILES_VEC.begin(),
                  PROJ_THIN_PROFILES_VEC.end(),
                  std::back_inserter(v));

        for (int i(0); i < axe_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetAxe(static_cast<axe_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < bladedstaff_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetBladedStaff(static_cast<bladedstaff_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < club_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetClub(static_cast<club_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < whip_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetWhip(static_cast<whip_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetKnife(static_cast<sfml_util::Size::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < sfml_util::Size::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetDagger(static_cast<sfml_util::Size::Enum>(i));
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetStaff(material::Nothing);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetQuarterStaff(material::Nothing);
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t TreasureFactory::ThinProfilesArmor(
        const bool WILL_INCLUDE_COVERINGS)
    {
        using namespace item;
        using namespace item::armor;

        ItemProfileVec_t v;

        if (WILL_INCLUDE_COVERINGS)
        {
            for (int i(0); i < cover_type::Count; ++i)
            {
                item::ItemProfile thinProfile;
                thinProfile.SetCover(static_cast<cover_type::Enum>(i));
                v.push_back(thinProfile);
            }
        }

        for (int i(0); i < helm_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetHelm(static_cast<helm_type::Enum>(i));
            v.push_back(thinProfile);
        }

        for (int i(0); i < shield_type::Count; ++i)
        {
            item::ItemProfile thinProfile;
            thinProfile.SetShield(static_cast<shield_type::Enum>(i));
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetAventail(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetBoots(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetBracer(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetGauntlets(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetPants(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetShirt(base_type::Count);
            v.push_back(thinProfile);
        }

        {
            item::ItemProfile thinProfile;
            thinProfile.SetGauntlets(base_type::Count);
            v.push_back(thinProfile);
        }

        return v;
    }


    const item::ItemProfileVec_t TreasureFactory::ThinProfiles(const item::named_type::Enum E)
    {
        using namespace item;
        using namespace item::armor;
        using namespace item::weapon;

        switch (E)
        {
            case item::named_type::Wicked:
            case item::named_type::Fiendish:
            case item::named_type::Infernal:
            case item::named_type::Raging:
            case item::named_type::Diabolic:
            case item::named_type::Searing:
            case item::named_type::Burning:
            case item::named_type::Fiery:
            case item::named_type::Icy:
            case item::named_type::Winter:
            case item::named_type::Frigid:
            case item::named_type::Proud:
            case item::named_type::Glory:
            case item::named_type::Pure:
            case item::named_type::Gloom:
            case item::named_type::Twilight:
            case item::named_type::Dusk:
            case item::named_type::Dark:
            case item::named_type::Betrayer:
            {
                return ThinProfilesWeaponsAll();
            }

            case item::named_type::Dancing:
            case item::named_type::Marauder:
            {
                return ThinProfilesWeaponsSwords();
            }

            case item::named_type::Honest:
            case item::named_type::Noble:
            case item::named_type::Daring:
            case item::named_type::Elite:
            case item::named_type::Valiant:
            case item::named_type::Heros:
            case item::named_type::Light:
            case item::named_type::Dawn:
            case item::named_type::Sun:
            case item::named_type::Sorrow:
            case item::named_type::Woe:
            case item::named_type::Misery:
            case item::named_type::Moon:
            {
                return ThinProfilesArmor(true);
            }

            case item::named_type::Army:
            case item::named_type::Charred:
            case item::named_type::Chill:
            case item::named_type::Frozen:
            case item::named_type::Arctic:
            {
                return ThinProfilesArmor(false);
            }

            case item::named_type::Gladiator:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Kettle);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAxe(axe_type::Handaxe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBladedStaff(bladedstaff_type::Spear);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Gladius);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Soldiers:
            {
                item::ItemProfileVec_t v;

                auto const THINPROFILES_WEAPONS_VEC{ ThinProfilesWeaponsAll() };
                auto const THINPROFILES_ARMOR_VEC{ ThinProfilesArmor(false) };

                std::copy(THINPROFILES_WEAPONS_VEC.begin(),
                          THINPROFILES_WEAPONS_VEC.end(),
                          std::back_inserter(v));
                
                std::copy(THINPROFILES_ARMOR_VEC.begin(),
                          THINPROFILES_ARMOR_VEC.end(),
                          std::back_inserter(v));

                return v;
            }

            case item::named_type::Wardens:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }
                
                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Shortsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Bascinet);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Princes:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Mail);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Rapier);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::MailCoif);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Ranger:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Plain);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetProjectile(projectile_type::Longbow);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Samurai:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                for (auto nextThinProfileArmor : v)
                {
                    nextThinProfileArmor.SetArmorTypeRestriction(armor::base_type::Scale);
                }

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Longsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cape);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Thors:
            {
                ItemProfileVec_t v{ ThinProfilesArmor(false) };

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Sheild); } ), v.end());

                v.erase(std::remove_if(v.begin(), v.end(),
                    []
                    (const ItemProfile & P)
                    { return (P.ArmorType() & armor_type::Helm); } ), v.end());

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Broadsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Pavis);
                    v.push_back(thinProfile);
                }

                return v;
            }
            case item::named_type::Nile:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(sword_type::Falcata);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(shield_type::Buckler);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(helm_type::Leather);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Imposters:
            case item::named_type::Pickpocket:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Robe);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Burglar:
            case item::named_type::Mountebank:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Charlatans:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(cover_type::Vest);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Robbers:
            case item::named_type::Thugs:
            case item::named_type::Knaves:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetKnife();
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Muggers:
            case item::named_type::Thief:
            case item::named_type::Pirate:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.push_back(thinProfile);
                }

                return v;
            }

            case item::named_type::Focus:
            {
                return ThinProfilesWeaponsProjectiles();
            }

            case item::named_type::NotNamed:
            case item::named_type::Count:
            default:
            {
                return {};
            }
        }
    }


    const item::ItemProfileVec_t TreasureFactory::ThinProfiles(const item::set_type::Enum E)
    {
        using namespace item;

        switch (E)
        {
            case set_type::TheAssassins:
            case set_type::TheTickler:
            case set_type::TheNeverwinter:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cloak);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetDagger();
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::LockPicks);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheLichKings:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Litch_Hand);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Wand);
                    v.push_back(thinProfile);
                }
                
                return v;
            }

            case set_type::TheAngelic:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Robe);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::Braid);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheBalladeers:
            case set_type::TheTroubadours:
            case set_type::TheMuses:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetSword(weapon::sword_type::Longsword);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetMisc(misc_type::DrumLute);
                    v.push_back(thinProfile);
                }
                
                return v;
            }

            case set_type::TheCavaliers:
            case set_type::TheChampions:
            case set_type::ThePaladins:
            case set_type::TheBerserkers:
            case set_type::TheRosewood:
            case set_type::TheDragonslayers:
            case set_type::TheEventideRider:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Cape);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Pavis);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Great);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetAventail();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plate);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheHunters:
            case set_type::TheSureShot:
            case set_type::TheMarksmans:
            case set_type::TheDeadeye:
            case set_type::TheDuskRanger:
            case set_type::TheVenomBow:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Plain);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Mail);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetCover(armor::cover_type::Vest);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Archers);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::TheCritterChannelers:
            case set_type::TheMammalianHead:
            case set_type::TheSavageTaskmasters:
            case set_type::TheMonsterOverseers:
            case set_type::TheBeastRulers:
            {
                ItemProfileVec_t v;

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBoots();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetPants();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShirt();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetBracer();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetGauntlets();
                    thinProfile.SetArmorTypeRestriction(armor::base_type::Scale);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetShield(armor::shield_type::Kite);
                    v.push_back(thinProfile);
                }

                {
                    ItemProfile thinProfile;
                    thinProfile.SetHelm(armor::helm_type::Bascinet);
                    v.push_back(thinProfile);
                }

                return v;
            }

            case set_type::NotASet:
            case set_type::Count:
            default:
            {
                return {};
            }
        }
    }

}
}
