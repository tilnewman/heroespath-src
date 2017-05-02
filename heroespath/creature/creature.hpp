#ifndef APPBASE_HEROESPATH_CREATURE_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_INCLUDED
//
// creature.hpp
//  A base class for all creatures
//
#include "sfml-util/date-time.hpp"
#include "utilz/boost-serialize-includes.hpp"

#include "heroespath/creature/condition-enum.hpp"
#include "heroespath/item/types.hpp"
#include "heroespath/item/inventory.hpp"
#include "heroespath/item/item.hpp"
#include "heroespath/creature/role.hpp"
#include "heroespath/creature/race.hpp"
#include "heroespath/creature/rank.hpp"
#include "heroespath/creature/title.hpp"
#include "heroespath/creature/sex-enum.hpp"
#include "heroespath/creature/body-type.hpp"
#include "heroespath/creature/dragon-class-enum.hpp"
#include "heroespath/creature/wolfen-class-enum.hpp"
#include "heroespath/creature/achievements.hpp"
#include "heroespath/spell/spell-type-enum.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>


namespace heroespath
{
namespace spell
{
    //forward declarations
    class Spell;
    using SpellPtr_t  = Spell *;
    using SpellPVec_t = std::vector<SpellPtr_t>;
}
namespace creature
{

    //forward declarations
    class Condition;
    using ConditionSPtr_t = std::shared_ptr<Condition>;
    using ConditionSVec_t = std::vector<ConditionSPtr_t>;


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
        Creature(const Creature &);

        //prevent copy assignment
        Creature & operator=(const Creature &);

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
                 const ConditionSVec_t &     CONDITIONS_SVEC = ConditionSVec_t(),
                 const TitlePVec_t &         TITLES_PVEC     = TitlePVec_t(),
                 const item::Inventory &     INVENTORY       = item::Inventory(),
                 const sfml_util::DateTime & DATE_TIME       = sfml_util::DateTime(),
                 const std::string &         IMAGE_FILENAME  = "",
                 const spell::SpellPVec_t &  SPELL_PVEC      = spell::SpellPVec_t());

        virtual ~Creature();

        inline virtual const BodyType Body() const              { return bodyType_; }

        inline virtual bool IsPlayerCharacter() const           { return false; }

        inline virtual const std::string Name() const           { return name_; }
        inline virtual void SetName(const std::string & N)      { name_ = N; }

        virtual const std::string DisplayableNameRaceRole() const;

        inline virtual const std::string ImageFilename() const  { return imageFilename_; };
        virtual void ImageFilename(const std::string & S)       { imageFilename_ = S; };

        inline virtual sex::Enum Sex() const                    { return sex_; }
        inline virtual const std::string SexName() const        { return creature::sex::ToString(sex_); }

        inline virtual const creature::Race Race() const        { return race_; }

        inline virtual bool IsPixie() const                     { return (race_.Which() == race::Pixie); }
        inline virtual bool IsBeast() const                     { return ((race::HasTorso(race_.Which()) == false) || (Body().IsHumanoid() == false)); }

        inline virtual stats::StatSet & Stats()                 { return stats_; }

        inline virtual dragon_class::Enum DragonClass() const   { return dragon_class::ClassFromRank( Rank() ); }
        inline virtual wolfen_class::Enum WolfenClass() const   { return wolfen_class::ClassFromRank( Rank() ); }

        inline virtual std::size_t Rank() const                 { return rank_; }
        inline virtual rank_class::Enum RankClass() const       { return rank_class::FromRank(rank_); }
        virtual const std::string RankClassName() const;

        inline virtual void IncreaseRank(const stats::Rank_t R) { rank_ += R; }
        inline virtual stats::Exp_t Exp() const                 { return experience_; }
        inline virtual void IncreaseExp(const stats::Exp_t E)   { experience_ += E; }

        inline virtual const creature::Role Role() const        { return role_; }

        virtual void TitleAdd(const TitlePtr_t titlePtr, const bool ALLOW_CHANGES = true);
        inline virtual const TitlePVec_t Titles() const         { return titlesPtrVec_; }

        inline virtual stats::Health_t HealthCurrent() const            { return healthCurrent_; }
        inline virtual stats::Health_t HealthNormal() const             { return healthNormal_; }
        inline virtual void HealthCurrentAdj(const stats::Health_t C)   { healthCurrent_ += C; }
        inline virtual void HealthNormalAdj(const stats::Health_t C)    { healthNormal_ += C; }
        inline virtual void HealthCurrentSet(const stats::Health_t H)   { healthCurrent_ = H; }
        inline virtual void HealthNormalSet(const stats::Health_t H)    { healthNormal_ = H; }
        inline virtual float HealthRatio() const                        { return static_cast<float>(healthCurrent_) / static_cast<float>(healthNormal_); }
        virtual const std::string HealthPercentStr(const bool WILL_APPEND_SYMBOL = true) const;

        //returns true only if the condition was actually added, and not a duplicate, etc.
        //prevents duplicate conditions
        //calls Condition::Change() on this creature after adding
        bool ConditionAdd(const ConditionSPtr_t, const bool ALLOW_CHANGES = true);

        //returns true if a condition was removed
        //if there are multiple that match CND_PTR, all are removed
        //ensures the 'Good' condition is in the vector if left empty
        //calls Condition::Undo() on this creature before removing
        bool ConditionRemove(const condition::Enum);

        bool ConditionRemove(const ConditionSPtr_t &);

