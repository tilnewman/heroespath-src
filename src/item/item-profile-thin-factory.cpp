// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile-thin-factory.cpp
//
#include "item-profile-thin-factory.hpp"

#include "misc/vectors.hpp"

namespace heroespath
{
namespace item
{
    const ItemProfileThinVec_t ItemProfileThinFactory::MakeAllWeapons() const
    {
        ItemProfileThinVec_t thinProfiles;
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::sword_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::projectile_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::axe_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::bladedstaff_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::club_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponSpecificAll<weapon::whip_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponKnifeOrDaggerAll(false), thinProfiles);
        misc::Vector::AppendMove(MakeWeaponKnifeOrDaggerAll(true), thinProfiles);
        thinProfiles.emplace_back(MakeWeaponStaffOrQuarterstaff(false));
        thinProfiles.emplace_back(MakeWeaponStaffOrQuarterstaff(true));
        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileThinFactory::MakeAllArmor() const
    {
        ItemProfileThinVec_t thinProfiles;
        misc::Vector::AppendMove(MakeArmorSpecificAll<armor::cover_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeArmorSpecificAll<armor::helm_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeArmorSpecificAll<armor::shield_type>(), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Aventail), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);
        misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);
        return thinProfiles;
    }

    const ItemProfileThinVec_t
        ItemProfileThinFactory::MakeAllNamedTypes(const named_type::Enum NAMED_TYPE) const
    {
        using namespace armor;
        using namespace weapon;

        auto removeIf{ [](std::vector<ItemProfileThin> & thinProfiles, auto ifLambda) {
            thinProfiles.erase(
                std::remove_if(std::begin(thinProfiles), std::end(thinProfiles), ifLambda),
                std::end(thinProfiles));
        } };

        auto removeIfArmorType{ [&](std::vector<ItemProfileThin> & thinProfiles,
                                    const armor_type::Enum ARMOR_TYPE) {
            removeIf(thinProfiles, [&](auto const & PROFILE) {
                return (PROFILE.ArmorInfo().Type() == ARMOR_TYPE);
            });
        } };

        switch (NAMED_TYPE)
        {
            case named_type::Wicked:
            {
                return MakeWeaponOfTypeAll(weapon_type::Pointed);
            }
            case named_type::Fiendish:
            {
                return MakeWeaponOfTypeAll(weapon_type::Bladed);
            }

            case named_type::Infernal:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponSpecific(projectile_type::Longbow),
                                                   MakeWeaponSpecific(
                                                       projectile_type::CompositeBow),
                                                   MakeWeaponSpecific(projectile_type::Sling) };

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                return thinProfiles;
            }

            case named_type::Raging:
            {
                return MakeWeaponOfTypeAll(weapon_type::Club);
            }

            case named_type::Icy:
            case named_type::Winter:
            case named_type::Frigid:
            case named_type::Diabolic:
            {
                return MakeWeaponOfTypeAll(weapon_type::Melee);
            }

            case named_type::Dancing:
            {
                return MakeWeaponOfTypeAll(weapon_type::Sword);
            }

            case named_type::Marauder:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponOfTypeAll(weapon_type::Sword) };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.WeaponInfo().SwordType() == sword_type::Shortsword)
                        || (PROFILE.WeaponInfo().SwordType() == sword_type::Falcata)
                        || (PROFILE.WeaponInfo().SwordType() == sword_type::Gladius));
                });

                return thinProfiles;
            }

            case named_type::Wardens:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponSpecific(sword_type::Shortsword) };

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor::base_type::Plain), thinProfiles);

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(MakeArmorSpecific(helm_type::Bascinet));
                thinProfiles.emplace_back(MakeArmorSpecific(shield_type::Buckler));

                return thinProfiles;
            }

            case named_type::Princes:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll(
                    armor::base_type::Mail) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(MakeArmorSpecific(helm_type::MailCoif));
                thinProfiles.emplace_back(MakeArmorSpecific(shield_type::Kite));
                thinProfiles.emplace_back(MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(MakeWeaponSpecific(sword_type::Rapier));

                return thinProfiles;
            }

            case named_type::Ranger:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll(
                    armor::base_type::Plain) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(MakeArmorSpecific(cover_type::Cloak));
                thinProfiles.emplace_back(MakeArmorSpecific(cover_type::Vest));

                thinProfiles.emplace_back(MakeWeaponSpecific(projectile_type::Longbow));

                thinProfiles.emplace_back(MakeWeaponSpecific(projectile_type::Shortbow));

                thinProfiles.emplace_back(MakeWeaponSpecific(projectile_type::CompositeBow));

                thinProfiles.emplace_back(MakeWeaponSpecific(projectile_type::Sling));

                return thinProfiles;
            }

            case named_type::Samurai:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll(
                    armor::base_type::Scale) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(MakeArmorSpecific(helm_type::Leather));
                thinProfiles.emplace_back(MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(MakeWeaponSpecific(sword_type::Longsword));

                return thinProfiles;
            }

            case named_type::Thors:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll(
                    armor::base_type::Plate) };

                removeIfArmorType(thinProfiles, armor_type::Shield);
                removeIfArmorType(thinProfiles, armor_type::Helm);

                thinProfiles.emplace_back(MakeArmorSpecific(helm_type::Great));
                thinProfiles.emplace_back(MakeArmorSpecific(shield_type::Pavis));
                thinProfiles.emplace_back(MakeArmorSpecific(cover_type::Cape));

                thinProfiles.emplace_back(MakeWeaponSpecific(sword_type::Claymore));

                return thinProfiles;
            }

            case named_type::Nile:
            {
                return { MakeWeaponSpecific(sword_type::Falcata),
                         MakeArmorSpecific(shield_type::Buckler),
                         MakeArmorSpecific(shield_type::Kite),
                         MakeArmorSpecific(helm_type::Leather),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorSpecific(cover_type::Cloak) };
            }

            case named_type::Searing:
            {
                return MakeWeaponOfTypeAll(weapon_type::Bladed);
            }

            case named_type::Burning:
            {
                ItemProfileThinVec_t thinProfiles;

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Axe), thinProfiles);

                misc::Vector::AppendMove(
                    MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Club), thinProfiles);

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Staff), thinProfiles);

                return thinProfiles;
            }

            case named_type::Fiery:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponSpecific(
                                                       projectile_type::CompositeBow),
                                                   MakeWeaponSpecific(projectile_type::Longbow),
                                                   MakeWeaponSpecific(projectile_type::Shortbow),
                                                   MakeWeaponSpecific(projectile_type::Crossbow) };

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Whip), thinProfiles);

                misc::Vector::AppendMove(
                    MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Sword), thinProfiles);

                return thinProfiles;
            }

            case named_type::Soldiers:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponSpecific(projectile_type::Shortbow),
                                                   MakeWeaponSpecific(projectile_type::Longbow),
                                                   MakeWeaponSpecific(sword_type::Broadsword),
                                                   MakeWeaponSpecific(sword_type::Longsword),
                                                   MakeWeaponSpecific(sword_type::Knightlysword),
                                                   MakeWeaponSpecific(sword_type::Claymore),
                                                   MakeWeaponSpecific(axe_type::Battleaxe),
                                                   MakeWeaponSpecific(axe_type::Waraxe) };

                misc::Vector::AppendMove(
                    MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                misc::Vector::AppendMove(MakeArmorSpecificAll<shield_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorSpecificAll<helm_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plate)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Pavis));
                });

                return thinProfiles;
            }

            case named_type::Dark:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeWeaponSpecific(projectile_type::Sling),
                    MakeWeaponSpecific(projectile_type::Shortbow),
                    MakeWeaponSpecific(projectile_type::Longbow),
                    MakeWeaponSpecific(projectile_type::CompositeBow),
                    MakeWeaponSpecific(sword_type::Falcata),
                    MakeWeaponSpecific(sword_type::Rapier),
                    MakeWeaponSpecific(sword_type::Flamberg),
                    MakeWeaponSpecific(axe_type::Sickle),
                    MakeWeaponSpecific(axe_type::Battleaxe),
                    MakeWeaponSpecific(whip_type::Bullwhip),
                    MakeWeaponSpecific(whip_type::Flail),
                    MakeWeaponSpecific(whip_type::MaceAndChain),
                    MakeWeaponSpecific(club_type::Spiked),
                    MakeWeaponSpecific(club_type::Maul),
                    MakeWeaponSpecific(bladedstaff_type::Spear),
                    MakeWeaponSpecific(bladedstaff_type::Pike),
                    MakeWeaponSpecific(bladedstaff_type::Scythe)
                };

                misc::Vector::AppendMove(MakeWeaponKnifeOrDaggerAll(false), thinProfiles);
                misc::Vector::AppendMove(MakeWeaponKnifeOrDaggerAll(true), thinProfiles);

                return thinProfiles;
            }

            case named_type::Betrayer:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeWeaponSpecific(projectile_type::Longbow),
                    MakeWeaponSpecific(projectile_type::CompositeBow),
                    MakeWeaponSpecific(sword_type::Falcata),
                    MakeWeaponSpecific(sword_type::Flamberg),
                    MakeWeaponSpecific(axe_type::Sickle),
                    MakeWeaponSpecific(whip_type::MaceAndChain),
                    MakeWeaponSpecific(club_type::Spiked),
                    MakeWeaponSpecific(bladedstaff_type::Scythe)
                };

                misc::Vector::AppendMove(MakeWeaponKnifeOrDaggerAll(true), thinProfiles);

                return thinProfiles;
            }

            case named_type::Robbers:
            case named_type::Thugs:
            case named_type::Knaves:
            {
                return { MakeWeaponKnifeOrDagger(false) };
            }

            case named_type::Thief:
            case named_type::Muggers:
            case named_type::Pirate:
            {
                return { MakeWeaponKnifeOrDagger(true) };
            }

            case named_type::Focus:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponOfTypeAll(weapon_type::Projectile) };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return PROFILE.WeaponInfo().ProjectileType() == projectile_type::Blowpipe;
                });

                return thinProfiles;
            }

            case named_type::Proud:
            case named_type::Glory:
            case named_type::Pure:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponSpecific(projectile_type::Longbow),
                                                   MakeWeaponSpecific(projectile_type::Crossbow),
                                                   MakeWeaponSpecific(
                                                       projectile_type::CompositeBow),
                                                   MakeWeaponSpecific(sword_type::Broadsword),
                                                   MakeWeaponSpecific(sword_type::Longsword),
                                                   MakeWeaponSpecific(sword_type::Knightlysword),
                                                   MakeWeaponSpecific(sword_type::Claymore),
                                                   MakeWeaponSpecific(axe_type::Battleaxe),
                                                   MakeWeaponSpecific(axe_type::Waraxe),
                                                   MakeWeaponSpecific(whip_type::MaceAndChain),
                                                   MakeWeaponSpecific(club_type::Warhammer) };

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Staff), thinProfiles);

                misc::Vector::AppendMove(
                    MakeWeaponOfTypeAll(weapon_type::BladedStaff), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return PROFILE.WeaponInfo().IsSpear();
                });

                return thinProfiles;
            }

            case named_type::Gloom:
            case named_type::Twilight:
            case named_type::Dusk:
            {
                ItemProfileThinVec_t thinProfiles{ MakeWeaponKnifeOrDagger(true),
                                                   MakeWeaponKnifeOrDagger(false),
                                                   MakeWeaponSpecific(sword_type::Cutlass),
                                                   MakeWeaponSpecific(sword_type::Falcata),
                                                   MakeWeaponSpecific(sword_type::Rapier),
                                                   MakeWeaponSpecific(sword_type::Saber),
                                                   MakeWeaponSpecific(sword_type::Flamberg),
                                                   MakeWeaponSpecific(axe_type::Handaxe),
                                                   MakeWeaponSpecific(axe_type::Sickle),
                                                   MakeWeaponSpecific(axe_type::Battleaxe),
                                                   MakeWeaponSpecific(axe_type::Waraxe),
                                                   MakeWeaponSpecific(club_type::Spiked),
                                                   MakeWeaponSpecific(club_type::Maul),
                                                   MakeWeaponSpecific(bladedstaff_type::ShortSpear),
                                                   MakeWeaponSpecific(bladedstaff_type::Spear),
                                                   MakeWeaponSpecific(bladedstaff_type::Pike),
                                                   MakeWeaponSpecific(bladedstaff_type::Scythe) };

                misc::Vector::AppendMove(
                    MakeWeaponOfTypeAll(weapon_type::Projectile), thinProfiles);

                misc::Vector::AppendMove(MakeWeaponOfTypeAll(weapon_type::Whip), thinProfiles);

                return thinProfiles;
            }

            case named_type::Honest:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe);
                });

                return thinProfiles;
            }

            case named_type::Noble:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Daring:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather);
                });

                return thinProfiles;
            }

            case named_type::Elite:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll() };

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().CoverType() == cover_type::Cloak)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Robe)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Scale)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Kettle);
                });

                return thinProfiles;
            }

            case named_type::Valiant:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeArmorSpecific(shield_type::Heater), MakeArmorSpecific(shield_type::Pavis),
                    MakeArmorSpecific(helm_type::Bascinet), MakeArmorSpecific(helm_type::Great),
                    MakeArmorSpecific(cover_type::Cape),    MakeArmorSpecific(cover_type::Vest)
                };

                misc::Vector::AppendMove(MakeArmorNonSpecificAll(base_type::Plate), thinProfiles);

                misc::Vector::AppendMove(MakeArmorNonSpecificAll(base_type::Mail), thinProfiles);

                return thinProfiles;
            }

            case named_type::Heros:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorSpecific(shield_type::Pavis),
                                                   MakeArmorSpecific(helm_type::Great),
                                                   MakeArmorSpecific(cover_type::Cape),
                                                   MakeArmorSpecific(cover_type::Vest) };

                misc::Vector::AppendMove(MakeArmorNonSpecificAll(base_type::Plate), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().Type() == armor_type::Bracers)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Aventail));
                });

                return thinProfiles;
            }

            case named_type::Army:
            {
                ItemProfileThinVec_t thinProfiles{};

                misc::Vector::AppendMove(MakeArmorSpecificAll<shield_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorSpecificAll<helm_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plate)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Pavis));
                });

                return thinProfiles;
            }

            case named_type::Gladiator:
            {
                return { MakeArmorSpecific(shield_type::Buckler),
                         MakeArmorSpecific(shield_type::Kite),
                         MakeArmorSpecific(helm_type::Great),
                         MakeArmorSpecific(helm_type::Bascinet),
                         MakeArmorSpecific(helm_type::Kettle),
                         MakeWeaponSpecific(axe_type::Handaxe),
                         MakeWeaponSpecific(bladedstaff_type::ShortSpear),
                         MakeWeaponSpecific(bladedstaff_type::Spear),
                         MakeWeaponSpecific(sword_type::Gladius),
                         MakeWeaponSpecific(sword_type::Shortsword),
                         MakeWeaponSpecific(sword_type::Broadsword) };
            }

            case named_type::Charred:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeArmorSpecific(cover_type::Vest),
                };

                misc::Vector::AppendMove(MakeArmorSpecificAll<shield_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorSpecificAll<helm_type>(), thinProfiles);

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().BaseType() != base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Moon:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorSpecific(cover_type::Robe),
                                                   MakeArmorSpecific(cover_type::Cloak),
                                                   MakeArmorSpecific(cover_type::Vest) };

                misc::Vector::AppendMove(MakeArmorSpecificAll<shield_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorSpecificAll<helm_type>(), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Kettle)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather));
                });

                return thinProfiles;
            }

            case named_type::Imposters:
            case named_type::Pickpocket:
            {
                return { MakeArmorSpecific(cover_type::Robe) };
            }

            case named_type::Chill:
            case named_type::Frozen:
            case named_type::Arctic:
            {
                ItemProfileThinVec_t thinProfiles{ MakeArmorNonSpecificAll() };

                misc::Vector::AppendMove(MakeArmorSpecificAll<cover_type>(), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().Type() == armor_type::Pants)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Boots)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Shirt)
                        || (PROFILE.ArmorInfo().Type() == armor_type::Aventail)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::MailCoif)
                        || (PROFILE.ArmorInfo().HelmType() == helm_type::Leather)
                        || (PROFILE.ArmorInfo().CoverType() == cover_type::Cape)
                        || (PROFILE.ArmorInfo().ShieldType() == shield_type::Buckler));
                });

                return thinProfiles;
            }

            case named_type::Light:
            case named_type::Dawn:
            case named_type::Sun:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeArmorSpecific(cover_type::Robe),    MakeArmorSpecific(cover_type::Cape),
                    MakeArmorSpecific(cover_type::Vest),    MakeArmorSpecific(shield_type::Kite),
                    MakeArmorSpecific(shield_type::Heater), MakeArmorSpecific(shield_type::Pavis),
                    MakeArmorSpecific(helm_type::Archers),  MakeArmorSpecific(helm_type::Bascinet),
                    MakeArmorSpecific(helm_type::Great),
                };

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Pants), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Boots), thinProfiles);
                misc::Vector::AppendMove(MakeArmorNonSpecificAll(armor_type::Shirt), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (
                        (PROFILE.ArmorInfo().BaseType() == base_type::Plain)
                        || (PROFILE.ArmorInfo().BaseType() == base_type::Scale));
                });

                return thinProfiles;
            }

            case named_type::Sorrow:
            case named_type::Woe:
            case named_type::Misery:
            {
                ItemProfileThinVec_t thinProfiles{
                    MakeArmorSpecific(cover_type::Cloak),    MakeArmorSpecific(cover_type::Vest),
                    MakeArmorSpecific(shield_type::Buckler), MakeArmorSpecific(shield_type::Kite),
                    MakeArmorSpecific(shield_type::Heater),  MakeArmorSpecific(helm_type::Leather),
                    MakeArmorSpecific(helm_type::MailCoif),  MakeArmorSpecific(helm_type::Archers),
                    MakeArmorSpecific(helm_type::Bascinet),
                };

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor_type::Gauntlets), thinProfiles);

                misc::Vector::AppendMove(
                    MakeArmorNonSpecificAll(armor_type::Bracers), thinProfiles);

                removeIf(thinProfiles, [](auto const & PROFILE) {
                    return (PROFILE.ArmorInfo().BaseType() == base_type::Plain);
                });

                return thinProfiles;
            }

            case named_type::Charlatans:
            {
                return { MakeArmorSpecific(cover_type::Vest) };
            }

            case named_type::Burglar:
            case named_type::Mountebank:
            {
                return { MakeArmorSpecific(cover_type::Cloak) };
            }

            case named_type::Not:
            case named_type::Count:
            default:
            {
                return {};
            }
        }
    }

    const ItemProfileThinVec_t
        ItemProfileThinFactory::MakeAllSetTypes(const set_type::Enum SET_TYPE) const
    {
        switch (SET_TYPE)
        {
            case set_type::TheAssassins:
            case set_type::TheTickler:
            case set_type::TheNeverwinter:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Cloak),
                         MakeWeaponKnifeOrDagger(true),
                         MakeMisc(misc_type::LockPicks) };
            }

            case set_type::TheMagus:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Wand) };
            }
            case set_type::TheNecromancers:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Orb) };
            }
            case set_type::TheWarlocks:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Scepter) };
            }

            case set_type::TheLichKings:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::LitchHand) };
            }

            case set_type::TheSages:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Shard) };
            }
            case set_type::TheShamans:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Orb) };
            }
            case set_type::TheOracles:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::Scepter) };
            }
            case set_type::TheAngelic:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Robe),
                         MakeMisc(misc_type::AngelBraid) };
            }

            case set_type::TheBalladeers:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest),
                         MakeWeaponSpecific(weapon::sword_type::Cutlass),
                         MakeMisc(misc_type::DrumLute) };
            }
            case set_type::TheTroubadours:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest),
                         MakeWeaponSpecific(weapon::sword_type::Rapier),
                         MakeMisc(misc_type::DrumLute) };
            }
            case set_type::TheMuses:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest),
                         MakeWeaponSpecific(weapon::sword_type::Longsword),
                         MakeMisc(misc_type::DrumLute) };
            }

            case set_type::TheCavaliers:
            {
                return { // Plain=Cloth
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Cape),

                         // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),

                         MakeArmorSpecific(armor::shield_type::Buckler),
                         MakeArmorSpecific(armor::helm_type::Leather)
                };
            }
            case set_type::TheChampions:
            {
                return { // Plain=Silk
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Cape),

                         // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),

                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::MailCoif)
                };
            }
            case set_type::ThePaladins:
            {
                return { // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Cape),

                         // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),

                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::Kettle)
                };
            }
            case set_type::TheBerserkers:
            {
                return { // Plain=Fur
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Cape),

                         // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Plain),

                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::Bascinet)
                };
            }
            case set_type::TheRosewood:
            {
                return { // Mail=Silver
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Mail),

                         // Silk
                         MakeArmorSpecific(armor::cover_type::Cape),

                         MakeArmorSpecific(armor::shield_type::Heater),
                         MakeArmorSpecific(armor::helm_type::Bascinet)
                };
            }
            case set_type::TheDragonslayers:
            {
                return { MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Scale),

                         // Leather
                         MakeArmorSpecific(armor::cover_type::Cape),

                         MakeArmorSpecific(armor::shield_type::Heater),
                         MakeArmorSpecific(armor::helm_type::Bascinet) };
            }
            case set_type::TheEventideRider:
            {
                return { // Plate=Platinum
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plate),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plate),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plate),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plate),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plate),
                         MakeArmorNonSpecific(armor_type::Aventail, armor::base_type::Plate),

                         // Fur
                         MakeArmorSpecific(armor::cover_type::Cape),

                         MakeArmorSpecific(armor::shield_type::Pavis),
                         MakeArmorSpecific(armor::helm_type::Great)
                };
            }

            case set_type::TheHunters:
            {
                return { // Plain=Silk
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest),

                         // Leather
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorSpecific(armor::helm_type::Leather)
                };
            }
            case set_type::TheSureShot:
            {
                return { // Plain=Leather
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest),
                         MakeArmorSpecific(armor::helm_type::Leather)
                };
            }
            case set_type::TheMarksmans:
            {
                return { // Mail=Steel
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Mail),
                         MakeArmorSpecific(armor::helm_type::MailCoif),

                         // Leather
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest)
                };
            }
            case set_type::TheDeadeye:
            {
                return { // Mail=Silver
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Mail),
                         MakeArmorSpecific(armor::helm_type::MailCoif),

                         // Leather
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest)
                };
            }
            case set_type::TheDuskRanger:
            {
                return { // Scale
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Scale),

                         // Platinum
                         MakeArmorSpecific(armor::helm_type::Archers),

                         // Leather
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest)
                };
            }
            case set_type::TheVenomBow:
            {
                return { // Scale
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Scale),

                         // Platinum
                         MakeArmorSpecific(armor::helm_type::Archers),

                         // Leather
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),
                         MakeArmorSpecific(armor::cover_type::Vest)
                };
            }

            case set_type::TheCritterChannelers:
            {
                return { // Plain=Silk
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),

                         // Steel
                         MakeArmorSpecific(armor::shield_type::Buckler),

                         // Leather
                         MakeArmorSpecific(armor::helm_type::Leather)
                };
            }
            case set_type::TheMammalianHead:
            {
                return { // Plain=Fur
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Plain),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Plain),

                         // Silver
                         MakeArmorSpecific(armor::shield_type::Buckler),
                         MakeArmorSpecific(armor::helm_type::MailCoif)
                };
            }
            case set_type::TheSavageTaskmasters:
            {
                return { // Mail=Silver
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Mail),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Mail),
                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::Archers)
                };
            }
            case set_type::TheMonsterOverseers:
            {
                return { // Scales
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Scale),

                         // Silver
                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::Bascinet)
                };
            }
            case set_type::TheBeastRulers:
            {
                return { // Scales
                         MakeArmorNonSpecific(armor_type::Boots, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Pants, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Shirt, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Bracers, armor::base_type::Scale),
                         MakeArmorNonSpecific(armor_type::Gauntlets, armor::base_type::Scale),

                         // Platinum
                         MakeArmorSpecific(armor::shield_type::Kite),
                         MakeArmorSpecific(armor::helm_type::Bascinet)
                };
            }

            case set_type::Not:
            case set_type::Count:
            default:
            {
                return {};
            }
        }
    }

    const ItemProfileThin ItemProfileThinFactory::MakeArmorNonSpecific(
        const armor_type::Enum ARMOR_TYPE,
        const armor::base_type::Enum BASE_TYPE,
        const misc_type::Enum MISC_TYPE) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((ARMOR_TYPE != armor_type::Shield) && (ARMOR_TYPE != armor_type::Covering)
             && (ARMOR_TYPE != armor_type::Helm)),
            "item::ItemProfileThinFactory::MakeArmorNonSpecific(armor_type="
                << armor_type::ToString(ARMOR_TYPE) << ", base_type="
                << ((BASE_TYPE == armor::base_type::Count) ? "Count"
                                                           : armor::base_type::ToString(BASE_TYPE))
                << ", misc_type="
                << ((MISC_TYPE == misc_type::Count) ? "Count" : misc_type::ToString(MISC_TYPE))
                << ") but the armor_type given IS a SPECIFIC type.");

        return ItemProfileThin(armor::ArmorTypeWrapper(ARMOR_TYPE, BASE_TYPE), MISC_TYPE);
    }

    const ItemProfileThinVec_t
        ItemProfileThinFactory::MakeArmorNonSpecificAll(const armor_type::Enum ARMOR_TYPE) const
    {
        ItemProfileThinVec_t thinProfiles;

        for (misc::EnumUnderlying_t i(0); i < armor::base_type::Count; ++i)
        {
            auto const BASE_TYPE{ static_cast<armor::base_type::Enum>(i) };
            thinProfiles.emplace_back(MakeArmorNonSpecific(ARMOR_TYPE, BASE_TYPE));
        }

        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileThinFactory::MakeArmorNonSpecificAll(
        const armor::base_type::Enum BASE_TYPE) const
    {
        ItemProfileThinVec_t thinProfiles;
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Aventail, BASE_TYPE));
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Boots, BASE_TYPE));
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Bracers, BASE_TYPE));
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Gauntlets, BASE_TYPE));
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Pants, BASE_TYPE));
        thinProfiles.emplace_back(MakeArmorNonSpecific(armor_type::Shirt, BASE_TYPE));
        return thinProfiles;
    }

    const ItemProfileThinVec_t ItemProfileThinFactory::MakeArmorNonSpecificAll() const
    {
        ItemProfileThinVec_t thinProfiles;

        for (misc::EnumUnderlying_t i(0); i < armor::base_type::Count; ++i)
        {
            auto const BASE_TYPE{ static_cast<armor::base_type::Enum>(i) };

            for (auto const & THIN_PROFILE : MakeArmorNonSpecificAll(BASE_TYPE))
            {
                thinProfiles.emplace_back(THIN_PROFILE);
            }
        }

        return thinProfiles;
    }

    const ItemProfileThinVec_t
        ItemProfileThinFactory::MakeWeaponOfTypeAll(const weapon_type::Enum WEAPON_TYPE) const
    {
        ItemProfileThinVec_t thinProfiles;
        for (auto const & WEAPON_TYPE_WRAPPER : weapon::WeaponTypeWrapper::MakeCompleteSet())
        {
            // skip body parts
            if ((WEAPON_TYPE_WRAPPER.IsBodyPart() == false)
                && (WEAPON_TYPE_WRAPPER.Type() & WEAPON_TYPE))
            {
                thinProfiles.emplace_back(MakeWeapon(WEAPON_TYPE_WRAPPER));
            }
        }

        return thinProfiles;
    }

    const ItemProfileThinVec_t
        ItemProfileThinFactory::MakeWeaponKnifeOrDaggerAll(const bool IS_DAGGER) const
    {
        return { MakeWeaponKnifeOrDagger(IS_DAGGER) };
    }

} // namespace item
} // namespace heroespath
