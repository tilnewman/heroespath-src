#ifndef APPBASE_HEROESPATH_SUMMARY_VIEW_INCLUDED
#define APPBASE_HEROESPATH_SUMMARY_VIEW_INCLUDED
//
// summary-view.hpp
//  A collection of classes that are responsible for displaying creature details on the Combat Display
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"

#include <memory>
#include <vector>


//forward declarations
namespace sfml_util
{
namespace gui
{
    class TextRegion;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
}
}

namespace heroespath
{
namespace item
{
    //forward declarations
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace combat
{

    //forward declarations
    class CombatNode;
    using CombatNodePtr_t = CombatNode *;


    //wraps an image with text for display on the screen
    struct ItemWithText
    {
        ItemWithText(const item::ItemSPtr_t & ITEM_SPTR = item::ItemSPtr_t());

        sf::Sprite sprite;
        sfml_util::TextureSPtr_t texture_sptr;
        sfml_util::gui::TextRegionSPtr_t name_text_region_sptr;
        sfml_util::gui::TextRegionSPtr_t desc_text_region_sptr;
        sfml_util::gui::TextRegionSPtr_t info_text_region_sptr;
        item::ItemSPtr_t item_sptr;
    };

    using ItemWithTextVec_t = std::vector<ItemWithText>;
    using ItemWithTextVecIter_t = ItemWithTextVec_t::iterator;
    using ItemWithTextVecCIter_t = ItemWithTextVec_t::const_iterator;


    //the infomation that displays when the mouse cursor is held over an enemy
    class SummaryView
    {
        //prevent copy construction
        SummaryView(const SummaryView &);

        //prevent copy assignment
        SummaryView & operator=(const SummaryView &);

    public:
        SummaryView();

        void StartTransitionBack();
        void StartTransitionTo(CombatNodePtr_t       combatNodePtr,
                               const sf::Vector2f &  DEST_POS_V,
                               const sf::FloatRect & ENEMYDISPLAY_RECT);

        void BackgroundColor(const sf::Color &);
        const sf::Color BackgroundColor() const;

        void BackgroundRegion(const sf::FloatRect &);

        void SetTransitionToComplete();
        void SetTransitionBackComplete();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void SetupAndStartTransition(CombatNodePtr_t combatNodePtr, const sf::FloatRect & COMABT_REGION);

        inline float GetTransitionStatus() const                { return slider_.CurrentAverage(); }

        inline bool IsTransitionToComplete() const              { return isTransToComplete_; }
        inline bool IsTransitionBackComplete() const            { return isTransBackComplete_; }

        inline sfml_util::Moving::Enum MovingDir() const        { return movingDir_; }
        inline void MovingDir(const sfml_util::Moving::Enum E)  { movingDir_ = E; }

        inline CombatNodePtr_t CombatNodePtr()                  { return combatNodePtr_; }

        inline sfml_util::gui::TextRegionSPtr_t DescTextRegion(){ return descTextRegionSPtr_; }

        inline bool WillPreventNextTransition() const           { return preventNextTrans_; }
        inline void WillPreventNextTransition(const bool B)     { preventNextTrans_ = B; }

        inline void ReleaseCombatNodePointer()                  { combatNodePtr_ = nullptr; slider_.ReleasePointer(); }
    public:
        static const float BACKGROUND_COLOR_ALPHA_;
        static const float SLIDER_SPEED_;

    private:
        const float                      BLOCK_POS_LEFT_;
        const float                      BLOCK_POS_TOP_;
        const float                      IMAGE_EDGE_PAD_;
        const float                      IMAGE_BETWEEN_PAD_;
        const sf::Uint8                  IMAGE_COLOR_ALPHA_;
        //
        bool                             isTransToComplete_;
        bool                             isTransBackComplete_;
        sfml_util::Moving::Enum          movingDir_;
        ItemWithTextVec_t                itemWithTextVec_;
        sf::VertexArray                  bgQuads_;
        CombatNodePtr_t                  combatNodePtr_;
        sfml_util::gui::TextRegionSPtr_t nameTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t healthTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t descTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t condTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t armorTextRegionSPtr_;
        sfml_util::gui::GuiEntitySlider  slider_;
        bool                             preventNextTrans_;
    };

}
}

#endif //APPBASE_HEROESPATH_SUMMARY_VIEW_INCLUDED
