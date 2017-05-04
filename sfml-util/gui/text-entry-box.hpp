#ifndef SFMLUTIL_GUI_TEXTENTRYBOX_INCLUDED
#define SFMLUTIL_GUI_TEXTENTRYBOX_INCLUDED
//
// text-entry-box.hpp
//  Code for a box that users can type a single line of text into, with a blinking cursor, etc.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/box-info.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    class TextRegion;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;


    class TextEntryBox;
    namespace callback
    {
        using TextEntryBoxCallbackPackage_t = sfml_util::callback::PtrWrapper<TextEntryBox>;
        using ITextEntryBoxCallbackHandler_t = sfml_util::callback::ICallbackHandler<TextEntryBoxCallbackPackage_t, bool>;
    }


    //Encapsulates a single line of user type-able text with a blinking cursor
    class TextEntryBox : public GuiEntity
    {
        //prevent copy construction and copy assignment
        TextEntryBox(const TextEntryBox &);
        TextEntryBox & operator=(const TextEntryBox &);

    public:
        //if using this constructor, Setup() must be called before any other function
        TextEntryBox(const std::string & NAME);

        TextEntryBox(const std::string &   NAME,
                     const sf::FloatRect & REGION,
                     const TextInfo &      TEXT_INFO,
                     const sf::Color &     CURSOR_COLOR = sf::Color::White,
                     const box::Info &     BOX_INFO     = box::Info(),
                     callback::ITextEntryBoxCallbackHandler_t * const CHANGE_HANDLER_PTR = nullptr);

        virtual ~TextEntryBox();

        void Setup(const sf::FloatRect & REGION,
                   const TextInfo &      TEXT_INFO,
                   const box::Info &     BOX_INFO = box::Info());

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetText(const std::string & NEW_TEXT);
        virtual const std::string GetText() const;

        virtual bool KeyPress(const sf::Event::KeyEvent &);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

        virtual bool UpdateTime(const float);

        virtual void SetTextColor(const sf::Color & TEXT_COLOR);

    protected:
        void UpdateText();
        virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnColorChange();

    protected:
        static const float  INNER_PAD_;
        static const float  CURSOR_WIDTH_;
        static const float  CURSOR_BLINK_DELAY_SEC_;
        //
        box::Box         box_;
        TextInfo         textInfo_;
        sf::FloatRect    cursorRect_;
        sf::Color        cursorColor_;
        sf::FloatRect    innerRegion_;
        TextRegionSPtr_t textRegionSPtr_;
        bool             willDrawCursor_;
        float            cursorBlinkTimer_;
        callback::ITextEntryBoxCallbackHandler_t * callbackHandlerPtr_;
    };

    using TextEntryBoxSPtr_t = std::shared_ptr<TextEntryBox>;
}
}
#endif //SFMLUTIL_GUI_TEXTENTRYBOX_INCLUDED
