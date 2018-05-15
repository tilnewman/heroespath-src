// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
#include <string>

namespace heroespath
{
namespace state
{

    // Responsible for storing all states that represent
    // the game world contained in a specific level.
    class Level
    {
    public:
        explicit Level(const map::Level::Enum = map::Level::Enum::Count);

        ~Level();

        map::Level::Enum Which() const { return level_; }

        const std::string Name() const { return map::Level::ToString(level_); }

        bool IsDoorLocked(const map::Level::Enum) const;

        void IsDoorLocked(const map::Level::Enum LEVEL, const bool IS_LOCKED)
        {
            doorLockMap_[LEVEL] = IS_LOCKED;
        }

        void HandleLevelLoad();
        void HandleLevelUnload();

        void AddSpecificNPC(const NpcPtr_t NPC_PTR) { specificNpcs_.emplace_back(NPC_PTR); }

        void AddRandomNpc(const NpcPtr_t NPC_PTR) { randomNpcs_.emplace_back(NPC_PTR); }

        void AddRandomNpcPlaceholder(const NpcPlaceholder & NPC_PLACEHOLDER)
        {
            randomNPCPlaceholders_.emplace_back(NPC_PLACEHOLDER);
        }

        const NpcPVec_t SpecificNPCs() const { return specificNpcs_; }

        const NpcPVec_t RandomNPCs() const { return randomNpcs_; }

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

    private:
        void CreateRandomNPCs();

    private:
        map::Level::Enum level_;
        misc::VectorMap<map::Level::Enum, bool> doorLockMap_;
        NpcPlaceholderVec_t randomNPCPlaceholders_;

        // these are observer pointers but they are needed to NpcWarehouse::Free(), see destructor
        //
        // these are misc::NotNull pointers that cannot be serialized, so the ...ToSerialize vectors
        // below are used
        NpcPVec_t specificNpcs_;
        NpcPVec_t randomNpcs_;
        std::vector<Npc *> specificNpcsToSerialize_;
        std::vector<Npc *> randomNpcsToSerialize_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & level_;
            ar & doorLockMap_;
            ar & randomNPCPlaceholders_;
            ar & specificNpcsToSerialize_;
            ar & randomNpcsToSerialize_;
        }
    };

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_LEVEL_HPP_INCLUDED
