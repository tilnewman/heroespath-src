#ifndef APPBASE_HEROESPATH_PLAYER_INITIAL_INCLUDED
#define APPBASE_HEROESPATH_PLAYER_INITIAL_INCLUDED
//
// initial.hpp
//  A collection of functions that performs initial setup of a player character
//
#include "heroespath/stats/types.hpp"


namespace heroespath
{
namespace player
{

    //forward declarations
    class Character;
    using CharacterPtrC_t  = Character * const;
    using CharacterCPtrC_t = const Character * const;


    class Initial
    {
    public:
        static void Setup(CharacterPtrC_t);

    private:
        static void EnsureValidImageFilename(CharacterPtrC_t);
        static void SetupInventory(CharacterPtrC_t);
        static void EquipBodyParts(CharacterPtrC_t);
        static stats::Health_t GetStartingHealth(CharacterCPtrC_t);
        static void SetStartingHealth(CharacterPtrC_t);
    };

}
}
#endif //APPBASE_HEROESPATH_PLAYER_INITIAL_INCLUDED
