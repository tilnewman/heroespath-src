// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
#define HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
//
// trap-holder.hpp
//
#include "combat/trap.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace combat
{
    namespace trap
    {

        // Responsible for the lifetimes of all Traps in the game.
        struct Holder
        {
            static void Fill();
            static void Empty();

            static int GetMinSeverity();
            static int GetMaxSeverity();

            static const TrapVec_t
                GetWithSeverityRatioBetween(const float THE_MIN, const float THE_MAX);

            static const Trap SelectRandomWithSeverityRatioNear(const float);

        private:
            static TrapVec_t traps_;
        };

    } // namespace trap
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TRAP_TRAP_HOLDER_HPP_INCLUDED
