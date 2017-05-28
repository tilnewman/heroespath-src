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
// name-info.cpp
//
#include "name-info.hpp"

#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/log-macros.hpp"

#include <sstream>
#include <string>


namespace game
{
namespace creature
{

    std::unique_ptr<NameInfo>  NameInfo::instanceUPtr_{ nullptr };


    NameInfo::NameInfo()
    :
        sizeMap_()
    {
        M_HP_LOG_DBG("Singleton Construction: NameInfo");
    }


    NameInfo::~NameInfo()
    {
        M_HP_LOG_DBG("Singleton Destruction: NameInfo");
    }


    NameInfo * NameInfo::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): NameInfo");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void NameInfo::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new NameInfo);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: NameInfo");
        }
    }


    void NameInfo::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::NameInfo::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    sfml_util::FontPtr_t NameInfo::DefaultFont() const
    {
        return sfml_util::FontManager::Instance()->Font_Default2();
    }


    unsigned int NameInfo::DefaultSize() const
    {
        return sfml_util::FontManager::Instance()->Size_Normal();
    }


    float NameInfo::LengthMax(const sfml_util::FontPtr_t FONT_PTR, const unsigned int CHAR_SIZE)
    {
        const FontSizePair_t FONT_SIZE_PAIR(std::make_pair(FONT_PTR, CHAR_SIZE));
        const float ORIG_WIDTH(sizeMap_[FONT_SIZE_PAIR]);

        if (ORIG_WIDTH < 1.0f)
        {
            const sfml_util::gui::TextInfo TEXT_INFO(LargestName(), FONT_PTR, CHAR_SIZE, sf::Color::White, sfml_util::Justified::Left);
            sfml_util::gui::TextRegion textRegion("CreatureNameInfoLengthDeterminer", TEXT_INFO, sf::FloatRect());

            const float NAME_WIDTH(textRegion.GetEntityRegion().width);
            sizeMap_[FONT_SIZE_PAIR] = NAME_WIDTH;

            return NAME_WIDTH;
        }
        else
            return ORIG_WIDTH;
    }


    float NameInfo::Length(const std::string & TEXT, const sfml_util::FontPtr_t FONT_PTR, const unsigned int CHAR_SIZE) const
    {
        return Length(sfml_util::gui::TextInfo(TEXT, FONT_PTR, CHAR_SIZE, sf::Color::White, sfml_util::Justified::Left));
    }


    float NameInfo::Length(const sfml_util::gui::TextInfo & TEXT_INFO) const
    {
        sfml_util::gui::TextRegion textRegion("CreatureNameInfoLengthDeterminer", TEXT_INFO, sf::FloatRect());
        return textRegion.GetEntityRegion().width;
    }

}
}
