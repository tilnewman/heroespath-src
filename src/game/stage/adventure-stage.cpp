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
// adventure-stage.cpp
//
#include "adventure-stage.hpp"

#include "sfml-util/display.hpp"


namespace game
{
namespace stage
{

    AdventureStage::AdventureStage()
    :
        Stage(
            "Adventure",
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight())
    {}


    AdventureStage::~AdventureStage()
    {}


    void AdventureStage::Setup()
    {}


    void AdventureStage::UpdateTime(const float)
    {}


    void AdventureStage::Draw(sf::RenderTarget &, const sf::RenderStates &)
    {}

}
}