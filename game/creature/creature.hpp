#ifndef GAME_CREATURE_INCLUDED
#define GAME_CREATURE_INCLUDED
//
// creature.hpp
//  A base class for all creatures
//
#include "sfml-util/date-time.hpp"
#include "utilz/boost-serialize-includes.hpp"

#include "game/creature/condition-enum.hpp"
#include "game/item/types.hpp"
#include "game/item/inventory.hpp"
#include "game/item/item.hpp"
#include "game/creature/role.hpp"
#include "game/creature/race.hpp"
#include "game/creature/rank.hpp"
#include "game/creature/title-enum.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/body-type.hpp"
#include "game/creature/dragon-class-enum.hpp"
#include "game/creature/wolfen-class-enum.hpp"
#include "game/creature/achievements.hpp"
#include "game/spell/spell-enum.hpp"
#include "game/spell/spell-type-enum.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>


namespace game
{

//forward declarations
namespace spell
{
    class Spell;
    using SpellPtr_t  = Spell *;
    using SpellPVec_t = std::vector<SpellPtr_t>;
}

namespace creature
{

    //forward declarations
    class Condition;
    using ConditionPtr_t  = Condition *;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;


    //unique ID for all creatures
    using SerialNumber_t = unsigned long long;

    //encapsulates everything permenant and unique about a creature
    //name/role/date-time-created/serial-number
    //Only non-player characters use the serial-number
    using UniqueTraits_t = std::tuple<std::string, role::Enum, sfml_util::DateTime, SerialNumber_t>;


    //base class for all creatures
    class Creature
    {
        //prevent copy construction
        Creature(const Creature &) =delete;

        //prevent copy assignment
        Creature & operator=(const Creature &) =delete;

    public:
        //Note:  This constructor will add the default 'Good' status if CONDITIONS is empty.
        Creature(const std::string &         NAME            = "no_name_error",
                 const sex::Enum             SEX             = creature::sex::Unknown,
                 const BodyType &            BODY_TYPE       = BodyType(),
                 const creature::Race &      RACE            = creature::Race(creature::race::Count_PlayerRaces),
                 const creature::Role &      ROLE            = creature::Role(creature::role::PlayerRoleCount),
                 const stats::StatSet &      STATS           = stats::StatSet(),
                 const stats::Health_t       HEALTH          = 0,
                 const stats::Rank_t         RANK            = 1,
                 const stats::Exp_t          EXPERIENCE      = 0,
                 const ConditionEnumVec_t &  CONDITIONS_VEC  = ConditionEnumVec_t(),
                 const TitleEnumVec_t &      TITLES_VEC      = TitleEnumVec_t(),
                 const item::Inventory &     INVENTORY       = item::Inventory(),
                 const sfml_util::DateTime & DATE_TIME       = sfml_util::DateTime(),
                 const std::string &         IMAGE_FILENAME  = "",
                 const spell::SpellVec_t &   SPELL_VEC       = spell::SpellVec_t(),
                 const stats::Mana_t         MANA            = 0);

        virtual ~Creature();

        inline const BodyType Body() const                      { return bodyType_; }

        inline bool IsPlayerCharacter() const                   { return false; }

        inline const std::string Name() const                   { return name_; }
        inline void SetName(const std::string & N)              { name_ = N; }

        const std::string NameOrRaceAndClass(const bool IS_FIRST_LETTER_CAPS = true) const;

        const std::string DisplayableNameRaceRole() const;

        inline const std::string ImageFilename() const          { return imageFilename_; };
        void ImageFilename(const std::string & S)               { imageFilename_ = S; };

        inline sex::Enum Sex() const                            { return sex_; }
        inline const std::string SexName() const                { return creature::sex::ToString(sex_); }

        inline const creature::Race Race() const                { return race_; }

        inline bool IsPixie() const                             { return (race_.Which() == race::Pixie); }
        inline bool IsBeast() const                             { return ((race::HasTorso(race_.Which()) == false) || (Body().IsHumanoid() == false)); }

        inline stats::StatSet & Stats()                         { return stats_; }

        void ReCalculateStats();

        inline dragon_class::Enum DragonClass() const           { return dragon_class::ClassFromRank( Rank() ); }
        inline wolfen_class::Enum WolfenClass() const           { return wolfen_class::ClassFromRank( Rank() ); }

        inline std::size_t Rank() const                         { return rank_; }
        inline rank_class::Enum RankClass() const               { return rank_class::FromRank(rank_); }
        const std::string RankClassName() const;

