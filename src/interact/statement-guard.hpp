// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
//
// statement-guar.hpp
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

            inline const StrVec_t RankLow() { return { "Private", "Corporal" }; }

            inline const StrVec_t RankHigh() { return { "Sergeant", "Sarge", "Major", "General" }; }

            inline const StrVec_t RankAll()
            {
                StrVec_t rankAll{ RankLow() };
                auto const RANK_HIGH{ RankHigh() };
                std::copy(std::begin(RANK_HIGH), std::end(RANK_HIGH), std::back_inserter(rankAll));
                return rankAll;
            }

        } // namespace compose

        struct GuardTalk
        {
            static const std::string Compose(const Mood, const creature::PlayerParty &);

        private:
            static const std::string ComposeKind();
            static const std::string ComposeMean();
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
