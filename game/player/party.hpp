#ifndef GAME_PLAYER_PARTY_INCLUDED
#define GAME_PLAYER_PARTY_INCLUDED
//
// party.hpp
//  A collection of characters under control of the user.
//
#include "utilz/boost-serialize-includes.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
    namespace creature
    {
        class Creature;
        using CreatureSPtr_t  = std::shared_ptr<Creature>;
        using CreaturePtr_t   = Creature *;
        using CreatureCPtrC_t = const Creature * const;
        using CreatureSVec_t  = std::vector<CreatureSPtr_t>;
    }

namespace player
{

    //forward declarations
    class Character;
    using CharacterSPtr_t = std::shared_ptr<Character>;
    using CharacterSVec_t = std::vector<CharacterSPtr_t>;


    //encapsulates a set of Characters under control of the user
    class Party
    {
        //prevent copy construction
        Party(const Party &) =delete;

        //prevent copy assignment
        Party & operator=(const Party &) =delete;

    public:
        Party(const CharacterSVec_t & CHARACTER_SVEC = CharacterSVec_t());
        virtual ~Party();

        inline const CharacterSVec_t Characters() const { return charactersSVec_; }

        //Sets error_msg to a message describing why upon failure, otherwise error_msg is not changed.
        bool Add(const CharacterSPtr_t & CHARACTER_SPTR, std::string error_msg = std::string());

        bool IsAddAllowed(const CharacterSPtr_t & CHARACTER_SPTR, std::string error_msg);

        //returns true if the character existed in the charactersSVec_ and was removed.
        bool Remove(const CharacterSPtr_t & CHARACTER_SPTR);

        inline creature::CreatureSPtr_t GetNextInOrderAfter(const creature::CreatureSPtr_t & CREATURE_SPTR) { return GetNextInOrder(CREATURE_SPTR, true); }
        inline creature::CreatureSPtr_t GetNextInOrderAfter(creature::CreatureCPtrC_t CREATURE_CPTRC)       { return GetNextInOrder(CREATURE_CPTRC, true); }

        inline creature::CreatureSPtr_t GetNextInOrderBefore(const creature::CreatureSPtr_t & CREATURE_SPTR){ return GetNextInOrder(CREATURE_SPTR, false); }
        inline creature::CreatureSPtr_t GetNextInOrderBefore(creature::CreatureCPtrC_t CREATURE_CPTRC)      { return GetNextInOrder(CREATURE_CPTRC, false); }

        creature::CreatureSPtr_t GetNextInOrder(const creature::CreatureSPtr_t &, const bool);
        creature::CreatureSPtr_t GetNextInOrder(creature::CreatureCPtrC_t, const bool);

        creature::CreatureSPtr_t GetAtOrderPos(const std::size_t);//zero indexed

        std::size_t GetOrderNum(const creature::CreatureSPtr_t &) const;//zero indexed
        std::size_t GetOrderNum(creature::CreatureCPtrC_t) const;//zero indexed

        std::size_t GetNumHumanoid() const;

    public:
        static const std::size_t MAX_CHARACTER_COUNT_;
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

    using PartySPtr_t = std::shared_ptr<Party>;

}
}
#endif //GAME_PLAYER_PARTY_INCLUDED