        inline void IncreaseRank(const stats::Rank_t R)         { rank_ += R; }
        inline stats::Exp_t Exp() const                         { return experience_; }
        inline void IncreaseExp(const stats::Exp_t E)           { experience_ += E; }

        inline const creature::Role Role() const                { return role_; }

        void TitleAdd(const Titles::Enum E, const bool ALLOW_CHANGES = true);
        inline const TitleEnumVec_t Titles() const              { return titlesVec_; }
        const TitlePVec_t TitlesPVec() const;

        inline stats::Health_t HealthCurrent() const            { return healthCurrent_; }
        inline stats::Health_t HealthNormal() const             { return healthNormal_; }
        inline void HealthCurrentAdj(const stats::Health_t C)   { healthCurrent_ += C; }
        inline void HealthNormalAdj(const stats::Health_t C)    { healthNormal_ += C; }
        inline void HealthCurrentSet(const stats::Health_t H)   { healthCurrent_ = H; }
        inline void HealthNormalSet(const stats::Health_t H)    { healthNormal_ = H; }
        inline float HealthRatio() const                        { return static_cast<float>(healthCurrent_) / static_cast<float>(healthNormal_); }
        const std::string HealthPercentStr(const bool WILL_APPEND_SYMBOL = true) const;

        //returns true only if the condition was actually added, and not a duplicate, etc.
        //prevents duplicate conditions
        //calls Condition::Change() on this creature after adding
        bool ConditionAdd(const Conditions::Enum, const bool ALLOW_CHANGES = true);

        //returns true if a condition was removed
        //if there are multiple that match CND_PTR, all are removed
        //ensures the 'Good' condition is in the vector if left empty
        //calls Condition::Undo() on this creature before removing
        bool ConditionRemove(const Conditions::Enum);

        //leaves the 'Good' condition behind in the vector, so it is never really empty
        //for each Condition, Condition::Undo() is called before removing
        std::size_t ConditionRemoveAll();

        inline const ConditionEnumVec_t Conditions() const      { return conditionsVec_; }
        const ConditionPVec_t ConditionsPVec() const;

        bool HasCondition(const Conditions::Enum) const;
        bool HasConditionNotAThreatTemp() const;
        bool HasConditionNotAThreatPerm() const;

        std::size_t GetWorstConditionSeverity() const;

        bool HasMagicalCondition() const;

        inline bool IsDead() const                              { return HasCondition(Conditions::Dead); }
        inline bool IsAlive() const                             { return ! IsDead(); }

        //returns most severe first
        //a count of zero means 'list all'
        const std::string ConditionList(const std::size_t MAX_TO_LIST_COUNT = 0,
                                        const size_t      SEVERITY_AT_LEAST = 0,
                                        const bool        WILL_WRAP         = false,
                                        const bool        WILL_AND          = false,
                                        const bool        WILL_ELLIPSIS     = false);

        inline bool CanTakeAction() const                       { return CanTakeActionStr().empty(); }
        const std::string CanTakeActionStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        inline  const item::Inventory Inventory() const         { return inventory_; }

        //these functions return false if attempt to reduce beyond zero
        inline bool CoinsAdj(const item::Coin_t A)              { return inventory_.CoinsAdj(A); }
        inline bool MeteorShardsAdj(const item::Meteor_t A)     { return inventory_.MeteorShardsAdj(A); }
        inline bool GemsAdj(const item::Gem_t A)                { return inventory_.GemsAdj(A); }

        //These functinons return the ITEM_ACTION_SUCCESS_STR_ (empty) string on success.
        //On failure, the string will be an explanation of the failure that can be shown to the player.
        const std::string ItemAdd(const item::ItemSPtr_t & ITEM_SPTR);
        const std::string ItemIsAddAllowed(const item::ItemSPtr_t & ITEM_SPTR) const;
        const std::string ItemEquip(const item::ItemSPtr_t & ITEM_SPTR);
        const std::string ItemIsEquipAllowed(const item::ItemSPtr_t & ITEM_SPTR) const;
        const std::string ItemIsEquipAllowedByRole(const item::ItemSPtr_t & ITEM_SPTR) const;

        //This function will not remove an equipped item.  Unequip first.
        void ItemRemove(const item::ItemSPtr_t & ITEM_SPTR);

        const std::string ItemUnEquip(const item::ItemSPtr_t & ITEM_SPTR);
        const std::string IsItemUnqeuipAllowed(const item::ItemSPtr_t & ITEM_SPTR);

