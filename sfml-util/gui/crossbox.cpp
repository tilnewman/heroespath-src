// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// crossbox.cpp
//
#include "crossbox.hpp"

#include "utilz/real.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "utilz/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{
namespace box
{

    float GetMinWidth_CrossBox(const bool WILL_GROW_TO_FIT)
    {
        auto const GE_PTR{ GuiElements::Instance() };

        float width(static_cast<float>(GE_PTR->GetRect_LineSmallCornerTopLeft().width * 2) + static_cast<float>(GE_PTR->GetRect_LineSmallCross().width));

        if (WILL_GROW_TO_FIT)
            width += GE_PTR->GetLineSmallBoxPadLeft() + GE_PTR->GetLineSmallBoxPadRight();

        return width;
    }


    float GetMinHeight_CrossBox(const bool WILL_GROW_TO_FIT)
    {
        auto const GE_PTR{ GuiElements::Instance() };

        float height(static_cast<float>(GE_PTR->GetRect_LineSmallCornerTopLeft().height * 2) + static_cast<float>(GE_PTR->GetRect_LineSmallCross().height));

        if (WILL_GROW_TO_FIT)
            height += (GE_PTR->GetLineSmallBoxPadTop() + GE_PTR->GetLineSmallBoxPadBot());

        return height;
    }