        //leaves the 'Good' condition behind in the vector, so it is never really empty
        //for each Condition, Condition::Undo() is called before removing
        std::size_t ConditionRemoveAll();

        inline const ConditionSVec_t Conditions() const                 { return conditionsSVec_; }

        bool HasCondition(const condition::Enum E) const;
        bool HasConditionNotAThreatTemp() const;
        bool HasConditionNotAThreatPerm() const;

        std::size_t GetWorstConditionSeverity() const;

        bool HasMagicalCondition() const;

        inline bool IsDead() const                                      { return HasCondition(condition::Dead); }
        inline bool IsAlive() const                                     { return ! IsDead(); }

        //returns most severe first
        //a count of zero means 'list all'
        virtual const std::string ConditionList(const std::size_t MAX_TO_LIST_COUNT = 0,
                                                const size_t      SEVERITY_AT_LEAST = 0,
                                                const bool        WILL_WRAP         = false,
                                                const bool        WILL_AND          = false);

        inline virtual bool CanTakeAction() const                       { return CanTakeActionStr().empty(); }
        virtual const std::string CanTakeActionStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        inline  const item::Inventory Inventory() const                 { return inventory_; }

        //these functions return false if attempt to reduce beyond zero
        inline virtual bool CoinsAdj(const item::Coin_t A)              { return inventory_.CoinsAdj(A); }
        inline virtual bool MeteorShardsAdj(const item::Meteor_t A)     { return inventory_.MeteorShardsAdj(A); }
        inline virtual bool GemsAdj(const item::Gem_t A)                { return inventory_.GemsAdj(A); }

        //These functinons return the ITEM_ACTION_SUCCESS_STR_ (empty) string on success.
        //On failure, the string will be an explanation of the failure that can be shown to the player.
        virtual const std::string ItemAdd(const item::ItemSPtr_t & ITEM_SPTR);
        virtual const std::string ItemIsAddAllowed(const item::ItemSPtr_t & ITEM_SPTR) const;
        virtual const std::string ItemEquip(const item::ItemSPtr_t & ITEM_SPTR);
        virtual const std::string ItemIsEquipAllowed(const item::ItemSPtr_t & ITEM_SPTR) const;
        virtual const std::string ItemIsEquipAllowedByRole(const item::ItemSPtr_t & ITEM_SPTR) const;

        //This function will not remove an equipped item.  Unequip first.
        virtual void ItemRemove(const item::ItemSPtr_t & ITEM_SPTR);

        virtual const std::string ItemUnEquip(const item::ItemSPtr_t & ITEM_SPTR);
        virtual const std::string IsItemUnqeuipAllowed(const item::ItemSPtr_t & ITEM_SPTR);

        virtual const item::ItemSVec_t CurrentWeaponsInc();
        virtual void SetCurrentWeaponsToBest();
        virtual void SetCurrentWeaponsToBestIfInvalidated();
        virtual const item::ItemSVec_t CurrentWeaponsCopy() const   { return currWeaponsSVec_; }
        virtual std::size_t WeaponsCount() const;
        inline virtual bool HasWeapons() const                      { return WeaponsCount() > 0; }
        inline virtual bool HasWeaponsHeld() const                  { return CurrentWeaponsCopy().empty() == false; }
        virtual bool IsHoldingProjectileWeapon() const;

        virtual const std::string WeaponsString() const;
        virtual const std::string ArmorString() const;

        virtual stats::Armor_t ArmorRating() const;

        inline virtual const sfml_util::DateTime DateTimeCreated() const    { return dateTimeCreated_; }

        inline virtual const UniqueTraits_t UniqueTraits() const            { return UniqueTraits_t( std::make_tuple(Name(), Role().Which(), DateTimeCreated(), serialNumber_) ); }

        inline virtual bool CanCastSpells() const                           { return CanCastSpellsStr().empty(); }
        virtual const std::string CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;
        virtual bool CanCastSpellByType(const spell::SpellType::Enum) const;

        inline virtual spell::SpellPVec_t Spells()                          { return spellsPVec_; }

        item::Weight_t WeightCanCarry() const;

        inline const Achievements AchievementsCopy() const                  { return achievements_; }

        inline bool CanFly() const                                          { return (race::CanFly(race_.Which()) && role::CanFly(role_.Which())); }

        virtual const std::string ToString() const;

        const stats::StatSet DivideStatsAndCreateInverseModifyStatSet(const float   STR_DIVISOR = 1.0f,
                                                                      const float   ACC_DIVISOR = 1.0f,
                                                                      const float   CHA_DIVISOR = 1.0f,
                                                                      const float   LCK_DIVISOR = 1.0f,
                                                                      const float   SPD_DIVISOR = 1.0f,
                                                                      const float   INT_DIVISOR = 1.0f);

        stats::Stat_t DivideAndGetInverseModifier(const stats::stat::Enum STAT_ENUM,
                                                  const float             DIVIDE_BY);


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
        ConditionSVec_t     conditionsSVec_;
        TitlePVec_t         titlesPtrVec_;
        item::Inventory     inventory_;
        sfml_util::DateTime dateTimeCreated_;
        spell::SpellPVec_t  spellsPVec_;
        Achievements        achievements_;
        item::ItemSVec_t    currWeaponsSVec_;

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
            ar & conditionsSVec_;
            ar & titlesPtrVec_;
            ar & inventory_;
            ar & dateTimeCreated_;
            ar & spellsPVec_;
            ar & achievements_;
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
#endif //APPBASE_HEROESPATH_CREATURE_INCLUDED
