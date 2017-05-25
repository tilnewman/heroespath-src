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
#ifndef SFMLUTIL_TEXTRENDERING_INCLUDED
#define SFMLUTIL_TEXTRENDERING_INCLUDED
//
// text-rendering.hpp
//  Code that draws text to an off-screen texture one character at a time.
//
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/margins.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    namespace gui
    {
        class SliderBar;
        using SliderBarSPtr_t = std::shared_ptr<SliderBar>;
    }

namespace text_render
{

    struct Constants
    {
        static const float SIDEBAR_HORIZ_OFFSET_;
    };


    //used by TextRender internally
    struct TextSnippet
    {
        TextSnippet()
        :
            sf_text  (),
            text_info()
        {}

        TextSnippet(const float           POS_LEFT,
                    const float           POS_TOP,
                    const std::string &   TEXT,
                    const sf::Text &      TEXT_OBJ,
                    const gui::TextInfo & TEXT_INFO)
        :
            sf_text  (TEXT_OBJ),
            text_info(TEXT_INFO)
        {
            Setup(POS_LEFT, POS_TOP, TEXT, TEXT_INFO);
        }

        void Setup( const float           POS_LEFT,
                    const float           POS_TOP,
                    const std::string &   TEXT,
                    const gui::TextInfo & TEXT_INFO)
        {
            gui::SetupText(sf_text, TEXT_INFO);
            sf_text.setString(TEXT);
            sf_text.setPosition(POS_LEFT, POS_TOP);
            text_info.text = TEXT;
        }

        void AddCharacter(const char C)
        {
            text_info.text += C;
            sf_text.setString(text_info.text);
        }

        sf::Text sf_text;//use to store position and text string as well
        gui::TextInfo text_info;
    };

    using TextSnippetVec_t = std::vector<TextSnippet>;
    using TextSnippetVecVec_t = std::vector<TextSnippetVec_t>;


    //All the information generated by rendering text,
    //ready to be drawn on screen or to an off-screen texture
    class RenderedText
    {
    public:
        RenderedText()
        :
            longest_line(0.0f),
            total_height(0.0f),
            vec_vec     ()
        {}

        RenderedText(const float                 LONGEST_LINE,
                     const float                 TOTAL_HEIGHT,
                     const TextSnippetVecVec_t & VEC_VEC)
        :
            longest_line(LONGEST_LINE),
            total_height(TOTAL_HEIGHT),
            vec_vec     (VEC_VEC)
        {}


        void Reset()
        {
            longest_line = 0.0f;
            total_height = 0.0f;
            vec_vec.clear();
        }


        float longest_line;
        float total_height;
        TextSnippetVecVec_t vec_vec;
    };

    using RenderedTextSPtr_t = std::shared_ptr<RenderedText>;


    //struct used internally by TextRender
    struct TextInfoSet
    {
        TextInfoSet(const std::string &   TEXT,
                    const gui::TextInfo & TEXT_INFO_NUM,
                    const gui::TextInfo & TEXT_INFO_CHAR)
        :
            text   (TEXT),
            ti_num (TEXT_INFO_NUM),
            ti_char(TEXT_INFO_CHAR)
        {}

        std::string text;
        gui::TextInfo ti_num;
        gui::TextInfo ti_char;
    };


    //Note:  sfml fonts drawn to a sf::RenderTexture must be set to BlendNone to look/work correctly,
    //       so keep two copies of the Draw() function, one for each render target type.
    //
    //Another Note:  Okay so I tried this and the text still looks wrong so screw it,
    //               and don't expect off-screen drawn text to look right.
    //
    //Final Note:  So...after beating my head against this problem I stumbled upon the
    //             solution:  THE TEXT DRAWN FROM A RENDERTEXTURE WILL LOOK WRONG IF
    //             THAT RENDERTEXTURE's DIMMENTIONS ARE NOT ALIGNED TO POWER OF 2.
    //             So simply make sure that when calling sf::RenderTexture::Create()
    //             that the sizes you give are power of two, which you should be doing
    //             anyway, and the fonts will look correct.
    //
    //Target_t must be either sf::RenderWindow or sf::RenderTexture
    template<typename Target_t>
    void Draw(RenderedText &   renderedText,
              Target_t &       target,
              sf::RenderStates states,
              const float      MOVE_VERT = 0.0f)
    {
        const bool WILL_MOVE(false == utilz::IsRealClose(MOVE_VERT, 0.0f));

        const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
        states.blendMode = sf::BlendNone;

        const std::size_t NUM_LINES(renderedText.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            const std::size_t NUM_SNIPPETS(renderedText.vec_vec[l].size());
            for (std::size_t c(0); c < NUM_SNIPPETS; ++c)
            {
                if (WILL_MOVE)
                    renderedText.vec_vec[l][c].sf_text.move(0.0f, MOVE_VERT);

                target.draw(renderedText.vec_vec[l][c].sf_text, states);
            }
        }

        states.blendMode = ORIG_BLEND_MODE;
    }

    void RenderToArea(const std::string &    NAME,
                      const gui::TextInfo &  TEXT_INFO,
                      gui::SliderBarSPtr_t & sliderBarSPtr,
                      RenderedText &         renderedText,
                      const sf::FloatRect &  REGION,
                      const unsigned int     SMALLER_FONT_SIZE    = 0,//zero means don't resize the font
                      const Margins &        MARGINS              = Margins(),
                      const bool             WILL_ALLOW_SCROLLBAR = true);

    void ApplyToArea(const std::string &    NAME,
                     gui::SliderBarSPtr_t & sliderBarSPtr,
                     const RenderedText &   RENDERED_TEXT,
                     const sf::FloatRect &  REGION);

    RendTextSPtr_t RenderAndDraw(RenderedText & renderedText);

    void Render(RenderedText &        renderText,
                const gui::TextInfo & TEXT_INFO,
                const float           WIDTH_LIMIT,
                const Margins &       MARGINS = Margins());

    void Render(RenderedText &        renderText,
                const gui::TextInfo & TEXT_INFO,
                const float           POS_LEFT,
                const float           POS_TOP,
                const Margins &       MARGINS = Margins());

    void Render(RenderedText &        renderText,
                const gui::TextInfo & TEXT_INFO,
                const sf::FloatRect & REGION,
                const Margins &       MARGINS = Margins());

    void Render(RenderedText &        renderText,
                const gui::TextInfo & TEXT_INFO_CHAR,
                const gui::TextInfo & TEXT_INFO_NUM,
                const std::string &   TEXT,
                const sf::FloatRect & REGION,
                const Margins &       MARGINS = Margins());

    TextSnippetVec_t RenderLine(const TextInfoSet & TEXT_INFO_SET,
                                sf::Vector2f &      textPos,
                                const float         WIDTH_LIMIT,
                                float &             heightTracker,
                                std::size_t &            strIndex);

    TextSnippetVec_t RenderWord(const TextInfoSet & TEXT_INFO_SET,
                                sf::Vector2f &      textPos,
                                float &             heightTracker,
                                std::size_t &            strIndex,
                                char &              termChar);

    void GroupTextSnippets(TextSnippetVec_t &       resultVec,
                           const TextSnippetVec_t & inputVec);

}
}

#endif //SFMLUTIL_TEXTRENDERING_INCLUDED
