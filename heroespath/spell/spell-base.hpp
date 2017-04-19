#ifndef HEROESPATH_SPELL_Spell_HPP_INCLUDED
#define HEROESPATH_SPELL_Spell_HPP_INCLUDED
//
// spell-base.hpp
//
#include "sfml-util/boost-serialize-includes.hpp"

#include "heroespath/stats/types.hpp"
#include "heroespath/spell/spell-enum.hpp"
#include "heroespath/spell/spell-type-enum.hpp"

#include <string>
#include <vector>
#include <tuple>


namespace heroespath
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
        Spell(const Spells::Enum    WHICH,
              const SpellType::Enum TYPE,
              const stats::Rank_t   RANK);

        virtual ~Spell();

        inline virtual const std::string Name() const   { return Spells::Name(which_); }
        inline virtual const std::string Desc() const   { return Spells::Desc(which_); }
        inline virtual Spells::Enum Which() const       { return which_; }
        inline virtual SpellType::Enum Type() const     { return type_; }

        virtual const std::string ToString() const;
        virtual const std::string LongDesc() const;

        virtual stats::Rank_t Rank() const = 0;
        virtual const std::string ActionPhrase() const = 0;

        virtual void Cast(creature::CreatureSPtr_t & TARGET_CREATURE_SPTR) = 0;

        friend bool operator<(const Spell & L, const Spell & R);
        friend bool operator==(const Spell & L, const Spell & R);

    protected:
        Spells::Enum which_;
        stats::Rank_t rank_;
        SpellType::Enum type_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & which_;
            ar & rank_;
            ar & type_;
        }
    };


    using SpellPtr_t       = Spell *;
    using SpellPVec_t      = std::vector<SpellPtr_t>;
    using SpellPVecIter_t  = SpellPVec_t::iterator;
    using SpellPVecCIter_t = SpellPVec_t::const_iterator;


    inline bool operator<(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_) < std::tie(R.which_, R.rank_, R.type_);
    }

    inline bool operator==(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_) == std::tie(R.which_, R.rank_, R.type_);
    }

    inline bool operator!=(const Spell & L, const Spell & R)
    {
        return ! (L == R);
    }

}
}
#endif //HEROESPATH_SPELL_Spell_HPP_INCLUDED
