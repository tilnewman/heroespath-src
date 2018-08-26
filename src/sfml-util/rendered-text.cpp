// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// rendered-text.cpp
//
#include "rendered-text.hpp"

#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

namespace heroespath
{
namespace sfml_util
{

    RenderedText::RenderedText()
        : text_vecs()
        , regions()
        , region(0.0f, 0.0f, 0.0f, 0.0f)
    {}

    RenderedText::RenderedText(const SfTextVecVec_t & SF_TEXT_VEC_VEC)
        : text_vecs(SF_TEXT_VEC_VEC)
        , regions()
        , region(0.0f, 0.0f, 0.0f, 0.0f)
    {
        ResetRegions();
    }

    void RenderedText::Clear()
    {
        text_vecs.clear();
        ResetRegions();
    }

    void RenderedText::ResetRegions()
    {
        regions.clear();

        for (const auto & SF_TEXT_VEC : text_vecs)
        {
            regions.emplace_back(MinimallyEnclosing(SF_TEXT_VEC, true));
        }

        ResetRegion();
    }

    void RenderedText::ResetRegion() { region = sfml_util::MinimallyEnclosing(regions, true); }

    void RenderedText::AppendLines(const RenderedText & RENDERED_TEXT_TO_APPEND)
    {
        const auto POS_DIFF_HORIZ { region.left - RENDERED_TEXT_TO_APPEND.region.left };
        const auto POS_DIFF_VERT { (Bottom(region) + 1.0f) - RENDERED_TEXT_TO_APPEND.region.top };

        for (const auto & SF_TEXT_VEC_LINE : RENDERED_TEXT_TO_APPEND.text_vecs)
        {
            text_vecs.emplace_back(SF_TEXT_VEC_LINE);

            for (auto & sfText : text_vecs.back())
            {
                sfText.move(POS_DIFF_HORIZ, POS_DIFF_VERT);
            }

            const auto LINE_REGION { MinimallyEnclosing(SF_TEXT_VEC_LINE, true) };
            region = sfml_util::MinimallyEnclosing(region, LINE_REGION, true);
        }
    }

    void RenderedText::SetColor(const sf::Color & NEW_COLOR)
    {
        for (auto & sfTextVec : text_vecs)
        {
            for (auto & sfText : sfTextVec)
            {
                sfText.setFillColor(NEW_COLOR);
            }
        }
    }

    void RenderedText::SetPos(const float LEFT, const float TOP)
    {
        const auto POS_DIFF_HORIZ { LEFT - region.left };
        const auto POS_DIFF_VERT { TOP - region.top };
        MovePos(POS_DIFF_HORIZ, POS_DIFF_VERT);
    }

    void RenderedText::MovePos(const float HORIZ, const float VERT)
    {
        if ((IsSizeZeroOrLessEither(region) == false) && (text_vecs.empty() == false))
        {
            for (auto & sfTextVec : text_vecs)
            {
                for (auto & sfText : sfTextVec)
                {
                    sfText.move(HORIZ, VERT);
                }
            }

            for (auto & lineRegion : regions)
            {
                lineRegion.left += HORIZ;
                lineRegion.top += VERT;
            }

            region.left += HORIZ;
            region.top += VERT;
        }
    }

    void RenderedText::CreateTextureAndRenderOffscreen(sf::RenderTexture & renderTexture) const
    {
        if (!(region.width < 1.0f) && !(region.height < 1.0f) && (text_vecs.empty() == false))
        {
            renderTexture.create(
                static_cast<unsigned>(region.width), static_cast<unsigned>(region.height));

            renderTexture.setSmooth(true);
            renderTexture.clear(sf::Color::Transparent);

            sf::RenderStates states;
            states.blendMode = sf::BlendNone;

            for (const auto & SF_TEXT_VEC_LINE : text_vecs)
            {
                for (const auto & SF_TEXT : SF_TEXT_VEC_LINE)
                {
                    renderTexture.draw(SF_TEXT, states);
                }
            }

            renderTexture.display();
        }
    }

    void RenderedText::Justify(
        const sfml_util::Justified::Enum JUSTIFICATION, const float WIDTH_LIMIT)
    {
        const auto TOTAL_WIDTH { region.width };
        const auto LINE_VEC_COUNT { text_vecs.size() };

        bool wasTextMoved { false };
        for (std::size_t i(0); i < LINE_VEC_COUNT; ++i)
        {
            auto & lineVec { text_vecs[i] };
            auto & lineVecRegion { regions[i] };
            const auto LINE_WIDTH { lineVecRegion.width };

            const auto OFFSET = [&]() {
                if (JUSTIFICATION == Justified::Center)
                {
                    if (WIDTH_LIMIT > 0.0f)
                    {
                        return ((WIDTH_LIMIT * 0.5f) - (LINE_WIDTH * 0.5f));
                    }
                    else
                    {
                        return ((TOTAL_WIDTH * 0.5f) - (LINE_WIDTH * 0.5f));
                    }
                }
                else if (JUSTIFICATION == Justified::Right)
                {
                    if (WIDTH_LIMIT > 0.0f)
                    {
                        return (WIDTH_LIMIT - LINE_WIDTH);
                    }
                    else
                    {
                        return (TOTAL_WIDTH - LINE_WIDTH);
                    }
                }
                else
                {
                    return 0.0f;
                }
            }();

            if (OFFSET > 0.0f)
            {
                wasTextMoved = true;
                for (auto & sfText : lineVec)
                {
                    sfText.move(OFFSET, 0.0f);
                }

                lineVecRegion.left += OFFSET;
            }
        }

        if (wasTextMoved)
        {
            region = sfml_util::MinimallyEnclosing(regions, true);
        }
    }

} // namespace sfml_util
} // namespace heroespath
