#ifndef GAME_MAINMENUTITLE_INCLUDED
#define GAME_MAINMENUTITLE_INCLUDED
//
// main-menu-title.hpp
//  Code that draws the title symbol and title text at the top of a main menu screen
//
#include "sfml-util/sfml-util.hpp"
#include <string>


namespace game
{

    //Responsible for drawing the title text and symbol at the top of a main menu stage screen.
    class MainMenuTitle : public sf::Drawable
    {
    public:
        //this constructor calls Setup() and SetPositionAndSize()
        explicit MainMenuTitle(const std::string & TITLE_IMAGE_FILENAME = "",
                               const bool          WILL_INVERT_SYMBOL   = false,
                               const float         SYMBOL_SCALE_HORIZ   = 1.0f,
                               const float         SYMBOL_SCALE_VERT    = 1.0f);

        virtual ~MainMenuTitle();

        void Setup(const std::string & TITLE_IMAGE_FILENAME = "",
                   const bool          WILL_INVERT_SYMBOL   = false,
                   const float         SYMBOL_SCALE_HORIZ   = 1.0f,
                   const float         SYMBOL_SCALE_VERT    = 1.0f);

        void SetPositionAndSize(const float SYMBOL_SCALE_HORIZ = 1.0f,
                                const float SYMBOL_SCALE_VERT  = 1.0f);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        inline const sf::FloatRect Region() const   { return symbolSprite_.getGlobalBounds(); }
        inline float LowerPad() const               { return sfml_util::MapByRes(35.0f, 135.0f); }
        inline float LowerPosition(const bool WILL_PAD = true) const { if (WILL_PAD) return Region().top + Region().height + LowerPad(); else return Region().top + Region().height; }

    private:
        sfml_util::TextureSPtr_t symbolTextureSPtr_;
        sf::Sprite               symbolSprite_;
        sfml_util::TextureSPtr_t titleTextureSPtr_;
        sf::Sprite               titleSprite_;
    };

}

#endif //GAME_MAINMENUTITLE_INCLUDED
