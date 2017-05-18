#ifndef SFMLUTIL_ANIMATION_INCLUDED
#define SFMLUTIL_ANIMATION_INCLUDED
//
// animation.hpp
//  Code to handle animations on screen.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"

#include <string>
#include <vector>


namespace sfml_util
{

    //Encapsulates an animatin on screen that loads frames from a single texture.
    class SingleTextureAnimation : public gui::GuiEntity
    {
        //prevent copy construction and copy assignment
        SingleTextureAnimation(const SingleTextureAnimation &) =delete;
        SingleTextureAnimation & operator=(const SingleTextureAnimation &) =delete;

    public:
        SingleTextureAnimation(const std::string & ENTITY_NAME,
                               const std::string & TEXTURE_FILE_PATH,
                               const float         SCREEN_POS_LEFT,
                               const float         SCREEN_POS_TOP,
                               const unsigned int  FRAME_WIDTH,
                               const unsigned int  FRAME_HEIGHT,
                               const float         TIME_BETWEEN_FRAMES,
                               const std::size_t   FRAME_COUNT          = 0,//zero here means each texture region big enough for a frame contains a frame, and frameCount_ will be set in the constructor
                               const sf::BlendMode BLEND_MODE           = sf::BlendAlpha,
                               const float         SCALE_HORIZ          = 1.0f,
                               const float         SCALE_VERT           = 1.0f,
                               const sf::Color &   COLOR                = sf::Color::White,
                               const unsigned int  FIRST_FRAME_POS_LEFT = 0,
                               const unsigned int  FIRST_FRAME_POS_TOP  = 0,
                               const unsigned int  FRAME_HORIZ_GAP      = 0,
                               const unsigned int  FRAME_VERT_GAP       = 0);

        virtual ~SingleTextureAnimation();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetEntityPos(const sf::Vector2f & V);
        virtual void SetEntityPos(const float LEFT, const float TOP);

        inline virtual void OnClick(const sf::Vector2f &)   {}
        inline virtual bool MouseUp(const sf::Vector2f &)   { return false; }
        inline virtual bool MouseDown(const sf::Vector2f &) { return false; }
        inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

        //returns true if frame count wrapped around back to zero
        virtual bool UpdateTime(const float SECONDS);

        inline std::size_t CurrentFrame() const             { return currentFrame_; }
        inline std::size_t FrameCount() const               { return rects_.size(); }

        virtual void MovePosition(const float HORIZ, const float VERT);
        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);

        inline float TimeBetweenFrames() const              { return timeBetweenFrames_; }
        inline void TimeBetweenFramesSet(const float TBF)   { timeBetweenFrames_ = TBF; }
        inline void TimeBetweenFramesAdj(const float ADJ)   { timeBetweenFrames_ += ADJ; }

        inline const sf::Color Color() const                { return color_; }
        inline void Color(const sf::Color & NEW_COLOR)      { color_ = NEW_COLOR; }

    protected:
        const unsigned int FRAME_WIDTH_;
        const unsigned int FRAME_HEIGHT_;
        const std::size_t  FRAME_COUNT_;
        const unsigned int FIRST_FRAME_POS_LEFT_;
        const unsigned int FIRST_FRAME_POS_TOP_;
        const unsigned int FRAME_HORIZ_GAP_;
        const unsigned int FRAME_VERT_GAP_;
        //
        const sf::BlendMode      BLEND_MODE_;
        float                    timeBetweenFrames_;
        sf::Sprite               sprite_;
        TextureSPtr_t            textureSPtr_;
        std::vector<sf::IntRect> rects_;//the size of this vector acts as a total frame count.
        std::size_t              currentFrame_;//counts up to 'total frame count' then restarts at zero
        float                    frameTimerSec_;
        sf::Color                color_;
    };

    using SingleTextureAnimationSPtr_t = std::shared_ptr<SingleTextureAnimation>;
    using SingleTextureAnimationSVec_t = std::vector<SingleTextureAnimationSPtr_t>;


    //Encapsulates an animatin on screen that loads frames from multiple textures.
    class MultiTextureAnimation : public gui::GuiEntity
    {
        //prevent copy construction and copy assignment
        MultiTextureAnimation(const MultiTextureAnimation &) =delete;
        MultiTextureAnimation & operator=(const MultiTextureAnimation &) =delete;

    public:
        MultiTextureAnimation(const std::string & ENTITY_NAME,
                              const std::string & TEXTURES_DIRECTORY,
                              const float         SCREEN_POS_LEFT,
                              const float         SCREEN_POS_TOP,
                              const float         TIME_BETWEEN_FRAMES,
                              const float         SCALE_HORIZ = 1.0f,
                              const float         SCALE_VERT  = 1.0f,
                              const sf::Color &   COLOR       = sf::Color::White,
                              const sf::BlendMode BLEND_MODE  = sf::BlendAlpha);

        virtual ~MultiTextureAnimation();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetEntityPos(const sf::Vector2f & V);
        virtual void SetEntityPos(const float LEFT, const float TOP);

        inline virtual void OnClick(const sf::Vector2f &)        {}
        inline virtual bool MouseUp(const sf::Vector2f &)        { return false; }
        inline virtual bool MouseDown(const sf::Vector2f &)      { return false; }
        inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }
        inline virtual bool UpdateMouseWheel(const sf::Vector2f &, const float) { return false; }

        //returns true if frame count wrapped around back to zero
        virtual bool UpdateTime(const float SECONDS);

        inline std::size_t CurrentFrame() const             { return currentFrame_; }
        inline std::size_t FrameCount() const               { return textureSVec_.size(); }

        virtual void MovePosition(const float HORIZ, const float VERT);
        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);

        inline float TimeBetweenFrames() const              { return timeBetweenFrames_; }
        inline void TimeBetweenFramesSet(const float TBF)   { timeBetweenFrames_ = TBF; }
        inline void TimeBetweenFramesAdj(const float ADJ)   { timeBetweenFrames_ += ADJ; }

        inline const sf::Color Color() const                { return color_; }
        inline void Color(const sf::Color & NEW_COLOR)      { color_ = NEW_COLOR; }

    protected:
        const sf::BlendMode BLEND_MODE_;
        float               timeBetweenFrames_;
        sf::Sprite          sprite_;
        TextureSVec_t       textureSVec_;//the size of this vec acts as a total frame count
        std::size_t         currentFrame_;//counts up to 'total frame count' then restarts at zero
        float               frameTimerSec_;
        sf::Color           color_;
    };

    using MultiTextureAnimationSPtr_t = std::shared_ptr<MultiTextureAnimation>;
    using MultiTextureAnimationSVec_t = std::vector<MultiTextureAnimationSPtr_t>;

}

#endif //SFMLUTIL_ANIMATION_INCLUDED
