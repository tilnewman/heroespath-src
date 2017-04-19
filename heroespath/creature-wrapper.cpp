//
// creature-wrapper.cpp
//
#include "creature-wrapper.hpp"

#include "heroespath/creature/creature.hpp"
#include "heroespath/player/character.hpp"
#include "heroespath/non-player/character.hpp"
#include "heroespath/assertlogandthrow.hpp"


namespace heroespath
{

    CreatureWrapper::CreatureWrapper(const creature::CreatureSPtr_t & CREATURE_SPTR)
    :
        creatureSPtr_(CREATURE_SPTR)
    {
        Verify();
    }


    CreatureWrapper::CreatureWrapper(const player::CharacterSPtr_t & PLAYER_CHAR_SPTR)
    :
        creatureSPtr_(PLAYER_CHAR_SPTR)
    {
        Verify();
    }


    CreatureWrapper::CreatureWrapper(const non_player::CharacterSPtr_t & NON_PLAYER_CHAR_SPTR)
    :
        creatureSPtr_(NON_PLAYER_CHAR_SPTR)
    {
        Verify();
    }


    bool CreatureWrapper::IsPlayerCharacter() const
    {
        return creatureSPtr_->IsPlayerCharacter();
    }


    void CreatureWrapper::Verify() const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creatureSPtr_.get() != nullptr), "heroespath::CreatureWrapper::Verify()  A CreatureWrapper object was constructed with a shared_ptr that was null.");
    }

}
