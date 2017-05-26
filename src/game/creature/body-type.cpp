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
// body-type.cpp
//
#include "body-type.hpp"
#include <exception>
#include <sstream>
#include <tuple>


namespace game
{
namespace creature
{

    BodyType::BodyType(const item::material::Enum SKIN_MATERIAL_INNER,
                       const item::material::Enum SKIN_MATERIAL_OUTER,
                       const std::size_t          NUM_HEADS,
                       const std::size_t          NUM_ARMS,
                       const std::size_t          NUM_LEGS,
                       const std::size_t          NUM_EYES,
                       const bool                 HAS_FANGS,
                       const bool                 HAS_CLAWS,
                       const bool                 HAS_WINGS,
                       const bool                 HAS_FINGERS,
                       const bool                 HAS_TAIL,
                       const bool                 HAS_SPIKES,
                       const bool                 HAS_HORNS,
                       const std::size_t          NUM_TENDRILS,
                       const bool                 HAS_BREATH)
    :
        has_wings_        (HAS_WINGS),
        has_fangs_        (HAS_FANGS),
        has_claws_        (HAS_CLAWS),
        has_fingers_      (HAS_FINGERS),
        has_tail_         (HAS_TAIL),
        has_spikes_       (HAS_SPIKES),
        has_horns_        (HAS_HORNS),
        has_breath_       (HAS_BREATH),
        num_arms_         (NUM_ARMS),
        num_legs_         (NUM_LEGS),
        num_heads_        (NUM_HEADS),
        num_eyes_         (NUM_EYES),
        num_tendrils_     (NUM_TENDRILS),
        skinMaterialInner_(SKIN_MATERIAL_INNER),
        skinMaterialOuter_(SKIN_MATERIAL_OUTER)
    {}


    BodyType::~BodyType()
    {}


    const std::string BodyType::ToString() const
    {
        std::ostringstream ss;
        ss << "A";

        if (0 == num_heads_)
            ss << " headless";
        else
        {
            if (num_heads_ > 1)
            {
                ss << num_heads_ << "-headed";

                if (0 == num_eyes_)
                    ss << " each with no eyes";
                else
                    if (num_eyes_ != 2)
                        ss << " each with" << num_eyes_ << " eyes";
            }
            else
            {
                if (0 == num_eyes_)
                    ss << "n eyeless";
                else
                    if (num_eyes_ != 2)
                        ss << " " << num_eyes_ << "-eyed";
            }
        }

        if (0 == num_arms_)
            ss << " armless";
        else
            if (2 != num_arms_)
                ss << " " << num_arms_ << "-armed";

        if (has_spikes_)
            ss << " spiked";

        if (has_fangs_)
            ss << " fanged";

        if (has_claws_)
            ss << " clawed";

        if (has_horns_)
            ss << " horned";

        if (has_breath_)
            ss << " breathes-weapon";

        if (0 == num_legs_)
        {
            ss << " legless";
        }
        else
        {
            if (IsBiped())
                ss << " biped";
            else
            {
                if (IsQuadruped())
                    ss << " quadruped";
                else
                    ss << " " << num_legs_ << "-legged creature";
            }
        }

        if (has_wings_)
            ss << " with wings";

        if (has_tail_)
            ss << " with a tail";

        if (num_tendrils_ > 0)
            ss << " with " << num_tendrils_ << " tendrils";

        if (skinMaterialInner_ == item::material::Ether)
            ss << " made of ghostly ether";
        else
        {
            ss << " skinned with " << item::material::ToString(skinMaterialInner_);
            if (item::material::Nothing != skinMaterialOuter_)
                ss << " and " << item::material::ToString(skinMaterialOuter_);
        }
        ss << ".";
            return ss.str();
    }


