// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
#define HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
//
// summary-view.hpp
//  Classes responsible for displaying creature details on the Combat Stage
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace combat
{

    class CombatNode;
    using CombatNodePtr_t = misc::NotNull<CombatNode *>;
    using CombatNodePtrOpt_t = boost::optional<CombatNodePtr_t>;

    // wraps an image with text for display on the screen
    struct ItemWithText
    {
        explicit ItemWithText(const item::ItemPtr_t ITEM_PTR);

        sf::Sprite sprite;
        sf::Texture texture;
        sfml_util::gui::TextRegionSPtr_t name_text_region_sptr;
        sfml_util::gui::TextRegionSPtr_t desc_text_region_sptr;
        sfml_util::gui::TextRegionSPtr_t info_text_region_sptr;
        item::ItemPtr_t item_ptr;
    };

    using ItemWithTextVec_t = std::vector<ItemWithText>;

    // the infomation that displays when the mouse cursor is held over an enemy
    class SummaryView
    {
    public:
        SummaryView(const SummaryView &) = delete;
        SummaryView(SummaryView &&) = delete;
        SummaryView & operator=(const SummaryView &) = delete;
        SummaryView & operator=(SummaryView &&) = delete;

    public:
        SummaryView();

        void StartTransitionBack();
        void StartTransitionTo(
            const CombatNodePtr_t COMBAT_NODE_PTR,
            const sf::Vector2f & DEST_POS_V,
            const sf::FloatRect & ENEMYDISPLAY_RECT);

        void BackgroundColor(const sf::Color &);
        const sf::Color BackgroundColor() const;

        void BackgroundRegion(const sf::FloatRect &);

        void SetTransitionToComplete();
        void SetTransitionBackComplete();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void SetupAndStartTransition(
            const CombatNodePtr_t COMBAT_NODE_PTR, const sf::FloatRect & COMABT_REGION);

        float GetTransitionStatus() const { return geSlider_.ProgressRatio(); }

        bool IsTransitionToComplete() const { return isTransToComplete_; }
        bool IsTransitionBackComplete() const { return isTransBackComplete_; }

        sfml_util::Moving::Enum MovingDir() const { return movingDir_; }
        void MovingDir(const sfml_util::Moving::Enum E) { movingDir_ = E; }

        const CombatNodePtrOpt_t CombatNodePtrOpt() const { return combatNodePtrOpt_; }

        bool WillPreventNextTransition() const { return preventNextTrans_; }
        void WillPreventNextTransition(const bool B) { preventNextTrans_ = B; }

    private:
        void ReleaseCombatNodePointer()
        {
            combatNodePtrOpt_ = boost::none;
            geSlider_.SetEntity(boost::none);
        }

    public:
        static const float BACKGROUND_COLOR_ALPHA_;
        static const float SLIDER_SPEED_;

    private:
        const float BLOCK_POS_LEFT_;
        const float BLOCK_POS_TOP_;
        const float IMAGE_EDGE_PAD_;
        const float IMAGE_BETWEEN_PAD_;
        const sf::Uint8 IMAGE_COLOR_ALPHA_;
        //
        bool isTransToComplete_;
        bool isTransBackComplete_;
        sfml_util::Moving::Enum movingDir_;
        ItemWithTextVec_t itemWithTextVec_;
        sf::VertexArray bgQuads_;
        CombatNodePtrOpt_t combatNodePtrOpt_;
        sfml_util::gui::TextRegionUPtr_t nameTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t rankTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t healthTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t condTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t armorTextRegionUPtr_;
        sfml_util::gui::GuiEntitySlider geSlider_;
        bool preventNextTrans_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
