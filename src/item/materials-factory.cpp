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

#include "item/item-template.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace item
{

    std::unique_ptr<MaterialFactory> MaterialFactory::instanceUPtr_;

    const MaterialVec_t MaterialFactory::metal_ { Material::Tin,     Material::Bronze,
                                                  Material::Iron,    Material::Steel,
                                                  Material::Silver,  Material::Gold,
                                                  Material::Platinum };

    const MaterialVec_t MaterialFactory::fancyJewel_ {
        Material::Amethyst, Material::Emerald, Material::Ruby, Material::Sapphire, Material::Diamond
    };

    const MaterialVec_t MaterialFactory::fancyMetal_ { Material::Silver, Material::Gold };

    const MaterialVec_t MaterialFactory::fancyOpaque_ { Material::Material::Obsidian,
                                                        Material::Material::Pearl,
                                                        Material::Material::Jade,
                                                        Material::Material::Lazuli };

    const MaterialVec_t MaterialFactory::fancyTribal_ {
        Material::Claw, Material::Bone, Material::Tooth, Material::Scales
    };

    const MaterialVec_t MaterialFactory::coreBlade_ { Material::Obsidian, Material::Jade,
                                                      Material::Diamond,  Material::Bronze,
                                                      Material::Iron,     Material::Steel,
                                                      Material::Platinum };

    const MaterialVec_t MaterialFactory::smallWeaponHandle_ { Material::Wood, Material::Bone };

    const MaterialVec_t MaterialFactory::corePole_ {
        Material::Wood, Material::Jade, Material::Bronze, Material::Iron, Material::Silver
    };

    const MaterialVec_t MaterialFactory::mailArmorMetals_ {
        Material::Tin, Material::Bronze, Material::Iron, Material::Steel, Material::Silver
    };

    MaterialFactory::MaterialFactory()
        : fancyJewelWithoutDiamond_()
        , fancy_()
        , verySmallWeaponHandle_()
        , fancyReinforced_()
        , fancyReinforcedArmor_()
        , fancyClasp_()
        , nonFleshEyesWithNothing_()
        , invalidMetalsMap_()
    {
        M_HP_LOG_DBG("Subsystem Construction: MaterialFactory");

        invalidMetalsMap_.Append(
            Material::Tin, { Material::Bronze, Material::Iron, Material::Steel });

        invalidMetalsMap_.Append(
            Material::Bronze, { Material::Tin, Material::Iron, Material::Steel });

        invalidMetalsMap_.Append(
            Material::Iron, { Material::Tin, Material::Bronze, Material::Steel });

        invalidMetalsMap_.Append(
            Material::Steel, { Material::Tin, Material::Bronze, Material::Iron });

        invalidMetalsMap_.Append(
            Material::Silver, { Material::Tin, Material::Bronze, Material::Iron, Material::Steel });

        invalidMetalsMap_.Append(
            Material::Gold, { Material::Tin, Material::Bronze, Material::Iron, Material::Steel });

        invalidMetalsMap_.Append(
            Material::Platinum,
            { Material::Tin, Material::Bronze, Material::Iron, Material::Steel, Material::Silver });
    }

    MaterialFactory::~MaterialFactory() { M_HP_LOG_DBG("Subsystem Destruction: MaterialFactory"); }

    misc::NotNull<MaterialFactory *> MaterialFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: MaterialFactory");
            Create();
        }

        return misc::NotNull<MaterialFactory *>(instanceUPtr_.get());
    }

    void MaterialFactory::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<MaterialFactory>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: MaterialFactory");
        }
    }

    void MaterialFactory::Destroy() { instanceUPtr_.reset(); }

    void MaterialFactory::Initialize()
    {
        fancyJewelWithoutDiamond_ = RemoveCopy(fancyJewel_, Material::Diamond);

        fancy_ = Combine(fancyJewel_, fancyMetal_, fancyOpaque_, fancyTribal_);

        verySmallWeaponHandle_
            = Combine({ Material::Wood, Material::Bone, Material::Horn }, fancyOpaque_);

        fancyReinforced_ = Combine(fancy_, { Material::Bronze, Material::Iron });

        fancyReinforcedArmor_ = Combine(fancy_, metal_);

        fancyClasp_ = fancyReinforcedArmor_;

        nonFleshEyesWithNothing_ = Combine(fancyJewel_, { Material::Count });
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(
        const ItemTemplate & TEMPLATE,
        const Misc::Enum MISC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const bool IS_MAGICAL_IN_OTHER_WAYS)
    {
        MaterialPairVec_t pairs;

        if ((MISC < Misc::Count) && (NAMED >= Named::Count) && (SET >= Set::Count))
        {
            pairs = MakeForMiscType(MISC, IS_MAGICAL_IN_OTHER_WAYS);
        }
        else
        {
            pairs = MakeForEquipment(TEMPLATE, NAMED, SET);
        }

        std::sort(std::begin(pairs), std::end(pairs));

        pairs.erase(std::unique(std::begin(pairs), std::end(pairs)), std::end(pairs));

        M_HP_ASSERT_OR_LOG_AND_THROW_SS(
            (!pairs.empty()),
            "No materials were generated for: "
                << TEMPLATE.ToString() << ", MISC=" << NAMEOF_ENUM(MISC)
                << ", NAMED=" << NAMEOF_ENUM(NAMED) << ", SET=" << NAMEOF_ENUM(SET)
                << ", IS_MAGICAL_IN_OTHER_WAYS=" << std::boolalpha << IS_MAGICAL_IN_OTHER_WAYS);

        return pairs;
    }

    const MaterialPair_t MaterialFactory::MakePairForBodyPartWeapon(
        const BodyPartWeapons::Enum BODY_PART_WEAPON_TYPE) const
    {
        switch (BODY_PART_WEAPON_TYPE)
        {
            case BodyPartWeapons::Claws: return std::make_pair(Material::Claw, Material::Count);

            case BodyPartWeapons::Tentacles:
                return std::make_pair(Material::Plant, Material::Count);

            case BodyPartWeapons::Bite: return std::make_pair(Material::Tooth, Material::Count);

            case BodyPartWeapons::BreathOfFire:
            case BodyPartWeapons::BreathOfFrost:
                return std::make_pair(Material::Gas, Material::Count);

            case BodyPartWeapons::Fists:
            case BodyPartWeapons::Count:
            default: return std::make_pair(Material::Bone, Material::Hide);
        }
    }

    const MaterialPair_t
        MaterialFactory::MakePairForSkinArmor(const creature::race::Enum RACE) const
    {
        if ((RACE == creature::race::Troll) || (RACE == creature::race::Boar)
            || (RACE == creature::race::ThreeHeadedHound) || (RACE == creature::race::Ramonaut)
            || (RACE == creature::race::Wereboar) || (RACE == creature::race::LionBoar))
        {
            return std::make_pair(Material::Hide, Material::Fur);
        }
        else if (
            (RACE == creature::race::Wolfen) || (RACE == creature::race::Minotaur)
            || (RACE == creature::race::Lion) || (RACE == creature::race::Werebear)
            || (RACE == creature::race::Werewolf))
        {
            return std::make_pair(Material::Fur, Material::Hide);
        }
        else if (
            (RACE == creature::race::Dragon) || (RACE == creature::race::Hydra)
            || (RACE == creature::race::LizardWalker) || (RACE == creature::race::Naga)
            || (RACE == creature::race::Serpent) || (RACE == creature::race::Cobra)
            || (RACE == creature::race::Wyvern))
        {
            return std::make_pair(Material::Scales, Material::Count);
        }
        else if (RACE == creature::race::Plant)
        {
            return std::make_pair(Material::Plant, Material::Count);
        }
        else
        {
            return std::make_pair(Material::Count, Material::Count);
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForEquipment(
        const ItemTemplate & TEMPLATE, const Named::Enum NAMED, const Set::Enum SET)
    {
        MaterialPairVec_t pairs;

        if (SET < Set::Count)
        {
            pairs = MakeForSetType(TEMPLATE, SET);
        }
        else if (TEMPLATE.IsArmor())
        {
            pairs = MakeForArmor(TEMPLATE);
        }
        else if (TEMPLATE.IsWeapon())
        {
            pairs = MakeForWeapon(TEMPLATE);
        }
        else if (TEMPLATE.IsMisc())
        {
            pairs = MakeForMiscType(TEMPLATE.MiscTypeBase(), false);
        }

        if (NAMED < Named::Count)
        {
            LimitForNamedType(TEMPLATE, NAMED, pairs);
        }

        const auto ALLOW_INVALID_MATERIALS_FOR_ITEMS_WITH_INVERTED_MATERIALS { (
            TEMPLATE.GetMaterialNameStyle() == MaterialNameStyle::Handle) };

        // even though Sets and Nameds are magical, they are very specific about
        // their materials so there is no need to pass true=WILL_REMOVE_LAME_MATERIALS
        CleanupMaterialPairs(
            pairs, false, ALLOW_INVALID_MATERIALS_FOR_ITEMS_WITH_INVERTED_MATERIALS);

        return pairs;
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForMiscType(const Misc::Enum MISC, const bool IS_MAGICAL) const
    {
        MaterialPairVec_t pairs { MakeForMiscInner(MISC) };

        const auto ALLOW_INVALID_MATERIALS_FOR_SPECIAL_ITEM {
            (MISC == Misc::ExoticGoldenGong) || (MISC == Misc::MadRatJuju)
            || (MISC == Misc::MagnifyingGlass) || (MISC == Misc::RoyalScoutSpyglass)
        };

        CleanupMaterialPairs(pairs, IS_MAGICAL, ALLOW_INVALID_MATERIALS_FOR_SPECIAL_ITEM);

        return pairs;
    }

    const MaterialPairVec_t MaterialFactory::MakeForMiscInner(const Misc::Enum MISC) const
    {
        switch (MISC)
        {
            // quest items first, be specific with these because there is only one of each
            case Misc::AngelBraid:
            {
                return MakePairs({ Material::Hair }, { Material::Spirit });
            }
            case Misc::Crumhorn:
            {
                return MakePairs(Material::Bone);
            }
            case Misc::DevilHorn:
            {
                return MakePairs({ Material::Horn }, { Material::Spirit });
            }
            case Misc::GolemFinger:
            {
                return MakePairs(Material::Stone);
            }
            case Misc::HurdyGurdy:
            {
                return MakePairs({ Material::Wood }, { Material::Ruby });
            }
            case Misc::Icicle:
            {
                return MakePairs(Material::Glass);
            }
            case Misc::LichHand:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::Lyre:
            {
                return MakePairs({ Material::Gold }, { Material::Diamond });
            }
            case Misc::MummyHand:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::PetrifiedSnake:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::PipeAndTabor:
            {
                return MakePairs({ Material::Wood }, { Material::Diamond });
            }
            case Misc::Recorder:
            {
                return MakePairs({ Material::Wood }, { Material::Pearl });
            }
            case Misc::UnicornHorn:
            {
                return MakePairs({ Material::Horn }, { Material::Spirit });
            }
            case Misc::Viol:
            {
                return MakePairs({ Material::Wood }, { Material::Obsidian });
            }

            // unique items, so again, be specific
            case Misc::BasiliskTongue:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::BerserkersBeard:
            {
                return MakePairs(Material::Hair);
            }
            case Misc::BishopsHanky:
            {
                return MakePairs(Material::Cloth);
            }
            case Misc::BleedingTrophy:
            {
                return MakePairs({ Material::Gold }, { Material::Blood });
            }
            case Misc::BloodyDragonScale:
            {
                return MakePairs({ Material::Scales }, { Material::Blood });
            }
            case Misc::BottleOfBansheeScreams:
            {
                return MakePairs({ Material::Glass }, { Material::Spirit });
            }
            case Misc::BronzeTroll:
            {
                return MakePairs(Material::Bronze);
            }
            case Misc::BroochCrown:
            case Misc::BroochFeather:
            case Misc::BroochFist:
            case Misc::BroochHourglass:
            case Misc::BroochKey:
            case Misc::BroochMask:
            {
                return MakePairs(
                    { Material::Bronze, Material::Silver, Material::Gold, Material::Platinum });
            }
            case Misc::BurialShroud:
            {
                return MakePairs(Material::Cloth);
            }
            case Misc::CapeCommanders:
            {
                return MakePairs({ Material::Cloth }, { Material::Silver });
            }
            case Misc::CapeGenerals:
            {
                return MakePairs({ Material::Silk }, { Material::Gold });
            }
            case Misc::CapeKings:
            {
                return MakePairs({ Material::Silk }, { Material::Diamond });
            }
            case Misc::CharmCrown:
            case Misc::CharmFeather:
            case Misc::CharmFist:
            case Misc::CharmHourglass:
            case Misc::CharmKey:
            case Misc::CharmMask:
            {
                return MakePairs({ Material::Bronze,
                                   Material::Silver,
                                   Material::Gold,
                                   Material::Platinum,
                                   Material::Jade });
            }
            case Misc::ChimeraBone:
            {
                return MakePairs(Material::Bone);
            }
            case Misc::CobraTooth:
            {
                return MakePairs(Material::Tooth);
            }
            case Misc::CrystalChimes:
            {
                return MakePairs(fancyJewel_);
            }
            case Misc::DemonDiary:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::DoveBloodVial:
            {
                return MakePairs(Material::Glass);
            }
            case Misc::DragonToothWhistle:
            {
                return MakePairs(Material::Tooth);
            }
            case Misc::DriedFrog:
            case Misc::DriedGecko:
            case Misc::DriedIguana:
            case Misc::DriedLizard:
            case Misc::DriedSalamander:
            case Misc::DriedSkink:
            case Misc::DriedToad:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::DruidLeaf:
            {
                return MakePairs(Material::Plant);
            }
            case Misc::EvilRabbitsFoot:
            {
                return MakePairs({ Material::Bone }, { Material::Fur });
            }
            case Misc::ExoticGoldenGong:
            {
                return MakePairs({ Material::Gold }, { Material::Wood });
            }
            case Misc::EyeCyclops:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::EyeGiantOwl:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::EyeHawk:
            {
                return MakePairs(Material::Silver);
            }
            case Misc::FlagFanatics:
            {
                return MakePairs(Material::Hide);
            }
            case Misc::FlagRegalCaptains:
            {
                return MakePairs(Material::Silk);
            }
            case Misc::FlagTribal:
            {
                return MakePairs(Material::Hide);
            }
            case Misc::FriarsChronicle:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::FuneralRecord:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::GhostSheet:
            {
                return MakePairs({ Material::Cloth }, { Material::Blood });
            }
            case Misc::GlassCat:
            {
                return MakePairs(Material::Glass);
            }
            case Misc::GriffinFeather:
            {
                return MakePairs(Material::Feather);
            }
            case Misc::HangmansNoose:
            {
                return MakePairs(Material::Rope);
            }
            case Misc::HobgoblinNose:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::HolyEpic:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::HornOfTheHorde:
            {
                return MakePairs(Material::Bronze);
            }
            case Misc::ImpTail:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::IslanderHeaddress:
            {
                return MakePairs({ Material::Wood }, { Material::Feather });
            }
            case Misc::JeweledArmband:
            {
                return MakePairs(
                    { Material::Bronze, Material::Silver, Material::Gold }, fancyJewel_);
            }
            case Misc::JeweledHandbag:
            {
                return MakePairs({ Material::Silk }, fancyJewel_);
            }
            case Misc::JeweledPrincessVeil:
            {
                return MakePairs({ Material::Silver }, fancyJewel_);
            }
            case Misc::LastRitesScroll:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::MacabreManuscript:
            {
                return MakePairs({ Material::Paper }, { Material::Blood });
            }
            case Misc::MadRatJuju:
            {
                return MakePairs({ Material::Bronze }, { Material::Pearl });
            }
            case Misc::MagicHorseshoe:
            {
                return MakePairs(Material::Iron);
            }
            case Misc::MagnifyingGlass:
            {
                return MakePairs({ Material::Silver }, { Material::Glass });
            }
            case Misc::ManaAmulet:
            {
                return MakePairs(
                    { Material::Glass,
                      Material::Bone,
                      Material::Wood,
                      Material::Stone,
                      Material::Obsidian,
                      Material::Jade,
                      Material::Bronze,
                      Material::Silver,
                      Material::Gold },
                    fancyJewel_);
            }
            case Misc::MaskMourners:
            {
                return MakePairs({ Material::Wood }, { Material::Obsidian });
            }
            case Misc::MaskRascal:
            {
                return MakePairs(Material::Wood);
            }
            case Misc::MinotaurHide:
            {
                return MakePairs({ Material::Hide }, { Material::Fur });
            }
            case Misc::MortuaryOrnament:
            {
                return MakePairs(Material::Bronze);
            }
            case Misc::NecklaceJeweledAnkh:
            {
                return MakePairs(
                    { Material::Stone, Material::Bone, Material::Obsidian }, fancyJewel_);
            }
            case Misc::NecklaceSharkTooth:
            {
                return MakePairs(Material::Tooth);
            }
            case Misc::NecklaceVampiresTooth:
            {
                return MakePairs(Material::Tooth);
            }
            case Misc::PantherPaw:
            {
                return MakePairs({ Material::DriedFlesh }, { Material::Fur });
            }
            case Misc::PinCrown:
            case Misc::PinFeather:
            case Misc::PinFist:
            case Misc::PinHourglass:
            case Misc::PinKey:
            case Misc::PinMask:
            {
                return MakePairs({ Material::Iron,
                                   Material::Bronze,
                                   Material::Silver,
                                   Material::Gold,
                                   Material::Platinum });
            }
            case Misc::PixieBell:
            {
                return MakePairs(Material::Silver);
            }
            case Misc::RattlesnakeTail:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::RavenClaw:
            {
                return MakePairs(Material::Claw);
            }
            case Misc::ReaperScythe:
            {
                return MakePairs({ Material::Wood }, { Material::Steel });
            }
            case Misc::RequiemRegister:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::RingHobo:
            {
                return MakePairs(Material::Tin);
            }
            case Misc::RingMendicant:
            {
                return MakePairs(Material::Iron);
            }
            case Misc::RingMonk:
            {
                return MakePairs(Material::Silver);
            }
            case Misc::RingPriest:
            {
                return MakePairs({ Material::Gold }, { Material::Pearl });
            }
            case Misc::RoyalScoutSpyglass:
            {
                return MakePairs({ Material::Silver }, { Material::Glass });
            }
            case Misc::SaintCameoPin:
            {
                return MakePairs({ Material::Silver }, { Material::Pearl });
            }
            case Misc::SaintsJournal:
            {
                return MakePairs(Material::Paper);
            }
            case Misc::SanguineRelic:
            {
                return MakePairs({ Material::Bronze }, { Material::Blood });
            }
            case Misc::ScoundrelSack:
            {
                return MakePairs(Material::Leather);
            }
            case Misc::SepultureDecoration:
            {
                return MakePairs({ Material::Wood }, { Material::Bronze });
            }
            case Misc::ShadeCloak:
            {
                return MakePairs(Material::Cloth);
            }
            case Misc::ShamanRainmaker:
            {
                return MakePairs(Material::Wood);
            }
            case Misc::SirenConch:
            {
                return MakePairs(Material::Jade);
            }
            case Misc::SpecterChains:
            {
                return MakePairs(Material::Iron);
            }
            case Misc::SpecterRobe:
            {
                return MakePairs(Material::Cloth);
            }
            case Misc::SpiderEggs:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::SprintersLegtie:
            {
                return MakePairs(Material::Leather);
            }
            case Misc::SwindlersBag:
            {
                return MakePairs(Material::Leather);
            }
            case Misc::TricksterPouch:
            {
                return MakePairs(Material::Leather);
            }
            case Misc::TuningFork:
            {
                return MakePairs(Material::Silver);
            }
            case Misc::TurtleShell:
            {
                return MakePairs(Material::Bone);
            }
            case Misc::VultureGizzard:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::WarhorseMarionette:
            {
                return MakePairs(Material::Wood);
            }
            case Misc::WarTrumpet:
            {
                return MakePairs(Material::Silver);
            }
            case Misc::WeaselTotem:
            {
                return MakePairs(Material::Wood);
            }
            case Misc::WolfenFur:
            {
                return MakePairs(Material::Fur);
            }
            case Misc::WraithTalisman:
            {
                return MakePairs({ Material::Wood }, { Material::Obsidian });
            }

            // standalone items
            case Misc::Bust:
            {
                return MakePairs({ Material::Stone }, nonFleshEyesWithNothing_);
            }
            case Misc::Doll:
            {
                return MakePairs(
                    { Material::Cloth, Material::Silk }, AppendNothingCopy(fancyJewel_));
            }
            case Misc::PuppetBlessed:
            case Misc::DollBlessed:
            {
                return MakePairs({ Material::Wood }, nonFleshEyesWithNothing_);
            }
            case Misc::PuppetCursed:
            case Misc::DollCursed:
            {
                return MakePairs({ Material::Bone }, nonFleshEyesWithNothing_);
            }
            case Misc::DriedHead:
            {
                return MakePairs({ Material::DriedFlesh }, nonFleshEyesWithNothing_);
            }
            case Misc::DrumLute:
            {
                // NOT a quest item
                // some Drumlutes are made by Sets and those materials are set below
                // leave sec=silver/amethyst/diamond for Sets
                return MakePairs(
                    { Material::Wood },
                    Combine({ Material::Emerald,
                              Material::Ruby,
                              Material::Sapphire,
                              Material::Gold,
                              Material::Count }));
            }
            case Misc::Egg:
            case Misc::Embryo:
            {
                return MakePairs(Material::DriedFlesh);
            }
            case Misc::FigurineBlessed:
            {
                return MakePairs(
                    { Material::Wood, Material::Pearl, Material::Glass }, nonFleshEyesWithNothing_);
            }
            case Misc::FigurineCursed:
            {
                return MakePairs(
                    { Material::Stone, Material::Bone, Material::Obsidian },
                    nonFleshEyesWithNothing_);
            }

            case Misc::Goblet:
            {
                return MakePairs(
                    { Material::Tin, Material::Bronze, Material::Silver, Material::Gold },
                    AppendNothingCopy(fancyJewel_));
            }
            case Misc::Key:
            {
                return MakePairs({ Material::Bone,
                                   Material::Wood,
                                   Material::Stone,
                                   Material::Obsidian,
                                   Material::Jade,
                                   Material::Bronze,
                                   Material::Iron,
                                   Material::Silver,
                                   Material::Gold,
                                   Material::Platinum });
            }
            case Misc::LockPicks:
            {
                // leave bone/silver/gold/platinum for Sets
                return MakePairs(Combine(
                    { Material::Tin, Material::Bronze, Material::Iron, Material::Steel },
                    { Material::Obsidian, Material::Tooth }));
            }
            case Misc::Mirror:
            {
                return MakePairs({ Material::Bone,
                                   Material::Wood,
                                   Material::Stone,
                                   Material::Obsidian,
                                   Material::Jade,
                                   Material::Silver });
            }
            case Misc::Orb:
            {
                return MakePairs(
                    fancyJewel_, { Material::Wood, Material::Stone, Material::Obsidian });
            }
            case Misc::Pendant:
            {
                return MakePairs(
                    { Material::Glass,
                      Material::Bone,
                      Material::Wood,
                      Material::Stone,
                      Material::Obsidian,
                      Material::Jade,
                      Material::Bronze,
                      Material::Silver,
                      Material::Gold },
                    fancyJewel_);
            }
            case Misc::Scepter:
            {
                return MakePairs(
                    { Material::Wood,
                      Material::Bone,
                      Material::Jade,
                      Material::Bronze,
                      Material::Silver,
                      Material::Gold,
                      Material::Glass },
                    Combine(fancyJewel_, fancyTribal_, { Material::Count }));
            }
            case Misc::Seeds:
            {
                return MakePairs(Material::Plant);
            }
            case Misc::Shard:
            {
                return MakePairs(fancyJewel_);
            }
            case Misc::SummoningStatue:
            {
                return MakePairs(
                    { Material::Bone,
                      Material::Wood,
                      Material::Stone,
                      Material::Jade,
                      Material::Bronze },
                    Combine(fancyJewel_, fancyMetal_));
            }
            case Misc::Wand:
            {
                // leave pri=Bone/Tooth/Obsidian/Jade and sec=Diamond for Sets
                return MakePairs(
                    { Material::Claw, Material::Horn, Material::Wood, Material::Stone },
                    Combine(
                        fancyJewelWithoutDiamond_,
                        { Material::Silver, Material::Gold, Material::Count }));
            }
            case Misc::Ring:
            {
                // leave DriedFlesh for Sets
                return MakePairs(
                    Combine(
                        { Material::Glass,
                          Material::Claw,
                          Material::Bone,
                          Material::Wood,
                          Material::Stone,
                          Material::Jade },
                        metal_),
                    AppendNothingCopy(fancyJewel_));
            }
            case Misc::Count:
            default:
            {
                return {};
            }
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForWeapon(const ItemTemplate & TEMPLATE) const
    {
        const auto BASE_SET = MakeForWeaponBase(TEMPLATE.WeaponInfo());
        const auto SOLID_SET = MakeForWeaponSolid(TEMPLATE.WeaponInfo());

        MaterialPairVec_t pairs;
        pairs.insert(std::end(pairs), std::begin(BASE_SET), std::end(BASE_SET));
        pairs.insert(std::end(pairs), std::begin(SOLID_SET), std::end(SOLID_SET));
        return pairs;
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForWeaponBase(const WeaponTemplate & WEAPON_INFO) const
    {
        switch (WEAPON_INFO.Type())
        {
            case Weapon::Bladedstaff:
            {
                if (WEAPON_INFO.IsEitherSpear())
                {
                    return MakePairs(
                        corePole_,
                        Combine(
                            coreBlade_,
                            { Material::Tooth, Material::Stone, Material::Horn },
                            fancyJewel_));
                }
                else
                {
                    return MakePairs(coreBlade_, corePole_);
                }
            }

            case Weapon::BodyPart:
            {
                return { MakePairForBodyPartWeapon(WEAPON_INFO.MinorAs<BodyPartWeapons>()) };
            }

            case Weapon::Knife:
            {
                return MakePairs(coreBlade_, verySmallWeaponHandle_);
            }

            case Weapon::Projectile:
            {
                switch (WEAPON_INFO.MinorAs<Projectiles>())
                {
                    // pri=solid, sec=decoration+N
                    case Projectiles::Blowpipe:
                        return MakePairs(
                            { Material::Wood,
                              Material::Bone,
                              Material::Jade,
                              Material::Bronze,
                              Material::Silver },
                            AppendNothingCopy(fancy_));

                        // pri=very_small_handle, sec=decoration
                    case Projectiles::Sling:
                        return MakePairs(verySmallWeaponHandle_, AppendNothingCopy(fancy_));

                        // pri=solid, sec=decoration+N
                    case Projectiles::Shortbow:
                    case Projectiles::Longbow:
                    case Projectiles::Compositebow:
                        return MakePairs(
                            { Material::Wood,
                              Material::Bone,
                              Material::Horn,
                              Material::Obsidian,
                              Material::Jade,
                              Material::Silver },
                            AppendNothingCopy(fancy_));

                        // pri=handle, sec=reinforced+N
                    case Projectiles::Crossbow:
                        return MakePairs(smallWeaponHandle_, AppendNothingCopy(fancyReinforced_));

                    case Projectiles::Count:
                    default: return {};
                }
            }

            case Weapon::Staff:
            {
                return MakePairs(corePole_, AppendNothingCopy(fancyReinforced_));
            }

            case Weapon::Whip:
            {
                switch (WEAPON_INFO.MinorAs<Whips>())
                {
                    // pri=very_small_handle, sec=decoration+N
                    case Whips::Bullwhip:
                        return MakePairs(verySmallWeaponHandle_, AppendNothingCopy(fancy_));

                    // pri=end, sec=small_handle
                    case Whips::Flail:
                        return MakePairs(
                            { Material::Horn,
                              Material::Bone,
                              Material::Wood,
                              Material::Obsidian,
                              Material::Jade },
                            smallWeaponHandle_);

                    // pri=end, sec=small_handle
                    case Whips::MaceAndChain:
                        return MakePairs(
                            { Material::Obsidian,
                              Material::Diamond,
                              Material::Bronze,
                              Material::Iron,
                              Material::Steel,
                              Material::Platinum },
                            smallWeaponHandle_);
                    case Whips::Count:
                    default: return {};
                }
            }

            case Weapon::Axe:
            case Weapon::Club:
            case Weapon::Sword:
            {
                return MakePairs(coreBlade_, smallWeaponHandle_);
            }

            case Weapon::Count:
            default: return {};
        }
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForWeaponSolid(const WeaponTemplate & WEAPON_INFO) const
    {
        if (WEAPON_INFO.IsAnyKnife())
        {
            return MakePairs(Combine(
                metal_,
                fancyJewel_,
                { Material::Obsidian,
                  Material::Jade,
                  Material::Bone,
                  Material::Tooth,
                  Material::Stone }));
        }
        else if (WEAPON_INFO.IsEitherSpear() || WEAPON_INFO.IsAnyStaff())
        {
            return MakePairs({ Material::Wood,
                               Material::Bronze,
                               Material::Iron,
                               Material::Jade,
                               Material::Bone });
        }
        else if (
            WEAPON_INFO.IsMinor<Swords>(Swords::Falcata) || WEAPON_INFO.IsMinor<Clubs>(Clubs::Mace))
        {
            return MakePairs(
                { Material::Bronze, Material::Jade, Material::Iron, Material::Platinum });
        }
        else if (WEAPON_INFO.IsBlowpipe())
        {
            return MakePairs({ Material::Bronze, Material::Jade, Material::Iron });
        }
        else
        {
            return {};
        }
    }

    const MaterialPairVec_t MaterialFactory::MakeForArmor(const ItemTemplate & TEMPLATE) const
    {
        const auto & ARMOR_INFO = TEMPLATE.ArmorInfo();
        const auto FORM = ARMOR_INFO.Form();

        switch (ARMOR_INFO.Type())
        {
            case Armor::Aventail:
            case Armor::Boot:
            case Armor::Bracer:
            {
                if (FORM == Forms::Mail)
                {
                    return MakePairs(
                        Combine(mailArmorMetals_, { Material::Gold, Material::Platinum }));
                }
                else
                {
                    MaterialVec_t primaryMaterials;

                    if (FORM == Forms::Plain)
                    {
                        primaryMaterials = { Material::Leather, Material::Hide };

                        if (ARMOR_INFO.IsAventail())
                        {
                            primaryMaterials.emplace_back(Material::Silk);
                        }

                        if (ARMOR_INFO.IsBoot() || ARMOR_INFO.IsAventail())
                        {
                            primaryMaterials.emplace_back(Material::Fur);
                        }
                    }
                    else if (FORM == Forms::Scale)
                    {
                        primaryMaterials = { Material::Scales };
                    }
                    else if (FORM == Forms::Plate)
                    {
                        primaryMaterials = metal_;
                    }

                    return MakePairs(primaryMaterials, fancyClasp_);
                }
            }

            case Armor::Pant:
            case Armor::Shirt:
            {
                if (FORM == Forms::Mail)
                {
                    return MakePairs(mailArmorMetals_);
                }
                else
                {
                    MaterialVec_t primaryMaterials;

                    if (FORM == Forms::Plain)
                    {
                        primaryMaterials = { Material::Cloth,
                                             Material::Leather,
                                             Material::Silk,
                                             Material::Hide,
                                             Material::Fur };
                    }
                    else if (FORM == Forms::Scale)
                    {
                        primaryMaterials = { Material::Scales };
                    }
                    else if (FORM == Forms::Plate)
                    {
                        primaryMaterials = { Material::Tin,
                                             Material::Bronze,
                                             Material::Iron,
                                             Material::Steel,
                                             Material::Platinum };
                    }

                    return MakePairs(primaryMaterials, fancyClasp_);
                }
            }

            case Armor::Gauntlet:
            {
                switch (ARMOR_INFO.Form())
                {
                    case Forms::Plain:
                        return MakePairs(
                            { Material::Cloth, Material::Leather, Material::Silk, Material::Hide },
                            AppendNothingCopy(fancyJewelWithoutDiamond_));

                    case Forms::Mail:
                        return MakePairs(
                            Combine(mailArmorMetals_, { Material::Gold, Material::Platinum }));

                    case Forms::Scale:
                        return MakePairs(
                            { Material::Scales }, AppendNothingCopy(fancyReinforcedArmor_));

                    case Forms::Plate:
                        return MakePairs(
                            metal_,
                            Combine(
                                fancyJewelWithoutDiamond_, { Material::Pearl, Material::Count }));

                    case Forms::Count:
                    default: return {};
                }
            }

            case Armor::Cover:
            {
                // pri=fabric sec=clasp (must have clasp so Nothing is not included)
                if (ARMOR_INFO.IsMinor<Covers>(Covers::Cape))
                {
                    return MakePairs({ Material::Cloth, Material::Silk }, fancyClasp_);
                }
                else
                {
                    return MakePairs(
                        { Material::Cloth,
                          Material::Leather,
                          Material::Silk,
                          Material::Fur,
                          Material::Hide },
                        fancyClasp_);
                }
            }

            case Armor::Helm:
            {
                // pri=leather, sec=reinforced_armor+Nothing except for MailCoif=Nothing

                const MaterialVec_t PRIMARY_MATERIALS = [&]() -> MaterialVec_t {
                    switch (ARMOR_INFO.MinorAs<Helms>())
                    {
                        case Helms::Leather: return { Material::Leather };
                        case Helms::MailCoif: return mailArmorMetals_;

                        case Helms::Kettle:
                            return { Material::Tin, Material::Bronze, Material::Iron };

                        case Helms::Archers:
                            return {
                                Material::Tin, Material::Bronze, Material::Iron, Material::Steel
                            };

                        case Helms::Bascinet:
                            return { Material::Tin,   Material::Bronze, Material::Iron,
                                     Material::Steel, Material::Silver, Material::Platinum };

                        case Helms::Great:
                            return { Material::Tin,     Material::Bronze, Material::Iron,
                                     Material::Steel,   Material::Silver, Material::Gold,
                                     Material::Platinum };

                        case Helms::Count:
                        default: return {};
                    }
                }();

                return MakePairs(PRIMARY_MATERIALS, AppendNothingCopy(fancyReinforcedArmor_));
            }

            case Armor::Shield:
            {
                if (ARMOR_INFO.IsMinor<Shields>(Shields::Buckler))
                {
                    // pri=it, sec=reinforced_armor+Nothing
                    return MakePairs(
                        Combine(
                            metal_,
                            { Material::Diamond,
                              Material::Obsidian,
                              Material::Wood,
                              Material::Scales,
                              Material::Bone,
                              Material::Jade }),
                        AppendNothingCopy(fancyReinforcedArmor_));
                }
                else
                {
                    // pri=it, sec=reinforce_armor+Nothing
                    return MakePairs(
                        { Material::Wood,
                          Material::Scales,
                          Material::Tin,
                          Material::Bronze,
                          Material::Steel,
                          Material::Silver,
                          Material::Gold },
                        AppendNothingCopy(fancyReinforcedArmor_));
                }
            }

            case Armor::Skin: // you have to use the bodypart armor functions for this
            case Armor::Count:
            default: return {};
        }
    }

    void MaterialFactory::LimitForNamedType(
        const ItemTemplate & TEMPLATE,
        const Named::Enum NAMED,
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
            if (TEMPLATE.GetMaterialNameStyle() == MaterialNameStyle::Handle)
            {
                return AppendCopy(MATERIALS, Material::Wood);
            }
            else
            {
                return MATERIALS;
            }
        } };

        switch (NAMED)
        {
            case Named::Betrayer:
            {
                excludeAnyOfPrimaryAndSecondary({ Material::Pearl,
                                                  Material::Diamond,
                                                  Material::Gold,
                                                  Material::Jade,
                                                  Material::Lazuli,
                                                  Material::Tin,
                                                  Material::Platinum,
                                                  Material::Silver });

                break;
            }
            case Named::Dancing:
            {
                keepOnlyPrimary(
                    { Material::Diamond, Material::Iron, Material::Steel, Material::Platinum });

                keepOnlySecondary({ Material::Wood, Material::Pearl });

                break;
            }
            case Named::Dark:
            {
                excludeAnyOfPrimaryAndSecondary({ Material::Pearl,
                                                  Material::Diamond,
                                                  Material::Gold,
                                                  Material::Jade,
                                                  Material::Lazuli,
                                                  Material::Tin,
                                                  Material::Platinum,
                                                  Material::Silver });

                break;
            }
            case Named::Diabolic:
            {
                keepOnlyPrimaryAndSecondary({ Material::Diamond,
                                              Material::Wood,
                                              Material::Steel,
                                              Material::Obsidian,
                                              Material::Bronze,
                                              Material::Bone });

                break;
            }
            case Named::Fiendish:
            {
                keepOnlyPrimary(
                    { Material::Steel, Material::Obsidian, Material::Platinum, Material::Diamond });

                keepOnlySecondaryIncludingNothing(
                    { Material::Wood, Material::Bone, Material::Iron });

                break;
            }
            case Named::Focus:
            {
                keepOnlyPrimary({ Material::Wood, Material::Horn, Material::Silver });

                keepOnlySecondary({ Material::Diamond,
                                    Material::Silver,
                                    Material::Gold,
                                    Material::Obsidian,
                                    Material::Claw });

                break;
            }
            case Named::Icy:
            case Named::Winter:
            case Named::Frigid:
            case Named::Arctic:
            case Named::Chill:
            case Named::Frozen:
            {
                if (TEMPLATE.WeaponInfo().IsMinor<Whips>(Whips::Flail))
                {
                    // materials will be all combinations of wood/bone/horn/obsidian/jade
                    excludeAnyOfPrimaryAndSecondary({ Material::Horn, Material::Jade });
                }
                else
                {
                    excludeAnyOfSecondary(fancyTribal_);

                    keepOnlyPrimaryAndSecondary({ Material::Wood,
                                                  Material::Fur,
                                                  Material::Hide,
                                                  Material::Sapphire,
                                                  Material::Diamond,
                                                  Material::Silver,
                                                  Material::Iron,
                                                  Material::Steel,
                                                  Material::Platinum,
                                                  Material::Lazuli,
                                                  Material::Scales,
                                                  Material::Count });
                }
                break;
            }
            case Named::Army:
            {
                excludeAnyOfSecondary(fancyTribal_);

                excludeAnyOfPrimaryAndSecondary(Combine(
                    fancyJewel_,
                    fancyOpaque_,
                    fancyMetal_,
                    { Material::Platinum, Material::Silk, Material::Bronze }));

                break;
            }
            case Named::Gladiator:
            {
                excludeAnyOfPrimaryAndSecondary(Combine(
                    RemoveCopy(fancy_, Material::Scales), { Material::Silk, Material::Fur }));

                break;
            }
            case Named::Soldiers:
            {
                excludeAnyOfPrimaryAndSecondary(
                    AppendCopy(RemoveCopy(fancy_, Material::Scales), Material::Silk));

                break;
            }
            case Named::Light:
            case Named::Dawn:
            case Named::Sun:
            case Named::Proud:
            case Named::Pure:
            case Named::Glory:
            {
                excludeAnyOfSecondary({ Material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, Material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { Material::Silk, Material::Cloth, Material::Scales, Material::Count },
                    {
                        Material::Steel,
                        Material::Silver,
                        Material::Gold,
                        Material::Platinum,
                    },
                    { Material::Diamond, Material::Amethyst, Material::Pearl, Material::Wood }));

                break;
            }

            case Named::Daring:
            {
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, Material::Scales));

                excludeAnyOfPrimaryAndSecondary(Combine(
                    { Material::Silk, Material::Platinum },
                    fancyMetal_,
                    fancyOpaque_,
                    RemoveCopy(fancyJewel_, Material::Amethyst)));

                break;
            }
            case Named::Honest:
            {
                excludeAnyOfSecondary({ Material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, Material::Scales));

                excludeAnyOfPrimaryAndSecondary(Combine(
                    { Material::Silk, Material::Platinum },
                    fancyMetal_,
                    RemoveCopy(fancyOpaque_, Material::Pearl),
                    RemoveCopy(fancyJewel_, Material::Amethyst)));

                break;
            }
            case Named::Elite:
            case Named::Noble:
            case Named::Valiant:
            {
                excludeAnyOfSecondary({ Material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, Material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { Material::Pearl,
                      Material::Silk,
                      Material::Platinum,
                      Material::Scales,
                      Material::Count },
                    fancyMetal_,
                    RemoveCopy(fancyJewel_, Material::Amethyst)));

                break;
            }
            case Named::Heroes:
            {
                excludeAnyOfSecondary({ Material::Scales });
                excludeAnyOfPrimaryAndSecondary(RemoveCopy(fancyTribal_, Material::Scales));

                keepOnlyPrimaryAndSecondary(Combine(
                    { Material::Diamond,
                      Material::Pearl,
                      Material::Silk,
                      Material::Platinum,
                      Material::Scales,
                      Material::Count },
                    fancyMetal_));

                break;
            }
            case Named::Charlatans:
            {
                keepOnlyPrimaryAndSecondary({ Material::Silk, Material::Silver, Material::Count });

                break;
            }
            case Named::Burglar:
            {
                keepOnlyPrimaryAndSecondary(
                    { Material::Leather, Material::Steel, Material::Count });

                break;
            }
            case Named::Mountebank:
            {
                keepOnlyPrimaryAndSecondary({ Material::Fur, Material::Steel, Material::Count });
                break;
            }
            case Named::Pickpocket:
            {
                keepOnlyPrimaryAndSecondary({ Material::Leather, Material::Tin, Material::Count });

                break;
            }
            case Named::Imposters:
            {
                keepOnlyPrimaryAndSecondary({ Material::Cloth, Material::Iron, Material::Count });
                break;
            }
            case Named::Infernal:
            {
                if (TEMPLATE.WeaponInfo().IsSword())
                {
                    keepOnlyPrimary(
                        { Material::Diamond, Material::Bronze, Material::Steel, Material::Gold });

                    keepOnlySecondaryIncludingNothing({ Material::Wood, Material::Bone });
                }
                else if (TEMPLATE.WeaponInfo().IsMinor<Projectiles>(Projectiles::Sling))
                {
                    keepOnlyPrimary({ Material::Wood, Material::Bone });

                    excludeAnyOfSecondary({ Material::Stone,
                                            Material::Horn,
                                            Material::Jade,
                                            Material::Amethyst,
                                            Material::Emerald,
                                            Material::Sapphire,
                                            Material::Silver });
                }
                else if (TEMPLATE.WeaponInfo().IsBow())
                {
                    excludeAnyOfPrimary({ Material::Obsidian, Material::Jade, Material::Silver });

                    excludeAnyOfSecondary({ Material::Jade,
                                            Material::Amethyst,
                                            Material::Emerald,
                                            Material::Sapphire,
                                            Material::Silver });
                }

                break;
            }
            case Named::Marauder:
            {
                keepOnlyPrimary({ Material::Iron, Material::Steel, Material::Platinum });
                keepOnlySecondary({ Material::Wood });
                break;
            }
            case Named::Gloom:
            case Named::Twilight:
            case Named::Dusk:
            case Named::Sorrow:
            case Named::Woe:
            case Named::Misery:
            {
                excludeAnyOfPrimaryAndSecondary({ Material::Pearl,
                                                  Material::Diamond,
                                                  Material::Gold,
                                                  Material::Jade,
                                                  Material::Lazuli,
                                                  Material::Silk,
                                                  Material::Fur,
                                                  Material::Tin,
                                                  Material::Platinum });

                break;
            }
            case Named::Moon:
            {
                excludeAnyOfPrimaryAndSecondary({ Material::Pearl,
                                                  Material::Diamond,
                                                  Material::Gold,
                                                  Material::Jade,
                                                  Material::Lazuli,
                                                  Material::Fur,
                                                  Material::Tin });

                break;
            }
            case Named::Nile:
            {
                keepOnlyPrimaryAndSecondary({ Material::Cloth,
                                              Material::Leather,
                                              Material::Steel,
                                              Material::Bronze,
                                              Material::Iron,
                                              Material::Gold,
                                              Material::Lazuli,
                                              Material::Count });

                break;
            }
            case Named::Thief:
            case Named::Robbers:
            {
                keepOnlyPrimaryAndSecondary({ Material::Stone,
                                              Material::Iron,
                                              Material::Steel,
                                              Material::Wood,
                                              Material::Count });

                break;
            }
            case Named::Muggers:
            case Named::Thugs:
            {
                keepOnlyPrimaryAndSecondary(
                    { Material::Iron, Material::Steel, Material::Wood, Material::Count });

                break;
            }
            case Named::Knaves:
            {
                keepOnlyPrimaryAndSecondary({ Material::Steel, Material::Wood, Material::Count });

                break;
            }
            case Named::Pirate:
            {
                keepOnlyPrimaryAndSecondary(
                    { Material::Obsidian, Material::Silver, Material::Bone, Material::Count });

                break;
            }
            case Named::Princes:
            {
                keepOnlyPrimaryAndSecondary(appendWoodIfHasHandleCopy(Combine(
                    { Material::Pearl,
                      Material::Silk,
                      Material::Platinum,
                      Material::Steel,
                      Material::Count },
                    fancyMetal_,
                    fancyJewel_)));

                break;
            }
            case Named::Raging:
            {
                keepOnlyPrimary(
                    { Material::Diamond, Material::Iron, Material::Steel, Material::Platinum });

                excludeAnyOfSecondary({ Material::Pearl });
                break;
            }
            case Named::Ranger:
            {
                keepOnlyPrimaryAndSecondary({ Material::Wood,
                                              Material::Steel,
                                              Material::Leather,
                                              Material::Emerald,
                                              Material::Scales,
                                              Material::Count });

                break;
            }
            case Named::Samurai:
            {
                keepOnlyPrimaryAndSecondary({ Material::Silk,
                                              Material::Leather,
                                              Material::Wood,
                                              Material::Steel,
                                              Material::Scales,
                                              Material::Jade,
                                              Material::Count });

                break;
            }
            case Named::Charred:
            case Named::Burning:
            case Named::Fiery:
            case Named::Searing:
            {
                keepOnlyPrimaryAndSecondary({ Material::Wood,
                                              Material::Leather,
                                              Material::Claw,
                                              Material::Scales,
                                              Material::Tooth,
                                              Material::Bone,
                                              Material::Steel,
                                              Material::Bronze,
                                              Material::Iron,
                                              Material::Platinum,
                                              Material::Ruby,
                                              Material::Diamond,
                                              Material::Count });

                break;
            }
            case Named::Thors:
            {
                keepOnlyPrimaryAndSecondary(appendWoodIfHasHandleCopy({ Material::Steel,
                                                                        Material::Silk,
                                                                        Material::Pearl,
                                                                        Material::Platinum,
                                                                        Material::Gold,
                                                                        Material::Diamond,
                                                                        Material::Count }));

                break;
            }
            case Named::Wardens:
            {
                excludeAnyOfPrimaryAndSecondary(
                    Combine(fancy_, { Material::Silk, Material::Fur, Material::Steel }));

                break;
            }
            case Named::Wicked:
            {
                if (TEMPLATE.WeaponInfo().IsEitherSpear())
                {
                    excludeAnyOfPrimary({ Material::Jade, Material::Bronze });
                    excludeAnyOfSecondary({ Material::Stone,
                                            Material::Horn,
                                            Material::Jade,
                                            Material::Bronze,
                                            Material::Amethyst,
                                            Material::Emerald,
                                            Material::Sapphire,
                                            Material::Gold });
                }
                else
                {
                    keepOnlyPrimary({ Material::Steel,
                                      Material::Obsidian,
                                      Material::Platinum,
                                      Material::Diamond });
                    keepOnlySecondaryIncludingNothing(
                        { Material::Wood, Material::Bone, Material::Iron });
                }

                break;
            }

            case Named::Count:
            default:
            {
                break;
            }
        }
    }

    const MaterialPairVec_t
        MaterialFactory::MakeForSetType(const ItemTemplate & TEMPLATE, const Set::Enum SET) const
    {
        Material::Enum primary { Material::Count };
        Material::Enum secondary { Material::Count };

        switch (SET)
        {
            case Set::Assassins:
            case Set::Neverwinter:
            case Set::Tickler:
            {
                if (TEMPLATE.IsArmor())
                {
                    // Boot/Pant/Shirt/Gloves/Cloak
                    if (SET == Set::Assassins)
                    {
                        primary = Material::Leather;
                        secondary = Material::Bone;
                    }
                    else if (SET == Set::Neverwinter)
                    {
                        primary = Material::Fur;
                        secondary = Material::Silver;
                    }
                    else
                    {
                        primary = Material::Leather;
                        secondary = Material::Gold;
                    }
                }
                else if (TEMPLATE.IsWeapon())
                {
                    // Dagger
                    if (SET == Set::Assassins)
                    {
                        primary = Material::Bone;
                        secondary = Material::Count;
                    }
                    else if (SET == Set::Neverwinter)
                    {
                        primary = Material::Silver;
                        secondary = Material::Sapphire;
                    }
                    else
                    {
                        primary = Material::Diamond;
                        secondary = Material::Count;
                    }
                }
                else
                {
                    // Lock Picks
                    if (SET == Set::Assassins)
                    {
                        primary = Material::Bone;
                    }
                    else if (SET == Set::Neverwinter)
                    {
                        primary = Material::Silver;
                    }
                    else
                    {
                        primary = Material::Gold;
                    }

                    secondary = Material::Count;
                }

                break;
            }

            case Set::Magus:
            case Set::Necromancers:
            case Set::Warlocks:
            case Set::LichKings:
            {
                // Boot/Pant/Shirt/Gloves/Robe
                if (TEMPLATE.IsArmor())
                {
                    if (SET == Set::Magus)
                    {
                        primary = Material::Cloth;
                        secondary = Material::Silver;
                    }
                    else if (SET == Set::Necromancers)
                    {
                        primary = Material::Leather;
                        secondary = Material::Bone;
                    }
                    else if (SET == Set::Warlocks)
                    {
                        primary = Material::Fur;
                        secondary = Material::Ruby;
                    }
                    else if (SET == Set::LichKings)
                    {
                        primary = Material::Silk;
                        secondary = Material::Diamond;
                    }
                }
                else
                {
                    if (SET == Set::Magus)
                    {
                        // Wand
                        primary = Material::Wood;
                        secondary = Material::Silver;
                    }
                    else if (SET == Set::Necromancers)
                    {
                        // Orb
                        primary = Material::Amethyst;
                        secondary = Material::Bone;
                    }
                    else if (SET == Set::Warlocks)
                    {
                        // Scepter
                        primary = Material::Obsidian;
                        secondary = Material::Ruby;
                    }
                    else if (SET == Set::LichKings)
                    {
                        // LichHand
                        primary = Material::DriedFlesh;
                        secondary = Material::Diamond;
                    }
                }

                break;
            }

            case Set::Sages:
            case Set::Shamans:
            case Set::Oracles:
            case Set::Angelic:
            {
                // Boot/Pant/Shirt/Gloves/Robe
                if (TEMPLATE.IsArmor())
                {
                    if (SET == Set::Sages)
                    {
                        primary = Material::Cloth;
                        secondary = Material::Amethyst;
                    }
                    else if (SET == Set::Shamans)
                    {
                        primary = Material::Leather;
                        secondary = Material::Emerald;
                    }
                    else if (SET == Set::Oracles)
                    {
                        primary = Material::Leather;
                        secondary = Material::Lazuli;
                    }
                    else if (SET == Set::Angelic)
                    {
                        primary = Material::Silk;
                        secondary = Material::Diamond;
                    }
                }
                else
                {
                    if (SET == Set::Sages)
                    {
                        // Shard
                        primary = Material::Amethyst;
                        secondary = Material::Count;
                    }
                    else if (SET == Set::Shamans)
                    {
                        // Orb
                        primary = Material::Emerald;
                        secondary = Material::Jade;
                    }
                    else if (SET == Set::Oracles)
                    {
                        // Scepter
                        primary = Material::Lazuli;
                        secondary = Material::Pearl;
                    }
                    else if (SET == Set::Angelic)
                    {
                        // Angel Braid
                        primary = Material::Hair;
                        secondary = Material::Diamond;
                    }
                }

                break;
            }

            case Set::Balladeers:
            {
                if (TEMPLATE.IsArmor())
                {
                    // Boot/Pant/Shirt
                    primary = Material::Cloth;
                }
                else if (TEMPLATE.IsWeapon())
                {
                    // Cutlass
                    primary = Material::Steel;
                }
                else if (Misc::IsMusicalInstrument(TEMPLATE.MiscTypeBase()))
                {
                    // Drumlute
                    primary = Material::Wood;
                }

                secondary = Material::Silver;
                break;
            }
            case Set::Troubadours:
            {
                if (TEMPLATE.IsArmor())
                {
                    if (TEMPLATE.ArmorInfo().IsGauntlet())
                    {
                        // Gloves
                        primary = Material::Leather;
                    }
                    else
                    {
                        // Mail: Boot/Pant/Shirt
                        primary = Material::Silver;
                    }
                }
                else if (TEMPLATE.IsWeapon())
                {
                    // Rapier
                    primary = Material::Silver;
                }
                else if (Misc::IsMusicalInstrument(TEMPLATE.MiscTypeBase()))
                {
                    // Drumlute
                    primary = Material::Wood;
                }

                secondary = Material::Amethyst;
                break;
            }
            case Set::Muses:
            {
                if (TEMPLATE.IsArmor())
                {
                    if ((TEMPLATE.ArmorInfo().IsCover()) || (TEMPLATE.ArmorInfo().IsGauntlet()))
                    {
                        // Gloves/Vest
                        primary = Material::Silk;
                    }
                    else
                    {
                        // Mail: Boot/Pant/Shirt
                        primary = Material::Silver;
                    }
                }
                else if (TEMPLATE.IsWeapon())
                {
                    // Rapier
                    primary = Material::Silver;
                }
                else if (Misc::IsMusicalInstrument(TEMPLATE.MiscTypeBase()))
                {
                    // Drumlute
                    primary = Material::Wood;
                }

                secondary = Material::Diamond;
                break;
            }

            case Set::Cavaliers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape)))
                {
                    // Boot/Pant/Shirt/Gloves/Cape
                    primary = Material::Cloth;
                }
                else if (
                    TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Aventail/Bracer/LeatherHelm
                    primary = Material::Leather;
                }
                else if (TEMPLATE.ArmorInfo().IsShield())
                {
                    // Buckler
                    primary = Material::Iron;
                }

                secondary = Material::Jade;
                break;
            }
            case Set::Champions:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape)))
                {
                    // Boot/Pant/Shirt/Gloves/Cape
                    primary = Material::Silk;
                }
                else if (TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Aventail/Bracer
                    primary = Material::Leather;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm() || TEMPLATE.ArmorInfo().IsShield())
                {
                    // MailCoif/Kite
                    primary = Material::Steel;
                }

                secondary = Material::Amethyst;
                break;
            }
            case Set::Paladins:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape))
                    || TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Gloves/Cape/Aventail/Bracer
                    primary = Material::Leather;
                }
                else if (TEMPLATE.ArmorInfo().IsShield() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Kite/Kettle
                    primary = Material::Steel;
                }

                secondary = Material::Pearl;
                break;
            }
            case Set::Berserkers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape)))
                {
                    // Boot/Pant/Shirt/Gloves/Cape
                    primary = Material::Fur;
                }
                else if (TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Aventail/Bracer
                    primary = Material::Leather;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm() || TEMPLATE.ArmorInfo().IsShield())
                {
                    // MailCoif/Kite
                    primary = Material::Silver;
                }

                secondary = Material::Emerald;
                break;
            }
            case Set::Rosewood:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsShield() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Boot/Pant/Shirt/Gloves/Aventail/Bracer/Heater/Bascinet
                    primary = Material::Silver;
                }
                else if (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape))
                {
                    // Cape
                    primary = Material::Silk;
                }

                secondary = Material::Ruby;
                break;
            }
            case Set::Dragonslayers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Gloves/Aventail/Bracer
                    primary = Material::Scales;
                }
                else if (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape))
                {
                    // Cape
                    primary = Material::Leather;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm() || TEMPLATE.ArmorInfo().IsShield())
                {
                    // Heater/Bascinet
                    primary = Material::Gold;
                }

                secondary = Material::Sapphire;
                break;
            }
            case Set::EventideRider:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || TEMPLATE.ArmorInfo().IsAventail() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsHelm() || TEMPLATE.ArmorInfo().IsShield())
                {
                    // Boot/Pant/Shirt/Gloves/Aventail/Bracer/Pavis/Great
                    primary = Material::Platinum;
                }
                else if (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Cape))
                {
                    // Cape
                    primary = Material::Fur;
                }

                secondary = Material::Diamond;
                break;
            }

            case Set::Hunters:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Vest)))
                {
                    // Boot/Pant/Shirt/Gloves/Vest
                    primary = Material::Silk;
                }
                else if (TEMPLATE.ArmorInfo().IsBracer() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Leather/Bracer
                    primary = Material::Leather;
                }

                secondary = Material::Amethyst;
                break;
            }
            case Set::SureShot:
            {
                // Boot/Pant/Shirt/Gloves/Vest/Bracer/LeatherHelm
                primary = Material::Leather;
                secondary = Material::Emerald;
                break;
            }
            case Set::Marksmans:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsHelm()
                    || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Bracer/MailCoif
                    primary = Material::Steel;
                }
                else if (
                    TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Vest)))
                {
                    // Gloves/Vest
                    primary = Material::Leather;
                }

                secondary = Material::Ruby;
                break;
            }
            case Set::Deadeye:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsHelm()
                    || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Bracer/MailCoif
                    primary = Material::Silver;
                }
                else if (
                    TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Vest)))
                {
                    // Gloves/Vest
                    primary = Material::Leather;
                }

                secondary = Material::Lazuli;
                break;
            }
            case Set::DuskRanger:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Bracer
                    primary = Material::Scales;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm())
                {
                    primary = Material::Platinum;
                }
                else if (
                    TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Vest)))
                {
                    // Gloves/Vest
                    primary = Material::Leather;
                }

                secondary = Material::Sapphire;
                break;
            }
            case Set::VenomBow:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer())
                {
                    // Boot/Pant/Shirt/Bracer
                    primary = Material::Scales;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm())
                {
                    primary = Material::Platinum;
                }
                else if (
                    TEMPLATE.ArmorInfo().IsGauntlet()
                    || (TEMPLATE.ArmorInfo().IsMinor<Covers>(Covers::Vest)))
                {
                    // Gloves/Vest
                    primary = Material::Leather;
                }

                secondary = Material::Diamond;
                break;
            }

            case Set::CritterChannelers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsGauntlet())
                {
                    primary = Material::Silk;
                }
                else if (TEMPLATE.ArmorInfo().IsShield())
                {
                    // Buckler
                    primary = Material::Steel;
                }
                else if (TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Leather
                    primary = Material::Leather;
                }

                secondary = Material::Amethyst;
                break;
            }
            case Set::MammalianHead:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsGauntlet())
                {
                    primary = Material::Fur;
                }
                else if (TEMPLATE.ArmorInfo().IsShield() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Buckler/MailCoif
                    primary = Material::Silver;
                }

                secondary = Material::Emerald;
                break;
            }
            case Set::SavageTaskmasters:
            {
                //.../Kite/Archers
                primary = Material::Silver;
                secondary = Material::Ruby;
                break;
            }
            case Set::MonsterOverseers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsGauntlet())
                {
                    primary = Material::Scales;
                }
                else if (TEMPLATE.ArmorInfo().IsShield() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Kite/Bascinet
                    primary = Material::Silver;
                }

                secondary = Material::Sapphire;
                break;
            }
            case Set::BeastRulers:
            {
                if (TEMPLATE.ArmorInfo().IsBoot() || TEMPLATE.ArmorInfo().IsPant()
                    || TEMPLATE.ArmorInfo().IsShirt() || TEMPLATE.ArmorInfo().IsBracer()
                    || TEMPLATE.ArmorInfo().IsGauntlet())
                {
                    primary = Material::Scales;
                }
                else if (TEMPLATE.ArmorInfo().IsShield() || TEMPLATE.ArmorInfo().IsHelm())
                {
                    // Kite/Bascinet
                    primary = Material::Platinum;
                }

                secondary = Material::Diamond;
                break;
            }

            case Set::Count:
            default:
            {
                break;
            }
        }

        return { std::make_pair(primary, secondary) };
    }

    bool MaterialFactory::IsCombinationValid(const MaterialPair_t & MATERIAL_PAIR) const
    {
        const auto PRIMARY { MATERIAL_PAIR.first };
        const auto SECONDARY { MATERIAL_PAIR.second };

        if (Material::IsSolid(PRIMARY) == false)
        {
            M_HP_LOG_WRN(
                "item::MaterialFactory::IsCombinationValid(" << NAMEOF_ENUM(PRIMARY) << ", "
                                                             << NAMEOF_ENUM(SECONDARY)
                                                             << ") found non-solid primary.");

            return false;
        }

        if (SECONDARY == PRIMARY)
        {
            return false;
        }
        else if (SECONDARY >= Material::Count)
        {
            M_HP_LOG_WRN(
                "item::MaterialFactory::IsCombinationValid(" << NAMEOF_ENUM(PRIMARY) << ", "
                                                             << NAMEOF_ENUM(SECONDARY)
                                                             << ") found secondary==Count.");

            return false;
        }
        else if (SECONDARY >= Material::Count)
        {
            return true;
        }

        if (Material::IsMetal(PRIMARY))
        {
            if (SECONDARY == Material::Stone)
            {
                return false;
            }

            if (Material::IsMetal(SECONDARY))
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

            if ((Material::IsFancyOpaque(SECONDARY) == false) && (SECONDARY != Material::Blood)
                && (Material::IsFancyJewel(SECONDARY) == false))
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
        return MakePairs(PRIMARIES, { Material::Count });
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(
        const MaterialVec_t & PRIMARIES, const MaterialVec_t & ORIG_SECONDARIES) const
    {
        MaterialPairVec_t materialPairs;

        const auto FINAL_SECONDARIES { (
            (ORIG_SECONDARIES.empty()) ? MaterialVec_t { Material::Count } : ORIG_SECONDARIES) };

        for (const auto MAT_PRI : Combine(PRIMARIES))
        {
            for (const auto MAT_SEC : Combine(FINAL_SECONDARIES))
            {
                if (MAT_PRI != MAT_SEC)
                {
                    materialPairs.emplace_back(std::make_pair(MAT_PRI, MAT_SEC));
                }
            }
        }

        return materialPairs;
    }

    const MaterialPairVec_t MaterialFactory::MakePairs(const Material::Enum MATERIAL) const
    {
        return MaterialPairVec_t({ std::make_pair(MATERIAL, Material::Count) });
    }

    const MaterialPairVec_t MaterialFactory::RemoveLameMaterialsFromSpecialItems(
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
        Material::Enum onlyPrimary { MATERIAL_PAIRS.front().first };
        for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS)
        {
            const auto PRIMARY { MATERIAL_PAIR.first };
            if (PRIMARY == Material::Leather)
            {
                hasPrimaryLeather = true;
            }
            else if (PRIMARY == Material::Silk)
            {
                hasPrimarySilk = true;
            }
            else if (Material::IsRigid(PRIMARY) && (PRIMARY != Material::Wood))
            {
                hasPrimaryRigidNonWood = true;
            }

            if (PRIMARY == Material::Steel)
            {
                hasPrimarySteel = true;
            }
            else if ((PRIMARY == Material::Silver) || (PRIMARY == Material::Gold))
            {
                hasPrimarySilverOrGold = true;
            }

            if (PRIMARY != onlyPrimary)
            {
                onlyPrimary = Material::Count;
            }
        }

        for (const auto & MATERIAL_PAIR : MATERIAL_PAIRS)
        {
            if (onlyPrimary < Material::Count)
            {
                if (hasPrimarySilk
                    && ((MATERIAL_PAIR.first == Material::Leather)
                        || (MATERIAL_PAIR.first == Material::Cloth)))
                {
                    continue;
                }
                else if (hasPrimaryLeather && (MATERIAL_PAIR.first == Material::Cloth))
                {
                    continue;
                }
                else if (hasPrimaryRigidNonWood && (MATERIAL_PAIR.first == Material::Wood))
                {
                    continue;
                }
                else if (hasPrimarySteel && (MATERIAL_PAIR.first == Material::Iron))
                {
                    continue;
                }
                else if (
                    (hasPrimarySteel || hasPrimarySilverOrGold)
                    && (MATERIAL_PAIR.first == Material::Bronze))
                {
                    continue;
                }
                else if (
                    (MATERIAL_PAIR.first == Material::Tin)
                    || (MATERIAL_PAIR.first == Material::Amethyst)
                    || (MATERIAL_PAIR.first == Material::Emerald))
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
                                                             Material::IsRigid(MATERIAL_PAIR.second)
                                                             && (MATERIAL_PAIR.second
                                                                 != Material::Wood));
                                                     })
                                                 != std::end(reducedMaterialPairs) };

        if (HAS_SECONDARY_RIGID_NONWOOD)
        {
            reducedMaterialPairs.erase(
                std::remove_if(
                    std::begin(reducedMaterialPairs),
                    std::end(reducedMaterialPairs),
                    [](const auto & MATERIAL_PAIR) {
                        return (MATERIAL_PAIR.second == Material::Wood);
                    }),
                std::end(reducedMaterialPairs));
        }

        const auto HAS_SECONDARY_RIGID_NOT_BRONZE_TIN_IRON {
            std::find_if(
                std::begin(reducedMaterialPairs),
                std::end(reducedMaterialPairs),
                [](const auto & MATERIAL_PAIR) {
                    return (
                        Material::IsRigid(MATERIAL_PAIR.second)
                        && ((MATERIAL_PAIR.second != Material::Tin)
                            && (MATERIAL_PAIR.second != Material::Bronze)
                            && (MATERIAL_PAIR.second != Material::Iron)));
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
                            Material::IsRigid(MATERIAL_PAIR.second)
                            && ((MATERIAL_PAIR.second == Material::Tin)
                                || (MATERIAL_PAIR.second == Material::Bronze)
                                || (MATERIAL_PAIR.second == Material::Iron)));
                    }),
                std::end(reducedMaterialPairs));
        }

        return reducedMaterialPairs;
    }

    const MaterialVec_t MaterialFactory::RemoveCopy(
        const MaterialVec_t & ORIG_MATERIALS, const Material::Enum MATERIAL_TO_REMOVE) const
    {
        MaterialVec_t materials { ORIG_MATERIALS };

        materials.erase(
            std::remove(std::begin(materials), std::end(materials), MATERIAL_TO_REMOVE),
            std::end(materials));

        return materials;
    }

    void MaterialFactory::CleanupMaterialPairs(
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
            RemoveLameMaterialsFromSpecialItems(materialPairs);
        }
    }

} // namespace item
} // namespace heroespath
