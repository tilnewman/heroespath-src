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
    {
        SyncRegion();
    }

    RenderedText::RenderedText(const SfTextVecVec_t & SF_TEXT_VEC_VEC)
        : text_vecs(SF_TEXT_VEC_VEC)
        , regions()
        , region(0.0f, 0.0f, 0.0f, 0.0f)
    {
        SyncRegion();
    }

    void RenderedText::Clear()
    {
        text_vecs.clear();
        SyncRegion();
    }

    void RenderedText::SyncRegion()
    {
        regions.clear();

        for (const auto & SF_TEXT_VEC : text_vecs)
        {
            regions.emplace_back(MinimallyEnclosing(SF_TEXT_VEC));
        }

        region = sfml_util::MinimallyEnclosing(regions);
    }

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
        }

        SyncRegion();
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
        for (auto & sfTextVec : text_vecs)
        {
            for (auto & sfText : sfTextVec)
            {
                sfText.move(HORIZ, VERT);
            }
        }

        SyncRegion();
    }

    void RenderedText::CreateTextureAndRenderOffscreen(sf::RenderTexture & renderTexture) const
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

} // namespace sfml_util
} // namespace heroespath
