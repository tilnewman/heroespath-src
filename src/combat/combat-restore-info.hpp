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
