#ifndef APPBASE_SFMLUTIL_GUI_LISTBOX_INCLUDED
#define APPBASE_SFMLUTIL_GUI_LISTBOX_INCLUDED
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

#include <memory>
#include <utility>
#include <string>
#include <list>
#include <map>


namespace sfml_util
{
    class IStage;
    using IStageSPtr_t = std::shared_ptr<IStage>;

namespace gui
{

    namespace box
    {
        class Box;
        using BoxSPtr_t = std::shared_ptr<Box>;
    }

    class ListBoxItem;
    using ListBoxItemSPtr_t = std::shared_ptr<ListBoxItem>;
    using ListBoxItemSLst_t = std::list<ListBoxItemSPtr_t>;

    //types required for the ListBoxPtrPackage_t and IListBoxCallbackHandler below
    class ListBox;
    using ListBoxSPtr_t = std::shared_ptr<ListBox>;

    using ImageMap_t      = std::map<ListBoxItemSPtr_t, std::pair<sfml_util::TextureSPtr_t, sf::Sprite> >;
    using ImageMapIter_t  = ImageMap_t::iterator;
    using ImageMapCIter_t = ImageMap_t::const_iterator;


    //the callback type for ListBox is a wrapper class called ListBoxItem
    namespace callback
    {
        using ListBoxPtrPackage_t = sfml_util::callback::PtrWrapper<ListBox>;

        struct ListBoxEventPackage
        {
            static const int INVALID_SELECTION_;
            static const sf::Vector2f INVALID_MOUSE_POS_V_;

            explicit ListBoxEventPackage(ListBoxPtrPackage_t              PACKAGE,
                                         const sfml_util::GuiEvent::Enum  GUI_EVENT         = sfml_util::GuiEvent::None,
                                         const sf::Vector2f &             MOUSE_POS_V       = INVALID_MOUSE_POS_V_,
                                         const int                        CURR_SELECTION    = INVALID_SELECTION_,
                                         const bool                       HAS_FOCUS_CHANGED = false,
                                         const sf::Event::KeyEvent        KEYPRESS_EVENT    = sf::Event::KeyEvent(),
                                         const sf::Event::MouseWheelEvent MOUSEWHEEL_EVENT  = sf::Event::MouseWheelEvent())
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

        using IListBoxCallbackHandler = sfml_util::callback::ICallbackHandler<ListBoxEventPackage, bool>;
    }


    //A class that manages a vertical list of IGuiEntitys that can be scrolled through and selected.
    //The template type Item_t must be/descend/implement IGuiEntity.
    class ListBox
    :
        public GuiEntity,
        public callback::ISliderBarCallbackHandler_t
    {
        //prevent copy construction
        ListBox(const ListBox &);

        //prevent copy construction
        ListBox & operator=(const ListBox &);

    public:
        explicit ListBox(const std::string &       NAME,
                         const sf::FloatRect &     REGION      = sf::FloatRect(),
                         const ListBoxItemSLst_t & ITEM_LIST   = ListBoxItemSLst_t(),
                         IStage * const            stagePtr    = nullptr,
                         const float               MARGIN      = 0.0f,
                         const float               BETWEEN_PAD = 0.0f,
                         const box::Info &         BOX_INFO    = box::Info(),
                         const sf::Color &         LINE_COLOR  = sf::Color::Transparent,
                         const int                 ITEM_LIMIT  = NO_LIMIT_,
                         callback::IListBoxCallbackHandler * const callbackPtr = nullptr);

        virtual ~ListBox();

        void Setup(const sf::FloatRect &     REGION      = sf::FloatRect(),
                   const ListBoxItemSLst_t & ITEM_LIST   = ListBoxItemSLst_t(),
                   IStage * const            stagePtr    = nullptr,
                   const float               MARGIN      = 0.0f,
                   const float               BETWEEN_PAD = 0.0f,
                   const box::Info &         BOX_INFO    = box::Info(),
                   const sf::Color &         LINE_COLOR  = sf::Color::Transparent,
                   const int                 ITEM_LIMIT  = NO_LIMIT_,
                   callback::IListBoxCallbackHandler * const callbackPtr = nullptr);

        inline virtual const std::string HandlerName() const { return GetEntityName(); }

        virtual bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE);

        inline void SetImageHeight(const float HEIGHT)      { imageSize_ = HEIGHT; }
        inline float GetImageHeight() const                 { return imageSize_; }

        inline void SetImageColor(const sf::Color & COLOR)  { imageColor_ = COLOR; }
        inline const sf::Color GetImageColor() const        { return imageColor_; }

        void DrawLine(sf::RenderTarget & target, const float POS_TOP) const;

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        ListBoxItemSPtr_t GetAtPosition(const sf::Vector2f & POS_V);

        inline const ListBoxItemSPtr_t GetSelected() const  { return selectedSPtr_; }

        std::size_t GetSelectedIndex() const;

        bool IsIndexValid(const std::size_t) const;

        //returns true if the selected index was valid and set
        bool SetSelectedIndex(const std::size_t NEW_INDEX, const bool WILL_PLAY_SOUNDEFFECT = true);

        ListBoxItemSPtr_t At(const std::size_t INDEX);

        inline std::size_t GetCount() const     { return list_.size(); }
        inline bool Empty() const               { return list_.empty(); }
        inline std::size_t GetLimit() const     { return itemLimit_; }

        void Add(const ListBoxItemSPtr_t & THING_SPTR,
                 const bool                WILL_INC_CURRENT_SEL = false);

        bool Remove(const ListBoxItemSPtr_t & THING_SPTR);

        inline bool RemoveSelected()            { return Remove(selectedSPtr_); }
        inline void Clear()                     { list_.clear(); }

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);

        virtual bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT);

        ListBoxItemSPtr_t GetItemAtLocation(const sf::Vector2f & MOUSE_POS_V);

        //if there is an image at location, then return the rect of the image
        const sf::FloatRect GetRectAtLocation(const sf::Vector2f & MOUSE_POS_V);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);

        inline const sf::Color GetHighlightColor() const { return highlightColor_; }
        inline void SetHighlightColor(const sf::Color C) { highlightColor_ = C; }

    protected:
        void CreateSelectionChangePackageAndCallHandler(const std::size_t NEW_SELECTED_INDEX);
        void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT);
        inline virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V);
        void SetupList();
        float GetTotalHeight() const;
        void PopulateImageMapAndAdjustLeftPos(ListBoxItemSPtr_t & LISTBOXITEM_SPTR,
                                              const float         POS_LEFT_ORIG,
                                              const float         POS_TOP,
                                              const bool          IS_SELECTED_ITEM);

        bool MoveSelectionUp();
        bool MoveSelectionDown();

    public:
        static const int NO_LIMIT_;
        static const sf::FloatRect ERROR_RECT_;

    protected:
        const float       IMAGE_HORIZ_PAD_;
        box::BoxSPtr_t    boxSPtr_;
        SliderBarSPtr_t   sliderBarSPtr_;
        sf::Color         lineColor_;
        sf::Color         highlightColor_;
        float             currentViewPos_;
        float             margin_;
        float             betweenPad_;
        IStage *          stagePtr_;
        ListBoxItemSLst_t list_;
        ListBoxItemSPtr_t selectedSPtr_;
        int               itemLimit_;
        float             imageSize_;
        ImageMap_t        imageMap_;
        sf::Color         imageColor_;
        callback::IListBoxCallbackHandler * callbackPtr_;
    };

}
}
#endif //APPBASE_SFMLUTIL_GUI_LISTBOX_INCLUDED
