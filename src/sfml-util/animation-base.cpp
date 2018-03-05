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
// animation-base.cpp
//
#include "animation-base.hpp"
#include "misc/random.hpp"

namespace heroespath
{
namespace sfml_util
{

    Animation::Animation(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
        : gui::GuiEntity(Animations::ToString(ENUM) + "_Animation", REGION)
        , which_(ENUM)
        , blendMode_(BLEND_MODE)
        , timePerFrameSec_(TIME_PER_FRAME_SEC)
        , currentFrame_(0)
        , frameTimerSec_(0.0f)
        , colorFrom_(COLOR_FROM)
        , colorTo_(COLOR_TO)
        , isFinished_(false)
    {}

    Animation::~Animation() {}

    void Animation::RandomVaryTimePerFrame()
    {
        auto const VARY_SEC_MAX{ 0.02f };
        timePerFrameSec_ -= VARY_SEC_MAX * 0.5f;
        timePerFrameSec_ += misc::random::Float(VARY_SEC_MAX);
    }
}
}
