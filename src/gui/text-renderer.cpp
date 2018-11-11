// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-renderer.cpp
//
#include "text-renderer.hpp"

#include "gui/text-render-cleaning.hpp"
#include "gui/text-render-context.hpp"
#include "gui/text-render-rendered-lines.hpp"
#include "gui/text-render-rendering.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{
    bool TextRenderer::ToTexture(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & BOUNDING_REGION,
        RenderTextureUPtr_t & renderTextureUPtr,
        sf::FloatRect & finalRegion)
    {
        auto logErrorMsgAndReturnErrorResult = [&](const std::string & ERROR_MSG) {
            std::ostringstream ss;

            ss << "gui::TextRenderer::ToTexture(" << TEXT_INFO << ", region=" << BOUNDING_REGION
               << ")  " << ERROR_MSG;

            M_HP_LOG_ERR(ss.str());
            return false;
        };

        if (!TEXT_INFO.IsValid())
        {
            return logErrorMsgAndReturnErrorResult(
                "No text was rendered because the TEXT_INFO object was invalid.");
        }

        const auto TEXT_CLEANER { text_rendering::TextCleaner(TEXT_INFO) };

        if (TEXT_CLEANER.text.empty())
        {
            return logErrorMsgAndReturnErrorResult("No text was rendered because the text was "
                                                   "empty after removing invalid characters.");
        }

        if (TEXT_INFO.text != TEXT_CLEANER.text)
        {
            M_HP_LOG_WRN(
                "gui::text_render::Render("
                << TEXT_INFO << ", region=" << BOUNDING_REGION
                << ")  Original text was changed during engine::CleanText():\nold=\""
                << TEXT_INFO.text << "\"\nnew=\"" << TEXT_CLEANER.text << "\"");
        }

        const text_rendering::Context CONTEXT(TEXT_INFO, TEXT_CLEANER, BOUNDING_REGION.width);

        const gui::text_rendering::RenderedLines RENDERED_LINES { text_rendering::Renderer::Make(
            CONTEXT) };

        if (RENDERED_LINES.IsValid())
        {
            if (!renderTextureUPtr)
            {
                renderTextureUPtr = std::make_unique<sf::RenderTexture>();
            }

            Draw(RENDERED_LINES, *renderTextureUPtr);
            finalRegion = RENDERED_LINES.CalcScreenRegion(sfutil::Position(BOUNDING_REGION));
            return true;
        }
        else
        {
            std::ostringstream ss;
            ss << "No text was rendered by RenderLines().  render=" << RENDERED_LINES.ToString();
            return logErrorMsgAndReturnErrorResult(ss.str());
        }
    }

    bool TextRenderer::ToSprite(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & BOUNDING_REGION,
        RenderTextureUPtr_t & renderTextureUPtr,
        sf::Sprite & sprite)
    {
        sf::FloatRect finalRegion;
        const auto RENDER_RESULT { ToTexture(
            TEXT_INFO, BOUNDING_REGION, renderTextureUPtr, finalRegion) };

        if (RENDER_RESULT && renderTextureUPtr)
        {
            sprite.setTexture(renderTextureUPtr->getTexture());

            sprite.setTextureRect(
                sf::IntRect(sf::Vector2i(), sf::Vector2i(sfutil::Size(finalRegion))));

            sprite.setPosition(sfutil::Position(finalRegion));
            sprite.setColor(TEXT_INFO.color);
            return true;
        }
        else
        {
            return false;
        }
    }

    void TextRenderer::Draw(
        const text_rendering::RenderedLines & RENDERED_LINES, sf::RenderTexture & renderTexture)
    {
        const sf::Vector2u TEXT_SIZE { sfutil::Size(RENDERED_LINES.Region()) };

        if ((renderTexture.getSize().x < TEXT_SIZE.x) && (renderTexture.getSize().y < TEXT_SIZE.y))
        {
            const auto ORIG_SIZE { renderTexture.getSize() };
            const auto CREATE_TEXTURE_RESULT { renderTexture.create(TEXT_SIZE.x, TEXT_SIZE.y) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (CREATE_TEXTURE_RESULT),
                "gui::TextRenderer::Draw(rendered_text="
                    << RENDERED_LINES.ToString() << ", render_texture_size_orig=" << ORIG_SIZE
                    << ")  The sf::RenderTexture give was not big enough, but attempting to "
                       "make "
                       "one big enough by calling sf::RenderTexture::Create("
                    << TEXT_SIZE << ") failed and returned false.");
        }

        renderTexture.setSmooth(true);
        renderTexture.clear(sf::Color::Transparent);

        // if you draw text to a transparent texture using BlendAlpha the result will be fuzzy
        // so use BlendNone and it will look good
        sf::RenderStates states;
        states.blendMode = sf::BlendNone;

        renderTexture.draw(RENDERED_LINES, states);
        renderTexture.display();
    }

} // namespace gui
} // namespace heroespath
