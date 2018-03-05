// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// condition.hpp
//
#include "condition.hpp"

#include "creature/creature.hpp"
#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace creature
{

    Condition::Condition(
        const Conditions::Enum TYPE, const bool IS_MAGICAL, const stats::TraitSet & TRAIT_SET)
        : type_(TYPE)
        , isMagical_(IS_MAGICAL)
        , traitSet_(TRAIT_SET)
    {}

    Condition::~Condition() {}

    const std::string Condition::ToString() const
    {
        std::ostringstream ss;
        ss << Conditions::Name(type_);

        if (isMagical_)
        {
            ss << " (magical)";
        }

        ss << ".  " << Conditions::Desc(type_);

        auto const TRAIT_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAIT_STR.empty() == false)
        {
            ss << "  Traits: " << TRAIT_STR;
        }

        return ss.str();
    }

    const std::string Condition::LongDesc() const
    {
        std::ostringstream ss;
        ss << Conditions::Desc(type_);

        if (isMagical_)
        {
            ss << " (a magical condition)";
        }

        auto const TRAIT_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAIT_STR.empty() == false)
        {
            ss << "  Traits: " << TRAIT_STR;
        }

        ss << ".";
        return ss.str();
    }

    bool operator<(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_, L.isMagical_, L.traitSet_)
            < std::tie(R.type_, R.isMagical_, R.traitSet_);
    }

    bool operator==(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_, L.isMagical_, L.traitSet_)
            == std::tie(R.type_, R.isMagical_, R.traitSet_);
    }

    // InitialChange() does not alter stats, see creature.cpp::ConditionAdd() for that
    void Condition::InitialChange(CreaturePtrC_t) const {}

    void Condition::FinalChange(CreaturePtrC_t) const {}

    void Condition::PerTurnEffect(CreaturePtr_t, combat::HitInfoVec_t &, bool &) const {}
}
}