        const item::ItemSVec_t CurrentWeaponsInc();
        void SetCurrentWeaponsToBest();
        void SetCurrentWeaponsToBestIfInvalidated();
        const item::ItemSVec_t CurrentWeaponsCopy() const       { return currWeaponsSVec_; }
        std::size_t WeaponsCount() const;
        inline bool HasWeapons() const                          { return WeaponsCount() > 0; }
        inline bool HasWeaponsHeld() const                      { return CurrentWeaponsCopy().empty() == false; }
        bool IsHoldingProjectileWeapon() const;

        const std::string WeaponsString() const;
        const std::string ArmorString() const;

        stats::Armor_t ArmorRating() const;

        inline const sfml_util::DateTime DateTimeCreated() const{ return dateTimeCreated_; }

        inline const UniqueTraits_t UniqueTraits() const        { return UniqueTraits_t( std::make_tuple(Name(), Role().Which(), DateTimeCreated(), serialNumber_) ); }

        inline bool CanCastSpells() const                       { return CanCastSpellsStr().empty(); }
        const std::string CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;
        bool CanCastSpellByType(const spell::SpellType::Enum) const;

        inline spell::SpellVec_t Spells() const                 { return spellsVec_; }

        const spell::SpellPVec_t SpellsPVec() const;

        item::Weight_t WeightCanCarry() const;

        inline const Achievements AchievementsCopy() const      { return achievements_; }

        inline bool CanFly() const                              { return (race::CanFly(race_.Which()) && role::CanFly(role_.Which())); }

        const std::string ToString() const;

        inline stats::Mana_t ManaCurrent() const                { return manaCurrent_; }
        inline void ManaCurrentSet(const stats::Mana_t M)       { manaCurrent_ = M; }
        void ManaCurrentAdj(const stats::Mana_t ADJ);

        inline stats::Mana_t ManaNormal() const                 { return manaNormal_; }
        inline void ManaNormalSet(const stats::Mana_t M)        { manaNormal_ = M; }
        void ManaNormalAdj(const stats::Mana_t ADJ);

        friend bool operator==(const Creature & L, const Creature & R);
        friend bool operator<(const Creature & L, const Creature & R);

    protected:
        const item::ItemSVecVec_t ComposeWeaponsList() const;

    public:
        static const std::string ITEM_ACTION_SUCCESS_STR_;
        static std::size_t globalSerialNumber_;

    protected:
        std::string         name_;
        std::string         imageFilename_;
        sex::Enum           sex_;
        BodyType            bodyType_;
        creature::Race      race_;
        creature::Role      role_;
        stats::StatSet      stats_;
        SerialNumber_t      serialNumber_;
        stats::Health_t     healthCurrent_;
        stats::Health_t     healthNormal_;
        stats::Rank_t       rank_;
        stats::Exp_t        experience_;
        ConditionEnumVec_t  conditionsVec_;
        TitleEnumVec_t      titlesVec_;
        item::Inventory     inventory_;
        sfml_util::DateTime dateTimeCreated_;
        spell::SpellVec_t   spellsVec_;
        Achievements        achievements_;
        item::ItemSVec_t    currWeaponsSVec_;
        stats::Mana_t       manaCurrent_;
        stats::Mana_t       manaNormal_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & imageFilename_;
            ar & sex_;
            ar & bodyType_;
            ar & race_;
            ar & role_;
            ar & stats_;
            ar & serialNumber_;
            ar & healthCurrent_;
            ar & healthNormal_;
            ar & rank_;
            ar & experience_;
            ar & conditionsVec_;
            ar & titlesVec_;
            ar & inventory_;
            ar & dateTimeCreated_;
            ar & spellsVec_;
            ar & achievements_;
            ar & manaCurrent_;
            ar & manaNormal_;
        }
    };


    bool operator==(const Creature & L, const Creature & R);

    bool operator!=(const Creature & L, const Creature & R);

    bool operator<(const Creature & L, const Creature & R);


    using CreaturePtr_t   = Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtr_t  = const Creature *;
    using CreatureCPtrC_t = const Creature * const;
    using CreatureSPtr_t  = std::shared_ptr<Creature>;

    using CreaturePVec_t     = std::vector<CreaturePtr_t>;
    using CreatureSVec_t     = std::vector<CreatureSPtr_t>;
    using CreatureSVecIter_t = CreatureSVec_t::iterator;
    using CreatureSVecCIter_ = CreatureSVec_t::const_iterator;

}
}
#endif //GAME_CREATURE_INCLUDED
