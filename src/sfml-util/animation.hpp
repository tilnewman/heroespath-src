///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
        SingleTextureAnimation(const std::string &   ENTITY_NAME,
                               const std::string &   TEXTURE_FILE_PATH,
                               const float           SCREEN_POS_LEFT,
                               const float           SCREEN_POS_TOP,
                               const unsigned int    FRAME_WIDTH,
                               const unsigned int    FRAME_HEIGHT,
                               const float           TIME_BETWEEN_FRAMES,

                               //zero here means each texture region big enough for a frame
                               //contains a frame, and frameCount_ will be set in the
                               //constructor
                               const std::size_t     FRAME_COUNT          = 0,
                               
                               const sf::BlendMode & BLEND_MODE           = sf::BlendAlpha,
                               const float           SCALE_HORIZ          = 1.0f,
                               const float           SCALE_VERT           = 1.0f,
                               const sf::Color &     COLOR                = sf::Color::White,
                               const unsigned int    FIRST_FRAME_POS_LEFT = 0,
                               const unsigned int    FIRST_FRAME_POS_TOP  = 0,
                               const unsigned int    FRAME_HORIZ_GAP      = 0,
                               const unsigned int    FRAME_VERT_GAP       = 0);

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

        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        inline float TimeBetweenFrames() const              { return timeBetweenFrames_; }
        inline void TimeBetweenFramesSet(const float TBF)   { timeBetweenFrames_ = TBF; }
        inline void TimeBetweenFramesAdj(const float ADJ)   { timeBetweenFrames_ += ADJ; }

        inline void ColorTransition(const sf::Color & FROM,
                                    const sf::Color & TO)
        {
            colorFrom_ = FROM;
            colorTo_ = TO;
        }

        inline void SetTargetSize(const sf::Vector2f & SIZE_V)
        {
            entityRegion_ = sf::FloatRect(entityRegion_.left,
                                          entityRegion_.top,
                                          SIZE_V.x,
                                          SIZE_V.y);
        }

        inline bool IsFinished() const { return isFinished_; }
    
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
        sf::Texture              texture_;
        std::vector<sf::IntRect> rects_;//the size of this vector acts as a total frame count.
        std::size_t              currentFrame_;//counts up to total then restarts at zero
        float                    frameTimerSec_;
        sf::Color                colorFrom_;
        sf::Color                colorTo_;
        float                    origSizeHoriz_;
        float                    origSizeVert_;
        bool                     isFinished_;
    };

    using SingleTextureAnimationUPtr_t = std::unique_ptr<SingleTextureAnimation>;
    using SingleTextureAnimationUVec_t = std::vector<SingleTextureAnimationUPtr_t>;
    using SingleTextureAnimationSPtr_t = std::shared_ptr<SingleTextureAnimation>;
    using SingleTextureAnimationSVec_t = std::vector<SingleTextureAnimationSPtr_t>;


    //Encapsulates an animatin on screen that loads frames from multiple textures.
    class MultiTextureAnimation : public gui::GuiEntity
    {
        //prevent copy construction and copy assignment
        MultiTextureAnimation(const MultiTextureAnimation &) =delete;
        MultiTextureAnimation & operator=(const MultiTextureAnimation &) =delete;

    public:
        MultiTextureAnimation(const std::string &   ENTITY_NAME,
                              const std::string &   TEXTURES_DIRECTORY,
                              const float           SCREEN_POS_LEFT,
                              const float           SCREEN_POS_TOP,
                              const float           TIME_BETWEEN_FRAMES,
                              const float           SCALE_HORIZ = 1.0f,
                              const float           SCALE_VERT  = 1.0f,
                              const sf::Color &     COLOR       = sf::Color::White,
                              const sf::BlendMode & BLEND_MODE  = sf::BlendAlpha);

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
        inline std::size_t FrameCount() const               { return textureVec_.size(); }

        virtual void MoveEntityPos(const float HORIZ, const float VERT);
        
        inline float TimeBetweenFrames() const              { return timeBetweenFrames_; }
        inline void TimeBetweenFramesSet(const float TBF)   { timeBetweenFrames_ = TBF; }
        inline void TimeBetweenFramesAdj(const float ADJ)   { timeBetweenFrames_ += ADJ; }

        inline void ColorTransition(const sf::Color & FROM,
                                    const sf::Color & TO)
        {
            colorFrom_ = FROM;
            colorTo_ = TO;
        }

        inline void SetTargetSize(const sf::Vector2f & SIZE_V)
        {
            entityRegion_ = sf::FloatRect(entityRegion_.left,
                                          entityRegion_.top,
                                          SIZE_V.x,
                                          SIZE_V.y);
        }

        inline bool IsFinished() const { return isFinished_; }

    protected:
        const sf::BlendMode BLEND_MODE_;
        float               timeBetweenFrames_;
        sf::Sprite          sprite_;
        TextureVec_t        textureVec_;//the size of this vec acts as a total frame count
        std::size_t         currentFrame_;//counts up to total then restarts at zero
        float               frameTimerSec_;
        sf::Color           colorFrom_;
        sf::Color           colorTo_;
        float               origSizeHoriz_;
        float               origSizeVert_;
        bool                isFinished_;
    };

    using MultiTextureAnimationUPtr_t = std::unique_ptr<MultiTextureAnimation>;
    using MultiTextureAnimationUVec_t = std::vector<MultiTextureAnimationUPtr_t>;
    using MultiTextureAnimationSPtr_t = std::shared_ptr<MultiTextureAnimation>;
    using MultiTextureAnimationSVec_t = std::vector<MultiTextureAnimationSPtr_t>;

}

#endif //SFMLUTIL_ANIMATION_INCLUDED
