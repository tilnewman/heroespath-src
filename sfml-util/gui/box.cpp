//
// box.cpp
//
#include "box.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/gui/gui-elements.hpp"

#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/log-macros.hpp"


namespace sfml_util
{
namespace gui
{
namespace box
{

    float GetMinWidth_Box(const bool WILL_GROW_TO_FIT)
    {
        float width(static_cast<float>(GuiElements::Instance()->GetRect_LineSmallCornerTopLeft().width * 2));

        if (WILL_GROW_TO_FIT)
            width += GuiElements::Instance()->GetLineSmallBoxPadLeft() + GuiElements::Instance()->GetLineSmallBoxPadRight();

        return width;
    }


    float GetMinHeight_Box(const bool WILL_GROW_TO_FIT)
    {
        float height(static_cast<float>(GuiElements::Instance()->GetRect_LineSmallCornerTopLeft().height * 2));

        if (WILL_GROW_TO_FIT)
            height += (GuiElements::Instance()->GetLineSmallBoxPadTop() + GuiElements::Instance()->GetLineSmallBoxPadBot());

        return height;
    }


    Box::Box(const std::string &    NAME,
             const box::Info &      BOX_INFO)
    :
        GuiEntity        (std::string(NAME).append("_Box"), BOX_INFO.region, BOX_INFO.color_set),
        boxInfo_         (BOX_INFO),
        topLine_         (std::string(NAME).append("Box's_Top Line"), Orientation::Horiz,   Side::Top,   false),
        rightLine_       (std::string(NAME).append("Box's_Right Line"), Orientation::Vert,  Side::Right, false),
        botLine_         (std::string(NAME).append("Box's_Bottom Line"), Orientation::Horiz,Side::Bottom,false),
        leftLine_        (std::string(NAME).append("Box's_Left Line"), Orientation::Vert,   Side::Left,  false),
        cTopLeftSprite_  ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallCornerTopLeft()),
        cTopRightSprite_ ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallCornerTopRight()),
        cBotLeftSprite_  ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallCornerBotLeft()),
        cBotRightSprite_ ( * GuiElements::Instance()->GetTexture(), GuiElements::Instance()->GetRect_LineSmallCornerBotRight()),
        backgroundImage_ ()
    {
        SetupBox(BOX_INFO);
    }


    Box::~Box()
    {}


    void Box::SetupBox(const box::Info & BOX_INFO)
    {
        boxInfo_ = BOX_INFO;

        //set initial values, but final values will be set further down in this function
        SetEntityRegion( BOX_INFO.region );

        //setup background within the initial region before it grows to fit
        BackgroundInfo bgInfo(BOX_INFO.bg_info);
        bgInfo.region = GetEntityRegion();
        backgroundImage_.Setup(bgInfo);

        //skip sprite setup if not drawing (region is invalid)
        if (BOX_INFO.IsValid() == false)
            return;

        //skip if not drawing with sprites
        if (BOX_INFO.gold_bars == false)
            return;

        //setup gold bar sprites
        const GuiElementsSPtr_t GE_SPTR{ GuiElements::Instance() };

        //check the minimum width and height
        const float MIN_WIDTH(GetMinWidth_Box(BOX_INFO.will_grow));
        M_ASSERT_OR_LOGANDTHROW_SS((BOX_INFO.region.width >= MIN_WIDTH), entityName_ << "'s width of " << BOX_INFO.region.width << " was smaller than the minimum of " << MIN_WIDTH << ".  (will_grow=" << std::boolalpha << BOX_INFO.will_grow << ")");
        const float HORIZ_MID_LEN(BOX_INFO.region.width - MIN_WIDTH + ((BOX_INFO.will_grow) ? ((GE_SPTR->GetLineSmallBoxPadLeft() + GE_SPTR->GetLineSmallBoxPadLeft()) * 2.0f) : (0.0f)));

        const float MIN_HEIGHT(GetMinHeight_Box(BOX_INFO.will_grow));
        M_ASSERT_OR_LOGANDTHROW_SS((BOX_INFO.region.height >= MIN_HEIGHT), entityName_ << "'s height of " << BOX_INFO.region.height << " was smaller than the minimum of " << MIN_HEIGHT << ".   (will_grow=" << std::boolalpha << BOX_INFO.will_grow << ")");
        const float VERT_MID_LEN(BOX_INFO.region.height - MIN_HEIGHT + ((BOX_INFO.will_grow) ? ((GE_SPTR->GetLineSmallBoxPadTop() + GE_SPTR->GetLineSmallBoxPadBot()) * 2.0f) : (0.0f)));

        //establish position and size
        const float LEFT_TO_USE  (GetEntityPos().x -       ((BOX_INFO.will_grow) ? (GE_SPTR->GetLineSmallBoxPadLeft()) : (0.0f)));
        const float TOP_TO_USE   (GetEntityPos().y -       ((BOX_INFO.will_grow) ? (GE_SPTR->GetLineSmallBoxPadTop())  : (0.0f)));
        const float WIDTH_TO_USE (BOX_INFO.region.width  + ((BOX_INFO.will_grow) ? (GE_SPTR->GetLineSmallBoxPadLeft() + GE_SPTR->GetLineSmallBoxPadRight()) : (0.0f)));
        const float HEIGHT_TO_USE(BOX_INFO.region.height + ((BOX_INFO.will_grow) ? (GE_SPTR->GetLineSmallBoxPadTop() + GE_SPTR->GetLineSmallBoxPadBot()) : (0.0f)));

        SetEntityRegion(sf::FloatRect(LEFT_TO_USE, TOP_TO_USE, WIDTH_TO_USE, HEIGHT_TO_USE));

        //position the sprites
        cTopLeftSprite_.setPosition(LEFT_TO_USE, TOP_TO_USE);

        if (HORIZ_MID_LEN > 0)
            topLine_.Setup(LEFT_TO_USE + cTopLeftSprite_.getLocalBounds().width, TOP_TO_USE, static_cast<std::size_t>(HORIZ_MID_LEN));

        const float RIGHT(LEFT_TO_USE + cTopLeftSprite_.getLocalBounds().width + HORIZ_MID_LEN);
        cTopRightSprite_.setPosition(RIGHT, TOP_TO_USE);

        if (VERT_MID_LEN > 0)
            rightLine_.Setup(RIGHT + cTopRightSprite_.getLocalBounds().width - GE_SPTR->GetRect_LineSmallVerticalRight().width, TOP_TO_USE + cTopRightSprite_.getLocalBounds().height, static_cast<std::size_t>(VERT_MID_LEN));

        const float BOTTOM(TOP_TO_USE + cTopRightSprite_.getLocalBounds().height + VERT_MID_LEN);
        cBotRightSprite_.setPosition(RIGHT, BOTTOM);

        if (HORIZ_MID_LEN > 0)
            botLine_.Setup(LEFT_TO_USE + cBotLeftSprite_.getLocalBounds().width, BOTTOM + cBotRightSprite_.getLocalBounds().height - GE_SPTR->GetRect_LineSmallHorizontalBot().height, static_cast<std::size_t>(HORIZ_MID_LEN));

        cBotLeftSprite_.setPosition(LEFT_TO_USE, BOTTOM);

        if (VERT_MID_LEN > 0)
            leftLine_.Setup(LEFT_TO_USE, TOP_TO_USE + cTopLeftSprite_.getLocalBounds().height, static_cast<std::size_t>(VERT_MID_LEN));

        SetEntityColors(BOX_INFO.color_set);
    }


    void Box::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);

        boxInfo_.region.left = POS_LEFT;
        boxInfo_.region.top  = POS_TOP;

        topLine_.MoveEntityPos  (sfml_util::RelativeOffset(topLine_.GetEntityPos().x,   POS_LEFT), sfml_util::RelativeOffset(topLine_.GetEntityPos().y,   POS_TOP));
        rightLine_.MoveEntityPos(sfml_util::RelativeOffset(rightLine_.GetEntityPos().x, POS_LEFT), sfml_util::RelativeOffset(rightLine_.GetEntityPos().y, POS_TOP));
        botLine_.MoveEntityPos  (sfml_util::RelativeOffset(botLine_.GetEntityPos().x,   POS_LEFT), sfml_util::RelativeOffset(botLine_.GetEntityPos().y,   POS_TOP));
        leftLine_.MoveEntityPos (sfml_util::RelativeOffset(leftLine_.GetEntityPos().x,  POS_LEFT), sfml_util::RelativeOffset(leftLine_.GetEntityPos().y,  POS_TOP));

        cTopLeftSprite_.move (sfml_util::RelativeOffset(cTopLeftSprite_.getPosition().x,  POS_LEFT), sfml_util::RelativeOffset(cTopLeftSprite_.getPosition().y,  POS_TOP));
        cTopRightSprite_.move(sfml_util::RelativeOffset(cTopRightSprite_.getPosition().x, POS_LEFT), sfml_util::RelativeOffset(cTopRightSprite_.getPosition().y, POS_TOP));
        cBotLeftSprite_.move (sfml_util::RelativeOffset(cBotLeftSprite_.getPosition().x,  POS_LEFT), sfml_util::RelativeOffset(cBotLeftSprite_.getPosition().y,  POS_TOP));
        cBotRightSprite_.move(sfml_util::RelativeOffset(cBotRightSprite_.getPosition().x, POS_LEFT), sfml_util::RelativeOffset(cBotRightSprite_.getPosition().y, POS_TOP));

        backgroundImage_.SetPosition(POS_LEFT, POS_TOP);
    }


    void Box::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);

        boxInfo_.region.left += HORIZ;
        boxInfo_.region.top  += VERT;

        topLine_.MoveEntityPos(HORIZ, VERT);
        rightLine_.MoveEntityPos(HORIZ, VERT);
        botLine_.MoveEntityPos(HORIZ, VERT);
        leftLine_.MoveEntityPos(HORIZ, VERT);

        cTopLeftSprite_.move(HORIZ, VERT);
        cTopRightSprite_.move(HORIZ, VERT);
        cBotLeftSprite_.move(HORIZ, VERT);
        cBotRightSprite_.move(HORIZ, VERT);

        backgroundImage_.MovePosition(HORIZ, VERT);
    }


    void Box::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //draws nothing if invalid/default
        target.draw(backgroundImage_, states);

        //draw nothing if invalid/default boxInfo_
        if (boxInfo_.WillBox())
        {
            if (boxInfo_.gold_bars)
            {
                target.draw(cTopLeftSprite_, states);
                topLine_.draw(target, states);
                target.draw(cTopRightSprite_, states);
                rightLine_.draw(target, states);
                target.draw(cBotRightSprite_, states);
                botLine_.draw(target, states);
                target.draw(cBotLeftSprite_, states);
                leftLine_.draw(target, states);
            }
            else
            {
                sfml_util::DrawRectangle(target,
                                         states,
                                         boxInfo_.region,
                                         GetEntityColorForeground(),
                                         boxInfo_.line_thickness);

                if (boxInfo_.frame_lines)
                {
                    sf::Color shadedColor(GetEntityColorForeground());
                    shadedColor.a = Info::DEFAULT_FRAME_LINES_ALPHA_;

                    sf::FloatRect outerRect(boxInfo_.region);
                    outerRect.left   -= 1.0f;
                    outerRect.top    -= 1.0f;
                    outerRect.width  += 2.0f;
                    outerRect.height += 2.0f;

                    sfml_util::DrawRectangle(target,
                                             states,
                                             outerRect,
                                             shadedColor,
                                             boxInfo_.line_thickness);

                    sf::FloatRect innerRect(boxInfo_.region);
                    innerRect.left   += 1.0f;
                    innerRect.top    += 1.0f;
                    innerRect.width  -= 2.0f;
                    innerRect.height -= 2.0f;

                    sfml_util::DrawRectangle(target,
                                             states,
                                             innerRect,
                                             shadedColor,
                                             boxInfo_.line_thickness);
                }
            }
        }
    }


    void Box::OnColorChange()
    {
        topLine_.SetEntityColors(entityColorSet_);
        rightLine_.SetEntityColors(entityColorSet_);
        botLine_.SetEntityColors(entityColorSet_);
        leftLine_.SetEntityColors(entityColorSet_);

        topLine_.FakeColorSetAsIfFocusIs(entityHasFocus_);
        rightLine_.FakeColorSetAsIfFocusIs(entityHasFocus_);
        botLine_.FakeColorSetAsIfFocusIs(entityHasFocus_);
        leftLine_.FakeColorSetAsIfFocusIs(entityHasFocus_);

        const sf::Color FG_COLOR(GetEntityColorForeground());
        cTopLeftSprite_.setColor (FG_COLOR);
        cTopRightSprite_.setColor(FG_COLOR);
        cBotLeftSprite_.setColor (FG_COLOR);
        cBotRightSprite_.setColor(FG_COLOR);

        backgroundImage_.SetColor(GetEntityColorBackground());
    }


    bool Box::SetHasFocus(const bool HAS_FOCUS)
    {
        const bool DID_FOCUS_CHANGE( GuiEntity::SetHasFocus(HAS_FOCUS));

        topLine_.SetHasFocus(HAS_FOCUS);
        rightLine_.SetHasFocus(HAS_FOCUS);
        botLine_.SetHasFocus(HAS_FOCUS);
        leftLine_.SetHasFocus(HAS_FOCUS);

        return DID_FOCUS_CHANGE;
    }

}
}
}
