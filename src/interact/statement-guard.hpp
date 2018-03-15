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
#ifndef HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
//
// statement-guar.hpp
//
#include "interact/statement.hpp"
#include "map/level-enum.hpp"
#include "player/party.hpp"
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
            static const std::string Compose(const Mood, const player::Party &);

        private:
            static const std::string ComposeKind();
            static const std::string ComposeMean();
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENT_GUARD_HPP_INCLUDED
