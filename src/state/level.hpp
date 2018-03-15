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
#ifndef HEROESPATH_STATE_LEVEL_HPP_INCLUDED
#define HEROESPATH_STATE_LEVEL_HPP_INCLUDED
//
// level.hpp
//
#include "map/level-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"
#include "state/npc-placeholder.hpp"
#include "state/npc.hpp"

#include <memory>

namespace heroespath
{
namespace state
{

    // Responsible for storing all states that represent
    // the game world contained in a specific level.
    class Level
    {
    public:
        Level(const map::Level::Enum = map::Level::Enum::Count);

        inline map::Level::Enum Which() const { return level_; }

        inline const std::string Name() const { return map::Level::ToString(level_); }

        bool IsDoorLocked(const map::Level::Enum) const;

        inline void IsDoorLocked(const map::Level::Enum LEVEL, const bool IS_LOCKED)
        {
            doorLockMap_[LEVEL] = IS_LOCKED;
        }

        void HandleLevelLoad();

        void AddSpecificNPC(const Npc & NPC) { specificNpcs_.emplace_back(NPC); }

        void ResetRandomNPCs();

        void AddNpcPlaceholder(const NpcPlaceholder & NPC_PLACEHOLDER)
        {
            placeholderNpcs_.emplace_back(NPC_PLACEHOLDER);
        }

        const NpcVec_t SpecificNPCs() const { return specificNpcs_; }

        const NpcVec_t RandomNPCs() const { return randomNpcs_; }

    private:
        map::Level::Enum level_;
        misc::VectorMap<map::Level::Enum, bool> doorLockMap_;
        NpcVec_t specificNpcs_;
        NpcVec_t randomNpcs_;
        NpcPlaceholderVec_t placeholderNpcs_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & level_;
            ar & doorLockMap_;
            ar & specificNpcs_;
            ar & randomNpcs_;
            ar & placeholderNpcs_;
        }
    };
} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_LEVEL_HPP_INCLUDED
