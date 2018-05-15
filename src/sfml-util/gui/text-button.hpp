// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTBUTTON_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTBUTTON_HPP_INCLUDED
//
// text-button.hpp
//  Drawing and handling code for buttons that are only text.
//
#include "misc/not-null.hpp"
#include "sfml-util/gui/gui-entity-text.hpp"
#include "sfml-util/i-callback-handler.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        class MouseTextInfo;

        class TextButton;
        namespace callback
        {
            using TextButtonCallbackPackage_t = sfml_util::callback::PtrWrapper<TextButton>;

            using ITextButtonCallbackHandler_t
                = sfml_util::callback::ICallbackHandler<TextButtonCallbackPackage_t, bool>;

            using ITextButtonCallbackHandlerPtr_t = misc::NotNull<ITextButtonCallbackHandler_t *>;
        } // namespace callback

        // Base class for a text button that has different text styles for mouse positions
        class TextButton : public GuiText
        {
        public:
            TextButton(const TextButton &) = delete;
            TextButton(TextButton &&) = delete;
            TextButton & operator=(const TextButton &) = delete;
            TextButton & operator=(TextButton &&) = delete;

        public:
            // if using this constructor, Setup() must be called before any other functions
            explicit TextButton(
                const std::string & NAME, const callback::ITextButtonCallbackHandlerPtr_t);

            TextButton(
                const std::string & NAME,
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const callback::ITextButtonCallbackHandlerPtr_t);

            virtual ~TextButton();

            void Setup(
                const float POS_LEFT,
                const float POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const callback::ITextButtonCallbackHandlerPtr_t);

        protected:
            virtual void OnClick(const sf::Vector2f &);

        private:
            callback::ITextButtonCallbackHandlerPtr_t callbackHandlerPtr_;
        };

        using TextButtonUPtr_t = std::unique_ptr<TextButton>;
        using TextButtonUVec_t = std::vector<TextButtonUPtr_t>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTBUTTON_HPP_INCLUDED
