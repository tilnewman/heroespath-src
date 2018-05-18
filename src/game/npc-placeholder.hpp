// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_NPC_PLACEHOLDER_HPP_INCLUDED
#define HEROESPATH_GAME_NPC_PLACEHOLDER_HPP_INCLUDED
//
// npc-placeholder.hpp
//
#include "avatar/avatar-enum.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <vector>

namespace heroespath
{
namespace game
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
                "NpcPlaceholder::NpcPlaceholder(countMin="
                    << COUNT_MIN << ", countMax=" << COUNT_MAX << ", walkBoundsIndex="
                    << WALK_BOUNDS_INDEX << ") was given an empty AVATAR_TYPE_VEC.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (COUNT_MIN <= COUNT_MAX),
                "NpcPlaceholder::NpcPlaceholder(countMin="
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

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_NPC_PLACEHOLDER_HPP_INCLUDED
