///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
#define HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
//
// item-detail-viewer.hpp
//
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/pos-slider.hpp"
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
} // namespace item
namespace stage
{

    // Responsible for displaying details about an item that fades in and out.
    // Tolerates repeated calls to FadeIn() and FadeOut() in any order.
    class ItemDetailViewer : public sf::Drawable
    {
    public:
        ItemDetailViewer(const ItemDetailViewer &) = delete;
        ItemDetailViewer(const ItemDetailViewer &&) = delete;
        ItemDetailViewer & operator=(const ItemDetailViewer &) = delete;
        ItemDetailViewer & operator=(const ItemDetailViewer &&) = delete;

    public:
        ItemDetailViewer();

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
        void SetupImage(const item::ItemPtr_t);
        void SetupText(const item::ItemPtr_t);

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
        sf::Texture texture_;
        sf::Sprite sprite_;
        sfml_util::sliders::PosSlider slider_;
        sfml_util::gui::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect sourceRect;
        item::ItemPtr_t itemPtr_;
        bool willShowImage_;
        bool isBeforeAnyChange_;
        bool isShowing_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ITEMDETAILVIEWER_HPP_INCLUDED
