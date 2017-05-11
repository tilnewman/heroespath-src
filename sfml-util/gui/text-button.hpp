#ifndef SFMLUTIL_TEXTBUTTON_INCLUDED
#define SFMLUTIL_TEXTBUTTON_INCLUDED
//
// text-button.hpp
//  Drawing and handling code for buttons that are only text.
//
#include "sfml-util/gui/gui-entity-text.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    class MouseTextInfo;


    //Base class for a text button that has different text styles for mouse positions
    class TextButton : public GuiText
    {
        //prevent copy construction
        TextButton(const TextButton &) =delete;

        //prevent copy assignment
        TextButton & operator=(const TextButton &) =delete;

    public:
        //if using this constructor, Setup() must be called before any other functions
        TextButton(const std::string & NAME);

        TextButton( const std::string &   NAME,
                    const float           POS_LEFT,
                    const float           POS_TOP,
                    const MouseTextInfo & MOUSE_TEXT_INFO,
                    const float           CLICK_SHIFT = CLICK_SHIFT_DEFAULT_);

        virtual ~TextButton();

        void Setup(const float           POS_LEFT,
                   const float           POS_TOP,
                   const MouseTextInfo & MOUSE_TEXT_INFO,
                   const float           CLICK_SHIFT = CLICK_SHIFT_DEFAULT_);

    protected:
        static float CLICK_SHIFT_DEFAULT_;
        inline virtual void OnClick(const sf::Vector2f &) {}

    private:
        float clickShift_;
    };

    using TextButtonSPtr_t = std::shared_ptr<TextButton>;
    using TextButtonSVec_t = std::vector<TextButtonSPtr_t>;

}
}
#endif //SFMLUTIL_TEXTBUTTON_INCLUDED
