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
#ifndef HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
//
// item-type-enum.hpp
//  Enumerations for all of an Item's various types.
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "misc/types.hpp"
#include "stats/trait.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    struct category
    {
        enum Enum : unsigned int
        {
            None = 0,

            // useless, can't do anything or be used, all magic/enchantments fail
            Broken = 1 << 0,

            Useable = 1 << 1,
            BodyPart = 1 << 2,
            Equippable = 1 << 3,
            Wearable = 1 << 4,
            OneHanded = 1 << 5,
            TwoHanded = 1 << 6,
            ConsumedOnUse = 1 << 7,
            ShowsEnemyInfo = 1 << 8,
            Last = ShowsEnemyInfo
        };

        static const std::string ToString(const category::Enum E, const bool WILL_WRAP = false);
    };

    struct material
    {
        enum Enum
        {
            // Use Nothing as default instead of Error so that an Item's
            // materialSec_ can be Nothing.
            Nothing = 0,

            // what Ghosts and Shades are made of
            Spirit,

            Gas,
            Water,
            Blood,
            Acid,
            Dirt,
            Paper,
            Glass,
            Feather,
            Hair,
            DriedFlesh,
            Rope,

            // garment types
            Cloth,
            Silk,
            Hide, // used a the skin material for many creatures
            Leather, // similar to hide but more processed and strong
            Fur,

            Plant,
            Claw,
            Scales,
            Horn,
            Bone,
            Tooth,
            Wood,
            Stone,
            Obsidian,
            Tin, // 18 /  7.3 -modulus of rigidity / density
            Bronze, // 45 /  7.5
            Iron, // 41 /  7.8
            Steel, // 77 /  8.0
            Jade,
            Amethyst,
            Emerald,
            Pearl,
            Ruby,
            Lazuli,
            Silver, // 48 / 10.5
            Sapphire,
            Gold, // 27 / 19.3
            Platinum, // 82 / 21.5
            Diamond,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);

        static const std::pair<Enum, Enum> SkinMaterial(const creature::race::Enum);

        static Armor_t ArmorRatingBonus(const Enum MATERIAL_PRIMARY, const Enum MATERIAL_SEC);

        static Armor_t ArmorRatingBonusPri(const Enum MATERIAL_PRI);
        static Armor_t ArmorRatingBonusSec(const Enum MATERIAL_SEC);

        static Coin_t PriceAdj(const Enum MATERIAL_PRIMARY, const Enum MATERIAL_SEC);

        static Coin_t PriceAdjPri(const Enum MATERIAL_PRI);
        static Coin_t PriceAdjSec(const Enum MATERIAL_SEC);

        static float WeightMult(const Enum MATEIAL_PRIMARY, const Enum MATERIAL_SEC);

        static float WeightMultPri(const Enum MATERIAL_PRI);
        static float WeightMultSec(const Enum MATERIAL_SEC);

        static int EnchantmentBonus(
            const material::Enum MATERIAL_PRIMARY, const material::Enum MATERIAL_SECONDARY);

        static float EnchantmentBonusRatio(const Enum);

        // solid/liquid/gas/spirit are complete and mutually exclusive
        static bool IsSolid(const Enum);
        static bool IsLiquid(const Enum);
        static bool IsGas(const Enum);
        static bool IsSpirit(const Enum);

        // i.e. paper/rope/fur/leather/etc.
        static bool IsBendy(const Enum);

        // all IsSolid() and not IsBendy()
        static bool IsRigid(const Enum);

        static bool IsBloody(const Enum PRIMARY, const Enum SEC = Nothing);

        static float FireDamageRatio(const Enum PRIMARY, const Enum SEC = Nothing);

        static bool IsMetal(const Enum);

        // Amethyst/Emerald/Ruby/Sapphire/Diamond
        static bool IsFancyJewel(const Enum);

        // Silver/Gold
        static bool IsFancyMetal(const Enum);

        // Obsidian/Pearl/Jade/Lazuli
        static bool IsFancyOpaque(const Enum);

        // Claw/Bone/Tooth/Scales
        // some other tribal-like secondary materials to consider: Fur/DriedFlesh/Hide/Horn
        static bool IsFancyTribal(const Enum);

        // all of the four 'fancy' categories above are mutually exclusive, this combines them
        static bool IsFancy(const Enum);

        static bool IsFancyJewelOrJade(const Enum MATERIAL)
        {
            return (IsFancyJewel(MATERIAL) || (MATERIAL == Jade));
        }

        static bool IsGarmentType(const Enum MATERIAL)
        {
            return (
                (MATERIAL == Fur) || (MATERIAL == Cloth) || (MATERIAL == Silk) || (MATERIAL == Hide)
                || (MATERIAL == Leather));
        }

        static bool RequiresAnPrefix(const Enum);
    };

    using MaterialVec_t = std::vector<material::Enum>;
    using MaterialPair_t = std::pair<material::Enum, material::Enum>;
    using MaterialPairVec_t = std::vector<MaterialPair_t>;

    struct element_type
    {
        enum Enum : unsigned int
        {
            None = 0,
            Fire = 1 << 0,
            Frost = 1 << 1,
            Honor = 1 << 2,
            Shadow = 1 << 3,
            Last = Shadow
        };

        static bool IsValid(const Enum);

        static const std::string ToString(
            const Enum, const bool WILL_WRAP = false, const std::string & SEPARATOR = ", ");

        static const std::string Name(const Enum, const bool WILL_WRAP = false);

        template <typename T>
        static element_type::Enum Make(const T VALUE)
        {
            return static_cast<element_type::Enum>(VALUE);
        }

        template <typename T>
        static const std::vector<element_type::Enum>
            AllCombinations(const T VALUE, const bool WILL_INCLUDE_NOTHING = false)
        {
            auto const ELEMENT_TYPE{ Make(VALUE) };

            std::vector<Enum> enums;

            if (WILL_INCLUDE_NOTHING)
            {
                enums.emplace_back(None);
            }

            if (ELEMENT_TYPE == element_type::None)
            {
                return enums;
            }

            if (ELEMENT_TYPE & Fire)
            {
                enums.emplace_back(Fire);
            }

            if (ELEMENT_TYPE & Frost)
            {
                enums.emplace_back(Frost);
            }

            if (ELEMENT_TYPE & Honor)
            {
                enums.emplace_back(Honor);
            }

            if (ELEMENT_TYPE & Shadow)
            {
                enums.emplace_back(Shadow);
            }

            if ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Honor))
            {
                enums.emplace_back(static_cast<Enum>(Fire | Honor));
            }

            if ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Shadow))
            {
                enums.emplace_back(static_cast<Enum>(Fire | Shadow));
            }

            if ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Honor))
            {
                enums.emplace_back(static_cast<Enum>(Frost | Honor));
            }

            if ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Shadow))
            {
                enums.emplace_back(static_cast<Enum>(Frost | Shadow));
            }

            return enums;
        }

        static const std::vector<element_type::Enum> AllCombinations()
        {
            return AllCombinations(static_cast<element_type::Enum>(
                element_type::Fire | element_type::Frost | element_type::Honor
                | element_type::Shadow));
        }

        static int ValidTypeCounter(const Enum);
    };

    using ElementEnumVec_t = std::vector<element_type::Enum>;

    struct misc_type
    {
        enum Enum
        {
            NotMisc = 0,

            // quest items (all musical instruments except for the DrumLute are quest items)
            Angel_Braid,
            Crumhorn,
            Devil_Horn,
            Golem_Finger,
            Hurdy_Gurdy,
            Icicle,
            Litch_Hand,
            Lyre,
            Mummy_Hand,
            Petrified_Snake,
            Pipe_And_Tabor,
            Recorder,
            Unicorn_Horn,
            Viol,

            // exclusively unique items
            Amulet, // ManaAmulet
            Ankh_Necklace, // JeweledAnkhNeclace
            Armband, // JeweledArmband
            Bag, // ScoundrelSack, SwindlersBag, TricksterPouch
            Beard, // BerserkersBeard
            Bell, // PixieBell
            Bird_Claw, // RavenClaw
            Bone, // ChimeraBone
            Bone_Whistle, // DragonToothWhistle
            Book, // FriarsChronicle, HolyEpic, MacabreManuscript, RequiemRegister, SaintsJournal
            Bracelet_Crown,
            Bracelet_Feather,
            Bracelet_Fist,
            Bracelet_Hourglass,
            Bracelet_Key,
            Bracelet_Mask,
            Brooch_Crown,
            Brooch_Feather,
            Brooch_Fist,
            Brooch_Hourglass,
            Brooch_Key,
            Brooch_Mask,
            Charm_Crown,
            Charm_Feather,
            Charm_Fist,
            Charm_Hourglass,
            Charm_Key,
            Charm_Mask,
            CameoPin, // SaintCameoPin
            Cape, // GeneralsCape, KingsCape, CommandersCape
            Cat, // GlassCat
            Chains, // SpecterChains
            Chimes, // CrystalChimes
            Cloak, // ShadeCloak
            Conch, // SirenConch
            Eye, // CyclopsEye, GiantOwlEye, HawkEye
            Feather, // GriffinFeather
            Flag, // FanaticsFlag, RegalCaptainsFlag, TribalFlag
            Frog, // DriedFrog, DriedToad
            Gecko, // DriedGecko
            Ghost_Sheet,
            Gizzard, // VultureGizzard
            Gong, // ExoticGoldenGong
            Grave_Ornament, // SepultureDecoration, MortuaryOrnament
            Handbag, // JeweledHandbag
            Hanky, // BishopsHanky
            Headdress, // IslanderHeaddress
            Hide, // MinotaurHide  ***this is NOT used for skin materials
            Horn, // HornOfTheHorde, WarTrumpet
            Horseshoe, // MagicHorseshoe
            Iguana, // DriedIguana
            Imp_Tail,
            Leaf, // DruidLeaf
            Legtie, // SprintersLegtie
            Lizard, // DriedLizard
            Magnifying_Glass,
            Mask, // RascalMask, MournersMask
            Necklace, // JeweledAnkhNecklace, SharkToothNecklace, VampiresToothNecklace
            Noose, // HangmansNoose  *** TODO make this effect when worn not held...sweet...
            Nose, // HobgoblinNose
            Paw, // PantherPaw
            Pin_Crown,
            Pin_Feather,
            Pin_Fist,
            Pin_Hourglass,
            Pin_Key,
            Pin_Mask,
            Potion, // DoveBloodVial, BottleOfBansheeScreams
            Rabbit_Foot, // EvilRabbitsFoot
            Rainmaker, // ShamanRainmaker
            Rat_Juju, // MadRatJuju
            Rattlesnake_Tail,
            Relic, // SanguineRelic
            Ring, // HoboRing, MendicantRing, MonkRing, PriestRing
            Robe, // SpecterRobe
            Salamander, // DriedSalamander
            Scales, // BloodyDragonScale
            Scroll, // FuneralRecord, LastRitesScroll
            Scythe, // ReaperScythe
            Shroud, // BurialShroud
            Signet_Crown,
            Signet_Feather,
            Signet_Fist,
            Signet_Hourglass,
            Signet_Key,
            Signet_Mask,
            Skink, // DriedSkink
            Shark_Tooth_Necklace, // SharkToothNecklace
            Spyglass, // RoyalScoutSpyglass
            Talisman, // WraithTalisman
            Tome, // DemonDiary
            Tongue, // BasiliskTonge
            Tooth, // CobraTooth
            Tooth_Necklace, // SharkToothNecklace, VampiresToothNecklace
            Troll_Figure, // BronzeTroll
            Trophy, // BleedingTrophy,
            Tuning_Fork,
            Turtle_Shell,
            Veil, // JeweledPrincessVeil
            Warhorse_Marionette,
            Weasel_Totem,
            Wolfen_Fur,

            // standalone items
            Bust,
            Doll,
            Doll_Blessed,
            Doll_Cursed,
            Dried_Head,
            DrumLute, // the only musical instrument that is not a quest item
            Embryo,
            Egg,
            Figurine_Blessed,
            Figurine_Cursed,
            Goblet,
            Key,
            LockPicks,
            Mirror,
            Orb,
            Pendant,
            Puppet_Blessed,
            Puppet_Cursed,
            // Ring  -both a standalone item and a unique item, see Ring above
            Scepter,
            Seeds,
            Shard,
            Spider_Eggs,
            Staff,
            Summoning_Statue,
            Wand,

            // TODO these need to be implemented as weapon_type
            Fingerclaw,

            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static bool IsMusicalInstrument(const Enum);

        // summoning works by "using" the item
        static bool IsSummoning(const Enum);

        static bool IsStandalone(const Enum);
        static bool IsOrdinary(const Enum);
        static bool HasPixieVersion(const Enum);
        static bool HasOnlyPixieVersion(const Enum);
        static float ReligiousRatio(const Enum);
        static bool HasNonFleshEyes(const Enum);
        static bool IsBlessed(const Enum);
        static bool IsCursed(const Enum);
        static Weight_t Weight(const Enum);
        static bool IsUseable(const Enum);

        // these two are not mutually exclusive, all wearable are equippable because creature
        // inventories only understand equipped vs held, wearable is just a way to display more
        // natural language to the player, so all wearable are equippable but not all equippable are
        // wearable.
        static bool IsWearable(const Enum);
        static bool IsEquippable(const Enum);

        static bool IsQuestItem(const Enum);
        static bool AllowsCasting(const Enum);
        static bool IsWeapon(const Enum);
        static bool IsArmor(const Enum);
        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
    };

    struct set_type
    {
        enum Enum
        {
            NotASet = 0,
            TheAssassins,
            TheNeverwinter,
            TheTickler,
            TheMagus,
            TheNecromancers,
            TheWarlocks,
            TheLichKings,
            TheSages,
            TheShamans,
            TheOracles,
            TheAngelic,
            TheBalladeers,
            TheTroubadours,
            TheMuses,
            TheCavaliers,
            TheChampions,
            ThePaladins,
            TheBerserkers,
            TheRosewood,
            TheDragonslayers,
            TheEventideRider,
            TheHunters,
            TheSureShot,
            TheMarksmans,
            TheDeadeye,
            TheDuskRanger,
            TheVenomBow,
            TheCritterChannelers,
            TheMammalianHead,
            TheSavageTaskmasters,
            TheMonsterOverseers,
            TheBeastRulers,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static creature::role::Enum Role(const Enum);
    };

    struct unique_type
    {
        enum Enum
        {
            NotUnique = 0,
            BasiliskTonge,
            BerserkersBeard,
            BishopsHanky,
            BleedingTrophy,
            BloodyDragonScale,
            BottleOfBansheeScreams,
            BraceletCrown,
            BraceletFeather,
            BraceletFist,
            BraceletHourglass,
            BraceletKey,
            BraceletMask,
            BroochCrown,
            BroochFeather,
            BroochFist,
            BroochHourglass,
            BroochKey,
            BroochMask,
            BurialShroud,
            CharmCrown,
            CharmFeather,
            CharmFist,
            CharmHourglass,
            CharmKey,
            CharmMask,
            ChimeraBone,
            CobraTooth,
            CommandersCape,
            BronzeTroll,
            GlassCat,
            CrystalChimes,
            CyclopsEye,
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
            DriedTurtle,
            DruidLeaf,
            EvilRabbitsFoot,
            ExoticGoldenGong,
            FanaticsFlag,
            FriarsChronicle,
            FuneralRecord,
            GeneralsCape,
            GhostSheet,
            GiantOwlEye,
            GriffinFeather,
            HangmansNoose,
            HawkEye,
            HobgoblinNose,
            HoboRing,
            HolyEpic,
            HornOfTheHorde,
            ImpTail,
            IslanderHeaddress,
            JeweledAnkhNecklace,
            JeweledArmband,
            JeweledHandbag,
            JeweledPrincessVeil,
            KingsCape,
            LastRitesScroll,
            MacabreManuscript,
            MadRatJuju,
            MagicHorseshoe,
            MagnifyingGlass,
            ManaAmulet,
            MendicantRing,
            MinotaurHide,
            MonkRing,
            MortuaryOrnament,
            MournersMask,
            PantherPaw,
            PinCrown,
            PinFeather,
            PinFist,
            PinHourglass,
            PinKey,
            PinMask,
            PixieBell,
            PriestRing,
            RascalMask,
            RattlesnakeTail,
            RavenClaw,
            RazorFingerclaw,
            ReaperScythe,
            RegalCaptainsFlag,
            RequiemRegister,
            RoyalScoutSpyglass,
            SaintCameoPin,
            SaintsJournal,
            SanguineRelic,
            ScorpionStingerFingerclaw,
            ScoundrelSack,
            SepultureDecoration,
            ShadeCloak,
            ShamanRainmaker,
            SharkToothNecklace,
            SignetCrown,
            SignetFeather,
            SignetFist,
            SignetHourglass,
            SignetKey,
            SignetMask,
            SirenConch,
            SpecterChains,
            SpecterRobe,
            SprintersLegtie,
            SwindlersBag,
            TribalFlag,
            TricksterPouch,
            TuningFork,
            TurtleShell,
            VampiresToothNecklace,
            ViperFangFingerclaw,
            VultureGizzard,
            WarhorseMarionette,
            WarTrumpet,
            WeaselTotem,
            WolfenFur,
            WraithTalisman,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static misc_type::Enum MiscType(const Enum);
        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
        static bool IsUseable(const Enum);
        static float ReligiousRatio(const Enum);
    };

    struct named_type
    {
        enum Enum
        {
            NotNamed = 0,
            Arctic,
            Army,
            Betrayer,
            Burglar,
            Burning,
            Charlatans,
            Charred,
            Chill,
            Dancing,
            Daring,
            Dark,
            Dawn,
            Diabolic,
            Dusk,
            Elite,
            Fiendish,
            Fiery,
            Focus,
            Frigid,
            Frozen,
            Gladiator,
            Gloom,
            Glory,
            Heros,
            Honest,
            Icy,
            Imposters,
            Infernal,
            Knaves,
            Light,
            Marauder,
            Misery,
            Moon,
            Mountebank,
            Muggers,
            Nile,
            Noble,
            Pickpocket,
            Pirate,
            Princes,
            Proud,
            Pure,
            Raging,
            Ranger,
            Robbers,
            Samurai,
            Searing,
            Soldiers,
            Sorrow,
            Sun,
            Thief,
            Thors,
            Thugs,
            Twilight,
            Valiant,
            Wardens,
            Wicked,
            Winter,
            Woe,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
    };

    struct weapon_type
    {
        enum Enum : unsigned int
        {
            NotAWeapon = 0,

            // all weapons are either Melee or Projectile
            Melee = 1 << 0,
            Projectile = 1 << 1,

            BodyPart = 1 << 2,

            // allows combat verbs slice/cut/slash/etc
            Bladed = 1 << 3,

            // allows combat verbs stab/etc
            Pointed = 1 << 4,

            Sword = 1 << 5,
            Axe = 1 << 6,
            Whip = 1 << 7,
            Knife = 1 << 8,
            Club = 1 << 9,
            Staff = 1 << 10,
            BladedStaff = 1 << 11,

            Last = BladedStaff
        };

        static const std::string ToString(
            const weapon_type::Enum, const bool WILL_WRAP = false, const bool IS_READBLE = false);

        static const std::string Name(const weapon_type::Enum);
        static weapon_type::Enum FromString(const std::string &);
    };

    struct armor_type
    {
        enum Enum
        {
            NotArmor,
            Shield,
            Helm,
            Gauntlets,
            Pants,
            Boots,

            // this type includes all torso coverings, i.e. plate/mail/scale armor
            Shirt,

            Bracers,
            Aventail,

            // either vest, robe, or cloak
            Covering,

            // only used by creatures with skin that is significant to their armor rating, such as
            // dragons with scales but not for human flesh
            Skin,

            Count
        };

        static const std::string ToString(const armor_type::Enum);
        static bool DoesRequireBaseType(const armor_type::Enum);
    };

    struct body_part
    {
        enum Enum
        {
            Fists = 0,
            Claws,
            Tentacles,
            Bite,
            Breath,
            Skin,
            Count
        };

        static const std::string ToString(const body_part::Enum);
    };

    // Responsible for defining the relationship between an item's name and materials.
    struct name_material_type
    {
        enum Enum
        {
            Misc = 0,
            BodyPart,
            Decoration,
            Handle,
            Reinforced,
            ReinforcedWithBase,
            Tipped,
            Claspped,
            ClasppedWithBase,
            Count
        };

        static const std::string ToString(const Enum);

        static bool IsSecondaryMaterialOfNothingRequired(const Enum TYPE)
        {
            return ((TYPE == Decoration) || (TYPE == Reinforced) || (TYPE == ReinforcedWithBase));
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
