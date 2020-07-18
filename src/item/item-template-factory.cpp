// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-template-factory.cpp
//
#include "item-template-factory.hpp"

#include "misc/vectors.hpp"

#include <vector>

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemTemplateFactory> ItemTemplateFactory::instanceUPtr_;

    ItemTemplateFactory::ItemTemplateFactory()
        : weaponSpecifics_()
        , armorSpecifics_()
        , weapons_()
        , armor_()
        , misc_()
        , namedToTemplates_()
        , setToTemplates_()
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemTemplateFactory");
    }

    ItemTemplateFactory::~ItemTemplateFactory()
    {
        M_HP_LOG_DBG("Subsystem Destruction: ItemTemplateFactory");
    }

    misc::NotNull<ItemTemplateFactory *> ItemTemplateFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR(
                "Subsystem Instance() called but instanceUPtr_ was null: ItemTemplateFactory");
            Create();
        }

        return misc::NotNull<ItemTemplateFactory *>(instanceUPtr_.get());
    }

    void ItemTemplateFactory::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemTemplateFactory>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: ItemTemplateFactory");
        }
    }

    void ItemTemplateFactory::Destroy() { instanceUPtr_.reset(); }

    void ItemTemplateFactory::Initialize()
    {
        weaponSpecifics_.reserve(
            Axes::Count + Bladedstaffs::Count + BodyPartWeapons::Count + Clubs::Count
            + Knifes::Count + Projectiles::Count + Staffs::Count + Swords::Count + Whips::Count);

        const auto WEAPON_SPECIFIC_RESERVED = weaponSpecifics_.capacity();

        MakeAndAppendWeaponSpecific<Axes>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Bladedstaffs>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<BodyPartWeapons>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Clubs>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Knifes>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Projectiles>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Staffs>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Swords>(weaponSpecifics_);
        MakeAndAppendWeaponSpecific<Whips>(weaponSpecifics_);

        M_HP_LOG_WRN(
            "ItemTemplateFactory Weapon Specific:  reserved="
            << WEAPON_SPECIFIC_RESERVED << ", final=" << weaponSpecifics_.size());

        weapons_.reserve(weaponSpecifics_.size());
        for (const auto & WEAPON_SPECIFIC_TEMPLATE : weaponSpecifics_)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                WEAPON_SPECIFIC_TEMPLATE.IsValid(),
                "WeaponTemplate was invalid: " << WEAPON_SPECIFIC_TEMPLATE.ToString());

            weapons_.emplace_back(WEAPON_SPECIFIC_TEMPLATE);

            M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                weapons_.back().IsValid(),
                "ItemTemplate (made from a WeaponTemplate) was invalid: "
                    << weapons_.back().ToString());
        }

        M_HP_LOG_WRN("ItemTemplateFactory Weapon=" << weapons_.size());

        armorSpecifics_.reserve(
            Forms::Count + // Aventail
            Forms::Count + // Boot
            Forms::Count + // Bracer
            Forms::Count + // Gauntlet
            Forms::Count + // Pant
            Forms::Count + // Shirt
            Skins::Count + // Skin
            Covers::Count + // Covers
            Helms::Count + // Helms
            Shields::Count // Shields
        );

        const auto ARMOR_SPECIFIC_RESERVED = armorSpecifics_.capacity();

        MakeAndAppendArmorSpecificWithForm(Armor::Aventail, armorSpecifics_);
        MakeAndAppendArmorSpecificWithForm(Armor::Boot, armorSpecifics_);
        MakeAndAppendArmorSpecificWithForm(Armor::Bracer, armorSpecifics_);
        MakeAndAppendArmorSpecificWithForm(Armor::Gauntlet, armorSpecifics_);
        MakeAndAppendArmorSpecificWithForm(Armor::Pant, armorSpecifics_);
        MakeAndAppendArmorSpecificWithForm(Armor::Shirt, armorSpecifics_);
        //
        MakeAndAppendArmorSpecificWithoutForm<Skins>(armorSpecifics_);
        MakeAndAppendArmorSpecificWithoutForm<Covers>(armorSpecifics_);
        MakeAndAppendArmorSpecificWithoutForm<Helms>(armorSpecifics_);
        MakeAndAppendArmorSpecificWithoutForm<Shields>(armorSpecifics_);

        M_HP_LOG_WRN(
            "ItemTemplateFactory Armor Specific:  reserved="
            << ARMOR_SPECIFIC_RESERVED << ", final=" << armorSpecifics_.size());

        armor_.reserve(armorSpecifics_.size());
        for (const auto & ARMOR_SPECIFIC_TEMPLATE : armorSpecifics_)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                ARMOR_SPECIFIC_TEMPLATE.IsValid(),
                "ArmorTemplate was invalid: " << ARMOR_SPECIFIC_TEMPLATE.ToString());

            armor_.emplace_back(ARMOR_SPECIFIC_TEMPLATE);

            M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                armor_.back().IsValid(),
                "ItemTemplate (made from an ArmorTemplate) was invalid: "
                    << armor_.back().ToString());
        }

        M_HP_LOG_WRN("ItemTemplateFactory Armor=" << armor_.size());

        for (EnumUnderlying_t i(0); i < Misc::Count; ++i)
        {
            const auto MISC = static_cast<Misc::Enum>(i);
            misc_.emplace_back(MISC);

            M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                misc_.back().IsValid(),
                "ItemTemplate (made from Misc=" << NAMEOF_ENUM(MISC)
                                                << ") was invalid: " << misc_.back().ToString());
        }

        M_HP_LOG_WRN(
            "ItemTemplateFactory Misc: misc_count=" << EnumUnderlying_t(Misc::Count)
                                                    << ", misc_created=" << misc_.size());

        std::size_t namedTotalCount = 0;
        for (EnumUnderlying_t i(0); i < Named::Count; ++i)
        {
            const auto NAMED = static_cast<Named::Enum>(i);
            const auto ALL_OF_THIS_NAME = MakeNamed(NAMED);

            for (const auto & TEMPLATE : ALL_OF_THIS_NAME)
            {
                M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                    TEMPLATE.IsValid(),
                    "ItemTemplate (made from Named=" << NAMEOF_ENUM(NAMED)
                                                     << ") was invalid: " << TEMPLATE.ToString());
            }

            namedToTemplates_.Append(NAMED, ALL_OF_THIS_NAME);
            namedTotalCount += ALL_OF_THIS_NAME.size();
        }

        M_HP_LOG_WRN(
            "ItemTemplateFactory Named: named_count="
            << EnumUnderlying_t(Named::Count) << ", named_created=" << namedToTemplates_.Size()
            << ", total=" << namedTotalCount);

        std::size_t setTotalCount = 0;
        for (EnumUnderlying_t i(0); i < Set::Count; ++i)
        {
            const auto SET = static_cast<Set::Enum>(i);
            const auto ALL_OF_THIS_SET = MakeSet(SET);

            for (const auto & TEMPLATE : ALL_OF_THIS_SET)
            {
                M_HP_ASSERT_OR_LOG_AND_THROW_SS(
                    TEMPLATE.IsValid(),
                    "ItemTemplate (made from Set=" << NAMEOF_ENUM(SET)
                                                   << ") was invalid: " << TEMPLATE.ToString());
            }

            setToTemplates_.Append(SET, ALL_OF_THIS_SET);
            setTotalCount += ALL_OF_THIS_SET.size();
        }

        M_HP_LOG_WRN(
            "ItemTemplateFactory Set: set_count=" << EnumUnderlying_t(Set::Count)
                                                  << ", set_created=" << setToTemplates_.Size()
                                                  << ", total=" << setTotalCount);
    }

    const ItemTemplateVec_t ItemTemplateFactory::MakeNamed(const Named::Enum NAMED_TYPE) const
    {
        auto removeIf { [](ItemTemplateVec_t & profiles, auto ifLambda) {
            profiles.erase(
                std::remove_if(std::begin(profiles), std::end(profiles), ifLambda),
                std::end(profiles));
        } };

        auto removeIfArmorType { [&](ItemTemplateVec_t & profiles, const Armor::Enum ARMOR_TYPE) {
            removeIf(profiles, [&](const ItemTemplate & P) {
                return (P.ArmorInfo().Type() == ARMOR_TYPE);
            });
        } };

        switch (NAMED_TYPE)
        {
            case Named::Wicked:
            {
                return MakeAllWeaponsIf([](const WeaponTemplate & WP) { return WP.IsPointed(); });
            }

            case Named::Fiendish:
            {
                return MakeAllWeaponsIf([](const WeaponTemplate & WP) { return WP.IsBladed(); });
            }

            case Named::Infernal:
            {
                ItemTemplateVec_t profiles { MakeWeapon(Projectiles::Longbow),
                                             MakeWeapon(Projectiles::Compositebow),
                                             MakeWeapon(Projectiles::Sling) };

                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Sword), profiles);
                return profiles;
            }

            case Named::Raging:
            {
                return MakeAllWeaponsOfType(Weapon::Club);
            }

            case Named::Icy:
            case Named::Winter:
            case Named::Frigid:
            case Named::Diabolic:
            {
                return MakeAllWeaponsIf([](const WeaponTemplate & WP) { return WP.IsMelee(); });
            }

            case Named::Dancing:
            {
                return MakeAllWeaponsOfType(Weapon::Sword);
            }

            case Named::Marauder:
            {
                ItemTemplateVec_t profiles { MakeAllWeaponsOfType(Weapon::Sword) };

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    const auto SWORD = PROFILE.WeaponInfo().MinorAs<Swords>();

                    return (
                        (SWORD == Swords::Shortsword) || (SWORD == Swords::Falcata)
                        || (SWORD == Swords::Gladius));
                });

                return profiles;
            }

            case Named::Wardens:
            {
                ItemTemplateVec_t profiles { MakeWeapon(Swords::Shortsword) };

                misc::Vector::AppendMove(MakeAllArmorOfForm(Forms::Plain), profiles);

                removeIfArmorType(profiles, Armor::Shield);
                removeIfArmorType(profiles, Armor::Helm);

                profiles.emplace_back(MakeArmor(Helms::Bascinet));
                profiles.emplace_back(MakeArmor(Shields::Buckler));

                return profiles;
            }

            case Named::Princes:
            {
                ItemTemplateVec_t profiles { MakeAllArmorOfForm(Forms::Mail) };

                removeIfArmorType(profiles, Armor::Shield);
                removeIfArmorType(profiles, Armor::Helm);

                profiles.emplace_back(MakeArmor(Helms::MailCoif));
                profiles.emplace_back(MakeArmor(Shields::Kite));
                profiles.emplace_back(MakeArmor(Covers::Cape));
                profiles.emplace_back(MakeWeapon(Swords::Rapier));

                return profiles;
            }

            case Named::Ranger:
            {
                ItemTemplateVec_t profiles { MakeAllArmorOfForm(Forms::Plain) };

                removeIfArmorType(profiles, Armor::Shield);
                removeIfArmorType(profiles, Armor::Helm);

                profiles.emplace_back(MakeArmor(Covers::Cloak));
                profiles.emplace_back(MakeArmor(Covers::Vest));
                profiles.emplace_back(MakeWeapon(Projectiles::Longbow));
                profiles.emplace_back(MakeWeapon(Projectiles::Shortbow));
                profiles.emplace_back(MakeWeapon(Projectiles::Compositebow));
                profiles.emplace_back(MakeWeapon(Projectiles::Sling));

                return profiles;
            }

            case Named::Samurai:
            {
                ItemTemplateVec_t profiles { MakeAllArmorOfForm(Forms::Scale) };

                removeIfArmorType(profiles, Armor::Shield);
                removeIfArmorType(profiles, Armor::Helm);

                profiles.emplace_back(MakeArmor(Helms::Leather));
                profiles.emplace_back(MakeArmor(Covers::Cape));
                profiles.emplace_back(MakeWeapon(Swords::Longsword));

                return profiles;
            }

            case Named::Thors:
            {
                ItemTemplateVec_t profiles { MakeAllArmorOfForm(Forms::Plate) };

                removeIfArmorType(profiles, Armor::Shield);
                removeIfArmorType(profiles, Armor::Helm);

                profiles.emplace_back(MakeArmor(Helms::Great));
                profiles.emplace_back(MakeArmor(Shields::Pavis));
                profiles.emplace_back(MakeArmor(Covers::Cape));
                profiles.emplace_back(MakeWeapon(Swords::Claymore));

                return profiles;
            }

            case Named::Nile:
            {
                return { MakeWeapon(Swords::Falcata),
                         MakeArmor(Shields::Buckler),
                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Leather),
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Covers::Cloak) };
            }

            case Named::Searing:
            {
                return MakeAllWeaponsIf([](const WeaponTemplate & WP) { return WP.IsBladed(); });
            }

            case Named::Burning:
            {
                ItemTemplateVec_t profiles;
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Sword), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Axe), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Bladedstaff), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Club), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Staff), profiles);
                return profiles;
            }

            case Named::Fiery:
            {
                ItemTemplateVec_t profiles { MakeWeapon(Projectiles::Compositebow),
                                             MakeWeapon(Projectiles::Longbow),
                                             MakeWeapon(Projectiles::Shortbow),
                                             MakeWeapon(Projectiles::Crossbow) };

                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Whip), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Bladedstaff), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Sword), profiles);
                return profiles;
            }

            case Named::Soldiers:
            {
                ItemTemplateVec_t profiles {
                    MakeWeapon(Projectiles::Shortbow), MakeWeapon(Projectiles::Longbow),
                    MakeWeapon(Swords::Broadsword),    MakeWeapon(Swords::Longsword),
                    MakeWeapon(Swords::Knightlysword), MakeWeapon(Swords::Claymore),
                    MakeWeapon(Axes::Battleaxe),       MakeWeapon(Axes::Waraxe)
                };

                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Bladedstaff), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shield), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Helm), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Pant), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Boot), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shirt), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsForm(Forms::Plate)
                        || PROFILE.ArmorInfo().IsMinor<Shields>(Shields::Pavis));
                });

                return profiles;
            }

            case Named::Dark:
            {
                ItemTemplateVec_t profiles {
                    MakeWeapon(Projectiles::Sling),   MakeWeapon(Projectiles::Shortbow),
                    MakeWeapon(Projectiles::Longbow), MakeWeapon(Projectiles::Compositebow),
                    MakeWeapon(Swords::Falcata),      MakeWeapon(Swords::Rapier),
                    MakeWeapon(Swords::Flamberg),     MakeWeapon(Axes::Sickle),
                    MakeWeapon(Axes::Battleaxe),      MakeWeapon(Whips::Bullwhip),
                    MakeWeapon(Whips::Flail),         MakeWeapon(Whips::MaceAndChain),
                    MakeWeapon(Clubs::Spiked),        MakeWeapon(Clubs::Maul),
                    MakeWeapon(Bladedstaffs::Spear),  MakeWeapon(Bladedstaffs::Pike),
                    MakeWeapon(Bladedstaffs::Scythe)
                };

                profiles.emplace_back(MakeWeapon(Knifes::Knife));
                profiles.emplace_back(MakeWeapon(Knifes::Dagger));
                return profiles;
            }

            case Named::Betrayer:
            {
                ItemTemplateVec_t profiles {
                    MakeWeapon(Projectiles::Longbow), MakeWeapon(Projectiles::Compositebow),
                    MakeWeapon(Swords::Falcata),      MakeWeapon(Swords::Flamberg),
                    MakeWeapon(Axes::Sickle),         MakeWeapon(Whips::MaceAndChain),
                    MakeWeapon(Clubs::Spiked),        MakeWeapon(Bladedstaffs::Scythe)
                };

                profiles.emplace_back(MakeWeapon(Knifes::Dagger));

                return profiles;
            }

            case Named::Robbers:
            case Named::Thugs:
            case Named::Knaves:
            {
                return { MakeWeapon(Knifes::Knife) };
            }

            case Named::Thief:
            case Named::Muggers:
            case Named::Pirate:
            {
                return { MakeWeapon(Knifes::Dagger) };
            }

            case Named::Focus:
            {
                ItemTemplateVec_t profiles { MakeAllWeaponsOfType(Weapon::Projectile) };

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return PROFILE.WeaponInfo().IsMinor<Projectiles>(Projectiles::Blowpipe);
                });

                return profiles;
            }

            case Named::Proud:
            case Named::Glory:
            case Named::Pure:
            {
                ItemTemplateVec_t profiles { MakeWeapon(Projectiles::Longbow),
                                             MakeWeapon(Projectiles::Crossbow),
                                             MakeWeapon(Projectiles::Compositebow),
                                             MakeWeapon(Swords::Broadsword),
                                             MakeWeapon(Swords::Longsword),
                                             MakeWeapon(Swords::Knightlysword),
                                             MakeWeapon(Swords::Claymore),
                                             MakeWeapon(Axes::Battleaxe),
                                             MakeWeapon(Axes::Waraxe),
                                             MakeWeapon(Whips::MaceAndChain),
                                             MakeWeapon(Clubs::Warhammer) };

                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Staff), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Bladedstaff), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return PROFILE.WeaponInfo().IsEitherSpear();
                });

                return profiles;
            }

            case Named::Gloom:
            case Named::Twilight:
            case Named::Dusk:
            {
                ItemTemplateVec_t profiles {
                    MakeWeapon(Knifes::Dagger),      MakeWeapon(Knifes::Knife),
                    MakeWeapon(Swords::Cutlass),     MakeWeapon(Swords::Falcata),
                    MakeWeapon(Swords::Rapier),      MakeWeapon(Swords::Saber),
                    MakeWeapon(Swords::Flamberg),    MakeWeapon(Axes::Handaxe),
                    MakeWeapon(Axes::Sickle),        MakeWeapon(Axes::Battleaxe),
                    MakeWeapon(Axes::Waraxe),        MakeWeapon(Clubs::Spiked),
                    MakeWeapon(Clubs::Maul),         MakeWeapon(Bladedstaffs::ShortSpear),
                    MakeWeapon(Bladedstaffs::Spear), MakeWeapon(Bladedstaffs::Pike),
                    MakeWeapon(Bladedstaffs::Scythe)
                };

                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Projectile), profiles);
                misc::Vector::AppendMove(MakeAllWeaponsOfType(Weapon::Whip), profiles);

                return profiles;
            }

            case Named::Honest:
            {
                ItemTemplateVec_t profiles(std::begin(armor_), std::end(armor_));

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Cloak)
                        || PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Robe));
                });

                return profiles;
            }

            case Named::Noble:
            {
                ItemTemplateVec_t profiles(std::begin(armor_), std::end(armor_));

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Cloak)
                        || PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Robe)
                        || PROFILE.ArmorInfo().IsForm(Forms::Plain));
                });

                return profiles;
            }

            case Named::Daring:
            {
                ItemTemplateVec_t profiles(std::begin(armor_), std::end(armor_));

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Cloak)
                        || PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Robe)
                        || PROFILE.ArmorInfo().IsForm(Forms::Plain)
                        || PROFILE.ArmorInfo().IsMinor<Shields>(Shields::Buckler)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Leather));
                });

                return profiles;
            }

            case Named::Elite:
            {
                ItemTemplateVec_t profiles(std::begin(armor_), std::end(armor_));

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Cloak)
                        || PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Robe)
                        || PROFILE.ArmorInfo().IsForm(Forms::Plain)
                        || PROFILE.ArmorInfo().IsForm(Forms::Scale)
                        || PROFILE.ArmorInfo().IsMinor<Shields>(Shields::Buckler)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Leather)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::MailCoif)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Kettle));
                });

                return profiles;
            }

            case Named::Valiant:
            {
                ItemTemplateVec_t profiles { MakeArmor(Shields::Heater), MakeArmor(Shields::Pavis),
                                             MakeArmor(Helms::Bascinet), MakeArmor(Helms::Great),
                                             MakeArmor(Covers::Cape),    MakeArmor(Covers::Vest) };

                misc::Vector::AppendMove(MakeAllArmorOfForm(Forms::Plate), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfForm(Forms::Mail), profiles);

                return profiles;
            }

            case Named::Heroes:
            {
                ItemTemplateVec_t profiles { MakeArmor(Shields::Pavis),
                                             MakeArmor(Helms::Great),
                                             MakeArmor(Covers::Cape),
                                             MakeArmor(Covers::Vest) };

                misc::Vector::AppendMove(MakeAllArmorOfForm(Forms::Plate), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (PROFILE.ArmorInfo().IsBracer() || PROFILE.ArmorInfo().IsAventail());
                });

                return profiles;
            }

            case Named::Army:
            {
                ItemTemplateVec_t profiles {};

                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shield), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Helm), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Pant), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Boot), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shirt), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsForm(Forms::Plate)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Leather)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::MailCoif)
                        || PROFILE.ArmorInfo().IsMinor<Shields>(Shields::Pavis));
                });

                return profiles;
            }

            case Named::Gladiator:
            {
                return { MakeArmor(Shields::Buckler),
                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Great),
                         MakeArmor(Helms::Bascinet),
                         MakeArmor(Helms::Kettle),
                         MakeWeapon(Axes::Handaxe),
                         MakeWeapon(Bladedstaffs::ShortSpear),
                         MakeWeapon(Bladedstaffs::Spear),
                         MakeWeapon(Swords::Gladius),
                         MakeWeapon(Swords::Shortsword),
                         MakeWeapon(Swords::Broadsword) };
            }

            case Named::Charred:
            {
                ItemTemplateVec_t profiles {
                    MakeArmor(Covers::Vest),
                };

                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shield), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Helm), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Gauntlet), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shirt), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Bracer), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return !PROFILE.ArmorInfo().IsForm(Forms::Plain);
                });

                return profiles;
            }

            case Named::Moon:
            {
                ItemTemplateVec_t profiles { MakeArmor(Covers::Robe),
                                             MakeArmor(Covers::Cloak),
                                             MakeArmor(Covers::Vest) };

                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shield), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Helm), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Pant), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shirt), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Boot), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsForm(Forms::Plain)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Kettle)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Leather));
                });

                return profiles;
            }

            case Named::Imposters:
            case Named::Pickpocket:
            {
                return { MakeArmor(Covers::Robe) };
            }

            case Named::Chill:
            case Named::Frozen:
            case Named::Arctic:
            {
                ItemTemplateVec_t profiles(std::begin(armor_), std::end(armor_));

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsPant() || PROFILE.ArmorInfo().IsBoot()
                        || PROFILE.ArmorInfo().IsShirt() || PROFILE.ArmorInfo().IsAventail()
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::MailCoif)
                        || PROFILE.ArmorInfo().IsMinor<Helms>(Helms::Leather)
                        || PROFILE.ArmorInfo().IsMinor<Covers>(Covers::Cape)
                        || PROFILE.ArmorInfo().IsMinor<Shields>(Shields::Buckler));
                });

                return profiles;
            }

            case Named::Light:
            case Named::Dawn:
            case Named::Sun:
            {
                ItemTemplateVec_t profiles {
                    MakeArmor(Covers::Robe),    MakeArmor(Covers::Cape),
                    MakeArmor(Covers::Vest),    MakeArmor(Shields::Kite),
                    MakeArmor(Shields::Heater), MakeArmor(Shields::Pavis),
                    MakeArmor(Helms::Archers),  MakeArmor(Helms::Bascinet),
                    MakeArmor(Helms::Great),
                };

                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Gauntlet), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Pant), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Boot), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Shirt), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return (
                        PROFILE.ArmorInfo().IsForm(Forms::Plain)
                        || PROFILE.ArmorInfo().IsForm(Forms::Scale));
                });

                return profiles;
            }

            case Named::Sorrow:
            case Named::Woe:
            case Named::Misery:
            {
                ItemTemplateVec_t profiles {
                    MakeArmor(Covers::Cloak),    MakeArmor(Covers::Vest),
                    MakeArmor(Shields::Buckler), MakeArmor(Shields::Kite),
                    MakeArmor(Shields::Heater),  MakeArmor(Helms::Leather),
                    MakeArmor(Helms::MailCoif),  MakeArmor(Helms::Archers),
                    MakeArmor(Helms::Bascinet),
                };

                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Gauntlet), profiles);
                misc::Vector::AppendMove(MakeAllArmorOfType(Armor::Bracer), profiles);

                removeIf(profiles, [](const ItemTemplate & PROFILE) {
                    return PROFILE.ArmorInfo().IsForm(Forms::Plain);
                });

                return profiles;
            }

            case Named::Charlatans:
            {
                return { MakeArmor(Covers::Vest) };
            }

            case Named::Burglar:
            case Named::Mountebank:
            {
                return { MakeArmor(Covers::Cloak) };
            }

            case Named::Count:
            default:
            {
                return {};
            }
        }
    } // namespace item

    const ItemTemplateVec_t ItemTemplateFactory::MakeSet(const Set::Enum SET_TYPE) const
    {
        switch (SET_TYPE)
        {
            case Set::Assassins:
            case Set::Tickler:
            case Set::Neverwinter:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Cloak),
                         MakeWeapon(Knifes::Dagger),
                         MakeMisc(Misc::LockPicks) };
            }

            case Set::Magus:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Wand) };
            }
            case Set::Necromancers:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Orb) };
            }
            case Set::Warlocks:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Scepter) };
            }

            case Set::LichKings:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::LichHand) };
            }

            case Set::Sages:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Shard) };
            }
            case Set::Shamans:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Orb) };
            }
            case Set::Oracles:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::Scepter) };
            }
            case Set::Angelic:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Robe),
                         MakeMisc(Misc::AngelBraid) };
            }

            case Set::Balladeers:
            {
                return { MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest),
                         MakeWeapon(Swords::Cutlass),
                         MakeMisc(Misc::DrumLute) };
            }
            case Set::Troubadours:
            {
                return { MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest),
                         MakeWeapon(Swords::Rapier),
                         MakeMisc(Misc::DrumLute) };
            }
            case Set::Muses:
            {
                return { MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest),
                         MakeWeapon(Swords::Longsword),
                         MakeMisc(Misc::DrumLute) };
            }

            case Set::Cavaliers:
            {
                return { // Plain=Cloth
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Cape),

                         // Plain=Leather
                         MakeArmor(Armor::Aventail, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),

                         MakeArmor(Shields::Buckler),
                         MakeArmor(Helms::Leather)
                };
            }
            case Set::Champions:
            {
                return { // Plain=Silk
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Cape),

                         // Plain=Leather
                         MakeArmor(Armor::Aventail, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),

                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::MailCoif)
                };
            }
            case Set::Paladins:
            {
                return { // Plain=Leather
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Cape),

                         // Plain=Leather
                         MakeArmor(Armor::Aventail, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),

                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Kettle)
                };
            }
            case Set::Berserkers:
            {
                return { // Plain=Fur
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Cape),

                         // Plain=Leather
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Armor::Aventail, Forms::Plain),

                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Bascinet)
                };
            }
            case Set::Rosewood:
            {
                return { // Mail=Silver
                         MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Gauntlet, Forms::Mail),
                         MakeArmor(Armor::Aventail, Forms::Mail),
                         MakeArmor(Armor::Bracer, Forms::Mail),

                         // Silk
                         MakeArmor(Covers::Cape),

                         MakeArmor(Shields::Heater),
                         MakeArmor(Helms::Bascinet)
                };
            }
            case Set::Dragonslayers:
            {
                return { MakeArmor(Armor::Boot, Forms::Scale),
                         MakeArmor(Armor::Pant, Forms::Scale),
                         MakeArmor(Armor::Shirt, Forms::Scale),
                         MakeArmor(Armor::Gauntlet, Forms::Scale),
                         MakeArmor(Armor::Bracer, Forms::Scale),
                         MakeArmor(Armor::Aventail, Forms::Scale),

                         // Leather
                         MakeArmor(Covers::Cape),

                         MakeArmor(Shields::Heater),
                         MakeArmor(Helms::Bascinet) };
            }
            case Set::EventideRider:
            {
                return { // Plate=Platinum
                         MakeArmor(Armor::Boot, Forms::Plate),
                         MakeArmor(Armor::Pant, Forms::Plate),
                         MakeArmor(Armor::Shirt, Forms::Plate),
                         MakeArmor(Armor::Gauntlet, Forms::Plate),
                         MakeArmor(Armor::Bracer, Forms::Plate),
                         MakeArmor(Armor::Aventail, Forms::Plate),

                         // Fur
                         MakeArmor(Covers::Cape),

                         MakeArmor(Shields::Pavis),
                         MakeArmor(Helms::Great)
                };
            }

            case Set::Hunters:
            {
                return { // Plain=Silk
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest),

                         // Leather
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Helms::Leather)
                };
            }
            case Set::SureShot:
            {
                return { // Plain=Leather
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Covers::Vest),
                         MakeArmor(Helms::Leather)
                };
            }
            case Set::Marksmans:
            {
                return { // Mail=Steel
                         MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Bracer, Forms::Mail),
                         MakeArmor(Helms::MailCoif),

                         // Leather
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest)
                };
            }
            case Set::Deadeye:
            {
                return { // Mail=Silver
                         MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Bracer, Forms::Mail),
                         MakeArmor(Helms::MailCoif),

                         // Leather
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest)
                };
            }
            case Set::DuskRanger:
            {
                return { // Scale
                         MakeArmor(Armor::Boot, Forms::Scale),
                         MakeArmor(Armor::Pant, Forms::Scale),
                         MakeArmor(Armor::Shirt, Forms::Scale),
                         MakeArmor(Armor::Bracer, Forms::Scale),

                         // Platinum
                         MakeArmor(Helms::Archers),

                         // Leather
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest)
                };
            }
            case Set::VenomBow:
            {
                return { // Scale
                         MakeArmor(Armor::Boot, Forms::Scale),
                         MakeArmor(Armor::Pant, Forms::Scale),
                         MakeArmor(Armor::Shirt, Forms::Scale),
                         MakeArmor(Armor::Bracer, Forms::Scale),

                         // Platinum
                         MakeArmor(Helms::Archers),

                         // Leather
                         MakeArmor(Armor::Gauntlet, Forms::Plain),
                         MakeArmor(Covers::Vest)
                };
            }

            case Set::CritterChannelers:
            {
                return { // Plain=Silk
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),

                         // Steel
                         MakeArmor(Shields::Buckler),

                         // Leather
                         MakeArmor(Helms::Leather)
                };
            }
            case Set::MammalianHead:
            {
                return { // Plain=Fur
                         MakeArmor(Armor::Boot, Forms::Plain),
                         MakeArmor(Armor::Pant, Forms::Plain),
                         MakeArmor(Armor::Shirt, Forms::Plain),
                         MakeArmor(Armor::Bracer, Forms::Plain),
                         MakeArmor(Armor::Gauntlet, Forms::Plain),

                         // Silver
                         MakeArmor(Shields::Buckler),
                         MakeArmor(Helms::MailCoif)
                };
            }
            case Set::SavageTaskmasters:
            {
                return { // Mail=Silver
                         MakeArmor(Armor::Boot, Forms::Mail),
                         MakeArmor(Armor::Pant, Forms::Mail),
                         MakeArmor(Armor::Shirt, Forms::Mail),
                         MakeArmor(Armor::Bracer, Forms::Mail),
                         MakeArmor(Armor::Gauntlet, Forms::Mail),
                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Archers)
                };
            }
            case Set::MonsterOverseers:
            {
                return { // Scales
                         MakeArmor(Armor::Boot, Forms::Scale),
                         MakeArmor(Armor::Pant, Forms::Scale),
                         MakeArmor(Armor::Shirt, Forms::Scale),
                         MakeArmor(Armor::Bracer, Forms::Scale),
                         MakeArmor(Armor::Gauntlet, Forms::Scale),

                         // Silver
                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Bascinet)
                };
            }
            case Set::BeastRulers:
            {
                return { // Scales
                         MakeArmor(Armor::Boot, Forms::Scale),
                         MakeArmor(Armor::Pant, Forms::Scale),
                         MakeArmor(Armor::Shirt, Forms::Scale),
                         MakeArmor(Armor::Bracer, Forms::Scale),
                         MakeArmor(Armor::Gauntlet, Forms::Scale),

                         // Platinum
                         MakeArmor(Shields::Kite),
                         MakeArmor(Helms::Bascinet)
                };
            }

            case Set::Count:
            default:
            {
                return {};
            }
        }
    }

    ItemTemplateVec_t ItemTemplateFactory::MakeAllWeaponsOfType(const Weapon::Enum TYPE_ENUM) const
    {
        ItemTemplateVec_t profiles;
        profiles.reserve(16);

        for (const auto & WEAPON_SPECIFIC_PROFILE : weaponSpecifics_)
        {
            if (WEAPON_SPECIFIC_PROFILE.Type() == TYPE_ENUM)
            {
                profiles.emplace_back(ItemTemplate(WEAPON_SPECIFIC_PROFILE));
            }
        }

        return profiles;
    }

    void ItemTemplateFactory::MakeAndAppendArmorSpecificWithForm(
        const Armor::Enum ARMOR_TYPE, std::vector<ArmorTemplate> & templates) const
    {
        for (EnumUnderlying_t i(0); i < Forms::Count; ++i)
        {
            const auto FORM = static_cast<Forms::Enum>(i);
            templates.push_back(ArmorTemplate(ARMOR_TYPE, FORM));
        }
    }

    std::size_t
        ItemTemplateFactory::ArmorIndexByType(const Armor::Enum TYPE, const Forms::Enum FORM) const
    {
        std::size_t index = 0;

        if (TYPE <= Armor::Shirt)
        {
            index = (static_cast<std::size_t>(TYPE) * static_cast<std::size_t>(Forms::Count));

            if (FORM < Forms::Count)
            {
                index += FORM;
            }
        }

        return index;
    }

    ItemTemplateVec_t ItemTemplateFactory::MakeAllArmorOfType(const Armor::Enum ARMOR_TYPE) const
    {
        ItemTemplateVec_t profiles;
        profiles.reserve(16);

        for (const auto & ARMOR_PROFILE : armorSpecifics_)
        {
            if (ARMOR_PROFILE.Type() == ARMOR_TYPE)
            {
                profiles.emplace_back(ItemTemplate(ARMOR_PROFILE));
            }
        }

        return profiles;
    }

    ItemTemplateVec_t ItemTemplateFactory::MakeAllArmorOfForm(const Forms::Enum FORM) const
    {
        ItemTemplateVec_t profiles;
        profiles.reserve(16);

        for (const auto & ARMOR_PROFILE : armorSpecifics_)
        {
            if (ARMOR_PROFILE.Form() == FORM)
            {
                profiles.emplace_back(ItemTemplate(ARMOR_PROFILE));
            }
        }

        return profiles;
    }

} // namespace item
} // namespace heroespath
