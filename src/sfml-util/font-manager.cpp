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
// font-manager.cpp
//
#include "font-manager.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>

#include <memory>


namespace sfml_util
{
    const unsigned int FontManager::SIZE_LARGER_MIN_(36);
    const unsigned int FontManager::SIZE_LARGER_MAX_(144);
    //
    const unsigned int FontManager::SIZE_LARGE_MIN_(31);
    const unsigned int FontManager::SIZE_LARGE_MAX_(124);
    //
    const unsigned int FontManager::SIZE_LARGEISH_MIN_(28);
    const unsigned int FontManager::SIZE_LARGEISH_MAX_(112);
    //
    const unsigned int FontManager::SIZE_NORMAL_MIN_(25);
    const unsigned int FontManager::SIZE_NORMAL_MAX_(100);
    //
    const unsigned int FontManager::SIZE_SMALLISH_MIN_(20);
    const unsigned int FontManager::SIZE_SMALLISH_MAX_(80);
    //
    const unsigned int FontManager::SIZE_SMALL_MIN_(17);
    const unsigned int FontManager::SIZE_SMALL_MAX_(68);
    //
    const unsigned int FontManager::SIZE_TINY_MIN_(14);
    const unsigned int FontManager::SIZE_TINY_MAX_(56);
    //
    std::string FontManager::fontsDirectoryPath_;
    std::unique_ptr<FontManager> FontManager::instanceUPtr_{ nullptr };


    FontManager::FontManager()
    :
        euler_fontSPtr_          (),
        gentiumPlus_fontSPtr_    (),
        goudyBookletter_fontSPtr_(),
        modernAntiqua_fontSPtr_  (),
        queenCountry_fontSPtr_   (),
        quillSword_fontSPtr_     (),
        valleyForge_fontSPtr_    ()
    {}


    FontManager * FontManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void FontManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new FontManager);
        }
    }


    void FontManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::FontManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void FontManager::SetFontsDirectory(const std::string & PATH)
    {
        fontsDirectoryPath_ = PATH;
    }


    unsigned int FontManager::Size_Larger() const
    {
        return sfml_util::MapByRes(SIZE_LARGER_MIN_, SIZE_LARGER_MAX_);
    }


    unsigned int FontManager::Size_Large() const
    {
        return sfml_util::MapByRes(SIZE_LARGE_MIN_, SIZE_LARGE_MAX_);
    }


    unsigned int FontManager::Size_Largeish() const
    {
        return sfml_util::MapByRes(SIZE_LARGEISH_MIN_, SIZE_LARGEISH_MAX_);
    }


    unsigned int FontManager::Size_Normal() const
    {
        return sfml_util::MapByRes(SIZE_NORMAL_MIN_, SIZE_NORMAL_MAX_);
    }


    unsigned int FontManager::Size_Smallish() const
    {
        return sfml_util::MapByRes(SIZE_SMALLISH_MIN_, SIZE_SMALLISH_MAX_);
    }


    unsigned int FontManager::Size_Small() const
    {
        return sfml_util::MapByRes(SIZE_SMALL_MIN_, SIZE_SMALL_MAX_);
    }


    unsigned int FontManager::Size_Tiny() const
    {
        return sfml_util::MapByRes(SIZE_TINY_MIN_, SIZE_TINY_MAX_);
    }


    void FontManager::LoadFont(const std::string & FONT_FILE_NAME, FontSPtr_t & fontSPtr) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(fontsDirectoryPath_) / bfs::path(FONT_FILE_NAME)));
        sfml_util::LoadFontSPtr(fontSPtr, PATH_OBJ.string());
    }

}
