#ifndef SFMLUTIL_TWOSTATEENTITY_INCLUDED
#define SFMLUTIL_TWOSTATEENTITY_INCLUDED
//
// two-state-entity.hpp
//  Code that combines a GuiImage and GuiText into an object that can be in one of two states.
//  This is useful as a base class for buttons, checkboxes, radio buttons, etc.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/gui-entity-text.hpp"
#include "sfml-util/gui/gui-entity-image.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Use guiImage_.upSprite as the first state sprite, and guiImage_.downSprite as the second.
    //Prevents using guiImage_.overSprite, but guiText_.overTextInfo_ is allowed.
    class TwoStateEntity : public GuiEntity
    {
        //prevent copy construction
        TwoStateEntity(const TwoStateEntity &) =delete;

        //prevent copy assignment
        TwoStateEntity & operator=(const TwoStateEntity &) =delete;

    public:
        TwoStateEntity( const std::string &   NAME,
                        const float           POS_LEFT,
                        const float           POS_TOP,
                        const sf::Sprite &    SPRITE_UP,
                        const sf::Sprite &    SPRITE_DOWN,
                        const MouseTextInfo & MOUSE_TEXT_INFO);

        TwoStateEntity( const std::string &   NAME,
                        const float           POS_LEFT,
                        const float           POS_TOP,
                        const MouseTextInfo & MOUSE_TEXT_INFO);

        TwoStateEntity( const std::string & NAME,
                        const float         POS_LEFT,
                        const float         POS_TOP,
                        const TextInfo &    TEXT_INFO_UP );

        virtual ~TwoStateEntity();

        inline virtual bool IsInFirstState() const { return (MouseState::Down != guiImage_.GetMouseState()); }
        virtual void        SetIsInFirstState(const bool IS_IN_FIRST_STATE);

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        inline virtual const std::string GetText() const { return guiText_.GetText(); }

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetMouseState(const MouseState::Enum);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}

        //call this function after constructors and after the sprites have been setup
        virtual void PostSpriteSetup(const float POS_LEFT, const float POS_TOP);

        static const float HORIZ_PAD_;
        static const float UPPER_PAD_;

    protected:
        GuiText  guiText_;
        GuiImage guiImage_;
        bool     wasDownInRegion_;
        //use guiImage_.upSprite_ as first state sprite, and guiImage_.downSprite for the second state.
    };


    using TwoStateEntitySPtr_t = std::shared_ptr<TwoStateEntity>;
    using TwoStateEntitySVec_t = std::vector<TwoStateEntitySPtr_t>;

}
}

#endif //SFMLUTIL_TWOSTATEENTITY_INCLUDED
