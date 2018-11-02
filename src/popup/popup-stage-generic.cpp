// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-generic.cpp
//
#include "popup-stage-generic.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    PopupStageGeneric::PopupStageGeneric(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
    {}

    PopupStageGeneric::~PopupStageGeneric() = default;

    void PopupStageGeneric::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);
        StageBase::draw(target, states);
    }

} // namespace popup
} // namespace heroespath
