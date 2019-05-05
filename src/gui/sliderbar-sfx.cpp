// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sliderbar-sfx.cpp
//
#include "sliderbar-sfx.hpp"

#include "gui/font-manager.hpp"
#include "gui/sound-manager.hpp"
#include "misc/log-macros.hpp"

#include <sstream>

namespace heroespath
{
namespace gui
{
    bool SliderBarSfx::isConstructionAndInitFinished_ { false };

    SliderBarSfx::SliderBarSfx(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        const SliderStyle & STYLE,
        const MouseTextInfo & THREE_TEXT_INFOS,
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
        , timeSinceLastPlaySec_(0.0f)
    {
        WillPlaySfx(false);
        PositionRatio(gui::SoundManager::Instance()->SoundEffectVolume() / 100.0f);
        isConstructionAndInitFinished_ = true;
    }

    SliderBarSfx::~SliderBarSfx() = default;

    void SliderBarSfx::OnChange(const float CURRENT_POS_RATIO)
    {
        const auto CURRENT_POS_PERCENT { CURRENT_POS_RATIO * 100.0f };
        SoundManager::Instance()->SoundEffectVolumeSet(CURRENT_POS_PERCENT);
        SliderBarLabeled::OnChange(CURRENT_POS_RATIO);
    }

    const TextInfo SliderBarSfx::CreateTextToDisplay(const float CURRENT_POS_RATIO)
    {
        const auto CURRENT_POS_PERCENT { CURRENT_POS_RATIO * 100.0f };
        const auto CURRENT_POS_PERCENT_INT { static_cast<int>(CURRENT_POS_PERCENT) };
        TextInfo textInfo { TextInfoFromCurrentPositionPercent(CURRENT_POS_PERCENT_INT) };

        textInfo.text.reserve(8);

        if (CURRENT_POS_PERCENT_INT == 0)
        {
            textInfo.text += "MUTE";
            textInfo.font_letters = gui::GuiFont::SystemCondensed;
        }
        else
        {
            textInfo.text += std::to_string(CURRENT_POS_PERCENT_INT);
            textInfo.font_letters = gui::GuiFont::Number;
        }

        return textInfo;
    }

    bool SliderBarSfx::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        if (isConstructionAndInitFinished_ && WillPlaySfx())
        {
            timeSinceLastPlaySec_ += ELAPSED_TIME_SEC;

            if (timeSinceLastPlaySec_ > 0.5f)
            {
                WillPlaySfx(false);
                timeSinceLastPlaySec_ = 0.0f;
                SoundManager::Instance()->PlaySfx_Reject();
                return true;
            }
        }

        return false;
    }

} // namespace gui
} // namespace heroespath
