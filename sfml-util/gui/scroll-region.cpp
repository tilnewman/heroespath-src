//
// scroll-region.cpp
//
#include "scroll-region.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/gui/box.hpp"

#include "game/log-macros.hpp"


namespace sfml_util
{
namespace gui
{

    ScrollRegion::ScrollRegion(const std::string &       NAME,
                               const sf::FloatRect &     REGION,
                               const IGuiEntitySList_t & STRETCHED_OBJ_LIST,
                               IStage * const            stagePtr,
                               const float               BETWEEN_PAD,
                               const box::Info &         BOX_INFO)
    :
        GuiEntity         (std::string(NAME).append("_ScrollRegion"), REGION),
        boxSPtr_          (),
        sliderBarSPtr_    (),
        list_             (STRETCHED_OBJ_LIST),
        pad_              (BETWEEN_PAD),
        currentVertPos_   (0.0f),
        sprite_           (),
        renderTextureSPtr_()
    {
        Setup(REGION, STRETCHED_OBJ_LIST, stagePtr, BETWEEN_PAD, BOX_INFO);
    }


    ScrollRegion::~ScrollRegion()
    {}


    void ScrollRegion::Setup(const sf::FloatRect &      REGION,
                             const IGuiEntitySList_t &  STRETCHED_OBJ_LIST,
                             IStage * const             stagePtr,
                             const float                BETWEEN_PAD,
                             const box::Info &          BOX_INFO)
    {
        SetEntityRegion(REGION);

        if (BOX_INFO.WillBox())
            boxSPtr_.reset( new sfml_util::gui::box::Box("ScrollRegion's", BOX_INFO) );
        else
            boxSPtr_.reset();

        list_.clear();
        list_ = STRETCHED_OBJ_LIST;

        pad_ = BETWEEN_PAD;

        currentVertPos_ = 0.0f;

        if (sliderBarSPtr_.get() == nullptr)
        {
            sliderBarSPtr_.reset( new SliderBar("ScrollRegion's",
                                                REGION.left + REGION.width - 15.0f,
                                                REGION.top + 10.0f,
                                                REGION.height - 20.0f,
                                                SliderStyle(),
                                                this) );

            stagePtr->EntityAdd(sliderBarSPtr_);
        }

        sliderBarSPtr_->SetCurrentValue(0.0f);

        SetupOffScreenTexture();
    }


    void ScrollRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //Note:  Use SetEntityWillDraw() to determine if ScrollRegion is resonsible for
        //       drawing the scrolled entitys
        if (entityWillDraw_ && (false == list_.empty()))
            target.draw(sprite_, states);
    }


    bool ScrollRegion::HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if (nullptr != sliderBarSPtr_.get())
        {
            currentVertPos_ = (GetTotalContentHeight() - GetEntityRegion().height) * PACKAGE.PTR_->GetCurrentValue();
            PositionSprite();
            return true;
        }
        else
            return false;
    }


    void ScrollRegion::Add(const IGuiEntitySPtr_t & E)
    {
        currentVertPos_ = 0.0f;
        sliderBarSPtr_->SetCurrentValue(0.0f);
        list_.push_back(E);
        SetupOffScreenTexture();
    }


    bool ScrollRegion::Remove(const IGuiEntitySPtr_t & E)
    {
        currentVertPos_ = 0.0f;
        sliderBarSPtr_->SetCurrentValue(0.0f);
        const std::size_t ORIG_SIZE(list_.size());
        list_.remove(E);
        SetupOffScreenTexture();
        return ! (ORIG_SIZE == list_.size());
    }


    void ScrollRegion::RemoveAll()
    {
        currentVertPos_ = 0.0f;
        sliderBarSPtr_->SetCurrentValue(0.0f);
        list_.clear();
        SetupOffScreenTexture();
    }


    void ScrollRegion::SetupOffScreenTexture()
    {
        //new up an off-screen texture
        renderTextureSPtr_.reset(new sf::RenderTexture);

        if (list_.empty())
        {
            renderTextureSPtr_->clear(sf::Color::Transparent);
            sprite_.setTexture(renderTextureSPtr_->getTexture());
            return;
        }

        //create()
        renderTextureSPtr_->create(static_cast<unsigned>(entityRegion_.width),
                                   static_cast<unsigned>(GetTotalContentHeight()));

        //make fully transparent
        renderTextureSPtr_->clear(sf::Color::Transparent);

        //draw all entitys to the off-screen texture
        float posY(0.0f);
        for(auto & nextGuiEntitySPtr : list_)
        {
            const float NEXT_GE_HEIGHT(nextGuiEntitySPtr->GetEntityRegion().height);
            nextGuiEntitySPtr->SetEntityPos(20.0f, posY); //+20.0f compensates for the sliderbar
            renderTextureSPtr_->draw( * nextGuiEntitySPtr );
            posY += NEXT_GE_HEIGHT + pad_;
        }

        renderTextureSPtr_->display();

        //setup the sprite
        sprite_.setTexture(renderTextureSPtr_->getTexture());
        sprite_.setPosition(entityRegion_.left, entityRegion_.top);

        PositionSprite();
    }


    void ScrollRegion::PositionSprite()
    {
        const sf::FloatRect RECTF(0.0f,
                                  currentVertPos_,
                                  entityRegion_.width,
                                  entityRegion_.height);

        sprite_.setTextureRect( sfml_util::ConvertRect<float, int>(RECTF) );
    }


    float ScrollRegion::GetTotalContentHeight() const
    {
        float total(0.0f);
        for (auto const & NEXT_ENTITY_SPTR: list_)
            total += NEXT_ENTITY_SPTR->GetEntityRegion().height + pad_;

        if (total > 0.0f)
            total -= pad_;

        return total;
    }

}
}
