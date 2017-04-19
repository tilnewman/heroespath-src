#ifndef HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
//
// turn-action-info.hpp
//
#include "heroespath/combat/turn-action-enum.hpp"
#include "heroespath/combat/strategy-enums.hpp"


namespace heroespath
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
    using CreaturePtr_t = Creature *;
    using CreatureCPtr_t = const Creature *;
    using CreaturePtrC_t = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
}
namespace combat
{

    //Everything required to describe what an enemy did on its turn
    class TurnActionInfo
    {
    public:
        TurnActionInfo(const TurnAction::Enum        ACTION     = TurnAction::Count,
                       const creature::CreaturePtr_t TARGET_PTR = nullptr,
                       const spell::SpellPtr_t       SPELL_PTR  = nullptr);

        inline TurnAction::Enum         Action() const  { return actionType_; }
        inline creature::CreaturePtr_t  Target() const  { return targetPtr_; }
        inline spell::SpellPtr_t        Spell() const   { return spellPtr_; }

        friend bool operator<(const TurnActionInfo & A, const TurnActionInfo & B);
        friend bool operator==(const TurnActionInfo & A, const TurnActionInfo & B);

    private:
        TurnAction::Enum        actionType_;
        creature::CreaturePtr_t targetPtr_;
        spell::SpellPtr_t       spellPtr_;
    };


    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R);

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R);

    inline bool operator!=(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return ! (L == R);
    }


}
}

#endif //HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
