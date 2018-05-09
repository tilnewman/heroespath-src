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
// materials-factory.cpp
//
#include "materials-factory.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <algorithm>

namespace heroespath
{
namespace item
{

    MaterialFactory::MaterialFactory()
        : metal_({ material::Tin,
                   material::Bronze,
                   material::Iron,
                   material::Steel,
                   material::Silver,
                   material::Gold,
                   material::Platinum })
        , fancyJewel_({ material::Amethyst,
                        material::Emerald,
                        material::Ruby,
                        material::Sapphire,
                        material::Diamond })
        , fancyJewelWithoutDiamond_(RemoveCopy(fancyJewel_, material::Diamond))
        , fancyMetal_({ material::Silver, material::Gold })
        , fancyOpaque_({ material::material::Obsidian,
                         material::material::Pearl,
                         material::material::Jade,
                         material::material::Lazuli })
        , fancyTribal_({ material::Claw, material::Bone, material::Tooth, material::Scales })
        , fancy_(Combine(fancyJewel_, fancyMetal_, fancyOpaque_, fancyTribal_))
        , coreBlade_({ material::Obsidian,
                       material::Jade,
                       material::Diamond,
                       material::Bronze,
                       material::Iron,
                       material::Steel,
                       material::Platinum })
        , smallWeaponHandle_({ material::Wood, material::Pearl, material::Bone })
        , verySmallWeaponHandle_(
              Combine({ material::Wood, material::Bone, material::Horn }, fancyOpaque_))
        , fancyReinforced_(Combine(fancy_, { material::Bronze, material::Iron }))
        , fancyReinforcedArmor_(Combine(fancy_, metal_))
        , corePole_({ material::Wood,
                      material::Jade,
                      material::Bronze,
                      material::Iron,
                      material::Silver })
        , fancyClasp_(fancyReinforcedArmor_)
        , mailArmorMetals_({ material::Tin,
                             material::Bronze,
                             material::Iron,
                             material::Steel,
                             material::Silver })
        , nonFleshEyesWithNothing_(Combine(fancyJewel_, { material::Nothing }))
    {}

