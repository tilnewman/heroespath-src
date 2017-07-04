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
// character-warehouse.cpp (player)
//
#include "character-warehouse.hpp"

#include "game/player/character.hpp"
#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace player
{

    std::unique_ptr<CharacterWarehouse> CharacterWarehouse::instanceUPtr_{ nullptr };


    CharacterWarehouse::CharacterWarehouse()
    :
        characterUVec_()
    {
        M_HP_LOG_DBG("Singleton Construction: Player CharacterWarehouse");
    }


    CharacterWarehouse::~CharacterWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: Player CharacterWarehouse");
    }


    CharacterWarehouse * CharacterWarehouse::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): Player CharacterWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void CharacterWarehouse::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new CharacterWarehouse);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: Player CharacterWarehouse");
        }
    }


    void CharacterWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::player::CharacterWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    CharacterPtr_t CharacterWarehouse::Store(const CharacterPtr_t CHARACTER_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CHARACTER_PTR != nullptr),
            "game::player::CharacterWarehouse::Store() given a nullptr.");

        std::size_t indexToSaveAt{ characterUVec_.size() };

        //Ensure this item is not already stored, and along the way,
        //look for an abandoned slot to use with indexToSaveAt.
        auto const NUM_CHARACTERS{ indexToSaveAt };
        for (std::size_t i(0); i<NUM_CHARACTERS; ++i)
        {
            auto const NEXT_CHARACTER_PTR{ characterUVec_[i].get() };
            if (NEXT_CHARACTER_PTR == CHARACTER_PTR)
            {
                std::ostringstream ss;
                ss << "game::player::CharacterWarehouse::Store((" << CHARACTER_PTR << ")"
                    << " name=\"" << CHARACTER_PTR->Name() << "\") was already in the warehouse.";

                throw std::runtime_error(ss.str());
            }
            else if (NEXT_CHARACTER_PTR == nullptr)
            {
                indexToSaveAt = i;
            }
        }

        if (indexToSaveAt < NUM_CHARACTERS)
        {
            characterUVec_[indexToSaveAt].reset(CHARACTER_PTR);
            return characterUVec_[indexToSaveAt].get();
        }
        else
        {
            CharacterUPtr_t tempUPtr;
            tempUPtr.reset(CHARACTER_PTR);
            characterUVec_.push_back( std::move(tempUPtr) );
            return characterUVec_[characterUVec_.size() - 1].get();
        }
    }


    void CharacterWarehouse::Free(CharacterPtr_t & creaturePtr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr != nullptr),
            "game::player::CharacterWarehouse::Free() given a nullptr.");

        for (auto & nextCharacterUPtr : characterUVec_)
        {
            if (nextCharacterUPtr.get() == creaturePtr)
            {
                creaturePtr = nullptr;
                nextCharacterUPtr.reset();
                return;
            }
        }

        M_HP_LOG_ERR("game::player::CharacterWarehouse::Free((" << creaturePtr
            << ") name=\"" << creaturePtr->Name() << "\") not found in the warehouse.");
    }

}
}
