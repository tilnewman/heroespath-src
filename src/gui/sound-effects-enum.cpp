// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sound-effects-enum.cpp
//
#include "sound-effects-enum.hpp"

#include "misc/random.hpp"

namespace heroespath
{
namespace gui
{

    sound_effect::Enum
        sound_effect::RandomMapTransitionSfx(const MapTransition TYPE, const DoorAction ACTION)
    {
        switch (TYPE)
        {
            case MapTransition::DoorCommon:
            {
                if (ACTION == DoorAction::Open)
                {
                    return ((misc::RandomBool()) ? DoorCommonOpen1 : DoorCommonOpen2);
                }
                else
                {
                    return misc::RandomSelect(
                        SfxEnumVec_t { DoorCommonClose1, DoorCommonClose2, DoorCommonClose3 });
                }
            }
            case MapTransition::DoorOld:
            {
                if (ACTION == DoorAction::Open)
                {
                    return ((misc::RandomBool()) ? DoorOldOpen1 : DoorOldOpen2);
                }
                else
                {
                    return DoorOldClose1;
                }
            }
            case MapTransition::DoorRattly:
            {
                if (ACTION == DoorAction::Open)
                {
                    return DoorRattlyOpen1;
                }
                else
                {
                    return misc::RandomSelect(
                        SfxEnumVec_t { DoorRattlyClose1, DoorRattlyClose2, DoorRattlyClose3 });
                }
            }
            case MapTransition::DoorSquaeky:
            {
                if (ACTION == DoorAction::Open)
                {
                    return misc::RandomSelect(
                        SfxEnumVec_t { DoorSqueakyOpen1, DoorSqueakyOpen2, DoorSqueakyOpen3 });
                }
                else
                {
                    return DoorSqueakyClose1;
                }
            }
            case MapTransition::Stairs: return Stairs;
            case MapTransition::Count:
            default: return Count;
        }
    }

} // namespace gui
} // namespace heroespath
