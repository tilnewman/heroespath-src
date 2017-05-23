#ifndef GAME_CREATURE_BODYTYPE_INCLUDED
#define GAME_CREATURE_BODYTYPE_INCLUDED
//
// body-type.hpp
//  Code that defines the body type of all creatures.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/item/item-type-enum.hpp"//for material

#include <cstddef> //for std::size_t
#include <string>


namespace game
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

        static inline const BodyType Make_Humanoid( const bool HAS_FANGS = false,
                                                    const bool HAS_CLAWS = false,
                                                    const bool HAS_TAIL  = false,
                                                    const bool HAS_HORNS = false)
        {
            return BodyType(item::material::Flesh, item::material::Nothing, 1, 2, 2, 2, HAS_FANGS, HAS_CLAWS, false, true, HAS_TAIL, false, HAS_HORNS);
        }

        static inline const BodyType Make_Wolfen() { return BodyType(item::material::Flesh, item::material::Fur, 1, 0, 4, 2, true, true, false, false, true); }
        static inline const BodyType Make_Dragon() { return BodyType(item::material::Scale, item::material::Scale, 1, 0, 4, 2, true, true, true, false, true, true, true, 0, true); }
        static inline const BodyType Make_Pixie()  { return BodyType(item::material::Flesh, item::material::Nothing, 1, 2, 2, 2, false, false, true); }

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

    inline bool operator!=(const BodyType & L, const BodyType & R) { return ! (L == R); }

}
}
#endif //GAME_CREATURE_BODYTYPE_INCLUDED
