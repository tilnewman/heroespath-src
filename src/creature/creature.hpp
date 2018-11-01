// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_HPP_INCLUDED
#define HEROESPATH_CREATURE_HPP_INCLUDED
//
// creature.hpp
//  A base class for all creatures
//
#include "combat/effect-type-enum.hpp"
#include "creature/achievements.hpp"
#include "creature/body-type.hpp"
#include "creature/condition-enum.hpp"
#include "creature/dragon-class-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/race-enum.hpp"
#include "creature/rank.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/stat-set.hpp"
#include "creature/title-enum.hpp"
#include "creature/trait.hpp"
#include "creature/traits-set.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "gui/date-time.hpp"
#include "item/inventory.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{

// forward declarations
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongPVec_t = std::vector<SongPtr_t>;
} // namespace song
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPVec_t = std::vector<SpellPtr_t>;
} // namespace spell
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPVec_t = std::vector<ItemPtr_t>;
    using ItemPVecVec_t = std::vector<ItemPVec_t>;
} // namespace item

namespace creature
{

    // forward declarations
    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;

    class Enchantment;
    using EnchantmentPtr_t = misc::NotNull<Enchantment *>;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

    enum class UnconOpt
    {
        Include,
        Exclude
    };

    // Responsible for all state and operation of all creatures in the game, including players and
    // non-players.
    class Creature
    {
    public:
        Creature(const Creature &) = delete;
        Creature(Creature &&) = delete;
        Creature & operator=(const Creature &) = delete;
        Creature & operator=(Creature &&) = delete;

        // Note:  This constructor will add the default 'Good' status if CONDITIONS is empty.
        explicit Creature(
            const bool IS_PLAYER = false,
            const std::string & NAME = "no_name_error",
            const sex::Enum SEX = creature::sex::Unknown,
            const race::Enum & RACE = race::Human,
            const role::Enum & ROLE = role::Archer,
            const StatSet & STATS = StatSet(),
            const std::string & IMAGE_FILENAME = "",
            const Health_t & HEALTH = 0_health,
            const Rank_t & RANK = 1_rank,
            const Experience_t & EXPERIENCE = 0_exp,
            const Mana_t & MANA = 0_mana);

        const BodyType Body() const { return bodyType_; }

        bool IsPlayerCharacter() const { return isPlayer_; }

        const std::string Name() const { return name_; }
        void SetName(const std::string & N) { name_ = N; }

        const std::string NameOrRaceAndRole(const bool IS_FIRST_LETTER_CAPS = true) const;
        const std::string NameAndRaceAndRole(const bool IS_FIRST_LETTER_CAPS = true) const;

        const std::string ImageFilename() const { return imageFilename_; }
        void ImageFilename(const std::string & S) { imageFilename_ = S; }

        sex::Enum Sex() const { return sex_; }

        const std::string SexName() const { return creature::sex::ToString(sex_); }

        race::Enum Race() const { return race_; }
        const std::string RaceName() const { return race::Name(race_); }
        bool IsPixie() const { return (race_ == race::Pixie); }

        bool IsBeast() const
        {
            return ((race::HasTorso(race_) == false) || (Body().IsHumanoid() == false));
        }

        dragon_class::Enum DragonClass() const { return dragon_class::ClassFromRank(Rank()); }

        wolfen_class::Enum WolfenClass() const { return wolfen_class::ClassFromRank(Rank()); }

        Rank_t Rank() const { return rank_; }

        rank_class::Enum RankClass() const { return rank_class::FromRank(Rank()); }

        const std::string RankClassName() const;
        float RankRatio() const;

        Rank_t IncreaseRank(const Rank_t & R) { return rank_ += R; }

        Experience_t Exp() const { return experience_; }
        Experience_t IncreaseExp(const Experience_t & E) { return experience_ += E; }

        Health_t HealthCurrent() const { return healthCurrent_; }
        void HealthCurrentSet(const Health_t & H) { healthCurrent_ = H; }
        Health_t HealthNormal() const { return healthNormal_; }
        void HealthNormalSet(const Health_t & H) { healthNormal_ = H; }
        void HealthReset() { healthCurrent_ = healthNormal_; }

        Health_t HealthCurrentAdj(const Health_t &);
        Health_t HealthNormalAdj(const Health_t &);

        role::Enum Role() const { return role_; }
        const std::string RoleName() const { return role::Name(role_); }

