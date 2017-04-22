#ifndef APPBASE_HEROESPATH_COMBAT_COMBATNODE_INCLUDED
#define APPBASE_HEROESPATH_COMBAT_COMBATNODE_INCLUDED
//
// combat-node.hpp
//  A class that handles drawing nodes on the combat tree.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sliders.hpp"

#include "heroespath/combat/node-type.hpp"
#include "heroespath/creature/creature.hpp"

#include <string>
#include <vector>


namespace heroespath
{
namespace combat
{

    //represents a node in the CombatTree
    class CombatNode : public sfml_util::gui::GuiEntity
    {
        //prevent copy construction
        CombatNode(const CombatNode &);

        //prevent copy assignment
        CombatNode & operator=(const CombatNode &);

    public:
        CombatNode(const creature::CreatureSPtr_t & CREATURE_SPTR,
                   const sfml_util::FontSPtr_t &    FONT_SPTR,
                   const unsigned int               FONT_CHAR_SIZE);

        virtual ~CombatNode();

        const std::string ToString() const;

        void ResetRegionAndTextSize(const sf::FloatRect & REGION, const unsigned int NAME_CHAR_SIZE);

        inline creature::CreatureSPtr_t Creature() const    { return creatureSPtr_; }

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline NodeType::Enum GetType() const               { return type_; }
        inline void SetType(const NodeType::Enum NEW_TYPE)  { type_ = NEW_TYPE; }

        inline int GetBlockingPos() const                   { return blockingPos_; }
        inline void SetBlockingPos(const int NEW_POS)       { blockingPos_ = NEW_POS; }

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        void SetToneDown(const float TONE_DOWN_VAL);

        inline bool IsSummaryView() const                   { return isSummaryView_; }
        void IsSummaryView(const bool);

        inline bool IsMoving() const                        { return isMoving_; }
        inline void IsMoving(const bool B)                  { isMoving_ = B; }

        void UpdateConditionText();

        inline bool GetIsFlying() const                     { return isFlying_; }
        void SetIsFlying(const bool);

        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        void SetImagePosOffset(const float HORIZ, const float VERT);

        inline const sf::FloatRect GetImageBounds() const   { return sprite_.getGlobalBounds(); }

        void HealthChangeTasks();

    protected:
        virtual void OnClick(const sf::Vector2f &) {}
        const sf::Color HealthColor() const;
        const sf::Color HealthColorRed() const;
        const sf::Color HealthColorTick() const;
        const sf::Color AdjustColorForToneDown(const sf::Color & ORIG_COLOR,
                                               const float       TONE_DOWN_VAL) const;

        void SetTextPositions(const bool DID_TRANSITION_TO_SUMMARY_VIEW = false);

        void SetWingImagePosition();
        void SetWingImageScaleAndOrigin();
        void UpdateImagePosition();

    protected:
        static const sf::Uint8 HIGHLIGHT_ADJ_VALUE_;
        static const sf::Color HIGHLIGHT_ADJ_COLOR_;
        static const sf::Color PLAYER_NAME_COLOR_;
        static const sf::Color CONDITION_COLOR_;
        static const sf::Color NONPLAYER_NAME_COLOR_;
        static const sf::Uint8 CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_;
        static const sf::Uint8 CREATURE_IMAGE_COLOR_ALPHA_;
        static const sf::Color CREATURE_IMAGE_COLOR_PLAYER_;
        static const sf::Color CREATURE_IMAGE_COLOR_NONPLAYER_;
        static const float     TONED_DOWN_MULT_;
        static const sf::Uint8 WING_IMAGE_ALPHA_;
        static const float     WING_IMAGE_SCALE_;
        static const float     WING_IMAGE_HORIZ_OFFSET_;
        static const float     WING_IMAGE_ANIM_SPEED_;
        static const float     WING_IMAGE_ROTATION_MAX_;
        //
        sf::Text                 nameTextObj_;
        sf::Text                 condTextObj_;
        int                      blockingPos_;
        NodeType::Enum           type_;
        sf::Color                healthLineColor_;
        sf::Color                healthLineColorRed_;
        sf::Color                healthLineColorTick_;
        sfml_util::TextureSPtr_t textureSPtr_;
        sf::Sprite               sprite_;
        sf::Color                creatureImageColor_;
        bool                     isSummaryView_;
        bool                     isMoving_;
        creature::CreatureSPtr_t creatureSPtr_;

        //members that control the flapping wing animation
        static sfml_util::TextureSPtr_t       wingTextureSPtr_;
        sf::Sprite                            wingSprite_;
        bool                                  isFlying_;
        sfml_util::sliders::ZeroSlider<float> wingFlapSlider_;

        //members that control the shake or image position offset
        sf::Vector2f imagePosV_;
        sf::Vector2f imagePosOffsetV_;
    };

    using CombatNodePtr_t   = CombatNode *;
    using CombatNodeCPtr_t  = const CombatNode *;
    using CombatNodePtrC_t  = CombatNode * const;
    using CombatNodeCPtrC_t = const CombatNode * const;

    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodeSVec_t = std::vector<CombatNodeSPtr_t>;

}
}
#endif //APPBASE_HEROESPATH_COMBAT_COMBATNODE_INCLUDED
