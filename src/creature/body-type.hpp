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
#ifndef HEROESPATH_CREATURE_BODYTYPE_INCLUDED
#define HEROESPATH_CREATURE_BODYTYPE_INCLUDED
//
// body-type.hpp
//  Code that defines the body type of all creatures.
//
#include "misc/boost-serialize-includes.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "item/item-type-enum.hpp"//for material

#include <cstddef> //for std::size_t
#include <string>


namespace heroespath
{
namespace creature
{

    //defines basic body composition
    class BodyType
    {
    public:
        explicit BodyType(const item::material::Enum SKIN_MATERIAL_INNER = item::material::Flesh,
                          const item::material::Enum SKIN_MATERIAL_OUTER = item::material::Nothing,
                          const std::size_t          NUM_HEADS           = 1,
                          const std::size_t          NUM_ARMS            = 2,
                          const std::size_t          NUM_LEGS            = 2,
                          const std::size_t          NUM_EYES            = 2,
                          const bool                 HAS_FANGS           = false,
                          const bool                 HAS_CLAWS           = false,
                          const bool                 HAS_WINGS           = false,
                          const bool                 HAS_FINGERS         = true,
                          const bool                 HAS_TAIL            = false,
                          const bool                 HAS_SPIKES          = false,
                          const bool                 HAS_HORNS           = false,
                          const std::size_t          NUM_TENDRILS        = 0,
                          const bool                 HAS_BREATH          = false);

        virtual ~BodyType();

        inline auto NumHeads() const        { return num_heads_; }
        inline auto NumArms() const         { return num_arms_; }
        inline auto NumLegs() const         { return num_legs_; }
        inline auto NumEyes() const         { return num_eyes_; }
        inline auto NumTendrils() const     { return num_tendrils_; }

        inline auto HasHead() const         { return (num_heads_ > 0); }
        inline auto HasArms() const         { return (num_arms_ > 0); }
        inline auto HasLegs() const         { return (num_legs_ > 0); }
        inline auto HasFangs() const        { return has_fangs_; }
        inline auto HasClaws() const        { return has_claws_; }
        inline auto HasWings() const        { return has_wings_; }
        inline auto HasEyes() const         { return (num_eyes_ > 0); }
        inline auto HasFingers() const      { return has_fingers_; }
        inline auto HasTail() const         { return has_tail_; }
        inline auto HasTendrils() const     { return (num_tendrils_ > 0); }
        inline auto HasSpikes() const       { return has_spikes_; }
        inline auto HasHorns() const        { return has_horns_; }
        inline auto HasBreath() const       { return has_breath_; }
        inline auto HasBite() const         { return HasFangs(); }

        inline auto IsBiped() const         { return (2 == num_legs_); }
        inline auto IsQuadruped() const     { return (4 == num_legs_); }
        inline auto IsHumanoid() const      { return (IsBiped() && (num_heads_ > 0) && (num_arms_ >= 2) && HasFingers()); }
        inline auto IsSerpentine() const    { return ((false == HasArms()) && (false == HasLegs()) && HasTail()); }

        inline auto SkinMaterialInner() const { return skinMaterialInner_; }
        inline auto SKinMaterialOuter() const { return skinMaterialOuter_; }

        const std::string ToString() const;

        static const BodyType Make_Humanoid(const bool HAS_FANGS = false,
                                            const bool HAS_CLAWS = false,
                                            const bool HAS_TAIL  = false,
                                            const bool HAS_HORNS = false);

        static const BodyType Make_Wolfen(const role::Enum ROLE_ENUM = role::Wolfen);
        static const BodyType Make_Dragon();
        static const BodyType Make_Pixie();

        static const BodyType Make_FromRaceAndRole(const creature::race::Enum RACE, const creature::role::Enum ROLE);

        friend bool operator<(const BodyType & L, const BodyType & R);
        friend bool operator==(const BodyType & L, const BodyType & R);

    private:
        bool   has_wings_;
        bool   has_fangs_;
        bool   has_claws_;
        bool   has_fingers_;
        bool   has_tail_;
        bool   has_spikes_;
        bool   has_horns_;
        bool   has_breath_;
        std::size_t num_arms_;
        std::size_t num_legs_;
        std::size_t num_heads_;
        std::size_t num_eyes_;
        std::size_t num_tendrils_;
        item::material::Enum skinMaterialInner_;
        item::material::Enum skinMaterialOuter_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & has_wings_;
            ar & has_fangs_;
            ar & has_claws_;
            ar & has_fingers_;
            ar & has_spikes_;
            ar & has_horns_;
            ar & has_breath_;
            ar & num_arms_;
            ar & num_legs_;
            ar & num_heads_;
            ar & num_eyes_;
            ar & num_tendrils_;
            ar & skinMaterialInner_;
            ar & skinMaterialOuter_;
        }
    };

    bool operator<(const BodyType & L, const BodyType & R);

    bool operator==(const BodyType & L, const BodyType & R);

    inline bool operator!=(const BodyType & L, const BodyType & R)
    {
        return ! (L == R);
    }

}
}
#endif //HEROESPATH_CREATURE_BODYTYPE_INCLUDED
