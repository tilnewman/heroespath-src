#ifndef APPBASE_SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
#define APPBASE_SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
//
// text-button-credits.hpp
//  A simple button that transfers to the credits screen
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/text-button.hpp"

#include <memory>
#include <string>


namespace heroespath
{

    //Base class for a
    class CreditsButton : public sfml_util::gui::TextButton
    {
        //prevent copy construction and copy assignment
        CreditsButton(const CreditsButton &);
        CreditsButton & operator=(const CreditsButton &);

    public:
        //if using this constructor, Setup() must be called before any other functions
        CreditsButton(const std::string & NAME);

        CreditsButton(const std::string &                   NAME,
                      const float                           POS_LEFT,
                      const float                           POS_TOP,
                      const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                      const float                           CLICK_SHIFT = TextButton::CLICK_SHIFT_DEFAULT_);

        virtual ~CreditsButton();

        void Setup(const float                           POS_LEFT,
                   const float                           POS_TOP,
                   const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                   const float                           CLICK_SHIFT = TextButton::CLICK_SHIFT_DEFAULT_);

    protected:
        inline virtual void OnClick(const sf::Vector2f &);
    };

    using CreditsButtonSPtr_t = std::shared_ptr<CreditsButton>;

}
#endif //APPBASE_SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
