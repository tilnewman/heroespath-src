// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_ANIMATION_HPP_INCLUDED
#define HEROESPATH_AVATAR_ANIMATION_HPP_INCLUDED
//
// animation.hpp
//
#include "avatar/pose-enum.hpp"
#include "sfml-util/direction-enum.hpp"

#include <vector>

namespace heroespath
{
namespace avatar
{

    using FrameNum_t = int;
    using FrameNumVec_t = std::vector<FrameNum_t>;

    // Responsible for wrapping all the information needed to animate an NPC.
    struct Animation
    {
        Animation(
            const Pose::Enum POSE,
            const sfml_util::Direction::Enum DIRECTION,
            const std::vector<FrameNum_t> & FRAME_NUM_VEC,
            const float FRAME_DURATION,
            const bool WILL_LOOP)
            : pose(POSE)
            , direction(DIRECTION)
            , frame_num_vec(FRAME_NUM_VEC)
            , frame_duration(FRAME_DURATION)
            , will_loop(WILL_LOOP)
        {}

        Pose::Enum pose;
        sfml_util::Direction::Enum direction;
        FrameNumVec_t frame_num_vec;
        float frame_duration;
        bool will_loop;
    };
} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_ANIMATION_HPP_INCLUDED
