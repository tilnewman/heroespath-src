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
#ifndef HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
#define HEROESPATH_COMBAT_SUMMARY_VIEW_HPP_INCLUDED
//
// summary-view.hpp
//  A collection of classes that are responsible for displaying creature details on the Combat
//  Display
//
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
    using ItemPtr_t = Item *;
} // namespace item

namespace combat
{

    class CombatNode;
    using CombatNodePtr_t = CombatNode *;

    // wraps an image with text for display on the screen
    struct ItemWithText
    {
        explicit ItemWithText(const item::ItemPtr_t ITEM_PTR = nullptr);

        ItemWithText(const ItemWithText &);

        ItemWithText & operator=(const ItemWithText &);

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
        SummaryView(const SummaryView &) = delete;
        SummaryView & operator=(const SummaryView &) = delete;

    public:
        SummaryView();

        void StartTransitionBack();
        void StartTransitionTo(
            CombatNodePtr_t combatNodePtr,
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
            CombatNodePtr_t combatNodePtr, const sf::FloatRect & COMABT_REGION);

        inline float GetTransitionStatus() const { return geSlider_.ProgressRatio(); }

        inline bool IsTransitionToComplete() const { return isTransToComplete_; }
        inline bool IsTransitionBackComplete() const { return isTransBackComplete_; }

        inline sfml_util::Moving::Enum MovingDir() const { return movingDir_; }
        inline void MovingDir(const sfml_util::Moving::Enum E) { movingDir_ = E; }

        inline CombatNodePtr_t CombatNodePtr() { return combatNodePtr_; }

        inline bool WillPreventNextTransition() const { return preventNextTrans_; }
        inline void WillPreventNextTransition(const bool B) { preventNextTrans_ = B; }

        inline void ReleaseCombatNodePointer()
        {
            combatNodePtr_ = nullptr;
            geSlider_.SetEntity(nullptr);
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
        CombatNodePtr_t combatNodePtr_;
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
