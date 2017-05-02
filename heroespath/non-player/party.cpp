//
// party.hpp (non-player)
//
#include "party.hpp"

#include "utilz/boost-string-includes.hpp"

#include "heroespath/non-player/character.hpp"

#include <map>
#include <sstream>
#include <utility>


namespace heroespath
{
namespace non_player
{


    Party::Party(const CharacterSVec_t & CHARACTERS_SVEC)
    :
        charactersSVec_(CHARACTERS_SVEC)
    {}


    Party::~Party()
    {}


    bool Party::Add(const CharacterSPtr_t & CHARACTER_SPTR)
    {
        if (charactersSVec_.size() < charactersSVec_.max_size())
        {
            charactersSVec_.push_back(CHARACTER_SPTR);
            return true;
        }
        else
            return false;
    }



    bool Party::Remove(const CharacterSPtr_t & CHARACTER_SPTR)
    {
        const CharacterSVecCIter_t FOUND_ITER( std::find(charactersSVec_.begin(), charactersSVec_.end(), CHARACTER_SPTR) );

        if (FOUND_ITER == charactersSVec_.end())
            return false;
        else
        {
            charactersSVec_.erase(FOUND_ITER);
            return true;
        }
    }


    CharacterSPtr_t Party::FindByCreaturePtr(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        for (auto const & NEXT_CHARACTER_SPTR : charactersSVec_)
            if (NEXT_CHARACTER_SPTR.get() == CREATURE_CPTRC)
                return NEXT_CHARACTER_SPTR;

        return CharacterSPtr_t();
    }


    const std::string Party::Summary() const
    {
        using RaceRolePair_t = std::pair<std::string, std::string>;
        using RaceRoleCountMap_t = std::map<RaceRolePair_t, std::size_t>;
        RaceRoleCountMap_t raceRoleMap;

        //count all race/role combinations
        for (auto const & NEXT_CHAR_SPTR : charactersSVec_)
            raceRoleMap[std::make_pair(NEXT_CHAR_SPTR->Race().Name(), NEXT_CHAR_SPTR->Role().Name())]++;

        //make a single string summary of all race/role combinations
        const std::size_t RACEROLE_PAIR_COUNT(raceRoleMap.size());
        std::size_t raceRolePairCounter(0);
        std::ostringstream ss;
        for (auto const & NEXT_RACEROLECOUNT_PAIR : raceRoleMap)
            ss << NEXT_RACEROLECOUNT_PAIR.second
               << " " << NEXT_RACEROLECOUNT_PAIR.first.first
               << " " << NEXT_RACEROLECOUNT_PAIR.first.second
               << ((NEXT_RACEROLECOUNT_PAIR.second > 1) ? "s" : "")
               << (((RACEROLE_PAIR_COUNT >= 2) && (raceRolePairCounter++ == (RACEROLE_PAIR_COUNT - 2))) ? ", and " : ", ");

        return boost::algorithm::erase_last_copy(ss.str(), ", ");
    }

}
}
