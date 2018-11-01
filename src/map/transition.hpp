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
#include "map/level-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"

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
        explicit Transition(
            const bool IS_ENTRY = true,
            const Level::Enum LEVEL = Level::Count,
            const sf::FloatRect & RECT = sf::FloatRect(),
            const gui::sound_effect::MapTransition TRANS_TYPE
            = gui::sound_effect::MapTransition::Count)
            : isEntry_(IS_ENTRY)
            , level_(LEVEL)
            , rect_(RECT)
            , transSfxType_(TRANS_TYPE)
        {}

        bool IsEntry() const { return isEntry_; }
        Level::Enum WhichLevel() const { return level_; }
        const sf::FloatRect Rect() const { return rect_; }
        gui::sound_effect::MapTransition TransType() const { return transSfxType_; }

        const sf::Vector2f Center() const
        {
            return sf::Vector2f(
                rect_.left + (rect_.width * 0.5f), rect_.top + (rect_.height * 0.5f));
        }

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
