#ifndef GAME_COMBAT_TURNACTIONINFO_HPP_INCLUDED
#define GAME_COMBAT_TURNACTIONINFO_HPP_INCLUDED
//
// turn-action-info.hpp
//
#include "game/combat/turn-action-enum.hpp"
#include "game/combat/strategy-enums.hpp"

#include <vector>


namespace game
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t  = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace combat
{

    //Everything required to describe what an enemy did on its turn
    class TurnActionInfo
    {
    public:
        TurnActionInfo(const TurnAction::Enum        ACTION = TurnAction::Nothing,
                       const creature::CreaturePtr_t TARGET_PTR = nullptr);

        TurnActionInfo(const spell::SpellPtr_t        SPELL_PTR,
                       const creature::CreaturePVec_t TARGET_PVEC);

        TurnActionInfo(const TurnActionInfo &);
        TurnActionInfo & operator=(const TurnActionInfo &);

        inline TurnAction::Enum         Action() const  { return actionType_; }
        inline spell::SpellPtr_t        Spell() const   { return spellPtr_; }
        inline creature::CreaturePVec_t Targets() const { return targetsPVec_; }

        creature::CreaturePtr_t Target() const;

        friend bool operator<(const TurnActionInfo & A, const TurnActionInfo & B);
        friend bool operator==(const TurnActionInfo & A, const TurnActionInfo & B);

    private:
        TurnAction::Enum         actionType_;
        creature::CreaturePVec_t targetsPVec_;
        spell::SpellPtr_t        spellPtr_;
    };


    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R);

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R);

    inline bool operator!=(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return ! (L == R);
    }


}
}

#endif //GAME_COMBAT_TURNACTIONINFO_HPP_INCLUDED
