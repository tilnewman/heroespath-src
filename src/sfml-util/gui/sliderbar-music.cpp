// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sliderbar-music.cpp
//
#include "sliderbar-music.hpp"

#include "log/log-macros.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"

#include <sstream>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        SliderBarMusic::SliderBarMusic(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const gui::SliderStyle & STYLE,
            const gui::MouseTextInfo & THREE_TEXT_INFOS,
            const float INITIAL_VALUE,
            const float RELATIVE_LABEL_POS_LEFT,
            const float RELATIVE_LABEL_POS_TOP)
            : SliderBarLabeled(
                  std::string(NAME).append("_SliderBarMusic"),
                  POS_LEFT,
                  POS_TOP,
                  LENGTH,
                  STYLE,
                  THREE_TEXT_INFOS,
                  INITIAL_VALUE,
                  RELATIVE_LABEL_POS_LEFT,
                  RELATIVE_LABEL_POS_TOP)
        {
            PositionRatio((sfml_util::SoundManager::Instance()->MusicVolume() / 100.0f));
        }

        SliderBarMusic::~SliderBarMusic() = default;

        void SliderBarMusic::OnChange(const float CURRENT_POS_RATIO)
        {
            auto const CURRENT_POS_PERCENT { CURRENT_POS_RATIO * 100.0f };
            sfml_util::SoundManager::Instance()->MusicVolumeSet(CURRENT_POS_PERCENT);
            SliderBarLabeled::OnChange(CURRENT_POS_RATIO);
        }

        const TextInfo SliderBarMusic::CreateTextToDisplay(const float CURRENT_POS_RATIO)
        {
            const auto CURRENT_POS_PERCENT { static_cast<int>(CURRENT_POS_RATIO * 100.0f) };
            gui::TextInfo textInfo { TextInfoFromCurrentPositionPercent(CURRENT_POS_PERCENT) };

            std::ostringstream ss;

            if (CURRENT_POS_PERCENT == 0)
            {
                ss << "MUTE";
                textInfo.font_letters = sfml_util::GuiFont::SystemCondensed;
            }
            else
            {
                ss << CURRENT_POS_PERCENT;
                textInfo.font_letters = sfml_util::GuiFont::Number;
            }

            textInfo.text = ss.str();
            return textInfo;
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
