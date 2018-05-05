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
// material-collections.cpp
//
#include "material-collections.hpp"

#include "log/log-macros.hpp"

#include <algorithm>

namespace heroespath
{
namespace item
{

    MaterialCollections::MaterialCollections()
        : coreMetalEnums_()
        , coreMetalIncludingNothingEnums_()
        , coreJewelEnums_()
        , coreJewelIncludingNothingEnums_()
        , coreMiscEnums_()
        , coreMiscIncludingNothingEnums_()
        , corePrimaryEnums_()
        , corePrimaryWithoutPearlEnums_()
        , coreSecondaryEnums_()
        , coreSecondaryIncludingNothingEnums_()
        , corePrimaryAndSecondaryEnumsPair_()
        , corePrimaryWithoutSecondaryEnumsPair_()
        , coreMetalAndCoreSecondaryEnumsPair_()
        , coreJewelAndCoreSecondaryEnumsPair_()
        , coreMetalJewelAndCoreSecondaryEnumsPair_()
        , corePrimaryWithoutPearlAndSecondaryEnumsPair_()
        , wandAndStaffMiscTypeEnumsPair_()
        , coreMetalAndMiscWithCoreSecondaryEnumsPair_()
        , namedDaggerEnumsPair_()
    {
        // TODO just use the initializer list...
        Setup();
    }

    void MaterialCollections::Setup()
    {
        coreMetalEnums_ = MakeCoreMetal(false);
        coreMetalIncludingNothingEnums_ = MakeCoreMetal(true);
        coreJewelEnums_ = MakeCoreJewel(false);
        coreJewelIncludingNothingEnums_ = MakeCoreJewel(true);
        coreMiscEnums_ = MakeCoreMisc(false);
        coreMiscIncludingNothingEnums_ = MakeCoreMisc(true);
        corePrimaryEnums_ = MakeCorePrimary();
        corePrimaryWithoutPearlEnums_ = MakeCorePrimaryWithoutPearl();
        coreSecondaryEnums_ = MakeCoreSecondary(false);
        coreSecondaryIncludingNothingEnums_ = MakeCoreSecondary(true);
        corePrimaryAndSecondaryEnumsPair_ = MakeCorePrimaryAndSecondary();
        corePrimaryWithoutSecondaryEnumsPair_ = MakeCorePrimaryWithoutSecondary();
        coreMetalAndCoreSecondaryEnumsPair_ = MakeCoreMetalAndCoreSecondary();
        coreJewelAndCoreSecondaryEnumsPair_ = MakeCoreJewelAndCoreSecondary();
        coreMetalJewelAndCoreSecondaryEnumsPair_ = MakeCoreMetalJewelAndCoreSecondary();
        corePrimaryWithoutPearlAndSecondaryEnumsPair_ = MakeCorePrimaryWithoutPearlAndSecondary();
        wandAndStaffMiscTypeEnumsPair_ = MakeForWandAndStaffMiscTypes();
        coreMetalAndMiscWithCoreSecondaryEnumsPair_ = MakeCoreMetalAndMiscWithCoreSecondary();
        namedDaggerEnumsPair_ = MakeNamedDaggerMaterialsPair();
    }