    const MaterialPairVec_t MaterialFactory::MakeForEquipment(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        if ((NAMED_TYPE != named_type::NotNamed) && (NAMED_TYPE != named_type::Count))
        {
            auto const NAMED_MATERIALS{ MakeForNamedType(NAMED_TYPE) };

            // often there are no materials specific to a named_type, if so then fall through to the
            // logic below that will make the materials based on misc/equipment type
            if (NAMED_MATERIALS.empty() == false)
            {
                return CleanupMaterialPairVectorAndEnsureNotEmpty(
                    NAMED_MATERIALS, THIN_PROFILE, true, "MakeForEquipment(named_type)");
            }
        }

        if ((SET_TYPE != set_type::Count) && (SET_TYPE != set_type::NotASet))
        {
            return CleanupMaterialPairVectorAndEnsureNotEmpty(
                MakeForSetType(THIN_PROFILE, SET_TYPE),
                THIN_PROFILE,
                true,
                "MakeForEquipment(set_type)");
        }
        else
        {
            auto const IS_MAGICAL{ (NAMED_TYPE != named_type::NotNamed)
                                   && (NAMED_TYPE != named_type::Count) };

            if (THIN_PROFILE.IsArmor())
            {
                return CleanupMaterialPairVectorAndEnsureNotEmpty(
                    MakeForArmor(THIN_PROFILE),
                    THIN_PROFILE,
                    IS_MAGICAL,
                    "MakeForEquipment(armor)");
            }
            else if (THIN_PROFILE.IsWeapon())
            {
                return CleanupMaterialPairVectorAndEnsureNotEmpty(
                    MakeForWeapon(THIN_PROFILE),
                    THIN_PROFILE,
                    IS_MAGICAL,
                    "MakeForEquipment(weapon)");
            }
            else
            {
                return CleanupMaterialPairVectorAndEnsureNotEmpty(
                    MakeForMiscType(THIN_PROFILE.MiscType(), IS_MAGICAL),
                    THIN_PROFILE,
                    IS_MAGICAL,
                    "MakeForEquipment(misc_type)");
            }
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForMiscType(
        const misc_type::Enum MISC_TYPE, const bool IS_MAGICAL) const
    {
        MaterialPairVec_t materialPairs;

        if (MISC_TYPE == misc_type::LockPicks)
        {
            // leave bone/silver/gold/platinum for set_types
            materialPairs = MakePairs(Combine(
                { material::Tin, material::Bronze, material::Iron, material::Steel },
                { material::Obsidian, material::Tooth }));
        }
        else if (
            (MISC_TYPE == misc_type::Spider_Eggs) || (MISC_TYPE == misc_type::Litch_Hand)
            || (MISC_TYPE == misc_type::Egg) || (MISC_TYPE == misc_type::Embryo)
            || (MISC_TYPE == misc_type::Petrified_Snake))
        {
            materialPairs = MakePairs(material::DriedFlesh);
        }
        else if ((MISC_TYPE == misc_type::Wand) || (MISC_TYPE == misc_type::Staff))
        {
            // leave pri=Bone/Tooth/Obsidian/Jade and sec=Diamond for set_types
            materialPairs = MakePairs(
                { material::Claw, material::Horn, material::Wood, material::Stone },
                Combine(
                    fancyJewelWithoutDiamond_,
                    { material::Silver, material::Gold, material::Nothing }));
        }
        else if (MISC_TYPE == misc_type::Angel_Braid)
        {
            materialPairs = MakePairs({ material::Hair }, { material::Spirit });
        }
        else if (MISC_TYPE == misc_type::DrumLute)
        {
            // leave sec=diamond for set_types
            materialPairs = MakePairs(
                { material::Wood },
                Combine(fancyJewelWithoutDiamond_, fancyMetal_, { material::Nothing }));
        }
        else if (MISC_TYPE == misc_type::Figurine_Blessed)
        {
            materialPairs = MakePairs(
                { material::Wood, material::Pearl, material::Glass }, nonFleshEyesWithNothing_);
        }
        else if (MISC_TYPE == misc_type::Figurine_Cursed)
        {
            materialPairs = MakePairs(
                { material::Stone, material::Bone, material::Obsidian }, nonFleshEyesWithNothing_);
        }
        else if ((MISC_TYPE == misc_type::Puppet_Blessed) || (MISC_TYPE == misc_type::Doll_Blessed))
        {
            materialPairs = MakePairs({ material::Wood }, nonFleshEyesWithNothing_);
        }
        else if ((MISC_TYPE == misc_type::Puppet_Cursed) || (MISC_TYPE == misc_type::Doll_Cursed))
        {
            materialPairs = MakePairs({ material::Bone }, nonFleshEyesWithNothing_);
        }
        else if (MISC_TYPE == misc_type::Bust)
        {
            materialPairs = MakePairs({ material::Stone }, nonFleshEyesWithNothing_);
        }
        else if (MISC_TYPE == misc_type::Dried_Head)
        {
            materialPairs = MakePairs({ material::DriedFlesh }, nonFleshEyesWithNothing_);
        }
        else if (MISC_TYPE == misc_type::Goblet)
        {
            materialPairs = MakePairs(
                { material::Tin, material::Bronze, material::Silver, material::Gold },
                AppendNothingCopy(fancyJewel_));
        }
        else if (MISC_TYPE == misc_type::Seeds)
        {
            materialPairs = MakePairs(material::Plant);
        }
        else if (MISC_TYPE == misc_type::Mummy_Hand)
        {
            materialPairs = MakePairs(material::DriedFlesh);
        }
        else if (MISC_TYPE == misc_type::Shard)
        {
            materialPairs = MakePairs(fancyJewel_);
        }
        else if (MISC_TYPE == misc_type::Orb)
        {
            materialPairs
                = MakePairs(fancyJewel_, { material::Wood, material::Stone, material::Obsidian });
        }
        else if (MISC_TYPE == misc_type::Scepter)
        {
            materialPairs = MakePairs(
                { material::Wood,
                  material::Bone,
                  material::Jade,
                  material::Bronze,
                  material::Silver,
                  material::Gold,
                  material::Glass },
                Combine(fancyJewel_, fancyTribal_, { material::Nothing }));
        }
        else if (MISC_TYPE == misc_type::Icicle)
        {
            materialPairs = MakePairs({ material::Glass }, { material::Spirit });
        }
        else if (MISC_TYPE == misc_type::Golem_Finger)
        {
            materialPairs = MakePairs(material::Stone);
        }
        else if (MISC_TYPE == misc_type::Unicorn_Horn)
        {
            materialPairs = MakePairs({ material::Horn }, { material::Spirit });
        }
        else if (MISC_TYPE == misc_type::Devil_Horn)
        {
            materialPairs = MakePairs({ material::Horn }, { material::Spirit });
        }
        else if (
            (MISC_TYPE == misc_type::Recorder) || (MISC_TYPE == misc_type::Viol)
            || (MISC_TYPE == misc_type::Pipe_And_Tabor) || (MISC_TYPE == misc_type::Lyre)
            || (MISC_TYPE == misc_type::Hurdy_Gurdy))
        {
            // leave sec=Diamond for set_types
            materialPairs = MakePairs(
                { material::Wood },
                Combine(fancyJewelWithoutDiamond_, fancyMetal_, { material::Nothing }));
        }
        else if (MISC_TYPE == misc_type::Ring)
        {
            // leave DriedFlesh for set_types
            materialPairs = MakePairs(
                Combine(
                    { material::Glass,
                      material::Claw,
                      material::Bone,
                      material::Wood,
                      material::Stone,
                      material::Jade },
                    metal_),
                AppendNothingCopy(fancyJewel_));
        }
        else if (MISC_TYPE == misc_type::Summoning_Statue)
        {
            materialPairs = MakePairs(
                { material::DriedFlesh,
                  material::Bone,
                  material::Wood,
                  material::Stone,
                  material::Jade,
                  material::Bronze },
                Combine(fancyJewel_, fancyMetal_));
        }
        else if (MISC_TYPE == misc_type::Key)
        {
            materialPairs = MakePairs({ material::Bone,
                                        material::Wood,
                                        material::Stone,
                                        material::Obsidian,
                                        material::Jade,
                                        material::Bronze,
                                        material::Iron,
                                        material::Silver,
                                        material::Gold,
                                        material::Platinum });
        }
        else if (MISC_TYPE == misc_type::Mirror)
        {
            materialPairs = MakePairs({ material::Bone,
                                        material::Wood,
                                        material::Stone,
                                        material::Obsidian,
                                        material::Jade,
                                        material::Silver });
        }
        else if (MISC_TYPE == misc_type::Doll)
        {
            materialPairs
                = MakePairs({ material::Cloth, material::Silk }, AppendNothingCopy(fancyJewel_));
        }
        else if (MISC_TYPE == misc_type::Pendant)
        {
            materialPairs = MakePairs(
                { material::Glass,
                  material::Bone,
                  material::Wood,
                  material::Stone,
                  material::Obsidian,
                  material::Jade,
                  material::Bronze,
                  material::Silver,
                  material::Gold },
                fancyJewel_);
        }

        return CleanupMaterialPairVectorAndEnsureNotEmpty(
            materialPairs, IS_MAGICAL, "MakeForMiscType()");
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForUniqueType(const unique_type::Enum UNIQUE_TYPE) const
    {
        MaterialPairVec_t materialPairs;

        switch (UNIQUE_TYPE)
        {
            case unique_type::BasiliskTonge:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::BerserkersBeard:
            {
                materialPairs = MakePairs(material::Hair);
                break;
            }
            case unique_type::BishopsHanky:
            {
                materialPairs = MakePairs(material::Cloth);
                break;
            }
            case unique_type::BleedingTrophy:
            {
                materialPairs = MakePairs({ material::Gold }, { material::Blood });
                break;
            }
            case unique_type::BloodyDragonScale:
            {
                materialPairs = MakePairs({ material::Scales }, { material::Blood });
                break;
            }
            case unique_type::BottleOfBansheeScreams:
            {
                materialPairs = MakePairs({ material::Glass }, { material::Spirit });
                break;
            }
            case unique_type::BraceletCrown:
            case unique_type::BraceletFeather:
            case unique_type::BraceletFist:
            case unique_type::BraceletHourglass:
            case unique_type::BraceletKey:
            case unique_type::BraceletMask:
            {
                materialPairs = MakePairs({ material::Bronze,
                                            material::Silver,
                                            material::Gold,
                                            material::Platinum,
                                            material::Bone,
                                            material::Tooth,
                                            material::Jade });

                break;
            }
            case unique_type::BroochCrown:
            case unique_type::BroochFeather:
            case unique_type::BroochFist:
            case unique_type::BroochHourglass:
            case unique_type::BroochKey:
            case unique_type::BroochMask:
            {
                materialPairs = MakePairs(
                    { material::Bronze, material::Silver, material::Gold, material::Platinum });

                break;
            }
            case unique_type::BurialShroud:
            {
                materialPairs = MakePairs(material::Cloth);
                break;
            }
            case unique_type::CharmCrown:
            case unique_type::CharmFeather:
            case unique_type::CharmFist:
            case unique_type::CharmHourglass:
            case unique_type::CharmKey:
            case unique_type::CharmMask:
            {
                materialPairs = MakePairs({ material::Bronze,
                                            material::Silver,
                                            material::Gold,
                                            material::Platinum,
                                            material::Jade });

                break;
            }
            case unique_type::ChimeraBone:
            {
                materialPairs = MakePairs(material::Bone);
                break;
            }
            case unique_type::CobraTooth:
            {
                materialPairs = MakePairs(material::Tooth);
                break;
            }
            case unique_type::CommandersCape:
            {
                materialPairs = MakePairs({ material::Cloth }, { material::Silver });
                break;
            }
            case unique_type::BronzeTroll:
            {
                materialPairs = MakePairs(material::Bronze);
                break;
            }
            case unique_type::GlassCat:
            {
                materialPairs = MakePairs(material::Glass);
                break;
            }
            case unique_type::CrystalChimes:
            {
                materialPairs = MakePairs(fancyJewel_);
                break;
            }
            case unique_type::CyclopsEye:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::DemonDiary:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::DoveBloodVial:
            {
                materialPairs = MakePairs(material::Glass);
                break;
            }
            case unique_type::DragonToothWhistle:
            {
                materialPairs = MakePairs(material::Tooth);
                break;
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
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::DruidLeaf:
            {
                materialPairs = MakePairs(material::Plant);
                break;
            }
            case unique_type::EvilRabbitsFoot:
            {
                materialPairs = MakePairs({ material::Bone }, { material::Fur });

                break;
            }
            case unique_type::ExoticGoldenGong:
            {
                materialPairs = MakePairs({ material::Gold }, { material::Wood });
                break;
            }
            case unique_type::FanaticsFlag:
            {
                materialPairs = MakePairs(material::Hide);
                break;
            }
            case unique_type::FriarsChronicle:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::FuneralRecord:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::GeneralsCape:
            {
                materialPairs = MakePairs({ material::Silk }, { material::Gold });
                break;
            }
            case unique_type::GhostSheet:
            {
                materialPairs = MakePairs({ material::Cloth }, { material::Blood });
                break;
            }
            case unique_type::GiantOwlEye:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::GriffinFeather:
            {
                materialPairs = MakePairs(material::Feather);
                break;
            }
            case unique_type::HangmansNoose:
            {
                materialPairs = MakePairs(material::Rope);
                break;
            }
            case unique_type::HawkEye:
            {
                materialPairs = MakePairs(material::Silver);
                break;
            }
            case unique_type::HobgoblinNose:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::HoboRing:
            {
                materialPairs = MakePairs(material::Tin);
                break;
            }
            case unique_type::HolyEpic:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::HornOfTheHorde:
            {
                materialPairs = MakePairs(material::Bronze);
                break;
            }
            case unique_type::ImpTail:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::IslanderHeaddress:
            {
                materialPairs = MakePairs({ material::Wood }, { material::Feather });
                break;
            }
            case unique_type::JeweledAnkhNecklace:
            {
                materialPairs = MakePairs(
                    { material::Stone, material::Bone, material::Obsidian }, fancyJewel_);

                break;
            }
            case unique_type::JeweledArmband:
            {
                materialPairs = MakePairs(
                    { material::Bronze, material::Silver, material::Gold }, fancyJewel_);

                break;
            }
            case unique_type::JeweledHandbag:
            {
                materialPairs = MakePairs({ material::Silk }, fancyJewel_);
                break;
            }
            case unique_type::JeweledPrincessVeil:
            {
                materialPairs = MakePairs({ material::Silver }, fancyJewel_);
                break;
            }
            case unique_type::KingsCape:
            {
                materialPairs = MakePairs({ material::Silk }, { material::Diamond });
                break;
            }
            case unique_type::LastRitesScroll:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::MacabreManuscript:
            {
                materialPairs = MakePairs({ material::Paper }, { material::Blood });
                break;
            }
            case unique_type::MadRatJuju:
            {
                materialPairs = MakePairs({ material::Bronze }, { material::Pearl });
                break;
            }
            case unique_type::MagicHorseshoe:
            {
                materialPairs = MakePairs(material::Iron);
                break;
            }
            case unique_type::MagnifyingGlass:
            {
                materialPairs = MakePairs({ material::Silver }, { material::Glass });
                break;
            }
            case unique_type::ManaAmulet:
            {
                materialPairs = MakePairs(
                    { material::Glass,
                      material::Bone,
                      material::Wood,
                      material::Stone,
                      material::Obsidian,
                      material::Jade,
                      material::Bronze,
                      material::Silver,
                      material::Gold },
                    fancyJewel_);

                break;
            }
            case unique_type::MendicantRing:
            {
                materialPairs = MakePairs(material::Iron);
                break;
            }
            case unique_type::MinotaurHide:
            {
                materialPairs = MakePairs({ material::Hide }, { material::Fur });
                break;
            }
            case unique_type::MonkRing:
            {
                materialPairs = MakePairs(material::Silver);

                break;
            }
            case unique_type::MortuaryOrnament:
            {
                materialPairs = MakePairs(material::Bronze);

                break;
            }
            case unique_type::MournersMask:
            {
                materialPairs = MakePairs({ material::Wood }, { material::Obsidian });

                break;
            }
            case unique_type::PantherPaw:
            {
                materialPairs = MakePairs({ material::DriedFlesh }, { material::Fur });
                break;
            }
            case unique_type::PinCrown:
            case unique_type::PinFeather:
            case unique_type::PinFist:
            case unique_type::PinHourglass:
            case unique_type::PinKey:
            case unique_type::PinMask:
            {
                materialPairs = MakePairs({ material::Iron,
                                            material::Bronze,
                                            material::Silver,
                                            material::Gold,
                                            material::Platinum });

                break;
            }
            case unique_type::PixieBell:
            {
                materialPairs = MakePairs(material::Silver);
                break;
            }
            case unique_type::PriestRing:
            {
                materialPairs = MakePairs({ material::Gold }, { material::Pearl });
                break;
            }
            case unique_type::RascalMask:
            {
                materialPairs = MakePairs(material::Wood);
                break;
            }
            case unique_type::RattlesnakeTail:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::RavenClaw:
            {
                materialPairs = MakePairs(material::Claw);
                break;
            }
            case unique_type::RazorFingerclaw:
            {
                materialPairs = MakePairs(material::Steel);
                break;
            }
            case unique_type::ReaperScythe:
            {
                materialPairs = MakePairs({ material::Wood }, { material::Steel });
                break;
            }
            case unique_type::RegalCaptainsFlag:
            {
                materialPairs = MakePairs(material::Silk);
                break;
            }
            case unique_type::RequiemRegister:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::RoyalScoutSpyglass:
            {
                materialPairs = MakePairs({ material::Silver }, { material::Glass });
                break;
            }
            case unique_type::SaintCameoPin:
            {
                materialPairs = MakePairs({ material::Silver }, { material::Pearl });
                break;
            }
            case unique_type::SaintsJournal:
            {
                materialPairs = MakePairs(material::Paper);
                break;
            }
            case unique_type::SanguineRelic:
            {
                materialPairs = MakePairs({ material::Bronze }, { material::Blood });
                break;
            }
            case unique_type::ScorpionStingerFingerclaw:
            {
                materialPairs = MakePairs(material::Steel);
                break;
            }
            case unique_type::ScoundrelSack:
            {
                materialPairs = MakePairs(material::Leather);
                break;
            }
            case unique_type::SepultureDecoration:
            {
                materialPairs = MakePairs({ material::Wood }, { material::Bronze });
                break;
            }
            case unique_type::ShadeCloak:
            {
                materialPairs = MakePairs(material::Cloth);
                break;
            }
            case unique_type::ShamanRainmaker:
            {
                materialPairs = MakePairs(material::Wood);
                break;
            }
            case unique_type::SharkToothNecklace:
            {
                materialPairs = MakePairs(material::Tooth);
                break;
            }
            case unique_type::SignetCrown:
            case unique_type::SignetFeather:
            case unique_type::SignetFist:
            case unique_type::SignetHourglass:
            case unique_type::SignetKey:
            case unique_type::SignetMask:
            {
                materialPairs = MakePairs({ material::Iron,
                                            material::Bronze,
                                            material::Silver,
                                            material::Gold,
                                            material::Platinum });

                break;
            }
            case unique_type::SirenConch:
            {
                materialPairs = MakePairs(material::Jade);
                break;
            }
            case unique_type::SpecterChains:
            {
                materialPairs = MakePairs(material::Iron);
                break;
            }
            case unique_type::SpecterRobe:
            {
                materialPairs = MakePairs(material::Cloth);
                break;
            }
            case unique_type::SprintersLegtie:
            {
                materialPairs = MakePairs(material::Leather);
                break;
            }
            case unique_type::SwindlersBag:
            {
                materialPairs = MakePairs(material::Leather);
                break;
            }
            case unique_type::TribalFlag:
            {
                materialPairs = MakePairs(material::Hide);
                break;
            }
            case unique_type::TricksterPouch:
            {
                materialPairs = MakePairs(material::Leather);
                break;
            }
            case unique_type::TuningFork:
            {
                materialPairs = MakePairs(material::Silver);
                break;
            }
            case unique_type::TurtleShell:
            {
                materialPairs = MakePairs(material::Bone);
                break;
            }
            case unique_type::VampiresToothNecklace:
            {
                materialPairs = MakePairs(material::Tooth);
                break;
            }
            case unique_type::ViperFangFingerclaw:
            {
                materialPairs = MakePairs(material::Steel);
                break;
            }
            case unique_type::VultureGizzard:
            {
                materialPairs = MakePairs(material::DriedFlesh);
                break;
            }
            case unique_type::WarhorseMarionette:
            {
                materialPairs = MakePairs(material::Wood);
                break;
            }
            case unique_type::WarTrumpet:
            {
                materialPairs = MakePairs(material::Silver);
                break;
            }
            case unique_type::WeaselTotem:
            {
                materialPairs = MakePairs(material::Wood);
                break;
            }
            case unique_type::WolfenFur:
            {
                materialPairs = MakePairs(material::Fur);
                break;
            }
            case unique_type::WraithTalisman:
            {
                materialPairs = MakePairs({ material::Wood }, { material::Obsidian });

                break;
            }
            case unique_type::NotUnique:
            case unique_type::Count:
            default:
            {}
        }

        return CleanupMaterialPairVectorAndEnsureNotEmpty(
            materialPairs, true, "MakeForUniqueType()");
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForWeapon(const ItemProfileThin & THIN_PROFILE) const
    {
        MaterialPairVec_t materialPairs;

        if (THIN_PROFILE.WeaponInfo().IsBodyPart())
        {
            if (THIN_PROFILE.WeaponInfo().IsBite())
            {
                materialPairs.emplace_back(std::make_pair(material::Tooth, material::Nothing));
            }
            else if (THIN_PROFILE.WeaponInfo().IsClaws())
            {
                materialPairs.emplace_back(std::make_pair(material::Claw, material::Nothing));
            }
            else if (THIN_PROFILE.WeaponInfo().IsFists())
            {
                materialPairs.emplace_back(std::make_pair(material::Bone, material::Hide));
            }
            else if (THIN_PROFILE.WeaponInfo().IsTentacles())
            {
                materialPairs.emplace_back(std::make_pair(material::Plant, material::Nothing));
            }
            else if (THIN_PROFILE.WeaponInfo().IsBreath())
            {
                materialPairs.emplace_back(std::make_pair(material::Gas, material::Nothing));
            }
        }
        else if (THIN_PROFILE.WeaponInfo().IsBlowpipe())
        {
            // pri=solid, sec=decoration+N
            materialPairs = MakePairs(
                { material::Wood,
                  material::Bone,
                  material::Jade,
                  material::Bronze,
                  material::Silver },
                AppendNothingCopy(fancy_));
        }
        else if (THIN_PROFILE.WeaponInfo().HasHandle())
        {
            if (THIN_PROFILE.WeaponInfo().IsWhip())
            {
                if (THIN_PROFILE.WeaponInfo().WhipType() == weapon::whip_type::Bullwhip)
                {
                    // pri=very_small_handle, sec=decoration+N
                    materialPairs = MakePairs(verySmallWeaponHandle_, AppendNothingCopy(fancy_));
                }
                else if (THIN_PROFILE.WeaponInfo().WhipType() == weapon::whip_type::Flail)
                {
                    // pri=end, sec=small_handle
                    materialPairs = MakePairs(
                        { material::Horn,
                          material::Bone,
                          material::Wood,
                          material::Obsidian,
                          material::Jade },
                        smallWeaponHandle_);
                }
                else if (THIN_PROFILE.WeaponInfo().WhipType() == weapon::whip_type::MaceAndChain)
                {
                    // pri=end, sec=small_handle
                    materialPairs = MakePairs(
                        { material::Obsidian,
                          material::Diamond,
                          material::Bronze,
                          material::Iron,
                          material::Steel,
                          material::Platinum },
                        smallWeaponHandle_);
                }
            }
            else if (THIN_PROFILE.WeaponInfo().IsProjectile())
            {
                if (THIN_PROFILE.WeaponInfo().ProjectileType() == weapon::projectile_type::Crossbow)
                {
                    // pri=handle, sec=reinforced+N
                    materialPairs
                        = MakePairs(smallWeaponHandle_, AppendNothingCopy(fancyReinforced_));
                }
                else if (
                    THIN_PROFILE.WeaponInfo().ProjectileType() == weapon::projectile_type::Sling)
                {
                    // pri=small_handle, sec=decoration
                    materialPairs = MakePairs(smallWeaponHandle_, AppendNothingCopy(fancy_));
                }
                else if (THIN_PROFILE.WeaponInfo().IsBow())
                {
                    // pri=solid, sec=decoration+N
                    materialPairs = MakePairs(
                        { material::Wood,
                          material::Bone,
                          material::Horn,
                          material::Obsidian,
                          material::Jade,
                          material::Silver },
                        AppendNothingCopy(fancy_));
                }
            }
            else
            {
                // at this point the weapon should be either SWORD/AXE/Knife/Dagger
                // pri=bladed=blade, non-bladed=CLUBs=end, sec=small_handle
                materialPairs = MakePairs(coreBlade_, smallWeaponHandle_);
            }
        }
        else if (THIN_PROFILE.WeaponInfo().IsPole())
        {
            if (THIN_PROFILE.WeaponInfo().Type() & weapon_type::Bladed)
            {
                // at this point the weapon should be a bladedstaff, one of
                // Scythe/Pike/Partisan/Halberd
                // pri=blade, sec=pole_handle
                materialPairs = MakePairs(coreBlade_, corePole_);
            }
            else if (
                ((THIN_PROFILE.WeaponInfo().Type() & weapon_type::Bladed) == 0)
                && (THIN_PROFILE.WeaponInfo().Type() & weapon_type::Pointed))
            {
                // at this point the weapon should be only spears: Spear/ShortSpear
                // pri=handle, sec=tip
                materialPairs = MakePairs(
                    corePole_,
                    Combine(
                        coreBlade_,
                        { material::Tooth, material::Stone, material::Horn },
                        fancyJewel_));
            }
            else
            {
                // at this point the weapon should be only staffs: Staff/Quarterstaff
                // pri=hande, sec=standard fancy reinforced
                materialPairs = MakePairs(corePole_, AppendNothingCopy(fancyReinforced_));
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (materialPairs.empty() == false),
            "item::MaterialFactory::MakeForWeapon(thin_profile={"
                << THIN_PROFILE.ToString() << ") failed to recognize that weapon type.");

        // add extra primary materials for weapons that can be special if only one material
        //(this can add duplicates, but they will be removed below)
        if (THIN_PROFILE.WeaponInfo().WillBeDescribedAsOneSolidMaterial())
        {
            if (THIN_PROFILE.WeaponInfo().IsKnife() || THIN_PROFILE.WeaponInfo().IsDagger())
            {
                for (auto const & MATERIAL_PAIR : MakePairs(Combine(
                         metal_,
                         fancyJewel_,
                         { material::Obsidian,
                           material::Jade,
                           material::Bone,
                           material::Tooth,
                           material::Stone })))
                {
                    materialPairs.emplace_back(MATERIAL_PAIR);
                }
            }
            else if (
                THIN_PROFILE.WeaponInfo().IsSpear() || THIN_PROFILE.WeaponInfo().IsStaff()
                || THIN_PROFILE.WeaponInfo().IsQuarterstaff())
            {
                for (auto const & MATERIAL_PAIR : MakePairs({ material::Wood,
                                                              material::Bronze,
                                                              material::Iron,
                                                              material::Jade,
                                                              material::Bone }))
                {
                    materialPairs.emplace_back(MATERIAL_PAIR);
                }
            }
            else if (
                (THIN_PROFILE.WeaponInfo().SwordType() == weapon::sword_type::Falcata)
                || (THIN_PROFILE.WeaponInfo().ClubType() == weapon::club_type::Mace))
            {
                for (auto const & MATERIAL_PAIR : MakePairs(
                         { material::Bronze, material::Jade, material::Iron, material::Platinum }))
                {
                    materialPairs.emplace_back(MATERIAL_PAIR);
                }
            }
            else if (THIN_PROFILE.WeaponInfo().IsBlowpipe())
            {
                for (auto const & MATERIAL_PAIR :
                     MakePairs({ material::Bronze, material::Jade, material::Iron }))
                {
                    materialPairs.emplace_back(MATERIAL_PAIR);
                }
            }
        }

        return materialPairs;
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForArmor(const ItemProfileThin & THIN_PROFILE) const
    {
        if ((THIN_PROFILE.ArmorInfo().Type() != armor_type::Covering)
            && (THIN_PROFILE.ArmorInfo().Type() != armor_type::Shield)
            && (THIN_PROFILE.ArmorInfo().Type() != armor_type::Helm))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (THIN_PROFILE.ArmorInfo().BaseType() != armor::base_type::Count),
                "item::ItemProfileWarehouse::MakeForArmor("
                "thin_profile={"
                    << THIN_PROFILE.ToString()
                    << "}) -which is a 'non-specific' armor_type that requires a valid base_type "
                       "but the base_type was Count.");
        }

        if (THIN_PROFILE.ArmorInfo().IsCover())
        {
            // pri=fabric sec=clasp (must have clasp so Nothing is not included)
            if (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape)
            {
                return MakePairs({ material::Cloth, material::Silk }, fancyClasp_);
            }
            else
            {
                return MakePairs(
                    { material::Cloth,
                      material::Leather,
                      material::Silk,
                      material::Fur,
                      material::Hide },
                    fancyClasp_);
            }
        }
        else if (THIN_PROFILE.ArmorInfo().IsShield())
        {
            if (THIN_PROFILE.ArmorInfo().ShieldType() == armor::shield_type::Buckler)
            {
                // pri=it, sec=reinforced_armor+Nothing
                return MakePairs(
                    Combine(
                        metal_,
                        { material::Diamond,
                          material::Obsidian,
                          material::Wood,
                          material::Scales,
                          material::Bone,
                          material::Jade }),
                    AppendNothingCopy(fancyReinforcedArmor_));
            }
            else
            {
                // pri=it, sec=reinforce_armor+Nothing
                return MakePairs(
                    { material::Wood,
                      material::Scales,
                      material::Tin,
                      material::Bronze,
                      material::Steel,
                      material::Silver,
                      material::Gold },
                    AppendNothingCopy(fancyReinforcedArmor_));
            }
        }
        else if (THIN_PROFILE.ArmorInfo().IsHelm())
        {
            // pri=leather, sec=reinforced_armor+Nothing except for MailCoif=Nothing

            MaterialVec_t primaries;
            if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Leather)
            {
                primaries = { material::Leather };
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::MailCoif)
            {
                primaries = mailArmorMetals_;
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Kettle)
            {
                primaries = { material::Tin, material::Bronze, material::Iron };
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Archers)
            {
                primaries = { material::Tin, material::Bronze, material::Iron, material::Steel };
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Bascinet)
            {
                primaries = { material::Tin,
                              material::Bronze,
                              material::Iron,
                              material::Steel,
                              material::Silver };
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Great)
            {
                primaries = { material::Tin,   material::Bronze, material::Iron,
                              material::Steel, material::Silver, material::Gold };
            }

            return MakePairs(primaries, AppendNothingCopy(fancyReinforcedArmor_));
        }
        else if (THIN_PROFILE.ArmorInfo().IsGauntlets())
        {
            if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plain)
            {
                // GLOVES pri=it, sec=decoration+N (jewels only)(leave out Diamond for set_types)
                return MakePairs(
                    { material::Cloth, material::Leather, material::Silk, material::Hide },
                    AppendNothingCopy(fancyJewelWithoutDiamond_));
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Mail)
            {
                // pri=it, sec=Nothing
                return MakePairs(Combine(mailArmorMetals_, { material::Gold, material::Platinum }));
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Scale)
            {
                // pri=Scales, sec=reinforced_armor+Nothing
                return MakePairs({ material::Scales }, AppendNothingCopy(fancyReinforcedArmor_));
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plate)
            {
                // pri=plate, sec=reinforced_custom + Nothing (leave out Diamond for set_types)
                return MakePairs(
                    metal_,
                    Combine(
                        fancyJewelWithoutDiamond_,
                        { material::Gold, material::Pearl, material::Nothing }));
            }
        }
        else if (THIN_PROFILE.ArmorInfo().IsPants() || THIN_PROFILE.ArmorInfo().IsShirt())
        {
            // pri=it, sec=clasp (must have clasp so Nothing is not included)
            MaterialVec_t primaryMaterials;

            if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plain)
            {
                primaryMaterials = { material::Cloth,
                                     material::Leather,
                                     material::Silk,
                                     material::Hide,
                                     material::Fur };
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Mail)
            {
                primaryMaterials = mailArmorMetals_;
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Scale)
            {
                primaryMaterials = { material::Scales };
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plate)
            {
                primaryMaterials
                    = { material::Tin, material::Bronze, material::Iron, material::Steel };
            }

            return MakePairs(primaryMaterials, fancyClasp_);
        }
        else if (
            THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsBracers()
            || THIN_PROFILE.ArmorInfo().IsAventail())
        {
            // pri=it, sec=clasp (must have clasp so Nothing is not included)
            MaterialVec_t primaryMaterials;

            if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plain)
            {
                primaryMaterials = { material::Leather, material::Hide };

                if (THIN_PROFILE.ArmorInfo().IsAventail())
                {
                    primaryMaterials.emplace_back(material::Silk);
                }

                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsAventail())
                {
                    primaryMaterials.emplace_back(material::Fur);
                }
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Mail)
            {
                primaryMaterials
                    = Combine(mailArmorMetals_, { material::Gold, material::Platinum });
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Scale)
            {
                primaryMaterials = { material::Scales };
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plate)
            {
                primaryMaterials = metal_;
            }

            return MakePairs(primaryMaterials, fancyClasp_);
        }

        std::ostringstream ss;
        ss << "item::ItemProfileWarehouse::MakeForArmor("
              "thin_profile={"
           << THIN_PROFILE.ToString() << "}) was unable to recognize that armor type.";

        throw std::runtime_error(ss.str());
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForNamedType(const named_type::Enum NAMED_TYPE) const
    {
        // TODO restrict materials for named types to make them special
        if ((NAMED_TYPE == named_type::Robbers) || (NAMED_TYPE == named_type::Thugs)
            || (NAMED_TYPE == named_type::Knaves) || (NAMED_TYPE == named_type::Muggers)
            || (NAMED_TYPE == named_type::Thief) || (NAMED_TYPE == named_type::Pirate))
        {
            return MakePairs(
                Combine(metal_, { material::Bone, material::Steel, material::Obsidian }));
        }
        else
        {
            return {};
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForSetType(
        const ItemProfileThin & THIN_PROFILE, const set_type::Enum SET_TYPE) const
    {
        material::Enum primary{ material::Nothing };
        material::Enum secondary{ material::Nothing };

        switch (SET_TYPE)
        {
            case set_type::TheAssassins:
            case set_type::TheNeverwinter:
            case set_type::TheTickler:
            {
                if (THIN_PROFILE.IsArmor())
                {
                    // Boots/Pants/Shirt/Gloves/Cloak
                    if (SET_TYPE == set_type::TheAssassins)
                    {
                        primary = material::Leather;
                        secondary = material::Bone;
                    }
                    else if (SET_TYPE == set_type::TheNeverwinter)
                    {
                        primary = material::Fur;
                        secondary = material::Silver;
                    }
                    else
                    {
                        primary = material::Leather;
                        secondary = material::Gold;
                    }
                }
                else if (THIN_PROFILE.IsWeapon())
                {
                    // Dagger
                    if (SET_TYPE == set_type::TheAssassins)
                    {
                        primary = material::Bone;
                        secondary = material::Nothing;
                    }
                    else if (SET_TYPE == set_type::TheNeverwinter)
                    {
                        primary = material::Silver;
                        secondary = material::Sapphire;
                    }
                    else
                    {
                        primary = material::Diamond;
                        secondary = material::Nothing;
                    }
                }
                else
                {
                    // Lock Picks
                    if (SET_TYPE == set_type::TheAssassins)
                    {
                        primary = material::Bone;
                    }
                    else if (SET_TYPE == set_type::TheNeverwinter)
                    {
                        primary = material::Silver;
                    }
                    else
                    {
                        primary = material::Gold;
                    }

                    secondary = material::Nothing;
                }

                break;
            }

            case set_type::TheMagus:
            case set_type::TheNecromancers:
            case set_type::TheWarlocks:
            case set_type::TheLichKings:
            {
                // Boots/Pants/Shirt/Gloves/Robe
                if (THIN_PROFILE.IsArmor())
                {
                    if (SET_TYPE == set_type::TheMagus)
                    {
                        primary = material::Cloth;
                        secondary = material::Silver;
                    }
                    else if (SET_TYPE == set_type::TheNecromancers)
                    {
                        primary = material::Leather;
                        secondary = material::Bone;
                    }
                    else if (SET_TYPE == set_type::TheWarlocks)
                    {
                        primary = material::Fur;
                        secondary = material::Ruby;
                    }
                    else if (SET_TYPE == set_type::TheLichKings)
                    {
                        primary = material::Silk;
                        secondary = material::Diamond;
                    }
                }
                else
                {
                    if (SET_TYPE == set_type::TheMagus)
                    {
                        // Wand
                        primary = material::Wood;
                        secondary = material::Silver;
                    }
                    else if (SET_TYPE == set_type::TheNecromancers)
                    {
                        // Orb
                        primary = material::Amethyst;
                        secondary = material::Bone;
                    }
                    else if (SET_TYPE == set_type::TheWarlocks)
                    {
                        // Scepter
                        primary = material::Obsidian;
                        secondary = material::Ruby;
                    }
                    else if (SET_TYPE == set_type::TheLichKings)
                    {
                        // LitchHand
                        primary = material::DriedFlesh;
                        secondary = material::Diamond;
                    }
                }

                break;
            }

            case set_type::TheSages:
            case set_type::TheShamans:
            case set_type::TheOracles:
            case set_type::TheAngelic:
            {
                // Boots/Pants/Shirt/Gloves/Robe
                if (THIN_PROFILE.IsArmor())
                {
                    if (SET_TYPE == set_type::TheSages)
                    {
                        primary = material::Cloth;
                        secondary = material::Amethyst;
                    }
                    else if (SET_TYPE == set_type::TheShamans)
                    {
                        primary = material::Leather;
                        secondary = material::Emerald;
                    }
                    else if (SET_TYPE == set_type::TheOracles)
                    {
                        primary = material::Leather;
                        secondary = material::Lazuli;
                    }
                    else if (SET_TYPE == set_type::TheAngelic)
                    {
                        primary = material::Silk;
                        secondary = material::Diamond;
                    }
                }
                else
                {
                    if (SET_TYPE == set_type::TheSages)
                    {
                        // Shard
                        primary = material::Amethyst;
                        secondary = material::Nothing;
                    }
                    else if (SET_TYPE == set_type::TheShamans)
                    {
                        // Orb
                        primary = material::Emerald;
                        secondary = material::Jade;
                    }
                    else if (SET_TYPE == set_type::TheOracles)
                    {
                        // Scepter
                        primary = material::Lazuli;
                        secondary = material::Pearl;
                    }
                    else if (SET_TYPE == set_type::TheAngelic)
                    {
                        // Angel Braid
                        primary = material::Hair;
                        secondary = material::Diamond;
                    }
                }

                break;
            }

            case set_type::TheBalladeers:
            {
                if (THIN_PROFILE.IsArmor())
                {
                    // Boots/Pants/Shirt
                    primary = material::Cloth;
                }
                else if (THIN_PROFILE.IsWeapon())
                {
                    // Cutlass
                    primary = material::Steel;
                }
                else if (misc_type::IsMusicalInstrument(THIN_PROFILE.MiscType()))
                {
                    // Drumlute
                    primary = material::Wood;
                }

                secondary = material::Silver;
                break;
            }
            case set_type::TheTroubadours:
            {
                if (THIN_PROFILE.IsArmor())
                {
                    if (THIN_PROFILE.ArmorInfo().IsGauntlets())
                    {
                        // Gloves
                        primary = material::Leather;
                    }
                    else
                    {
                        // Mail: Boots/Pants/Shirt
                        primary = material::Silver;
                    }
                }
                else if (THIN_PROFILE.IsWeapon())
                {
                    // Rapier
                    primary = material::Silver;
                }
                else if (misc_type::IsMusicalInstrument(THIN_PROFILE.MiscType()))
                {
                    // Drumlute
                    primary = material::Wood;
                }

                secondary = material::Amethyst;
                break;
            }
            case set_type::TheMuses:
            {
                if (THIN_PROFILE.IsArmor())
                {
                    if ((THIN_PROFILE.ArmorInfo().IsCover())
                        || (THIN_PROFILE.ArmorInfo().IsGauntlets()))
                    {
                        // Gloves/Vest
                        primary = material::Silk;
                    }
                    else
                    {
                        // Mail: Boots/Pants/Shirt
                        primary = material::Silver;
                    }
                }
                else if (THIN_PROFILE.IsWeapon())
                {
                    // Rapier
                    primary = material::Silver;
                }
                else if (misc_type::IsMusicalInstrument(THIN_PROFILE.MiscType()))
                {
                    // Drumlute
                    primary = material::Wood;
                }

                secondary = material::Diamond;
                break;
            }

            case set_type::TheCavaliers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape))
                {
                    // Boots/Pants/Shirt/Gloves/Cape
                    primary = material::Cloth;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsAventail() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Aventail/Bracers/LeatherHelm
                    primary = material::Leather;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // Buckler
                    primary = material::Iron;
                }

                secondary = material::Jade;
                break;
            }
            case set_type::TheChampions:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape))
                {
                    // Boots/Pants/Shirt/Gloves/Cape
                    primary = material::Silk;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsAventail() || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Aventail/Bracers
                    primary = material::Leather;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm() || THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // MailCoif/Kite
                    primary = material::Steel;
                }

