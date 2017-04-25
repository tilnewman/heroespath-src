#ifndef HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
#define HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
//
// combat-anim.hpp
//
#include "sfml-util/sfml-graphics.hpp"

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
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
}
namespace combat
{

    class CombatDisplay;
    using CombatDisplayPtr_t = CombatDisplay *;

    class CombatNode;
    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodeSVec_t = std::vector<CombatNodeSPtr_t>;


    //Responsible for displaying combat related animations.
    class CombatAnim
    {
        //prevent copy construction
        CombatAnim(const CombatAnim &) =delete;

        //prevent copy assignment
        CombatAnim operator=(const CombatAnim &) =delete;

        //prevent non-singleton construction
        CombatAnim();

    public:
        virtual ~CombatAnim();

        static void GiveCombatDisplay(CombatDisplayPtr_t);
        static CombatAnim * const Instance();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void ProjectileShootAnimStart(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                      creature::CreatureCPtrC_t CREATURE_DEFENDING_CPTRC,
                                      const item::ItemSPtr_t &  WEAPON_SPTR,
                                      const bool                WILL_HIT);

        void ProjectileShootAnimUpdate(const float SLIDER_POS);
        void ProjectileShootAnimStop();

        void DeathAnimStart(const creature::CreaturePVec_t &);
        void DeathAnimUpdate(const float SLIDER_POS);
        void DeathAnimStop();

    private:
        static CombatAnim * instance_;
        static CombatDisplayPtr_t combatDisplayPtr_;

        //members supporting the projectile shoot animation
        sfml_util::TextureSPtr_t projAnimTextureSPtr_;
        sf::Sprite projAnimSprite_;
        sf::Vector2f projAnimBeginPosV_;
        sf::Vector2f projAnimEndPosV_;
        bool projAnimWillSpin_;

        //members supporting the death animation
        CombatNodeSVec_t deadAnimNodesSVec_;
    };

    using CombatAnimPtr_t = CombatAnim *;

}
}

#endif //HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
