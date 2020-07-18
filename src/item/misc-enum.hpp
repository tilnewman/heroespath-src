// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_MISC_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_MISC_ENUM_HPP_INCLUDED
//
// misc-enum.hpp
//
#include "game/strong-types.hpp"
#include "item/category-enum.hpp"
#include "item/element-enum.hpp"
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace item
{

    // never Named, only a few are Set
    struct Misc : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            // quest items (all magical) (all musical instruments except for DrumLute)
            AngelBraid,
            Crumhorn,
            DevilHorn,
            GolemFinger,
            HurdyGurdy,
            Icicle,
            LichHand,
            Lyre,
            MummyHand,
            PetrifiedSnake,
            PipeAndTabor,
            Recorder,
            UnicornHorn,
            Viol,

            // unique items (all magical)
            BasiliskTongue,
            BerserkersBeard,
            BishopsHanky,
            BleedingTrophy,
            BloodyDragonScale,
            BottleOfBansheeScreams,
            BronzeTroll,
            BroochCrown,
            BroochFeather,
            BroochFist,
            BroochHourglass,
            BroochKey,
            BroochMask,
            BurialShroud,
            CapeCommanders,
            CapeGenerals,
            CapeKings,
            CharmCrown,
            CharmFeather,
            CharmFist,
            CharmHourglass,
            CharmKey,
            CharmMask,
            ChimeraBone,
            CobraTooth,
            CrystalChimes,
            DemonDiary,
            DoveBloodVial,
            DragonToothWhistle,
            DriedFrog,
            DriedGecko,
            DriedIguana,
            DriedLizard,
            DriedSalamander,
            DriedSkink,
            DriedToad,
            DruidLeaf,
            EvilRabbitsFoot,
            ExoticGoldenGong,
            EyeCyclops,
            EyeGiantOwl,
            EyeHawk,
            FlagFanatics,
            FlagRegalCaptains,
            FlagTribal,
            FriarsChronicle,
            FuneralRecord,
            GhostSheet,
            GlassCat,
            GriffinFeather,
            HangmansNoose,
            HobgoblinNose,
            HolyEpic,
            HornOfTheHorde,
            ImpTail,
            IslanderHeaddress,
            JeweledArmband,
            JeweledHandbag,
            JeweledPrincessVeil,
            LastRitesScroll,
            MacabreManuscript,
            MadRatJuju,
            MagicHorseshoe,
            MagnifyingGlass,
            ManaAmulet,
            MaskMourners,
            MaskRascal,
            MinotaurHide,
            MortuaryOrnament,
            NecklaceJeweledAnkh,
            NecklaceSharkTooth,
            NecklaceVampiresTooth,
            PantherPaw,
            PinCrown,
            PinFeather,
            PinFist,
            PinHourglass,
            PinKey,
            PinMask,
            PixieBell,
            RattlesnakeTail,
            RavenClaw,
            ReaperScythe,
            RequiemRegister,
            RingHobo,
            RingMendicant,
            RingMonk,
            RingPriest,
            RoyalScoutSpyglass,
            SaintCameoPin,
            SaintsJournal,
            SanguineRelic,
            ScoundrelSack,
            SepultureDecoration,
            ShadeCloak,
            ShamanRainmaker,
            SirenConch,
            SpecterChains,
            SpecterRobe,
            SpiderEggs,
            SprintersLegtie,
            SwindlersBag,
            TricksterPouch,
            TuningFork,
            TurtleShell,
            VultureGizzard,
            WarhorseMarionette,
            WarTrumpet,
            WeaselTotem,
            WolfenFur,
            WraithTalisman,

            // these misc items are always magical
            DollBlessed,
            DollCursed,
            DriedHead,
            FigurineBlessed,
            FigurineCursed,
            PuppetBlessed,
            PuppetCursed,

            // these are always magical and are used as summoning items
            Embryo,
            Egg,
            SummoningStatue,
            Seeds,

            // these  are always magical and are used to cast spells
            Orb,
            Wand,
            Scepter,
            Shard,

            // these misc items may or may not be magical
            Bust,
            Doll,
            DrumLute, // the only musical instrument that is not a quest item
            Goblet,
            Key,
            LockPicks,
            Mirror,
            Pendant,
            Ring,

            Count
        };

        static constexpr std::string_view Name(const Misc::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case AngelBraid: return "Angel Braid";
                case Crumhorn: return "Crumhorn";
                case DevilHorn: return "Devil Horn";
                case GolemFinger: return "Golem Finger";
                case HurdyGurdy: return "Hannibal's Hurdy Gurdy";
                case Icicle: return "Icicle";
                case LichHand: return "Lich Hand";
                case Lyre: return "Satyr's Lyre";
                case MummyHand: return "Mummy Hand";
                case PetrifiedSnake: return "Petrified Snake";
                case PipeAndTabor: return "Puritan's Pipe And Tabor";
                case Recorder: return "Reverend's Recorder";
                case UnicornHorn: return "Unicorn Horn";
                case Viol: return "Villain's Viol";
                case BasiliskTongue: return "Basilisk Tongue";
                case BerserkersBeard: return "Berserker's Beard";
                case BishopsHanky: return "Bishop's Hanky";
                case BleedingTrophy: return "Bleeding Trophy";
                case BloodyDragonScale: return "Bloody Dragon Scale";
                case BottleOfBansheeScreams: return "Bottle Of Banshee Screams";
                case BronzeTroll: return "Bronze Troll";
                case BroochCrown: return "Crown Brooch";
                case BroochFeather: return "Feather Brooch";
                case BroochFist: return "Fist Brooch";
                case BroochHourglass: return "Hourglass Brooch";
                case BroochKey: return "Key Brooch";
                case BroochMask: return "Mask Brooch";
                case BurialShroud: return "Burial Shroud";
                case CapeCommanders: return "Commander's Cape";
                case CapeGenerals: return "General's Cape";
                case CapeKings: return "King's Cape";
                case CharmCrown: return "Crown Charm";
                case CharmFeather: return "Feather Charm";
                case CharmFist: return "Fist Charm";
                case CharmHourglass: return "Hourglass Charm";
                case CharmKey: return "Key Charm";
                case CharmMask: return "Mask Charm";
                case ChimeraBone: return "Chimera Bone";
                case CobraTooth: return "Cobra Tooth";
                case CrystalChimes: return "Crystal CrystalChimes";
                case DemonDiary: return "Demon's Diary";
                case DoveBloodVial: return "Dove Blood Vial";
                case DragonToothWhistle: return "Dragon Tooth Whistle";
                case DriedFrog: return "Dried Frog";
                case DriedGecko: return "Dried Gecko";
                case DriedIguana: return "Dried Iguana";
                case DriedLizard: return "Dried Lizard";
                case DriedSalamander: return "Dried Salamander";
                case DriedSkink: return "Dried Skink";
                case DriedToad: return "Dried Toad";
                case DruidLeaf: return "Druid Leaf";
                case EvilRabbitsFoot: return "Evil Rabbits Foot";
                case ExoticGoldenGong: return "Exotic Golden Gong";
                case EyeCyclops: return "Cyclop's Eye";
                case EyeGiantOwl: return "Giant Owl Eye";
                case EyeHawk: return "Hawk Eye";
                case FlagFanatics: return "Fanatic's Flag";
                case FlagRegalCaptains: return "Regal Captain's Flag";
                case FlagTribal: return "Tribal Flag";
                case FriarsChronicle: return "Friar's Chronicle";
                case FuneralRecord: return "Funeral Record";
                case GhostSheet: return "Ghost Sheet";
                case GlassCat: return "Glass Cat";
                case GriffinFeather: return "Griffin Feather";
                case HangmansNoose: return "Hangman's Noose";
                case HobgoblinNose: return "Hobgoblin Nose";
                case HolyEpic: return "Holy Epic";
                case HornOfTheHorde: return "Horn of The Horde";
                case ImpTail: return "Imp Tail";
                case IslanderHeaddress: return "Islander Headdress";
                case JeweledArmband: return "Armband";
                case JeweledHandbag: return "Handbag";
                case JeweledPrincessVeil: return "Princess Veil";
                case LastRitesScroll: return "Last Rites Scroll";
                case MacabreManuscript: return "Macabre Manuscript";
                case MadRatJuju: return "Mad Rat Juju";
                case MagicHorseshoe: return "Magic Horseshoe";
                case MagnifyingGlass: return "Magnifying Glass";
                case ManaAmulet: return "Mana Amulet";
                case MaskMourners: return "Mourner's Mask";
                case MaskRascal: return "Rascal Mask";
                case MinotaurHide: return "Minotaur Hide";
                case MortuaryOrnament: return "Mortuary Ornament";
                case NecklaceJeweledAnkh: return "Ankh Necklace";
                case NecklaceSharkTooth: return "Shark Tooth Necklace";
                case NecklaceVampiresTooth: return "Vampire's Tooth Necklace";
                case PantherPaw: return "Panther Paw";
                case PinCrown: return "Crown Pin";
                case PinFeather: return "Feather Pin";
                case PinFist: return "Fist Pin";
                case PinHourglass: return "Hourglass Pin";
                case PinKey: return "Key Pin";
                case PinMask: return "Mask Pin";
                case PixieBell: return "Pixie Bell";
                case RattlesnakeTail: return "Rattlesnake Tail";
                case RavenClaw: return "Raven Claw";
                case ReaperScythe: return "Reaper Scythe";
                case RequiemRegister: return "Requiem Register";
                case RingHobo: return "Hobo Ring";
                case RingMendicant: return "Mendicant Ring";
                case RingMonk: return "Monk Ring";
                case RingPriest: return "PriestRing";
                case RoyalScoutSpyglass: return "Royal Scout Spyglass";
                case SaintCameoPin: return "Saint Cameo Pin";
                case SaintsJournal: return "Saint's Journal";
                case SanguineRelic: return "Sanguine Relic";
                case ScoundrelSack: return "Scoundrel Sack";
                case SepultureDecoration: return "Sepulture Decoration";
                case ShadeCloak: return "Shade Cloak";
                case ShamanRainmaker: return "Shaman Rainmaker";
                case SirenConch: return "Siren Conch";
                case SpecterChains: return "Specter Chains";
                case SpecterRobe: return "Specter Robe";
                case SpiderEggs: return "Spider Eggs";
                case SprintersLegtie: return "Sprinter's Legtie";
                case SwindlersBag: return "Swindler's Bag";
                case TricksterPouch: return "Trickster Pouch";
                case TuningFork: return "Tuning Fork";
                case TurtleShell: return "Turtle Shell";
                case VultureGizzard: return "Vulture Gizzard";
                case WarhorseMarionette: return "Warhorse Marionette";
                case WarTrumpet: return "War Trumpet";
                case WeaselTotem: return "Weasel Totem";
                case WolfenFur: return "Wolfen Fur";
                case WraithTalisman: return "Wraith Talisman";
                case Bust: return "Bust";
                case Doll: return "Doll";
                case DollBlessed: return "Doll";
                case DollCursed: return "Doll";
                case DriedHead: return "Dried Head";
                case DrumLute: return "Drumlute";
                case Egg: return "Egg";
                case Embryo: return "Embryo";
                case FigurineBlessed: return "Figurine";
                case FigurineCursed: return "Figurine";
                case Goblet: return "Goblet";
                case Key: return "Key";
                case LockPicks: return "Lockpicks";
                case Mirror: return "Mirror";
                case Orb: return "Orb";
                case Pendant: return "Pendant";
                case PuppetBlessed: return "Puppet";
                case PuppetCursed: return "Puppet";
                case Ring: return "Ring";
                case Scepter: return "Scepter";
                case Seeds: return "Seeds";
                case Shard: return "Shard";
                case SummoningStatue: return "Summoning Statue";
                case Wand: return "Wand";
                case Count: return "item::Misc::Name(COUNT)";
                default: return "item::Misc::Name(ENUM=out_of_range)";
            }
        }

        static constexpr bool IsBodyPart(const Misc::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case SpiderEggs:
                case TurtleShell:
                case VultureGizzard:
                case WolfenFur:
                case DriedHead:
                case Egg:
                case Embryo:
                case RattlesnakeTail:
                case RavenClaw:
                case PantherPaw:
                case GriffinFeather:
                case HobgoblinNose:
                case ImpTail:
                case EvilRabbitsFoot:
                case EyeCyclops:
                case EyeGiantOwl:
                case EyeHawk:
                case DragonToothWhistle:
                case DriedFrog:
                case DriedGecko:
                case DriedIguana:
                case DriedLizard:
                case DriedSalamander:
                case DriedSkink:
                case DriedToad:
                case ChimeraBone:
                case CobraTooth:
                case BloodyDragonScale:
                case BasiliskTongue:
                case BerserkersBeard:
                case UnicornHorn:
                case MummyHand:
                case PetrifiedSnake:
                case LichHand:
                case DevilHorn:
                case GolemFinger:
                case AngelBraid: return true;

                case Crumhorn:
                case HurdyGurdy:
                case Icicle:
                case Lyre:
                case PipeAndTabor:
                case Recorder:
                case Viol:
                case BishopsHanky:
                case BleedingTrophy:
                case BottleOfBansheeScreams:
                case BronzeTroll:
                case BroochCrown:
                case BroochFeather:
                case BroochFist:
                case BroochHourglass:
                case BroochKey:
                case BroochMask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case CharmCrown:
                case CharmFeather:
                case CharmFist:
                case CharmHourglass:
                case CharmKey:
                case CharmMask:
                case CrystalChimes:
                case DemonDiary:
                case DoveBloodVial:
                case DruidLeaf:
                case ExoticGoldenGong:
                case FlagFanatics:
                case FlagRegalCaptains:
                case FlagTribal:
                case FriarsChronicle:
                case FuneralRecord:
                case GhostSheet:
                case GlassCat:
                case HangmansNoose:
                case HolyEpic:
                case HornOfTheHorde:
                case IslanderHeaddress:
                case JeweledArmband:
                case JeweledHandbag:
                case JeweledPrincessVeil:
                case LastRitesScroll:
                case MacabreManuscript:
                case MadRatJuju:
                case MagicHorseshoe:
                case MagnifyingGlass:
                case ManaAmulet:
                case MaskMourners:
                case MaskRascal:
                case MinotaurHide:
                case MortuaryOrnament:
                case NecklaceJeweledAnkh:
                case NecklaceSharkTooth:
                case NecklaceVampiresTooth:
                case PinCrown:
                case PinFeather:
                case PinFist:
                case PinHourglass:
                case PinKey:
                case PinMask:
                case PixieBell:
                case ReaperScythe:
                case RequiemRegister:
                case RingHobo:
                case RingMendicant:
                case RingMonk:
                case RingPriest:
                case RoyalScoutSpyglass:
                case SaintCameoPin:
                case SaintsJournal:
                case SanguineRelic:
                case ScoundrelSack:
                case SepultureDecoration:
                case ShadeCloak:
                case ShamanRainmaker:
                case SirenConch:
                case SpecterChains:
                case SpecterRobe:
                case SprintersLegtie:
                case SwindlersBag:
                case TricksterPouch:
                case TuningFork:
                case WarhorseMarionette:
                case WarTrumpet:
                case WeaselTotem:
                case WraithTalisman:
                case Bust:
                case Doll:
                case DollBlessed:
                case DollCursed:
                case DrumLute:
                case FigurineBlessed:
                case FigurineCursed:
                case Goblet:
                case Key:
                case LockPicks:
                case Mirror:
                case Orb:
                case Pendant:
                case PuppetBlessed:
                case PuppetCursed:
                case Ring:
                case Scepter:
                case Seeds:
                case Shard:
                case SummoningStatue:
                case Wand:
                case Count:
                default: return false;
            }
        }

        static constexpr bool IsUnique(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case BasiliskTongue:
                case BerserkersBeard:
                case BishopsHanky:
                case BleedingTrophy:
                case BloodyDragonScale:
                case BottleOfBansheeScreams:
                case BronzeTroll:
                case BroochCrown:
                case BroochFeather:
                case BroochFist:
                case BroochHourglass:
                case BroochKey:
                case BroochMask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case CharmCrown:
                case CharmFeather:
                case CharmFist:
                case CharmHourglass:
                case CharmKey:
                case CharmMask:
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
                case DruidLeaf:
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
                case GhostSheet:
                case GlassCat:
                case GriffinFeather:
                case HangmansNoose:
                case HobgoblinNose:
                case HolyEpic:
                case HornOfTheHorde:
                case ImpTail:
                case IslanderHeaddress:
                case JeweledArmband:
                case JeweledHandbag:
                case JeweledPrincessVeil:
                case LastRitesScroll:
                case MacabreManuscript:
                case MadRatJuju:
                case MagicHorseshoe:
                case MagnifyingGlass:
                case ManaAmulet:
                case MaskMourners:
                case MaskRascal:
                case MinotaurHide:
                case MortuaryOrnament:
                case NecklaceJeweledAnkh:
                case NecklaceSharkTooth:
                case NecklaceVampiresTooth:
                case PantherPaw:
                case PinCrown:
                case PinFeather:
                case PinFist:
                case PinHourglass:
                case PinKey:
                case PinMask:
                case PixieBell:
                case RattlesnakeTail:
                case RavenClaw:
                case ReaperScythe:
                case RequiemRegister:
                case RingHobo:
                case RingMendicant:
                case RingMonk:
                case RingPriest:
                case RoyalScoutSpyglass:
                case SaintCameoPin:
                case SaintsJournal:
                case SanguineRelic:
                case ScoundrelSack:
                case SepultureDecoration:
                case ShadeCloak:
                case ShamanRainmaker:
                case SirenConch:
                case SpecterChains:
                case SpecterRobe:
                case SpiderEggs:
                case SprintersLegtie:
                case SwindlersBag:
                case TricksterPouch:
                case TuningFork:
                case TurtleShell:
                case VultureGizzard:
                case WarhorseMarionette:
                case WarTrumpet:
                case WeaselTotem:
                case WolfenFur:
                case WraithTalisman: return true;

                case AngelBraid:
                case Crumhorn:
                case DevilHorn:
                case GolemFinger:
                case HurdyGurdy:
                case Icicle:
                case LichHand:
                case Lyre:
                case MummyHand:
                case PetrifiedSnake:
                case PipeAndTabor:
                case Recorder:
                case UnicornHorn:
                case Viol:
                case DollBlessed:
                case DollCursed:
                case DriedHead:
                case FigurineBlessed:
                case FigurineCursed:
                case PuppetBlessed:
                case PuppetCursed:
                case Embryo:
                case Egg:
                case SummoningStatue:
                case Seeds:
                case Orb:
                case Wand:
                case Scepter:
                case Shard:
                case Bust:
                case Doll:
                case DrumLute:
                case Goblet:
                case Key:
                case LockPicks:
                case Mirror:
                case Pendant:
                case Ring:
                case Count:
                default: return false;
            }
        }

        static constexpr bool IsQuest(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case AngelBraid:
                case Crumhorn:
                case DevilHorn:
                case GolemFinger:
                case HurdyGurdy:
                case Icicle:
                case LichHand:
                case Lyre:
                case MummyHand:
                case PetrifiedSnake:
                case PipeAndTabor:
                case Recorder:
                case UnicornHorn:
                case Viol: return false;

                case BasiliskTongue:
                case BerserkersBeard:
                case BishopsHanky:
                case BleedingTrophy:
                case BloodyDragonScale:
                case BottleOfBansheeScreams:
                case BronzeTroll:
                case BroochCrown:
                case BroochFeather:
                case BroochFist:
                case BroochHourglass:
                case BroochKey:
                case BroochMask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case CharmCrown:
                case CharmFeather:
                case CharmFist:
                case CharmHourglass:
                case CharmKey:
                case CharmMask:
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
                case DruidLeaf:
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
                case GhostSheet:
                case GlassCat:
                case GriffinFeather:
                case HangmansNoose:
                case HobgoblinNose:
                case HolyEpic:
                case HornOfTheHorde:
                case ImpTail:
                case IslanderHeaddress:
                case JeweledArmband:
                case JeweledHandbag:
                case JeweledPrincessVeil:
                case LastRitesScroll:
                case MacabreManuscript:
                case MadRatJuju:
                case MagicHorseshoe:
                case MagnifyingGlass:
                case ManaAmulet:
                case MaskMourners:
                case MaskRascal:
                case MinotaurHide:
                case MortuaryOrnament:
                case NecklaceJeweledAnkh:
                case NecklaceSharkTooth:
                case NecklaceVampiresTooth:
                case PantherPaw:
                case PinCrown:
                case PinFeather:
                case PinFist:
                case PinHourglass:
                case PinKey:
                case PinMask:
                case PixieBell:
                case RattlesnakeTail:
                case RavenClaw:
                case ReaperScythe:
                case RequiemRegister:
                case RingHobo:
                case RingMendicant:
                case RingMonk:
                case RingPriest:
                case RoyalScoutSpyglass:
                case SaintCameoPin:
                case SaintsJournal:
                case SanguineRelic:
                case ScoundrelSack:
                case SepultureDecoration:
                case ShadeCloak:
                case ShamanRainmaker:
                case SirenConch:
                case SpecterChains:
                case SpecterRobe:
                case SpiderEggs:
                case SprintersLegtie:
                case SwindlersBag:
                case TricksterPouch:
                case TuningFork:
                case TurtleShell:
                case VultureGizzard:
                case WarhorseMarionette:
                case WarTrumpet:
                case WeaselTotem:
                case WolfenFur:
                case WraithTalisman:
                case DollBlessed:
                case DollCursed:
                case DriedHead:
                case FigurineBlessed:
                case FigurineCursed:
                case PuppetBlessed:
                case PuppetCursed:
                case Embryo:
                case Egg:
                case SummoningStatue:
                case Seeds:
                case Orb:
                case Wand:
                case Scepter:
                case Shard:
                case Bust:
                case Doll:
                case DrumLute:
                case Goblet:
                case Key:
                case LockPicks:
                case Mirror:
                case Pendant:
                case Ring:
                case Count:
                default: return false;
            }
        }

        static constexpr bool IsAlwaysMagical(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case AngelBraid:
                case DevilHorn:
                case GolemFinger:
                case Icicle:
                case LichHand:
                case MummyHand:
                case PetrifiedSnake:
                case UnicornHorn:
                case BasiliskTongue:
                case BerserkersBeard:
                case BishopsHanky:
                case BleedingTrophy:
                case BloodyDragonScale:
                case BottleOfBansheeScreams:
                case BronzeTroll:
                case BroochCrown:
                case BroochFeather:
                case BroochFist:
                case BroochHourglass:
                case BroochKey:
                case BroochMask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case CharmCrown:
                case CharmFeather:
                case CharmFist:
                case CharmHourglass:
                case CharmKey:
                case CharmMask:
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
                case DruidLeaf:
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
                case GhostSheet:
                case GlassCat:
                case GriffinFeather:
                case HangmansNoose:
                case HobgoblinNose:
                case HolyEpic:
                case HornOfTheHorde:
                case ImpTail:
                case IslanderHeaddress:
                case JeweledArmband:
                case JeweledHandbag:
                case JeweledPrincessVeil:
                case LastRitesScroll:
                case MacabreManuscript:
                case MadRatJuju:
                case MagicHorseshoe:
                case MagnifyingGlass:
                case ManaAmulet:
                case MaskMourners:
                case MaskRascal:
                case MinotaurHide:
                case MortuaryOrnament:
                case NecklaceJeweledAnkh:
                case NecklaceSharkTooth:
                case NecklaceVampiresTooth:
                case PantherPaw:
                case PinCrown:
                case PinFeather:
                case PinFist:
                case PinHourglass:
                case PinKey:
                case PinMask:
                case PixieBell:
                case RattlesnakeTail:
                case RavenClaw:
                case ReaperScythe:
                case RequiemRegister:
                case RingHobo:
                case RingMendicant:
                case RingMonk:
                case RingPriest:
                case RoyalScoutSpyglass:
                case SaintCameoPin:
                case SaintsJournal:
                case SanguineRelic:
                case ScoundrelSack:
                case SepultureDecoration:
                case ShadeCloak:
                case ShamanRainmaker:
                case SirenConch:
                case SpecterChains:
                case SpecterRobe:
                case SpiderEggs:
                case SprintersLegtie:
                case SwindlersBag:
                case TricksterPouch:
                case TuningFork:
                case TurtleShell:
                case VultureGizzard:
                case WarhorseMarionette:
                case WarTrumpet:
                case WeaselTotem:
                case WolfenFur:
                case WraithTalisman:
                case DollBlessed:
                case DollCursed:
                case DriedHead:
                case FigurineBlessed:
                case FigurineCursed:
                case PuppetBlessed:
                case PuppetCursed:
                case SummoningStatue:
                case Wand:
                    return true;

                    // these musical instruments probably don't seem like misc items that would
                    // always be magical but they were only added to support specific magical quest
                    // items for bards, so they are always magical...wait...that means that bards
                    // can only use DrumLute's until they complete quests, and I'm not sure I like
                    // that idea....TODO maybe reconsider this...
                case Crumhorn:
                case HurdyGurdy:
                case Lyre:
                case Recorder:
                case Viol:
                case PipeAndTabor:
                    return true;

                    // I set these misc items to "not-always-magical" because it seemed likely that
                    // they could appear in the game without being magical, but then again, they
                    // were only added to support unique items, so...not sure what to do here...
                case Orb:
                case Pendant:
                case Ring:
                case Scepter:
                case Seeds:
                case Shard:
                case Bust:
                case Doll:
                case Egg:
                case Embryo:
                case Goblet:
                case Mirror: return false;

                case DrumLute:
                case Key:
                case LockPicks:
                case Count:
                default: return false;
            }
        }

        static constexpr bool IsMusicalInstrument(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DrumLute) || (ENUM == Crumhorn) || (ENUM == HurdyGurdy) || (ENUM == Lyre)
                || (ENUM == PipeAndTabor) || (ENUM == Recorder) || (ENUM == Viol));
        }

        static constexpr bool HasPixieVersion(const Enum ENUM) noexcept
        {
            return (
                (ENUM == JeweledArmband) || (ENUM == CapeCommanders) || (ENUM == CapeKings)
                || (ENUM == CapeGenerals) || (ENUM == ShadeCloak) || (ENUM == GhostSheet)
                || (ENUM == SpecterRobe) || (ENUM == BurialShroud) || IsMusicalInstrument(ENUM));
        }

        static constexpr bool MustBePixieVersionForPixiesToEquip(const Enum ENUM) noexcept
        {
            return (
                IsMusicalInstrument(ENUM) || (ENUM == JeweledArmband) || (ENUM == CapeCommanders)
                || (ENUM == CapeKings) || (ENUM == CapeGenerals) || (ENUM == ShadeCloak)
                || (ENUM == GhostSheet) || (ENUM == SpecterRobe) || (ENUM == BurialShroud) ||

                // these items must be pixie for pixies to equip, but there are no pixie
                // versions of these items so pixies can never equip them.
                (ENUM == ExoticGoldenGong) || (ENUM == IslanderHeaddress)
                || (ENUM == HornOfTheHorde) || (ENUM == WarTrumpet)
                || (ENUM == WarhorseMarionette));
        }

        static constexpr float ReligiousRatio(const Enum ENUM) noexcept
        {
            if ((ENUM == SpecterChains) || (ENUM == DoveBloodVial))
            {
                return 0.1f;
            }
            else if ((ENUM == SpecterRobe) || (ENUM == ManaAmulet))
            {
                return 0.2f;
            }
            else if (
                (ENUM == ShadeCloak) || (ENUM == SanguineRelic) || (ENUM == EvilRabbitsFoot)
                || (ENUM == BleedingTrophy) || (ENUM == WraithTalisman))
            {
                return 0.5f;
            }
            else if (ENUM == Bust)
            {
                return 0.6f;
            }
            else if (ENUM == SepultureDecoration)
            {
                return 0.65f;
            }
            else if (
                (ENUM == RingMendicant) || (ENUM == DriedHead) || (ENUM == DemonDiary)
                || (ENUM == MacabreManuscript))
            {
                return 0.75f;
            }
            else if (
                (ENUM == BurialShroud) || (ENUM == MortuaryOrnament) || (ENUM == GhostSheet)
                || (ENUM == FuneralRecord) || (ENUM == FriarsChronicle))
            {
                return 0.8f;
            }
            else if (
                (ENUM == SaintsJournal) || (ENUM == SaintCameoPin) || (ENUM == RequiemRegister)
                || (ENUM == RingMonk) || (ENUM == LastRitesScroll) || (ENUM == HolyEpic)
                || (ENUM == DruidLeaf) || (ENUM == ShamanRainmaker) || (ENUM == FigurineBlessed)
                || (ENUM == FigurineCursed) || (ENUM == DollBlessed) || (ENUM == DollCursed))
            {
                return 0.9f;
            }
            else if ((ENUM == RingPriest) || (ENUM == BishopsHanky))
            {
                return 0.99f;
            }
            else
            {
                return 0.0f;
            }
        }

        static constexpr bool HasNonFleshEyes(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Bust) || (ENUM == Doll) || (ENUM == DollBlessed) || (ENUM == DollCursed)
                || (ENUM == FigurineBlessed) || (ENUM == FigurineCursed) || (ENUM == PuppetBlessed)
                || (ENUM == PuppetCursed) || (ENUM == MadRatJuju) || (ENUM == SummoningStatue)
                || (ENUM == WraithTalisman) || (ENUM == BronzeTroll) || (ENUM == WeaselTotem)
                || (ENUM == DriedHead)); // yeah it's flesh but what the hell having special
                                         // eyes is cool (oh and it's not edible so it's okay)
        }

        static constexpr bool IsBlessed(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DollBlessed) || (ENUM == FigurineBlessed) || (ENUM == PuppetBlessed)
                || (ENUM == Bust));
        }

        static constexpr bool IsCursed(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DollCursed) || (ENUM == FigurineCursed) || (ENUM == PuppetCursed)
                || (ENUM == DriedHead));
        }

        static constexpr Weight_t Weight(const Misc::Enum ENUM) noexcept
        {
            // clang-format off
            switch (ENUM)
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
                case PinCrown:             { return 11_weight;  }
                case PinFeather:           { return 11_weight;  }
                case PinFist:              { return 11_weight;  }
                case PinHourglass:         { return 11_weight;  }
                case PinKey:               { return 11_weight;  }
                case PinMask:              { return 11_weight;  }
                case SaintCameoPin:         { return 11_weight;  }
                case NecklaceJeweledAnkh:   { return 12_weight;  }
                case NecklaceSharkTooth:    { return 12_weight;  }
                case NecklaceVampiresTooth: { return 12_weight;  }
                case BroochCrown:          { return 15_weight;  }
                case BroochFeather:        { return 15_weight;  }
                case BroochFist:           { return 15_weight;  }
                case BroochHourglass:      { return 15_weight;  }
                case BroochKey:            { return 15_weight;  }
                case BroochMask:           { return 15_weight;  }
                case CharmCrown:           { return 17_weight;  }
                case CharmFeather:         { return 17_weight;  }
                case CharmFist:            { return 17_weight;  }
                case CharmHourglass:       { return 17_weight;  }
                case CharmKey:             { return 17_weight;  }
                case CharmMask:            { return 17_weight;  }
                case Seeds:                 { return 18_weight;  }
                case Wand:                  { return 20_weight;  }
                case MummyHand:             { return 25_weight;  }
                case LichHand:              { return 25_weight;  }
                case Icicle:                { return 26_weight;  }
                case UnicornHorn:           { return 27_weight;  }
                case DevilHorn:             { return 28_weight;  }
                case Scepter:               { return 29_weight;  }//intentionally set low
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
                case BasiliskTongue:        { return 38_weight;  }
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
                case Count:
                default: return 0_weight;
            }
            // clang-format on
        }

        static constexpr bool IsUseable(const Enum ENUM) noexcept
        {
            return (
                IsSummoning(ENUM) || (ENUM == CrystalChimes) || (ENUM == DoveBloodVial)
                || (ENUM == DragonToothWhistle) || (ENUM == DriedFrog) || (ENUM == DriedGecko)
                || (ENUM == DriedIguana) || (ENUM == DriedLizard) || (ENUM == DriedSalamander)
                || (ENUM == DriedSkink) || (ENUM == DriedToad) || (ENUM == ExoticGoldenGong)
                || (ENUM == MagnifyingGlass) || (ENUM == PixieBell) || (ENUM == ShamanRainmaker)
                || (ENUM == SpecterChains) || (ENUM == VultureGizzard) || (ENUM == WarTrumpet)
                || (ENUM == WraithTalisman) || (ENUM == Key) || (ENUM == BottleOfBansheeScreams));
        }

        static constexpr bool IsSummoning(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Egg) || (ENUM == Embryo) || (ENUM == Seeds) || (ENUM == SummoningStatue)
                || ((ENUM == SpiderEggs)));
        }

        static constexpr Category::Enum CategoryAdditions(const Enum ENUM) noexcept
        {
            Category::Enum result = Category::None;

            if (IsUseable(ENUM))
            {
                result |= Category::Useable;
            }

            if (ENUM == Misc::MagnifyingGlass)
            {
                result |= Category::ShowsEnemyInfo;
            }

            switch (ENUM)
            {
                case BerserkersBeard:
                case BroochCrown:
                case BroochFeather:
                case BroochFist:
                case BroochHourglass:
                case BroochKey:
                case BroochMask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case CharmCrown:
                case CharmFeather:
                case CharmFist:
                case CharmHourglass:
                case CharmKey:
                case CharmMask:
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
                case PinCrown:
                case PinFeather:
                case PinFist:
                case PinHourglass:
                case PinKey:
                case PinMask:
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
                    return (result | Category::Equipable | Category::Wearable);
                }

                case AngelBraid:
                case DevilHorn:
                case GolemFinger:
                case Icicle:
                case LichHand:
                case MummyHand:
                case PetrifiedSnake:
                case UnicornHorn:
                case Orb:
                case Scepter:
                case Wand:
                case Shard:
                {
                    return (result | Category::Equipable | Category::OneHanded);
                }

                case Crumhorn:
                case HurdyGurdy:
                case Lyre:
                case PipeAndTabor:
                case Recorder:
                case Viol:
                case DrumLute:
                case ReaperScythe:
                {
                    return (result | Category::Equipable | Category::TwoHanded);
                }

                case BasiliskTongue:
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
                case Count:
                default: return result;
            }
        }

        static constexpr bool AllowsCasting(const Enum ENUM) noexcept
        {
            return (
                (ENUM == AngelBraid) || (ENUM == DevilHorn) || (ENUM == GolemFinger)
                || (ENUM == Icicle) || (ENUM == LichHand) || (ENUM == MummyHand)
                || (ENUM == PetrifiedSnake) || (ENUM == UnicornHorn) || (ENUM == Orb)
                || (ENUM == Scepter) || (ENUM == Wand) || (ENUM == Shard));
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum MISC_TYPE) noexcept
        {
            if ((MISC_TYPE == CapeCommanders) || (MISC_TYPE == CapeGenerals)
                || (MISC_TYPE == CapeKings) || (MISC_TYPE == SaintCameoPin))
            {
                return ElementList::Honor;
            }

            if (MISC_TYPE == JeweledPrincessVeil)
            {
                return (ElementList::Honor | ElementList::Shadow);
            }

            if ((MISC_TYPE == JeweledArmband) || (MISC_TYPE == ManaAmulet))
            {
                return ElementList::All;
            }

            return ElementList::None;
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MISC_ENUM_HPP_INCLUDED
