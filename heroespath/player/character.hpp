#ifndef APPBASE_HEROESPATH_PLAYER_CHARACTER_INCLUDED
#define APPBASE_HEROESPATH_PLAYER_CHARACTER_INCLUDED
//
// character.hpp (player)
//  A character under the control of the user.
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
#include <set>


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
        using TitlePtr_t  = Title *;
        using TitlePVec_t = std::vector<TitlePtr_t>;

        class Condition;
        using ConditionSPtr_t = std::shared_ptr<Condition>;
        using ConditionSVec_t = std::vector<ConditionSPtr_t>;
    }

    namespace stats
    {
        class StateSet;
    }

namespace player
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
                  const creature::Race &             RACE            = creature::Race(creature::race::Count_PlayerRaces),
                  const creature::Role &             ROLE            = creature::Role(creature::role::PlayerRoleCount),
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

        inline virtual bool IsPlayerCharacter() const { return true; }

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

    using CharacterPtr_t  = Character *;
    using CharacterCPtr_t = const Character *;
    using CharacterPtrC_t = Character * const;
    using CharacerCPtrC_t = const Character * const;

    using CharacterSPtr_t = std::shared_ptr<Character>;
    using CharacterSVec_t = std::vector<CharacterSPtr_t>;
    using CharacterSSet_t = std::set<CharacterSPtr_t>;


    bool operator==(const Character & L, const Character & R);

    inline bool operator!=(const Character & L, const Character & R) { return ! (L == R); }

    bool operator<(const Character & L, const Character & R);
}
}
#endif //APPBASE_HEROESPATH_PLAYER_CHARACTER_INCLUDED
