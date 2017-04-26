#ifndef APPBASE_SFMLUTIL_STAGE_INCLUDED
#define APPBASE_SFMLUTIL_STAGE_INCLUDED
//
// stage.hpp
//  Helper classes and functions for managing GuiEntity's in a collection on screen.
//
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>
#include <set>


namespace sfml_util
{

    //forward declaration
    namespace gui
    {
        class IGuiEntity;
        using IGuiEntitySPtr_t = std::shared_ptr<IGuiEntity>;
        using IGuiEntitySSet_t = std::set<IGuiEntitySPtr_t>;

        namespace box
        {
            class Box;
            using BoxSPtr_t = std::shared_ptr<Box>;
        }
    }


    //A base class for types that hold and draw a group of on screen resources.
    class Stage : public IStage
    {
        //prevent copy construction
        Stage(const Stage &);

        //prevent copy assignment
        Stage & operator=(const Stage &);

    public:
        Stage(const std::string & NAME);

        Stage(const std::string &   NAME,
              const sf::FloatRect & REGION);

        Stage(const std::string & NAME,
              const float         REGION_LEFT,
              const float         REGION_TOP,
              const float         REGION_WIDTH,
              const float         REGION_HEIGHT);

        virtual ~Stage();

        inline virtual void Setup() {}

        inline virtual const std::string GetStageName() const           { return STAGE_NAME_; }

        inline virtual const sf::FloatRect StageRegion() const          { return stageRegion_; }
        inline virtual void StageRegionSet(const sf::FloatRect & RECT)  { stageRegion_ = RECT; }

        inline virtual float StageRegionLeft() const                    { return stageRegion_.left;   }
        inline virtual float StageRegionTop() const                     { return stageRegion_.top;    }
        inline virtual float StageRegionWidth() const                   { return stageRegion_.width;  }
        inline virtual float StageRegionHeight() const                  { return stageRegion_.height; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA);

        virtual gui::IGuiEntitySPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        inline virtual gui::IGuiEntitySPtr_t GetEntityWithFocus() const  { return entityWithFocusSPtr_; }
        virtual void RemoveFocus();
        virtual bool SetFocus(const gui::IGuiEntitySPtr_t & ENTITY_SPTR);

        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);

        //only a required function for the SettingsStage which can change resolution
        virtual void HandleResolutionChange() {}

        //throws if the entity to add was already there
        virtual bool EntityAdd(const gui::IGuiEntitySPtr_t &);

        //returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IGuiEntitySPtr_t &);
        virtual bool EntityRemovePtr(gui::IGuiEntity *);

        inline virtual const gui::IGuiEntitySSet_t EntitySetCopy() { return entitySSet_; }

        virtual void SetMouseHover(const sf::Vector2f &, const bool);

        inline virtual void TestingStrAppend(const std::string &) {}
        inline virtual void TestingStrIncrement(const std::string &) {}
        inline virtual void TestingImageSet(const sfml_util::TextureSPtr_t &) {}
        inline virtual void PerformNextTest() {}

    protected:
        void DrawHoverText(sf::RenderTarget &, sf::RenderStates &);

    private:
        const std::string     STAGE_NAME_;
        sf::FloatRect         stageRegion_;
        gui::IGuiEntitySSet_t entitySSet_;
        gui::IGuiEntitySPtr_t entityWithFocusSPtr_;
        gui::box::BoxSPtr_t   hoverTextBoxSPtr_;
        sf::Text              hoverSfText_;
    };

    using StageSPtr_t = std::shared_ptr<Stage>;
    using StageSVec_t = std::vector<StageSPtr_t>;

}
#endif //APPBASE_SFMLUTIL_STAGE_INCLUDED
