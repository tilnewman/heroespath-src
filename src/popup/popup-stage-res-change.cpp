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

#include <SFML/Graphics/RenderTarget.hpp>

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

    void PopupStageResChange::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);
        StageBase::draw(target, states);
    }

    void PopupStageResChange::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        if (secondCounter_ > 0)
        {
            elapsedTimeCounter_ += ELAPSED_TIME_SECONDS;

            if (elapsedTimeCounter_ > 1.0f)
            {
                elapsedTimeCounter_ = 0.0f;
                --secondCounter_;

                if (secondCounter_ > 0)
                {
                    gui::TextInfo textInfo(popupInfo_.TextInfo());

                    std::ostringstream ss;
                    ss << textInfo.text << "\n" << secondCounter_;

                    textInfo.text = ss.str();

                    textRegionUPtr_->Setup(textInfo, textRegion_, stage::IStagePtr_t(this));
                }
                else
                {
                    RemovePopup(PopupButtons::No);
                }
            }
        }
    }

} // namespace popup
} // namespace heroespath
