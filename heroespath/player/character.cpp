//
// character.hpp (player)
//
#include "character.hpp"

#include "sfml-util/date-time.hpp"

#include "heroespath/creature/body-type.hpp"
#include "heroespath/spell/spell-base.hpp"
#include "heroespath/stats/stat-set.hpp"
#include "heroespath/creature/race.hpp"
#include "heroespath/creature/role.hpp"
#include "heroespath/creature/title.hpp"
#include "heroespath/creature/condition.hpp"
#include "heroespath/item/inventory.hpp"


namespace heroespath
{
namespace player
{

    Character::Character(const std::string &                NAME,
                         const creature::sex::Enum          SEX,
                         const creature::BodyType &         BODY_TYPE,
                         const creature::Race &             RACE,
                         const creature::Role &             ROLE,
                         const stats::StatSet &             STATS,
                         const stats::Health_t              HEALTH,
                         const stats::Rank_t                RANK,
                         const stats::Exp_t                 EXPERIENCE,
                         const creature::ConditionSVec_t &  CONDITIONS_SVEC,
                         const creature::TitlePVec_t &      TITLES_PVEC,
                         const item::Inventory &            INVENTORY,
                         const sfml_util::DateTime &        DATE_TIME,
                         const std::string &                IMAGE_FILENAME,
                         const spell::SpellPVec_t &         SPELLS_PVEC)
    :
        Creature(NAME,
                 SEX,
                 BODY_TYPE,
                 RACE,
                 ROLE,
                 STATS,
                 HEALTH,
                 RANK,
                 EXPERIENCE,
                 CONDITIONS_SVEC,
                 TITLES_PVEC,
                 INVENTORY,
                 DATE_TIME,
                 IMAGE_FILENAME,
                 SPELLS_PVEC)
    {}


    Character::~Character()
    {}


    bool operator==(const Character & L, const Character & R)
    {
        return std::tie(L.name_,
                        L.imageFilename_,
                        L.sex_,
                        L.bodyType_,
                        L.race_,
                        L.role_,
                        L.stats_,
                        L.serialNumber_,
                        L.healthCurrent_,
                        L.healthNormal_,
                        L.rank_,
                        L.experience_,
                        L.dateTimeCreated_,
                        L.titlesPtrVec_,
                        L.conditionsSVec_,
                        L.spellsPVec_,
                        L.achievements_)
                ==
                std::tie(R.name_,
                         R.imageFilename_,
                         R.sex_,
                         R.bodyType_,
                         R.race_,
                         R.role_,
                         R.stats_,
                         R.serialNumber_,
                         R.healthCurrent_,
                         R.healthNormal_,
                         R.rank_,
                         R.experience_,
                         R.dateTimeCreated_,
                         R.titlesPtrVec_,
                         R.conditionsSVec_,
                         R.spellsPVec_,
                         R.achievements_);
    }


    bool operator<(const Character & L, const Character & R)
    {
        return std::tie(L.name_,
                        L.imageFilename_,
                        L.sex_,
                        L.bodyType_,
                        L.race_,
                        L.role_,
                        L.stats_,
                        L.serialNumber_,
                        L.healthCurrent_,
                        L.healthNormal_,
                        L.rank_,
                        L.experience_,
                        L.dateTimeCreated_,
                        L.titlesPtrVec_,
                        L.conditionsSVec_,
                        L.spellsPVec_,
                        L.achievements_)
                <
                std::tie(R.name_,
                         R.imageFilename_,
                         R.sex_,
                         R.bodyType_,
                         R.race_,
                         R.role_,
                         R.stats_,
                         R.serialNumber_,
                         R.healthCurrent_,
                         R.healthNormal_,
                         R.rank_,
                         R.experience_,
                         R.dateTimeCreated_,
                         R.titlesPtrVec_,
                         R.conditionsSVec_,
                         R.spellsPVec_,
                         R.achievements_);
    }

}
}
