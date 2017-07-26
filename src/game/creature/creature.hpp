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
#ifndef GAME_CREATURE_INCLUDED
#define GAME_CREATURE_INCLUDED
//
// creature.hpp
//  A base class for all creatures
//
#include "sfml-util/date-time.hpp"
#include "misc/boost-serialize-includes.hpp"

#include "game/item/types.hpp"
#include "game/item/inventory.hpp"
#include "game/creature/enchantment-type.hpp"
#include "game/creature/condition-enum.hpp"
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
#include "game/effect-type-enum.hpp"
#include "game/song/song-enum.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>


namespace game
{

//forward declarations
namespace song
{
    class Song;
    using SongPtr_t = Song *;
    using SongPVec_t = std::vector<SongPtr_t>;
}
namespace spell
{
    class Spell;
    using SpellPtr_t  = Spell *;
    using SpellPVec_t = std::vector<SpellPtr_t>;
}
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
    using ItemPVec_t = std::vector<ItemPtr_t>;
    using ItemPVecVec_t = std::vector<ItemPVec_t>;
}

namespace creature
{

    //forward declarations
    class Condition;
    using ConditionPtr_t  = Condition *;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;

    class Enchantment;
    using EnchantmentPtr_t = Enchantment *;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;


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
        explicit Creature(const std::string &         NAME            = "no_name_error",
                          const sex::Enum             SEX             = creature::sex::Unknown,
                          const BodyType &            BODY_TYPE       = BodyType(),
                          const creature::Race &      RACE            = creature::Race(creature::race::Count_PlayerRaces),
                          const creature::Role &      ROLE            = creature::Role(creature::role::PlayerRoleCount),
                          const stats::StatSet &      STATS           = stats::StatSet(),
                          const stats::Health_t       HEALTH          = 0,
                          const stats::Rank_t         RANK            = 1,
                          const stats::Exp_t          EXPERIENCE      = 0,
                          const CondEnumVec_t &  CONDITIONS_VEC  = CondEnumVec_t(),
                          const TitleEnumVec_t &      TITLES_VEC      = TitleEnumVec_t(),
                          const item::Inventory &     INVENTORY       = item::Inventory(),
                          const sfml_util::DateTime & DATE_TIME       = sfml_util::DateTime(),
                          const std::string &         IMAGE_FILENAME  = "",
                          const spell::SpellVec_t &   SPELL_VEC       = spell::SpellVec_t(),
                          const stats::Mana_t         MANA            = 0,
                          const song::SongVec_t &     SONG_VEC        = song::SongVec_t());

        virtual ~Creature();

        inline const BodyType Body() const                      { return bodyType_; }

        inline virtual bool IsPlayerCharacter() const           { return false; }

        inline const std::string Name() const                   { return name_; }
        inline void SetName(const std::string & N)              { name_ = N; }

        const std::string NameOrRaceAndRole(const bool IS_FIRST_LETTER_CAPS = true) const;
        const std::string NameAndRaceAndRole(const bool IS_FIRST_LETTER_CAPS = true) const;

        inline const std::string ImageFilename() const          { return imageFilename_; }
        void ImageFilename(const std::string & S)               { imageFilename_ = S; }

        inline sex::Enum Sex() const                            { return sex_; }
        inline const std::string SexName() const                { return creature::sex::ToString(sex_); }

        inline const creature::Race Race() const                { return race_; }

        inline bool IsPixie() const                             { return (race_.Which() == race::Pixie); }
        inline bool IsBeast() const                             { return ((race::HasTorso(race_.Which()) == false) || (Body().IsHumanoid() == false)); }

        inline stats::StatSet & Stats()                         { return stats_; }

        void ReCalculateStats();

        inline dragon_class::Enum DragonClass() const           { return dragon_class::ClassFromRank( Rank() ); }
        inline wolfen_class::Enum WolfenClass() const           { return wolfen_class::ClassFromRank( Rank() ); }

        inline stats::Rank_t Rank() const                       { return rank_; }
        inline rank_class::Enum RankClass() const               { return rank_class::FromRank(rank_); }
        const std::string RankClassName() const;
        float RankRatio() const;

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
        
