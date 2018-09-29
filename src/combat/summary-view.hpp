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
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/entity-slider.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;

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
    struct ItemWithText : public sf::Drawable
    {
        explicit ItemWithText(const item::ItemPtr_t ITEM_PTR);
        virtual ~ItemWithText() = default;

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        static const float ITEM_IMAGE_SCALE_MIN_;

        item::ItemPtr_t item_ptr;
        sfml_util::CachedTexture cached_texture;
        sf::Sprite sprite;
        sfml_util::TextRegionUPtr_t name_text_region_uptr;
        sfml_util::TextRegionUPtr_t desc_text_region_uptr;
        sfml_util::TextRegionUPtr_t info_text_region_uptr;
    };

    using ItemWithTextUPtr_t = std::unique_ptr<ItemWithText>;
    using ItemWithTextUVec_t = std::vector<ItemWithTextUPtr_t>;

    // the infomation that displays when the mouse cursor is held over an enemy
    class SummaryView : public sf::Drawable
    {
    public:
        SummaryView(const SummaryView &) = delete;
        SummaryView(SummaryView &&) = delete;
        SummaryView & operator=(const SummaryView &) = delete;
        SummaryView & operator=(SummaryView &&) = delete;

    public:
        SummaryView();
        virtual ~SummaryView() = default;

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

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

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
        //
        bool isTransToComplete_;
        bool isTransBackComplete_;
        sfml_util::Moving::Enum movingDir_;
        ItemWithTextUVec_t itemWithTextUVec_;
        sf::VertexArray bgQuads_;
        CombatNodePtrOpt_t combatNodePtrOpt_;
        sfml_util::TextRegionUPtr_t nameTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t rankTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t healthTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t descTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t condTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t armorTextRegionUPtr_;
        sfml_util::EntitySlider geSlider_;
        bool preventNextTrans_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
