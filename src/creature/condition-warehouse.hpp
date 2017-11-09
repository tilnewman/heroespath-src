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
#ifndef HEROESPATH_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
//
// condition-warehouse.hpp
//
#include "creature/condition-enum.hpp"

#include <memory>
#include <vector>


namespace heroespath
{
namespace creature
{
    class Condition;
    using ConditionPtr_t = Condition *;
    using ConditionUPtr_t = std::unique_ptr<Condition>;
    using ConditionUVec_t = std::vector<ConditionUPtr_t>;

namespace condition
{

    struct Warehouse
    {
        static void Fill();
        static void Empty();
        static bool Test();
        static ConditionPtr_t Get(const Conditions::Enum);

    private:
        static ConditionUVec_t conditionsUVec_;
    };

}
}
}

#endif //HEROESPATH_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
