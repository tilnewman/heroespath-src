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
#include "sfutil/scale.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{
    const sf::FloatRect TextRenderer::ToTexture(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & BOUNDING_REGION,
        RenderTextureUPtr_t & renderTextureUPtr)
    {
        auto handleandReturnError = [&]() {
            renderTextureUPtr.reset();
            return sf::FloatRect();
        };

        if (!TEXT_INFO.WillDraw())
        {
            M_HP_LOG_ERR(
                "Render skipped because TEXT_INFO.WillDraw()==false." + M_HP_VAR_STR(TEXT_INFO)
                + M_HP_VAR_STR(BOUNDING_REGION));

            return handleandReturnError();
        }

        const auto TEXT_CLEANER { text_rendering::TextCleaner(TEXT_INFO) };

        if (TEXT_CLEANER.text.empty())
        {
            M_HP_LOG_ERR(
                "Render skipped because there was no text after cleaning." + M_HP_VAR_STR(TEXT_INFO)
                + M_HP_VAR_STR(BOUNDING_REGION));

            return handleandReturnError();
        }

        if (TEXT_INFO.text != TEXT_CLEANER.text)
        {
            M_HP_LOG_WRN(
                "Original text was changed during text_rendering::TextCleaner(): old=\""
                + TEXT_INFO.text + "\", new=\"" + TEXT_CLEANER.text + "\"" + M_HP_VAR_STR(TEXT_INFO)
                + M_HP_VAR_STR(BOUNDING_REGION));
        }

        const text_rendering::Context CONTEXT(TEXT_INFO, TEXT_CLEANER, BOUNDING_REGION.width);

        const gui::text_rendering::RenderedLines RENDERED_LINES { text_rendering::Renderer::Make(
            CONTEXT) };

        if (RENDERED_LINES.IsValid() == false)
        {
            M_HP_LOG_ERR(
                "No text was rendered by text_rendering::Renderer::Make()."
                + M_HP_VAR_STR(RENDERED_LINES.ToString()) + M_HP_VAR_STR(TEXT_INFO)
                + M_HP_VAR_STR(BOUNDING_REGION));
        }

        if (DrawRenderedLinesOffScreen(RENDERED_LINES, renderTextureUPtr) == false)
        {
            return handleandReturnError();
        }

        return RENDERED_LINES.CalcScreenRegion(sfutil::Position(BOUNDING_REGION));
    }

    bool TextRenderer::ToSprite(
        const TextInfo & TEXT_INFO,
        const sf::FloatRect & BOUNDING_REGION,
        RenderTextureUPtr_t & renderTextureUPtr,
        sf::Sprite & sprite)
    {
        sprite = sf::Sprite();

        const auto FINAL_REGION { ToTexture(TEXT_INFO, BOUNDING_REGION, renderTextureUPtr) };

        if ((FINAL_REGION == sf::FloatRect()) || !renderTextureUPtr)
        {
            return false;
        }

        sprite.setTexture(renderTextureUPtr->getTexture());

        sprite.setTextureRect(
            sf::IntRect(sf::Vector2i(), sf::Vector2i(sfutil::Size(FINAL_REGION))));

        sprite.setPosition(sfutil::Position(FINAL_REGION));
        sprite.setColor(TEXT_INFO.color);
        return true;
    }

    bool TextRenderer::DrawRenderedLinesOffScreen(
        const text_rendering::RenderedLines & RENDERED_LINES,
        RenderTextureUPtr_t & renderTextureUPtr)
    {
        if (!renderTextureUPtr)
        {
            renderTextureUPtr = std::make_unique<sf::RenderTexture>();
        }

        const sf::Vector2u TEXT_SIZE_V { sfutil::Size(RENDERED_LINES.Region()) };
        const sf::Vector2u ORIG_TEXTURE_SIZE_V { renderTextureUPtr->getSize() };

        if ((ORIG_TEXTURE_SIZE_V.x < TEXT_SIZE_V.x) || (ORIG_TEXTURE_SIZE_V.y < TEXT_SIZE_V.y))
        {
            const auto CREATE_TEXTURE_RESULT { renderTextureUPtr->create(
                TEXT_SIZE_V.x, TEXT_SIZE_V.y) };

            if (CREATE_TEXTURE_RESULT == false)
            {
                M_HP_LOG_ERR(
                    "The sf::RenderTexture given was not big enough, but attempting to "
                    "sf::RenderTexture::Create(TEXT_SIZE_V="
                    << TEXT_SIZE_V
                    << ") one big enough failed (returned false).  Check the console output for "
                       "details on why."
                    << M_HP_VAR_STR(ORIG_TEXTURE_SIZE_V)
                    << M_HP_VAR_STR(RENDERED_LINES.ToString()));

                renderTextureUPtr.reset();
                return false;
            }
        }

        renderTextureUPtr->setSmooth(true);
        renderTextureUPtr->clear(sf::Color::Transparent);
        renderTextureUPtr->display();

        // If you draw text to a transparent texture using BlendAlpha the result will be ugly/fuzzy
        // so use BlendNone to avoid this problem.
        sf::RenderStates states;
        states.blendMode = sf::BlendNone;

        renderTextureUPtr->draw(RENDERED_LINES, states);
        renderTextureUPtr->display();
        return true;
    }

} // namespace gui
} // namespace heroespath