        void TitleAdd(const Titles::Enum E, const bool ALLOW_CHANGES = true);
        const TitleEnumVec_t Titles() const { return titlesVec_; }
        const TitlePVec_t TitlesPVec() const;

        float HealthRatio() const
        {
            return (healthCurrent_.As<float>() / healthNormal_.As<float>());
        }

        Health_t HealthMissing() const { return healthNormal_ - healthCurrent_; }

        Mana_t ManaMissing() const
        {
            return Mana_t(TraitNormal(Traits::Mana) - TraitWorking(Traits::Mana));
        }

        Mana_t Mana() const { return Mana_t(TraitWorking(Traits::Mana)); }

        Mana_t ManaAdj(const Mana_t & ADJ)
        {
            return Mana_t(TraitCurrentAdj(Traits::Mana, ADJ.As<int>()));
        }

        Mana_t ManaNormal() const { return Mana_t(TraitNormal(Traits::Mana)); }

        Mana_t ManaNormalAdj(const Mana_t & ADJ)
        {
            return Mana_t(TraitNormalAdj(Traits::Mana, ADJ.As<int>()));
        }

        float ManaRatio() const { return (Mana().As<float>() / ManaNormal().As<float>()); }

        bool IsDead() const { return HasCondition(Conditions::Dead); }
        bool IsAlive() const { return !IsDead(); }

        Strength_t Strength() const { return Strength_t(TraitWorking(Traits::Strength)); }

        Accuracy_t Accuracy() const { return Accuracy_t(TraitWorking(Traits::Accuracy)); }

        Charm_t Charm() const { return Charm_t(TraitWorking(Traits::Charm)); }

        Luck_t Luck() const { return Luck_t(TraitWorking(Traits::Luck)); }

        Speed_t Speed() const { return Speed_t(TraitWorking(Traits::Speed)); }

        Intell_t Intelligence() const { return Intell_t(TraitWorking(Traits::Intelligence)); }

        const gui::DateTime DateTimeCreated() const { return dateTimeCreated_; }

        const std::string HealthPercentStr(const bool WILL_APPEND_SYMBOL = true) const;

        // returns true only if the condition was actually added, and not a duplicate, etc.
        // prevents duplicate conditions
        // calls Condition::Change() on this creature after adding
        bool ConditionAdd(const Conditions::Enum, const bool ALLOW_CHANGES = true);

        // returns true if a condition was removed
        // if there are multiple that match CND_PTR, all are removed
        // ensures the 'Good' condition is in the vector if left empty
        // calls Condition::Undo() on this creature before removing
        bool ConditionRemove(const Conditions::Enum);

        // leaves the 'Good' condition behind in the vector, so it is never really empty
        // for each Condition, Condition::Undo() is called before removing
        std::size_t ConditionRemoveAll();

        const CondEnumVec_t Conditions() const { return conditionsVec_; }
        const ConditionPVec_t ConditionsPVec() const;

        bool HasCondition(const Conditions::Enum) const;
        bool HasConditionNotAThreatTemp() const;

        bool HasConditionNotAThreatPerm(const UnconOpt UNCON_OPTION = UnconOpt::Exclude) const;

        std::size_t GetWorstConditionSeverity() const;

        bool HasMagicalCondition() const;

        // assumes descending sort, not wrapped, no and appended, and with ellipsis if needed
        const std::string ConditionNames(
            const std::size_t MAX_TO_LIST = 0,
            const size_t MIN_SEVERITY = condition::Severity::ALL);

        bool CanTakeAction() const { return CanTakeActionStr().empty(); }
        const std::string CanTakeActionStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        const item::Inventory & Inventory() const { return inventory_; }

        // these functions return false if attempt to reduce beyond zero
        bool CoinsAdj(const Coin_t & C) { return inventory_.CoinsAdj(C); }
        bool MeteorShardsAdj(const MeteorShard_t & M) { return inventory_.MeteorShardsAdj(M); }
        bool GemsAdj(const Gem_t & G) { return inventory_.GemsAdj(G); }

        // These functinons return the ITEM_ACTION_SUCCESS_STR_ (empty) string on success.
        // On failure, the string will be an explanation of the failure that can be shown to the
        // player.
        const std::string ItemAdd(const item::ItemPtr_t);
        const std::string ItemIsAddAllowed(const item::ItemPtr_t) const;
        const std::string ItemEquip(const item::ItemPtr_t);
        const std::string ItemIsEquipAllowed(const item::ItemPtr_t) const;
        const std::string ItemIsEquipAllowedByRole(const item::ItemPtr_t) const;

