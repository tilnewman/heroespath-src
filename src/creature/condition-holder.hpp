// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_CONDITION_HOLDER_HPP_INCLUDED
#define HEROESPATH_CREATURE_CONDITION_HOLDER_HPP_INCLUDED
//
// condition-holder.hpp
//
#include "creature/condition-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
    using ConditionUPtr_t = std::unique_ptr<Condition>;
    using ConditionUVec_t = std::vector<ConditionUPtr_t>;

    namespace condition
    {

        // Responsible for the lifetime of all Conditions in the game.
        struct Holder
        {
            static void Fill();
            static void Empty();
            static bool Test();
            static const ConditionPtr_t Get(const Conditions::Enum);

        private:
            static ConditionUVec_t conditionsUVec_;
        };

    } // namespace condition
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CONDITION_HOLDER_HPP_INCLUDED
