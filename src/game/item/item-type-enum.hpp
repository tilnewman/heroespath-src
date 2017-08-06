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
#ifndef GAME_ITEM_ITEMTYPEENUM_INCLUDED
#define GAME_ITEM_ITEMTYPEENUM_INCLUDED
//
// item-type-enum.hpp
//  Enumerations for all of an Item's various types.
//
#include "game/stats/types.hpp"
#include "game/creature/role-enum.hpp"

#include <string>


namespace game
{
namespace item
{

    struct category
    {
        enum Enum : unsigned int
        {
            Broken          = 0, //useless, unable to do or be used for anything
            Weapon          = 1 << 0,
            Armor           = 1 << 1,
            Useable         = 1 << 2,
            Equippable      = 1 << 3,
            BodyPart        = 1 << 4,
            Wearable        = 1 << 5, //if not wearable then it must be one or two-handed
            OneHanded       = 1 << 6,
            TwoHanded       = 1 << 7,
            QuestItem       = 1 << 8,
            Blessed         = 1 << 9,
            Cursed          = 1 << 10,
            AllowsCasting   = 1 << 11,
            ConsumedOnUse   = 1 << 12
        };

        static const std::string ToString(const category::Enum E, const bool WILL_WRAP);
    };


    struct misc_type
    {
        enum Enum
        {
            NotMisc = 0,
            Amulet,
            Ankh_Necklace,
            Armband,
            Bag,
            Balm_Pot,
            Beard,
            Bell,
            Bird_Claw,
            Bone,
            Bone_Whistle,
            Book,
            Bracelet_Crown,
            Bracelet_Feather,
            Bracelet_Fist,
            Bracelet_Hourglass,
            Bracelet_Key,
            Bracelet_Mask,
            Braid,
            Brooch_Crown,
            Brooch_Feather,
            Brooch_Fist,
            Brooch_Hourglass,
            Brooch_Key,
            Brooch_Mask,
            Bust,
            Cameo,
            Cape,
            Cat,
            Chains,
            Charm_Crown,
            Charm_Feather,
            Charm_Fist,
            Charm_Hourglass,
            Charm_Key,
            Charm_Mask,
            Chest,
            Chimes,
            Cloak,
            Conch,
            Crown,
            Crumhorn,
            Devil_Horn,
            Doll,
            Doll_Blessed,
            Doll_Cursed,
            Dried_Head,
            Drink,
            DrumLute,
            Embryo,
            Egg,
            Eye,
            Feather,
            Figurine_Blessed,
            Figurine_Cursed,
            Finger,
            Fingerclaw,
            Flag,
            Frog,
            Gecko,
            Ghost_Sheet,
            Gizzard,
            Goblet,
            Gong,
            Grave_Ornament,
            Handbag,
            Hanky,
            Headdress,
            Herbs,
            Hide,
            Horn,
            Horseshoe,
            Hourglass,
            Hurdy_Gurdy,
            Icicle,
            Iguana,
            Imp_Tail,
            Key,
            Lantern,
            Leaf,
            Legtie,
            Litch_Hand,
            Lizard,
            Lockbox,
            LockPicks,
            Lyre,
            Magnifying_Glass,
            Mask,
            Medallion,
            Mirror,
            Mummy_Hand,
            Necklace,
            Noose,
            Nose,
            Orb,
            Paw,
            Pendant,
            Petrified_Snake,
            Pin_Crown,
            Pin_Feather,
            Pin_Fist,
            Pin_Hourglass,
            Pin_Key,
            Pin_Mask,
            Pipe_And_Tabor,
            Potion,
            Puppet,
            Rabbit_Foot,
            Rainmaker,
            Rat_Juju,
            Rattlesnake_Tail,
            Recorder,
            Relic,
            Ring,
            Robe,
            Salamander,
            Salve,
            Scale,
            Scepter,
            Scroll,
            Scythe,
            Seeds,
            Shard,
            Shark_Tooth_Necklace,
            Shroud,
            Signet_Crown,
            Signet_Feather,
            Signet_Fist,
            Signet_Hourglass,
            Signet_Key,
            Signet_Mask,
            Skink,
            Skull,
            Spider_Eggs,
            Spyglass,
            Talisman,
            Tome,
            Tongue,
            Tooth,
            Tooth_Necklace,
            Torch,
            Troll_Figure,
            Trophy,
            Tuning_Fork,
            Turtle_Shell,
            Unicorn_Horn,
            Veil,
            Viol,
            Wand,
            Warhorse_Marionette,
            Weasel_Totem,
            Wolfen_Fur,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static bool IsMusicalInstrument(const Enum E);
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
            CopperTroll,
            CrystalCat,
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
            ZombieSeeds,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static misc_type::Enum MiscType(const Enum);
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
    };


    struct material
    {
        enum Enum
        {
            //Use Nothing as default instead of Error so that an Item's
            //materialSec_ can be Nothing.
            Nothing = 0,

