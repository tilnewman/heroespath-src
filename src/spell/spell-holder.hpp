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
#ifndef HEROESPATH_SPELL_SPELL_HOLDER_HPP_INCLUDED
#define HEROESPATH_SPELL_SPELL_HOLDER_HPP_INCLUDED
//
// spell-holder.hpp
//
#include "misc/not-null.hpp"
#include "spell/spell-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace spell
{

    // forward declarations
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellUPtr_t = std::unique_ptr<Spell>;
    using SpellUVec_t = std::vector<SpellUPtr_t>;

    // Responsible for the lifetimes of all Spells in the game.
    struct Holder
    {
        static void Fill();
        static void Empty();
        static bool Test();
        static const SpellPtr_t Get(const Spells::Enum);

    private:
        static SpellUVec_t spellsUVec_;
    };

} // namespace spell
} // namespace heroespath

#endif // HEROESPATH_SPELL_SPELL_HOLDER_HPP_INCLUDED