    CrossBox::CrossBox( const std::string &    NAME,
                        const float            SCREEN_POS_LEFT,
                        const float            SCREEN_POS_TOP,
                        const float            INTENDED_FIRST_WIDTH,
                        const float            INTENDED_FIRST_HEIGHT,
                        const float            INTENDED_SECOND_WIDTH,
                        const float            INTENDED_SECOND_HEIGHT,
                        const bool             WILL_GROW_TO_FIT,
                        const BackgroundInfo & BACKGROUND_INFO)
    :
        Box               (NAME),
        topLine2_         (std::string(NAME).append("CrossBox's Second Top Line"),    Orientation::Horiz, Side::Top,    false),
        rightLine2_       (std::string(NAME).append("CrossBox's Second Right Line"),  Orientation::Vert,  Side::Right,  false),
        botLine2_         (std::string(NAME).append("CrossBox's Second Bottom Line"), Orientation::Horiz, Side::Bottom, false),
        leftLine2_        (std::string(NAME).append("CrossBox's Second Left Line"),   Orientation::Vert,  Side::Left,   false),
        centerLeftLine_   (std::string(NAME).append("CrossBox's Center Left Line"),   Orientation::Horiz, Side::Bottom, false),
        centerRightLine_  (std::string(NAME).append("CrossBox's Center Right Line"),  Orientation::Horiz, Side::Bottom, false),
        centerTopLine_    (std::string(NAME).append("CrossBox's Center Top Line"),    Orientation::Vert,  Side::Right,  false),
        centerBotLine_    (std::string(NAME).append("CrossBox's Center Bottom Line"), Orientation::Vert,  Side::Right,  false),
        crossSprite_      ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallCross()),
        tJointLeftSprite_ ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallTLeft()),
        tJointRightSprite_( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallTRight()),
        tJointTopSprite_  ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallTTop()),
        tJointBotSprite_  ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallTBot())
    {
        SetupCrossBox(  SCREEN_POS_LEFT,
                        SCREEN_POS_TOP,
                        INTENDED_FIRST_WIDTH,
                        INTENDED_FIRST_HEIGHT,
                        INTENDED_SECOND_WIDTH,
                        INTENDED_SECOND_HEIGHT,
                        WILL_GROW_TO_FIT,
                        BACKGROUND_INFO );
    }


    CrossBox::~CrossBox()
    {}


    void CrossBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(backgroundImage_, states);

        target.draw(cTopLeftSprite_, states);
        topLine_.draw(target, states);
        target.draw(tJointTopSprite_, states);
        topLine2_.draw(target, states);

        target.draw(cTopRightSprite_, states);
        rightLine_.draw(target, states);
        target.draw(tJointRightSprite_, states);
        rightLine2_.draw(target, states);

        target.draw(cBotRightSprite_, states);
        botLine_.draw(target, states);
        target.draw(tJointBotSprite_, states);
        botLine2_.draw(target, states);

        target.draw(cBotLeftSprite_, states);
        leftLine_.draw(target, states);
        target.draw(tJointLeftSprite_, states);
        leftLine2_.draw(target, states);

        //using crossSprite_ as the center cross sprite
        target.draw(crossSprite_, states);

        target.draw(centerLeftLine_,  states);
        target.draw(centerRightLine_, states);
        target.draw(centerTopLine_,   states);
        target.draw(centerBotLine_,   states);
    }


    void CrossBox::OnColorChange()
    {
        topLine2_.SetEntityColors(entityColorSet_);
        rightLine2_.SetEntityColors(entityColorSet_);
        botLine2_.SetEntityColors(entityColorSet_);
        leftLine2_.SetEntityColors(entityColorSet_);
        centerLeftLine_.SetEntityColors(entityColorSet_);
        centerRightLine_.SetEntityColors(entityColorSet_);
        centerTopLine_.SetEntityColors(entityColorSet_);
        centerBotLine_.SetEntityColors(entityColorSet_);

        const sf::Color FG_COLOR(GetEntityColorForeground());

        crossSprite_.setColor(FG_COLOR);
        tJointLeftSprite_.setColor(FG_COLOR);
        tJointRightSprite_.setColor(FG_COLOR);
        tJointTopSprite_.setColor(FG_COLOR);
        tJointBotSprite_.setColor(FG_COLOR);
    }


    void CrossBox::SetupCrossBox(const float            POS_LEFT,
                                 const float            POS_TOP,
                                 const float            INTENDED_FIRST_WIDTH,
                                 const float            INTENDED_FIRST_HEIGHT,
                                 const float            INTENDED_SECOND_WIDTH,
                                 const float            INTENDED_SECOND_HEIGHT,
                                 const bool             WILL_GROW_TO_FIT,
                                 const BackgroundInfo & BACKGROUND_INFO)
    {
        auto const GE_PTR{ GuiElements::Instance() };

        SetEntityRegion( sf::FloatRect(POS_LEFT, POS_TOP, INTENDED_FIRST_WIDTH+INTENDED_SECOND_WIDTH, INTENDED_FIRST_HEIGHT + INTENDED_SECOND_HEIGHT) );

        //setup background within the initial region before it grows to fit
        BackgroundInfo bgInfo(BACKGROUND_INFO);
        bgInfo.region = GetEntityRegion();
        bgInfo.region.left   += GE_PTR->GetLineSmallBoxPadLeft();
        bgInfo.region.top    += GE_PTR->GetLineSmallBoxPadTop();
        bgInfo.region.width  -= GE_PTR->GetLineSmallBoxPadLeft() + GE_PTR->GetLineSmallBoxPadRight();
        bgInfo.region.height -= GE_PTR->GetLineSmallBoxPadBot() + GE_PTR->GetLineSmallBoxPadTop();
        backgroundImage_.Setup(bgInfo);

        const float INTENDED_WIDTH(INTENDED_FIRST_WIDTH + INTENDED_SECOND_WIDTH);
        const float INTENDED_HEIGHT(INTENDED_FIRST_HEIGHT + INTENDED_SECOND_HEIGHT);

        //check the minimum width and height

        const float MIN_WIDTH(GetMinWidth_CrossBox(WILL_GROW_TO_FIT));
        M_ASSERT_OR_LOGANDTHROW_SS((INTENDED_WIDTH >= MIN_WIDTH), entityName_ << "'s width of " << INTENDED_WIDTH << " was smaller than the minimum of " << MIN_WIDTH << ".  (will_grow=" << std::boolalpha << WILL_GROW_TO_FIT << ")");
        //const float HORIZ_MID_LEN(INTENDED_WIDTH - MIN_WIDTH + ((WILL_GROW_TO_FIT) ? ((GE_PTR->GetLineSmallBoxPadLeft() + GE_PTR->GetLineSmallBoxPadLeft()) * 2.0f) : (0.0f)));

        const float FIRST_HORIZ_LEN(INTENDED_FIRST_WIDTH   - static_cast<float>(GE_PTR->GetRect_LineSmallCornerTopLeft().width)  - static_cast<float>(GE_PTR->GetRect_LineSmallCross().width / 2));
        const float SECOND_HORIZ_LEN(INTENDED_SECOND_WIDTH - static_cast<float>(GE_PTR->GetRect_LineSmallCornerTopRight().width) - static_cast<float>(GE_PTR->GetRect_LineSmallCross().width / 2));

        //std::cout << entityName_ << "'s first_int_wid=" << INTENDED_FIRST_WIDTH << ", second_int_wid=" << INTENDED_SECOND_WIDTH << ", min_wid=" << MIN_WIDTH << ", horiz_mid_len=" << HORIZ_MID_LEN << std::endl;
        //M_ASSERT_OR_LOGANDTHROW_SS((HORIZ_MID_LEN == (FIRST_HORIZ_LEN + SECOND_HORIZ_LEN)), entityName_ << "'s first_horiz_len=" << FIRST_HORIZ_LEN << " + second_horiz_len=" << SECOND_HORIZ_LEN << " != horiz_mid_len=" << HORIZ_MID_LEN);

        const float MIN_HEIGHT(GetMinHeight_CrossBox(WILL_GROW_TO_FIT));
        M_ASSERT_OR_LOGANDTHROW_SS((INTENDED_HEIGHT >= MIN_HEIGHT), entityName_ << "'s height of " << INTENDED_HEIGHT << " was smaller than the minimum of " << MIN_HEIGHT << ".   (will_grow=" << std::boolalpha << WILL_GROW_TO_FIT << ")");
        //const float VERT_MID_LEN(INTENDED_HEIGHT - MIN_HEIGHT + ((WILL_GROW_TO_FIT) ? ((GE_PTR->GetLineSmallBoxPadTop() + GE_PTR->GetLineSmallBoxPadBot()) * 2.0f) : (0.0f)));

        const float FIRST_VERT_LEN (INTENDED_FIRST_HEIGHT  - static_cast<float>(GE_PTR->GetRect_LineSmallCornerTopLeft().height) - static_cast<float>(GE_PTR->GetRect_LineSmallCross().height / 2));
        const float SECOND_VERT_LEN(INTENDED_SECOND_HEIGHT - static_cast<float>(GE_PTR->GetRect_LineSmallCornerBotLeft().height) - static_cast<float>(GE_PTR->GetRect_LineSmallCross().height / 2));

        //std::cout << entityName_ << "'s first_int_hgt=" << INTENDED_FIRST_HEIGHT << ", second_int_hgt=" << INTENDED_SECOND_HEIGHT << ", min_hgt=" << MIN_HEIGHT << ", vert_mid_len=" << VERT_MID_LEN << std::endl;

        //establish overall position and size
        const float LEFT_TO_USE(GetEntityPos().x - ((WILL_GROW_TO_FIT) ? (GE_PTR->GetLineSmallBoxPadLeft()) : (0.0f)));
        const float TOP_TO_USE(GetEntityPos().y - ((WILL_GROW_TO_FIT) ? (GE_PTR->GetLineSmallBoxPadTop()) : (0.0f)));
        const float WIDTH_TO_USE(INTENDED_WIDTH + ((WILL_GROW_TO_FIT) ? (GE_PTR->GetLineSmallBoxPadLeft() + GE_PTR->GetLineSmallBoxPadRight()) : (0.0f)));
        const float HEIGHT_TO_USE(INTENDED_HEIGHT + ((WILL_GROW_TO_FIT) ? (GE_PTR->GetLineSmallBoxPadTop() + GE_PTR->GetLineSmallBoxPadBot()) : (0.0f)));

        SetEntityRegion(sf::FloatRect(LEFT_TO_USE, TOP_TO_USE, WIDTH_TO_USE, HEIGHT_TO_USE));

        //position the sprites
        float x(LEFT_TO_USE);
        float y(TOP_TO_USE);

        cTopLeftSprite_.setPosition(x, y);
        x += cTopLeftSprite_.getLocalBounds().width;

        if (FIRST_HORIZ_LEN > 0)
            topLine_.Setup(x, y, static_cast<std::size_t>(FIRST_HORIZ_LEN));
        x += FIRST_HORIZ_LEN;

        tJointTopSprite_.setPosition(x, y);
        x += tJointTopSprite_.getLocalBounds().width;

        const float SECOND_TOP_AND_BOT_X(x);

        if (SECOND_HORIZ_LEN > 0)
            topLine2_.Setup(x, y, static_cast<std::size_t>(SECOND_HORIZ_LEN));
        x += SECOND_HORIZ_LEN;

        cTopRightSprite_.setPosition(x, y);
        x += (cTopRightSprite_.getLocalBounds().width - static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width));
        y += cTopRightSprite_.getLocalBounds().height;

        if (FIRST_VERT_LEN > 0)
            rightLine_.Setup(x, y, static_cast<std::size_t>(FIRST_VERT_LEN));
        y += FIRST_VERT_LEN;
        x -= (tJointRightSprite_.getLocalBounds().width - static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width));

        tJointRightSprite_.setPosition(x, y);
        y += tJointRightSprite_.getLocalBounds().height;
        x += (tJointRightSprite_.getLocalBounds().width - static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width));

        if (utilz::IsRealClose(SECOND_VERT_LEN, 0.0f))
            rightLine2_.Setup(x, y, static_cast<std::size_t>(SECOND_VERT_LEN));

        y += SECOND_VERT_LEN;
        x += (static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width) - cBotRightSprite_.getLocalBounds().width);

        cBotRightSprite_.setPosition(x, y);
        x = SECOND_TOP_AND_BOT_X;
        y += (cBotRightSprite_.getLocalBounds().height - static_cast<float>(GE_PTR->GetRect_LineSmallHorizontalBot().height));

        const float BOTTOM_LINE_Y(y);

        if (utilz::IsRealClose(SECOND_HORIZ_LEN, 0.0f))
            botLine_.Setup(x, y, static_cast<std::size_t>(SECOND_HORIZ_LEN));

        //x -= (SECOND_HORIZ_LEN + tJointBotSprite_.getLocalBounds().width);
        y -= (tJointBotSprite_.getLocalBounds().height - static_cast<float>(GE_PTR->GetRect_LineSmallHorizontalBot().height));

        x = (LEFT_TO_USE + cBotLeftSprite_.getLocalBounds().width + FIRST_HORIZ_LEN);
        tJointBotSprite_.setPosition(x, y);

        y = (TOP_TO_USE + tJointTopSprite_.getLocalBounds().height + FIRST_VERT_LEN);
        x = (LEFT_TO_USE + tJointLeftSprite_.getLocalBounds().width + FIRST_HORIZ_LEN);
        crossSprite_.setPosition(x, y);

        y = BOTTOM_LINE_Y;
        x = (LEFT_TO_USE + cBotLeftSprite_.getLocalBounds().width);

        if (utilz::IsRealClose(FIRST_HORIZ_LEN, 0.0f))
            botLine2_.Setup(x, y, static_cast<std::size_t>(FIRST_HORIZ_LEN));

        x = LEFT_TO_USE;
        y -= (cBotLeftSprite_.getLocalBounds().height - static_cast<float>(GE_PTR->GetRect_LineSmallHorizontalBot().height));
        cBotLeftSprite_.setPosition(x, y);

        y = (TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height + FIRST_VERT_LEN + tJointLeftSprite_.getLocalBounds().height);
        if (SECOND_VERT_LEN > 0)
            leftLine_.Setup(x, y, static_cast<std::size_t>(SECOND_VERT_LEN));

        y = TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height + FIRST_VERT_LEN;
        x = LEFT_TO_USE;
        tJointLeftSprite_.setPosition(x, y);

        y = (TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height);

        if (FIRST_VERT_LEN > 0)
            leftLine2_.Setup(x, y, static_cast<std::size_t>(FIRST_VERT_LEN));

        if (FIRST_HORIZ_LEN > 0)
        {
            centerLeftLine_.Setup((LEFT_TO_USE + tJointLeftSprite_.getLocalBounds().width), (TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height + FIRST_VERT_LEN + (tJointLeftSprite_.getLocalBounds().height * 0.5f) - static_cast<float>(GE_PTR->GetRect_LineSmallHorizontalBot().height / 2)), static_cast<std::size_t>(FIRST_HORIZ_LEN));
        }

        if (SECOND_HORIZ_LEN > 0)
        {
            centerRightLine_.Setup((LEFT_TO_USE + tJointLeftSprite_.getLocalBounds().width + FIRST_HORIZ_LEN + crossSprite_.getLocalBounds().width), (TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height + FIRST_VERT_LEN + (tJointLeftSprite_.getLocalBounds().height / 2.0f) - static_cast<float>(GE_PTR->GetRect_LineSmallHorizontalBot().height / 2)), static_cast<std::size_t>(SECOND_HORIZ_LEN));
        }

        if (FIRST_VERT_LEN > 0)
        {
            centerTopLine_.Setup((LEFT_TO_USE + cTopLeftSprite_.getLocalBounds().width + FIRST_HORIZ_LEN + (tJointTopSprite_.getLocalBounds().width * 0.5f) - static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width / 2)), (TOP_TO_USE + tJointTopSprite_.getLocalBounds().height), static_cast<std::size_t>(FIRST_VERT_LEN));
        }

        if (SECOND_VERT_LEN > 0)
        {
            centerBotLine_.Setup((LEFT_TO_USE + cTopLeftSprite_.getLocalBounds().width + FIRST_HORIZ_LEN + (tJointTopSprite_.getLocalBounds().width * 0.5f) - static_cast<float>(GE_PTR->GetRect_LineSmallVerticalRight().width / 2)), (TOP_TO_USE + tJointTopSprite_.getLocalBounds().height + FIRST_VERT_LEN + crossSprite_.getLocalBounds().height), static_cast<std::size_t>(SECOND_VERT_LEN));
        }
    }

}
}
}
