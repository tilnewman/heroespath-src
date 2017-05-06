#ifndef GAME_SPELL_Spell_HPP_INCLUDED
#define GAME_SPELL_Spell_HPP_INCLUDED
//
// spell-base.hpp
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/stats/types.hpp"
#include "game/spell/spell-enum.hpp"
#include "game/spell/spell-type-enum.hpp"
#include "game/target-enum.hpp"

#include <string>
#include <vector>
#include <tuple>


namespace game
{

//forward declarations
namespace creature
{
    class Creature;
    using CreatureSPtr_t = std::shared_ptr<Creature>;
}

namespace spell
{

    //common base code to all spell classes
    class Spell
    {
    public:
        Spell(const Spells::Enum     WHICH,
              const SpellType::Enum  TYPE,
              const SpellClass::Enum CLASS,
              const stats::Mana_t    MANA_COST,
              const stats::Rank_t    RANK,
              const TargetType::Enum TARGET_TYPE);

        virtual ~Spell();

        inline const std::string Name() const       { return Spells::Name(which_); }
        
        const std::string ToString() const;

        inline const std::string Desc() const       { return Spells::ShortDesc(which_); }
        inline const std::string DescExtra() const  { return Spells::ExtraDesc(which_); }
        const std::string DescDetails() const;
        const std::string DescComplete() const;

        inline Spells::Enum Which() const           { return which_; }
        inline SpellType::Enum Type() const         { return type_; }
        inline SpellClass::Enum Class() const       { return class_; }
        inline stats::Mana_t ManaCost() const       { return manaCost_; }
        inline stats::Rank_t Rank() const           { return rank_; }
        inline TargetType::Enum TargetType() const  { return targetType_; }

        virtual const std::string ActionPhrase() const = 0;

        virtual void Cast(creature::CreatureSPtr_t & TARGET_CREATURE_SPTR) = 0;

        friend bool operator<(const Spell & L, const Spell & R);
        friend bool operator==(const Spell & L, const Spell & R);

    protected:
        Spells::Enum which_;
        stats::Rank_t rank_;
        SpellType::Enum type_;
        SpellClass::Enum class_;
        stats::Mana_t manaCost_;
        TargetType::Enum targetType_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & which_;
            ar & rank_;
            ar & type_;
            ar & class_;
            ar & manaCost_;
            ar & targetType_;
        }
    };


    using SpellPtr_t       = Spell *;
    using SpellPVec_t      = std::vector<SpellPtr_t>;
    using SpellPVecIter_t  = SpellPVec_t::iterator;
    using SpellPVecCIter_t = SpellPVec_t::const_iterator;


    inline bool operator<(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_, L.class_, L.manaCost_, L.targetType_)
                <
               std::tie(R.which_, R.rank_, R.type_, R.class_, R.manaCost_, R.targetType_);
    }

    inline bool operator==(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_, L.class_, L.manaCost_, L.targetType_)
                ==
               std::tie(R.which_, R.rank_, R.type_, R.class_, R.manaCost_, R.targetType_);
    }

    inline bool operator!=(const Spell & L, const Spell & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_SPELL_Spell_HPP_INCLUDED
