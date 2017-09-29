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
// song-image-manager.cpp
//
#include "song-image-manager.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    std::unique_ptr<SongImageManager> SongImageManager::instanceUPtr_{ nullptr };


    SongImageManager::SongImageManager()
    {
        M_HP_LOG_DBG("Singleton Construction: SongImageManager");
    }


    SongImageManager::~SongImageManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: SongImageManager");
    }


    SongImageManager * SongImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): SongImageManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void SongImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<SongImageManager>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: SongImageManager");
        }
    }


    void SongImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::SongImageManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

}
}
