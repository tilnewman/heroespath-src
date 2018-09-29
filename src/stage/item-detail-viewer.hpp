// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
#define HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
//
// item-detail-viewer.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/pos-slider.hpp"
#include "sfml-util/sfml-util-vertex.hpp"
#include "sfml-util/text-region.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
} // namespace item
namespace stage
{

    // Responsible for displaying details about an item that fades in and out.
    // Tolerates repeated calls to FadeIn() and FadeOut() in any order.
    class ItemDetailViewer : public sf::Drawable
    {
    public:
        ItemDetailViewer(const ItemDetailViewer &) = delete;
        ItemDetailViewer(ItemDetailViewer &&) = delete;
        ItemDetailViewer & operator=(const ItemDetailViewer &) = delete;
        ItemDetailViewer & operator=(ItemDetailViewer &&) = delete;

        ItemDetailViewer();
        virtual ~ItemDetailViewer() = default;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void FadeIn(const item::ItemPtr_t, const sf::FloatRect &);
        void FadeOut();

        bool IsShowing() const { return isShowing_; }
        bool IsMoving() const { return slider_.IsMoving(); }
        bool IsMovingOrShowing() const { return (IsShowing() || IsMoving()); }

    private:
        void SetupBackgroundQuadColors(const float PROGRESS_RATIO);
        void SetupBackgroundQuadPositions(const float PROGRESS_RATIO);
        void SetupImage(const item::ItemPtrOpt_t);
        void SetupText(const item::ItemPtrOpt_t);

    private:
        const float SCREEN_WIDTH_;
        const float HALF_SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float IMAGE_TOP_SPACER_;
        const float INNER_SPACER_;
        const float DOUBLE_INNER_SPACER_;
        const float TARGET_WIDTH_;
        const sf::FloatRect TARGET_RECT_;
        const float SLIDER_SPEED_;

        sf::VertexArray backgroundQuads_;
        sfml_util::CachedTextureOpt_t cachedTextureOpt_;
        sf::Sprite sprite_;
        sfml_util::sliders::PosSlider slider_;
        sfml_util::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect sourceRect;
        item::ItemPtrOpt_t itemPtrOpt_;
        bool willShowImage_;
        bool isBeforeAnyChange_;
        bool isShowing_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
