// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// character-stage-stat-box.cpp
//
#include "character-stage-stat-box.hpp"

#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-primitives.hpp"

namespace heroespath
{
namespace stage
{

    StatBox::StatBox(
        sfml_util::IStagePtr_t owningStagePtr,
        const float WIDTH,
        const float HEIGHT_RATIO,
        const sf::Color & TEXT_COLOR)
        : owningStagePtr_(owningStagePtr)
        , boxUPtr_()
        , fullRegions_()
        , labelRegions_()
        , numberRegions_()
        , textLabels_()
        , textValues_()
        , vertexArray_(sf::Lines)
        , heldDown_(StatInvalid())
        , baseSet_()
        , modSet_()
        , currentSet_()
        , modTextRegionVecs_()
        , willShowModValues_(true)
        , doAllStatsHaveValues_(false)
        , textScale_(0.0f)
    {
        const sf::Vector2f SIZE_V(WIDTH, HEIGHT_RATIO * WIDTH);
        const sf::FloatRect REGION_ORIG(sfml_util::CenterCopy(SIZE_V), SIZE_V);

        sfml_util::gui::BoxEntityInfo statsBoxInfo;
        statsBoxInfo.SetupImage(sfml_util::CachedTexture("media-images-backgrounds-tile-wood"));
        statsBoxInfo.SetupBorder(true);
        statsBoxInfo.focus_colors = sfml_util::defaults::GuiFocusColors;

        boxUPtr_ = std::make_unique<sfml_util::gui::BoxEntity>(
            "CharacterStageStatBox", REGION_ORIG, statsBoxInfo);

        owningStagePtr_->EntityAdd(boxUPtr_.get());

        const auto REGION_INNER { RegionInner() };
        const auto REGION_FULL_HEIGHT { REGION_INNER.height / static_cast<float>(StatCount()) };
        const auto REGION_NUMBER_WIDTH { REGION_INNER.width * 0.2f };
        const auto TEXT_SCALE_DOWN_RATIO { 0.9f };

        for (std::size_t i(0); i < StatCount(); ++i)
        {
            const auto TRAIT { StatEnum(i) };

            // the full row within the box
            const sf::FloatRect REGION_FULL(
                REGION_INNER.left + 1.0f,
                REGION_INNER.top + (static_cast<float>(i) * REGION_FULL_HEIGHT) + 1.0f,
                REGION_INNER.width - 2.0f,
                REGION_FULL_HEIGHT - 2.0f);

            fullRegions_.emplace_back(REGION_FULL);

            // left end of the row with margins where the stat label/name goes
            const sf::FloatRect REGION_LABEL(
                REGION_FULL.left + 1.0f,
                REGION_FULL.top + 1.0f,
                (REGION_FULL.width - REGION_NUMBER_WIDTH) - 2.0f,
                REGION_FULL.height - 2.0f);

            labelRegions_.emplace_back(REGION_LABEL);

            // right end of the row with margins where the actual stat number goes
            const sf::FloatRect REGION_NUMBER(
                sfml_util::Right(REGION_LABEL) + 1.0f,
                REGION_FULL.top + 1.0f,
                (sfml_util::Right(REGION_FULL) - 1.0f) - (sfml_util::Right(REGION_LABEL) + 1.0f),
                REGION_LABEL.height);

            numberRegions_.emplace_back(REGION_NUMBER);

            // the stat name/label text
            sf::Text labelText(
                StatName(TRAIT),
                *sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::SystemCondensed),
                sfml_util::FontManager::Instance()->Size_Larger());

            labelText.setColor(TEXT_COLOR);

            if ((textScale_ > 0.0f) == false)
            {
                labelText.setString(StatName(creature::Traits::Intelligence));

                textScale_ = (REGION_LABEL.width / labelText.getGlobalBounds().width)
                    * TEXT_SCALE_DOWN_RATIO;

                labelText.setString(StatName(TRAIT));
            }

            labelText.setScale(textScale_, textScale_);

            {
                const auto TEXT_HEIGHT { labelText.getGlobalBounds().height };
                const auto REGION_HEIGHT { REGION_LABEL.height };
                const auto REGION_CENTER_VERT { REGION_LABEL.top + (REGION_HEIGHT * 0.5f) };
                const auto TEXT_POS_TOP { REGION_CENTER_VERT - (TEXT_HEIGHT * 0.5f) };

                sfml_util::SetTextPosition(labelText, REGION_LABEL.left, TEXT_POS_TOP);

                M_HP_LOG_WRN(
                    "\n\torig=" << REGION_ORIG << "\n\touter=" << RegionOuter() << "\n\tinner="
                                << RegionInner() << "\n\n\t" << labelText.getString().toAnsiString()
                                << "\n\tTEXT_HEIGHT=" << TEXT_HEIGHT << "\n\tLABEL_REGION"
                                << REGION_LABEL << "\n\tREGION_CENTER_VERT=" << REGION_CENTER_VERT
                                << "\n\tTEXT_POS_TOP=" << TEXT_POS_TOP << "\n\tTEXT_REGION="
                                << labelText.getGlobalBounds() << "\n\ttext_scale=" << textScale_
                                << "\n\ttext_local_bounds=" << labelText.getLocalBounds()
                                << "\n\n");
            }

            textLabels_.emplace_back(labelText);

            // the stat value text
            sf::Text valueText(labelText);
            valueText.setString("");
            textValues_.emplace_back(valueText);

            // the horizontal line between the rows
            if (i < (StatCount() - 1))
            {
                const sf::Color LINE_COLOR { 255, 255, 255, 100 };

                const auto LINE_POS_LEFT { REGION_FULL.left };
                const auto LINE_POS_CENTER { sfml_util::CenterOfHoriz(REGION_FULL) };
                const auto LINE_POS_RIGHT { sfml_util::Right(REGION_FULL) };
                const auto LINE_POS_TOP { sfml_util::Bottom(REGION_FULL) };

                vertexArray_.append(
                    sf::Vertex(sf::Vector2f(LINE_POS_LEFT, LINE_POS_TOP), sf::Color::Transparent));

                vertexArray_.append(
                    sf::Vertex(sf::Vector2f(LINE_POS_CENTER, LINE_POS_TOP), LINE_COLOR));

                vertexArray_.append(
                    sf::Vertex(sf::Vector2f(LINE_POS_CENTER + 1.0f, LINE_POS_TOP), LINE_COLOR));

                vertexArray_.append(
                    sf::Vertex(sf::Vector2f(LINE_POS_RIGHT, LINE_POS_TOP), sf::Color::Transparent));
            }

            modTextRegionVecs_.emplace_back(sfml_util::gui::TextRegionUVec_t());
        }

