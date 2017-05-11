#ifndef SFMLUTIL_CHECKBOX_INCLUDED
#define SFMLUTIL_CHECKBOX_INCLUDED
//
// check-box.hpp
//  Check box drawing and handling code.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/two-state-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/handy-types.hpp"

#include <memory>
#include <string>
#include <vector>
#include <deque>


namespace sfml_util
{
namespace gui
{

    //Represents a single check-able box
    class CheckBox : public TwoStateEntity
    {
    public:
        CheckBox(const std::string &    NAME,
                 const Brightness::Enum BRIGHTNESS,
                 const float            POS_LEFT,
                 const float            POS_TOP,
                 const sf::Sprite &     SPRITE_UP,
                 const sf::Sprite &     SPRITE_DOWN,
                 const MouseTextInfo &  MOUSE_TEXT_INFO,
                 const bool             IS_CHECKED,
                 const bool             IS_INVALID = false);

        CheckBox(const std::string &    NAME,
                 const Brightness::Enum BRIGHTNESS,
                 const float            POS_LEFT,
                 const float            POS_TOP,
                 const MouseTextInfo &  MOUSE_TEXT_INFO,
                 const bool             IS_CHECKED,
                 const bool             IS_INVALID = false);

        CheckBox(const std::string &    NAME,
                 const Brightness::Enum BRIGHTNESS,
                 const float            POS_LEFT,
                 const float            POS_TOP,
                 const TextInfo &       TEXT_INFO_UP,
                 const bool             IS_CHECKED,
                 const bool             IS_INVALID = false);

        virtual ~CheckBox();

        void Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID);

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

    private:
        virtual void SetupSprites(const Brightness::Enum BRIGHTNESS);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
    };


    using CheckBoxSPtr_t = std::shared_ptr<CheckBox>;
    using CheckBoxSVec_t = std::vector<CheckBoxSPtr_t>;


    //Represents a set of radio buttons
    class CheckBoxSet : public GuiEntity
    {
        //prevent copy construction and copy assignment
        CheckBoxSet(const CheckBoxSet &) =delete;
        CheckBoxSet & operator=(const CheckBoxSet &) =delete;

    public:
        //Constructor for cases where each radio button's label text has a different style, size, etc.
        CheckBoxSet(const std::string &       NAME,
                    const float               POS_LEFT,
                    const float               POS_TOP,
                    const TextInfoVec_t &     TEXT_INFO_VEC,
                    const Brightness::Enum    BRIGHTNESS,
                    const IndexVec_t &        INVALID_SEL_VEC    = IndexVec_t(),
                    const gui::box::Info &    BOX_INFO           = gui::box::Info(),
                    const std::deque<bool> &  INITIAL_SELECTIONS = std::deque<bool>(),
                    const float               OUTER_PAD          = OUTER_PAD_DEFAULT_,
                    const float               BETWEEN_PAD        = BETWEEN_PAD_DEFAULT_);

        CheckBoxSet(const std::string &        NAME,
                    const float                POS_LEFT,
                    const float                POS_TOP,
                    const MouseTextInfoVec_t & TEXT_INFO_VEC,
                    const Brightness::Enum     BRIGHTNESS,
                    const IndexVec_t &         INVALID_SEL_VEC    = IndexVec_t(),
                    const gui::box::Info &     BOX_INFO           = gui::box::Info(),
                    const std::deque<bool> &   INITIAL_SELECTIONS = std::deque<bool>(),
                    const float                OUTER_PAD          = OUTER_PAD_DEFAULT_,
                    const float                BETWEEN_PAD        = BETWEEN_PAD_DEFAULT_);

        //Constructor for cases where each radio button's label text has the same style, size, etc.
        CheckBoxSet(const std::string &               NAME,
                    const float                       POS_LEFT,
                    const float                       POS_TOP,
                    const TextInfo &                  TEXT_INFO,
                    const std::vector<std::string> &  LABEL_VEC,
                    const Brightness::Enum            BRIGHTNESS,
                    const IndexVec_t &                INVALID_SEL_VEC    = IndexVec_t(),
                    const gui::box::Info &            BOX_INFO           = gui::box::Info(),
                    const std::deque<bool> &          INITIAL_SELECTIONS = std::deque<bool>(),
                    const float                       OUTER_PAD          = OUTER_PAD_DEFAULT_,
                    const float                       BETWEEN_PAD        = BETWEEN_PAD_DEFAULT_);

        virtual ~CheckBoxSet();

        void SetupInitialSelections();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        const std::vector<std::size_t> GetCheckedNumbers() const;
        const CheckBoxSVec_t GetCheckedBoxes();

        inline const IndexVec_t GetInvalidSelections() const { return invalidSelectionsVec_; }

        //returns true if any boxes were checked or un-checked
        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);

        //cannot change any checkboxes, so always  returns false
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

        //returns true if any MouseStates were changed
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnColorChange();

        bool IsInvalid(const std::size_t INDEX);

        void PostSetupTasks(const float            POS_LEFT,
                            const float            POS_TOP,
                            const gui::box::Info & BOX_INFO);

        void SetCheckboxPositions(const float POS_LEFt, const float POS_TOP);

    private:
        static const float OUTER_PAD_DEFAULT_;
        static const float BETWEEN_PAD_DEFAULT_;
        //
        float            outerPad_;
        float            betweenPad_;
        std::size_t      downInWhichRegion_;
        std::deque<bool> currentSelections_;
        CheckBoxSVec_t   checkBoxSVec_;
        box::Box         box_;
        IndexVec_t       invalidSelectionsVec_;
    };

    using CheckBoxSetSPtr_t = std::shared_ptr<CheckBoxSet>;
    using CheckBoxSetSVec_t = std::vector<CheckBoxSetSPtr_t>;
}
}
#endif //SFMLUTIL_CHECKBOX_INCLUDED