    bool MaterialCollections::IsMaterialCombinationValid(
        const ItemProfileThin & THIN_PROFILE,
        const material::Enum PRIMARY,
        const material::Enum SECONDARY) const
    {
        if (material::IsSolid(PRIMARY) == false)
        {
            M_HP_LOG_WRN(
                "item::MaterialCollections::IsMaterialCombinationValid("
                << material::ToString(PRIMARY) << ", " << material::ToString(SECONDARY)
                << ") found non-solid primary.");

            return false;
        }
        else if (PRIMARY == material::Pearl)
        {
            M_HP_LOG_WRN(
                "item::MaterialCollections::IsMaterialCombinationValid("
                << material::ToString(PRIMARY) << ", " << material::ToString(SECONDARY)
                << ") found pearl primary.  Note the secondary and make sure we aren't doing "
                   "anything stupid...");
        }
        else if (PRIMARY == material::Glass)
        {
            M_HP_LOG_WRN(
                "item::MaterialCollections::IsMaterialCombinationValid("
                << material::ToString(PRIMARY) << ", " << material::ToString(SECONDARY)
                << ") found glass primary.  Note the secondary and make sure we aren't doing "
                   "anything stupid...");
        }

        auto isSecondaryAnyOf{ [SECONDARY](const MaterialVec_t & MATERIALS) {
            return (
                std::find(std::begin(MATERIALS), std::end(MATERIALS), SECONDARY)
                != std::end(MATERIALS));
        } };

        if (material::IsMetal(PRIMARY))
        {
            if (PRIMARY == material::Platinum)
            {
                if (isSecondaryAnyOf({ material::Silver,
                                       material::Steel,
                                       material::Iron,
                                       material::Bronze,
                                       material::Tin,
                                       material::Obsidian,
                                       material::Stone,
                                       material::Wood }))
                {
                    return false;
                }
            }
            else if (PRIMARY == material::Gold)
            {
                if (isSecondaryAnyOf({ material::Steel,
                                       material::Iron,
                                       material::Bronze,
                                       material::Tin,
                                       material::Stone,
                                       material::Wood }))
                {
                    return false;
                }
            }
            else if (PRIMARY == material::Silver)
            {
                if (isSecondaryAnyOf({ material::Platinum,
                                       material::Steel,
                                       material::Iron,
                                       material::Bronze,
                                       material::Tin,
                                       material::Obsidian,
                                       material::Stone,
                                       material::Wood }))
                {
                    return false;
                }
            }
            else if (PRIMARY == material::Steel)
            {
                if (isSecondaryAnyOf({ material::Platinum,
                                       material::Silver,
                                       material::Steel,
                                       material::Iron,
                                       material::Bronze,
                                       material::Tin,
                                       material::Obsidian,
                                       material::Stone,
                                       material::Wood }))
                {
                    return false;
                }
            }
        }

        if (THIN_PROFILE.IsWeapon())
        {
        }

        return true;
    }

    const MaterialVec_t MaterialCollections::MakeCoreMetal(const bool WILL_INCLUDE_NOTHING)
    {
        MaterialVec_t materials{ material::Tin,     material::Bronze, material::Iron,
                                 material::Steel,   material::Silver, material::Gold,
                                 material::Platinum };

        if (WILL_INCLUDE_NOTHING)
        {
            materials.emplace_back(material::Nothing);
        }

        return MakeVectorSortedAndUnique(materials);
    }

    const MaterialVec_t MaterialCollections::MakeCoreJewel(const bool WILL_INCLUDE_NOTHING)
    {
        MaterialVec_t materials{ material::Jade,   material::Amethyst, material::Emerald,
                                 material::Ruby,   material::Lazuli,   material::Sapphire,
                                 material::Diamond };

        if (WILL_INCLUDE_NOTHING)
        {
            materials.emplace_back(material::Nothing);
        }

        return MakeVectorSortedAndUnique(materials);
    }

    const MaterialVec_t MaterialCollections::MakeCoreMisc(const bool WILL_INCLUDE_NOTHING)
    {
        MaterialVec_t materials{
            material::Bone, material::Stone, material::Obsidian, material::Pearl
        };

        if (WILL_INCLUDE_NOTHING)
        {
            materials.emplace_back(material::Nothing);
        }

        return MakeVectorSortedAndUnique(materials);
    }

    const MaterialVec_t MaterialCollections::MakeCorePrimary()
    {
        return MakeVectorSortedAndUnique(CoreMetal(), CoreJewel(), CoreMisc());
    }

    const MaterialVec_t MaterialCollections::MakeCorePrimaryWithoutPearl()
    {
        return MakeVectorSortedAndUnique(
            CoreMetal(), CoreJewel(), { material::Bone, material::Stone, material::Obsidian });
    }

    const MaterialVec_t MaterialCollections::MakeCoreSecondary(const bool WILL_INCLUDE_NOTHING)
    {
        MaterialVec_t materials{ CoreJewel() };

        const MaterialVec_t MISC_MATERIALS{ material::Obsidian,
                                            material::Pearl,
                                            material::Silver,
                                            material::Gold,
                                            material::Platinum };

        materials.insert(std::end(materials), std::begin(MISC_MATERIALS), std::end(MISC_MATERIALS));

        if (WILL_INCLUDE_NOTHING)
        {
            materials.emplace_back(material::Nothing);
        }

        return MakeVectorSortedAndUnique(materials);
    }

    const MaterialVec_t MaterialCollections::MakeVectorSortedAndUnique(
        const MaterialVec_t & VECTOR1, const MaterialVec_t & VECTOR2, const MaterialVec_t & VECTOR3)
    {
        MaterialVec_t materials{ VECTOR1 };

        materials.insert(std::end(materials), std::begin(VECTOR2), std::end(VECTOR2));
        materials.insert(std::end(materials), std::begin(VECTOR3), std::end(VECTOR3));

        if (materials.size() > 1)
        {
            std::sort(std::begin(materials), std::end(materials));

            materials.erase(
                std::unique(std::begin(materials), std::end(materials)), std::end(materials));
        }

        return materials;
    }

