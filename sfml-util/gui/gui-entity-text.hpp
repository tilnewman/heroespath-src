#ifndef SFMLUTIL_GUITEXT_INCLUDED
#define SFMLUTIL_GUITEXT_INCLUDED
//
// gui-entity-text.hpp
//  Code for handling a piece of text that can be in one of three states.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    namespace text_render
    {
        class RenderedText;
        using RenderedTextSPtr_t = std::shared_ptr<RenderedText>;
    }

namespace gui
{

    //Base class for gui text that can be in any of MouseState::Enum states.
    class GuiText : public GuiEntity
    {
        //prevent copy construction
        GuiText(const GuiText &);

        //prevent copy assignment
        GuiText & operator=(const GuiText &);

    public:
        //if this constructor is used, then Setup() must be called before any other functions
        GuiText(const std::string & NAME,
                const float         TEXT_WIDTH_LIMIT = 0.0f,
                const FontSPtr_t &  NUMBERS_FONTSPTR = FontSPtr_t());

        GuiText(const std::string &   NAME,
                const sf::FloatRect & REGION,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const float           TEXT_WIDTH_LIMIT = 0.0f,
                const FontSPtr_t &    NUMBERS_FONTSPTR = FontSPtr_t());

        GuiText(const std::string &   NAME,
                const float           POS_LEFT,
                const float           POS_TOP,
                const MouseTextInfo & MOUSE_TEXT_INFO,
                const float           TEXT_WIDTH_LIMIT = 0.0f,
                const FontSPtr_t &    NUMBERS_FONTSPTR = FontSPtr_t());

        virtual ~GuiText();

        virtual void SetMouseState(const MouseState::Enum E);

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        inline virtual const std::string GetText() const { return text_; }

        inline virtual float GetTextWidth() const { return sprite_.getLocalBounds().width; }
        inline virtual float GetTextHeight() const { return sprite_.getLocalBounds().height; }

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetNewColors(const sf::Color & UP_COLOR,
                                  const sf::Color & DOWN_COLOR,
                                  const sf::Color & OVER_COLOR);

        virtual void SpriteColorSet(const sf::Color & NEW_COLOR);
        virtual void SpriteColorReset();

        void Setup(const std::string &   TEXT,
                   const float           POS_LEFT,
                   const float           POS_TOP,
                   const MouseTextInfo & MOUSE_TEXT_INFO,
                   const float           TEXT_WIDTH_LIMIT = 0.0f,
                   const FontSPtr_t &    NUMBERS_FONTSPTR = FontSPtr_t());

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        virtual void ResetText();
        inline virtual void OnClick(const sf::Vector2f &) {}

    protected:
        std::string    text_;
        TextInfo       upTextInfo_;
        TextInfo       downTextInfo_;
        TextInfo       overTextInfo_;
        FontSPtr_t     numberFontSPtr_;
        RendTextSPtr_t textureSPtr_;
        sf::Sprite     sprite_;
        float          textWidthLimit_;
        text_render::RenderedTextSPtr_t renderedTextSPtr_;
    };


    using GuiTextSPtr_t = std::shared_ptr<GuiText>;
    using GuiTextSVec_t = std::vector<GuiTextSPtr_t>;

}
}
#endif //SFMLUTIL_GUITEXT_INCLUDED
