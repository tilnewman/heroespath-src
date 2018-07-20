// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_CHECKBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_CHECKBOX_HPP_INCLUDED
//
// check-box.hpp
//  Check box drawing and handling code.
//
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/two-state-entity.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Represents a single check-able box
        class CheckBox : public TwoStateEntity
        {
        public:
            CheckBox(const CheckBox &) = delete;
            CheckBox(CheckBox &&) = delete;
            CheckBox & operator=(const CheckBox &) = delete;
            CheckBox & operator=(CheckBox &&) = delete;

            CheckBox(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const sf::Sprite & SPRITE_UP,
                const sf::Sprite & SPRITE_DOWN,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const bool IS_CHECKED,
                const bool IS_INVALID = false);

            CheckBox(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const bool IS_CHECKED,
                const bool IS_INVALID = false);

            CheckBox(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfo & TEXT_INFO_UP,
                const bool IS_CHECKED,
                const bool IS_INVALID = false);

            virtual ~CheckBox();

            void Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID);

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        private:
            virtual void SetupSprites(const Brightness::Enum BRIGHTNESS);

        protected:
            void OnClick(const sf::Vector2f &) override {}
        };

        using CheckBoxUPtr_t = std::unique_ptr<CheckBox>;
        using CheckBoxUVec_t = std::vector<CheckBoxUPtr_t>;

        // Represents a set of radio buttons
        class CheckBoxSet : public GuiEntity
        {
        public:
            CheckBoxSet(const CheckBoxSet &) = delete;
            CheckBoxSet(CheckBoxSet &&) = delete;
            CheckBoxSet & operator=(const CheckBoxSet &) = delete;
            CheckBoxSet & operator=(CheckBoxSet &&) = delete;

            // Constructor for cases where each radio button's label text has a different style,
            // size, etc.
            CheckBoxSet(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfoVec_t & TEXT_INFO_VEC,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const gui::box::Info & BOX_INFO = gui::box::Info(),
                const std::deque<bool> & INITIAL_SELECTIONS = std::deque<bool>(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            CheckBoxSet(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfoVec_t & TEXT_INFO_VEC,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const gui::box::Info & BOX_INFO = gui::box::Info(),
                const std::deque<bool> & INITIAL_SELECTIONS = std::deque<bool>(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            // Constructor for cases where each radio button's label text has the same style, size,
            // etc.
            CheckBoxSet(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfo & TEXT_INFO,
                const std::vector<std::string> & LABEL_VEC,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const gui::box::Info & BOX_INFO = gui::box::Info(),
                const std::deque<bool> & INITIAL_SELECTIONS = std::deque<bool>(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            virtual ~CheckBoxSet();

            void SetupInitialSelections();

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            const std::vector<std::size_t> GetCheckedNumbers() const;

            const std::vector<std::size_t> GetInvalidSelections() const
            {
                return invalidSelectionsVec_;
            }

            // returns true if any boxes were checked or un-checked
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

            // cannot change any checkboxes, so always  returns false
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

            // returns true if any MouseStates were changed
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

        protected:
            void OnClick(const sf::Vector2f &) override {}
            void OnColorChange() override;

            bool IsInvalid(const std::size_t INDEX);

            void PostSetupTasks(
                const float POS_LEFT, const float POS_TOP, const gui::box::Info & BOX_INFO);

            void SetCheckboxPositions(const float POS_LEFt, const float POS_TOP);

        private:
            static const float OUTER_PAD_DEFAULT_;
            static const float BETWEEN_PAD_DEFAULT_;
            //
            float outerPad_;
            float betweenPad_;
            std::size_t downInWhichRegion_;
            std::deque<bool> currentSelections_;
            CheckBoxUVec_t checkBoxUVec_;
            box::Box box_;
            std::vector<std::size_t> invalidSelectionsVec_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_CHECKBOX_HPP_INCLUDED
