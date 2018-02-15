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
#ifndef HEROESPATH_NPC_ANIMATION_HPP_INCLUDED
#define HEROESPATH_NPC_ANIMATION_HPP_INCLUDED
//
// animation.hpp
//
#include "npc/pose-enum.hpp"
#include "sfml-util/direction-enum.hpp"

#include <vector>


namespace heroespath
{
namespace npc
{

    using FrameNum_t = int;
    using FrameNumVec_t = std::vector<FrameNum_t>;


    //Responsible for wrapping all the information needed to animate an NPC.
    struct Animation
    {
        Animation(
            const Pose::Enum POSE,
            const sfml_util::Direction::Enum DIRECTION,
            const std::vector<FrameNum_t> FRAME_NUM_VEC,
            const float FRAME_DURATION,
            const bool WILL_LOOP)
        :
            pose(POSE),
            direction(DIRECTION),
            frame_num_vec(FRAME_NUM_VEC),
            frame_duration(FRAME_DURATION),
            will_loop(WILL_LOOP)
        {}

        Pose::Enum pose;
        sfml_util::Direction::Enum direction;
        FrameNumVec_t frame_num_vec;
        float frame_duration;
        bool will_loop;
    };

}
}

#endif //HEROESPATH_NPC_ANIMATION_HPP_INCLUDED
