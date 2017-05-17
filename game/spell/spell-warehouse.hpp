#ifndef GAME_SPELL_SPELLWAREHOUSE_HPP_INCLUDED
#define GAME_SPELL_SPELLWAREHOUSE_HPP_INCLUDED
//
// spell-warehouse.hpp
//
#include "game/spell/spell-enum.hpp"

#include <vector>
#include <memory>


namespace game
{
namespace spell
{

    //forward declarations
    class Spell;
    using SpellPtr_t  = Spell *;
    using SpellSPtr_t = std::shared_ptr<Spell>;
    using SpellSVec_t = std::vector<SpellSPtr_t>;


    //Responsible for pre-game-start creation and testing of all spells, and
    //then providing access to them.
    struct Warehouse
    {
        static void Setup();
        static bool Test();
        static SpellPtr_t Get(const Spells::Enum);

    private:
        static SpellSVec_t spellsSVec_;
    };

}
}

#endif //GAME_SPELL_SPELLWAREHOUSE_HPP_INCLUDED