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
            if (E & category::Weapon)    ss << "weapon";
            if (E & category::Armor)     ss << ((ss.str().empty()) ? "" : ", ") << "armor";
            if (E & category::Useable)   ss << ((ss.str().empty()) ? "" : ", ") << "useable";
            if (E & category::Equippable)ss << ((ss.str().empty()) ? "" : ", ") << "equippable";
            if (E & category::BodyPart)  ss << ((ss.str().empty()) ? "" : ", ") << "bodypart";
            if (E & category::Wearable)  ss << ((ss.str().empty()) ? "" : ", ") << "wearable";
            if (E & category::OneHanded) ss << ((ss.str().empty()) ? "" : ", ") << "one-handed";
            if (E & category::TwoHanded) ss << ((ss.str().empty()) ? "" : ", ") << "two-handed";
            if (E & category::QuestItem) ss << ((ss.str().empty()) ? "" : ", ") << "quest item";
            if (E & category::Edible)    ss << ((ss.str().empty()) ? "" : ", ") << "edible";
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
            case NotMisc          : { return "NotMisc"; }
            case Amulet           : { return "Amulet"; }
            case Bag              : { return "Bag"; }
            case Pendant          : { return "Pendant"; }
            case Medallion        : { return "Medallion"; }
            case Necklas          : { return "Necklas"; }
            case Book             : { return "Book"; }
            case Tome             : { return "Tome"; }
            case Charm            : { return "Charm"; }
            case Cape             : { return "Cape"; }
            case Cloak            : { return "Cloak"; }
            case Robe             : { return "Robe"; }
            case Crown            : { return "Crown"; }
            case Doll             : { return "Doll"; }
            case Drink            : { return "Drink"; }
            case Potion           : { return "Potion"; }
            case Goblet           : { return "Goblet"; }
            case Herbs            : { return "Herbs"; }
            case Hourglass        : { return "Hourglass"; }
            case Key              : { return "Key"; }
            case Lantern          : { return "Lantern"; }
            case Torch            : { return "Torch"; }
            case Lockbox          : { return "Lockbox"; }
            case Chest            : { return "Chest"; }
            case LockPicks        : { return "LockPicks"; }
            case Mask             : { return "Mask"; }
            case Mirror           : { return "Mirror"; }
            case DrumLute         : { return "DrumLute"; }
            case Scroll           : { return "Scroll"; }
            case Orb              : { return "Orb"; }
            case Ring             : { return "Ring"; }
            case Skull            : { return "Skull"; }
            case Shard            : { return "Shard"; }
            case Salve            : { return "Salve"; }
            case Wand             : { return "Wand"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::misc_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
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
            case Feathers   : { return "Feathers"; }
            case Fur        : { return "Fur"; }
            case Hair       : { return "Hair"; }
            case Scale      : { return "Scale"; }
            case Obsidian   : { return "Obsidian"; }
            case Amethyst   : { return "Amethyst"; }
            case Emerald    : { return "Emerald"; }
            case Pearl      : { return "Pearl"; }
            case Ruby       : { return "Ruby"; }
            case Lapis      : { return "Lapis"; }
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
            case Ether      : { return "Ether"; }
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
            case Ether      :
            case Gas        :
            case Blood      :
            case Water      :
            case Dirt       :
            case Acid       :
            case Paper      :
            case Glass      :
            case Feathers   :
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
            case Tin        : { return 20; }
            case Bronze     : { return 21; }
            case Iron       : { return 22; }
            case Stone      : { return 23; }
            case Obsidian   : { return 24; }
            case Steel      : { return 25; }
            case Silver     : { return 26; }
            case Amethyst   : { return 27; }
            case Emerald    : { return 28; }
            case Pearl      : { return 29; }
            case Ruby       : { return 30; }
            case Lapis      : { return 31; }
            case Sapphire   : { return 32; }
            case Gold       : { return 35; }
            case Platinum   : { return 40; }
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
            case Cloth      : { return 0; }
            case SoftLeather: { return 5; }
            case HardLeather: { return 10; }
            case Bone       : { return 12; }
            case Wood       : { return 14; }
            case Tin        : { return 30; }
            case Bronze     : { return 31; }
            case Iron       : { return 35; }
            case Steel      : { return 40; }
            case Silver     : { return 500; }
            case Gold       : { return 2000; }
            case Platinum   : { return 10000; }
            case Stone      : { return 0; }
            case Obsidian   : { return 1000; }
            case Amethyst   : { return 2000; }
            case Emerald    : { return 3000; }
            case Pearl      : { return 8000; }
            case Ruby       : { return 4000; }
            case Lapis      : { return 5000; }
            case Sapphire   : { return 6000; }
            case Diamond    : { return 10000; }
            case Claw       : { break; }
            case Horn       : { break; }
            case Tooth      : { break; }
            case Ether      : { break; }
            case Glass      : { return 10; }
            case Gas        : { break; }
            case Blood      : { break; }
            case Paper      : { break; }
            case Rope       : { break; }
            case Plant      : { break; }
            case Flesh      : { break; }
            case Hide       : { return 7; }
            case Feathers   : { break; }
            case Fur        : { return 8; }
            case Hair       : { break; }
            case Scale      : { return 1000; }
            case Water      : { break; }
            case Dirt       : { break; }
            case Acid       : { return 100; }
            case Nothing    : { return 0; }
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "game::item::material::PriceAdjPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }

        return 0;
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
            case Ether      : { return 0.0f; }
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
            case Feathers   : { return 0.5f; }
            case Fur        : { return 0.75f; }
            case Hair       : { return 0.5f; }
            case Scale      : { return 0.9f; }
            case Glass      : { return 1.6f; }
            case Wood       : { return 1.85f; }
            case Dirt       : { return 1.9f; }
            case Obsidian   : { return 2.2f; }
            case Amethyst   : { return 2.1f; }
            case Emerald    : { return 2.15f; }
            case Pearl      : { return 1.2f; }
            case Ruby       : { return 2.2f; }
            case Lapis      : { return 2.5f; }
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
            case Ether      :
            case Gas        :
            case Blood      :
            case Water      :
            case Dirt       :
            case Acid       :
            case Paper      :
            case Glass      :
            case Feathers   :
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
            case Tin        : { return 2; }
            case Bronze     : { return 3; }
            case Iron       : { return 4; }
            case Stone      : { return 0; }
            case Obsidian   : { return 5; }
            case Steel      : { return 6; }
            case Silver     : { return 7; }
            case Amethyst   : { return 8; }
            case Emerald    : { return 9; }
            case Pearl      : { return 0; }
            case Ruby       : { return 10; }
            case Lapis      : { return 11; }
            case Sapphire   : { return 12; }
            case Gold       : { return 13; }
            case Platinum   : { return 14; }
            case Diamond    : { return 15; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::material::Bonus(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool material::IsMagical(const material::Enum PRI, const material::Enum SEC)
    {
        return ContainsEther(PRI, SEC);
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
                (MATERIAL != material::Ether));
    }


    bool material::IsBendy(const material::Enum MATERIAL)
    {
        return ((IsSolid(MATERIAL) == false)        ||
                (MATERIAL == material::SoftLeather) ||
                (MATERIAL == material::Dirt)        ||
                (MATERIAL == material::Plant)       ||
                (MATERIAL == material::Flesh)       ||
                (MATERIAL == material::Hide)        ||
                (MATERIAL == material::Feathers)    ||
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


    bool material::ContainsEther(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Ether) || (SEC == material::Ether));
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
            case material::Feathers   : { fireDamageRatio = 1.0f;   break; }
            case material::Fur        : { fireDamageRatio = 1.0f;   break; }
            case material::Hair       : { fireDamageRatio = 2.0f;   break; }
            case material::Scale      : { fireDamageRatio = 0.1f;   break; }
            case material::Obsidian   : { fireDamageRatio = 0.0f;   break; }
            case material::Amethyst   : { fireDamageRatio = 0.0f;   break; }
            case material::Emerald    : { fireDamageRatio = 0.0f;   break; }
            case material::Pearl      : { fireDamageRatio = 0.01f;  break; }
            case material::Ruby       : { fireDamageRatio = 0.0f;   break; }
            case material::Lapis      : { fireDamageRatio = 0.0f;   break; }
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
            case material::Ether      : { fireDamageRatio = 0.0f;   break; }
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
            case material::Feathers   : { fireDamageRatio += 0.2f;   break; }
            case material::Fur        : { fireDamageRatio += 0.3f;   break; }
            case material::Hair       : { fireDamageRatio += 0.5f;   break; }
            case material::Scale      : { fireDamageRatio -= 0.75f;  break; }
            case material::Obsidian   : { fireDamageRatio -= 0.5f;   break; }
            case material::Amethyst   : { fireDamageRatio -= 0.5f;   break; }
            case material::Emerald    : { fireDamageRatio -= 0.5f;   break; }
            case material::Pearl      : { fireDamageRatio -= 0.5f;   break; }
            case material::Ruby       : { fireDamageRatio -= 0.5f;   break; }
            case material::Lapis      : { fireDamageRatio -= 0.5f;   break; }
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
            case material::Ether      : { break; }
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
                (PRI == material::Obsidian) ||
                (PRI == material::Amethyst) ||
                (PRI == material::Emerald)  ||
                (PRI == material::Ruby)     ||
                (PRI == material::Lapis)    ||
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
                (MATERIAL == material::Emerald)  ||
                (MATERIAL == material::Ruby)     ||
                (MATERIAL == material::Sapphire) ||
                (MATERIAL == material::Pearl)    ||
                (MATERIAL == material::Lapis)    ||
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
