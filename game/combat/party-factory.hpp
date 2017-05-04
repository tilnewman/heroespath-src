#ifndef GAME_COMBAT_PARTYFACTORY_INCLUDED
#define GAME_COMBAT_PARTYFACTORY_INCLUDED
//
// party-factory.hpp
//  A singleton class that creates parties of creatures that confront the player characters.
//
#include "game/item/types.hpp"

#include <memory>
#include <vector>
#include <string>


namespace game
{
namespace non_player
{
    class Party;
    using PartySPtr_t = std::shared_ptr<Party>;

    class Character;
    using CharacterSPtr_t = std::shared_ptr<Character>;
}
namespace combat
{

    //types required by the singleton implementation
    class PartyFactory;
    using PartyFactorySPtr_t = std::shared_ptr<PartyFactory>;


    //Creates partys of creatures that confront the player characters
    class PartyFactory
    {
        //prevent copy construction
        PartyFactory(const PartyFactory &);

        //prevent copy assignment
        PartyFactory & operator=(const PartyFactory &);

        //prevent non-singletone instantiation
        PartyFactory();

    public:
        virtual ~PartyFactory();
        static PartyFactorySPtr_t Instance();

        non_player::PartySPtr_t MakeParty_FirstEncounter() const;

    private:
        non_player::CharacterSPtr_t MakeCreature_GoblinGrunt() const;

    private:
        static PartyFactorySPtr_t instance_;
    };

}
}
#endif //GAME_COMBAT_PARTYFACTORY_INCLUDED
