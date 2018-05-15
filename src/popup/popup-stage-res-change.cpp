// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-res-chang.cpp
//
#include "popup-stage-res-change.hpp"

#include "game/loop-manager.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageResChange::PopupStageResChange(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , elapsedTimeCounter_(0.0f)
        , secondCounter_(10)
    {}

    PopupStageResChange::~PopupStageResChange() = default;

    void PopupStageResChange::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);
        Stage::Draw(target, STATES);
    }

    void PopupStageResChange::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (secondCounter_ > 0)
        {
            elapsedTimeCounter_ += ELAPSED_TIME_SECONDS;

            if (elapsedTimeCounter_ > 1.0f)
            {
                elapsedTimeCounter_ = 0.0f;
                --secondCounter_;

                if (secondCounter_ > 0)
                {
                    sfml_util::gui::TextInfo textInfo(popupInfo_.TextInfo());

                    std::ostringstream ss;
                    ss << textInfo.text << "\n" << secondCounter_;

                    textInfo.text = ss.str();

                    textRegionUPtr_->Setup(textInfo, textRegion_, sfml_util::IStagePtr_t(this));
                }
                else
                {
                    game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::No);
                }
            }
        }
    }
} // namespace popup
} // namespace heroespath
