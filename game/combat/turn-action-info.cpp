//
// turn-action-info.hpp
//
#include "turn-action-info.hpp"
#include <tuple>


namespace game
{
namespace combat
{

    TurnActionInfo::TurnActionInfo(const TurnAction::Enum         ACTION,
                                   const creature::CreaturePtr_t TARGET_PTR)
    :
        actionType_ (ACTION),
        targetsPVec_(1, TARGET_PTR),
        spellPtr_   (nullptr)
    {}


    TurnActionInfo::TurnActionInfo(const spell::SpellPtr_t        SPELL_PTR,
                                   const creature::CreaturePVec_t TARGET_PVEC)
    :
        actionType_ (TurnAction::Cast),
        targetsPVec_(TARGET_PVEC),
        spellPtr_   (SPELL_PTR)
    {}


    creature::CreaturePtr_t TurnActionInfo::Target() const
    {
        if (targetsPVec_.empty())
        {
            return nullptr;
        }
        else
        {
            return targetsPVec_[0];
        }
    }


    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return std::tie(L.actionType_, L.targetsPVec_, L.spellPtr_)
               <
               std::tie(R.actionType_, R.targetsPVec_, R.spellPtr_);
    }


    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        return std::tie(L.actionType_, L.targetsPVec_, L.spellPtr_)
               ==
               std::tie(R.actionType_, R.targetsPVec_, R.spellPtr_);
    }

}
}
