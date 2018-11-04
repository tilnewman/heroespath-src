// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTBUTTON_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTBUTTON_HPP_INCLUDED
//
// text-button.hpp
//  Drawing and handling code for buttons that are only text.
//
#include "gui/text-entity.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Base class for a text button that has different text styles for mouse positions
    class TextButton : public TextEntity
    {
    public:
        using Callback_t = misc::Callback<TextButton>;

        TextButton(const TextButton &) = delete;
        TextButton(TextButton &&) = delete;
        TextButton & operator=(const TextButton &) = delete;
        TextButton & operator=(TextButton &&) = delete;

        // if using this constructor, Setup() must be called before any other functions
        TextButton(
            const std::string & NAME, const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT);

        TextButton(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT);

        virtual ~TextButton();

        void Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT);

    protected:
        void OnClick(const sf::Vector2f &) override;

    private:
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
    };

    using TextButtonUPtr_t = std::unique_ptr<TextButton>;
    using TextButtonUVec_t = std::vector<TextButtonUPtr_t>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTBUTTON_HPP_INCLUDED