    const BodyType BodyType::Make_FromRaceAndRole(const creature::race::Enum RACE, const creature::role::Enum ROLE)
    {
        switch (RACE)
        {
            case creature::race::Enum::Human:
            case creature::race::Enum::Troll:
            case creature::race::Enum::Ogre:
            case creature::race::Enum::Halfling:
            case creature::race::Enum::Pug:
            case creature::race::Enum::Gnome:   { return Make_Humanoid(); }
            case creature::race::Enum::Pixie:   { return Make_Pixie(); }
            case creature::race::Enum::Lion:
            case creature::race::Enum::Wolfen:  { return Make_Wolfen(); }
            case creature::race::Enum::Dragon:  { return Make_Dragon(); }
            case creature::race::Enum::Goblin:
            case creature::race::Enum::Orc:     { return Make_Humanoid(true, true); }
            case creature::race::Enum::Newt:    { return Make_Humanoid(false, true); }
            case creature::race::Enum::Spider:  { return BodyType(item::material::Flesh, item::material::Nothing, 1, 0, 8, 6, true, false, false, false); }
            case creature::race::Enum::Bog:
            {
                if (ROLE == role::Spike)
                    return BodyType(item::material::Flesh, item::material::Nothing, 1, 2, 4, 2, false, true, false, false, true, true);

                if (ROLE == role::Tendrilus)
                    return BodyType(item::material::Flesh, item::material::Nothing, 1, 0, 4, 2, false, true, false, true, true, false, false, 4);

                if (ROLE == role::Whelp)
                    return BodyType(item::material::Flesh, item::material::Nothing, 1, 0, 4, 2, true, true, false, false, true, true);

                if (ROLE == role::Wing)
                    return BodyType(item::material::Flesh, item::material::Nothing, 1, 2, 2, 2, true, true, true, false, true, true, true);

                //Grunt
                return BodyType::Make_Humanoid(false, false, false);
            }
            case creature::race::Enum::CaveCrawler: { return BodyType(item::material::Flesh, item::material::Nothing, 1, 0, 6, 4, true, true, false, false, false, false); }
            case creature::race::Enum::Hydra:       { return BodyType(item::material::Flesh, item::material::Scale, 6, 0, 4, 12, true, true, true, false, true, false, false, true); }
            case creature::race::Enum::LizardWalker:{ return Make_Humanoid(true, true, true); }
            case creature::race::Enum::Minotaur:    { return Make_Humanoid(false, false, true, true); }
            case creature::race::Enum::Plant:
            {
                if (ROLE == role::Pod)
                    return BodyType(item::material::Plant, item::material::Nothing, 1, 0, 0, 1, true, true, false, false, false, true, false, 4);

                if (ROLE == role::Smasher)
                    return BodyType(item::material::Plant, item::material::Nothing, 1, 2, 2, 2, false, true, false, false, false, true, true);

                if (ROLE == role::Strangler)
                    return BodyType(item::material::Plant, item::material::Nothing, 1, 0, 2, 2, false, false, false, false, false, false, false, 2);

                if (ROLE == role::Tendrilus)
                    return BodyType(item::material::Plant, item::material::Nothing, 1, 2, 2, 0, true, false, false, false, false, true, true, 8);

                //Whelp
                return BodyType(item::material::Plant, item::material::Nothing, 1, 2, 2, 2, true, true, false, true, false, false, false);
            }
            case creature::race::Enum::Shade:   { return BodyType(item::material::Ether, item::material::Nothing, 1, 2, 0, 2, false, true, false, true, false, false, false); }
            case creature::race::Enum::Skeleton:
            {
                if (ROLE == role::Chieftain)
                    return BodyType(item::material::Bone, item::material::Nothing, 1, 2, 2, 2, false, true, false, true, false, false, true);

                if (ROLE == role::FourArmed)
                    return BodyType(item::material::Bone, item::material::Nothing, 1, 4, 2, 2, false, true, false, true, false, false, false);

                if (ROLE == role::Mountain)
                    return BodyType(item::material::Bone, item::material::Fur, 1, 2, 2, 2, false, true, false, true, false, false, false);

                //um...default skeleton
                return BodyType(item::material::Bone, item::material::Nothing, 1, 2, 2, 2, false, true, false, true, false, false, false);
            }
            case creature::race::Enum::Beetle:  { return BodyType(item::material::Scale, item::material::Nothing, 1, 0, 4, 4, true, true, false, false, false, false, false); }
            case creature::race::Enum::Boar:    { return BodyType(item::material::Hide, item::material::Nothing, 1, 0, 4, 2, true, false, false, false, true, false, false); }
            case creature::race::Enum::Demon:
            {
                if (ROLE == role::Skeleton)
                    return BodyType(item::material::Bone, item::material::Nothing, 1, 2, 2, 2, false, true, true, true, true, false, false);

                if (ROLE == role::Spike)
                    return BodyType(item::material::Scale, item::material::Nothing, 1, 2, 2, 2, false, true, false, true, true, true, true);

                if (ROLE == role::Strangler)
                    return BodyType(item::material::Hide, item::material::Nothing, 1, 2, 2, 2, true, true, false, true, true, false, false);

                if (ROLE == role::Whelp)
                    return BodyType(item::material::Flesh, item::material::Nothing, 1, 0, 8, 2, false, true, true, false, false, false, false);

                //Wing
                return BodyType(item::material::Hide, item::material::Nothing, 1, 2, 2, 2, true, true, true, false, true, true, true);
            }
            case creature::race::Enum::Griffin:
            {
                if (ROLE == role::Whelp)
                    return BodyType(item::material::Flesh, item::material::Feathers, 1, 0, 2, 2, true, true, true, false, true, false, true);

                return BodyType(item::material::Flesh, item::material::Feathers, 1, 0, 4, 2, true, true, true, false, true, false, true);
            }
            case creature::race::Enum::LionBoar:    { return BodyType(item::material::Hide,  item::material::Nothing, 1, 0, 4, 2, true, false, false, false, true, false, true); }
            case creature::race::Enum::Naga:        { return BodyType(item::material::Scale, item::material::Nothing, 1, 2, 0, 2, true, true,  false, true,  true, false, false); }
            case creature::race::Enum::Ramonaut:    { return BodyType(item::material::Hide,  item::material::Nothing, 1, 0, 4, 2, false, false, false, false, true, true, true); }
            case creature::race::Enum::Serpent:
            case creature::race::Enum::Cobra:       { return BodyType(item::material::Scale, item::material::Nothing, 1, 0, 0, 2, true, false, false, false, true, false, false);  }
            case creature::race::Enum::Werebear:    { return BodyType(item::material::Hide, item::material::Fur, 1, 2, 2, 2, true, true, false, true, false, true, false); }
            case creature::race::Enum::Wereboar:    { return BodyType(item::material::Hide, item::material::Fur, 1, 0, 4, 2, true, true, false, false, false, true, true); }
            case creature::race::Enum::Werecat:     { return BodyType(item::material::Hide, item::material::Fur, 1, 0, 4, 2, true, true, false, false, true, false, false); }
            case creature::race::Enum::Werewolf:    { return BodyType(item::material::Hide, item::material::Fur, 1, 2, 2, 2, true, true, false, true, true, false, false); }
            case creature::race::Enum::Wyvern:
            {
                BodyType wyvernBodyType( Make_Dragon() );
                wyvernBodyType.num_arms_ = 0;
                return wyvernBodyType;
            }
            case creature::race::Enum::Werebat:     { return BodyType(item::material::Hide, item::material::Nothing, 1, 2, 2, 2, true, true, true, false, false, false, false, 0); }
            case creature::race::Enum::Ghoul:
            {
                BodyType ghoulBodyType( Make_Humanoid(false, false, false, false) );

                if (ROLE == role::Spike)
                {
                    ghoulBodyType.has_claws_ = true;
                    ghoulBodyType.has_spikes_ = true;
                }
                else if (ROLE == role::Strangler)
                {
                    ghoulBodyType.has_claws_ = true;
                }

                return ghoulBodyType;
            }
            case creature::race::Enum::Giant:
            {
                BodyType giantBodyType( Make_Humanoid(false, false, false, false) );

                if (ROLE == role::Warlord)
                {
                    giantBodyType.has_claws_ = true;
                    giantBodyType.has_spikes_ = true;
                }
                else if ((ROLE == role::Mountain) || (ROLE == role::Strangler))
                {
                    giantBodyType.has_claws_ = true;
                }

                return giantBodyType;
            }
            case creature::race::Enum::Count:
            default: { break; }
        }

        std::ostringstream ss;
        ss << "game::creature::BodyType::Make_FromRace(race=" << RACE << ", role=" << ROLE << ") -these values failed to create a BodyType.";
        throw std::range_error(ss.str());
    }