    const MaterialVecPair_t MaterialCollections::MakeVectorPairSortedAndUnique(
        const MaterialVec_t & VECTOR1, const MaterialVec_t & VECTOR2)
    {
        return std::make_pair(
            MakeVectorSortedAndUnique(VECTOR1), MakeVectorSortedAndUnique(VECTOR2));
    }

    const MaterialVecPair_t MaterialCollections::RemoveLameMaterialsForSpecialItems(
        const MaterialVecPair_t & ORIG_MATERIALS_PAIR) const
    {
        MaterialVecPair_t materialsPair{ ORIG_MATERIALS_PAIR };

        if (materialsPair.first.size() != 1)
        {
            auto & primaryMaterials{ materialsPair.first };

            const MaterialVec_t PRIMARY_MATERIALS_TO_REMOVE{
                material::Nothing,  material::Cloth,  material::SoftLeather, material::Wood,
                material::Tin,      material::Bronze, material::Iron,        material::Jade,
                material::Amethyst, material::Emerald
            };

            for (auto const MATERIAL_TO_REMOVE : PRIMARY_MATERIALS_TO_REMOVE)
            {
                primaryMaterials.erase(
                    std::remove(
                        std::begin(primaryMaterials),
                        std::end(primaryMaterials),
                        MATERIAL_TO_REMOVE),
                    std::end(primaryMaterials));
            }
        }

        if (materialsPair.second.size() != 1)
        {
            auto & secondaryMaterials{ materialsPair.second };

            const MaterialVec_t SECONDARY_MATERIALS_TO_REMOVE{
                material::Wood, material::Tin, material::Bronze, material::Iron
            };

            for (auto const MATERIAL_TO_REMOVE : SECONDARY_MATERIALS_TO_REMOVE)
            {
                secondaryMaterials.erase(
                    std::remove(
                        std::begin(secondaryMaterials),
                        std::end(secondaryMaterials),
                        MATERIAL_TO_REMOVE),
                    std::end(secondaryMaterials));
            }
        }

        return materialsPair;
    }

