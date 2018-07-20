// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_RADIOBUTTON_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_RADIOBUTTON_HPP_INCLUDED
//
// radio-button.hpp
//  Radio button drawing and handling code.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/two-state-entity.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class RadioButtonSet;
    }

    namespace callback
    {
        using RadioButtonCallbackPackage_t
            = sfml_util::callback::PtrWrapper<sfml_util::gui::RadioButtonSet>;

        using IRadioButtonSetCallbackHandler_t
            = ICallbackHandler<RadioButtonCallbackPackage_t, bool>;

        using RadioButtonSetCallbackHandlerPtr_t
            = misc::NotNull<IRadioButtonSetCallbackHandler_t *>;
    } // namespace callback

    namespace gui
    {

        // Represents a clickable radio button or check box, etc.
        class RadioButton : public TwoStateEntity
        {
        public:
            RadioButton(const RadioButton &) = delete;
            RadioButton(RadioButton &&) = delete;
            RadioButton & operator=(const RadioButton &) = delete;
            RadioButton & operator=(RadioButton &&) = delete;

            RadioButton(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const sf::Sprite & SPRITE_UP,
                const sf::Sprite & SPRITE_DOWN,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const bool IS_SELECTED = false,
                const bool IS_INVALID = false);

            RadioButton(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const bool IS_SELECTED = false,
                const bool IS_INVALID = false);

            RadioButton(
                const std::string & NAME,
                const Brightness::Enum BRIGHTNESS,
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfo & TEXT_INFO_UP,
                const bool IS_SELECTED = false,
                const bool IS_INVALID = false);

            virtual ~RadioButton();

            void Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID);

            void SetValidity(const bool IS_VALID);

            static const sf::Color COLOR_VALID_;
            static const sf::Color COLOR_INVALID_;

        private:
            virtual void SetupSprites(const Brightness::Enum BRIGHTNESS);
        };

        using RadioButtonUPtr_t = std::unique_ptr<RadioButton>;
        using RadioButtonUVec_t = std::vector<RadioButtonUPtr_t>;

        // Represents a set of radio buttons
        class RadioButtonSet : public GuiEntity
        {
        public:
            RadioButtonSet(const RadioButtonSet &) = delete;
            RadioButtonSet(RadioButtonSet &&) = delete;
            RadioButtonSet & operator=(const RadioButtonSet &) = delete;
            RadioButtonSet & operator=(RadioButtonSet &&) = delete;

        public:
            // If Using this constructor, then one of the Setup() functions must be called before
            // any other member
            explicit RadioButtonSet(
                const sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t,
                const std::string & NAME);

            // Constructor for cases where each radio button's label text has the same style, size,
            // etc.
            RadioButtonSet(
                const sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t,
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfo & TEXT_INFO,
                const std::vector<std::string> & LABEL_VEC,
                const std::size_t INITIAL_SELECTION,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const box::Info & BOX_INFO = box::Info(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            virtual ~RadioButtonSet();

            void Setup(
                const float POS_LEFT,
                const float POS_TOP,
                const TextInfo & TEXT_INFO,
                const std::vector<std::string> & LABEL_VEC,
                const std::size_t INITIAL_SELECTION,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const box::Info & BOX_INFO = box::Info(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            void Setup(
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfoVec_t & MOUSE_TEXT_INFO_VEC,
                const std::size_t INITIAL_SELECTION,
                const Brightness::Enum BRIGHTNESS,
                const std::vector<std::size_t> & INVALID_SEL_VEC = std::vector<std::size_t>(),
                const box::Info & BOX_INFO = box::Info(),
                const float OUTER_PAD = OUTER_PAD_DEFAULT_,
                const float BETWEEN_PAD = BETWEEN_PAD_DEFAULT_);

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            std::size_t GetSelectedNumber() const { return currentSelection_; }

            // throws on out of bounds, but will allow setting a grayed-out number
            void SetSelectNumber(const std::size_t);

            void SetInvalidSelections(const std::vector<std::size_t> & INVALID_SELECTIONS_VEC);
            const std::vector<std::size_t> GetInvalidSelections() const
            {
                return invalidSelectionVec_;
            }

            // returns true if the currentSelection_ changed
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

            // cannot change any states, so always returns false
            bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

            // returns true if any MouseStates were changed
            bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            bool SetHasFocus(const bool) override;

        protected:
            bool IsInvalid(const std::size_t INDEX) const;

            void PostSetupTasks(
                const float POS_LEFT, const float POS_TOP, const box::Info & BOX_INFO);

            void PositionRadioButtons(const float POS_LEFT, const float POS_TOP);

            void RevertToPreviousSelection();

            void ChangeCurrentSelection(const std::size_t NEW_SELECTION);

            void OnClick(const sf::Vector2f &) override;

            void OnColorChange() override;

            virtual void TriggerCallback();

        public:
            static const float OUTER_PAD_DEFAULT_;
            static const float BETWEEN_PAD_DEFAULT_;

        protected:
            sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t callbackHandlerPtr_;
            float outerPad_;
            float betweenPad_;
            std::size_t currentSelection_;
            RadioButtonUVec_t buttonUVec_;
            box::Box box_;
            std::size_t downInWhichRegion_;
            std::vector<std::size_t> invalidSelectionVec_;
            std::size_t prevSelection_;
        };

        using RadioButtonSetUPtr_t = std::unique_ptr<RadioButtonSet>;
        using RadioButtonSetUVec_t = std::vector<RadioButtonSetUPtr_t>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_RADIOBUTTON_HPP_INCLUDED
