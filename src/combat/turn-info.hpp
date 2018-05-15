// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
//
// turn-info.hpp
//
#include "combat/strategy-info.hpp"
#include "combat/turn-action-enum.hpp"
#include "combat/turn-action-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"
#include "stats/trait.hpp"

#include <utility>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // Everything needed to decide what a creature will do during their turn in combat.
    class TurnInfo
    {
    public:
        explicit TurnInfo(
            const bool IS_FLYING = false,
            const TurnAction::Enum TURN_STATE = TurnAction::Count,
            const strategy::Info & STRATEGY_INFO = strategy::Info());

        bool GetIsFlying() const { return isFlying_; }
        void SetIsFlying(const bool B) { isFlying_ = B; }

        const TurnActionInfo GetTurnActionInfo() const { return turnActionInfo_; }
        void SetTurnActionInfo(const TurnActionInfo & TAI) { turnActionInfo_ = TAI; }

        strategy::Info GetStrategyInfo() const { return strategyInfo_; }
        void SetStrategyInfo(const strategy::Info & SI) { strategyInfo_ = SI; }

        const creature::CreaturePtrOpt_t GetFirstAttackedByCreaturePtrOpt() const
        {
            return firstAttackedByCreaturePtrOpt_;
        }

        void SetFirstAttackedByCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            firstAttackedByCreaturePtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetLastAttackedByCreaturePtrOpt() const
        {
            return lastAttackedByCreaturePtrOpt_;
        }

        void SetLastAttackedByCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            lastAttackedByCreaturePtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetFirstHitByCreaturePtrOpt() const
        {
            return firstHitByCreaturePtrOpt_;
        }

        void SetFirstHitByCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            firstHitByCreaturePtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetLastHitByCreaturePtrOpt() const
        {
            return lastHitByCreaturePtrOpt_;
        }

        void SetLastHitByCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            lastHitByCreaturePtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetFirstToMakeMusicCreaturePtrOpt() const
        {
            return firstToMakeMusicPtrOpt_;
        }

        void SetFirstToMakeMusicCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            firstToMakeMusicPtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetLastToMakeMusicCreaturePtrOpt() const
        {
            return lastToMakeMusicPtrOpt_;
        }

        void SetLastToMakeMusicCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            lastToMakeMusicPtrOpt_ = CREATURE_PTR;
        }

        bool IsFirstAttackedByAlsoFirstHitByCreature() const
        {
            return firstAttackedByCreaturePtrOpt_ == firstHitByCreaturePtrOpt_;
        }

        bool IsLastAttackedByAlsoLastHitByCreature() const
        {
            return lastAttackedByCreaturePtrOpt_ == lastHitByCreaturePtrOpt_;
        }

        const creature::CreaturePtrOpt_t GetFirstToCastCreaturePtrOpt() const
        {
            return firstToCastPtrOpt_;
        }

        void SetFirstToCastCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            firstToCastPtrOpt_ = CREATURE_PTR;
        }

        const creature::CreaturePtrOpt_t GetLastToCastCreaturePtrOpt() const
        {
            return lastToCastPtrOpt_;
        }

        void SetLastToCastCreature(const creature::CreaturePtr_t CREATURE_PTR)
        {
            lastToCastPtrOpt_ = CREATURE_PTR;
        }

        void UpdateMostDamagedCreature(const Health_t DAMAGE_ABS, const creature::CreaturePtr_t);

        const creature::CreaturePtrOpt_t MostDamagedCreaturePtrOpt() const
        {
            return mostDamagedCreaturePtrOpt_;
        }

        std::size_t CastCount() const { return castCount_; }
        std::size_t CastCountIncrement() { return ++castCount_; }

        std::size_t SongCount() const { return songCount_; }
        std::size_t SongCountIncrement() { return ++songCount_; }

        bool GetWasHitLastTurn() const { return wasHitLastTurn_; }
        void SetWasHitLastTurn(const bool B) { wasHitLastTurn_ = B; }

        void RemoveDeadCreatureTasks(const creature::CreaturePtr_t);

        friend bool operator<(const TurnInfo & L, const TurnInfo & R);
        friend bool operator==(const TurnInfo & L, const TurnInfo & R);

    private:
        bool isFlying_;
        TurnActionInfo turnActionInfo_;
        strategy::Info strategyInfo_;
        creature::CreaturePtrOpt_t firstAttackedByCreaturePtrOpt_;
        creature::CreaturePtrOpt_t firstHitByCreaturePtrOpt_;
        creature::CreaturePtrOpt_t lastAttackedByCreaturePtrOpt_;
        creature::CreaturePtrOpt_t lastHitByCreaturePtrOpt_;
        creature::CreaturePtrOpt_t firstToMakeMusicPtrOpt_;
        creature::CreaturePtrOpt_t lastToMakeMusicPtrOpt_;
        creature::CreaturePtrOpt_t firstToCastPtrOpt_;
        creature::CreaturePtrOpt_t lastToCastPtrOpt_;
        Health_t mostDamagedHealthAbs_;
        creature::CreaturePtrOpt_t mostDamagedCreaturePtrOpt_;
        std::size_t castCount_;
        std::size_t songCount_;
        bool wasHitLastTurn_;
    };

    using TurnInfoMap_t = misc::VectorMap<creature::CreaturePtr_t, TurnInfo>;

    bool operator<(const TurnInfo & L, const TurnInfo & R);

    bool operator==(const TurnInfo & L, const TurnInfo & R);

    inline bool operator!=(const TurnInfo & L, const TurnInfo & R) { return !(L == R); }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
