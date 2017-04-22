#ifndef HEROESPATH_COMBAT_COMBATTEXT_HPP_INCLUDED
#define HEROESPATH_COMBAT_COMBATTEXT_HPP_INCLUDED
//
// combat-text.hpp
//
#include "heroespath/combat/turn-action-info.hpp"
#include "heroespath/combat/fight-results.hpp"

#include <string>
#include <vector>
#include <memory>


namespace heroespath
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreatureCPtr_t = const Creature *;
    using CreaturePtrC_t = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace combat
{
    //forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = CombatDisplay *;
    using CombatDisplayCPtr_t = const CombatDisplay *;
    using CombatDisplayPtrC_t = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;


    //A collection of functions that generate text used during combat.
    struct Text
    {
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_;

        static const std::string MouseOverTextAttackStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextFightStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextCastStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextAdvanceStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextRetreatStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextBlockStr(creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextFlyStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextLandStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextRoarStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextPounceStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string ActionText(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                            const TurnActionInfo &    TURN_ACTION_INFO,
                                            const FightResult &       FIGHT_RESULT,
                                            const bool                WILL_USE_NAME       = false,
                                            const bool                IS_STATUS_VERSION   = false,
                                            const bool                IS_PREAMBLE_VERSION = false,
                                            const std::size_t         EFFECT_INDEX = 0,
                                            const std::size_t         HIT_INDEX = 0);

        static const std::string WeaponActionVerb(const item::ItemSPtr_t & WEAPON_SPTR,
                                                  const bool               WILL_APPEND_ING = false);
        
        static const std::string HitDescription(const HitInfo & HIT_INFO);
        
        static const std::string DodgeDescription(const HitInfo & HIT_INFO);
        
        static const std::string AttackDescriptionStatusVersion(const FightResult & FIGHT_RESULT);
    
        static const std::string AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT);

        static const std::string AttackDescriptionFullVersion(const FightResult & FIGHT_RESULT,
                                                              const std::size_t   EFFECT_INDEX,
                                                              const std::size_t   HIT_INDEX);
    
        static const std::string CastDescriptionStatusVersion(const TurnActionInfo & TURN_ACTION_INFO, 
                                                              const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionPreambleVersion(const TurnActionInfo & TURN_ACTION_INFO, 
                                                                const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionFullVersion(const TurnActionInfo & TURN_ACTION_INFO,
                                                            const FightResult &    FIGHT_RESULT);
    
        static const std::string WeaponActionVerbList(const FightResult & FIGHT_RESULT,
                                                      const bool          WILL_SKIP_MISSES);
    
        static const std::string CountPhrase(const std::size_t);

        static const std::string ConditionNameList(const creature::ConditionSVec_t & CONDITIONS_SVEC,
                                                   const bool                        WILL_EXCLUDE_DEAD,
                                                   const std::size_t                 NUM_TO_LIST);
        
        static const std::string AttackConditionsSummaryList(const FightResult & FIGHT_RESULT);
    
        //requires FIGHT_RESULT is not empty
        static const std::string FirstTargetNamePhrase(const FightResult & FIGHT_RESULT);
    
        static const std::string InitialCombatStatusMessagePrefix();
    };

}
}

#endif //HEROESPATH_COMBAT_COMBATTEXT_HPP_INCLUDED