            //what Ghosts and Shades are made of
            Spirit,

            Gas,
            Water,
            Blood,
            Acid,
            Dirt,
            Paper,
            Glass,
            Feather,
            Fur,
            Hair,
            Flesh,
            Rope,
            Cloth,
            Hide,
            SoftLeather,
            HardLeather,
            Plant,
            Claw,
            Scale,
            Horn,
            Bone,
            Tooth,
            Wood,
            Stone,
            Obsidian,
            Tin,        // 18 /  7.3 -modulus of rigidity / density
            Bronze,     // 45 /  7.5
            Iron,       // 41 /  7.8
            Steel,      // 77 /  8.0
            Jade,
            Amethyst,
            Emerald,
            Pearl,
            Ruby,
            Lazuli,
            Silver,     // 48 / 10.5
            Sapphire,
            Gold,       // 27 / 19.3
            Platinum,   // 82 / 21.5
            Diamond,
            Count
        };

        static const std::string ToString(const material::Enum);
        static const std::string ToReadableString(const material::Enum);
        static stats::Trait_t ArmorRatingBonus(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC);
        static stats::Trait_t ArmorRatingBonusPri(const material::Enum MATERIAL_PRI);
        static stats::Trait_t ArmorRatingBonusSec(const material::Enum MATERIAL_SEC);
        static stats::Trait_t PriceAdj(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC);
        static stats::Trait_t PriceAdjPri(const material::Enum MATERIAL_PRI);
        static stats::Trait_t PriceAdjSec(const material::Enum MATERIAL_SEC);
        static float WeightMult(const material::Enum MATEIAL_PRI, const material::Enum MATERIAL_SEC);
        static float WeightMultPri(const material::Enum MATERIAL_PRI);
        static float WeightMultSec(const material::Enum MATERIAL_SEC);
        static int Bonus(const material::Enum);

        static bool IsMagical(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static bool IsLiquid(const material::Enum MATERIAL);
        static bool IsSolid(const material::Enum MATERIAL);
        static bool IsBendy(const material::Enum MATERIAL);
        static bool IsRigid(const material::Enum MATERIAL);
        static bool ContainsSpirit(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static bool IsBloody(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static bool ContainsFlesh(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static float FireDamageRatio(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static bool IsMetal(const material::Enum MATERIAL);
        static bool ContainsMetal(const material::Enum PRI, const material::Enum SEC);
        static bool IsStone(const material::Enum PRI);
        static bool IsPrecious(const material::Enum MATERIAL);
        static bool ContiansPrecious(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        static bool IsJewel(const material::Enum MATERIAL);
        static bool ContainsJewel(const material::Enum PRI, const material::Enum SEC = material::Nothing);
        inline bool static IsLeather(const material::Enum M) { return ((M == material::SoftLeather) || (M == material::HardLeather)); }
        inline bool static IsClothOrLeather(const material::Enum M) { return ((M == material::Cloth) || IsLeather(M)); }
    };


    struct weapon_type
    {
        enum Enum : unsigned int
        {
            NotAWeapon  = 0,
            Melee       = 1 << 0,
            Projectile  = 1 << 1,
            Bladed      = 1 << 2,
            Pointed     = 1 << 3,
            Claws       = 1 << 4,
            Bite        = 1 << 5,
            Sword       = 1 << 6,
            Axe         = 1 << 7,
            Whip        = 1 << 8,
            Blowpipe    = 1 << 9,
            Bow         = 1 << 10,
            Crossbow    = 1 << 11,
            Spear       = 1 << 12,
            Knife       = 1 << 13,
            Club        = 1 << 14,
            Staff       = 1 << 15,
            Sling       = 1 << 16,
            BladedStaff = 1 << 17,
            Fists       = 1 << 18,
            Tendrils    = 1 << 19,
            Breath      = 1 << 20
        };

        static const std::string ToString(const weapon_type::Enum E, const bool WILL_WRAP);
        static weapon_type::Enum FromString(const std::string &);
    };


    struct armor_type
    {
        enum Enum : unsigned int
        {
            NotArmor    = 0,
            Sheild      = 1 << 0,
            Helm        = 1 << 1,
            Gauntlets   = 1 << 2,
            Pants       = 1 << 3,
            Boots       = 1 << 4,
            Shirt       = 1 << 5,//includes torso coverings, i.e. plate/mail/scale/etc.
            Bracer      = 1 << 6,//forearm covering
            Aventail    = 1 << 7,//neck covering attached to the bottom of a helm
            Covering    = 1 << 8,//vest/robe/cloak
            Skin        = 1 << 9 //hide or scales for beast creatures
        };

        static const std::string ToString(const armor_type::Enum E, const bool WILL_WRAP);
    };

}
}
#endif //GAME_ITEM_ITEMTYPEENUM_INCLUDED