        inline float HealthRatio() const
        {
            return static_cast<float>(healthCurrent_) / static_cast<float>(healthNormal_);
        }
        
        const std::string HealthPercentStr(const bool WILL_APPEND_SYMBOL = true) const;
        
        inline stats::Health_t HealthMissing() const
        {
            return HealthNormal() - HealthCurrent();
        }
        
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

        inline const CondEnumVec_t Conditions() const      { return conditionsVec_; }
        const ConditionPVec_t ConditionsPVec() const;

        bool HasCondition(const Conditions::Enum) const;
        bool HasConditionNotAThreatTemp() const;
        bool HasConditionNotAThreatPerm(const bool WILL_INCLUDE_UNCONSCIOUS = false) const;

        std::size_t GetWorstConditionSeverity() const;

        bool HasMagicalCondition() const;

        inline bool IsDead() const                              { return HasCondition(Conditions::Dead); }
        inline bool IsAlive() const                             { return ! IsDead(); }

        //assumes descending sort, not wrapped, no and appended, and with ellipsis if needed
        const std::string ConditionNames(const std::size_t MAX_TO_LIST  = 0,
                                         const size_t      MIN_SEVERITY = condition::Severity::ALL);

        inline bool CanTakeAction() const                       { return CanTakeActionStr().empty(); }
        const std::string CanTakeActionStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        inline const item::Inventory & Inventory() const         { return inventory_; }

        //these functions return false if attempt to reduce beyond zero
        inline bool CoinsAdj(const item::Coin_t A)              { return inventory_.CoinsAdj(A); }
        inline bool MeteorShardsAdj(const item::Meteor_t A)     { return inventory_.MeteorShardsAdj(A); }
        inline bool GemsAdj(const item::Gem_t A)                { return inventory_.GemsAdj(A); }

        //These functinons return the ITEM_ACTION_SUCCESS_STR_ (empty) string on success.
        //On failure, the string will be an explanation of the failure that can be shown to the player.
        const std::string ItemAdd(const item::ItemPtr_t);
        const std::string ItemIsAddAllowed(const item::ItemPtr_t) const;
        const std::string ItemEquip(const item::ItemPtr_t);
        const std::string ItemIsEquipAllowed(const item::ItemPtr_t) const;
        const std::string ItemIsEquipAllowedByRole(const item::ItemPtr_t) const;

        //This function will not remove an equipped item.  Unequip first.
        void ItemRemove(const item::ItemPtr_t);

        const std::string ItemUnEquip(const item::ItemPtr_t);
        const std::string IsItemUnqeuipAllowed(const item::ItemPtr_t);

        const item::ItemPVec_t CurrentWeaponsInc();
        void SetCurrentWeaponsToBest();
        void SetCurrentWeaponsToBestIfInvalidated();
        inline const item::ItemPVec_t CurrentWeaponsCopy() const{ return currWeaponsPVec_; }
        std::size_t WeaponsCount() const;
        inline bool HasWeapons() const                          { return WeaponsCount() > 0; }
        inline bool HasWeaponsHeld() const                      { return CurrentWeaponsCopy().empty() == false; }
        bool IsHoldingProjectileWeapon() const;

        const std::string WeaponsString() const;
        const std::string ArmorString() const;

        stats::Armor_t ArmorRating() const;

        inline const sfml_util::DateTime DateTimeCreated() const{ return dateTimeCreated_; }

        inline const UniqueTraits_t UniqueTraits() const        { return UniqueTraits_t( std::make_tuple(Name(), Role().Which(), DateTimeCreated(), serialNumber_) ); }

        //spell related functions
        inline bool CanCastSpells() const                       { return CanCastSpellsStr().empty(); }

        const std::string CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        bool CanCastSpellByEffectType(const EffectType::Enum) const;
        bool CanCastSpellByEffectType(const EffectTypeVec_t &) const;

        inline spell::SpellVec_t Spells() const                 { return spellsVec_; }

        const spell::SpellPVec_t SpellsPVec() const;

        bool SpellAdd(const spell::Spells::Enum);
        bool SpellRemove(const spell::Spells::Enum);