                secondary = material::Amethyst;
                break;
            }
            case set_type::ThePaladins:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape)
                    || THIN_PROFILE.ArmorInfo().IsAventail()
                    || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Gloves/Cape/Aventail/Bracers
                    primary = material::Leather;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Kite/Kettle
                    primary = material::Steel;
                }

                secondary = material::Pearl;
                break;
            }
            case set_type::TheBerserkers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape))
                {
                    // Boots/Pants/Shirt/Gloves/Cape
                    primary = material::Fur;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsAventail() || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Aventail/Bracers
                    primary = material::Leather;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm() || THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // MailCoif/Kite
                    primary = material::Silver;
                }

                secondary = material::Emerald;
                break;
            }
            case set_type::TheRosewood:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || THIN_PROFILE.ArmorInfo().IsAventail() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsShield() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Boots/Pants/Shirt/Gloves/Aventail/Bracers/Heater/Bascinet
                    primary = material::Silver;
                }
                else if (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape)
                {
                    // Cape
                    primary = material::Silk;
                }

                secondary = material::Ruby;
                break;
            }
            case set_type::TheDragonslayers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || THIN_PROFILE.ArmorInfo().IsAventail()
                    || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Gloves/Aventail/Bracers
                    primary = material::Scales;
                }
                else if (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape)
                {
                    // Cape
                    primary = material::Leather;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm() || THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // Heater/Bascinet
                    primary = material::Gold;
                }

                secondary = material::Sapphire;
                break;
            }
            case set_type::TheEventideRider:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || THIN_PROFILE.ArmorInfo().IsAventail() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsHelm() || THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // Boots/Pants/Shirt/Gloves/Aventail/Bracers/Pavis/Great
                    primary = material::Platinum;
                }
                else if (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Cape)
                {
                    // Cape
                    primary = material::Fur;
                }

                secondary = material::Diamond;
                break;
            }

            case set_type::TheHunters:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Vest))
                {
                    // Boots/Pants/Shirt/Gloves/Vest
                    primary = material::Silk;
                }
                else if (THIN_PROFILE.ArmorInfo().IsBracers() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Leather/Bracers
                    primary = material::Leather;
                }

                secondary = material::Amethyst;
                break;
            }
            case set_type::TheSureShot:
            {
                // Boots/Pants/Shirt/Gloves/Vest/Bracers/LeatherHelm
                primary = material::Leather;
                secondary = material::Emerald;
                break;
            }
            case set_type::TheMarksmans:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsHelm()
                    || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Bracers/MailCoif
                    primary = material::Steel;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Vest))
                {
                    // Gloves/Vest
                    primary = material::Leather;
                }

                secondary = material::Ruby;
                break;
            }
            case set_type::TheDeadeye:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsHelm()
                    || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Bracers/MailCoif
                    primary = material::Silver;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Vest))
                {
                    // Gloves/Vest
                    primary = material::Leather;
                }

                secondary = material::Lazuli;
                break;
            }
            case set_type::TheDuskRanger:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Bracers
                    primary = material::Scales;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    primary = material::Platinum;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Vest))
                {
                    // Gloves/Vest
                    primary = material::Leather;
                }

                secondary = material::Sapphire;
                break;
            }
            case set_type::TheVenomBow:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers())
                {
                    // Boots/Pants/Shirt/Bracers
                    primary = material::Scales;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    primary = material::Platinum;
                }
                else if (
                    THIN_PROFILE.ArmorInfo().IsGauntlets()
                    || (THIN_PROFILE.ArmorInfo().CoverType() == armor::cover_type::Vest))
                {
                    // Gloves/Vest
                    primary = material::Leather;
                }

                secondary = material::Diamond;
                break;
            }

            case set_type::TheCritterChannelers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsGauntlets())
                {
                    primary = material::Silk;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield())
                {
                    // Buckler
                    primary = material::Steel;
                }
                else if (THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Leather
                    primary = material::Leather;
                }

                secondary = material::Amethyst;
                break;
            }
            case set_type::TheMammalianHead:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsGauntlets())
                {
                    primary = material::Fur;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Buckler/MailCoif
                    primary = material::Silver;
                }

                secondary = material::Emerald;
                break;
            }
            case set_type::TheSavageTaskmasters:
            {
                //.../Kite/Archers
                primary = material::Silver;
                secondary = material::Ruby;
                break;
            }
            case set_type::TheMonsterOverseers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsGauntlets())
                {
                    primary = material::Scales;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Kite/Bascinet
                    primary = material::Silver;
                }

                secondary = material::Sapphire;
                break;
            }
            case set_type::TheBeastRulers:
            {
                if (THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsPants()
                    || THIN_PROFILE.ArmorInfo().IsShirt() || THIN_PROFILE.ArmorInfo().IsBracers()
                    || THIN_PROFILE.ArmorInfo().IsGauntlets())
                {
                    primary = material::Scales;
                }
                else if (THIN_PROFILE.ArmorInfo().IsShield() || THIN_PROFILE.ArmorInfo().IsHelm())
                {
                    // Kite/Bascinet
                    primary = material::Platinum;
                }

                secondary = material::Diamond;
                break;
            }

            case set_type::NotASet:
            case set_type::Count:
            default:
            {
                break;
            }
        }

        return { std::make_pair(primary, secondary) };
    }

    bool MaterialFactory::IsCombinationValid(const MaterialPair_t & MATERIAL_PAIR) const
    {
        auto const PRIMARY{ MATERIAL_PAIR.first };
        auto const SECONDARY{ MATERIAL_PAIR.second };

        if (material::IsSolid(PRIMARY) == false)
        {
            M_HP_LOG_WRN(
                "item::MaterialFactory::IsCombinationValid(" << material::ToString(PRIMARY) << ", "
                                                             << material::ToString(SECONDARY)
                                                             << ") found non-solid primary.");

            return false;
        }

        if (SECONDARY == PRIMARY)
        {
            return false;
        }
        else if (SECONDARY == material::Count)
        {
            M_HP_LOG_WRN(
                "item::MaterialFactory::IsCombinationValid(" << material::ToString(PRIMARY) << ", "
                                                             << material::ToString(SECONDARY)
                                                             << ") found secondary==Count.");

            return false;
        }
        else if (SECONDARY == material::Nothing)
        {
            return true;
        }

        if (material::IsMetal(PRIMARY))
        {
            if (SECONDARY == material::Stone)
            {
                return false;
            }

            if (SECONDARY != material::Gold)
            {
                return false;
            }
        }

        return true;
    }

    const MaterialVec_t MaterialFactory::Combine(
        const MaterialVec_t & VECTOR1,
        const MaterialVec_t & VECTOR2,
        const MaterialVec_t & VECTOR3,
        const MaterialVec_t & VECTOR4) const
    {
        MaterialVec_t materials{ VECTOR1 };
        materials.insert(std::end(materials), std::begin(VECTOR2), std::end(VECTOR2));
        materials.insert(std::end(materials), std::begin(VECTOR3), std::end(VECTOR3));
        materials.insert(std::end(materials), std::begin(VECTOR4), std::end(VECTOR4));
        return materials;
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(const MaterialVec_t & PRIMARIES) const
    {
        return MakePairs(PRIMARIES, { material::Nothing });
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(
        const MaterialVec_t & PRIMARIES, const MaterialVec_t & ORIG_SECONDARIES) const
    {
        MaterialPairVec_t materialPairs;

        auto const FINAL_SECONDARIES{ (
            (ORIG_SECONDARIES.empty()) ? MaterialVec_t{ material::Nothing } : ORIG_SECONDARIES) };

        for (auto const MATERIAL_PRIMARY : Combine(PRIMARIES))
        {
            for (auto const MATERIAL_SECONDARY : Combine(FINAL_SECONDARIES))
            {
                materialPairs.emplace_back(std::make_pair(MATERIAL_PRIMARY, MATERIAL_SECONDARY));
            }
        }

        return materialPairs;
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(const material::Enum MATERIAL) const
    {
        return MaterialPairVec_t({ std::make_pair(MATERIAL, material::Nothing) });
    }

    const MaterialPairVec_t MaterialFactory::RemoveLameMaterialsForSpecialItems(
        const MaterialPairVec_t & MATERIAL_PAIRS) const
    {
        if (MATERIAL_PAIRS.empty())
        {
            return MATERIAL_PAIRS;
        }

        MaterialPairVec_t reducedMaterialPairs;
        reducedMaterialPairs.reserve(MATERIAL_PAIRS.size());

        bool hasPrimaryLeather{ false };
        bool hasPrimarySilk{ false };
        bool hasPrimaryRigidNonWood{ false };
        bool hasPrimarySteel{ false };
        bool hasPrimarySilverOrGold{ false };
        material::Enum onlyPrimary{ MATERIAL_PAIRS.front().first };
        for (auto const & MATERIAL_PAIR : MATERIAL_PAIRS)
        {
            auto const PRIMARY{ MATERIAL_PAIR.first };
            if (PRIMARY == material::Leather)
            {
                hasPrimaryLeather = true;
            }
            else if (PRIMARY == material::Silk)
            {
                hasPrimarySilk = true;
            }
            else if (material::IsRigid(PRIMARY) && (PRIMARY != material::Wood))
            {
                hasPrimaryRigidNonWood = true;
            }

            if (PRIMARY == material::Steel)
            {
                hasPrimarySteel = true;
            }
            else if ((PRIMARY == material::Silver) || (PRIMARY == material::Gold))
            {
                hasPrimarySilverOrGold = true;
            }

            if (PRIMARY != onlyPrimary)
            {
                onlyPrimary = material::Nothing;
            }
        }

        for (auto const & MATERIAL_PAIR : MATERIAL_PAIRS)
        {
            if (onlyPrimary != material::Nothing)
            {
                if (hasPrimarySilk
                    && ((MATERIAL_PAIR.first == material::Leather)
                        || (MATERIAL_PAIR.first == material::Cloth)))
                {
                    continue;
                }
                else if (hasPrimaryLeather && (MATERIAL_PAIR.first == material::Cloth))
                {
                    continue;
                }
                else if (hasPrimaryRigidNonWood && (MATERIAL_PAIR.first == material::Wood))
                {
                    continue;
                }
                else if (hasPrimarySteel && (MATERIAL_PAIR.first == material::Iron))
                {
                    continue;
                }
                else if (
                    (hasPrimarySteel || hasPrimarySilverOrGold)
                    && (MATERIAL_PAIR.first == material::Bronze))
                {
                    continue;
                }
                else if (
                    (MATERIAL_PAIR.first == material::Tin)
                    || (MATERIAL_PAIR.first == material::Amethyst)
                    || (MATERIAL_PAIR.first == material::Emerald))
                {
                    continue;
                }
            }

            reducedMaterialPairs.emplace_back(MATERIAL_PAIR);
        }

        auto const HAS_SECONDARY_RIGID_NONWOOD{ std::find_if(
                                                    std::begin(reducedMaterialPairs),
                                                    std::end(reducedMaterialPairs),
                                                    [](auto const & MATERIAL_PAIR) {
                                                        return (
                                                            material::IsRigid(MATERIAL_PAIR.second)
                                                            && (MATERIAL_PAIR.second
                                                                != material::Wood));
                                                    })
                                                != std::end(reducedMaterialPairs) };

        if (HAS_SECONDARY_RIGID_NONWOOD)
        {
            reducedMaterialPairs.erase(
                std::remove_if(
                    std::begin(reducedMaterialPairs),
                    std::end(reducedMaterialPairs),
                    [](auto const & MATERIAL_PAIR) {
                        return (MATERIAL_PAIR.second == material::Wood);
                    }),
                std::end(reducedMaterialPairs));
        }

        auto const HAS_SECONDARY_RIGID_NOT_BRONZE_TIN_IRON{
            std::find_if(
                std::begin(reducedMaterialPairs),
                std::end(reducedMaterialPairs),
                [](auto const & MATERIAL_PAIR) {
                    return (
                        material::IsRigid(MATERIAL_PAIR.second)
                        && ((MATERIAL_PAIR.second != material::Tin)
                            && (MATERIAL_PAIR.second != material::Bronze)
                            && (MATERIAL_PAIR.second != material::Iron)));
                })
            != std::end(reducedMaterialPairs)
        };

        if (HAS_SECONDARY_RIGID_NOT_BRONZE_TIN_IRON)
        {
            reducedMaterialPairs.erase(
                std::remove_if(
                    std::begin(reducedMaterialPairs),
                    std::end(reducedMaterialPairs),
                    [](auto const & MATERIAL_PAIR) {
                        return (
                            material::IsRigid(MATERIAL_PAIR.second)
                            && ((MATERIAL_PAIR.second == material::Tin)
                                || (MATERIAL_PAIR.second == material::Bronze)
                                || (MATERIAL_PAIR.second == material::Iron)));
                    }),
                std::end(reducedMaterialPairs));
        }

        return reducedMaterialPairs;
    }

    const MaterialVec_t MaterialFactory::RemoveCopy(
        const MaterialVec_t & ORIG_MATERIALS, const material::Enum MATERIAL_TO_REMOVE) const
    {
        MaterialVec_t materials{ ORIG_MATERIALS };

        materials.erase(
            std::remove(std::begin(materials), std::end(materials), MATERIAL_TO_REMOVE),
            std::end(materials));

        return materials;
    }

    const MaterialPairVec_t MaterialFactory::CleanupMaterialPairVectorAndEnsureNotEmpty(
        const MaterialPairVec_t & MATERIAL_PAIRS,
        const bool IS_MAGICAL,
        const std::string & ERROR_MSG) const
    {
        MaterialPairVec_t materialPairs{ MATERIAL_PAIRS };

        std::remove_if(
            std::begin(materialPairs), std::end(materialPairs), [&](auto const & MATERIAL_PAIR) {
                return (IsCombinationValid(MATERIAL_PAIR) == false);
            });

        if (IS_MAGICAL)
        {
            RemoveLameMaterialsForSpecialItems(materialPairs);
        }

        std::sort(std::begin(materialPairs), std::end(materialPairs));

        materialPairs.erase(
            std::unique(std::begin(materialPairs), std::end(materialPairs)),
            std::end(materialPairs));

        M_ASSERT_OR_LOGANDTHROW_SS(
            (materialPairs.empty() == false),
            "item::MaterialFactory::CleanupMaterialPairVectorAndEnsureNotEmpty(is_magical="
                << std::boolalpha << IS_MAGICAL
                << ") ended up with an empty material pair vector.  error_msg=\"" << ERROR_MSG
                << "\"");

        return materialPairs;
    }

} // namespace item
} // namespace heroespath
