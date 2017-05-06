#ifndef GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
#define GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
//
// combat-restore-info.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <vector>
#include <map>


namespace game
{
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t  = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace combat
{

    //forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = CombatDisplay *;
    using CombatDisplayPtrC_t = CombatDisplay * const;


    using CreatureBlockingPosMap_t = std::map<creature::CreaturePtr_t, int>;


    using CreatureLastCastSpellNumMap_t = std::map<creature::CreaturePtr_t, std::size_t>;


    //Everything required to save the state of combat so it can be restored later.
    class RestoreInfo
    {
    public:
        RestoreInfo();

        void PrepareForStageChange(const CombatDisplayPtrC_t);
        void Save(const CombatDisplayPtrC_t);
        void Restore(CombatDisplayPtrC_t);

        inline bool HasRestored() const             { return hasRestored_; }
        inline bool CanTurnAdvance() const          { return canTurnAdvance_; }
        inline void CanTurnAdvance(const bool B)    { canTurnAdvance_ = B; }

        inline std::size_t LastCastSpellNum(const creature::CreaturePtr_t CPTR)                 { return creatureLastCastSpellNumMap_[CPTR]; }
        inline void LastCastSpellNum(const creature::CreaturePtr_t CPTR, const std::size_t NUM) { creatureLastCastSpellNumMap_[CPTR] = NUM; }

    private:
        void CreatureHealthSave(const CombatDisplayPtrC_t);
        void CreatureHealthRestore(CombatDisplayPtrC_t);

        void FlyingCreaturesSave(const CombatDisplayPtrC_t);
        void FlyingCreaturesRestore(CombatDisplayPtrC_t);

        void CreaturePositionsSave(const CombatDisplayPtrC_t);
        void CreaturePositionsRestore(CombatDisplayPtrC_t);

    private:
        bool canTurnAdvance_;
        bool hasRestored_;
        creature::CreaturePVec_t creaturesFlyingPVec_;
        CreatureBlockingPosMap_t creatureBlockingPosMap_;
        CreatureLastCastSpellNumMap_t creatureLastCastSpellNumMap_;
    };

}
}

#endif //GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