    bool operator<(const BodyType & L, const BodyType & R)
    {
        return std::tie(L.has_wings_,
                        L.has_fangs_,
                        L.has_claws_,
                        L.has_fingers_,
                        L.has_tail_,
                        L.has_spikes_,
                        L.has_horns_,
                        L.has_breath_,
                        L.num_arms_,
                        L.num_legs_,
                        L.num_heads_,
                        L.num_eyes_,
                        L.num_tendrils_,
                        L.skinMaterialInner_,
                        L.skinMaterialOuter_)
            <
            std::tie(R.has_wings_,
                     R.has_fangs_,
                     R.has_claws_,
                     R.has_fingers_,
                     R.has_tail_,
                     R.has_spikes_,
                     R.has_horns_,
                     R.has_breath_,
                     R.num_arms_,
                     R.num_legs_,
                     R.num_heads_,
                     R.num_eyes_,
                     R.num_tendrils_,
                     R.skinMaterialInner_,
                     R.skinMaterialOuter_);
    }


    bool operator==(const BodyType & L, const BodyType & R)
    {
        return std::tie(L.has_wings_,
                        L.has_fangs_,
                        L.has_claws_,
                        L.has_fingers_,
                        L.has_tail_,
                        L.has_spikes_,
                        L.has_horns_,
                        L.has_breath_,
                        L.num_arms_,
                        L.num_legs_,
                        L.num_heads_,
                        L.num_eyes_,
                        L.num_tendrils_,
                        L.skinMaterialInner_,
                        L.skinMaterialOuter_)
            ==
            std::tie(R.has_wings_,
                     R.has_fangs_,
                     R.has_claws_,
                     R.has_fingers_,
                     R.has_tail_,
                     R.has_spikes_,
                     R.has_horns_,
                     R.has_breath_,
                     R.num_arms_,
                     R.num_legs_,
                     R.num_heads_,
                     R.num_eyes_,
                     R.num_tendrils_,
                     R.skinMaterialInner_,
                     R.skinMaterialOuter_);
    }

}
}