        Reset();
    }

    StatBox::~StatBox() { owningStagePtr_->EntityRemove(boxUPtr_.get()); }

    void StatBox::Reset()
    {
        for (auto & sfText : textValues_)
        {
            sfText.setString("");
        }

        heldDown_ = StatInvalid();
        baseSet_ = creature::StatSet();
        modSet_ = creature::StatSet();
        currentSet_ = creature::StatSet();

        for (auto & textRegionUVec : modTextRegionVecs_)
        {
            textRegionUVec.clear();
        }

        willShowModValues_ = true;

        Sync();
    }

    void StatBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // don't draw the box because the stage will do that

        for (const auto & SF_TEXT : textLabels_)
        {
            target.draw(SF_TEXT, states);
        }

        for (const auto & SF_TEXT : textValues_)
        {
            target.draw(SF_TEXT, states);
        }

        target.draw(sfml_util::MakeRectangleHollow(RegionOuter(), sf::Color::Blue), states);

        for (const auto & REGION : fullRegions_)
        {
            target.draw(sfml_util::MakeRectangleHollow(REGION, sf::Color::Red), states);
        }

        for (const auto & REGION : labelRegions_)
        {
            target.draw(sfml_util::MakeRectangleHollow(REGION, sf::Color::Green), states);
        }

