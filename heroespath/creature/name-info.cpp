//
// name-info.cpp
//
#include "name-info.hpp"

#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "heroespath/log-macros.hpp"

#include <sstream>


namespace heroespath
{
namespace creature
{

    NameInfoSPtr_t NameInfo::instance_(nullptr);


    NameInfo::NameInfo()
    :
        sizeMap_()
    {}


    NameInfo::~NameInfo()
    {}


    NameInfoSPtr_t NameInfo::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new NameInfo );

        return instance_;
    }


    sfml_util::FontSPtr_t NameInfo::DefaultFont() const
    {
        return sfml_util::FontManager::Instance()->Font_Default2();
    }


    unsigned int NameInfo::DefaultSize() const
    {
        return sfml_util::FontManager::Instance()->Size_Normal();
    }


    float NameInfo::LengthMax(const sfml_util::FontSPtr_t & FONT_SPTR, const unsigned int CHAR_SIZE)
    {
        const FontSizePair_t FONT_SIZE_PAIR(std::make_pair(FONT_SPTR, CHAR_SIZE));
        const float ORIG_WIDTH(sizeMap_[FONT_SIZE_PAIR]);

        if (ORIG_WIDTH < 1.0f)
        {
            const sfml_util::gui::TextInfo TEXT_INFO(LargestName(), FONT_SPTR, CHAR_SIZE, sf::Color::White, sfml_util::Justified::Left);
            sfml_util::gui::TextRegion textRegion("CreatureNameInfoLengthDeterminer", TEXT_INFO, sf::FloatRect());

            const float NAME_WIDTH(textRegion.GetEntityRegion().width);
            sizeMap_[FONT_SIZE_PAIR] = NAME_WIDTH;

            return NAME_WIDTH;
        }
        else
            return ORIG_WIDTH;
    }


    float NameInfo::Length(const std::string & TEXT, const sfml_util::FontSPtr_t & FONT_SPTR, const unsigned int CHAR_SIZE) const
    {
        return Length(sfml_util::gui::TextInfo(TEXT, FONT_SPTR, CHAR_SIZE, sf::Color::White, sfml_util::Justified::Left));
    }


    float NameInfo::Length(const sfml_util::gui::TextInfo & TEXT_INFO) const
    {
        sfml_util::gui::TextRegion textRegion("CreatureNameInfoLengthDeterminer", TEXT_INFO, sf::FloatRect());
        return textRegion.GetEntityRegion().width;
    }

}
}