        //song related functions
        inline bool CanPlaySongs() const { return CanPlaySongsStr().empty(); }

        const std::string CanPlaySongsStr(const bool WILL_PREFIX_AND_POSTFIX = true) const;

        bool CanPlaySongsByEffectType(const EffectType::Enum) const;
        bool CanPlaySongsByEffectType(const EffectTypeVec_t &) const;

        inline song::SongVec_t Songs() const { return songsVec_; }

        const song::SongPVec_t SongsPVec() const;

        bool SongAdd(const song::Songs::Enum);
        bool SongRemove(const song::Songs::Enum);

        item::Weight_t WeightCanCarry() const;

        inline Achievements & GetAchievements()                 { return achievements_; }

        inline bool CanFly() const                              { return (race::CanFly(race_.Which()) && role::CanFly(role_.Which())); }

        const std::string ToString() const;

        inline stats::Mana_t ManaCurrent() const                { return manaCurrent_; }
        inline void ManaCurrentSet(const stats::Mana_t M)       { manaCurrent_ = M; }
        void ManaCurrentAdj(const stats::Mana_t ADJ);

        inline stats::Mana_t ManaNormal() const                 { return manaNormal_; }
        inline void ManaNormalSet(const stats::Mana_t M)        { manaNormal_ = M; }
        void ManaNormalAdj(const stats::Mana_t ADJ);

        inline stats::Mana_t ManaMissing() const
        {
            return (ManaNormal() - ManaCurrent());
        }

        //should only be called after loading a saved game
        void StoreItemsInWarehouseAfterLoad();

        std::size_t LastSpellCastNum() const                    { return lastSpellCastNum_; }
        void LastSpellCastNum(const std::size_t N)              { lastSpellCastNum_ = N; }

        std::size_t LastSongPlayedNum() const                   { return lastSongPlayedNum_; }
        void LastSongPlayedNum(const std::size_t N)             { lastSongPlayedNum_ = N; }

        inline const EnchantmentPVec_t Enchantments() const     { return enchantmentsPVec_; }

        void EnchantmentApplyOrRemove(const EnchantmentPtr_t, const bool WILL_APPLY);

        friend bool operator==(const Creature & L, const Creature & R);
        friend bool operator<(const Creature & L, const Creature & R);

    protected:
        const item::ItemPVecVec_t ComposeWeaponsList() const;

        void HandleEnachantments(const EnchantmentPVec_t &   PVEC,
                                 const EnchantmentType::Enum TYPE,
                                 const bool                  WILL_APPLY);

        bool HasEnchantmentType(const EnchantmentType::Enum) const;

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
        CondEnumVec_t       conditionsVec_;
        TitleEnumVec_t      titlesVec_;
        item::Inventory     inventory_;
        sfml_util::DateTime dateTimeCreated_;
        spell::SpellVec_t   spellsVec_;
        Achievements        achievements_;
        item::ItemPVec_t    currWeaponsPVec_;
        stats::Mana_t       manaCurrent_;
        stats::Mana_t       manaNormal_;
        std::size_t         lastSpellCastNum_;
        song::SongVec_t     songsVec_;
        std::size_t         lastSongPlayedNum_;

        //The Creature class is not responsible for the lifetime of
        //Enchantment objects, the Item class is.  This vector of
        //pointers is just for short term observation, and is not
        //serialized with the Creature class when the game is saved.
        EnchantmentPVec_t enchantmentsPVec_;

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
            ar & currWeaponsPVec_;
            ar & manaCurrent_;
            ar & manaNormal_;
            ar & lastSpellCastNum_;
            ar & songsVec_;
            ar & lastSongPlayedNum_;
        }
    };


    bool operator==(const Creature & L, const Creature & R);

    inline bool operator!=(const Creature & L, const Creature & R)
    {
        return ! (L == R);
    }

    bool operator<(const Creature & L, const Creature & R);


    using CreaturePtr_t   = Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtr_t  = const Creature *;
    using CreatureCPtrC_t = const Creature * const;
    using CreatureUPtr_t  = std::unique_ptr<Creature>;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
    
}
}
#endif //GAME_CREATURE_INCLUDED
