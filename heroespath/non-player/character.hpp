#ifndef APPBASE_HEROESPATH_NONPLAYER_CHARACTER_INCLUDED
#define APPBASE_HEROESPATH_NONPLAYER_CHARACTER_INCLUDED
//
// character.hpp (non-player)
//  A character NOT under the control of the user.
//
#include "utilz/boost-serialize-includes.hpp"

#include "heroespath/spell/spell-base.hpp"
#include "heroespath/creature/sex-enum.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/item/types.hpp"
#include "heroespath/stats/types.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    class DateTime;
}

namespace heroespath
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

        class Condition;
        using ConditionSPtr_t = std::shared_ptr<Condition>;
        using ConditionSVec_t = std::vector<ConditionSPtr_t>;
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
        Character(const Character &);

        //prevent copy assignment
        Character & operator=(const Character &);

    public:
        Character(const std::string &                NAME            = "no_name_error",
                  const creature::sex::Enum          SEX             = creature::sex::Unknown,
                  const creature::BodyType &         BODY_TYPE       = creature::BodyType(),
                  const creature::Race &             RACE            = creature::race::Count,
                  const creature::Role &             ROLE            = creature::Role(creature::role::Count),
                  const stats::StatSet &             STATS           = stats::StatSet(),
                  const stats::Health_t              HEALTH          = 0,
                  const stats::Rank_t                RANK            = 1,
                  const stats::Exp_t                 EXPERIENCE      = 0,
                  const creature::ConditionSVec_t &  CONDITIONS_SVEC = creature::ConditionSVec_t(),
                  const creature::TitlePVec_t &      TITLES_PVEC     = creature::TitlePVec_t(),
                  const item::Inventory &            INVENTORY       = item::Inventory(),
                  const sfml_util::DateTime &        DATE_TIME       = sfml_util::DateTime(),
                  const std::string &                IMAGE_FILENAME  = "",
                  const spell::SpellPVec_t &         SPELLS_PVEC     = spell::SpellPVec_t());

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

    inline bool operator!=(const Character & L, const Character & R) { return ! (L == R); }

    bool operator<(const Character & L, const Character & R);
}
}
#endif //APPBASE_HEROESPATH_NONPLAYER_CHARACTER_INCLUDED
