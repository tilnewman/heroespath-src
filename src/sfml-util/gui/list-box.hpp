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
#ifndef HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
//
// list-box.hpp
//  A class that manages a list of some template GuiEntity types wrapped in a box
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui-event-enum.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "misc/vector-map.hpp"

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>


namespace heroespath
{
namespace sfml_util
{
    class IStage;

namespace gui
{

    namespace box
    {
        class Box;
        using BoxUPtr_t = std::unique_ptr<Box>;
    }

    class ListBoxItem;
    using ListBoxItemSPtr_t = std::shared_ptr<ListBoxItem>;
    using ListBoxItemSVec_t = std::vector<ListBoxItemSPtr_t>;

    //types required for the ListBoxPtrPackage_t and IListBoxCallbackHandler below
    class ListBox;
    using ListBoxUPtr_t = std::unique_ptr<ListBox>;

    using ImagePair_t = std::pair<std::shared_ptr<sf::Texture>, sf::Sprite>;
    using ImageMap_t = misc::VectorMap<ListBoxItemSPtr_t, ImagePair_t>;
    

    //the callback type for ListBox is a wrapper class called ListBoxItem
    namespace callback
    {
        using ListBoxPtrPackage_t = sfml_util::callback::PtrWrapper<ListBox>;

        struct ListBoxEventPackage
        {
            static const int INVALID_SELECTION_;
            static const sf::Vector2f INVALID_MOUSE_POS_V_;

            explicit ListBoxEventPackage(
                ListBoxPtrPackage_t                PACKAGE,
                const sfml_util::GuiEvent::Enum    GUI_EVENT         = sfml_util::GuiEvent::None,
                const sf::Vector2f &               MOUSE_POS_V       = INVALID_MOUSE_POS_V_,
                const int                          CURR_SELECTION    = INVALID_SELECTION_,
                const bool                         HAS_FOCUS_CHANGED = false,
                const sf::Event::KeyEvent        & KEYPRESS_EVENT    = sf::Event::KeyEvent(),
                const sf::Event::MouseWheelEvent & MOUSEWHEEL_EVENT  = sf::Event::MouseWheelEvent())
            :
                package          (PACKAGE),
                gui_event        (GUI_EVENT),
                mouse_pos        (MOUSE_POS_V),
                curr_selection   (CURR_SELECTION),
                has_focus_changed(HAS_FOCUS_CHANGED),
                keypress_event   (KEYPRESS_EVENT),
                mousewheel_event (MOUSEWHEEL_EVENT)
            {}

            ListBoxPtrPackage_t        package;
            sfml_util::GuiEvent::Enum  gui_event;
            sf::Vector2f               mouse_pos;
            int                        curr_selection;
            bool                       has_focus_changed;
            sf::Event::KeyEvent        keypress_event;
            sf::Event::MouseWheelEvent mousewheel_event;
        };

        using IListBoxCallbackHandler =
            sfml_util::callback::ICallbackHandler<ListBoxEventPackage, bool>;
    }


    //A class that manages a vertical list of IGuiEntitys that can be scrolled through and selected.
    //The template type Item_t must be/descend/implement IGuiEntity.
    class ListBox
    :
        public GuiEntity,
        public callback::ISliderBarCallbackHandler_t
    {
        ListBox(const ListBox &) =delete;
        ListBox & operator=(const ListBox &) =delete;

    public:
        explicit ListBox(
            const std::string &       NAME,
            const sf::FloatRect &     REGION      = sf::FloatRect(),
            const ListBoxItemSVec_t & ITEM_VEC    = ListBoxItemSVec_t(),
            IStage * const            stagePtr    = nullptr,
            const float               MARGIN      = 0.0f,
            const float               BETWEEN_PAD = 0.0f,
            const box::Info &         BOX_INFO    = box::Info(),
            const sf::Color &         LINE_COLOR  = sf::Color::Transparent,
            callback::IListBoxCallbackHandler * const callbackPtr = nullptr);

        virtual ~ListBox();

        void Setup(
            const sf::FloatRect &     REGION      = sf::FloatRect(),
            const ListBoxItemSVec_t & ITEM_VEC    = ListBoxItemSVec_t(),
            IStage * const            stagePtr    = nullptr,
            const float               MARGIN      = 0.0f,
            const float               BETWEEN_PAD = 0.0f,
            const box::Info &         BOX_INFO    = box::Info(),
            const sf::Color &         LINE_COLOR  = sf::Color::Transparent,
            callback::IListBoxCallbackHandler * const callbackPtr = nullptr);

        inline virtual const std::string HandlerName() const override { return GetEntityName(); }

        virtual bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE) override;