    const MaterialVecPair_t
        MaterialCollections::VectorPairByMiscType(const misc_type::Enum MISC_TYPE) const
    {
        if (MISC_TYPE == misc_type::LockPicks)
        {
            return MakeVectorPairSortedAndUnique(
                MakeVectorSortedAndUnique(
                    CoreMetal(), CoreJewel(), { material::Bone, material::Obsidian }),
                {});
        }
        else if (
            (MISC_TYPE == misc_type::Spider_Eggs) || (MISC_TYPE == misc_type::Litch_Hand)
            || (MISC_TYPE == misc_type::Egg) || (MISC_TYPE == misc_type::Embryo)
            || (MISC_TYPE == misc_type::Petrified_Snake))
        {
            return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
        }
        else if ((MISC_TYPE == misc_type::Wand) || (MISC_TYPE == misc_type::Staff))
        {
            return WandAndStaffMiscType();
        }
        else if (MISC_TYPE == misc_type::Angel_Braid)
        {
            return MakeVectorPairSortedAndUnique({ material::Hair }, {});
        }
        else if (MISC_TYPE == misc_type::DrumLute)
        {
            return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary());
        }
        else if (MISC_TYPE == misc_type::Figurine_Blessed)
        {
            return MakeVectorPairSortedAndUnique(
                { material::Wood, material::Glass }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Figurine_Cursed)
        {
            return MakeVectorPairSortedAndUnique(
                { material::Stone, material::Bone, material::Obsidian }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Doll_Blessed)
        {
            return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Doll_Cursed)
        {
            return MakeVectorPairSortedAndUnique({ material::Bone }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Bust)
        {
            return MakeVectorPairSortedAndUnique({ material::Stone }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Puppet_Blessed)
        {
            return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Puppet_Cursed)
        {
            return MakeVectorPairSortedAndUnique({ material::Bone }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Dried_Head)
        {
            return MakeVectorPairSortedAndUnique({ material::Flesh }, CoreSecondary(false));
        }
        else if (MISC_TYPE == misc_type::Goblet)
        {
            return MakeVectorPairSortedAndUnique(
                { material::Tin, material::Bronze, material::Silver, material::Gold },
                CoreSecondary());
        }
        else if (MISC_TYPE == misc_type::Seeds)
        {
            return MakeVectorPairSortedAndUnique({ material::Plant }, {});
        }
        else if (MISC_TYPE == misc_type::Mummy_Hand)
        {
            return MakeVectorPairSortedAndUnique({ material::Flesh }, { material::Cloth });
        }
        else if (MISC_TYPE == misc_type::Shard)
        {
            return MakeVectorPairSortedAndUnique(CoreJewel(), {});
        }
        else if (MISC_TYPE == misc_type::Orb)
        {
            return MakeVectorPairSortedAndUnique(CoreJewel(), { material::Wood });
        }
        else if (MISC_TYPE == misc_type::Scepter)
        {
            return MakeVectorPairSortedAndUnique(CorePrimaryWithoutPearl(), CoreSecondary());
        }
        else if (MISC_TYPE == misc_type::Icicle)
        {
            return MakeVectorPairSortedAndUnique({ material::Glass }, {});
        }
        else if (MISC_TYPE == misc_type::Golem_Finger)
        {
            return MakeVectorPairSortedAndUnique({ material::Stone }, {});
        }
        else if (MISC_TYPE == misc_type::Unicorn_Horn)
        {
            return MakeVectorPairSortedAndUnique({ material::Horn }, {});
        }
        else if (MISC_TYPE == misc_type::Devil_Horn)
        {
            return MakeVectorPairSortedAndUnique({ material::Horn }, {});
        }
        else if (
            (MISC_TYPE == misc_type::Recorder) || (MISC_TYPE == misc_type::Viol)
            || (MISC_TYPE == misc_type::Pipe_And_Tabor) || (MISC_TYPE == misc_type::Lyre)
            || (MISC_TYPE == misc_type::Hurdy_Gurdy))
        {
            return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary());
        }
        else if (MISC_TYPE == misc_type::Ring)
        {
            return MakeVectorPairSortedAndUnique(CorePrimary(), CoreSecondary());
        }
        else if (MISC_TYPE == misc_type::Summoning_Statue)
        {
            return MakeVectorPairSortedAndUnique(CorePrimaryWithoutPearl(), {});
        }
        else if (
            (MISC_TYPE == misc_type::Pendant) || (MISC_TYPE == misc_type::Mirror)
            || (MISC_TYPE == misc_type::Key))
        {
            return CoreMetalAndMiscWithCoreSecondary();
        }
        else if (MISC_TYPE == misc_type::Doll)
        {
            return MakeVectorPairSortedAndUnique(
                { material::Cloth }, MakeVectorSortedAndUnique(CoreMisc(true), { material::Wood }));
        }
        else
        {
            return MakeVectorPairSortedAndUnique({}, {});
        }
    }

    const MaterialVecPair_t
        MaterialCollections::VectorPairByUniqueType(const unique_type::Enum UNIQUE_TyPE) const
    {
        switch (UNIQUE_TyPE)
        {
            case unique_type::BasiliskTonge:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::BerserkersBeard:
            {
                return MakeVectorPairSortedAndUnique({ material::Hair }, {});
            }
            case unique_type::BishopsHanky:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::BleedingTrophy:
            {
                return MakeVectorPairSortedAndUnique(CorePrimary(), { material::Blood });
            }
            case unique_type::BloodyDragonScale:
            {
                return MakeVectorPairSortedAndUnique({ material::Scales }, { material::Blood });
            }
            case unique_type::BottleOfBansheeScreams:
            {
                return MakeVectorPairSortedAndUnique({ material::Glass }, { material::Spirit });
            }
            case unique_type::BraceletCrown:
            case unique_type::BraceletFeather:
            case unique_type::BraceletFist:
            case unique_type::BraceletHourglass:
            case unique_type::BraceletKey:
            case unique_type::BraceletMask:
            case unique_type::BroochCrown:
            case unique_type::BroochFeather:
            case unique_type::BroochFist:
            case unique_type::BroochHourglass:
            case unique_type::BroochKey:
            case unique_type::BroochMask:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::BurialShroud:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::CharmCrown:
            case unique_type::CharmFeather:
            case unique_type::CharmFist:
            case unique_type::CharmHourglass:
            case unique_type::CharmKey:
            case unique_type::CharmMask:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::ChimeraBone:
            {
                return MakeVectorPairSortedAndUnique({ material::Bone }, {});
            }
            case unique_type::CobraTooth:
            {
                return MakeVectorPairSortedAndUnique({ material::Tooth }, {});
            }
            case unique_type::CommandersCape:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, CoreSecondary());
            }
            case unique_type::CopperTroll:
            {
                return MakeVectorPairSortedAndUnique({ material::Bronze }, {});
            }
            case unique_type::CrystalCat:
            {
                return MakeVectorPairSortedAndUnique({ material::Glass }, CoreSecondary());
            }
            case unique_type::CrystalChimes:
            {
                return MakeVectorPairSortedAndUnique({ material::Glass }, {});
            }
            case unique_type::CyclopsEye:
            {
                return MakeVectorPairSortedAndUnique(
                    MakeVectorSortedAndUnique(CoreMetal(), CoreJewel()), {});
            }
            case unique_type::DemonDiary:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::DoveBloodVial:
            {
                return MakeVectorPairSortedAndUnique({ material::Glass }, {});
            }
            case unique_type::DragonToothWhistle:
            {
                return MakeVectorPairSortedAndUnique({ material::Tooth }, {});
            }
            case unique_type::DriedFrog:
            case unique_type::DriedGecko:
            case unique_type::DriedIguana:
            case unique_type::DriedLizard:
            case unique_type::DriedSalamander:
            case unique_type::DriedSkink:
            case unique_type::DriedToad:
            case unique_type::DriedTurtle:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::DruidLeaf:
            {
                return CoreMetalJewelAndCoreSecondary();
            }
            case unique_type::EvilRabbitsFoot:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, CoreSecondary());
            }
            case unique_type::ExoticGoldenGong:
            {
                return MakeVectorPairSortedAndUnique({ material::Gold }, { material::Wood });
            }
            case unique_type::FanaticsFlag:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::FriarsChronicle:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::FuneralRecord:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::GeneralsCape:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, CoreSecondary());
            }
            case unique_type::GhostSheet:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::GiantOwlEye:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::GriffinFeather:
            {
                return MakeVectorPairSortedAndUnique({ material::Feather }, {});
            }
            case unique_type::HangmansNoose:
            {
                return MakeVectorPairSortedAndUnique({ material::Rope }, {});
            }
            case unique_type::HawkEye:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::HobgoblinNose:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::HoboRing:
            {
                return MakeVectorPairSortedAndUnique(CorePrimary(), {});
            }
            case unique_type::HolyEpic:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::HornOfTheHorde:
            {
                return MakeVectorPairSortedAndUnique({ material::Bronze }, {});
            }
            case unique_type::ImpTail:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::IslanderHeaddress:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, { material::Feather });
            }
            case unique_type::JeweledAnkhNecklace:
            {
                return CoreMetalJewelAndCoreSecondary();
            }
            case unique_type::JeweledArmband:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::JeweledHandbag:
            case unique_type::JeweledPrincessVeil:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, CoreJewel());
            }
            case unique_type::KingsCape:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, CoreJewel());
            }
            case unique_type::LastRitesScroll:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::MacabreManuscript:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::MadRatJuju:
            {
                return MakeVectorPairSortedAndUnique({ material::Bronze }, { material::Pearl });
            }
            case unique_type::MagicHorseshoe:
            {
                return MakeVectorPairSortedAndUnique({ material::Iron }, CoreSecondary());
            }
            case unique_type::MagnifyingGlass:
            {
                return MakeVectorPairSortedAndUnique(CorePrimary(), { material::Glass });
            }
            case unique_type::ManaAmulet:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::MendicantRing:
            {
                return MakeVectorPairSortedAndUnique({ material::Iron }, {});
            }
            case unique_type::MinotaurHide:
            {
                return MakeVectorPairSortedAndUnique({ material::Hide }, {});
            }
            case unique_type::MonkRing:
            {
                return MakeVectorPairSortedAndUnique({ material::Gold }, CoreSecondary());
            }
            case unique_type::MortuaryOrnament:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary());
            }
            case unique_type::MournersMask:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary());
            }
            case unique_type::PantherPaw:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::PinCrown:
            case unique_type::PinFeather:
            case unique_type::PinFist:
            case unique_type::PinHourglass:
            case unique_type::PinKey:
            case unique_type::PinMask:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::PixieBell:
            {
                return MakeVectorPairSortedAndUnique({ material::Steel }, CoreSecondary());
            }
            case unique_type::PriestRing:
            {
                return MakeVectorPairSortedAndUnique({ material::Gold }, CoreSecondary());
            }
            case unique_type::RascalMask:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, CoreSecondary());
            }
            case unique_type::RattlesnakeTail:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::RavenClaw:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::RazorFingerclaw:
            {
                return MakeVectorPairSortedAndUnique({ material::Steel }, CoreJewel(true));
            }
            case unique_type::ReaperScythe:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, { material::Steel });
            }
            case unique_type::RegalCaptainsFlag:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::RequiemRegister:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::RoyalScoutSpyglass:
            {
                return MakeVectorPairSortedAndUnique({ material::Steel }, { material::Glass });
            }
            case unique_type::SaintCameoPin:
            {
                return MakeVectorPairSortedAndUnique(CorePrimary(), CoreJewel(true));
            }
            case unique_type::SaintsJournal:
            {
                return MakeVectorPairSortedAndUnique({ material::Paper }, {});
            }
            case unique_type::SanguineRelic:
            {
                return MakeVectorPairSortedAndUnique({ CorePrimary() }, {});
            }
            case unique_type::ScorpionStingerFingerclaw:
            {
                return MakeVectorPairSortedAndUnique({ material::Steel }, CoreJewel(true));
            }
            case unique_type::ScoundrelSack:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::SepultureDecoration:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, CoreJewel(true));
            }
            case unique_type::ShadeCloak:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::ShamanRainmaker:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, {});
            }
            case unique_type::SharkToothNecklace:
            {
                return MakeVectorPairSortedAndUnique({ material::Tooth }, {});
            }
            case unique_type::SignetCrown:
            case unique_type::SignetFeather:
            case unique_type::SignetFist:
            case unique_type::SignetHourglass:
            case unique_type::SignetKey:
            case unique_type::SignetMask:
            {
                return CorePrimaryAndSecondary();
            }
            case unique_type::SirenConch:
            {
                return MakeVectorPairSortedAndUnique({ material::Glass }, {});
            }
            case unique_type::SpecterChains:
            {
                return MakeVectorPairSortedAndUnique({ material::Iron }, {});
            }
            case unique_type::SpecterRobe:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, {});
            }
            case unique_type::SprintersLegtie:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::SwindlersBag:
            {
                return MakeVectorPairSortedAndUnique({ material::SoftLeather }, {});
            }
            case unique_type::TribalFlag:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::TricksterPouch:
            {
                return MakeVectorPairSortedAndUnique({ material::Cloth }, {});
            }
            case unique_type::TuningFork:
            {
                return MakeVectorPairSortedAndUnique(CoreMetal(), {});
            }
            case unique_type::TurtleShell:
            {
                return MakeVectorPairSortedAndUnique(
                    MakeVectorSortedAndUnique(CoreMetal(), CoreJewel()), {});
            }
            case unique_type::VampiresToothNecklace:
            {
                return MakeVectorPairSortedAndUnique({ material::Tooth }, {});
            }
            case unique_type::ViperFangFingerclaw:
            {
                return MakeVectorPairSortedAndUnique({ material::Steel }, {});
            }
            case unique_type::VultureGizzard:
            {
                return MakeVectorPairSortedAndUnique({ material::Flesh }, {});
            }
            case unique_type::WarhorseMarionette:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, {});
            }
            case unique_type::WarTrumpet:
            {
                return MakeVectorPairSortedAndUnique(CoreMetal(), {});
            }
            case unique_type::WeaselTotem:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, {});
            }
            case unique_type::WolfenFur:
            {
                return MakeVectorPairSortedAndUnique({ material::Fur }, {});
            }
            case unique_type::WraithTalisman:
            {
                return MakeVectorPairSortedAndUnique({ material::Wood }, CoreJewel(true));
            }
            case unique_type::NotUnique:
            case unique_type::Count:
            default:
            {
                return MakeVectorPairSortedAndUnique({}, {});
            }
        }
    }

    const MaterialVecPair_t
        MaterialCollections::VectorPairByNamedType(const named_type::Enum NAMED_TYPE) const
    {
        if ((NAMED_TYPE == named_type::Robbers) || (NAMED_TYPE == named_type::Thugs)
            || (NAMED_TYPE == named_type::Knaves) || (NAMED_TYPE == named_type::Muggers)
            || (NAMED_TYPE == named_type::Thief) || (NAMED_TYPE == named_type::Pirate))
        {
            return namedDaggerEnumsPair_;
        }
        else
        {
            return MakeVectorPairSortedAndUnique({}, {});
        }
    }

} // namespace item
} // namespace heroespath
