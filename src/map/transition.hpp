// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
#define HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
//
// transition.hpp
//
#include "gui/sound-effects-enum.hpp"
#include "map/level-enum.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for storing all the information about a transition between two maps.
    class Transition
    {
    public:
        Transition(
            const bool IS_ENTRY,
            const Level::Enum LEVEL,
            const sf::FloatRect & RECT,
            const gui::sound_effect::MapTransition TRANS_TYPE)
            : isEntry_(IS_ENTRY)
            , level_(LEVEL)
            , rect_(RECT)
            , transSfxType_(TRANS_TYPE)
        {}

        Transition(const Transition &) = default;
        Transition(Transition &&) = default;
        Transition & operator=(const Transition &) = default;
        Transition & operator=(Transition &&) = default;

        bool IsEntry() const { return isEntry_; }
        Level::Enum WhichLevel() const { return level_; }
        const sf::FloatRect Rect() const { return rect_; }
        gui::sound_effect::MapTransition SfxType() const { return transSfxType_; }

    private:
        bool isEntry_;
        Level::Enum level_;
        sf::FloatRect rect_;
        gui::sound_effect::MapTransition transSfxType_;
    };

    using TransitionVec_t = std::vector<Transition>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