        inline void ImageColor(const sf::Color & COLOR) { imageColor_ = COLOR; SetupForDraw(); }
        inline const sf::Color ImageColor() const { return imageColor_; }

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        ListBoxItemSPtr_t AtPos(const sf::Vector2f & POS_V);

        const ListBoxItemSPtr_t Selected() const;
        
        inline std::size_t DisplayIndex() const
        {
            return displayIndex_;
        }

        void DisplayIndex(const std::size_t);

        inline std::size_t SelectedIndex() const
        {
            return selectionDisplayIndex_ + selectionOffsetIndex_;
        }

        //sets displayIndex_ to whatever selectionDisplayIndex_ turns out to be
        void SelectedIndex(const std::size_t);
        
        inline bool IsIndexValid(const std::size_t INDEX) const { return (INDEX < items_.size()); }

        inline bool IsIndexVisible_Display(const std::size_t INDEX) const
        {
            return ((INDEX >= displayIndex_) &&
                    (INDEX <= CalcLastVisibleIndex_Display()));
        }

        inline bool IsIndexVisible_Selection(const std::size_t INDEX) const
        {
            return ((INDEX >= selectionDisplayIndex_) &&
                    (INDEX <= CalcLastVisibleIndex_Selection()));
        }

        inline ListBoxItemSPtr_t At(const std::size_t INDEX) { return items_.at(INDEX); }

        inline std::size_t Size() const { return items_.size(); }
        inline bool Empty() const { return items_.empty(); }

        void Add(const ListBoxItemSPtr_t & ITEM_SPTR);

        bool Remove(const ListBoxItemSPtr_t & THING_SPTR);

        inline bool RemoveSelected() { return Remove(Selected()); }
        inline void Clear() { items_.clear(); }

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        virtual bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT) override;

        //if there is an image at location, then return the rect of the image
        const sf::FloatRect ImageRectOfItemAtPos(const sf::Vector2f & MOUSE_POS_V);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        inline const sf::Color GetHighlightColor() const { return highlightColor_; }
        inline void SetHighlightColor(const sf::Color & C) { highlightColor_ = C; }

        inline bool WillPlaySoundEffects() const { return willPlaySfx_; }
        inline void WillPlaySoundEffects(const bool B) { willPlaySfx_ = B; }

        inline const ListBoxItemSVec_t Items() const { return items_; }
        void Items(const ListBoxItemSVec_t & NEW_ITEMS_VEC);

        //these two functions return true if the selection changed
        bool SelectPrev();
        bool SelectNext();

    protected:
        std::size_t CalcVisibleItems() const;
        std::size_t CalcGreatestFirstDisplayedIndex() const;
        void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT);
        inline void OnClick(const sf::Vector2f &) override {}
        virtual void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V) override;
        void SetupForDraw();

        void LoadImageAndSetPosition(
            ListBoxItemSPtr_t & LISTBOXITEM_SPTR,
            const float         POS_TOP,
            const bool          IS_SELECTED_ITEM);

        std::size_t CalcVisibleCount() const;
        std::size_t CalcLastVisibleIndex_Display() const;
        std::size_t CalcLastVisibleIndex_Selection() const;

        void PlaySfx() const;

        void DrawLine(sf::RenderTarget & target, const float POS_TOP) const;

        bool IsPosWithinItemRegion(
            const sf::Vector2f & POS_V,
            const ListBoxItemSPtr_t & ITEM_SPTR) const;

    public:
        static const sf::FloatRect ERROR_RECT_;

    protected:
        const float       IMAGE_HORIZ_PAD_;
        const float       IMAGE_SIZE_;
        box::BoxUPtr_t    boxUPtr_;
        SliderBarUPtr_t   sliderbarUPtr_;
        sf::Color         lineColor_;
        sf::Color         highlightColor_;
        float             margin_;
        float             betweenPad_;
        IStage *          stagePtr_;
        ListBoxItemSVec_t items_;
        ImageMap_t        imageMap_;
        sf::Color         imageColor_;
        bool              willPlaySfx_;
        callback::IListBoxCallbackHandler * callbackPtr_;
        
        //which item is displayed at the top of the list
        std::size_t displayIndex_;

        //which item item is displayed at the top of the list when the selection is displayed
        std::size_t selectionDisplayIndex_;

        //offset from selectionIndexA_ to the actual selection
        std::size_t selectionOffsetIndex_;

        //how many items are listed
        std::size_t visibleCount_;
    };

}
}
}

#endif //HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
