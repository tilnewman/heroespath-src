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
#include <vector>
#include <utility>


namespace game
{
namespace item
{

    struct category
    {
        enum Enum : unsigned int
        {
            None            = 0,
            Broken          = 1 << 0, //useless, unable to do or be used for anything
            Weapon          = 1 << 1,
            Armor           = 1 << 2,
            Useable         = 1 << 3,
            Equippable      = 1 << 4,
            BodyPart        = 1 << 5,
            Wearable        = 1 << 6, //if not wearable then it must be one or two-handed
            OneHanded       = 1 << 7,
            TwoHanded       = 1 << 8,
            QuestItem       = 1 << 9,
            Blessed         = 1 << 10,
            Cursed          = 1 << 11,
            AllowsCasting   = 1 << 12,
            ConsumedOnUse   = 1 << 13,
            ShowsEnemyInfo  = 1 << 14
        };

        static const std::string ToString(const category::Enum E, const bool WILL_WRAP);
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

        static const std::vector<material::Enum> CoreMetal();
        static const std::vector<material::Enum> CoreJewel();
        static const std::vector<material::Enum> CoreMisc();
        static const std::vector<material::Enum> CorePrimary();
        static const std::vector<material::Enum> CorePrimaryNoPearl();
        static const std::vector<material::Enum> CoreSecondary();

        static void AppendCoreMetal(std::vector<material::Enum> &);
        static void AppendCoreJewel(std::vector<material::Enum> &);
        static void AppendCoreMisc(std::vector<material::Enum> &);
        static void AppendCorePrimary(std::vector<material::Enum> &);
        static void AppendCorePrimaryNoPearl(std::vector<material::Enum> &);
        static void AppendCoreSecondary(std::vector<material::Enum> &);

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CorePrimaryAndSecondary()
        {
            return std::make_pair(CorePrimary(), CoreSecondary());
        }

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CorePrimaryAndNoSecondary()
        {
            return std::make_pair(CorePrimary(), std::vector<material::Enum>());
        }

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CoreMetalAndCoreSecondary()
        {
            return std::make_pair(CoreMetal(), CoreSecondary());
        }

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CoreJewelAndCoreSecondary()
        {
            return std::make_pair(CoreJewel(), CoreSecondary());
        }

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CoreMetalJewelAndCoreSecondary()
        {
            std::vector<material::Enum> v;
            AppendCoreMetal(v);
            AppendCoreJewel(v);
            return std::make_pair(v, CoreSecondary());
        }

        inline static const std::pair<std::vector<material::Enum>, std::vector<material::Enum> >
            CorePrimaryNoPearlAndSecondary()
        {
            return std::make_pair(CorePrimaryNoPearl(), CoreSecondary());
        }
        
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

    using MaterialVec_t = std::vector<material::Enum>;
    using MaterialVecPair_t = std::pair<MaterialVec_t, MaterialVec_t>;
    

    struct element_type
    {
        enum Enum : unsigned int
        {
            None    = 0,
            Fire    = 1 << 0,
            Frost   = 1 << 1,
            Honor   = 1 << 2,
            Shadow  = 1 << 3
        };

        static const std::string ToString(const Enum, const bool WILL_WRAP = false);
        static const std::string Name(const Enum, const bool WILL_WRAP = false);
        static const std::vector<element_type::Enum> Combinations(const Enum);
        static const std::vector<element_type::Enum> AllCombinations()
        {
            return Combinations(static_cast<element_type::Enum>(element_type::Fire |
                                                                element_type::Frost |
                                                                element_type::Honor |
                                                                element_type::Shadow));
        }
    };

    using ElementTypeVec_t = std::vector<element_type::Enum>;


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
            Staff,
            Summoning_Statue,
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
        static bool IsMusicalInstrument(const Enum);
        static const MaterialVecPair_t Materials(const Enum);
        static bool IsStandaloneItem(const Enum);
        static bool HasPixieVersion(const Enum);
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
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static misc_type::Enum MiscType(const Enum);
        static element_type::Enum ElementTypes(const Enum);
        static const MaterialVecPair_t Materials(const Enum);
        static bool IsUseable(const Enum);
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
        static element_type::Enum ElementTypes(const Enum);
        static const MaterialVecPair_t Materials(const Enum);
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
