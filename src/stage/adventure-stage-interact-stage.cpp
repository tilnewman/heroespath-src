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
// adventure-stage-interact-stage.hpp
//
#include "adventure-stage-interact-stage.hpp"
#include "map/map.hpp"


namespace heroespath
{
namespace stage
{

    InteractStage::InteractStage(map::Map & map, const sf::FloatRect & STAGE_REGION)
    :
        Stage("AdventureInteract", STAGE_REGION),
        map_(map),
        texture_(),
        sprite_()
    {}


    void InteractStage::Setup()
    {
        sf::Image image;
        image.create(10, 10);

        texture_.loadFromImage(image);

        sprite_.setTexture(texture_, true);

        auto const SCALE_HORIZ{ StageRegion().width / sprite_.getLocalBounds().width };
        auto const SCALE_VERT{ StageRegion().height / sprite_.getLocalBounds().height };
        sprite_.setScale(SCALE_HORIZ, SCALE_VERT);

        sprite_.setPosition(StageRegion().left, StageRegion().top);

        sprite_.setColor(sf::Color(255, 255, 255, 64));
    }


    void InteractStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(sprite_, STATES);
        Stage::Draw(target, STATES);
    }


    void InteractStage::UpdateTime(const float TIME_ELAPSED)
    {
        map_.UpdateInteractions(TIME_ELAPSED);
    }


}
}
