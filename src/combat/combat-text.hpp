// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
//
// combat-text.hpp
//
#include "combat/fight-results.hpp"
#include "combat/turn-action-info.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{
    // forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    // A collection of functions that generate text used during combat.
    class Text
    {
    public:
        Text(const Text &) = delete;
        Text(Text &&) = delete;
        Text & operator=(const Text &) = delete;
        Text & operator=(Text &&) = delete;

        Text() = default;

        const std::string
            MouseOverTextAttackStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextFightStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextCastStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextPlayStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextAdvanceStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextRetreatStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextBlockStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextFlyStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextLandStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextRoarStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextPounceStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextRunStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string
            MouseOverTextSkipStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const;

        const std::string ActionText(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult & FIGHT_RESULT,
            const bool WILL_USE_NAME = false,
            const bool IS_STATUS_VERSION = false,
            const bool IS_PREAMBLE_VERSION = false) const;

        const std::string ActionTextIndexed(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult & FIGHT_RESULT,
            const bool WILL_USE_NAME,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX,
            bool & wasCollapsed) const;

        const std::string WeaponActionVerb(
            const item::ItemPtr_t WEAPON_PTR, const bool WILL_APPEND_ING = false) const;

        const std::string AttackDescriptionStatusVersion(const FightResult & FIGHT_RESULT) const;

        const std::string AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT) const;

        const std::string AttackDescriptionFullVersion(
            const FightResult & FIGHT_RESULT,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX) const;

        const std::string CastDescriptionStatusVersion(
            const spell::SpellPtr_t & SPELL_PTR, const FightResult & FIGHT_RESULT) const;

        const std::string CastDescriptionPreambleVersion(
            const spell::SpellPtr_t & SPELL_PTR, const FightResult & FIGHT_RESULT) const;

        const std::string CastDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_CASTING_PTR,
            const spell::SpellPtr_t & SPELL_PTR,
            const FightResult & FIGHT_RESULT,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX,
            bool & wasCollapsed) const;

        const std::string PlaySongDescriptionStatusVersion(
            const song::SongPtr_t & SONG_PTR, const FightResult & FIGHT_RESULT) const;

        const std::string PlaySongDescriptionPreambleVersion(
            const song::SongPtr_t & SONG_PTR, const FightResult & FIGHT_RESULT) const;

        const std::string PlaySongDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_PLAYINGING_PTR,
            const song::SongPtr_t & SONG_PTR,
            const FightResult & FIGHT_RESULT,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX,
            bool & wasCollapsed) const;

        const std::string RoarDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_ROARING_PTR,
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult & FIGHT_RESULT,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX,
            bool & wasCollapsed) const;

        const std::string TrapDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_UNLOCKING_PTR,
            const FightResult & FIGHT_RESULT,
            const std::size_t EFFECT_INDEX,
            const std::size_t HIT_INDEX) const;

        const std::string InitialCombatStatusMessagePrefix() const;

        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_;

    private:
        const std::string WeaponActionVerbList(
            const HitInfoVec_t & HIT_INFO_VEC, const bool WILL_SKIP_MISSES) const;

        const std::string CountPhrase(const HitInfoVec_t &) const;

        const std::string CountPhrase(const std::size_t) const;

        const std::string NamePhrase(const creature::CreaturePtr_t) const;

        const std::string AttackConditionsSummaryList(const CreatureEffect & CREATURE_EFFECT) const;

        const FightResultSummary SummarizeFightResult(
            const creature::CreaturePtr_t CREATURE_INITIATING_PTR, const FightResult &) const;

        bool SummarizeCreatureEffect(
            FightResultSummary & frs, const CreatureEffect & CREATURE_EFFECT) const;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
