#ifndef APPBASE_SFMLUTIL_ISTAGE_INCLUDED
#define APPBASE_SFMLUTIL_ISTAGE_INCLUDED
//
// i-stage.hpp
//  Interface for all Stage objects.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/i-gui-entity.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{

    //interface for all Stage classes
    class IStage
    {
    public:
        virtual ~IStage() {}

        virtual void Setup() = 0;

        virtual const std::string GetStageName() const = 0;

        inline virtual const sf::FloatRect StageRegion() const = 0;
        inline virtual void StageRegionSet(const sf::FloatRect & RECT) = 0;

        virtual float StageRegionLeft() const = 0;
        virtual float StageRegionTop() const = 0;
        virtual float StageRegionWidth() const = 0;
        virtual float StageRegionHeight() const = 0;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) = 0;
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual void UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA) = 0;

        virtual gui::IGuiEntitySPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual bool KeyPress(const sf::Event::KeyEvent & KE) = 0;
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE) = 0;

        virtual gui::IGuiEntitySPtr_t GetEntityWithFocus() const = 0;
        virtual void RemoveFocus() = 0;
        virtual bool SetFocus(const gui::IGuiEntitySPtr_t & ENTITY_SPTR) = 0;

        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states) = 0;

        virtual void HandleResolutionChange() = 0;

        //throws if the entity to add was already there
        virtual bool EntityAdd(const gui::IGuiEntitySPtr_t &) = 0;

        //returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IGuiEntitySPtr_t &) = 0;

        virtual const gui::IGuiEntitySSet_t EntitySetCopy() = 0;

        virtual void SetMouseHover(const sf::Vector2f &, const bool) = 0;

        virtual void TestingStrAppend(const std::string &) = 0;
        virtual void TestingStrIncrement(const std::string &) = 0;
        virtual void TestingImageSet(const sfml_util::TextureSPtr_t &) = 0;
        virtual void PerformNextTest() = 0;
    };

    using IStageSPtr_t = std::shared_ptr<IStage>;
    using IStageSVec_t = std::vector<IStageSPtr_t>;

}

#endif //APPBASE_SFMLUTIL_ISTAGE_INCLUDED
