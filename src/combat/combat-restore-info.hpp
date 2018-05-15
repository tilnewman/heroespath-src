// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
//
// combat-restore-info.hpp
//
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    using CreatureBlockingPosMap_t = misc::VectorMap<creature::CreaturePtr_t, int>;

    // Everything required to save the state of combat so it can be restored later.
    class RestoreInfo
    {
    public:
        RestoreInfo();

        void PrepareForStageChange(const CombatDisplayPtr_t);
        void Save(const CombatDisplayPtr_t);
        void Restore(const CombatDisplayPtr_t);

        bool HasRestored() const { return hasRestored_; }
        bool CanTurnAdvance() const { return canTurnAdvance_; }
        void CanTurnAdvance(const bool B) { canTurnAdvance_ = B; }

    private:
        void CreatureHealthSave(const CombatDisplayPtr_t);
        void CreatureHealthRestore(const CombatDisplayPtr_t);

        void FlyingCreaturesSave(const CombatDisplayPtr_t);
        void FlyingCreaturesRestore(const CombatDisplayPtr_t);

        void CreaturePositionsSave(const CombatDisplayPtr_t);
        void CreaturePositionsRestore(const CombatDisplayPtr_t);

    private:
        bool canTurnAdvance_;
        bool hasRestored_;
        creature::CreaturePVec_t creaturesFlyingPVec_;
        CreatureBlockingPosMap_t creatureBlockingPosMap_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATRESTOREINFO_HPP_INCLUDED
