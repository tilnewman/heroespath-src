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
#ifndef GAME_CREATURE_WRAPPER_HPP_INCLUDED
#define GAME_CREATURE_WRAPPER_HPP_INCLUDED
//
// creature-wrapper
//
#include <memory>
#include <vector>


namespace game
{

    // forward declarations
    namespace creature
    {
        class Creature;
        using CreatureSPtr_t = std::shared_ptr<Creature>;
    }

    namespace player
    {
        class Character;
        using CharacterSPtr_t = std::shared_ptr<Character>;
    }

    namespace non_player
    {
        class Character;
        using CharacterSPtr_t = std::shared_ptr<Character>;
    }


    //A class that holds either a player or non_player character as a creature sptr.
    class CreatureWrapper
    {
    public:
        explicit CreatureWrapper(const creature::CreatureSPtr_t & CREATURE_SPTR);
        explicit CreatureWrapper(const player::CharacterSPtr_t & PLAYER_CHAR_SPTR);
        explicit CreatureWrapper(const non_player::CharacterSPtr_t & NON_PLAYER_CHAR_SPTR);

        bool IsPlayerCharacter() const;
        inline creature::CreatureSPtr_t Creature() const    { return creatureSPtr_; }
        void Verify() const;

        friend bool operator<(const CreatureWrapper & L, const CreatureWrapper & R);
        friend bool operator==(const CreatureWrapper & L, const CreatureWrapper & R);

    private:
        creature::CreatureSPtr_t creatureSPtr_;
    };

    using CreatureWrapperVec_t = std::vector<CreatureWrapper>;

    inline bool operator< (const CreatureWrapper & L, const CreatureWrapper & R) { return (L.creatureSPtr_ < R.creatureSPtr_); }
    inline bool operator==(const CreatureWrapper & L, const CreatureWrapper & R) { return (L.creatureSPtr_ == R.creatureSPtr_); }
    inline bool operator!=(const CreatureWrapper & L, const CreatureWrapper & R) { return ! (L == R); }

}

#endif //GAME_CREATURE_WRAPPER_HPP_INCLUDED
