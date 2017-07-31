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
// item-type-enum.cpp
//
#include "item-type-enum.hpp"

#include "game/game-data-file.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace item
{

    const std::string category::ToString(const item::category::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == category::Broken)
        {
            ss << "broken/useless";
        }
        else
        {
            if (E & category::Weapon)       ss << "weapon";
            if (E & category::Armor)        ss << ((ss.str().empty()) ? "" : ", ") << "armor";
            if (E & category::Useable)      ss << ((ss.str().empty()) ? "" : ", ") << "useable";
            if (E & category::Equippable)   ss << ((ss.str().empty()) ? "" : ", ") << "equippable";
            if (E & category::BodyPart)     ss << ((ss.str().empty()) ? "" : ", ") << "bodypart";
            if (E & category::Wearable)     ss << ((ss.str().empty()) ? "" : ", ") << "wearable";
            if (E & category::OneHanded)    ss << ((ss.str().empty()) ? "" : ", ") << "one-handed";
            if (E & category::TwoHanded)    ss << ((ss.str().empty()) ? "" : ", ") << "two-handed";
            if (E & category::QuestItem)    ss << ((ss.str().empty()) ? "" : ", ") << "quest item";
            if (E & category::Edible)       ss << ((ss.str().empty()) ? "" : ", ") << "edible";
            if (E & category::Blessed)      ss << ((ss.str().empty()) ? "" : ", ") << "blessed";
            if (E & category::Cursed)       ss << ((ss.str().empty()) ? "" : ", ") << "cursed";
            if (E & category::AllowsCasting)ss << ((ss.str().empty()) ? "" : ", ") << "allows casting";
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::item::category::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    const std::string misc_type::ToString(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:               { return "NotMisc"; }
            case Amulet:                { return "Amulet"; }
            case Ankh_Necklace:         { return "Ankh_Necklace"; }
            case Armband:               { return "Armband"; }
            case Bag:                   { return "Bag"; }
            case Balm_Pot:              { return "Balm_Pot"; }
            case Beard:                 { return "Beard"; }
            case Bell:                  { return "Bell"; }
            case Bird_Claw:             { return "Bird_Claw"; }
            case Bone:                  { return "Bone"; }
            case Bone_Whistle:          { return "Bone_Whistle"; }
            case Book:                  { return "Book"; }
            case Bracelet_Crown:        { return "Bracelet_Crown"; }
            case Bracelet_Feather:      { return "Bracelet_Feather"; }
            case Bracelet_Fist:         { return "Bracelet_Fist"; }
            case Bracelet_Hourglass:    { return "Bracelet_Hourglass"; }
            case Bracelet_Key:          { return "Bracelet_Key"; }
            case Bracelet_Mask:         { return "Bracelet_Mask"; }
            case Braid:                 { return "Braid"; }
            case Brooch_Crown:          { return "Brooch_Crown"; }
            case Brooch_Feather:        { return "Brooch_Feather"; }
            case Brooch_Fist:           { return "Brooch_Fist"; }
            case Brooch_Hourglass:      { return "Brooch_Hourglass"; }
            case Brooch_Key:            { return "Brooch_Key"; }
            case Brooch_Mask:           { return "Brooch_Mask"; }
            case Cameo:                 { return "Cameo"; }
            case Cape:                  { return "Cape"; }
            case Cat:                   { return "Cat"; }
            case Chains:                { return "Chains"; }
            case Charm_Crown:           { return "Charm_Crown"; }
            case Charm_Feather:         { return "Charm_Feather"; }
            case Charm_Fist:            { return "Charm_Fist"; }
            case Charm_Hourglass:       { return "Charm_Hourglass"; }
            case Charm_Key:             { return "Charm_Key"; }
            case Charm_Mask:            { return "Charm_Mask"; }
            case Chest:                 { return "Chest"; }
            case Chimes:                { return "Chimes"; }
            case Cloak:                 { return "Cloak"; }
            case Conch:                 { return "Conch"; }
            case Crown:                 { return "Crown"; }
            case Crumhorn:              { return "Crumhorn"; }
            case Devil_Horn:            { return "Devil_Horn"; }
            case Doll:                  { return "Doll"; }
            case Drink:                 { return "Drink"; }
            case DrumLute:              { return "DrumLute"; }
            case Eye:                   { return "Eye"; }
            case Finger:                { return "Finger"; }
            case Fingerclaw:            { return "Fingerclaw"; }
            case Flag:                  { return "Flag"; }
            case Frog:                  { return "Frog"; }
            case Gecko:                 { return "Gecko"; }
            case Ghost_Sheet:           { return "Ghost_Sheet"; }
            case Gizzard:               { return "Gizzard"; }
            case Goblet:                { return "Goblet"; }
            case Gong:                  { return "Gong"; }
            case Handbag:               { return "Handbag"; }
            case Herbs:                 { return "Herbs"; }
            case Headdress:             { return "Headdress"; }
            case Hourglass:             { return "Hourglass"; }
            case Horn:                  { return "Horn"; }
            case Hurdy_Gurdy:           { return "Hurdy_Gurdy"; }
            case Icicle:                { return "Icicle"; }
            case Iguana:                { return "Iguana"; }
            case Imp_Tail:              { return "Imp_Tail"; }
            case Key:                   { return "Key"; }
            case Lantern:               { return "Lantern"; }
            case Leaf:                  { return "Leaf"; }
            case Legtie:                { return "Legtie"; }
            case Litch_Hand:            { return "Litch_Hand"; }
            case Lizard:                { return "Lizard"; }
            case Lockbox:               { return "Lockbox"; }
            case LockPicks:             { return "LockPicks"; }
            case Lyre:                  { return "Lyre"; }
            case Magnifying_Glass:      { return "Magnifying_Glass"; }
            case Mask:                  { return "Mask"; }
            case Medallion:             { return "Medallion"; }
            case Mirror:                { return "Mirror"; }
            case Mummy_Hand:            { return "Mummy_Hand"; }
            case Necklace:              { return "Necklace"; }
            case Nose:                  { return "Nose"; }
            case Orb:                   { return "Orb"; }
            case Paw:                   { return "Paw"; }
            case Pendant:               { return "Pendant"; }
            case Petrified_Snake:       { return "Petrified_Snake"; }
            case Pin_Book:              { return "Pin_Book"; }
            case Pin_Clover:            { return "Pin_Clover"; }
            case Pin_Foot:              { return "Pin_Foot"; }
            case Pin_Nymph:             { return "Pin_Nymph"; }
            case Pin_Quiver:            { return "Pin_Quiver"; }
            case Pipe_And_Tabor:        { return "Pipe_And_Tabor"; }
            case Potion:                { return "Potion"; }
            case Rabbit_Foot:           { return "Rabbit_Foot"; }
            case Rainmaker:             { return "Rainmaker"; }
            case Rat_Juju:              { return "Rat_Juju"; }
            case Rattlesnake_Tail:      { return "Rattlesnake_Tail"; }
            case Recorder:              { return "Recorder"; }
            case Relic:                 { return "Relic"; }
            case Ring:                  { return "Ring"; }
            case Robe:                  { return "Robe"; }
            case Salve:                 { return "Salve"; }
            case Scepter:               { return "Scepter"; }
            case Scroll:                { return "Scroll"; }
            case Seeds:                 { return "Seeds"; }
            case Shard:                 { return "Shard"; }
            case Shark_Tooth_Necklace:  { return "Shark_Tooth_Necklace"; }
            case Skull:                 { return "Skull"; }
            case Spider_Eggs:           { return "Spider_Eggs"; }
            case Spyglass:              { return "Spyglass"; }
            case Tome:                  { return "Tome"; }
            case Tongue:                { return "Tongue"; }
            case Tooth:                 { return "Tooth"; }
            case Tooth_Necklace:        { return "Tooth_Necklace"; }
            case Torch:                 { return "Torch"; }
            case Troll_Figure:          { return "Troll_Figure"; }
            case Trophy:                { return "Trophy"; }
            case Tuning_Fork:           { return "Tuning_Fork"; }
            case Turtle_Shell:          { return "Turtle_Shell"; }
            case Unicorn_Horn:          { return "Unicorn_Horn"; }
            case Veil:                  { return "Veil"; }
            case Viol:                  { return "Viol"; }
            case Wand:                  { return "Wand"; }
            case Warhorse_Marionette:   { return "Warhorse_Marionette"; }
            case Weasel_Totem:          { return "Weasel_Totem"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::misc_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string misc_type::Name(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:
            case Ankh_Necklace:         { return "Ankh Necklace"; }
            case Balm_Pot:              { return "Pot of Balm"; }
            case Bird_Claw:             { return "Raven Claw"; }
            case Bone_Whistle:          { return "Dragon Bone Whistle"; }
            case Bracelet_Crown:        { return "Crown Bracelet"; }
            case Bracelet_Feather:      { return "Feather Bracelet"; }
            case Bracelet_Fist:         { return "Fist Bracelet"; }
            case Bracelet_Hourglass:    { return "Hourglass Bracelet"; }
            case Bracelet_Key:          { return "Key Bracelet"; }
            case Bracelet_Mask:         { return "Mask Bracelet"; }
            case Brooch_Crown:          { return "Crown Brooch"; }
            case Brooch_Feather:        { return "Feather Brooch"; }
            case Brooch_Fist:           { return "Fist Brooch"; }
            case Brooch_Hourglass:      { return "Hourglass Brooch"; }
            case Brooch_Key:            { return "Key Brooch"; }
            case Brooch_Mask:           { return "Mask Brooch"; }
            case Charm_Crown:           { return "Crown Charm"; }
            case Charm_Feather:         { return "Feather Charm"; }
            case Charm_Fist:            { return "Fist Charm"; }
            case Charm_Hourglass:       { return "Hourglass Charm"; }
            case Charm_Key:             { return "Key Charm"; }
            case Charm_Mask:            { return "Mask Charm"; }
            case Devil_Horn:            { return "Devil Horn"; }
            case Ghost_Sheet:           { return "Ghost Sheet"; }
            case Hurdy_Gurdy:           { return "Hurdy-Gurdy"; }
            case Imp_Tail:              { return "Imp Tail"; }
            case Litch_Hand:            { return "Litch Hand"; }
            case Magnifying_Glass:      { return "Magnifying Glass"; }
            case Petrified_Snake:       { return "Petrified Snake"; }
            case Pin_Book:              { return "Arcane Tome Pin"; }
            case Pin_Clover:            { return "Leprechaun Clover Pin"; }
            case Pin_Foot:              { return "Titan's Foot Pin"; }
            case Pin_Nymph:             { return "Jeweled Nymph Pin"; }
            case Pin_Quiver:            { return "King's Quiver Pin"; }
            case Pipe_And_Tabor:        { return "Pipe and Tabor"; }
            case Rabbit_Foot:           { return "Rabbit Foot"; }
            case Rat_Juju:              { return "Rat Juju"; }
            case Rattlesnake_Tail:      { return "Rattlesnake Tail"; }
            case Shark_Tooth_Necklace:  { return "Shark Tooth Necklace"; }
            case Spider_Eggs:           { return "Spider Eggs"; }
            case Tooth_Necklace:        { return "Vampire Tooth Necklace"; }
            case Troll_Figure:          { return "Troll Figure"; }
            case Tuning_Fork:           { return "Tuning Fork"; }
            case Turtle_Shell:          { return "Turtle Shell"; }
            case Unicorn_Horn:          { return "Unicorn Horn"; }
            case Warhorse_Marionette:   { return "Warhorse Marionette"; }
            case Weasel_Totem:          { return "Weasel Totem"; }
            case Amulet:
            case Armband:
            case Bag:
            case Beard:
            case Bell:
            case Bone:
            case Book:
            case Braid:
            case Cameo:
            case Cape:
            case Cat:
            case Chains:
            case Chest:
            case Chimes:
            case Cloak:
            case Conch:
            case Crown:
            case Crumhorn:
            case Doll:
            case Drink:
            case DrumLute:
            case Eye:
            case Finger:
            case Fingerclaw:
            case Flag:
            case Frog:
            case Gecko:
            case Gizzard:
            case Goblet:
            case Gong:
            case Handbag:
            case Headdress:
            case Herbs:
            case Horn:
            case Hourglass:
            case Icicle:
            case Iguana:
            case Key:
            case Lantern:
            case Leaf:
            case Legtie:
            case Lizard:
            case Lockbox:
            case LockPicks:
            case Lyre:
            case Mask:
            case Medallion:
            case Mirror:
            case Mummy_Hand:
            case Necklace:
            case Nose:
            case Orb:
            case Paw:
            case Pendant:
            case Potion:
            case Rainmaker:
            case Recorder:
            case Relic:
            case Ring:
            case Robe:
            case Salve:
            case Scepter:
            case Scroll:
            case Seeds:
            case Skull:
            case Shard:
            case Spyglass:
            case Torch:
            case Tome:
            case Tongue:
            case Tooth:
            case Trophy:
            case Veil:
            case Viol:
            case Wand: { return misc_type::ToString(E); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::misc_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool misc_type::IsMusicalInstrument(const misc_type::Enum E)
    {
        return ((E == DrumLute) ||
                (E == Crumhorn) ||
                (E == Hurdy_Gurdy) ||
                (E == Lyre) ||
                (E == Pipe_And_Tabor) ||
                (E == Recorder) ||
                (E == Viol));
    }


    const std::string material::ToString(const item::material::Enum E)
    {
        switch (E)
        {
            case Nothing    : { return "Nothing"; }
            case Wood       : { return "Wood"; }
            case HardLeather: { return "HardLeather"; }
            case SoftLeather: { return "SoftLeather"; }
            case Bone       : { return "Bone"; }
            case Water      : { return "Water"; }
            case Dirt       : { return "Dirt"; }
            case Acid       : { return "Acid"; }
            case Stone      : { return "Stone"; }
            case Plant      : { return "Plant"; }
            case Flesh      : { return "Flesh"; }
            case Hide       : { return "Hide"; }
            case Feather    : { return "Feather"; }
            case Fur        : { return "Fur"; }
            case Hair       : { return "Hair"; }
            case Scale      : { return "Scale"; }
            case Obsidian   : { return "Obsidian"; }
            case Jade       : { return "Jade"; }
            case Amethyst   : { return "Amethyst"; }
            case Emerald    : { return "Emerald"; }
            case Pearl      : { return "Pearl"; }
            case Ruby       : { return "Ruby"; }
            case Lazuli     : { return "Lazuli"; }
            case Sapphire   : { return "Sapphire"; }
            case Diamond    : { return "Diamond"; }
            case Glass      : { return "Glass"; }
            case Gas        : { return "Gas"; }
            case Blood      : { return "Blood"; }
            case Paper      : { return "Paper"; }
            case Rope       : { return "Rope"; }
            case Cloth      : { return "Cloth"; }
            case Claw       : { return "Claw"; }
            case Horn       : { return "Horn"; }
            case Tooth      : { return "Tooth"; }
            case Spirit     : { return "Spirit"; }
            case Tin        : { return "Tin"; }
            case Gold       : { return "Gold"; }
            case Iron       : { return "Iron"; }
            case Bronze     : { return "Bronze"; }
            case Silver     : { return "Silver"; }
            case Steel      : { return "Steel"; }
            case Platinum   : { return "Platinum"; }
            case Count      :
            default:
            {
                std::ostringstream ss;
                ss << "game::item::material::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string material::ToReadableString(const item::material::Enum E)
    {
        if (E == material::SoftLeather)
        {
            return "Soft-Leather";
        }
        else if (E == material::HardLeather)
        {
            return "Hard-Leather";
        }
        else
        {
            return ToString(E);
        }
    }


    stats::Armor_t material::ArmorRatingBonus(const material::Enum MATERIAL_PRI,
                                              const material::Enum MATERIAL_SEC)
    {
        return ArmorRatingBonusPri(MATERIAL_PRI) + ArmorRatingBonusSec(MATERIAL_SEC);
    }


    stats::Armor_t material::ArmorRatingBonusPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    :
            case Spirit     :
            case Gas        :
            case Water      :
            case Blood      :
            case Acid       :
            case Dirt       :
            case Paper      :
            case Glass      :
            case Feather    :
            case Fur        :
            case Hair       : { return 0; }
            case Flesh      : { return 1; }
            case Rope       :
            case Cloth      : { return 2; }
            case Hide       : { return 5; }
            case SoftLeather: { return 6; }
            case HardLeather: { return 10; }
            case Plant      : { return 11; }
            case Claw       : { return 12; }
            case Scale      : { return 13; }
            case Horn       : { return 14; }
            case Bone       : { return 15; }
            case Tooth      : { return 16; }
            case Wood       : { return 17; }
            case Stone      : { return 20; }
            case Obsidian   : { return 21; }
            case Tin        : { return 25; }
            case Bronze     : { return 26; }
            case Iron       : { return 27; }
            case Steel      : { return 28; }
            case Jade       : { return 29; }
            case Amethyst   : { return 30; }
            case Emerald    : { return 31; }
            case Pearl      : { return 32; }
            case Ruby       : { return 33; }
            case Lazuli     : { return 34; }
            case Silver     : { return 35; }
            case Sapphire   : { return 36; }
            case Gold       : { return 40; }
            case Platinum   : { return 45; }
            case Diamond    : { return 50; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::material::ArmorRatingBonusPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    stats::Armor_t material::ArmorRatingBonusSec(const material::Enum MATERIAL_SEC)
    {
        auto const SEC_MATERIAL_ARMOR_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-item-secondary-material-armor-adj-ratio") };

        return static_cast<stats::Armor_t>(static_cast<float>(ArmorRatingBonusPri(MATERIAL_SEC)) *
            SEC_MATERIAL_ARMOR_ADJ_RATIO);
    }


    item::Coin_t material::PriceAdj(const material::Enum MATERIAL_PRI,
                                    const material::Enum MATERIAL_SEC)
    {
        return PriceAdjPri(MATERIAL_PRI) + PriceAdjSec(MATERIAL_SEC);
    }


    item::Coin_t material::PriceAdjPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    : { return 0; }
            case Spirit     : { return 0; }
            case Gas        : { return 0; }
            case Water      : { return 0; }
            case Blood      : { return 0; }
            case Acid       : { return 100; }
            case Dirt       : { return 0; }
            case Paper      : { return 0; }
            case Glass      : { return 10; }
            case Feather    : { return 0; }
            case Fur        : { return 8; }
            case Hair       : { return 0; }
            case Flesh      : { return 0; }
            case Rope       : { return 0; }
            case Cloth      : { return 0; }
            case Hide       : { return 7; }
            case SoftLeather: { return 5; }
            case HardLeather: { return 10; }
            case Plant      : { return 0; }
            case Claw       : { return 4; }
            case Scale      : { return 1000; }
            case Horn       : { return 6; }
            case Bone       : { return 12; }
            case Tooth      : { return 5; }
            case Wood       : { return 0; }
            case Stone      : { return 0; }
            case Obsidian   : { return 20; }
            case Tin        : { return 30; }
            case Bronze     : { return 32; }
            case Iron       : { return 36; }
            case Steel      : { return 40; }
            case Jade       : { return 100; }
            case Amethyst   : { return 200; }
            case Emerald    : { return 300; }
            case Pearl      : { return 500; }
            case Ruby       : { return 600; }
            case Lazuli     : { return 700; }
            case Silver     : { return 1000; }
            case Sapphire   : { return 2000; }
            case Gold       : { return 5000; }
            case Platinum   : { return 7500; }
            case Diamond    : { return 10000; }
            case Count      :
            default         :
            {
                std::ostringstream ss;

                ss << "game::item::material::PriceAdjPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    item::Coin_t material::PriceAdjSec(const material::Enum E)
    {
        return PriceAdjPri(E) / 10;
    }


    float material::WeightMult(const material::Enum MATERIAL_PRI,
                               const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_PRI) + WeightMultSec(MATERIAL_SEC);
    }


    float material::WeightMultPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    : { return 0.0f; }
            case Spirit     : { return 0.0f; }
            case Gas        : { return 0.001f; }
            case Plant      : { return 0.9f; }
            case Cloth      : { return 0.9f; }
            case SoftLeather: { return 1.0f; }
            case HardLeather: { return 1.1f; }
            case Rope       : { return 1.1f; }
            case Bone       : { return 1.15f; }
            case Paper      : { return 1.2f; }
            case Claw       : { return 1.2f; }
            case Horn       : { return 1.2f; }
            case Tooth      : { return 1.2f; }
            case Water      : { return 1.5f; }
            case Acid       : { return 1.5f; }
            case Blood      : { return 1.55f; }
            case Flesh      : { return 1.6f; }
            case Hide       : { return 1.35f; }
            case Feather    : { return 0.5f; }
            case Fur        : { return 0.75f; }
            case Hair       : { return 0.5f; }
            case Scale      : { return 0.9f; }
            case Glass      : { return 1.6f; }
            case Wood       : { return 1.85f; }
            case Dirt       : { return 1.9f; }
            case Obsidian   : { return 2.2f; }
            case Jade       : { return 2.1f; }
            case Amethyst   : { return 2.1f; }
            case Emerald    : { return 2.15f; }
            case Pearl      : { return 1.2f; }
            case Ruby       : { return 2.2f; }
            case Lazuli      : { return 2.5f; }
            case Sapphire   : { return 2.1f; }
            case Diamond    : { return 2.6f; }
            case Stone      : { return 3.0f; }
            case Tin        : { return 4.0f; } // 7.3 - density
            case Bronze     : { return 4.2f; } // 7.5
            case Iron       : { return 4.6f; } // 7.8
            case Steel      : { return 4.8f; } // 8.0
            case Silver     : { return 7.3f; } // 10.5
            case Gold       : { return 16.6f; }// 19.3
            case Platinum   : { return 18.8f; }// 21.5
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "game::item::material::WeightMultPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    float material::WeightMultSec(const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_SEC) / 10.0f;
    }


    int material::Bonus(const material::Enum E)
    {
        switch (E)
        {
            case Nothing    :
            case Spirit     :
            case Gas        :
            case Blood      :
            case Water      :
            case Dirt       :
            case Acid       :
            case Paper      :
            case Glass      :
            case Feather    :
            case Fur        :
            case Hair       :
            case Flesh      :
            case Rope       :
            case Cloth      :
            case Hide       :
            case SoftLeather:
            case HardLeather:
            case Plant      :
            case Claw       :
            case Scale      :
            case Horn       : { return 0; }
            case Bone       : { return 1; }
            case Tooth      : { return 0; }
            case Wood       : { return 0; }
            case Stone      : { return 2; }
            case Obsidian   : { return 3; }
            case Tin        : { return 4; }
            case Bronze     : { return 5; }
            case Iron       : { return 6; }
            case Steel      : { return 7; }
            case Jade       : { return 8; }
            case Amethyst   : { return 9; }
            case Emerald    : { return 10; }
            case Pearl      : { return 11; }
            case Ruby       : { return 12; }
            case Lazuli     : { return 13; }
            case Silver     : { return 14; }
            case Sapphire   : { return 15; }
            case Gold       : { return 16; }
            case Platinum   : { return 17; }
            case Diamond    : { return 18; }
            case Count      :
            default         :
            {
                std::ostringstream ss;
                ss << "game::item::material::Bonus(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool material::IsMagical(const material::Enum PRI, const material::Enum SEC)
    {
        return ContainsSpirit(PRI, SEC);
    }


    bool material::IsLiquid(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Acid)  ||
                (MATERIAL == material::Blood) ||
                (MATERIAL == material::Water));
    }


    bool material::IsSolid(const material::Enum MATERIAL)
    {
        return ((IsLiquid(MATERIAL) == false) &&
                (MATERIAL != material::Gas)   &&
                (MATERIAL != material::Spirit));
    }


    bool material::IsBendy(const material::Enum MATERIAL)
    {
        return ((IsSolid(MATERIAL) == false)        ||
                (MATERIAL == material::SoftLeather) ||
                (MATERIAL == material::Dirt)        ||
                (MATERIAL == material::Plant)       ||
                (MATERIAL == material::Flesh)       ||
                (MATERIAL == material::Hide)        ||
                (MATERIAL == material::Feather)     ||
                (MATERIAL == material::Fur)         ||
                (MATERIAL == material::Hair)        ||
                (MATERIAL == material::Paper)       ||
                (MATERIAL == material::Rope)        ||
                (MATERIAL == material::Cloth));
    }


    bool material::IsRigid(const material::Enum MATERIAL)
    {
        return ! IsBendy(MATERIAL);
    }


    bool material::ContainsSpirit(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Spirit) || (SEC == material::Spirit));
    }


    bool material::IsBloody(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Blood) || (SEC == material::Blood));
    }


    bool material::ContainsFlesh(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Flesh) || (SEC == material::Flesh));
    }


    float material::FireDamageRatio(const material::Enum PRI, const material::Enum SEC)
    {
        float fireDamageRatio(0.0f);
        switch (PRI)
        {
            case material::Nothing    : { break; }
            case material::Wood       : { fireDamageRatio = 0.5f;   break; }
            case material::HardLeather: { fireDamageRatio = 0.7f;   break; }
            case material::SoftLeather: { fireDamageRatio = 0.75f;  break; }
            case material::Bone       : { fireDamageRatio = 0.2f;   break; }
            case material::Water      : { fireDamageRatio = 0.01f;  break; }
            case material::Dirt       : { fireDamageRatio = 0.01f;  break; }
            case material::Acid       : { fireDamageRatio = 0.01f;  break; }
            case material::Stone      : { fireDamageRatio = 0.001f; break; }
            case material::Plant      : { fireDamageRatio = 0.85f;  break; }
            case material::Flesh      : { fireDamageRatio = 0.9f;   break; }
            case material::Hide       : { fireDamageRatio = 0.77f;  break; }
            case material::Feather    : { fireDamageRatio = 1.0f;   break; }
            case material::Fur        : { fireDamageRatio = 1.0f;   break; }
            case material::Hair       : { fireDamageRatio = 2.0f;   break; }
            case material::Scale      : { fireDamageRatio = 0.1f;   break; }
            case material::Obsidian   : { fireDamageRatio = 0.0f;   break; }
            case material::Jade       : { fireDamageRatio = 0.0f;   break; }
            case material::Amethyst   : { fireDamageRatio = 0.0f;   break; }
            case material::Emerald    : { fireDamageRatio = 0.0f;   break; }
            case material::Pearl      : { fireDamageRatio = 0.01f;  break; }
            case material::Ruby       : { fireDamageRatio = 0.0f;   break; }
            case material::Lazuli     : { fireDamageRatio = 0.0f;   break; }
            case material::Sapphire   : { fireDamageRatio = 0.0f;   break; }
            case material::Diamond    : { fireDamageRatio = 0.0f;   break; }
            case material::Glass      : { fireDamageRatio = 0.9f;   break; }
            case material::Gas        : { fireDamageRatio = 0.0f;   break; }
            case material::Blood      : { fireDamageRatio = 0.2f;   break; }
            case material::Paper      : { fireDamageRatio = 0.99f;  break; }
            case material::Rope       : { fireDamageRatio = 0.85f;  break; }
            case material::Cloth      : { fireDamageRatio = 0.85f;  break; }
            case material::Claw       : { fireDamageRatio = 0.1f;   break; }
            case material::Horn       : { fireDamageRatio = 0.1f;   break; }
            case material::Tooth      : { fireDamageRatio = 0.05f;  break; }
            case material::Spirit     : { fireDamageRatio = 0.0f;   break; }
            case material::Gold       : { fireDamageRatio = 0.01f;  break; }
            case material::Iron       : { fireDamageRatio = 0.01f;  break; }
            case material::Tin        : { fireDamageRatio = 0.01f;  break; }
            case material::Bronze     : { fireDamageRatio = 0.01f;  break; }
            case material::Silver     : { fireDamageRatio = 0.01f;  break; }
            case material::Steel      : { fireDamageRatio = 0.01f;  break; }
            case material::Platinum   : { fireDamageRatio = 0.01f;  break; }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::FireDamageRatio(material_pri=" << PRI
                    << ", material_sec=...) -but that material_pri type is unknown.";

                throw std::range_error(ss.str());
            }
        }
        switch (SEC)
        {
            case material::Nothing    : { break; }
            case material::Wood       : { fireDamageRatio += 0.2f;   break; }
            case material::HardLeather: { fireDamageRatio += 0.1f;   break; }
            case material::SoftLeather: { fireDamageRatio += 0.1f;   break; }
            case material::Bone       : { fireDamageRatio += 0.05f;  break; }
            case material::Water      : { fireDamageRatio -= 0.75f;  break; }
            case material::Dirt       : { fireDamageRatio -= 0.6f;   break; }
            case material::Acid       : { fireDamageRatio -= 0.75f;  break; }
            case material::Stone      : { fireDamageRatio -= 0.5f;   break; }
            case material::Plant      : { fireDamageRatio += 0.4f;   break; }
            case material::Flesh      : { fireDamageRatio += 0.4f;   break; }
            case material::Hide       : { fireDamageRatio += 0.1f;   break; }
            case material::Feather    : { fireDamageRatio += 0.2f;   break; }
            case material::Fur        : { fireDamageRatio += 0.3f;   break; }
            case material::Hair       : { fireDamageRatio += 0.5f;   break; }
            case material::Scale      : { fireDamageRatio -= 0.75f;  break; }
            case material::Obsidian   : { fireDamageRatio -= 0.5f;   break; }
            case material::Jade       : { fireDamageRatio -= 0.5f;   break; }
            case material::Amethyst   : { fireDamageRatio -= 0.5f;   break; }
            case material::Emerald    : { fireDamageRatio -= 0.5f;   break; }
            case material::Pearl      : { fireDamageRatio -= 0.5f;   break; }
            case material::Ruby       : { fireDamageRatio -= 0.5f;   break; }
            case material::Lazuli     : { fireDamageRatio -= 0.5f;   break; }
            case material::Sapphire   : { fireDamageRatio -= 0.5f;   break; }
            case material::Diamond    : { fireDamageRatio -= 0.5f;   break; }
            case material::Glass      : { break; }
            case material::Gas        : { break; }
            case material::Blood      : { fireDamageRatio -= 0.75f;  break; }
            case material::Paper      : { fireDamageRatio += 0.5f;   break; }
            case material::Rope       : { fireDamageRatio += 0.25f;  break; }
            case material::Cloth      : { fireDamageRatio += 0.25f;  break; }
            case material::Claw       : { fireDamageRatio -= 0.1f;   break; }
            case material::Horn       : { fireDamageRatio -= 0.1f;   break; }
            case material::Tooth      : { fireDamageRatio -= 0.1f;   break; }
            case material::Spirit     : { break; }
            case material::Gold       : { fireDamageRatio -= 0.5f;   break; }
            case material::Iron       : { fireDamageRatio -= 0.5f;   break; }
            case material::Tin        : { fireDamageRatio -= 0.5f;   break; }
            case material::Bronze     : { fireDamageRatio -= 0.5f;   break; }
            case material::Silver     : { fireDamageRatio -= 0.5f;   break; }
            case material::Steel      : { fireDamageRatio -= 0.5f;   break; }
            case material::Platinum   : { fireDamageRatio -= 0.5f;   break; }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::FireDamageRatio(material_pri=" << material::ToString(PRI)
                    << ", material_sec=" << SEC << " -but that material_sec type is unknown.";

                throw std::range_error(ss.str());
            }
        }

        if (fireDamageRatio < 0.0f)
            fireDamageRatio = 0.0f;

        if (fireDamageRatio > 1.0f)
            fireDamageRatio = 1.0f;

        return fireDamageRatio;
    }


    bool material::IsMetal(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Tin)    ||
                (MATERIAL == material::Gold)   ||
                (MATERIAL == material::Iron)   ||
                (MATERIAL == material::Bronze) ||
                (MATERIAL == material::Silver) ||
                (MATERIAL == material::Steel)  ||
                (MATERIAL == material::Platinum));
    }


    bool material::ContainsMetal(const material::Enum PRI, const material::Enum SEC)
    {
        return IsMetal(PRI) || IsMetal(SEC);
    }


    bool material::IsStone(const material::Enum PRI)
    {
        return ((PRI == material::Stone)    ||
                (PRI == material::Jade)     ||
                (PRI == material::Obsidian) ||
                (PRI == material::Amethyst) ||
                (PRI == material::Emerald)  ||
                (PRI == material::Ruby)     ||
                (PRI == material::Lazuli)   ||
                (PRI == material::Sapphire));
    }


    bool material::IsPrecious(const material::Enum MATERIAL)
    {
        return (IsJewel(MATERIAL)                ||
                (MATERIAL == material::Silver)   ||
                (MATERIAL == material::Gold)     ||
                (MATERIAL == material::Platinum));
    }


    bool material::ContiansPrecious(const material::Enum PRI, const material::Enum SEC)
    {
        return IsPrecious(PRI) || IsPrecious(SEC);
    }


    bool material::IsJewel(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Amethyst) ||
                (MATERIAL == material::Jade)     ||
                (MATERIAL == material::Emerald)  ||
                (MATERIAL == material::Ruby)     ||
                (MATERIAL == material::Sapphire) ||
                (MATERIAL == material::Pearl)    ||
                (MATERIAL == material::Lazuli)   ||
                (MATERIAL == material::Diamond));
    }


    bool material::ContainsJewel(const material::Enum PRI, const material::Enum SEC)
    {
        return IsJewel(PRI) || IsJewel(SEC);
    }


    const std::string weapon_type::ToString(const item::weapon_type::Enum E,
                                            const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if (E == weapon_type::NotAWeapon)
        {
            ss << "not a weapon";
        }
        else
        {
            if (E & weapon_type::Bladed)      ss << "bladed";
            if (E & weapon_type::Melee)       ss << ((ss.str().empty()) ? "" : "/") << "melee";
            if (E & weapon_type::Projectile)  ss << ((ss.str().empty()) ? "" : "/") << "projectile";
            if (E & weapon_type::Pointed)     ss << ((ss.str().empty()) ? "" : "/") << "pointed";
            if (E & weapon_type::Claws)       ss << ((ss.str().empty()) ? "" : "/") << "claws";
            if (E & weapon_type::Bite)        ss << ((ss.str().empty()) ? "" : "/") << "bite";
            if (E & weapon_type::Sword)       ss << ((ss.str().empty()) ? "" : "/") << "sword";
            if (E & weapon_type::Axe)         ss << ((ss.str().empty()) ? "" : "/") << "axe";
            if (E & weapon_type::Whip)        ss << ((ss.str().empty()) ? "" : "/") << "whip";
            if (E & weapon_type::Blowpipe)    ss << ((ss.str().empty()) ? "" : "/") << "blowpipe";
            if (E & weapon_type::Bow)         ss << ((ss.str().empty()) ? "" : "/") << "bow";
            if (E & weapon_type::Crossbow)    ss << ((ss.str().empty()) ? "" : "/") << "crossbow";
            if (E & weapon_type::Spear)       ss << ((ss.str().empty()) ? "" : "/") << "spear";
            if (E & weapon_type::Knife)       ss << ((ss.str().empty()) ? "" : "/") << "knife";
            if (E & weapon_type::Club)        ss << ((ss.str().empty()) ? "" : "/") << "club";
            if (E & weapon_type::Staff)       ss << ((ss.str().empty()) ? "" : "/") << "staff";
            if (E & weapon_type::Sling)       ss << ((ss.str().empty()) ? "" : "/") << "sling";
            if (E & weapon_type::BladedStaff) ss << ((ss.str().empty()) ? "" : "/") << "bladed staff";
            if (E & weapon_type::Fists)       ss << ((ss.str().empty()) ? "" : "/") << "fists";
            if (E & weapon_type::Tendrils)    ss << ((ss.str().empty()) ? "" : "/") << "tendrils";
            if (E & weapon_type::Breath)      ss << ((ss.str().empty()) ? "" : "/") << "breath";
        }

        if ((ss.str().empty()) || (ss.str() == "("))
        {
            std::ostringstream ssErr;
            ssErr << "game::item::weapon_type::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
            ss << ")";

        return ss.str();
    }

    weapon_type::Enum weapon_type::FromString(const std::string & S)
    {
        if      (S == "Melee")      return Melee;
        else if (S == "Projectile") return Projectile;
        else if (S == "Bladed")     return Bladed;
        else if (S == "Pointed")    return Pointed;
        else if (S == "Claws")      return Claws;
        else if (S == "Bite")       return Bite;
        else if (S == "Sword")      return Sword;
        else if (S == "Axe")        return Axe;
        else if (S == "Whip")       return Whip;
        else if (S == "Blowpipe")   return Blowpipe;
        else if (S == "Bow")        return Bow;
        else if (S == "Crossbow")   return Crossbow;
        else if (S == "Spear")      return Spear;
        else if (S == "Knife")      return Knife;
        else if (S == "Club")       return Club;
        else if (S == "Staff")      return Staff;
        else if (S == "Sling")      return Sling;
        else if (S == "BladedStaff")return BladedStaff;
        else if (S == "Fists")      return Fists;
        else if (S == "Tendrils")   return Tendrils;
        else if (S == "Breath")     return Breath;
        else
        {
            return NotAWeapon;
        }
    }


    const std::string armor_type::ToString(const item::armor_type::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if (E == armor_type::NotArmor)
        {
            ss << "not armor";
        }
        else
        {
            if (E & armor_type::Sheild)      ss << "sheild";
            if (E & armor_type::Helm)        ss << ((ss.str().empty()) ? "" : "/") << "helm";
            if (E & armor_type::Gauntlets)   ss << ((ss.str().empty()) ? "" : "/") << "gauntlets";
            if (E & armor_type::Pants)       ss << ((ss.str().empty()) ? "" : "/") << "pants";
            if (E & armor_type::Boots)       ss << ((ss.str().empty()) ? "" : "/") << "boots";
            if (E & armor_type::Shirt)       ss << ((ss.str().empty()) ? "" : "/") << "shirt";
            if (E & armor_type::Bracer)      ss << ((ss.str().empty()) ? "" : "/") << "bracer";
            if (E & armor_type::Aventail)    ss << ((ss.str().empty()) ? "" : "/") << "aventail";
            if (E & armor_type::Skin)        ss << ((ss.str().empty()) ? "" : "/") << "skin";
            if (E & armor_type::Covering)    ss << ((ss.str().empty()) ? "" : "/") << "covering";
        }

        if ((ss.str().empty()) || (ss.str() == "("))
        {
            std::ostringstream ssErr;
            ssErr << "game::item::armor_type::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
            ss << ")";

        return ss.str();
    }

}
}
