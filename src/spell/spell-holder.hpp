// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
