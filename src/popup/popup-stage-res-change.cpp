// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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

                    textRegionUPtr_->Setup(textInfo, textRegion_, this);
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
