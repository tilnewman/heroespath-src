//
// turn-action-info.hpp
//
#include "turn-action-info.hpp"
#include <tuple>


namespace heroespath
{
namespace combat
{

    TurnActionInfo::TurnActionInfo(const TurnAction::Enum         ACTION,
                                   const creature::CreaturePtr_t TARGET_PTR,
                                   const spell::SpellPtr_t       SPELL_PTR)
    :
        actionType_(ACTION),
        targetPtr_ (TARGET_PTR),
        spellPtr_  (SPELL_PTR)
    {}


    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return std::tie(L.actionType_, L.targetPtr_, L.spellPtr_)
               <
               std::tie(R.actionType_, R.targetPtr_, R.spellPtr_);
    }


    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return std::tie(L.actionType_, L.targetPtr_, L.spellPtr_)
               ==
               std::tie(R.actionType_, R.targetPtr_, R.spellPtr_);
    }

}
}
