#ifndef APPBASE_HEROESPATH_NONPLAYER_PARTY_INCLUDED
#define APPBASE_HEROESPATH_NONPLAYER_PARTY_INCLUDED
//
// party.hpp
//  A collection of characters NOT under control of the user.
//
#include "sfml-util/boost-serialize-includes.hpp"

#include <memory>
#include <string>
#include <vector>


namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtr_t  = const Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
}
namespace non_player
{

    //forward declarations
    class Character;
    using CharacterSPtr_t = std::shared_ptr<Character>;
    using CharacterSVec_t = std::vector<CharacterSPtr_t>;


    //encapsulates a set of Characters under control of the user
    class Party
    {
        //prevent copy construction
        Party(const Party &);

        //prevent copy assignment
        Party & operator=(const Party &);

    public:
        Party(const CharacterSVec_t & CHARACTER_SVEC = CharacterSVec_t());
        virtual ~Party();

        inline const CharacterSVec_t Characters() const { return charactersSVec_; }

        bool Add(const CharacterSPtr_t & CHARACTER_SPTR);
        bool Remove(const CharacterSPtr_t & CHARACTER_SPTR);
        inline void Clear() { charactersSVec_.clear(); }

        CharacterSPtr_t FindByCreaturePtr(creature::CreatureCPtrC_t) const;

        const std::string Summary() const;

    private:
        CharacterSVec_t charactersSVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & charactersSVec_;
        }
    };

    using PartySPtr_t = std::shared_ptr<non_player::Party>;

}
}
#endif //APPBASE_HEROESPATH_NONPLAYER_PARTY_INCLUDED
