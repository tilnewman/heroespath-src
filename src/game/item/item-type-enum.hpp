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
#include "game/item/types.hpp"
#include "game/stats/types.hpp"

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
            Edible          = 1 << 9,
            Blessed         = 1 << 10,
            Cursed          = 1 << 11,
            AllowsCasting   = 1 << 12
        };

        static const std::string ToString(const category::Enum E, const bool WILL_WRAP);
    };


    struct unique_type
    {
        enum Enum
        {
            NotUnique = 0,
            //TODO
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
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
            Drink,
            DrumLute,
            Eye,
            Finger,
            Fingerclaw,
            Flag,
            Frog,
            Gecko,
            Ghost_Sheet,
            Gizzard,
            Goblet,
            Gong,
            Handbag,
            Headdress,
            Herbs,
            Horn,
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
            Nose,
            Orb,
            Paw,
            Pendant,
            Petrified_Snake,
            Pin_Book,
            Pin_Clover,
            Pin_Foot,
            Pin_Nymph,
            Pin_Quiver,
            Pin_Hourglass,
            Pipe_And_Tabor,
            Potion,
            Rabbit_Foot,
            Rainmaker,
            Rat_Juju,
            Rattlesnake_Tail,
            Recorder,
            Relic,
            Ring,
            Robe,
            Salve,
            Scepter,
            Scroll,
            Seeds,
            Shard,
            Shark_Tooth_Necklace,
            Skull,
            Spider_Eggs,
            Spyglass,
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
            Count
        };

        static const std::string ToString(const misc_type::Enum);
        static const std::string Name(const misc_type::Enum);
        static bool IsMusicalInstrument(const misc_type::Enum E);
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
        static stats::Armor_t ArmorRatingBonus(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC);
        static stats::Armor_t ArmorRatingBonusPri(const material::Enum MATERIAL_PRI);
        static stats::Armor_t ArmorRatingBonusSec(const material::Enum MATERIAL_SEC);
        static item::Coin_t PriceAdj(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC);
        static item::Coin_t PriceAdjPri(const material::Enum MATERIAL_PRI);
        static item::Coin_t PriceAdjSec(const material::Enum MATERIAL_SEC);
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
