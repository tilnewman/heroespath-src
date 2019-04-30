// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCLUDED
//
// enchantment.hpp
//  Similar to a Condition, an enchantment is something that changes a creature.
//
#include "creature/condition-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/trait.hpp"
#include "creature/traits-set.hpp"
#include "game/phase-enum.hpp"
#include "game/strong-types.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"
#include "spell/spell-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    // all the 'use' info about an enchantment
    class UseInfo
    {
    public:
        explicit UseInfo(
            const int COUNT = 0, // negative means infinite
            const game::Phase::Enum PHASE_RESTRICT = game::Phase::None,
            const spell::Spells::Enum SPELL = spell::Spells::Count,
            const CondEnumVec_t & CONDS_REMOVED_VEC = CondEnumVec_t())
            : countOrig_(COUNT)
            , countRemain_(COUNT)
            , phase_(PHASE_RESTRICT)
            , spell_(SPELL)
            , condsRemovedVec_(CONDS_REMOVED_VEC)
        {}

        int CountOrig() const { return countOrig_; }
        int CountRemaining() const { return countRemain_; }
        void CountConsume() { --countRemain_; }
        game::Phase::Enum RestrictedToPhase() const { return phase_; }
        spell::Spells::Enum Spell() const { return spell_; }
        const CondEnumVec_t & ConditionsRemoved() const { return condsRemovedVec_; }

        friend bool operator==(const UseInfo & L, const UseInfo & R);

    private:
        // negative means infinite
        int countOrig_;
        int countRemain_;

        game::Phase::Enum phase_;
        spell::Spells::Enum spell_;
        CondEnumVec_t condsRemovedVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & countOrig_;
            ar & countRemain_;
            ar & phase_;
            ar & spell_;
            ar & condsRemovedVec_;
        }
    };

    inline bool operator==(const UseInfo & L, const UseInfo & R)
    {
        return std::tie(L.countOrig_, L.countRemain_, L.phase_, L.spell_, L.condsRemovedVec_)
            == std::tie(R.countOrig_, R.countRemain_, R.phase_, R.spell_, R.condsRemovedVec_);
    }

    inline bool operator!=(const UseInfo & L, const UseInfo & R) { return !(L == R); }

    // Responsible for all state and operation of Enchantments in the game.
    class Enchantment
    {
    public:
        enum class UseEffectType
        {
            None,
            PixieBell,
            CrystalChimes,
            GoldenGong,
            DragonToothWhistle,
            DoveBloodVial,
            DriedEdible,
            ShamanRainmaker,
            SpecterChains,
            VultureGizzard,
            WarTrumpet,
            Blessed,
            Cursed
        };

        explicit Enchantment(
            const EnchantmentType::Enum TYPE = EnchantmentType::None,
            const TraitSet & TRAIT_SET = TraitSet(),
            const UseInfo & USE_INFO = UseInfo(),
            const std::string & EFFECTS_STR = "",
            const Score_t SCORE = 0_score,
            const UseEffectType EFFECT_TYPE = UseEffectType::None);

        EnchantmentType::Enum Type() const { return type_; }
        bool IsType(const EnchantmentType::Enum ENUM) const { return (ENUM & type_); }
        const TraitSet & Traits() const { return traitSet_; }
        const UseInfo & Use() const { return useInfo_; }
        void UseCountConsume() { useInfo_.CountConsume(); }

        bool IsUseableEver() const
        {
            // negative useCountOrig_ means infinite
            return ((type_ & EnchantmentType::WhenUsed) && (useInfo_.CountOrig() != 0));
        }

        bool IsUseableNow() const { return (IsUseableEver() && (useInfo_.CountRemaining() > 0)); }

        const std::string EffectStr() const;

        void UseEffect(const CreaturePtr_t);

        Score_t TreasureScore() const { return score_; }

        const std::string ToString() const;

        friend bool operator==(const Enchantment & L, const Enchantment & R);

    private:
        Score_t CalcTreasureScore() const;

    private:
        EnchantmentType::Enum type_;
        TraitSet traitSet_;
        UseInfo useInfo_;
        std::string effectsStr_;
        UseEffectType effectType_;
        Score_t score_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & traitSet_;
            ar & useInfo_;
            ar & effectsStr_;
            ar & effectType_;
            ar & score_;
        }
    };

    inline bool operator==(const Enchantment & L, const Enchantment & R)
    {
        return std::tie(L.type_, L.traitSet_, L.useInfo_, L.effectsStr_, L.effectType_, L.score_)
            == std::tie(R.type_, R.traitSet_, R.useInfo_, R.effectsStr_, R.effectType_, R.score_);
    }

    inline bool operator!=(const Enchantment & L, const Enchantment & R) { return !(L == R); }

    using EnchantmentVec_t = std::vector<Enchantment>;
    using EnchantmentPtr_t = misc::NotNull<Enchantment *>;
    using EnchantmentPtrOpt_t = boost::optional<EnchantmentPtr_t>;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCLUDED
