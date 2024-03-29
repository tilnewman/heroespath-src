// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_BODYTYPE_INCLUDED
#define HEROESPATH_CREATURE_BODYTYPE_INCLUDED
//
// body-type.hpp
//  Code that defines the body type of all creatures.
//
#include "misc/boost-serialize-includes.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "item/item-type-enum.hpp" //for material

#include <cstddef> //for std::size_t
#include <string>

namespace heroespath
{
namespace creature
{

    // Responsible for wrapping all the information about a creature's body.
    class BodyType
    {
        explicit BodyType(
            const std::size_t NUM_HEADS = 1,
            const std::size_t NUM_ARMS = 2,
            const std::size_t NUM_LEGS = 2,
            const std::size_t NUM_EYES = 2,
            const bool HAS_FANGS = false,
            const bool HAS_CLAWS = false,
            const bool HAS_WINGS = false,
            const bool HAS_FINGERS = false,
            const bool HAS_TAIL = false,
            const bool HAS_SPIKES = false,
            const bool HAS_HORNS = false,
            const std::size_t NUM_TENTACLES = 0,
            const bool HAS_BREATH = false);

    public:
        auto NumHeads() const { return num_heads_; }
        auto NumArms() const { return num_arms_; }
        auto NumLegs() const { return num_legs_; }
        auto NumEyes() const { return num_eyes_; }
        auto NumTentacles() const { return num_tentacles_; }

        auto HasHead() const { return (num_heads_ > 0); }
        auto HasArms() const { return (num_arms_ > 0); }
        auto HasLegs() const { return (num_legs_ > 0); }
        auto HasFangs() const { return has_fangs_; }
        auto HasClaws() const { return has_claws_; }
        auto HasWings() const { return has_wings_; }
        auto HasEyes() const { return (num_eyes_ > 0); }
        auto HasFingers() const { return has_fingers_; }
        auto HasTail() const { return has_tail_; }
        auto HasTentacles() const { return (num_tentacles_ > 0); }
        auto HasSpikes() const { return has_spikes_; }
        auto HasHorns() const { return has_horns_; }
        auto HasBreath() const { return has_breath_; }
        auto HasBite() const { return HasFangs(); }

        auto IsBiped() const { return (2 == num_legs_); }
        auto IsQuadruped() const { return (4 == num_legs_); }

        auto IsHumanoid() const
        {
            return (IsBiped() && (num_heads_ > 0) && (num_arms_ >= 2) && HasFingers());
        }

        auto IsSerpentine() const
        {
            return ((false == HasArms()) && (false == HasLegs()) && HasTail());
        }

        const std::string ToString() const;

        static const BodyType Make_Humanoid(
            const bool HAS_FANGS = false,
            const bool HAS_CLAWS = false,
            const bool HAS_TAIL = false,
            const bool HAS_HORNS = false);

        static const BodyType Make_Wolfen(const role::Enum ROLE_ENUM = role::Wolfen);
        static const BodyType Make_Dragon(const bool HAS_BREATH);
        static const BodyType Make_Pixie();

        static const BodyType
            Make_FromRaceAndRole(const creature::race::Enum RACE, const creature::role::Enum ROLE);

        friend bool operator<(const BodyType & L, const BodyType & R);
        friend bool operator==(const BodyType & L, const BodyType & R);

    private:
        bool has_wings_;
        bool has_fangs_;
        bool has_claws_;
        bool has_fingers_;
        bool has_tail_;
        bool has_spikes_;
        bool has_horns_;
        bool has_breath_;
        std::size_t num_arms_;
        std::size_t num_legs_;
        std::size_t num_heads_;
        std::size_t num_eyes_;
        std::size_t num_tentacles_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
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
            ar & num_tentacles_;
        }
    };

    bool operator<(const BodyType & L, const BodyType & R);

    bool operator==(const BodyType & L, const BodyType & R);

    inline bool operator!=(const BodyType & L, const BodyType & R) { return !(L == R); }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_BODYTYPE_INCLUDED