        // This function will not remove an equipped item.  Unequip first.
        void ItemRemove(const item::ItemPtr_t);

        const std::string ItemUnEquip(const item::ItemPtr_t);
        const std::string IsItemUnqeuipAllowed(const item::ItemPtr_t);

        void IncrementHeldWeapons();
        void SetHeldWeaponsToBest();
        void SetHeldWeaponsToBestIfInvalidated();
        const item::ItemPVec_t HeldWeapons() const { return heldWeaponsPVec_; }
        std::size_t HeldWeaponsCount() const;
        bool HasWeapons() const { return HeldWeaponsCount() > 0; }
        bool HasWeaponsHeld() const { return heldWeaponsPVec_.empty() == false; }
        bool IsHoldingProjectileWeapon() const;

        const std::string WeaponsString() const;
        const std::string ArmorString() const;

        Armor_t ArmorRating() const;

        // spell related functions
        bool CanCastSpells() const { return CanCastSpellsStr().empty(); }

        const std::string CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        bool CanCastSpellByEffectType(const combat::EffectType::Enum) const;
        bool CanCastSpellByEffectType(const combat::EffectTypeVec_t &) const;

        const spell::SpellEnumVec_t Spells() const { return spellsVec_; }

        const spell::SpellPVec_t SpellsPVec() const;

        bool SpellAdd(const spell::Spells::Enum);
        bool SpellRemove(const spell::Spells::Enum);

        // song related functions
        bool CanPlaySongs() const { return CanPlaySongsStr().empty(); }

        const std::string CanPlaySongsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        bool CanPlaySongsByEffectType(const combat::EffectType::Enum) const;
        bool CanPlaySongsByEffectType(const combat::EffectTypeVec_t &) const;

        const song::SongEnumVec_t Songs() const { return songsVec_; }

        const song::SongPVec_t SongsPVec() const;

        bool SongAdd(const song::Songs::Enum);
        bool SongRemove(const song::Songs::Enum);

        Weight_t WeightCanCarry() const;

        Achievements & GetAchievements() { return achievements_; }

        bool CanFly() const { return (race::CanFly(race_) && role::CanFly(role_)); }

        const std::string ToString() const;

        std::size_t LastSpellCastNum() const { return lastSpellCastNum_; }
        void LastSpellCastNum(const std::size_t N) { lastSpellCastNum_ = N; }

        std::size_t LastSongPlayedNum() const { return lastSongPlayedNum_; }
        void LastSongPlayedNum(const std::size_t N) { lastSongPlayedNum_ = N; }

        const EnchantmentPVec_t Enchantments() const { return enchantmentsPVec_; }

        void EnchantmentApplyOrRemove(const EnchantmentPtr_t, const bool WILL_APPLY);

        void ReCalculateTraitBonuses();

        int TraitNormal(const Traits::Enum E) const { return actualSet_.GetCopy(E).Normal(); }

        Trait_t TraitNormalAdj(const Traits::Enum E, const Trait_t ADJ)
        {
            return actualSet_.Get(E).NormalAdj(ADJ);
        }

        void TraitNormalSet(const Traits::Enum E, const Trait_t NEW_VALUE)
        {
            actualSet_.Get(E).NormalSet(NEW_VALUE);
        }

        Trait_t TraitCurrent(const Traits::Enum E) const { return actualSet_.GetCopy(E).Current(); }

        Trait_t TraitCurrentAdj(const Traits::Enum E, const Trait_t ADJ)
        {
            return actualSet_.Get(E).CurrentAdj(ADJ);
        }

        void TraitCurrentSet(const Traits::Enum E, const Trait_t NEW_VALUE)
        {
            actualSet_.Get(E).CurrentSet(NEW_VALUE);
        }

        Trait_t TraitWorking(const Traits::Enum E) const;

        Trait_t TraitBonusNormal(const Traits::Enum E) const
        {
            return bonusSet_.GetCopy(E).Normal();
        }

        Trait_t TraitBonusNormalAdj(const Traits::Enum E, const Trait_t ADJ);

        Trait_t TraitBonusCurrent(const Traits::Enum E) const
        {
            return bonusSet_.GetCopy(E).Current();
        }

        bool TraitBonusTest(const Traits::Enum) const;

        float TraitBonusActualAsRatio(const Traits::Enum E) const
        {
            return static_cast<float>(bonusSet_.GetCopy(E).Current()) / 100.0f;
        }

        const TraitSet TraitsWorking() const;

