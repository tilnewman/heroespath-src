#ifndef GAME_NONPLAYER_CHARACTER_INCLUDED
#define GAME_NONPLAYER_CHARACTER_INCLUDED
//
// character.hpp (non-player)
//  A character NOT under the control of the user.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/spell/spell-enum.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/creature.hpp"
#include "game/item/types.hpp"
#include "game/stats/types.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    class DateTime;
}

namespace game
{
namespace item
{
    class Inventory;
}

namespace creature
{
    class Race;
    class Role;
    class BodyType;

    class Title;
    using TitlePtr_t = Title *;
    using TitlePVec_t = std::vector<TitlePtr_t>;
}

namespace stats
{
    class StateSet;
}

namespace non_player
{

    class Character : public creature::Creature
    {
        //prevent copy constrution
        Character(const Character &) =delete;

        //prevent copy assignment
        Character & operator=(const Character &) =delete;

    public:
        Character(const std::string &                  NAME           = "no_name_error",
                  const creature::sex::Enum            SEX            = creature::sex::Unknown,
                  const creature::BodyType &           BODY_TYPE      = creature::BodyType(),
                  const creature::Race &               RACE           = creature::race::Count,
                  const creature::Role &               ROLE           = creature::Role(creature::role::Count),
                  const stats::StatSet &               STATS          = stats::StatSet(),
                  const stats::Health_t                HEALTH         = 0,
                  const stats::Rank_t                  RANK           = 1,
                  const stats::Exp_t                   EXPERIENCE     = 0,
                  const creature::ConditionEnumVec_t & CONDITIONS_VEC = creature::ConditionEnumVec_t(),
                  const creature::TitlePVec_t &        TITLES_PVEC    = creature::TitlePVec_t(),
                  const item::Inventory &              INVENTORY      = item::Inventory(),
                  const sfml_util::DateTime &          DATE_TIME      = sfml_util::DateTime(),
                  const std::string &                  IMAGE_FILENAME = "",
                  const spell::SpellVec_t &            SPELLS_VEC     = spell::SpellVec_t(),
                  const stats::Mana_t                  MANA           = 0);

        virtual ~Character();

        friend bool operator==(const Character & L, const Character & R);
        friend bool operator<(const Character & L, const Character & R);

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Creature>(*this);
        }
    };

    using CharacterSPtr_t = std::shared_ptr<non_player::Character>;
    using CharacterSVec_t = std::vector<CharacterSPtr_t>;
    using CharacterSVecIter_t = CharacterSVec_t::iterator;
    using CharacterSVecCIter_t = CharacterSVec_t::const_iterator;

    bool operator==(const Character & L, const Character & R);

    inline bool operator!=(const Character & L, const Character & R)
    {
        return ! (L == R);
    }

    bool operator<(const Character & L, const Character & R);

}
}

#endif //GAME_NONPLAYER_CHARACTER_INCLUDED
