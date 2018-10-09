// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// materials-factory.cpp
//
#include "materials-factory.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"

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
        , smallWeaponHandle_({ material::Wood, material::Bone })
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
        , invalidMetalsMap_()
    {
        invalidMetalsMap_[material::Tin] = { material::Bronze, material::Iron, material::Steel };
        invalidMetalsMap_[material::Bronze] = { material::Tin, material::Iron, material::Steel };
        invalidMetalsMap_[material::Iron] = { material::Tin, material::Bronze, material::Steel };
        invalidMetalsMap_[material::Steel] = { material::Tin, material::Bronze, material::Iron };

        invalidMetalsMap_[material::Silver]
            = { material::Tin, material::Bronze, material::Iron, material::Steel };

        invalidMetalsMap_[material::Gold]
            = { material::Tin, material::Bronze, material::Iron, material::Steel };

        invalidMetalsMap_[material::Platinum] = {
            material::Tin, material::Bronze, material::Iron, material::Steel, material::Silver
        };
    }

    const MaterialPairVec_t MaterialFactory::MakeForEquipment(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        const set_type::Enum SET_TYPE)
    {
        MaterialPairVec_t materialPairs;

        if ((SET_TYPE != set_type::Count) && (SET_TYPE != set_type::Not))
        {
            materialPairs = MakeForSetType(THIN_PROFILE, SET_TYPE);
        }
        else if (THIN_PROFILE.IsArmor())
        {
            materialPairs = MakeForArmor(THIN_PROFILE);
        }
        else if (THIN_PROFILE.IsWeapon())
        {
            materialPairs = MakeForWeapon(THIN_PROFILE);
        }
        else if (THIN_PROFILE.IsMisc())
        {
            materialPairs = MakeForMiscType(THIN_PROFILE.MiscType(), false);
        }

        if ((NAMED_TYPE != named_type::Not) && (NAMED_TYPE != named_type::Count))
        {
            LimitForNamedType(THIN_PROFILE, NAMED_TYPE, materialPairs);
        }

        const auto MATERIAL_PAIRS_COPY { materialPairs };

        try
        {
            const auto ALLOW_INVALID_MATERIALS_FOR_ITEMS_WITH_INVERTED_MATERIALS { (
                THIN_PROFILE.NameMaterialType() == name_material_type::Handle) };

            // even though set_types and named_types are magical, they are very specific about
            // their materials so there is no need to pass true=WILL_REMOVE_LAME_MATERIALS
            CleanupMaterialPairVectorAndEnsureNotEmpty(
                materialPairs, false, ALLOW_INVALID_MATERIALS_FOR_ITEMS_WITH_INVERTED_MATERIALS);
        }
        catch (const std::exception & EXCEPTION)
        {
            std::ostringstream ss;
            ss << EXCEPTION.what()
               << " --this was thrown during "
                  "item::MaterialFactory::MakeForEquipment(thin_profile={"
               << THIN_PROFILE.ToString() << "}, named_type=" << named_type::ToString(NAMED_TYPE)
               << ", set_type=" << set_type::ToString(SET_TYPE) << ")";

            for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS_COPY)
            {
                ss << "\n(" << material::ToString(MATERIAL_PAIR.first) << ", "
                   << material::ToString(MATERIAL_PAIR.second) << ")\t" << std::boolalpha
                   << IsCombinationValid(MATERIAL_PAIR);
            }

            throw std::runtime_error(ss.str());
        }

        return materialPairs;
    }

    const MaterialPairVec_t MaterialFactory::MakeForMiscType(
        const misc_type::Enum MISC_TYPE, const bool IS_MAGICAL) const
    {
        MaterialPairVec_t materialPairs { MakeForMiscInner(MISC_TYPE) };

        const auto MATERIAL_PAIRS_COPY { materialPairs };

        try
        {
            const auto ALLOW_INVALID_MATERIALS_FOR_SPECIAL_ITEM {
                (MISC_TYPE == misc_type::ExoticGoldenGong) || (MISC_TYPE == misc_type::MadRatJuju)
                || (MISC_TYPE == misc_type::MagnifyingGlass)
                || (MISC_TYPE == misc_type::RoyalScoutSpyglass)
            };

            CleanupMaterialPairVectorAndEnsureNotEmpty(
                materialPairs, IS_MAGICAL, ALLOW_INVALID_MATERIALS_FOR_SPECIAL_ITEM);
        }
        catch (const std::exception & EXCEPTION)
        {
            std::ostringstream ss;
            ss << EXCEPTION.what()
               << " --this was thrown during item::MaterialFactory::MakeForMiscType(misc_type="
               << misc_type::ToString(MISC_TYPE) << ", is_magical=" << std::boolalpha << IS_MAGICAL
               << ")";

            for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS_COPY)
            {
                ss << "\n(" << material::ToString(MATERIAL_PAIR.first) << ", "
                   << material::ToString(MATERIAL_PAIR.second) << ")\t" << std::boolalpha
                   << IsCombinationValid(MATERIAL_PAIR);
            }

            throw std::runtime_error(ss.str());
        }

        return materialPairs;
    }

    const MaterialPairVec_t MaterialFactory::MakeForMiscInner(const misc_type::Enum MISC_TYPE) const
    {
        switch (MISC_TYPE)
        {
            // quest items first, be specific with these because there is only one of each
            case misc_type::AngelBraid:
            {
                return MakePairs({ material::Hair }, { material::Spirit });
            }
            case misc_type::Crumhorn: { return MakePairs(material::Bone);
            }
            case misc_type::DevilHorn: { return MakePairs({ material::Horn }, { material::Spirit });
            }
            case misc_type::GolemFinger: { return MakePairs(material::Stone);
            }
            case misc_type::HurdyGurdy: { return MakePairs({ material::Wood }, { material::Ruby });
            }
            case misc_type::Icicle: { return MakePairs(material::Glass);
            }
            case misc_type::LitchHand: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::Lyre: { return MakePairs({ material::Gold }, { material::Diamond });
            }
            case misc_type::MummyHand: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::PetrifiedSnake: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::PipeAndTabor:
            {
                return MakePairs({ material::Wood }, { material::Diamond });
            }
            case misc_type::Recorder: { return MakePairs({ material::Wood }, { material::Pearl });
            }
            case misc_type::UnicornHorn:
            {
                return MakePairs({ material::Horn }, { material::Spirit });
            }
            case misc_type::Viol: { return MakePairs({ material::Wood }, { material::Obsidian });
            }

            // unique items, so again, be specific
            case misc_type::BasiliskTonge: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::BerserkersBeard: { return MakePairs(material::Hair);
            }
            case misc_type::BishopsHanky: { return MakePairs(material::Cloth);
            }
            case misc_type::BleedingTrophy:
            {
                return MakePairs({ material::Gold }, { material::Blood });
            }
            case misc_type::BloodyDragonScale:
            {
                return MakePairs({ material::Scales }, { material::Blood });
            }
            case misc_type::BottleOfBansheeScreams:
            {
                return MakePairs({ material::Glass }, { material::Spirit });
            }
            case misc_type::BronzeTroll: { return MakePairs(material::Bronze);
            }
            case misc_type::Brooch_Crown:
            case misc_type::Brooch_Feather:
            case misc_type::Brooch_Fist:
            case misc_type::Brooch_Hourglass:
            case misc_type::Brooch_Key:
            case misc_type::Brooch_Mask:
            {
                return MakePairs(
                    { material::Bronze, material::Silver, material::Gold, material::Platinum });
            }
            case misc_type::BurialShroud: { return MakePairs(material::Cloth);
            }
            case misc_type::CapeCommanders:
            {
                return MakePairs({ material::Cloth }, { material::Silver });
            }
            case misc_type::CapeGenerals:
            {
                return MakePairs({ material::Silk }, { material::Gold });
            }
            case misc_type::CapeKings:
            {
                return MakePairs({ material::Silk }, { material::Diamond });
            }
            case misc_type::Charm_Crown:
            case misc_type::Charm_Feather:
            case misc_type::Charm_Fist:
            case misc_type::Charm_Hourglass:
            case misc_type::Charm_Key:
            case misc_type::Charm_Mask:
            {
                return MakePairs({ material::Bronze,
                                   material::Silver,
                                   material::Gold,
                                   material::Platinum,
                                   material::Jade });
            }
            case misc_type::ChimeraBone: { return MakePairs(material::Bone);
            }
            case misc_type::CobraTooth: { return MakePairs(material::Tooth);
            }
            case misc_type::CrystalChimes: { return MakePairs(fancyJewel_);
            }
            case misc_type::DemonDiary: { return MakePairs(material::Paper);
            }
            case misc_type::DoveBloodVial: { return MakePairs(material::Glass);
            }
            case misc_type::DragonToothWhistle: { return MakePairs(material::Tooth);
            }
            case misc_type::DriedFrog:
            case misc_type::DriedGecko:
            case misc_type::DriedIguana:
            case misc_type::DriedLizard:
            case misc_type::DriedSalamander:
            case misc_type::DriedSkink:
            case misc_type::DriedToad: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::DruidLeaf: { return MakePairs(material::Plant);
            }
            case misc_type::EvilRabbitsFoot:
            {
                return MakePairs({ material::Bone }, { material::Fur });
            }
            case misc_type::ExoticGoldenGong:
            {
                return MakePairs({ material::Gold }, { material::Wood });
            }
            case misc_type::EyeCyclops: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::EyeGiantOwl: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::EyeHawk: { return MakePairs(material::Silver);
            }
            case misc_type::FlagFanatics: { return MakePairs(material::Hide);
            }
            case misc_type::FlagRegalCaptains: { return MakePairs(material::Silk);
            }
            case misc_type::FlagTribal: { return MakePairs(material::Hide);
            }
            case misc_type::FriarsChronicle: { return MakePairs(material::Paper);
            }
            case misc_type::FuneralRecord: { return MakePairs(material::Paper);
            }
            case misc_type::GhostSheet:
            {
                return MakePairs({ material::Cloth }, { material::Blood });
            }
            case misc_type::GlassCat: { return MakePairs(material::Glass);
            }
            case misc_type::GriffinFeather: { return MakePairs(material::Feather);
            }
            case misc_type::HangmansNoose: { return MakePairs(material::Rope);
            }
            case misc_type::HobgoblinNose: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::HolyEpic: { return MakePairs(material::Paper);
            }
            case misc_type::HornOfTheHorde: { return MakePairs(material::Bronze);
            }
            case misc_type::ImpTail: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::IslanderHeaddress:
            {
                return MakePairs({ material::Wood }, { material::Feather });
            }
            case misc_type::JeweledArmband:
            {
                return MakePairs(
                    { material::Bronze, material::Silver, material::Gold }, fancyJewel_);
            }
            case misc_type::JeweledHandbag: { return MakePairs({ material::Silk }, fancyJewel_);
            }
            case misc_type::JeweledPrincessVeil:
            {
                return MakePairs({ material::Silver }, fancyJewel_);
            }
            case misc_type::LastRitesScroll: { return MakePairs(material::Paper);
            }
            case misc_type::MacabreManuscript:
            {
                return MakePairs({ material::Paper }, { material::Blood });
            }
            case misc_type::MadRatJuju:
            {
                return MakePairs({ material::Bronze }, { material::Pearl });
            }
            case misc_type::MagicHorseshoe: { return MakePairs(material::Iron);
            }
            case misc_type::MagnifyingGlass:
            {
                return MakePairs({ material::Silver }, { material::Glass });
            }
            case misc_type::ManaAmulet:
            {
                return MakePairs(
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
            case misc_type::MaskMourners:
            {
                return MakePairs({ material::Wood }, { material::Obsidian });
            }
            case misc_type::MaskRascal: { return MakePairs(material::Wood);
            }
            case misc_type::MinotaurHide: { return MakePairs({ material::Hide }, { material::Fur });
            }
            case misc_type::MortuaryOrnament: { return MakePairs(material::Bronze);
            }
            case misc_type::NecklaceJeweledAnkh:
            {
                return MakePairs(
                    { material::Stone, material::Bone, material::Obsidian }, fancyJewel_);
            }
            case misc_type::NecklaceSharkTooth: { return MakePairs(material::Tooth);
            }
            case misc_type::NecklaceVampiresTooth: { return MakePairs(material::Tooth);
            }
            case misc_type::PantherPaw:
            {
                return MakePairs({ material::DriedFlesh }, { material::Fur });
            }
            case misc_type::Pin_Crown:
            case misc_type::Pin_Feather:
            case misc_type::Pin_Fist:
            case misc_type::Pin_Hourglass:
            case misc_type::Pin_Key:
            case misc_type::Pin_Mask:
            {
                return MakePairs({ material::Iron,
                                   material::Bronze,
                                   material::Silver,
                                   material::Gold,
                                   material::Platinum });
            }
            case misc_type::PixieBell: { return MakePairs(material::Silver);
            }
            case misc_type::RattlesnakeTail: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::RavenClaw: { return MakePairs(material::Claw);
            }
            case misc_type::ReaperScythe:
            {
                return MakePairs({ material::Wood }, { material::Steel });
            }
            case misc_type::RequiemRegister: { return MakePairs(material::Paper);
            }
            case misc_type::RingHobo: { return MakePairs(material::Tin);
            }
            case misc_type::RingMendicant: { return MakePairs(material::Iron);
            }
            case misc_type::RingMonk: { return MakePairs(material::Silver);
            }
            case misc_type::RingPriest: { return MakePairs({ material::Gold }, { material::Pearl });
            }
            case misc_type::RoyalScoutSpyglass:
            {
                return MakePairs({ material::Silver }, { material::Glass });
            }
            case misc_type::SaintCameoPin:
            {
                return MakePairs({ material::Silver }, { material::Pearl });
            }
            case misc_type::SaintsJournal: { return MakePairs(material::Paper);
            }
            case misc_type::SanguineRelic:
            {
                return MakePairs({ material::Bronze }, { material::Blood });
            }
            case misc_type::ScoundrelSack: { return MakePairs(material::Leather);
            }
            case misc_type::SepultureDecoration:
            {
                return MakePairs({ material::Wood }, { material::Bronze });
            }
            case misc_type::ShadeCloak: { return MakePairs(material::Cloth);
            }
            case misc_type::ShamanRainmaker: { return MakePairs(material::Wood);
            }
            case misc_type::SirenConch: { return MakePairs(material::Jade);
            }
            case misc_type::SpecterChains: { return MakePairs(material::Iron);
            }
            case misc_type::SpecterRobe: { return MakePairs(material::Cloth);
            }
            case misc_type::SpiderEggs: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::SprintersLegtie: { return MakePairs(material::Leather);
            }
            case misc_type::SwindlersBag: { return MakePairs(material::Leather);
            }
            case misc_type::TricksterPouch: { return MakePairs(material::Leather);
            }
            case misc_type::TuningFork: { return MakePairs(material::Silver);
            }
            case misc_type::TurtleShell: { return MakePairs(material::Bone);
            }
            case misc_type::VultureGizzard: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::WarhorseMarionette: { return MakePairs(material::Wood);
            }
            case misc_type::WarTrumpet: { return MakePairs(material::Silver);
            }
            case misc_type::WeaselTotem: { return MakePairs(material::Wood);
            }
            case misc_type::WolfenFur: { return MakePairs(material::Fur);
            }
            case misc_type::WraithTalisman:
            {
                return MakePairs({ material::Wood }, { material::Obsidian });
            }

            // standalone items
            case misc_type::Bust: { return MakePairs({ material::Stone }, nonFleshEyesWithNothing_);
            }
            case misc_type::Doll:
            {
                return MakePairs(
                    { material::Cloth, material::Silk }, AppendNothingCopy(fancyJewel_));
            }
            case misc_type::PuppetBlessed:
            case misc_type::DollBlessed:
            {
                return MakePairs({ material::Wood }, nonFleshEyesWithNothing_);
            }
            case misc_type::PuppetCursed:
            case misc_type::DollCursed:
            {
                return MakePairs({ material::Bone }, nonFleshEyesWithNothing_);
            }
            case misc_type::DriedHead:
            {
                return MakePairs({ material::DriedFlesh }, nonFleshEyesWithNothing_);
            }
            case misc_type::DrumLute:
            {
                // NOT a quest item
                // some Drumlutes are made by set_types and those materials are set below
                // leave sec=silver/amethyst/diamond for set_types
                return MakePairs(
                    { material::Wood },
                    Combine({ material::Emerald,
                              material::Ruby,
                              material::Sapphire,
                              material::Gold,
                              material::Nothing }));
            }
            case misc_type::Egg:
            case misc_type::Embryo: { return MakePairs(material::DriedFlesh);
            }
            case misc_type::FigurineBlessed:
            {
                return MakePairs(
                    { material::Wood, material::Pearl, material::Glass }, nonFleshEyesWithNothing_);
            }
            case misc_type::FigurineCursed:
            {
                return MakePairs(
                    { material::Stone, material::Bone, material::Obsidian },
                    nonFleshEyesWithNothing_);
            }

            case misc_type::Goblet:
            {
                return MakePairs(
                    { material::Tin, material::Bronze, material::Silver, material::Gold },
                    AppendNothingCopy(fancyJewel_));
            }
            case misc_type::Key:
            {
                return MakePairs({ material::Bone,
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
            case misc_type::LockPicks:
            {
                // leave bone/silver/gold/platinum for set_types
                return MakePairs(Combine(
                    { material::Tin, material::Bronze, material::Iron, material::Steel },
                    { material::Obsidian, material::Tooth }));
            }
            case misc_type::Mirror:
            {
                return MakePairs({ material::Bone,
                                   material::Wood,
                                   material::Stone,
                                   material::Obsidian,
                                   material::Jade,
                                   material::Silver });
            }
            case misc_type::Orb:
            {
                return MakePairs(
                    fancyJewel_, { material::Wood, material::Stone, material::Obsidian });
            }
            case misc_type::Pendant:
            {
                return MakePairs(
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
            case misc_type::Scepter:
            {
                return MakePairs(
                    { material::Wood,
                      material::Bone,
                      material::Jade,
                      material::Bronze,
                      material::Silver,
                      material::Gold,
                      material::Glass },
                    Combine(fancyJewel_, fancyTribal_, { material::Nothing }));
            }
            case misc_type::Seeds: { return MakePairs(material::Plant);
            }
            case misc_type::Shard: { return MakePairs(fancyJewel_);
            }
            case misc_type::Staff:
            {
                return MakePairs(
                    { material::Claw, material::Horn, material::Wood, material::Stone },
                    AppendNothingCopy(fancyReinforced_));
            }
            case misc_type::SummoningStatue:
            {
                return MakePairs(
                    { material::Bone,
                      material::Wood,
                      material::Stone,
                      material::Jade,
                      material::Bronze },
                    Combine(fancyJewel_, fancyMetal_));
            }
            case misc_type::Wand:
            {
                // leave pri=Bone/Tooth/Obsidian/Jade and sec=Diamond for set_types
                return MakePairs(
                    { material::Claw, material::Horn, material::Wood, material::Stone },
                    Combine(
                        fancyJewelWithoutDiamond_,
                        { material::Silver, material::Gold, material::Nothing }));
            }
            case misc_type::Ring:
            {
                // leave DriedFlesh for set_types
                return MakePairs(
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
            case misc_type::Not:
            case misc_type::Count:
            default: { return {};
            }
        }
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
                    // pri=very_small_handle, sec=decoration
                    materialPairs = MakePairs(verySmallWeaponHandle_, AppendNothingCopy(fancy_));
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
                // at this point the weapon should be either SWORD/CLUB/AXE/Knife/Dagger
                // pri=bladed=blade, non-bladed=CLUBs=end, sec=small_handle
                if (THIN_PROFILE.WeaponInfo().IsKnife() || THIN_PROFILE.WeaponInfo().IsDagger())
                {
                    materialPairs = MakePairs(coreBlade_, verySmallWeaponHandle_);
                }
                else
                {
                    materialPairs = MakePairs(coreBlade_, smallWeaponHandle_);
                }
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

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (materialPairs.empty() == false),
            "item::MaterialFactory::MakeForWeapon(thin_profile={"
                << THIN_PROFILE.ToString() << ") failed to recognize that weapon type.");

        // add extra primary materials for weapons that can be special if only one material
        //(this can add duplicates, but they will be removed below)
        if (THIN_PROFILE.WeaponInfo().WillBeDescribedAsOneSolidMaterial())
        {
            if (THIN_PROFILE.WeaponInfo().IsKnife() || THIN_PROFILE.WeaponInfo().IsDagger())
            {
                for (const auto & MATERIAL_PAIR : MakePairs(Combine(
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
                for (const auto & MATERIAL_PAIR : MakePairs({ material::Wood,
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
                for (const auto & MATERIAL_PAIR : MakePairs(
                         { material::Bronze, material::Jade, material::Iron, material::Platinum }))
                {
                    materialPairs.emplace_back(MATERIAL_PAIR);
                }
            }
            else if (THIN_PROFILE.WeaponInfo().IsBlowpipe())
            {
                for (const auto & MATERIAL_PAIR :
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
            M_HP_ASSERT_OR_LOG_AND_THROW(
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
                primaries = { material::Tin,   material::Bronze, material::Iron,
                              material::Steel, material::Silver, material::Platinum };
            }
            else if (THIN_PROFILE.ArmorInfo().HelmType() == armor::helm_type::Great)
            {
                primaries
                    = { material::Tin,    material::Bronze, material::Iron,    material::Steel,
                        material::Silver, material::Gold,   material::Platinum };
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
                    Combine(fancyJewelWithoutDiamond_, { material::Pearl, material::Nothing }));
            }
        }
        else if (THIN_PROFILE.ArmorInfo().IsPants() || THIN_PROFILE.ArmorInfo().IsShirt())
        {
            // pri=it, sec=clasp (must have clasp so Nothing is not included) (except for Mail...)
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
                return MakePairs(mailArmorMetals_);
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Scale)
            {
                primaryMaterials = { material::Scales };
            }
            else if (THIN_PROFILE.ArmorInfo().BaseType() == armor::base_type::Plate)
            {
                primaryMaterials = { material::Tin,
                                     material::Bronze,
                                     material::Iron,
                                     material::Steel,
                                     material::Platinum };
            }

            return MakePairs(primaryMaterials, fancyClasp_);
        }
        else if (
            THIN_PROFILE.ArmorInfo().IsBoots() || THIN_PROFILE.ArmorInfo().IsBracers()
            || THIN_PROFILE.ArmorInfo().IsAventail())
        {
            // pri=it, sec=clasp (must have clasp so Nothing is not included) (except for Mail)
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
                return MakePairs(Combine(mailArmorMetals_, { material::Gold, material::Platinum }));
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

    void MaterialFactory::LimitForNamedType(
        const ItemProfileThin & THIN_PROFILE,
        const named_type::Enum NAMED_TYPE,
        MaterialPairVec_t & materialPairs) const
    {
        const auto MATERIAL_PAIRS_COPY { materialPairs };

        enum class WhichMaterial
        {
            Pri,
            Sec
        };

        enum class MaterialOp
        {
            KeepOnly,
            ExcludeAnyOf
        };

        auto processByMaterials { [&](const MaterialOp OPERATION,
                                      const WhichMaterial WHICH,
                                      const MaterialVec_t & MATERIALS_TO_KEEP) {
            materialPairs.erase(
                std::remove_if(
                    std::begin(materialPairs),
                    std::end(materialPairs),
                    [&](const auto MATERIAL_PAIR) {
                        return (
                            (std::find(
                                 std::begin(MATERIALS_TO_KEEP),
                                 std::end(MATERIALS_TO_KEEP),
                                 ((WHICH == WhichMaterial::Pri) ? MATERIAL_PAIR.first
                                                                : MATERIAL_PAIR.second))
                             == std::end(MATERIALS_TO_KEEP))
                            == (OPERATION == MaterialOp::KeepOnly));
                    }),
                std::end(materialPairs));
        } };

        auto keepOnlyPrimary { [&](const MaterialVec_t & MATERIALS_TO_KEEP) {
            processByMaterials(MaterialOp::KeepOnly, WhichMaterial::Pri, MATERIALS_TO_KEEP);
        } };

        auto keepOnlySecondary { [&](const MaterialVec_t & MATERIALS_TO_KEEP) {
            processByMaterials(MaterialOp::KeepOnly, WhichMaterial::Sec, MATERIALS_TO_KEEP);
        } };

        auto keepOnlyPrimaryAndSecondary { [&](const MaterialVec_t & MATERIALS_TO_KEEP) {
            processByMaterials(MaterialOp::KeepOnly, WhichMaterial::Pri, MATERIALS_TO_KEEP);
            processByMaterials(MaterialOp::KeepOnly, WhichMaterial::Sec, MATERIALS_TO_KEEP);
        } };

        auto keepOnlySecondaryIncludingNothing { [&](const MaterialVec_t & MATERIALS_TO_KEEP) {
            processByMaterials(
                MaterialOp::KeepOnly, WhichMaterial::Sec, AppendNothingCopy(MATERIALS_TO_KEEP));
        } };

        auto excludeAnyOfPrimary { [&](const MaterialVec_t & MATERIALS_TO_EXCLUDE) {
            processByMaterials(MaterialOp::ExcludeAnyOf, WhichMaterial::Pri, MATERIALS_TO_EXCLUDE);
        } };

        auto excludeAnyOfSecondary { [&](const MaterialVec_t & MATERIALS_TO_EXCLUDE) {
            processByMaterials(MaterialOp::ExcludeAnyOf, WhichMaterial::Sec, MATERIALS_TO_EXCLUDE);
        } };

        auto excludeAnyOfPrimaryAndSecondary { [&](const MaterialVec_t & MATERIALS_TO_EXCLUDE) {
            processByMaterials(MaterialOp::ExcludeAnyOf, WhichMaterial::Pri, MATERIALS_TO_EXCLUDE);
            processByMaterials(MaterialOp::ExcludeAnyOf, WhichMaterial::Sec, MATERIALS_TO_EXCLUDE);
        } };

        auto appendWoodIfHasHandleCopy { [&](const MaterialVec_t MATERIALS) {
            if (THIN_PROFILE.NameMaterialType() == name_material_type::Handle)
            {
                return AppendCopy(MATERIALS, material::Wood);
            }
            else
            {
                return MATERIALS;
            }
        } };

        switch (NAMED_TYPE)
        {
            case named_type::Betrayer:
            {
                excludeAnyOfPrimaryAndSecondary({ material::Pearl,
                                                  material::Diamond,
                                                  material::Gold,
                                                  material::Jade,
                                                  material::Lazuli,
                                                  material::Tin,
                                                  material::Platinum,
                                                  material::Silver });

                break;
            }
            case named_type::Dancing:
            {
                keepOnlyPrimary(
                    { material::Diamond, material::Iron, material::Steel, material::Platinum });

                keepOnlySecondary({ material::Wood, material::Pearl });

                break;
            }
            case named_type::Dark:
            {
                excludeAnyOfPrimaryAndSecondary({ material::Pearl,
                                                  material::Diamond,
                                                  material::Gold,
                                                  material::Jade,
                                                  material::Lazuli,
                                                  material::Tin,
                                                  material::Platinum,
                                                  material::Silver });

                break;
            }
            case named_type::Diabolic:
            {
                keepOnlyPrimaryAndSecondary({ material::Diamond,
                                              material::Wood,
                                              material::Steel,
                                              material::Obsidian,
                                              material::Bronze,
                                              material::Bone });

                break;
            }
            case named_type::Fiendish:
            {
                keepOnlyPrimary(
                    { material::Steel, material::Obsidian, material::Platinum, material::Diamond });

                keepOnlySecondaryIncludingNothing(
                    { material::Wood, material::Bone, material::Iron });

                break;
            }
            case named_type::Focus:
            {
                keepOnlyPrimary({ material::Wood, material::Horn, material::Silver });

                keepOnlySecondary({ material::Diamond,
                                    material::Silver,
                                    material::Gold,
                                    material::Obsidian,
                                    material::Claw });

                break;
            }
            case named_type::Icy:
            case named_type::Winter:
            case named_type::Frigid:
            case named_type::Arctic:
            case named_type::Chill:
            case named_type::Frozen:
            {
                if (THIN_PROFILE.WeaponInfo().WhipType() == weapon::whip_type::Flail)
                {
                    // materials will be all combinations of wood/bone/horn/obsidian/jade
                    excludeAnyOfPrimaryAndSecondary({ material::Horn, material::Jade });
                }
                else
                {
                    excludeAnyOfSecondary(fancyTribal_);

                    keepOnlyPrimaryAndSecondary({ material::Wood,
                                                  material::Fur,
                                                  material::Hide,
                                                  material::Sapphire,
                                                  material::Diamond,
                                                  material::Silver,
                                                  material::Iron,
                                                  material::Steel,
                                                  material::Platinum,
                                                  material::Lazuli,
                                                  material::Scales,
                                                  material::Nothing });
                }
                break;
            }
            case named_type::Army:
            {
                excludeAnyOfSecondary(fancyTribal_);

                excludeAnyOfPrimaryAndSecondary(Combine(
                    fancyJewel_,
                    fancyOpaque_,
                    fancyMetal_,
                    { material::Platinum, material::Silk, material::Bronze }));

                break;
            }
            case named_type::Gladiator:
            {
                excludeAnyOfPrimaryAndSecondary(Combine(
                    RemoveCopy(fancy_, material::Scales), { material::Silk, material::Fur }));

                break;
            }
            case named_type::Soldiers:
            {
                excludeAnyOfPrimaryAndSecondary(
                    AppendCopy(RemoveCopy(fancy_, material::Scales), material::Silk));

                break;
            }
            case named_type::Light:
            case named_type::Dawn:
            case named_type::Sun:
            case named_type::Proud:
            case named_type::Pure:
            case named_type::Glory:
            {
                excludeAnyOfSecondary({ material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { material::Silk, material::Cloth, material::Scales, material::Nothing },
                    {
                        material::Steel,
                        material::Silver,
                        material::Gold,
                        material::Platinum,
                    },
                    { material::Diamond, material::Amethyst, material::Pearl, material::Wood }));

                break;
            }

            case named_type::Daring:
            {
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, material::Scales));

                excludeAnyOfPrimaryAndSecondary(Combine(
                    { material::Silk, material::Platinum },
                    fancyMetal_,
                    fancyOpaque_,
                    RemoveCopy(fancyJewel_, material::Amethyst)));

                break;
            }
            case named_type::Honest:
            {
                excludeAnyOfSecondary({ material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, material::Scales));

                excludeAnyOfPrimaryAndSecondary(Combine(
                    { material::Silk, material::Platinum },
                    fancyMetal_,
                    RemoveCopy(fancyOpaque_, material::Pearl),
                    RemoveCopy(fancyJewel_, material::Amethyst)));

                break;
            }
            case named_type::Elite:
            case named_type::Noble:
            case named_type::Valiant:
            {
                excludeAnyOfSecondary({ material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { material::Pearl,
                      material::Silk,
                      material::Platinum,
                      material::Scales,
                      material::Nothing },
                    fancyMetal_,
                    RemoveCopy(fancyJewel_, material::Amethyst)));

                break;
            }
            case named_type::Heros:
            {
                excludeAnyOfSecondary({ material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { material::Diamond,
                      material::Pearl,
                      material::Silk,
                      material::Platinum,
                      material::Scales,
                      material::Nothing },
                    fancyMetal_));

                break;
            }
            case named_type::Charlatans:
            {
                keepOnlyPrimaryAndSecondary(
                    { material::Silk, material::Silver, material::Nothing });

                break;
            }
            case named_type::Burglar:
            {
                keepOnlyPrimaryAndSecondary(
                    { material::Leather, material::Steel, material::Nothing });

                break;
            }
            case named_type::Mountebank:
            {
                keepOnlyPrimaryAndSecondary({ material::Fur, material::Steel, material::Nothing });
                break;
            }
            case named_type::Pickpocket:
            {
                keepOnlyPrimaryAndSecondary(
                    { material::Leather, material::Tin, material::Nothing });

                break;
            }
            case named_type::Imposters:
            {
                keepOnlyPrimaryAndSecondary({ material::Cloth, material::Iron, material::Nothing });
                break;
            }
            case named_type::Infernal:
            {
                if (THIN_PROFILE.WeaponInfo().IsSword())
                {
                    keepOnlyPrimary(
                        { material::Diamond, material::Bronze, material::Steel, material::Gold });

                    keepOnlySecondaryIncludingNothing({ material::Wood, material::Bone });
                }
                else if (
                    THIN_PROFILE.WeaponInfo().ProjectileType() == weapon::projectile_type::Sling)
                {
                    keepOnlyPrimary({ material::Wood, material::Bone });

                    excludeAnyOfSecondary({ material::Stone,
                                            material::Horn,
                                            material::Jade,
                                            material::Amethyst,
                                            material::Emerald,
                                            material::Sapphire,
                                            material::Silver });
                }
                else if (THIN_PROFILE.WeaponInfo().IsBow())
                {
                    excludeAnyOfPrimary({ material::Obsidian, material::Jade, material::Silver });

                    excludeAnyOfSecondary({ material::Jade,
                                            material::Amethyst,
                                            material::Emerald,
                                            material::Sapphire,
                                            material::Silver });
                }

                break;
            }
            case named_type::Marauder:
            {
                keepOnlyPrimary({ material::Iron, material::Steel, material::Platinum });
                keepOnlySecondary({ material::Wood });
                break;
            }
            case named_type::Gloom:
            case named_type::Twilight:
            case named_type::Dusk:
            case named_type::Sorrow:
            case named_type::Woe:
            case named_type::Misery:
            {
                excludeAnyOfPrimaryAndSecondary({ material::Pearl,
                                                  material::Diamond,
                                                  material::Gold,
                                                  material::Jade,
                                                  material::Lazuli,
                                                  material::Silk,
                                                  material::Fur,
                                                  material::Tin,
                                                  material::Platinum });

                break;
            }
            case named_type::Moon:
            {
                excludeAnyOfPrimaryAndSecondary({ material::Pearl,
                                                  material::Diamond,
                                                  material::Gold,
                                                  material::Jade,
                                                  material::Lazuli,
                                                  material::Fur,
                                                  material::Tin });

                break;
            }
            case named_type::Nile:
            {
                keepOnlyPrimaryAndSecondary({ material::Cloth,
                                              material::Leather,
                                              material::Steel,
                                              material::Bronze,
                                              material::Iron,
                                              material::Gold,
                                              material::Lazuli,
                                              material::Nothing });

                break;
            }
            case named_type::Thief:
            case named_type::Robbers:
            {
                keepOnlyPrimaryAndSecondary({ material::Stone,
                                              material::Iron,
                                              material::Steel,
                                              material::Wood,
                                              material::Nothing });

                break;
            }
            case named_type::Muggers:
            case named_type::Thugs:
            {
                keepOnlyPrimaryAndSecondary(
                    { material::Iron, material::Steel, material::Wood, material::Nothing });

                break;
            }
            case named_type::Knaves:
            {
                keepOnlyPrimaryAndSecondary({ material::Steel, material::Wood, material::Nothing });

                break;
            }
            case named_type::Pirate:
            {
                keepOnlyPrimaryAndSecondary(
                    { material::Obsidian, material::Silver, material::Bone, material::Nothing });

                break;
            }
            case named_type::Princes:
            {
                keepOnlyPrimaryAndSecondary(appendWoodIfHasHandleCopy(Combine(
                    { material::Pearl,
                      material::Silk,
                      material::Platinum,
                      material::Steel,
                      material::Nothing },
                    fancyMetal_,
                    fancyJewel_)));

                break;
            }
            case named_type::Raging:
            {
                keepOnlyPrimary(
                    { material::Diamond, material::Iron, material::Steel, material::Platinum });

                excludeAnyOfSecondary({ material::Pearl });
                break;
            }
            case named_type::Ranger:
            {
                keepOnlyPrimaryAndSecondary({ material::Wood,
                                              material::Steel,
                                              material::Leather,
                                              material::Emerald,
                                              material::Scales,
                                              material::Nothing });

                break;
            }
            case named_type::Samurai:
            {
                keepOnlyPrimaryAndSecondary({ material::Silk,
                                              material::Leather,
                                              material::Wood,
                                              material::Steel,
                                              material::Scales,
                                              material::Jade,
                                              material::Nothing });

                break;
            }
            case named_type::Charred:
            case named_type::Burning:
            case named_type::Fiery:
            case named_type::Searing:
            {
                keepOnlyPrimaryAndSecondary({ material::Wood,
                                              material::Leather,
                                              material::Claw,
                                              material::Scales,
                                              material::Tooth,
                                              material::Bone,
                                              material::Steel,
                                              material::Bronze,
                                              material::Iron,
                                              material::Platinum,
                                              material::Ruby,
                                              material::Diamond,
                                              material::Nothing });

                break;
            }
            case named_type::Thors:
            {
                keepOnlyPrimaryAndSecondary(appendWoodIfHasHandleCopy({ material::Steel,
                                                                        material::Silk,
                                                                        material::Pearl,
                                                                        material::Platinum,
                                                                        material::Gold,
                                                                        material::Diamond,
                                                                        material::Nothing }));

                break;
            }
            case named_type::Wardens:
            {
                excludeAnyOfPrimaryAndSecondary(
                    Combine(fancy_, { material::Silk, material::Fur, material::Steel }));

                break;
            }
            case named_type::Wicked:
            {
                if (THIN_PROFILE.WeaponInfo().IsSpear())
                {
                    excludeAnyOfPrimary({ material::Jade, material::Bronze });
                    excludeAnyOfSecondary({ material::Stone,
                                            material::Horn,
                                            material::Jade,
                                            material::Bronze,
                                            material::Amethyst,
                                            material::Emerald,
                                            material::Sapphire,
                                            material::Gold });
                }
                else
                {
                    keepOnlyPrimary({ material::Steel,
                                      material::Obsidian,
                                      material::Platinum,
                                      material::Diamond });
                    keepOnlySecondaryIncludingNothing(
                        { material::Wood, material::Bone, material::Iron });
                }

                break;
            }
            case named_type::Not:
            case named_type::Count:
            default: { break;
            }
        }

        if (materialPairs.empty())
        {
            std::ostringstream ss;
            ss << "item::MaterialFactory::LimitForNamedType(named_type="
               << named_type::ToString(NAMED_TYPE) << ", thin_profile={" << THIN_PROFILE.ToString()
               << "}) resulted in no materials.  Original materials:";

            for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS_COPY)
            {
                ss << "\n(" << material::ToString(MATERIAL_PAIR.first) << ", "
                   << material::ToString(MATERIAL_PAIR.second) << ")\t" << std::boolalpha
                   << IsCombinationValid(MATERIAL_PAIR);
            }

            throw std::runtime_error(ss.str());
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForSetType(
        const ItemProfileThin & THIN_PROFILE, const set_type::Enum SET_TYPE) const
    {
        material::Enum primary { material::Nothing };
        material::Enum secondary { material::Nothing };

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

            case set_type::Not:
            case set_type::Count:
            default: { break;
            }
        }

        return { std::make_pair(primary, secondary) };
    }

    bool MaterialFactory::IsCombinationValid(const MaterialPair_t & MATERIAL_PAIR) const
    {
        const auto PRIMARY { MATERIAL_PAIR.first };
        const auto SECONDARY { MATERIAL_PAIR.second };

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

            if (material::IsMetal(SECONDARY))
            {
                MaterialVec_t invalidSecondaries;
                invalidMetalsMap_.Find(PRIMARY, invalidSecondaries);

                if (std::find(
                        std::begin(invalidSecondaries), std::end(invalidSecondaries), SECONDARY)
                    != std::end(invalidSecondaries))
                {
                    return false;
                }
            }

            if ((material::IsFancyOpaque(SECONDARY) == false) && (SECONDARY != material::Blood)
                && (material::IsFancyJewel(SECONDARY) == false))
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
        const MaterialVec_t & VECTOR4,
        const MaterialVec_t & VECTOR5) const
    {
        MaterialVec_t materials { VECTOR1 };
        materials.insert(std::end(materials), std::begin(VECTOR2), std::end(VECTOR2));
        materials.insert(std::end(materials), std::begin(VECTOR3), std::end(VECTOR3));
        materials.insert(std::end(materials), std::begin(VECTOR4), std::end(VECTOR4));
        materials.insert(std::end(materials), std::begin(VECTOR5), std::end(VECTOR5));
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

        const auto FINAL_SECONDARIES { (
            (ORIG_SECONDARIES.empty()) ? MaterialVec_t { material::Nothing } : ORIG_SECONDARIES) };

        for (const auto MATERIAL_PRIMARY : Combine(PRIMARIES))
        {
            for (const auto MATERIAL_SECONDARY : Combine(FINAL_SECONDARIES))
            {
                if (MATERIAL_PRIMARY != MATERIAL_SECONDARY)
                {
                    materialPairs.emplace_back(
                        std::make_pair(MATERIAL_PRIMARY, MATERIAL_SECONDARY));
                }
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

        bool hasPrimaryLeather { false };
        bool hasPrimarySilk { false };
        bool hasPrimaryRigidNonWood { false };
        bool hasPrimarySteel { false };
        bool hasPrimarySilverOrGold { false };
        material::Enum onlyPrimary { MATERIAL_PAIRS.front().first };
        for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS)
        {
            const auto PRIMARY { MATERIAL_PAIR.first };
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

        for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS)
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

        const auto HAS_SECONDARY_RIGID_NONWOOD { std::find_if(
                                                     std::begin(reducedMaterialPairs),
                                                     std::end(reducedMaterialPairs),
                                                     [](const auto & MATERIAL_PAIR) {
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
                    [](const auto & MATERIAL_PAIR) {
                        return (MATERIAL_PAIR.second == material::Wood);
                    }),
                std::end(reducedMaterialPairs));
        }

        const auto HAS_SECONDARY_RIGID_NOT_BRONZE_TIN_IRON {
            std::find_if(
                std::begin(reducedMaterialPairs),
                std::end(reducedMaterialPairs),
                [](const auto & MATERIAL_PAIR) {
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
                    [](const auto & MATERIAL_PAIR) {
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
        MaterialVec_t materials { ORIG_MATERIALS };

        materials.erase(
            std::remove(std::begin(materials), std::end(materials), MATERIAL_TO_REMOVE),
            std::end(materials));

        return materials;
    }

    void MaterialFactory::CleanupMaterialPairVectorAndEnsureNotEmpty(
        MaterialPairVec_t & materialPairs,
        const bool WILL_REMOVE_LAME_MATERIALS,
        const bool ALLOW_INVALID) const
    {
        if (ALLOW_INVALID == false)
        {
            materialPairs.erase(
                std::remove_if(
                    std::begin(materialPairs),
                    std::end(materialPairs),
                    [&](const auto & MATERIAL_PAIR) {
                        return (IsCombinationValid(MATERIAL_PAIR) == false);
                    }),
                std::end(materialPairs));
        }

        if (WILL_REMOVE_LAME_MATERIALS)
        {
            RemoveLameMaterialsForSpecialItems(materialPairs);
        }

        std::sort(std::begin(materialPairs), std::end(materialPairs));

        materialPairs.erase(
            std::unique(std::begin(materialPairs), std::end(materialPairs)),
            std::end(materialPairs));

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (materialPairs.empty() == false),
            "item::MaterialFactory::CleanupMaterialPairVectorAndEnsureNotEmpty(will_remove_lame="
                << std::boolalpha << WILL_REMOVE_LAME_MATERIALS << ", allow_invalid="
                << ALLOW_INVALID << ") ended up with an empty material pair vector.");
    }

} // namespace item
} // namespace heroespath