        const TraitSet TraitsBonuses() const { return bonusSet_; }

        const std::string TraitModifiedString(const Traits::Enum E, const bool WILL_WRAP) const;

        void StatTraitsModify(const StatSet &);

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

        friend bool operator==(const Creature & L, const Creature & R);
        friend bool operator<(const Creature & L, const Creature & R);

        bool ShallowCompare(const Creature &);

    protected:
        const item::ItemPVecVec_t ComposeWeaponsList() const;

        void EnchantmentsApplyOrRemoveByType(
            const EnchantmentPVec_t & PVEC,
            const EnchantmentType::Enum TYPE,
            const bool WILL_APPLY);

        bool HasEnchantmentType(const EnchantmentType::Enum) const;

    public:
        static const std::string ITEM_ACTION_SUCCESS_STR_;

    protected:
        bool isPlayer_;
        std::string name_;
        std::string imageFilename_;
        sex::Enum sex_;
        BodyType bodyType_;
        race::Enum race_;
        role::Enum role_;
        CondEnumVec_t conditionsVec_;
        TitleEnumVec_t titlesVec_;
        item::Inventory inventory_;
        gui::DateTime dateTimeCreated_;
        spell::SpellEnumVec_t spellsVec_;
        Achievements achievements_;

        // these are copies of pointers in the inventory_ and are not serialized
        item::ItemPVec_t heldWeaponsPVec_;

        std::size_t lastSpellCastNum_;
        song::SongEnumVec_t songsVec_;
        std::size_t lastSongPlayedNum_;
        Health_t healthCurrent_;
        Health_t healthNormal_;
        Rank_t rank_;
        Experience_t experience_;

        //
        // actualSet_.Normal   -The non-percent, non-bonus, positive, standing value,
        //                     unaffected by bonuses.  For example, this would be
        //                     Mana/Stats normal or max value.  See TraitNormal().
        //
        // actualSet_.Current  -The non-percent, non-bonus, positive, current value,
        //                     unaffected by bonuses.  See TraitCurrent().
        //
        // bonusSet_.Normal    -The percent, bonus, standing value.  See TraitBonusNormal().
        //                     This is a permenant bonus, not from a temporary source
        //                     (enchantment or condition), but from a permenant source
        //                     (rank increase or title).
        //
        // bonusSet_.Current   -The percent, sum of all bonuses from temp sources
        //                     (enchantments and conditions).  See TraitBonusCurrent()
        //                     and ReCalculateTraitBonuses().
        //
        //  Things with normal/current values like Mana/Stats:
        //      Get using TraitWorking() which applies bonuses and ensures >=0.
        //      Adjust normal with TraitNormalAdj() and current with TraitCurrentAdj().
        //      Get the entire set with TraitsWorking().
        //
        //  Things that are only bonuses like Backstab/HitPower/Surprise:
        //      Get with TraitBonusCurrent() which is the sum of all temp/perm sources.
        //      Set permanent bonuses with TraitBonusNormalAdj(), which calls the
        //      ReCalculateTraitBonuses() function changing bonusSet_.Current and what
        //      TraitBonusCurrent() returns.
        //      Get the entire set with TraitsBonuses();
        //
        TraitSet actualSet_;
        TraitSet bonusSet_;

        // The Creature class is not responsible for the lifetime of
        // Enchantment objects, the Item class is.  This vector of
        // pointers is just for short term observation, and is not
        // serialized with the Creature class when the game is saved.
        EnchantmentPVec_t enchantmentsPVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & isPlayer_;
            ar & name_;
            ar & imageFilename_;
            ar & sex_;
            ar & bodyType_;
            ar & race_;
            ar & role_;
            ar & conditionsVec_;
            ar & titlesVec_;
            ar & inventory_;
            ar & dateTimeCreated_;
            ar & spellsVec_;
            ar & achievements_;
            ar & lastSpellCastNum_;
            ar & songsVec_;
            ar & lastSongPlayedNum_;
            ar & healthCurrent_;
            ar & healthNormal_;
            ar & rank_;
            ar & experience_;
            ar & actualSet_;
            ar & bonusSet_;
        }
    };

    bool operator==(const Creature & L, const Creature & R);

    inline bool operator!=(const Creature & L, const Creature & R) { return !(L == R); }

    bool operator<(const Creature & L, const Creature & R);

    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
    using CreatureUPtr_t = std::unique_ptr<Creature>;
    using CreatureUPVec_t = std::vector<CreatureUPtr_t>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_HPP_INCLUDED