        for (const auto & REGION : numberRegions_)
        {
            target.draw(sfml_util::MakeRectangleHollow(REGION, sf::Color::Yellow), states);
        }

        for (const auto & SF_TEXT : textLabels_)
        {
            target.draw(
                sfml_util::MakeRectangleHollow(SF_TEXT.getGlobalBounds(), sf::Color::Cyan), states);
        }

        target.draw(vertexArray_, states);
    }

    void StatBox::Focus()
    {
        boxUPtr_->SetHasFocus(true);
        owningStagePtr_->SetFocus(boxUPtr_.get());
    }

    const sf::FloatRect StatBox::RegionOuter() const { return boxUPtr_->OuterRegion(); }
    const sf::FloatRect StatBox::RegionInner() const { return boxUPtr_->InnerRegion(); }

    void StatBox::SetVerticalPos(const float POS_TOP)
    {
        const auto VERT_DIFF { POS_TOP - RegionOuter().top };

        boxUPtr_->MoveEntityPos(0.0f, VERT_DIFF);

        for (auto & rect : fullRegions_)
        {
            rect.top += VERT_DIFF;
        }

        for (auto & rect : labelRegions_)
        {
            rect.top += VERT_DIFF;
        }

        for (auto & rect : numberRegions_)
        {
            rect.top += VERT_DIFF;
        }

        for (auto & sfText : textLabels_)
        {
            sfText.move(0.0f, VERT_DIFF);
        }

        for (auto & sfText : textValues_)
        {
            sfText.move(0.0f, VERT_DIFF);
        }

        for (std::size_t i(0); i < vertexArray_.getVertexCount(); ++i)
        {
            vertexArray_[i].position.y += VERT_DIFF;
        }
    }

    bool StatBox::IsValid(const creature::Traits::Enum TRAIT)
    {
        return (TRAIT < creature::Traits::StatCount);
    }

    bool StatBox::IsValid(const std::size_t INDEX) { return (INDEX < StatCount()); }

    std::size_t StatBox::Index(const creature::Traits::Enum TRAIT) const
    {
        if (IsValid(TRAIT))
        {
            return TRAIT;
        }
        else
        {
            return StatInvalid();
        }
    }

    creature::Traits::Enum StatBox::StatEnum(const std::size_t INDEX) const
    {
        if (IsValid(INDEX))
        {
            return static_cast<creature::Traits::Enum>(INDEX);
        }
        else
        {
            return StatInvalid();
        }
    }

    creature::Traits::Enum StatBox::Prevoius(const creature::Traits::Enum TRAIT) const
    {
        if (TRAIT == 0)
        {
            return StatInvalid();
        }
        else
        {
            return static_cast<creature::Traits::Enum>(
                static_cast<misc::EnumUnderlying_t>(TRAIT) - 1);
        }
    }

    creature::Traits::Enum StatBox::Next(const creature::Traits::Enum TRAIT) const
    {
        if (TRAIT < creature::Traits::Intelligence)
        {
            return static_cast<creature::Traits::Enum>(
                static_cast<misc::EnumUnderlying_t>(TRAIT) + 1);
        }
        else
        {
            return StatInvalid();
        }
    }

    void StatBox::AppendMod(
        const creature::Traits::Enum TRAIT,
        const creature::Trait_t VALUE,
        const std::string & MODIFIER_NAME)
    {
        if (!doAllStatsHaveValues_ || !IsValid(TRAIT))
        {
            return;
        }

        const auto INDEX { Index(TRAIT) };

        const auto TEXT_COLOR = [VALUE]() {
            const sf::Int8 COLOR_BASE(100);
            if (VALUE < 0)
            {
                return sf::Color(255, COLOR_BASE, COLOR_BASE);
            }
            else
            {
                return sf::Color(COLOR_BASE, 255, COLOR_BASE);
            }
        }();

        const auto TEXT = [&]() {
            std::ostringstream ss;
            ss << MODIFIER_NAME << ((VALUE > 0) ? "+" : "") << VALUE;
            return ss.str();
        }();

        const sfml_util::gui::TextInfo TEXT_INFO(
            TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::GuiFont::System),
            sfml_util::FontManager::Instance()->Size_Small(),
            TEXT_COLOR);

        modTextRegionVecs_.at(INDEX).emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
            "CharacterStageStatBoxModText_" + TEXT, TEXT_INFO));

        modSet_.Set(TRAIT, modSet_.Get(TRAIT) + VALUE);

        Sync();
    }

    void StatBox::ClearMods()
    {
        modSet_ = creature::StatSet();

        for (auto & textRegionUVec : modTextRegionVecs_)
        {
            textRegionUVec.clear();
        }

        Sync();
    }

    void StatBox::SwapBaseValue(const creature::Traits::Enum A, const creature::Traits::Enum B)
    {
        if (doAllStatsHaveValues_ && IsValid(A) && IsValid(B))
        {
            const auto A_ORIG { baseSet_.Get(A) };
            baseSet_.Set(A, baseSet_.Get(A));
            baseSet_.Set(B, A_ORIG);

            Sync();
        }
    }

    void StatBox::SetBaseValue(const creature::Traits::Enum TRAIT, const creature::Trait_t VALUE)
    {
        if (TRAIT < creature::Traits::StatCount)
        {
            baseSet_.Set(TRAIT, VALUE);
            textValues_.at(Index(TRAIT)).setString(" "); // anything other than empty works here
            Sync();
        }
    }

    const std::string StatBox::StatName(const creature::Traits::Enum TRAIT) const
    {
        if (IsValid(TRAIT))
        {
            return creature::Traits::ToString(TRAIT);
        }
        else
        {
            return "";
        }
    }

    float StatBox::NextModTextPositionLeft(const creature::Traits::Enum TRAIT) const
    {
        if (!IsValid(TRAIT))
        {
            return -1.0f;
        }

        const auto INDEX { Index(TRAIT) };
        float posLeft { 0.0f };

        for (const auto & TEXT_REGION_UPTR : modTextRegionVecs_.at(INDEX))
        {
            const auto POS_RIGHT { sfml_util::Right(TEXT_REGION_UPTR->GetEntityRegion()) };
            if (POS_RIGHT > posLeft)
            {
                posLeft = POS_RIGHT;
            }
        }

        const auto BETWEEN_MOD_TEXT_SPACER { sfml_util::ScreenRatioToPixelsHoriz(0.005f) };

        if (posLeft > 0.0f)
        {
            return posLeft + BETWEEN_MOD_TEXT_SPACER;
        }
        else
        {
            const auto BETWEEN_BOX_AND_MOD_TEXT_SPACER { BETWEEN_MOD_TEXT_SPACER * 2.0f };
            return sfml_util::Right(RegionOuter()) + BETWEEN_BOX_AND_MOD_TEXT_SPACER;
        }
    }

    void StatBox::Sync()
    {
        doAllStatsHaveValues_ = true;

        for (std::size_t i(0); i < StatCount(); ++i)
        {
            const auto TRAIT { StatEnum(i) };

            currentSet_.Set(TRAIT, baseSet_.Get(TRAIT) + modSet_.Get(TRAIT));

            auto & sfTextValue { textValues_.at(i) };

            if (sfTextValue.getString().isEmpty())
            {
                doAllStatsHaveValues_ = false;
            }
            else
            {
                std::ostringstream ss;

                if (willShowModValues_)
                {
                    ss << currentSet_.Get(TRAIT);
                }
                else
                {
                    ss << baseSet_.Get(TRAIT);
                }

                sfTextValue.setString(ss.str());

                sfTextValue.setPosition(sfml_util::CenterToCopy(
                    sfml_util::Size(sfTextValue.getGlobalBounds()), numberRegions_.at(i)));
            }
        }
    }

} // namespace stage
} // namespace heroespath
