// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
//
// item-type-enum.hpp
//  Enumerations for all of an Item's various types.
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/trait.hpp"
#include "misc/enum-util.hpp"
#include "misc/types.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    struct category : public misc::EnumBaseBitField<category>
    {
        enum Enum : misc::EnumUnderlying_t
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

        static void ToStringPopulate(
            std::ostringstream &,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR);
    };

    struct material : public misc::EnumBaseCounting<material, misc::EnumFirstValueNothing>
    {
        enum Enum : misc::EnumUnderlying_t
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

    struct element_type : public misc::EnumBaseBitField<element_type>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Fire = 1 << 0,
            Frost = 1 << 1,
            Honor = 1 << 2,
            Shadow = 1 << 3,
            Last = Shadow
        };

        static void ToStringPopulate(
            std::ostringstream &,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR);

        static bool IsValid(const Enum);

        static const std::string Name(const Enum);

        template <typename T>
        static const std::vector<element_type::Enum>
            ValidCombinations(const T VALUE, const bool WILL_INCLUDE_NOTHING = false)
        {
            auto const ELEMENT_TYPE{ static_cast<element_type::Enum>(VALUE) };

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

        static const std::vector<element_type::Enum> ValidCombinations()
        {
            return ValidCombinations(static_cast<element_type::Enum>(
                element_type::Fire | element_type::Frost | element_type::Honor
                | element_type::Shadow));
        }
    };

    using ElementEnumVec_t = std::vector<element_type::Enum>;

    struct misc_type : public misc::EnumBaseCounting<misc_type, misc::EnumFirstValueNot>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Not = 0,

            // quest items
            // all musical instruments except for the DrumLute are quest items
            AngelBraid,
            Crumhorn,
            DevilHorn,
            GolemFinger,
            HurdyGurdy,
            Icicle,
            LitchHand,
            Lyre,
            MummyHand,
            PetrifiedSnake,
            PipeAndTabor,
            Recorder,
            UnicornHorn,
            Viol,

            // unique items
            BasiliskTonge,
            BerserkersBeard,
            BishopsHanky,
            BleedingTrophy,
            BloodyDragonScale,
            BottleOfBansheeScreams,
            BronzeTroll,
            Brooch_Crown,
            Brooch_Feather,
            Brooch_Fist,
            Brooch_Hourglass,
            Brooch_Key,
            Brooch_Mask,
            BurialShroud,
            CapeCommanders,
            CapeGenerals,
            CapeKings,
            Charm_Crown,
            Charm_Feather,
            Charm_Fist,
            Charm_Hourglass,
            Charm_Key,
            Charm_Mask,
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
            Pin_Crown,
            Pin_Feather,
            Pin_Fist,
            Pin_Hourglass,
            Pin_Key,
            Pin_Mask,
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

            // standalone items
            Bust,
            Doll,
            DollBlessed,
            DollCursed,
            DriedHead,
            DrumLute, // the only musical instrument that is not a quest item
            Egg,
            Embryo,
            FigurineBlessed,
            FigurineCursed,
            Goblet,
            Key,
            LockPicks,
            Mirror,
            Orb,
            Pendant,
            PuppetBlessed,
            PuppetCursed,
            Ring,
            Scepter,
            Seeds,
            Shard,
            Staff,
            SummoningStatue,
            Wand,

            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);

        // these three functions are complete and mutually exclusive
        static bool IsQuestItem(const Enum);
        static bool IsStandalone(const Enum);
        static bool IsUnique(const Enum);

        static bool HasPixieVersion(const Enum);
        static bool HasOnlyPixieVersion(const Enum);
        static bool MustBePixieVersionForPixiesToEquip(const Enum);

        static float ReligiousRatio(const Enum);
        static bool HasNonFleshEyes(const Enum);
        static bool IsBlessed(const Enum);
        static bool IsCursed(const Enum);
        static Weight_t Weight(const Enum);
        static bool IsUseable(const Enum);
        static bool IsMusicalInstrument(const Enum);
        static bool IsSummoning(const Enum);
        static category::Enum EquipCategory(const Enum);
        static bool AllowsCasting(const Enum);
        static bool IsWeapon(const Enum);
        static bool IsArmor(const Enum);

        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
    };

    struct set_type : public misc::EnumBaseCounting<set_type, misc::EnumFirstValueNot>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Not = 0,
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

    struct named_type : public misc::EnumBaseCounting<named_type, misc::EnumFirstValueNot>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Not = 0,
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

    struct weapon_type : public misc::EnumBaseBitField<weapon_type>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,

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

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR = "/");

        static const std::string Name(const weapon_type::Enum);
    };

    struct armor_type : public misc::EnumBaseCounting<armor_type, misc::EnumFirstValueNot>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Not,
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

    struct body_part : public misc::EnumBaseCounting<body_part, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
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
        : public misc::EnumBaseCounting<name_material_type, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
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
