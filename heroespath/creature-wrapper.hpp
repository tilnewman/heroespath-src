#ifndef HEROESPATH_CREATURE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_CREATURE_WRAPPER_HPP_INCLUDED
//
// creature-wrapper
//
#include <memory>
#include <vector>


namespace heroespath
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
        CreatureWrapper(const creature::CreatureSPtr_t & CREATURE_SPTR);
        CreatureWrapper(const player::CharacterSPtr_t & PLAYER_CHAR_SPTR);
        CreatureWrapper(const non_player::CharacterSPtr_t & NON_PLAYER_CHAR_SPTR);

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

#endif //HEROESPATH_CREATURE_WRAPPER_HPP_INCLUDED
