// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_SLIDERBAR_MUSIC_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_SLIDERBAR_MUSIC_HPP_INCLUDED
//
// sliderbar-music.hpp
//
#include "sfml-util/gui/sliderbar-labeled.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Encapsulates a gui sliderbar with a label that controls the music volume.
        class SliderBarMusic : public SliderBarLabeled
        {
        public:
            SliderBarMusic(const SliderBarMusic &) = delete;
            SliderBarMusic(SliderBarMusic &&) = delete;
            SliderBarMusic & operator=(const SliderBarMusic &) = delete;
            SliderBarMusic & operator=(SliderBarMusic &&) = delete;

            SliderBarMusic(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const float LENGTH,
                const SliderStyle & STYLE,
                const MouseTextInfo & THREE_TEXT_INFOS,
                const float INITIAL_VALUE = 0.0f,
                const float RELATIVE_LABEL_POS_LEFT = 0.0f,
                const float RELATIVE_LABEL_POS_TOP = 0.0f); // must be [0.0f, 1.0f]

            virtual ~SliderBarMusic();

        protected:
            void OnChange(const float CURRENT_POS_RATIO) override;
            const TextInfo CreateTextToDisplay(const float CURRENT_POS_RATIO) override;
        };

        using SliderBarMusicUPtr_t = std::unique_ptr<SliderBarMusic>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_SLIDERBAR_MUSIC_HPP_INCLUDED
