#ifndef HEROESPATH_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
//
// combat-restore-info.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <vector>
#include <map>


namespace heroespath
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
    };

}
}

#endif //HEROESPATH_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
