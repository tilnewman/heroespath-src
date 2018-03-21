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
#ifndef HEROESPATH_STATE_NPC_PLACEHOLDER_HPP_INCLUDED
#define HEROESPATH_STATE_NPC_PLACEHOLDER_HPP_INCLUDED
//
// npc-placeholder.hpp
//
#include "avatar/avatar-enum.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-serialize-includes.hpp"
#include <vector>

namespace heroespath
{
namespace state
{

    // Responsible for wrapping all the state needed to create a random NPC in the game.
    class NpcPlaceholder
    {
    public:
        explicit NpcPlaceholder(
            const std::size_t COUNT_MIN = 1,
            const std::size_t COUNT_MAX = 1,
            const avatar::Avatar::NameVec_t & AVATAR_TYPE_VEC = avatar::Avatar::NameVec_t(),
            const std::size_t WALK_BOUNDS_INDEX = 0,
            const bool IS_DRUNK = false)
            : countMin_(COUNT_MIN)
            , countMax_(COUNT_MAX)
            , avatarTypeVec_(AVATAR_TYPE_VEC)
            , walkBoundsIndex_(WALK_BOUNDS_INDEX)
            , isDrunk_(IS_DRUNK)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (AVATAR_TYPE_VEC.empty() == false),
                "state::NpcPlaceholder::NpcPlaceholder(countMin="
                    << COUNT_MIN << ", countMax=" << COUNT_MAX << ", walkBoundsIndex="
                    << WALK_BOUNDS_INDEX << ") was given an empty AVATAR_TYPE_VEC.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (COUNT_MIN <= COUNT_MAX),
                "state::NpcPlaceholder::NpcPlaceholder(countMin="
                    << COUNT_MIN << ", countMax=" << COUNT_MAX
                    << ", walkBoundsIndex=" << WALK_BOUNDS_INDEX
                    << ") was given COUNT_MIN that was greater than COUNT_MAX.");
        }

        std::size_t CountMin() const { return countMin_; }

        std::size_t CountMax() const { return countMax_; }

        const avatar::Avatar::NameVec_t AvatarTypeVec() const { return avatarTypeVec_; }

        std::size_t WalkBoundsIndex() const { return walkBoundsIndex_; }

        bool IsDrunk() const { return isDrunk_; }

    private:
        std::size_t countMin_;
        std::size_t countMax_;
        avatar::Avatar::NameVec_t avatarTypeVec_;
        std::size_t walkBoundsIndex_;
        bool isDrunk_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & countMin_;
            ar & countMax_;
            ar & avatarTypeVec_;
            ar & walkBoundsIndex_;
            ar & isDrunk_;
        }
    };

    using NpcPlaceholderVec_t = std::vector<NpcPlaceholder>;

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_NPC_PLACEHOLDER_HPP_INCLUDED
