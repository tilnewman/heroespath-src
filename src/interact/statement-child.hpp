// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENTCHILD_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTCHILD_HPP_INCLUDED
//
// statement-child.hpp
//
#include "creature/player-party.hpp"
#include "interact/statement.hpp"
#include "map/level-enum.hpp"
#include <algorithm>
#include <string>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        namespace compose
        {
            inline const StrVec_t Parents()
            {
                return { "mother", "father", "mom", "dad", "mommy", "daddy", "ma", "pa" };
            }

            inline const StrVec_t Relations()
            {
                return { "sister",
                         "brother",
                         "big sister",
                         "big brother",
                         "little sister",
                         "little brother",
                         "uncle",
                         "aunt",
                         "cousin"
                         "grandma",
                         "grandpa" };
            }

            inline const StrVec_t ParentsAndRelations()
            {
                StrVec_t allVec{ Relations() };
                const StrVec_t PARENTS_VEC{ Parents() };

                std::copy(
                    std::begin(PARENTS_VEC), std::end(PARENTS_VEC), std::back_inserter(allVec));

                return allVec;
            }
        } // namespace compose

        struct ChildTalk
        {
            static const std::string Compose(const Mood, const creature::PlayerParty &);

        private:
            static const std::string ComposeKind(const creature::PlayerParty &);
            static const std::string ComposeMean(const creature::PlayerParty &);
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTCHILD_HPP_INCLUDED
