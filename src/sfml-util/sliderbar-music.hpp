// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDERBARMUSIC_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDERBARMUSIC_HPP_INCLUDED
//
// sliderbar-music.hpp
//  A SliderBar that controls the music volume.
//
#include "sfml-util/gui/sliderbar-labeled.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // forward declaration
    namespace gui
    {
        class MouseTextInfo;
    }

    // Encapsulates a gui sliderbar with a label that controls the music volume.
    class SliderBarLabeled_Music : public gui::SliderBarLabeled
    {
    public:
        SliderBarLabeled_Music(const SliderBarLabeled_Music &) = delete;
        SliderBarLabeled_Music(SliderBarLabeled_Music &&) = delete;
        SliderBarLabeled_Music & operator=(const SliderBarLabeled_Music &) = delete;
        SliderBarLabeled_Music & operator=(SliderBarLabeled_Music &&) = delete;

        SliderBarLabeled_Music(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const gui::SliderStyle & STYLE,
            const gui::MouseTextInfo & THREE_TEXT_INFOS_HOLDER,
            const float INITIAL_VALUE = 0.0f,
            const float RELATIVE_LABEL_POS_LEFT = 0.0f,
            const float RELATIVE_LABEL_POS_TOP = 0.0f); // must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled_Music();

        void OnChange(const float NEW_VALUE) override;
        void SetPadPosition() override;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
        void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V) override;
    };

    using SliderBarLabeled_MusicUPtr_t = std::unique_ptr<SliderBarLabeled_Music>;
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDERBARMUSIC_HPP_INCLUDED
