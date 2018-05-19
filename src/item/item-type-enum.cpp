// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-type-enum.cpp
//
#include "item-type-enum.hpp"

#include "game/game-data-file.hpp"

#include <utility>

namespace heroespath
{
namespace item
{

    void category::ToStringPopulate(
        std::ostringstream & ss, const misc::EnumUnderlying_t ENUM_VALUE, const std::string &)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::Broken, "broken");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::Useable, "useable");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::BodyPart, "bodypart");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::Equippable, "equippable");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::Wearable, "wearable");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::OneHanded, "one-handed");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::TwoHanded, "two-handed");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::ConsumedOnUse, "consumed upon use");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, category::ShowsEnemyInfo, "shows enemy info");
    }

    void element_type::ToStringPopulate(
        std::ostringstream & ss, const misc::EnumUnderlying_t ENUM_VALUE, const std::string &)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, element_type::Fire, "Fire");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, element_type::Frost, "Frost");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, element_type::Honor, "Honor");
        AppendNameIfBitIsSet(ss, ENUM_VALUE, element_type::Shadow, "Shadow");
    }

    const std::string element_type::Name(const Enum ELEMENT_TYPE, const bool INCLUDE_OF)
    {
        std::ostringstream ss;

        auto appendIfBitIsSet{ [&](const element_type::Enum BIT) {
            if (ELEMENT_TYPE & BIT)
            {
                if (ss.str().empty())
                {
                    if (INCLUDE_OF)
                    {
                        ss << "of ";
                    }
                }
                else
                {
                    ss << " and ";
                }

                ss << element_type::ToString(BIT);
            }
        } };

        appendIfBitIsSet(element_type::Fire);
        appendIfBitIsSet(element_type::Frost);
        appendIfBitIsSet(element_type::Honor);
        appendIfBitIsSet(element_type::Shadow);

        return ss.str();
    }

    bool element_type::IsValid(const element_type::Enum E)
    {
        if (((E & element_type::Fire) && (E & element_type::Frost))
            || ((E & element_type::Honor) && (E & element_type::Shadow)))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    const std::string misc_type::ToString(const misc_type::Enum MISC_TYPE)
    {
        switch (MISC_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case AngelBraid:
            {
                return "AngelBraid";
            }
            case Crumhorn:
            {
                return "Crumhorn";
            }
            case DevilHorn:
            {
                return "DevilHorn";
            }
            case GolemFinger:
            {
                return "GolemFinger";
            }
            case HurdyGurdy:
            {
                return "HurdyGurdy";
            }
            case Icicle:
            {
                return "Icicle";
            }
            case LitchHand:
            {
                return "LitchHand";
            }
            case Lyre:
            {
                return "Lyre";
            }
            case MummyHand:
            {
                return "MummyHand";
            }
            case PetrifiedSnake:
            {
                return "PetrifiedSnake";
            }
            case PipeAndTabor:
            {
                return "PipeAndTabor";
            }
            case Recorder:
            {
                return "Recorder";
            }
            case UnicornHorn:
            {
                return "UnicornHorn";
            }
            case Viol:
            {
                return "Viol";
            }
            case BasiliskTonge:
            {
                return "BasiliskTonge";
            }
            case BerserkersBeard:
            {
                return "BerserkersBeard";
            }
            case BishopsHanky:
            {
                return "BishopsHanky";
            }
            case BleedingTrophy:
            {
                return "BleedingTrophy";
            }
            case BloodyDragonScale:
            {
                return "BloodyDragonScale";
            }
            case BottleOfBansheeScreams:
            {
                return "BottleOfBansheeScreams";
            }
            case BronzeTroll:
            {
                return "BronzeTroll";
            }
            case Brooch_Crown:
            {
                return "Brooch_Crown";
            }
            case Brooch_Feather:
            {
                return "Brooch_Feather";
            }
            case Brooch_Fist:
            {
                return "Brooch_Fist";
            }
            case Brooch_Hourglass:
            {
                return "Brooch_Hourglass";
            }
            case Brooch_Key:
            {
                return "Brooch_Key";
            }
            case Brooch_Mask:
            {
                return "Brooch_Mask";
            }
            case BurialShroud:
            {
                return "BurialShroud";
            }
            case CapeCommanders:
            {
                return "CapeCommanders";
            }
            case CapeGenerals:
            {
                return "CapeGenerals";
            }
            case CapeKings:
            {
                return "CapeKings";
            }
            case Charm_Crown:
            {
                return "Charm_Crown";
            }
            case Charm_Feather:
            {
                return "Charm_Feather";
            }
            case Charm_Fist:
            {
                return "Charm_Fist";
            }
            case Charm_Hourglass:
            {
                return "Charm_Hourglass";
            }
            case Charm_Key:
            {
                return "Charm_Key";
            }
            case Charm_Mask:
            {
                return "Charm_Mask";
            }
            case ChimeraBone:
            {
                return "ChimeraBone";
            }
            case CobraTooth:
            {
                return "CobraTooth";
            }
            case CrystalChimes:
            {
                return "CrystalChimes";
            }
            case DemonDiary:
            {
                return "DemonDiary";
            }
            case DoveBloodVial:
            {
                return "DoveBloodVial";
            }
            case DragonToothWhistle:
            {
                return "DragonToothWhistle";
            }
            case DriedFrog:
            {
                return "DriedFrog";
            }
            case DriedGecko:
            {
                return "DriedGecko";
            }
            case DriedIguana:
            {
                return "DriedIguana";
            }
            case DriedLizard:
            {
                return "DriedLizard";
            }
            case DriedSalamander:
            {
                return "DriedSalamander";
            }
            case DriedSkink:
            {
                return "DriedSkink";
            }
            case DriedToad:
            {
                return "DriedToad";
            }
            case DruidLeaf:
            {
                return "DruidLeaf";
            }
            case EvilRabbitsFoot:
            {
                return "EvilRabbitsFoot";
            }
            case ExoticGoldenGong:
            {
                return "ExoticGoldenGong";
            }
            case EyeCyclops:
            {
                return "EyeCyclops";
            }
            case EyeGiantOwl:
            {
                return "EyeGiantOwl";
            }
            case EyeHawk:
            {
                return "EyeHawk";
            }
            case FlagFanatics:
            {
                return "FlagFanatics";
            }
            case FlagRegalCaptains:
            {
                return "FlagRegalCaptains";
            }
            case FlagTribal:
            {
                return "FlagTribal";
            }
            case FriarsChronicle:
            {
                return "FriarsChronicle";
            }
            case FuneralRecord:
            {
                return "FuneralRecord";
            }
            case GhostSheet:
            {
                return "GhostSheet";
            }
            case GlassCat:
            {
                return "GlassCat";
            }
            case GriffinFeather:
            {
                return "GriffinFeather";
            }
            case HangmansNoose:
            {
                return "HangmansNoose";
            }
            case HobgoblinNose:
            {
                return "HobgoblinNose";
            }
            case HolyEpic:
            {
                return "HolyEpic";
            }
            case HornOfTheHorde:
            {
                return "HornOfTheHorde";
            }
            case ImpTail:
            {
                return "ImpTail";
            }
            case IslanderHeaddress:
            {
                return "IslanderHeaddress";
            }
            case JeweledArmband:
            {
                return "JeweledArmband";
            }
            case JeweledHandbag:
            {
                return "JeweledHandbag";
            }
            case JeweledPrincessVeil:
            {
                return "JeweledPrincessVeil";
            }
            case LastRitesScroll:
            {
                return "LastRitesScroll";
            }
            case MacabreManuscript:
            {
                return "MacabreManuscript";
            }
            case MadRatJuju:
            {
                return "MadRatJuju";
            }
            case MagicHorseshoe:
            {
                return "MagicHorseshoe";
            }
            case MagnifyingGlass:
            {
                return "MagnifyingGlass";
            }
            case ManaAmulet:
            {
                return "ManaAmulet";
            }
            case MaskMourners:
            {
                return "MaskMourners";
            }
            case MaskRascal:
            {
                return "MaskRascal";
            }
            case MinotaurHide:
            {
                return "MinotaurHide";
            }
            case MortuaryOrnament:
            {
                return "MortuaryOrnament";
            }
            case NecklaceJeweledAnkh:
            {
                return "NecklaceJeweledAnkh";
            }
            case NecklaceSharkTooth:
            {
                return "NecklaceSharkTooth";
            }
            case NecklaceVampiresTooth:
            {
                return "NecklaceVampiresTooth";
            }
            case PantherPaw:
            {
                return "PantherPaw";
            }
            case Pin_Crown:
            {
                return "Pin_Crown";
            }
            case Pin_Feather:
            {
                return "Pin_Feather";
            }
            case Pin_Fist:
            {
                return "Pin_Fist";
            }
            case Pin_Hourglass:
            {
                return "Pin_Hourglass";
            }
            case Pin_Key:
            {
                return "Pin_Key";
            }
            case Pin_Mask:
            {
                return "Pin_Mask";
            }
            case PixieBell:
            {
                return "PixieBell";
            }
            case RattlesnakeTail:
            {
                return "RattlesnakeTail";
            }
            case RavenClaw:
            {
                return "RavenClaw";
            }
            case ReaperScythe:
            {
                return "ReaperScythe";
            }
            case RequiemRegister:
            {
                return "RequiemRegister";
            }
            case RingHobo:
            {
                return "RingHobo";
            }
            case RingMendicant:
            {
                return "RingMendicant";
            }
            case RingMonk:
            {
                return "RingMonk";
            }
            case RingPriest:
            {
                return "RingPriest";
            }
            case RoyalScoutSpyglass:
            {
                return "RoyalScoutSpyglass";
            }
            case SaintCameoPin:
            {
                return "SaintCameoPin";
            }
            case SaintsJournal:
            {
                return "SaintsJournal";
            }
            case SanguineRelic:
            {
                return "SanguineRelic";
            }
            case ScoundrelSack:
            {
                return "ScoundrelSack";
            }
            case SepultureDecoration:
            {
                return "SepultureDecoration";
            }
            case ShadeCloak:
            {
                return "ShadeCloak";
            }
            case ShamanRainmaker:
            {
                return "ShamanRainmaker";
            }
            case SirenConch:
            {
                return "SirenConch";
            }
            case SpecterChains:
            {
                return "SpecterChains";
            }
            case SpecterRobe:
            {
                return "SpecterRobe";
            }
            case SpiderEggs:
            {
                return "SpiderEggs";
            }
            case SprintersLegtie:
            {
                return "SprintersLegtie";
            }
            case SwindlersBag:
            {
                return "SwindlersBag";
            }
            case TricksterPouch:
            {
                return "TricksterPouch";
            }
            case TuningFork:
            {
                return "TuningFork";
            }
            case TurtleShell:
            {
                return "TurtleShell";
            }
            case VultureGizzard:
            {
                return "VultureGizzard";
            }
            case WarhorseMarionette:
            {
                return "WarhorseMarionette";
            }
            case WarTrumpet:
            {
                return "WarTrumpet";
            }
            case WeaselTotem:
            {
                return "WeaselTotem";
            }
            case WolfenFur:
            {
                return "WolfenFur";
            }
            case WraithTalisman:
            {
                return "WraithTalisman";
            }
            case Bust:
            {
                return "Bust";
            }
            case Doll:
            {
                return "Doll";
            }
            case DollBlessed:
            {
                return "DollBlessed";
            }
            case DollCursed:
            {
                return "DollCursed";
            }
            case DriedHead:
            {
                return "DriedHead";
            }
            case DrumLute:
            {
                return "DrumLute";
            }
            case Egg:
            {
                return "Egg";
            }
            case Embryo:
            {
                return "Embryo";
            }
            case FigurineBlessed:
            {
                return "FigurineBlessed";
            }
            case FigurineCursed:
            {
                return "FigurineCursed";
            }
            case Goblet:
            {
                return "Goblet";
            }
            case Key:
            {
                return "Key";
            }
            case LockPicks:
            {
                return "LockPicks";
            }
            case Mirror:
            {
                return "Mirror";
            }
            case Orb:
            {
                return "Orb";
            }
            case Pendant:
            {
                return "Pendant";
            }
            case PuppetBlessed:
            {
                return "PuppetBlessed";
            }
            case PuppetCursed:
            {
                return "PuppetCursed";
            }
            case Ring:
            {
                return "Ring";
            }
            case Scepter:
            {
                return "Scepter";
            }
            case Seeds:
            {
                return "Seeds";
            }
            case Shard:
            {
                return "Shard";
            }
            case Staff:
            {
                return "Staff";
            }
            case SummoningStatue:
            {
                return "SummoningStatue";
            }
            case Wand:
            {
                return "Wand";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MISC_TYPE, "ToString");
            }
        }
    }

    const std::string misc_type::Name(const misc_type::Enum MISC_TYPE)
    {
        switch (MISC_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case AngelBraid:
            {
                return "Angel Braid";
            }
            case Crumhorn:
            {
                return "Crumhorn";
            }
            case DevilHorn:
            {
                return "Devil Horn";
            }
            case GolemFinger:
            {
                return "Golem Finger";
            }
            case HurdyGurdy:
            {
                return "Hannibal's Hurdy Gurdy";
            }
            case Icicle:
            {
                return "Icicle";
            }
            case LitchHand:
            {
                return "Litch Hand";
            }
            case Lyre:
            {
                return "Satyr's Lyre";
            }
            case MummyHand:
            {
                return "Mummy Hand";
            }
            case PetrifiedSnake:
            {
                return "Petrified Snake";
            }
            case PipeAndTabor:
            {
                return "Puritan's Pipe And Tabor";
            }
            case Recorder:
            {
                return "Reverend's Recorder";
            }
            case UnicornHorn:
            {
                return "Unicorn Horn";
            }
            case Viol:
            {
                return "Villain's Viol";
            }
            case BasiliskTonge:
            {
                return "Basilisk Tonge";
            }
            case BerserkersBeard:
            {
                return "Berserker's Beard";
            }
            case BishopsHanky:
            {
                return "Bishop's Hanky";
            }
            case BleedingTrophy:
            {
                return "Bleeding Trophy";
            }
            case BloodyDragonScale:
            {
                return "Bloody Dragon Scale";
            }
            case BottleOfBansheeScreams:
            {
                return "Bottle Of Banshee Screams";
            }
            case BronzeTroll:
            {
                return "Bronze Troll";
            }
            case Brooch_Crown:
            {
                return "Crown Brooch";
            }
            case Brooch_Feather:
            {
                return "Feather Brooch";
            }
            case Brooch_Fist:
            {
                return "Fist Brooch";
            }
            case Brooch_Hourglass:
            {
                return "Hourglass Brooch";
            }
            case Brooch_Key:
            {
                return "Key Brooch";
            }
            case Brooch_Mask:
            {
                return "Mask Brooch";
            }
            case BurialShroud:
            {
                return "Burial Shroud";
            }
            case CapeCommanders:
            {
                return "Commander's Cape";
            }
            case CapeGenerals:
            {
                return "General's Cape";
            }
            case CapeKings:
            {
                return "King's Cape";
            }
            case Charm_Crown:
            {
                return "Crown Charm";
            }
            case Charm_Feather:
            {
                return "Feather Charm";
            }
            case Charm_Fist:
            {
                return "Fist Charm";
            }
            case Charm_Hourglass:
            {
                return "Hourglass Charm";
            }
            case Charm_Key:
            {
                return "Key Charm";
            }
            case Charm_Mask:
            {
                return "Mask Charm";
            }
            case ChimeraBone:
            {
                return "Chimera Bone";
            }
            case CobraTooth:
            {
                return "Cobra Tooth";
            }
            case CrystalChimes:
            {
                return "Crystal CrystalChimes";
            }
            case DemonDiary:
            {
                return "Demon's Diary";
            }
            case DoveBloodVial:
            {
                return "Dove Blood Vial";
            }
            case DragonToothWhistle:
            {
                return "Dragon Tooth Whistle";
            }
            case DriedFrog:
            {
                return "Dried Frog";
            }
            case DriedGecko:
            {
                return "Dried Gecko";
            }
            case DriedIguana:
            {
                return "Dried Iguana";
            }
            case DriedLizard:
            {
                return "Dried Lizard";
            }
            case DriedSalamander:
            {
                return "Dried Salamander";
            }
            case DriedSkink:
            {
                return "Dried Skink";
            }
            case DriedToad:
            {
                return "Dried Toad";
            }
            case DruidLeaf:
            {
                return "Druid Leaf";
            }
            case EvilRabbitsFoot:
            {
                return "Evil Rabbits Foot";
            }
            case ExoticGoldenGong:
            {
                return "Exotic Golden Gong";
            }
            case EyeCyclops:
            {
                return "Cyclop's Eye";
            }
            case EyeGiantOwl:
            {
                return "Giant Owl Eye";
            }
            case EyeHawk:
            {
                return "Hawk Eye";
            }
            case FlagFanatics:
            {
                return "Fanatic's Flag";
            }
            case FlagRegalCaptains:
            {
                return "Regal Captain's Flag";
            }
            case FlagTribal:
            {
                return "Tribal Flag";
            }
            case FriarsChronicle:
            {
                return "Friar's Chronicle";
            }
            case FuneralRecord:
            {
                return "Funeral Record";
            }
            case GhostSheet:
            {
                return "Ghost Sheet";
            }
            case GlassCat:
            {
                return "Glass Cat";
            }
            case GriffinFeather:
            {
                return "Griffin Feather";
            }
            case HangmansNoose:
            {
                return "Hangman's Noose";
            }
            case HobgoblinNose:
            {
                return "Hobgoblin Nose";
            }
            case HolyEpic:
            {
                return "Holy Epic";
            }
            case HornOfTheHorde:
            {
                return "Horn of The Horde";
            }
            case ImpTail:
            {
                return "Imp Tail";
            }
            case IslanderHeaddress:
            {
                return "Islander Headdress";
            }
            case JeweledArmband:
            {
                return "Armband";
            }
            case JeweledHandbag:
            {
                return "Handbag";
            }
            case JeweledPrincessVeil:
            {
                return "Princess Veil";
            }
            case LastRitesScroll:
            {
                return "Last Rites Scroll";
            }
            case MacabreManuscript:
            {
                return "Macabre Manuscript";
            }
            case MadRatJuju:
            {
                return "Mad Rat Juju";
            }
            case MagicHorseshoe:
            {
                return "Magic Horseshoe";
            }
            case MagnifyingGlass:
            {
                return "Magnifying Glass";
            }
            case ManaAmulet:
            {
                return "Mana Amulet";
            }
            case MaskMourners:
            {
                return "Mourner's Mask";
            }
            case MaskRascal:
            {
                return "Rascal Mask";
            }
            case MinotaurHide:
            {
                return "Minotaur Hide";
            }
            case MortuaryOrnament:
            {
                return "Mortuary Ornament";
            }
            case NecklaceJeweledAnkh:
            {
                return "Ankh Necklace ";
            }
            case NecklaceSharkTooth:
            {
                return "Shark Tooth Necklace";
            }
            case NecklaceVampiresTooth:
            {
                return "Vampire's Tooth Necklace";
            }
            case PantherPaw:
            {
                return "Panther Paw";
            }
            case Pin_Crown:
            {
                return "Crown Pin";
            }
            case Pin_Feather:
            {
                return "Feather Pin";
            }
            case Pin_Fist:
            {
                return "Fist Pin";
            }
            case Pin_Hourglass:
            {
                return "Hourglass Pin";
            }
            case Pin_Key:
            {
                return "Key Pin";
            }
            case Pin_Mask:
            {
                return "Mask Pin";
            }
            case PixieBell:
            {
                return "Pixie Bell";
            }
            case RattlesnakeTail:
            {
                return "Rattlesnake Tail";
            }
            case RavenClaw:
            {
                return "Raven Claw";
            }
            case ReaperScythe:
            {
                return "Reaper Scythe";
            }
            case RequiemRegister:
            {
                return "Requiem Register";
            }
            case RingHobo:
            {
                return "Hobo Ring";
            }
            case RingMendicant:
            {
                return "Mendicant Ring";
            }
            case RingMonk:
            {
                return "Monk Ring";
            }
            case RingPriest:
            {
                return "PriestRing";
            }
            case RoyalScoutSpyglass:
            {
                return "Royal Scout Spyglass";
            }
            case SaintCameoPin:
            {
                return "Saint Cameo Pin";
            }
            case SaintsJournal:
            {
                return "Saint's Journal";
            }
            case SanguineRelic:
            {
                return "Sanguine Relic";
            }
            case ScoundrelSack:
            {
                return "Scoundrel Sack";
            }
            case SepultureDecoration:
            {
                return "Sepulture Decoration";
            }
            case ShadeCloak:
            {
                return "Shade Cloak";
            }
            case ShamanRainmaker:
            {
                return "Shaman Rainmaker";
            }
            case SirenConch:
            {
                return "Siren Conch";
            }
            case SpecterChains:
            {
                return "Specter Chains";
            }
            case SpecterRobe:
            {
                return "Specter Robe";
            }
            case SpiderEggs:
            {
                return "Spider Eggs";
            }
            case SprintersLegtie:
            {
                return "Sprinter's Legtie";
            }
            case SwindlersBag:
            {
                return "Swindler's Bag";
            }
            case TricksterPouch:
            {
                return "Trickster Pouch";
            }
            case TuningFork:
            {
                return "Tuning Fork";
            }
            case TurtleShell:
            {
                return "Turtle Shell";
            }
            case VultureGizzard:
            {
                return "Vulture Gizzard";
            }
            case WarhorseMarionette:
            {
                return "Warhorse Marionette";
            }
            case WarTrumpet:
            {
                return "War Trumpet";
            }
            case WeaselTotem:
            {
                return "Weasel Totem";
            }
            case WolfenFur:
            {
                return "Wolfen Fur";
            }
            case WraithTalisman:
            {
                return "Wraith Talisman";
            }
            case Bust:
            {
                return "Bust";
            }
            case Doll:
            {
                return "Doll";
            }
            case DollBlessed:
            {
                return "Doll";
            }
            case DollCursed:
            {
                return "Doll";
            }
            case DriedHead:
            {
                return "Dried Head";
            }
            case DrumLute:
            {
                return "Drumlute";
            }
            case Egg:
            {
                return "Egg";
            }
            case Embryo:
            {
                return "Embryo";
            }
            case FigurineBlessed:
            {
                return "Figurine";
            }
            case FigurineCursed:
            {
                return "Figurine";
            }
            case Goblet:
            {
                return "Goblet";
            }
            case Key:
            {
                return "Key";
            }
            case LockPicks:
            {
                return "Lockpicks";
            }
            case Mirror:
            {
                return "Mirror";
            }
            case Orb:
            {
                return "Orb";
            }
            case Pendant:
            {
                return "Pendant";
            }
            case PuppetBlessed:
            {
                return "Puppet";
            }
            case PuppetCursed:
            {
                return "Puppet";
            }
            case Ring:
            {
                return "Ring";
            }
            case Scepter:
            {
                return "Scepter";
            }
            case Seeds:
            {
                return "Seeds";
            }
            case Shard:
            {
                return "Shard";
            }
            case Staff:
            {
                return "Staff";
            }
            case SummoningStatue:
            {
                return "Summoning Statue";
            }
            case Wand:
            {
                return "Wand";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MISC_TYPE, "Name");
            }
        }
    }

    bool misc_type::IsQuestItem(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == AngelBraid) || (MISC_TYPE == Crumhorn) || (MISC_TYPE == DevilHorn)
            || (MISC_TYPE == GolemFinger) || (MISC_TYPE == HurdyGurdy) || (MISC_TYPE == Icicle)
            || (MISC_TYPE == LitchHand) || (MISC_TYPE == Lyre) || (MISC_TYPE == MummyHand)
            || (MISC_TYPE == PetrifiedSnake) || (MISC_TYPE == PipeAndTabor)
            || (MISC_TYPE == Recorder) || (MISC_TYPE == UnicornHorn) || (MISC_TYPE == Viol));
    }

    bool misc_type::IsStandalone(const misc_type::Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == Bust) || (MISC_TYPE == Doll) || (MISC_TYPE == DollBlessed)
            || (MISC_TYPE == DollCursed) || (MISC_TYPE == DriedHead) || (MISC_TYPE == DrumLute)
            || (MISC_TYPE == Egg) || (MISC_TYPE == Embryo) || (MISC_TYPE == FigurineBlessed)
            || (MISC_TYPE == FigurineCursed) || (MISC_TYPE == Goblet) || (MISC_TYPE == Key)
            || (MISC_TYPE == LockPicks) || (MISC_TYPE == Mirror) || (MISC_TYPE == Orb)
            || (MISC_TYPE == Pendant) || (MISC_TYPE == PuppetBlessed) || (MISC_TYPE == PuppetCursed)
            || (MISC_TYPE == Ring) || (MISC_TYPE == Scepter) || (MISC_TYPE == Seeds)
            || (MISC_TYPE == Shard) || (MISC_TYPE == Staff) || (MISC_TYPE == SummoningStatue)
            || (MISC_TYPE == Wand));
    }

    bool misc_type::IsUnique(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE != Not) && (MISC_TYPE != Count) && (IsQuestItem(MISC_TYPE) == false)
            && (IsStandalone(MISC_TYPE) == false));
    }

    bool misc_type::HasPixieVersion(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == JeweledArmband) || (MISC_TYPE == CapeCommanders)
            || (MISC_TYPE == CapeKings) || (MISC_TYPE == CapeGenerals) || (MISC_TYPE == ShadeCloak)
            || (MISC_TYPE == GhostSheet) || (MISC_TYPE == SpecterRobe)
            || (MISC_TYPE == BurialShroud) || IsMusicalInstrument(MISC_TYPE)
            || (MISC_TYPE == Staff));
    }

    bool misc_type::HasOnlyPixieVersion(const misc_type::Enum) { return false; }

    bool misc_type::MustBePixieVersionForPixiesToEquip(const Enum MISC_TYPE)
    {
        return (
            IsMusicalInstrument(MISC_TYPE) || (MISC_TYPE == JeweledArmband)
            || (MISC_TYPE == CapeCommanders) || (MISC_TYPE == CapeKings)
            || (MISC_TYPE == CapeGenerals) || (MISC_TYPE == ShadeCloak) || (MISC_TYPE == GhostSheet)
            || (MISC_TYPE == SpecterRobe) || (MISC_TYPE == BurialShroud) || (MISC_TYPE == Staff) ||

            // these items must be pixie for pixies to equip, but there are no pixie versions of
            // these items so pixies can never equip them.
            (MISC_TYPE == ExoticGoldenGong) || (MISC_TYPE == IslanderHeaddress)
            || (MISC_TYPE == HornOfTheHorde) || (MISC_TYPE == WarTrumpet)
            || (MISC_TYPE == WarhorseMarionette));
    }

    float misc_type::ReligiousRatio(const misc_type::Enum MISC_TYPE)
    {
        if ((MISC_TYPE == SpecterChains) || (MISC_TYPE == DoveBloodVial))
        {
            return 0.1f;
        }
        else if ((MISC_TYPE == SpecterRobe) || (MISC_TYPE == ManaAmulet))
        {
            return 0.2f;
        }
        else if (
            (MISC_TYPE == ShadeCloak) || (MISC_TYPE == SanguineRelic)
            || (MISC_TYPE == EvilRabbitsFoot) || (MISC_TYPE == BleedingTrophy)
            || (MISC_TYPE == WraithTalisman))
        {
            return 0.5f;
        }
        else if (MISC_TYPE == Bust)
        {
            return 0.6f;
        }
        else if (MISC_TYPE == SepultureDecoration)
        {
            return 0.65f;
        }
        else if (
            (MISC_TYPE == RingMendicant) || (MISC_TYPE == DriedHead) || (MISC_TYPE == DemonDiary)
            || (MISC_TYPE == MacabreManuscript))
        {
            return 0.75f;
        }
        else if (
            (MISC_TYPE == BurialShroud) || (MISC_TYPE == MortuaryOrnament)
            || (MISC_TYPE == GhostSheet) || (MISC_TYPE == FuneralRecord)
            || (MISC_TYPE == FriarsChronicle))
        {
            return 0.8f;
        }
        else if (
            (MISC_TYPE == SaintsJournal) || (MISC_TYPE == SaintCameoPin)
            || (MISC_TYPE == RequiemRegister) || (MISC_TYPE == RingMonk)
            || (MISC_TYPE == LastRitesScroll) || (MISC_TYPE == HolyEpic) || (MISC_TYPE == DruidLeaf)
            || (MISC_TYPE == ShamanRainmaker) || (MISC_TYPE == FigurineBlessed)
            || (MISC_TYPE == FigurineCursed) || (MISC_TYPE == DollBlessed)
            || (MISC_TYPE == DollCursed))
        {
            return 0.9f;
        }
        else if ((MISC_TYPE == RingPriest) || (MISC_TYPE == BishopsHanky))
        {
            return 0.99f;
        }
        else
        {
            return 0.0f;
        }
    }

    bool misc_type::HasNonFleshEyes(const misc_type::Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == Bust) || (MISC_TYPE == Doll) || (MISC_TYPE == DollBlessed)
            || (MISC_TYPE == DollCursed) || (MISC_TYPE == FigurineBlessed)
            || (MISC_TYPE == FigurineCursed) || (MISC_TYPE == PuppetBlessed)
            || (MISC_TYPE == PuppetCursed) || (MISC_TYPE == MadRatJuju)
            || (MISC_TYPE == SummoningStatue) || (MISC_TYPE == WraithTalisman)
            || (MISC_TYPE == BronzeTroll) || (MISC_TYPE == WeaselTotem)
            || (MISC_TYPE == DriedHead)); // yeah it's flesh but what the hell having special
                                          // eyes is cool (oh and it's not edible so it's okay)
    }

    bool misc_type::IsBlessed(const misc_type::Enum E)
    {
        return (
            (E == DollBlessed) || (E == FigurineBlessed) || (E == PuppetBlessed) || (E == Bust));
    }

    bool misc_type::IsCursed(const misc_type::Enum E)
    {
        return (
            (E == DollCursed) || (E == FigurineCursed) || (E == PuppetCursed) || (E == DriedHead));
    }

    Weight_t misc_type::Weight(const misc_type::Enum MISC_TYPE)
    {
        // clang-format off
        switch (MISC_TYPE)
        {
            case DruidLeaf:             { return 1_weight;   }
            case GriffinFeather:        { return 2_weight;   }
            case CobraTooth:            { return 3_weight;   }
            case AngelBraid:            { return 4_weight;   }
            case BerserkersBeard:       { return 5_weight;   }
            case RattlesnakeTail:       { return 6_weight;   }
            case RavenClaw:             { return 6_weight;   }
            case EyeHawk:               { return 7_weight;   }
            case EyeGiantOwl:           { return 8_weight;   }
            case EyeCyclops:            { return 9_weight;   }
            case BishopsHanky:          { return 9_weight;   }
            case EvilRabbitsFoot:       { return 9_weight;   }
            case HobgoblinNose:         { return 10_weight;  }
            case PantherPaw:            { return 10_weight;  }
            case DragonToothWhistle:    { return 10_weight;  }
            case Ring:                  { return 10_weight;  }
            case RingHobo:              { return 11_weight;  }
            case RingMendicant:         { return 11_weight;  }
            case RingMonk:              { return 11_weight;  }
            case RingPriest:            { return 11_weight;  }
            case Pin_Crown:             { return 11_weight;  }
            case Pin_Feather:           { return 11_weight;  }
            case Pin_Fist:              { return 11_weight;  }
            case Pin_Hourglass:         { return 11_weight;  }
            case Pin_Key:               { return 11_weight;  }
            case Pin_Mask:              { return 11_weight;  }
            case SaintCameoPin:         { return 11_weight;  }
            case NecklaceJeweledAnkh:   { return 12_weight;  }
            case NecklaceSharkTooth:    { return 12_weight;  }
            case NecklaceVampiresTooth: { return 12_weight;  }
            case Brooch_Crown:          { return 15_weight;  }
            case Brooch_Feather:        { return 15_weight;  }
            case Brooch_Fist:           { return 15_weight;  }
            case Brooch_Hourglass:      { return 15_weight;  }
            case Brooch_Key:            { return 15_weight;  }
            case Brooch_Mask:           { return 15_weight;  }
            case Charm_Crown:           { return 17_weight;  }
            case Charm_Feather:         { return 17_weight;  }
            case Charm_Fist:            { return 17_weight;  }
            case Charm_Hourglass:       { return 17_weight;  }
            case Charm_Key:             { return 17_weight;  }
            case Charm_Mask:            { return 17_weight;  }
            case Seeds:                 { return 18_weight;  }
            case Wand:                  { return 20_weight;  }
            case MummyHand:             { return 25_weight;  }
            case LitchHand:             { return 25_weight;  }
            case Icicle:                { return 26_weight;  }
            case UnicornHorn:           { return 27_weight;  }
            case DevilHorn:             { return 28_weight;  }
            case Scepter:               { return 29_weight;  }//intentinoally set low
            case FlagFanatics:          { return 30_weight;  }
            case FlagTribal:            { return 30_weight;  }
            case FlagRegalCaptains:     { return 30_weight;  }
            case MadRatJuju:            { return 31_weight;  }
            case Pendant:               { return 31_weight;  }
            case ManaAmulet:            { return 33_weight;  }
            case WeaselTotem:           { return 33_weight;  }
            case SprintersLegtie:       { return 33_weight;  }
            case HangmansNoose:         { return 34_weight;  }
            case JeweledPrincessVeil:   { return 35_weight;  }
            case Doll:                  { return 35_weight;  }
            case DollCursed:            { return 35_weight;  }
            case PuppetCursed:          { return 35_weight;  }
            case DollBlessed:           { return 35_weight;  }
            case FigurineBlessed:       { return 35_weight;  }
            case PuppetBlessed:         { return 35_weight;  }
            case FigurineCursed:        { return 35_weight;  }
            case Recorder:              { return 36_weight;  }
            case SirenConch:            { return 36_weight;  }
            case TurtleShell:           { return 36_weight;  }
            case Embryo:                { return 36_weight;  }
            case ChimeraBone:           { return 37_weight;  }
            case DriedIguana:           { return 38_weight;  }
            case DriedSkink:            { return 38_weight;  }
            case DriedGecko:            { return 38_weight;  }
            case DriedLizard:           { return 38_weight;  }
            case DriedFrog:             { return 38_weight;  }
            case DriedSalamander:       { return 38_weight;  }
            case DriedToad:             { return 38_weight;  }
            case PetrifiedSnake:        { return 38_weight;  }
            case BasiliskTonge:         { return 38_weight;  }
            case VultureGizzard:        { return 38_weight;  }
            case ImpTail:               { return 39_weight;  }
            case BloodyDragonScale:     { return 39_weight;  }
            case DriedHead:             { return 40_weight;  }
            case MaskRascal:            { return 40_weight;  }
            case MaskMourners:          { return 40_weight;  }
            case TuningFork:            { return 50_weight;  }
            case Key:                   { return 50_weight;  }
            case LockPicks:             { return 55_weight;  }
            case PixieBell:             { return 55_weight;  }
            case ScoundrelSack:         { return 60_weight;  }
            case SwindlersBag:          { return 60_weight;  }
            case TricksterPouch:        { return 60_weight;  }
            case JeweledHandbag:        { return 60_weight;  }
            case DoveBloodVial:         { return 62_weight;  }
            case BottleOfBansheeScreams:{ return 62_weight;  }
            case Mirror:                { return 63_weight;  }
            case MagnifyingGlass:       { return 64_weight;  }
            case JeweledArmband:        { return 65_weight;  }
            case LastRitesScroll:       { return 65_weight;  }
            case ShamanRainmaker:       { return 65_weight;  }
            case CapeCommanders:        { return 66_weight;  }
            case CapeGenerals:          { return 66_weight;  }
            case CapeKings:             { return 66_weight;  }
            case WolfenFur:             { return 70_weight;  }
            case MinotaurHide:          { return 70_weight;  }
            case WraithTalisman:        { return 75_weight;  }
            case SepultureDecoration:   { return 75_weight;  }
            case MortuaryOrnament:      { return 75_weight;  }
            case BleedingTrophy:        { return 75_weight;  }
            case SanguineRelic:         { return 75_weight;  }
            case GlassCat:              { return 75_weight;  }
            case Goblet:                { return 78_weight;  }
            case GhostSheet:            { return 80_weight;  }
            case ShadeCloak:            { return 80_weight;  }
            case SpecterRobe:           { return 80_weight;  }
            case BurialShroud:          { return 80_weight;  }
            case FuneralRecord:         { return 120_weight; }
            case MacabreManuscript:     { return 120_weight; }
            case RequiemRegister:       { return 120_weight; }
            case DemonDiary:            { return 120_weight; }
            case SaintsJournal:         { return 120_weight; }
            case FriarsChronicle:       { return 120_weight; }
            case HolyEpic:              { return 120_weight; }
            case MagicHorseshoe:        { return 130_weight; }
            case GolemFinger:           { return 140_weight; }
            case Shard:                 { return 140_weight; }
            case Crumhorn:              { return 140_weight; }
            case Staff:                 { return 150_weight; }
            case SummoningStatue:       { return 160_weight; }
            case BronzeTroll:           { return 160_weight; }
            case RoyalScoutSpyglass:    { return 160_weight; }
            case Orb:                   { return 160_weight; }
            case Bust:                  { return 165_weight; }
            case Egg:                   { return 200_weight; }
            case SpiderEggs:            { return 200_weight; }
            case Lyre:                  { return 205_weight; }
            case CrystalChimes:         { return 205_weight; }
            case IslanderHeaddress:     { return 210_weight; }
            case PipeAndTabor:          { return 220_weight; }
            case Viol:                  { return 230_weight; }
            case DrumLute:              { return 250_weight; }
            case ReaperScythe:          { return 260_weight; }
            case HurdyGurdy:            { return 280_weight; }
            case WarTrumpet:            { return 300_weight; }
            case HornOfTheHorde:        { return 300_weight; }
            case WarhorseMarionette:    { return 320_weight; }
            case SpecterChains:         { return 400_weight; }
            case ExoticGoldenGong:      { return 450_weight; }
            case Not:
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MISC_TYPE, "Weight");
            }
        }
        // clang-format on
    }

    bool misc_type::IsUseable(const misc_type::Enum MISC_TYPE)
    {
        return (
            IsSummoning(MISC_TYPE) || (MISC_TYPE == CrystalChimes) || (MISC_TYPE == DoveBloodVial)
            || (MISC_TYPE == DragonToothWhistle) || (MISC_TYPE == DriedFrog)
            || (MISC_TYPE == DriedGecko) || (MISC_TYPE == DriedIguana) || (MISC_TYPE == DriedLizard)
            || (MISC_TYPE == DriedSalamander) || (MISC_TYPE == DriedSkink)
            || (MISC_TYPE == DriedToad) || (MISC_TYPE == ExoticGoldenGong)
            || (MISC_TYPE == MagnifyingGlass) || (MISC_TYPE == PixieBell)
            || (MISC_TYPE == ShamanRainmaker) || (MISC_TYPE == SpecterChains)
            || (MISC_TYPE == VultureGizzard) || (MISC_TYPE == WarTrumpet)
            || (MISC_TYPE == WraithTalisman) || (MISC_TYPE == Key)
            || (MISC_TYPE == BottleOfBansheeScreams));
    }

    bool misc_type::IsMusicalInstrument(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == DrumLute) || (MISC_TYPE == Crumhorn) || (MISC_TYPE == HurdyGurdy)
            || (MISC_TYPE == Lyre) || (MISC_TYPE == PipeAndTabor) || (MISC_TYPE == Recorder)
            || (MISC_TYPE == Viol));
    }

    bool misc_type::IsSummoning(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == Egg) || (MISC_TYPE == Embryo) || (MISC_TYPE == Seeds)
            || (MISC_TYPE == SummoningStatue) || ((MISC_TYPE == SpiderEggs)));
    }

    category::Enum misc_type::EquipCategory(const Enum MISC_TYPE)
    {
        switch (MISC_TYPE)
        {
            case BerserkersBeard:
            case Brooch_Crown:
            case Brooch_Feather:
            case Brooch_Fist:
            case Brooch_Hourglass:
            case Brooch_Key:
            case Brooch_Mask:
            case BurialShroud:
            case CapeCommanders:
            case CapeGenerals:
            case CapeKings:
            case Charm_Crown:
            case Charm_Feather:
            case Charm_Fist:
            case Charm_Hourglass:
            case Charm_Key:
            case Charm_Mask:
            case DruidLeaf:
            case GhostSheet:
            case HangmansNoose:
            case IslanderHeaddress:
            case JeweledArmband:
            case JeweledPrincessVeil:
            case ManaAmulet:
            case MaskMourners:
            case MaskRascal:
            case NecklaceJeweledAnkh:
            case NecklaceSharkTooth:
            case NecklaceVampiresTooth:
            case Pin_Crown:
            case Pin_Feather:
            case Pin_Fist:
            case Pin_Hourglass:
            case Pin_Key:
            case Pin_Mask:
            case RingHobo:
            case RingMendicant:
            case RingMonk:
            case RingPriest:
            case SaintCameoPin:
            case ShadeCloak:
            case SpecterRobe:
            case SprintersLegtie:
            case Pendant:
            case Ring:
            {
                return static_cast<category::Enum>(category::Equippable | category::Wearable);
            }

            case AngelBraid:
            case DevilHorn:
            case GolemFinger:
            case Icicle:
            case LitchHand:
            case MummyHand:
            case PetrifiedSnake:
            case UnicornHorn:
            case Orb:
            case Scepter:
            case Wand:
            case Shard:
            {
                return static_cast<category::Enum>(category::Equippable | category::OneHanded);
            }

            case Crumhorn:
            case HurdyGurdy:
            case Lyre:
            case PipeAndTabor:
            case Recorder:
            case Viol:
            case DrumLute:
            case ReaperScythe:
            case Staff:
            {
                return static_cast<category::Enum>(category::Equippable | category::TwoHanded);
            }

            case BasiliskTonge:
            case BishopsHanky:
            case BleedingTrophy:
            case BloodyDragonScale:
            case BottleOfBansheeScreams:
            case BronzeTroll:
            case ChimeraBone:
            case CobraTooth:
            case CrystalChimes:
            case DemonDiary:
            case DoveBloodVial:
            case DragonToothWhistle:
            case DriedFrog:
            case DriedGecko:
            case DriedIguana:
            case DriedLizard:
            case DriedSalamander:
            case DriedSkink:
            case DriedToad:
            case EvilRabbitsFoot:
            case ExoticGoldenGong:
            case EyeCyclops:
            case EyeGiantOwl:
            case EyeHawk:
            case FlagFanatics:
            case FlagRegalCaptains:
            case FlagTribal:
            case FriarsChronicle:
            case FuneralRecord:
            case GlassCat:
            case GriffinFeather:
            case MadRatJuju:
            case HobgoblinNose:
            case HolyEpic:
            case HornOfTheHorde:
            case ImpTail:
            case JeweledHandbag:
            case LastRitesScroll:
            case MacabreManuscript:
            case MagicHorseshoe:
            case MagnifyingGlass:
            case MinotaurHide:
            case MortuaryOrnament:
            case PantherPaw:
            case PixieBell:
            case RattlesnakeTail:
            case RavenClaw:
            case RequiemRegister:
            case RoyalScoutSpyglass:
            case SaintsJournal:
            case SanguineRelic:
            case ScoundrelSack:
            case SepultureDecoration:
            case ShamanRainmaker:
            case SirenConch:
            case SpecterChains:
            case Seeds:
            case SwindlersBag:
            case SpiderEggs:
            case TricksterPouch:
            case TuningFork:
            case TurtleShell:
            case VultureGizzard:
            case WarhorseMarionette:
            case WarTrumpet:
            case WeaselTotem:
            case WolfenFur:
            case WraithTalisman:
            case Bust:
            case Doll:
            case DollBlessed:
            case DollCursed:
            case DriedHead:
            case Egg:
            case Embryo:
            case FigurineBlessed:
            case FigurineCursed:
            case Goblet:
            case Key:
            case LockPicks:
            case Mirror:
            case PuppetBlessed:
            case PuppetCursed:
            case SummoningStatue:
            case Not:
            case Count:
            default:
            {
                return category::None;
            }
        }
    }

    bool misc_type::AllowsCasting(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == AngelBraid) || (MISC_TYPE == DevilHorn) || (MISC_TYPE == GolemFinger)
            || (MISC_TYPE == Icicle) || (MISC_TYPE == LitchHand) || (MISC_TYPE == MummyHand)
            || (MISC_TYPE == PetrifiedSnake) || (MISC_TYPE == UnicornHorn) || (MISC_TYPE == Orb)
            || (MISC_TYPE == Scepter) || (MISC_TYPE == Wand) || (MISC_TYPE == Shard)
            || (MISC_TYPE == Staff));
    }

    bool misc_type::IsWeapon(const Enum MISC_TYPE)
    {
        return (MISC_TYPE == ReaperScythe) || (MISC_TYPE == Staff);
    }

    bool misc_type::IsArmor(const Enum MISC_TYPE)
    {
        return (
            (MISC_TYPE == BurialShroud) || (MISC_TYPE == CapeCommanders)
            || (MISC_TYPE == CapeGenerals) || (MISC_TYPE == CapeKings) || (MISC_TYPE == GhostSheet)
            || (MISC_TYPE == ShadeCloak) || (MISC_TYPE == SpecterRobe));
    }

    const ElementEnumVec_t
        misc_type::ElementTypes(const Enum MISC_TYPE, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        if ((MISC_TYPE == CapeCommanders) || (MISC_TYPE == CapeGenerals) || (MISC_TYPE == CapeKings)
            || (MISC_TYPE == SaintCameoPin))
        {
            elementTypes = { element_type::Honor };
        }
        else if (MISC_TYPE == JeweledPrincessVeil)
        {
            elementTypes = { element_type::Honor, element_type::Shadow };
        }
        else if ((MISC_TYPE == JeweledArmband) || (MISC_TYPE == ManaAmulet))
        {
            elementTypes = {
                element_type::Fire, element_type::Frost, element_type::Honor, element_type::Shadow
            };
        }

        if (WILL_INCLUDE_NONE)
        {
            elementTypes.emplace_back(element_type::None);
        }

        return elementTypes;
    }

    const std::string set_type::ToString(const set_type::Enum SET_TYPE)
    {
        switch (SET_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case TheAssassins:
            {
                return "TheAssassins";
            }
            case TheNeverwinter:
            {
                return "TheNeverwinter";
            }
            case TheTickler:
            {
                return "TheTickler";
            }
            case TheMagus:
            {
                return "TheMagus";
            }
            case TheNecromancers:
            {
                return "TheNecromancers";
            }
            case TheWarlocks:
            {
                return "TheWarlocks";
            }
            case TheLichKings:
            {
                return "TheLichKings";
            }
            case TheSages:
            {
                return "TheSages";
            }
            case TheShamans:
            {
                return "TheShamans";
            }
            case TheOracles:
            {
                return "TheOracles";
            }
            case TheAngelic:
            {
                return "TheAngelic";
            }
            case TheBalladeers:
            {
                return "TheBalladeers";
            }
            case TheTroubadours:
            {
                return "TheTroubadours";
            }
            case TheMuses:
            {
                return "TheMuses";
            }
            case TheCavaliers:
            {
                return "TheCavaliers";
            }
            case TheChampions:
            {
                return "TheChampions";
            }
            case ThePaladins:
            {
                return "ThePaladins";
            }
            case TheBerserkers:
            {
                return "TheBerserkers";
            }
            case TheRosewood:
            {
                return "TheRosewood";
            }
            case TheDragonslayers:
            {
                return "TheDragonslayers";
            }
            case TheEventideRider:
            {
                return "TheEventideRider";
            }
            case TheHunters:
            {
                return "TheHunters";
            }
            case TheSureShot:
            {
                return "TheSureShot";
            }
            case TheMarksmans:
            {
                return "TheMarksmans";
            }
            case TheDeadeye:
            {
                return "TheDeadeye";
            }
            case TheDuskRanger:
            {
                return "TheDuskRanger";
            }
            case TheVenomBow:
            {
                return "TheVenomBow";
            }
            case TheCritterChannelers:
            {
                return "TheCritterChannelers";
            }
            case TheMammalianHead:
            {
                return "TheMammalianHead";
            }
            case TheSavageTaskmasters:
            {
                return "TheSavageTaskmasters";
            }
            case TheMonsterOverseers:
            {
                return "TheMonsterOverseers";
            }
            case TheBeastRulers:
            {
                return "TheBeastRulers";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(SET_TYPE, "ToString");
            }
        }
    }

    const std::string set_type::Name(const set_type::Enum SET_TYPE)
    {
        switch (SET_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case TheAssassins:
            {
                return "The Assassin's";
            }
            case TheNeverwinter:
            {
                return "The Neverwinter";
            }
            case TheTickler:
            {
                return "The Tickler's";
            }
            case TheMagus:
            {
                return "The Magus'";
            }
            case TheNecromancers:
            {
                return "The Necromancer's";
            }
            case TheWarlocks:
            {
                return "The Warlock's";
            }
            case TheLichKings:
            {
                return "The Lich King's";
            }
            case TheSages:
            {
                return "The Sage's";
            }
            case TheShamans:
            {
                return "The Shaman's";
            }
            case TheOracles:
            {
                return "The Oracle's";
            }
            case TheAngelic:
            {
                return "The Angelic";
            }
            case TheBalladeers:
            {
                return "The Balladeer's";
            }
            case TheTroubadours:
            {
                return "The Troubadour's";
            }
            case TheMuses:
            {
                return "The Muses'";
            }
            case TheCavaliers:
            {
                return "The Cavalier's";
            }
            case TheChampions:
            {
                return "The Champion's";
            }
            case ThePaladins:
            {
                return "The Paladin's";
            }
            case TheBerserkers:
            {
                return "The Berserker's";
            }
            case TheRosewood:
            {
                return "The Rosewood";
            }
            case TheDragonslayers:
            {
                return "The Dragonslayer's";
            }
            case TheEventideRider:
            {
                return "The Eventide Rider's";
            }
            case TheHunters:
            {
                return "The Hunter's";
            }
            case TheSureShot:
            {
                return "The Sure Shot's";
            }
            case TheMarksmans:
            {
                return "The Marksman's";
            }
            case TheDeadeye:
            {
                return "The Deadeye's";
            }
            case TheDuskRanger:
            {
                return "The Dusk Ranger's";
            }
            case TheVenomBow:
            {
                return "The Venom Bow";
            }
            case TheCritterChannelers:
            {
                return "The Critter Channeler's";
            }
            case TheMammalianHead:
            {
                return "The Mammalian Head's";
            }
            case TheSavageTaskmasters:
            {
                return "The Savage Taskmaster's";
            }
            case TheMonsterOverseers:
            {
                return "The Monster Overseer's";
            }
            case TheBeastRulers:
            {
                return "The Beast Ruler's";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(SET_TYPE, "Name");
            }
        }
    }

    creature::role::Enum set_type::Role(const set_type::Enum SET_TYPE)
    {
        switch (SET_TYPE)
        {
            case Not:
            {
                return creature::role::Count;
            }
            case TheAssassins:
            case TheTickler:
            case TheNeverwinter:
            {
                return creature::role::Thief;
            }
            case TheMagus:
            case TheNecromancers:
            case TheWarlocks:
            case TheLichKings:
            {
                return creature::role::Sorcerer;
            }
            case TheSages:
            case TheShamans:
            case TheOracles:
            case TheAngelic:
            {
                return creature::role::Cleric;
            }
            case TheBalladeers:
            case TheTroubadours:
            case TheMuses:
            {
                return creature::role::Bard;
            }
            case TheCavaliers:
            case TheChampions:
            case ThePaladins:
            case TheBerserkers:
            case TheRosewood:
            case TheDragonslayers:
            case TheEventideRider:
            {
                return creature::role::Knight;
            }
            case TheHunters:
            case TheSureShot:
            case TheMarksmans:
            case TheDeadeye:
            case TheDuskRanger:
            case TheVenomBow:
            {
                return creature::role::Archer;
            }
            case TheCritterChannelers:
            case TheMammalianHead:
            case TheSavageTaskmasters:
            case TheMonsterOverseers:
            case TheBeastRulers:
            {
                return creature::role::Beastmaster;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(SET_TYPE, "Role");
            }
        }
    }

    const std::string named_type::ToString(const named_type::Enum NAMED_TYPE)
    {
        switch (NAMED_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case Arctic:
            {
                return "Arctic";
            }
            case Army:
            {
                return "Army";
            }
            case Betrayer:
            {
                return "Betrayer";
            }
            case Burglar:
            {
                return "Burglar";
            }
            case Burning:
            {
                return "Burning";
            }
            case Charlatans:
            {
                return "Charlatans";
            }
            case Charred:
            {
                return "Charred";
            }
            case Chill:
            {
                return "Chill";
            }
            case Dancing:
            {
                return "Dancing";
            }
            case Daring:
            {
                return "Daring";
            }
            case Dark:
            {
                return "Dark";
            }
            case Dawn:
            {
                return "Dawn";
            }
            case Diabolic:
            {
                return "Diabolic";
            }
            case Dusk:
            {
                return "Dusk";
            }
            case Elite:
            {
                return "Elite";
            }
            case Fiendish:
            {
                return "Fiendish";
            }
            case Fiery:
            {
                return "Fiery";
            }
            case Focus:
            {
                return "Focus";
            }
            case Frigid:
            {
                return "Frigid";
            }
            case Frozen:
            {
                return "Frozen";
            }
            case Gladiator:
            {
                return "Gladiator";
            }
            case Gloom:
            {
                return "Gloom";
            }
            case Glory:
            {
                return "Glory";
            }
            case Heros:
            {
                return "Heros";
            }
            case Honest:
            {
                return "Honest";
            }
            case Icy:
            {
                return "Icy";
            }
            case Imposters:
            {
                return "Imposters";
            }
            case Infernal:
            {
                return "Infernal";
            }
            case Knaves:
            {
                return "Knaves";
            }
            case Light:
            {
                return "Light";
            }
            case Marauder:
            {
                return "Marauder";
            }
            case Misery:
            {
                return "Misery";
            }
            case Moon:
            {
                return "Moon";
            }
            case Mountebank:
            {
                return "Mountebank";
            }
            case Muggers:
            {
                return "Muggers";
            }
            case Nile:
            {
                return "Nile";
            }
            case Noble:
            {
                return "Noble";
            }
            case Pickpocket:
            {
                return "Pickpocket";
            }
            case Pirate:
            {
                return "Pirate";
            }
            case Princes:
            {
                return "Princes";
            }
            case Proud:
            {
                return "Proud";
            }
            case Pure:
            {
                return "Pure";
            }
            case Raging:
            {
                return "Raging";
            }
            case Ranger:
            {
                return "Ranger";
            }
            case Robbers:
            {
                return "Robbers";
            }
            case Samurai:
            {
                return "Samurai";
            }
            case Searing:
            {
                return "Searing";
            }
            case Soldiers:
            {
                return "Soldiers";
            }
            case Sorrow:
            {
                return "Sorrow";
            }
            case Sun:
            {
                return "Sun";
            }
            case Thief:
            {
                return "Theif";
            }
            case Thors:
            {
                return "Thors";
            }
            case Thugs:
            {
                return "Thugs";
            }
            case Twilight:
            {
                return "Twilight";
            }
            case Valiant:
            {
                return "Valiant";
            }
            case Wardens:
            {
                return "Wardens";
            }
            case Wicked:
            {
                return "Wicked";
            }
            case Winter:
            {
                return "Winter";
            }
            case Woe:
            {
                return "Woe";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(NAMED_TYPE, "ToString");
            }
        }
    }

    const std::string named_type::Name(const named_type::Enum NAMED_TYPE)
    {
        switch (NAMED_TYPE)
        {
            case Charlatans:
            {
                return "Charlatan's";
            }
            case Heros:
            {
                return "Hero's";
            }
            case Imposters:
            {
                return "Imposter's";
            }
            case Knaves:
            {
                return "Knave's";
            }
            case Muggers:
            {
                return "Mugger's";
            }
            case Princes:
            {
                return "Prince's";
            }
            case Robbers:
            {
                return "Robber's";
            }
            case Soldiers:
            {
                return "Soldier's";
            }
            case Thors:
            {
                return "Thor's";
            }
            case Thugs:
            {
                return "Thug's";
            }
            case Wardens:
            {
                return "Warden's";
            }
            case Not:
            case Arctic:
            case Army:
            case Betrayer:
            case Burglar:
            case Burning:
            case Charred:
            case Chill:
            case Dancing:
            case Daring:
            case Dark:
            case Dawn:
            case Diabolic:
            case Dusk:
            case Elite:
            case Fiendish:
            case Fiery:
            case Focus:
            case Frigid:
            case Frozen:
            case Gladiator:
            case Gloom:
            case Glory:
            case Honest:
            case Icy:
            case Infernal:
            case Light:
            case Marauder:
            case Misery:
            case Moon:
            case Mountebank:
            case Nile:
            case Noble:
            case Pickpocket:
            case Pirate:
            case Proud:
            case Pure:
            case Searing:
            case Raging:
            case Ranger:
            case Samurai:
            case Sorrow:
            case Sun:
            case Thief:
            case Twilight:
            case Valiant:
            case Wicked:
            case Winter:
            case Woe:
            {
                return named_type::ToString(NAMED_TYPE);
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(NAMED_TYPE, "Name");
            }
        }
    }

    const ElementEnumVec_t
        named_type::ElementTypes(const named_type::Enum NAMED_TYPE, const bool WILL_INCLUDE_NONE)
    {
        ElementEnumVec_t elementTypes;

        if (NAMED_TYPE == named_type::Wicked)
        {
            elementTypes = { element_type::Fire, element_type::Shadow };
        }
        else if (NAMED_TYPE == named_type::Infernal)
        {
            elementTypes
                = element_type::ValidCombinations(element_type::Fire | element_type::Shadow);
        }
        else if (
            (NAMED_TYPE == named_type::Light) || (NAMED_TYPE == named_type::Dawn)
            || (NAMED_TYPE == named_type::Sun))
        {
            elementTypes
                = element_type::ValidCombinations(element_type::Fire | element_type::Honor);
        }
        else if (NAMED_TYPE == named_type::Dancing)
        {
            elementTypes = { element_type::Fire, element_type::Frost };
        }
        else if (NAMED_TYPE == named_type::Princes)
        {
            elementTypes = { element_type::Honor, element_type::Shadow };
        }
        else if (
            (NAMED_TYPE == named_type::Searing) || (NAMED_TYPE == named_type::Burning)
            || (NAMED_TYPE == named_type::Fiery) || (NAMED_TYPE == named_type::Charred))
        {
            elementTypes = { element_type::Fire };
        }
        else if (
            (NAMED_TYPE == named_type::Icy) || (NAMED_TYPE == named_type::Winter)
            || (NAMED_TYPE == named_type::Frigid) || (NAMED_TYPE == named_type::Chill)
            || (NAMED_TYPE == named_type::Frozen) || (NAMED_TYPE == named_type::Arctic))
        {
            elementTypes = { element_type::Frost };
        }
        else if (
            (NAMED_TYPE == named_type::Honest) || (NAMED_TYPE == named_type::Noble)
            || (NAMED_TYPE == named_type::Daring) || (NAMED_TYPE == named_type::Elite)
            || (NAMED_TYPE == named_type::Valiant) || (NAMED_TYPE == named_type::Heros)
            || (NAMED_TYPE == named_type::Gladiator) || (NAMED_TYPE == named_type::Proud)
            || (NAMED_TYPE == named_type::Glory) || (NAMED_TYPE == named_type::Pure)
            || (NAMED_TYPE == named_type::Samurai) || (NAMED_TYPE == named_type::Thors))
        {
            elementTypes = { element_type::Honor };
        }
        else if (
            (NAMED_TYPE == named_type::Diabolic) || (NAMED_TYPE == named_type::Fiendish)
            || (NAMED_TYPE == named_type::Nile) || (NAMED_TYPE == named_type::Gloom)
            || (NAMED_TYPE == named_type::Twilight) || (NAMED_TYPE == named_type::Dusk)
            || (NAMED_TYPE == named_type::Sorrow) || (NAMED_TYPE == named_type::Woe)
            || (NAMED_TYPE == named_type::Misery) || (NAMED_TYPE == named_type::Moon)
            || (NAMED_TYPE == named_type::Dark) || (NAMED_TYPE == named_type::Betrayer)
            || (NAMED_TYPE == named_type::Burglar) || (NAMED_TYPE == named_type::Mountebank)
            || (NAMED_TYPE == named_type::Charlatans))
        {
            elementTypes = { element_type::Shadow };
        }

        if (WILL_INCLUDE_NONE)
        {
            elementTypes.emplace_back(element_type::None);
        }

        return elementTypes;
    }

    const std::string material::ToString(const item::material::Enum MATERIAL)
    {
        switch (MATERIAL)
        {
            case Nothing:
            {
                return "Nothing";
            }
            case Wood:
            {
                return "Wood";
            }
            case Leather:
            {
                return "Leather";
            }
            case Silk:
            {
                return "Silk";
            }
            case Bone:
            {
                return "Bone";
            }
            case Water:
            {
                return "Water";
            }
            case Dirt:
            {
                return "Dirt";
            }
            case Acid:
            {
                return "Acid";
            }
            case Stone:
            {
                return "Stone";
            }
            case Plant:
            {
                return "Plant";
            }
            case DriedFlesh:
            {
                return "DriedFlesh";
            }
            case Hide:
            {
                return "Hide";
            }
            case Feather:
            {
                return "Feather";
            }
            case Fur:
            {
                return "Fur";
            }
            case Hair:
            {
                return "Hair";
            }
            case Scales:
            {
                return "Scales";
            }
            case Obsidian:
            {
                return "Obsidian";
            }
            case Jade:
            {
                return "Jade";
            }
            case Amethyst:
            {
                return "Amethyst";
            }
            case Emerald:
            {
                return "Emerald";
            }
            case Pearl:
            {
                return "Pearl";
            }
            case Ruby:
            {
                return "Ruby";
            }
            case Lazuli:
            {
                return "Lazuli";
            }
            case Sapphire:
            {
                return "Sapphire";
            }
            case Diamond:
            {
                return "Diamond";
            }
            case Glass:
            {
                return "Glass";
            }
            case Gas:
            {
                return "Gas";
            }
            case Blood:
            {
                return "Blood";
            }
            case Paper:
            {
                return "Paper";
            }
            case Rope:
            {
                return "Rope";
            }
            case Cloth:
            {
                return "Cloth";
            }
            case Claw:
            {
                return "Claw";
            }
            case Horn:
            {
                return "Horn";
            }
            case Tooth:
            {
                return "Tooth";
            }
            case Spirit:
            {
                return "Spirit";
            }
            case Tin:
            {
                return "Tin";
            }
            case Gold:
            {
                return "Gold";
            }
            case Iron:
            {
                return "Iron";
            }
            case Bronze:
            {
                return "Bronze";
            }
            case Silver:
            {
                return "Silver";
            }
            case Steel:
            {
                return "Steel";
            }
            case Platinum:
            {
                return "Platinum";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MATERIAL, "ToString");
            }
        }
    }

    const std::string material::Name(const item::material::Enum MATERIAL)
    {
        if (MATERIAL == material::DriedFlesh)
        {
            return "Dried Flesh";
        }
        else
        {
            return ToString(MATERIAL);
        }
    }

    Armor_t material::ArmorRatingBonus(
        const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return ArmorRatingBonusPri(MATERIAL_PRI) + ArmorRatingBonusSec(MATERIAL_SEC);
    }

    Armor_t material::ArmorRatingBonusPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            case Spirit:
            case Gas:
            case Water:
            case Blood:
            case Acid:
            case Dirt:
            case Paper:
            case Glass:
            case Feather:
            case Hair:
            case Rope:
            {
                return 0_armor;
            }
            case Cloth:
            {
                return 1_armor;
            }
            case Silk:
            {
                return 2_armor;
            }
            case Hide:
            {
                return 4_armor;
            }
            case DriedFlesh:
            {
                return 5_armor;
            }
            case Leather:
            {
                return 7_armor;
            }
            case Fur:
            {
                return 9_armor;
            }
            case Plant:
            {
                return 11_armor;
            }
            case Claw:
            case Horn:
            {
                return 12_armor;
            }
            case Bone:
            case Tooth:
            {
                return 13_armor;
            }
            case Wood:
            {
                return 15_armor;
            }
            case Scales:
            {
                return 18_armor;
            }
            case Stone:
            {
                return 20_armor;
            }
            case Obsidian:
            {
                return 21_armor;
            }
            case Tin:
            {
                return 25_armor;
            }
            case Bronze:
            {
                return 26_armor;
            }
            case Iron:
            {
                return 27_armor;
            }
            case Steel:
            {
                return 28_armor;
            }
            case Jade:
            {
                return 29_armor;
            }
            case Amethyst:
            {
                return 30_armor;
            }
            case Emerald:
            {
                return 31_armor;
            }
            case Pearl:
            {
                return 32_armor;
            }
            case Ruby:
            {
                return 33_armor;
            }
            case Lazuli:
            {
                return 34_armor;
            }
            case Silver:
            {
                return 35_armor;
            }
            case Sapphire:
            {
                return 36_armor;
            }
            case Gold:
            {
                return 40_armor;
            }
            case Platinum:
            {
                return 45_armor;
            }
            case Diamond:
            {
                return 50_armor;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MATERIAL_PRI, "ArmorRatingBonusPri");
            }
        }
    }

    Armor_t material::ArmorRatingBonusSec(const material::Enum MATERIAL_SEC)
    {
        auto const SEC_MATERIAL_ARMOR_ADJ_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-item-secondary-material-armor-adj-ratio") };

        return Armor_t::Make(
            ArmorRatingBonusPri(MATERIAL_SEC).As<float>() * SEC_MATERIAL_ARMOR_ADJ_RATIO);
    }

    Coin_t material::PriceAdj(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return PriceAdjPri(MATERIAL_PRI) + PriceAdjSec(MATERIAL_SEC);
    }

    Coin_t material::PriceAdjPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            {
                return 0_coin;
            }
            case Spirit:
            {
                return 0_coin;
            }
            case Gas:
            {
                return 0_coin;
            }
            case Water:
            {
                return 0_coin;
            }
            case Blood:
            {
                return 0_coin;
            }
            case Acid:
            {
                return 100_coin;
            }
            case Dirt:
            {
                return 0_coin;
            }
            case Paper:
            {
                return 0_coin;
            }
            case Glass:
            {
                return 10_coin;
            }
            case Feather:
            {
                return 0_coin;
            }
            case Fur:
            {
                return 8_coin;
            }
            case Hair:
            {
                return 0_coin;
            }
            case DriedFlesh:
            {
                return 4_coin;
            }
            case Rope:
            {
                return 0_coin;
            }
            case Cloth:
            {
                return 0_coin;
            }
            case Silk:
            {
                return 16_coin;
            }
            case Hide:
            {
                return 7_coin;
            }
            case Leather:
            {
                return 10_coin;
            }
            case Plant:
            {
                return 0_coin;
            }
            case Claw:
            {
                return 4_coin;
            }
            case Scales:
            {
                return 1000_coin;
            }
            case Horn:
            {
                return 6_coin;
            }
            case Bone:
            {
                return 12_coin;
            }
            case Tooth:
            {
                return 5_coin;
            }
            case Wood:
            {
                return 0_coin;
            }
            case Stone:
            {
                return 0_coin;
            }
            case Obsidian:
            {
                return 20_coin;
            }
            case Tin:
            {
                return 30_coin;
            }
            case Bronze:
            {
                return 32_coin;
            }
            case Iron:
            {
                return 36_coin;
            }
            case Steel:
            {
                return 40_coin;
            }
            case Jade:
            {
                return 100_coin;
            }
            case Amethyst:
            {
                return 200_coin;
            }
            case Emerald:
            {
                return 300_coin;
            }
            case Pearl:
            {
                return 500_coin;
            }
            case Ruby:
            {
                return 600_coin;
            }
            case Lazuli:
            {
                return 700_coin;
            }
            case Silver:
            {
                return 1000_coin;
            }
            case Sapphire:
            {
                return 2000_coin;
            }
            case Gold:
            {
                return 5000_coin;
            }
            case Platinum:
            {
                return 7500_coin;
            }
            case Diamond:
            {
                return 10000_coin;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MATERIAL_PRI, "PriceAdjPri");
            }
        }
    }

    Coin_t material::PriceAdjSec(const material::Enum MATERIAL)
    {
        return PriceAdjPri(MATERIAL) / 10_coin;
    }

    float material::WeightMult(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_PRI) + WeightMultSec(MATERIAL_SEC);
    }

    float material::WeightMultPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing:
            {
                return 0.0f;
            }
            case Spirit:
            {
                return 0.0f;
            }
            case Gas:
            {
                return 0.001f;
            }
            case Plant:
            {
                return 0.9f;
            }
            case Cloth:
            {
                return 0.9f;
            }
            case Silk:
            {
                return 0.8f;
            }
            case Leather:
            {
                return 1.05f;
            }
            case Rope:
            {
                return 1.1f;
            }
            case Bone:
            {
                return 1.15f;
            }
            case Paper:
            {
                return 1.2f;
            }
            case Claw:
            {
                return 1.2f;
            }
            case Horn:
            {
                return 1.2f;
            }
            case Tooth:
            {
                return 1.2f;
            }
            case Water:
            {
                return 1.5f;
            }
            case Acid:
            {
                return 1.5f;
            }
            case Blood:
            {
                return 1.55f;
            }
            case DriedFlesh:
            {
                return 1.6f;
            }
            case Hide:
            {
                return 1.35f;
            }
            case Feather:
            {
                return 0.5f;
            }
            case Fur:
            {
                return 0.75f;
            }
            case Hair:
            {
                return 0.5f;
            }
            case Scales:
            {
                return 0.9f;
            }
            case Glass:
            {
                return 1.6f;
            }
            case Wood:
            {
                return 1.85f;
            }
            case Dirt:
            {
                return 1.9f;
            }
            case Obsidian:
            {
                return 2.2f;
            }
            case Jade:
            {
                return 2.1f;
            }
            case Amethyst:
            {
                return 2.1f;
            }
            case Emerald:
            {
                return 2.15f;
            }
            case Pearl:
            {
                return 1.2f;
            }
            case Ruby:
            {
                return 2.2f;
            }
            case Lazuli:
            {
                return 2.5f;
            }
            case Sapphire:
            {
                return 2.1f;
            }
            case Diamond:
            {
                return 2.6f;
            }
            case Stone:
            {
                return 3.0f;
            }
            case Tin:
            {
                return 4.0f;
            } // 7.3 - density
            case Bronze:
            {
                return 4.2f;
            } // 7.5
            case Iron:
            {
                return 4.6f;
            } // 7.8
            case Steel:
            {
                return 4.8f;
            } // 8.0
            case Silver:
            {
                return 7.3f;
            } // 10.5
            case Gold:
            {
                return 16.6f;
            } // 19.3
            case Platinum:
            {
                return 18.8f;
            } // 21.5
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(MATERIAL_PRI, "WeightMultPri");
            }
        }
    }

    float material::WeightMultSec(const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_SEC) / 10.0f;
    }

    int material::EnchantmentBonus(
        const material::Enum MATERIAL_PRIMARY, const material::Enum MATERIAL_SECONDARY)
    {
        // the enchantment factory has been setup to work with values that range from [0,20]
        auto const RANGE{ 20.0f };

        // these two ratios should add up to 1.0f
        auto const PRIMARY_RATIO{ 0.75f };
        auto const SECONDARY_RATIO{ 0.25f };

        if (MATERIAL_SECONDARY == material::Nothing)
        {
            return static_cast<int>(EnchantmentBonusRatio(MATERIAL_PRIMARY) * RANGE);
        }
        else
        {
            return static_cast<int>(
                ((EnchantmentBonusRatio(MATERIAL_PRIMARY) * PRIMARY_RATIO)
                 + (EnchantmentBonusRatio(MATERIAL_SECONDARY) * SECONDARY_RATIO))
                * RANGE);
        }
    }

    float material::EnchantmentBonusRatio(const material::Enum MATERIAL)
    {
        // clang-format off
        switch (MATERIAL)
        {
            // this is the standard set that runs the full range of values
            case Glass:                    { return 0.1f;  }
            case DriedFlesh:               { return 0.5f;  }
            case Blood:                    { return 0.85f; }
            case Spirit:                   { return 1.25f; }//extra bonus for spirit is intentional

            // this is the "tribal" set that should range from about 5% to 50%
            case Leather:                  { return 0.05f;  }
            case Stone:                    { return 0.1f;   }
            case Fur:                      { return 0.136f; }
            case Horn:                     { return 0.172f; }
            case Hide:                     { return 0.23f;  }
            case Silk:                     { return 0.262f; }
            case Claw:                     { return 0.28f;  }
            case Tooth:                    { return 0.33f;  }
            case Scales:                   { return 0.38f;  }
            case Bone:                     { return 0.45f;  }
            case Obsidian:                 { return 0.5f;   }

            // the jewel set should start where tribal left off (50%) and reach 100% with diamond
            case Jade:                     { return 0.5f;  }
            case Amethyst:                 { return 0.55f; }
            case Emerald:                  { return 0.6f;  }
            case Lazuli:                   { return 0.68f; }
            case Pearl:                    { return 0.76f; }
            case Ruby:                     { return 0.84f; }
            case Sapphire:                 { return 0.92f; }
            case Diamond:                  { return 1.0f;  }

            // the metal set varies wildly, but should stop short of diamond's value
            case Tin:                      { return 0.05f;  }
            case Iron:                     { return 0.09f;  }
            case Steel:                    { return 0.11f;  }
            case Bronze:                   { return 0.333f; }
            case Silver:                   { return 0.666f; }
            case Gold:                     { return 0.8f;   }
            case Platinum:                 { return 0.85f;  }

            // all others are 0%
            case Nothing:
            case Feather:
            case Gas:
            case Water:
            case Dirt:
            case Acid:
            case Paper:
            case Hair:
            case Rope:
            case Cloth:
            case Plant:
            case Wood:
            case Count:
            default:
            {
                return 0.0f;
            }
        }
        // clang-format on
    }

    // TODO move to MaterialFactory
    const MaterialPair_t material::SkinMaterial(const creature::race::Enum RACE)
    {
        // keep in sync with ItemImageLoader::GetSkinImageFilename()

        if ((RACE == creature::race::Wolfen) || (RACE == creature::race::Troll)
            || (RACE == creature::race::Boar) || (RACE == creature::race::LionBoar)
            || (RACE == creature::race::Ramonaut) || (RACE == creature::race::Wereboar))
        {
            return std::make_pair(material::Hide, material::Nothing);
        }
        else if (
            (RACE == creature::race::ThreeHeadedHound) || (RACE == creature::race::Minotaur)
            || (RACE == creature::race::Lion) || (RACE == creature::race::Werebear)
            || (RACE == creature::race::Werewolf))
        {
            return std::make_pair(material::Hide, material::Fur);
        }
        else if (
            (RACE == creature::race::Dragon) || (RACE == creature::race::Hydra)
            || (RACE == creature::race::LizardWalker) || (RACE == creature::race::Naga)
            || (RACE == creature::race::Serpent) || (RACE == creature::race::Cobra)
            || (RACE == creature::race::Wyvern))
        {
            return std::make_pair(material::Scales, material::Nothing);
        }
        else if (RACE == creature::race::Plant)
        {
            return std::make_pair(material::Plant, material::Nothing);
        }
        else
        {
            return std::make_pair(material::Nothing, material::Nothing);
        }
    }

    bool material::IsSolid(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL != material::Nothing) && (IsLiquid(MATERIAL) == false)
            && (IsGas(MATERIAL) == false) && (IsSpirit(MATERIAL) == false));
    }

    bool material::IsLiquid(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Acid) || (MATERIAL == material::Blood)
            || (MATERIAL == material::Water));
    }

    bool material::IsGas(const material::Enum MATERIAL) { return (MATERIAL == material::Gas); }

    bool material::IsSpirit(const material::Enum MATERIAL)
    {
        return (MATERIAL == material::Spirit);
    }

    bool material::IsBendy(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Leather) || (MATERIAL == material::Silk)
            || (MATERIAL == material::Dirt) || (MATERIAL == material::Plant)
            || (MATERIAL == material::DriedFlesh) || (MATERIAL == material::Hide)
            || (MATERIAL == material::Feather) || (MATERIAL == material::Fur)
            || (MATERIAL == material::Hair) || (MATERIAL == material::Paper)
            || (MATERIAL == material::Rope) || (MATERIAL == material::Cloth));
    }

    bool material::IsRigid(const material::Enum MATERIAL)
    {
        return (IsSolid(MATERIAL) && (IsBendy(MATERIAL) == false));
    }

    bool material::IsBloody(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Blood) || (SEC == material::Blood));
    }

    float material::FireDamageRatio(const material::Enum PRI, const material::Enum SEC)
    {
        float fireDamageRatio(0.0f);
        switch (PRI)
        {
            case material::Nothing:
            {
                break;
            }
            case material::Wood:
            {
                fireDamageRatio = 0.5f;
                break;
            }
            case material::Leather:
            {
                fireDamageRatio = 0.6f;
                break;
            }
            case material::Silk:
            {
                fireDamageRatio = 0.9f;
                break;
            }
            case material::Bone:
            {
                fireDamageRatio = 0.2f;
                break;
            }
            case material::Water:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Dirt:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Acid:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Stone:
            {
                fireDamageRatio = 0.001f;
                break;
            }
            case material::Plant:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::DriedFlesh:
            {
                fireDamageRatio = 0.75f;
                break;
            }
            case material::Hide:
            {
                fireDamageRatio = 0.87f;
                break;
            }
            case material::Feather:
            {
                fireDamageRatio = 1.0f;
                break;
            }
            case material::Fur:
            {
                fireDamageRatio = 1.0f;
                break;
            }
            case material::Hair:
            {
                fireDamageRatio = 2.0f;
                break;
            }
            case material::Scales:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Obsidian:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Jade:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Amethyst:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Emerald:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Pearl:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Ruby:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Lazuli:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Sapphire:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Diamond:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Glass:
            {
                fireDamageRatio = 0.9f;
                break;
            }
            case material::Gas:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Blood:
            {
                fireDamageRatio = 0.2f;
                break;
            }
            case material::Paper:
            {
                fireDamageRatio = 0.99f;
                break;
            }
            case material::Rope:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::Cloth:
            {
                fireDamageRatio = 0.85f;
                break;
            }
            case material::Claw:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Horn:
            {
                fireDamageRatio = 0.1f;
                break;
            }
            case material::Tooth:
            {
                fireDamageRatio = 0.05f;
                break;
            }
            case material::Spirit:
            {
                fireDamageRatio = 0.0f;
                break;
            }
            case material::Gold:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Iron:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Tin:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Bronze:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Silver:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Steel:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Platinum:
            {
                fireDamageRatio = 0.01f;
                break;
            }
            case material::Count:
            default:
            {
                ThrowInvalidValueForFunction(PRI, "FireDamageRatio(PRI)");
            }
        }
        switch (SEC)
        {
            case material::Nothing:
            {
                break;
            }
            case material::Wood:
            {
                fireDamageRatio += 0.2f;
                break;
            }
            case material::Leather:
            {
                fireDamageRatio += 0.1f;
                break;
            }
            case material::Silk:
            {
                fireDamageRatio += 0.27f;
                break;
            }
            case material::Bone:
            {
                fireDamageRatio += 0.05f;
                break;
            }
            case material::Water:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Dirt:
            {
                fireDamageRatio -= 0.6f;
                break;
            }
            case material::Acid:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Stone:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Plant:
            {
                fireDamageRatio += 0.4f;
                break;
            }
            case material::DriedFlesh:
            {
                fireDamageRatio += 0.1f;
                break;
            }
            case material::Hide:
            {
                fireDamageRatio += 0.3f;
                break;
            }
            case material::Feather:
            {
                fireDamageRatio += 0.2f;
                break;
            }
            case material::Fur:
            {
                fireDamageRatio += 0.3f;
                break;
            }
            case material::Hair:
            {
                fireDamageRatio += 0.5f;
                break;
            }
            case material::Scales:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Obsidian:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Jade:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Amethyst:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Emerald:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Pearl:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Ruby:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Lazuli:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Sapphire:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Diamond:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Glass:
            {
                break;
            }
            case material::Gas:
            {
                break;
            }
            case material::Blood:
            {
                fireDamageRatio -= 0.75f;
                break;
            }
            case material::Paper:
            {
                fireDamageRatio += 0.5f;
                break;
            }
            case material::Rope:
            {
                fireDamageRatio += 0.25f;
                break;
            }
            case material::Cloth:
            {
                fireDamageRatio += 0.25f;
                break;
            }
            case material::Claw:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Horn:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Tooth:
            {
                fireDamageRatio -= 0.1f;
                break;
            }
            case material::Spirit:
            {
                break;
            }
            case material::Gold:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Iron:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Tin:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Bronze:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Silver:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Steel:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Platinum:
            {
                fireDamageRatio -= 0.5f;
                break;
            }
            case material::Count:
            default:
            {
                ThrowInvalidValueForFunction(SEC, "FireDamageRatio(SEC)");
            }
        }

        if (fireDamageRatio < 0.0f)
        {
            fireDamageRatio = 0.0f;
        }

        if (fireDamageRatio > 1.0f)
        {
            fireDamageRatio = 1.0f;
        }

        return fireDamageRatio;
    }

    bool material::IsMetal(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Tin) || (MATERIAL == material::Gold)
            || (MATERIAL == material::Iron) || (MATERIAL == material::Bronze)
            || (MATERIAL == material::Silver) || (MATERIAL == material::Steel)
            || (MATERIAL == material::Platinum));
    }

    bool material::IsFancyJewel(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == Amethyst) || (MATERIAL == Emerald) || (MATERIAL == Ruby)
            || (MATERIAL == Sapphire) || (MATERIAL == Diamond));
    }

    bool material::IsFancyMetal(const material::Enum MATERIAL)
    {
        return ((MATERIAL == Silver) || (MATERIAL == Gold));
    }

    bool material::IsFancyOpaque(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == material::Obsidian) || (MATERIAL == material::Pearl)
            || (MATERIAL == material::Jade) || (MATERIAL == material::Lazuli));
    }

    bool material::IsFancyTribal(const material::Enum MATERIAL)
    {
        return (
            (MATERIAL == Claw) || (MATERIAL == Bone) || (MATERIAL == Tooth)
            || (MATERIAL == Scales));
    }

    bool material::IsFancy(const Enum MATERIAL)
    {
        return (
            IsFancyJewel(MATERIAL) || IsFancyMetal(MATERIAL) || IsFancyOpaque(MATERIAL)
            || IsFancyTribal(MATERIAL));
    }

    bool material::RequiresAnPrefix(const Enum MATERIAL)
    {
        return (
            (MATERIAL == Obsidian) || (MATERIAL == Iron) || (MATERIAL == Amethyst)
            || (MATERIAL == Emerald));
    }

    void weapon_type::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::BodyPart, "BodyPart", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Sword, "Sword", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Axe, "Axe", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Whip, "Whip", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Knife, "Knife", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Club, "Club", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Staff, "Staff", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::BladedStaff, "BladedStaff", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Melee, "Melee", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Projectile, "Projectile", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Bladed, "Bladed", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, weapon_type::Pointed, "Pointed", SEPARATOR);
    }

    const std::string weapon_type::Name(const weapon_type::Enum WEAPON_TYPE)
    {
        if (WEAPON_TYPE & weapon_type::BladedStaff)
        {
            return boost::algorithm::replace_all_copy(
                weapon_type::ToString(WEAPON_TYPE), "BladedStaff", "Bladed Staff");
        }
        else
        {
            return weapon_type::ToString(WEAPON_TYPE);
        }
    }

    const std::string armor_type::ToString(const item::armor_type::Enum ARMOR_TYPE)
    {
        switch (ARMOR_TYPE)
        {
            case Not:
            {
                return "Not";
            }
            case Shield:
            {
                return "Shield";
            }
            case Helm:
            {
                return "Helm";
            }
            case Gauntlets:
            {
                return "Gauntlets";
            }
            case Pants:
            {
                return "Pants";
            }
            case Boots:
            {
                return "Boots";
            }
            case Shirt:
            {
                return "Shirt";
            }
            case Bracers:
            {
                return "Bracers";
            }
            case Aventail:
            {
                return "Aventail";
            }
            case Covering:
            {
                return "Covering";
            }
            case Skin:
            {
                return "Skin";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(ARMOR_TYPE, "ToString");
            }
        }
    }

    bool armor_type::DoesRequireBaseType(const armor_type::Enum ARMOR_TYPE)
    {
        switch (ARMOR_TYPE)
        {
            case Gauntlets:
            case Pants:
            case Boots:
            case Shirt:
            case Bracers:
            case Aventail:
            {
                return true;
            }
            case Not:
            case Shield:
            case Helm:
            case Covering:
            case Skin:
            case Count:
            default:
            {
                return false;
            }
        }
    }

    const std::string body_part::ToString(const body_part::Enum BODY_PART)
    {
        switch (BODY_PART)
        {
            case Fists:
            {
                return "Fists";
            }
            case Claws:
            {
                return "Claws";
            }
            case Tentacles:
            {
                return "Tentacles";
            }
            case Bite:
            {
                return "Bite";
            }
            case Breath:
            {
                return "Breath";
            }
            case Skin:
            {
                return "Skin";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(BODY_PART, "ToString");
            }
        }
    }

    const std::string
        name_material_type::ToString(const name_material_type::Enum NAME_MATERIAL_TYPE)
    {
        switch (NAME_MATERIAL_TYPE)
        {
            case name_material_type::Misc:
            {
                return "Misc";
            }
            case name_material_type::BodyPart:
            {
                return "BodyPart";
            }
            case name_material_type::Decoration:
            {
                return "Decoration";
            }
            case name_material_type::Handle:
            {
                return "Handle";
            }
            case name_material_type::Reinforced:
            {
                return "Reinforced";
            }
            case name_material_type::ReinforcedWithBase:
            {
                return "ReinforcedWithBase:";
            }
            case name_material_type::Tipped:
            {
                return "Tipped";
            }
            case name_material_type::Claspped:
            {
                return "Claspped";
            }
            case name_material_type::ClasppedWithBase:
            {
                return "ClasppedWithBase";
            }
            case name_material_type::Count:
            default:
            {
                ThrowInvalidValueForFunction(NAME_MATERIAL_TYPE, "ToString");
            }
        }
    }

} // namespace item
} // namespace heroespath
