// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation-base.cpp
//
#include "animation-base.hpp"
#include "misc/random.hpp"

namespace heroespath
{
namespace gui
{

    Animation::Animation(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
        : Entity(Animations::ToString(ENUM) + "_Animation", REGION)
        , which_(ENUM)
        , blendMode_(BLEND_MODE)
        , timePerFrameSec_(TIME_PER_FRAME_SEC)
        , currentFrame_(0)
        , frameTimerSec_(0.0f)
        , colorFrom_(COLOR_FROM)
        , colorTo_(COLOR_TO)
        , isFinished_(false)
    {}

    Animation::~Animation() = default;

    void Animation::RandomVaryTimePerFrame()
    {
        const auto VARY_SEC_MAX { 0.02f };
        timePerFrameSec_ -= VARY_SEC_MAX * 0.5f;
        timePerFrameSec_ += misc::random::Float(VARY_SEC_MAX);
    }

} // namespace gui
} // namespace heroespath
