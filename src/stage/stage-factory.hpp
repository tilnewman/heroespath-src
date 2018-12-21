// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_STAGE_FACTORY_HPP_INCLUDED
#define HEROESPATH_STAGE_STAGE_FACTORY_HPP_INCLUDED
//
// stage-factory.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "stage/i-stage.hpp"
#include "stage/stage-enum.hpp"
#include "stage/stage-setup-packet.hpp"

namespace heroespath
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature

namespace popup
{
    class PopupInfo;
}

namespace stage
{
    // makes Stages and performs all needed setup before returning them.
    struct StageFactory
    {
        StageFactory() = delete;

        static IStageUVec_t Make(const SetupPacket & SETUP_PACKET);
        static IStageUPtr_t MakePopup(const popup::PopupInfo & POPUP_INFO);

    private:
        static IStageUVec_t CreateAll(const SetupPacket & SETUP_PACKET);
        static void SetupAll(IStageUVec_t &);
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_STAGE_FACTORY_HPP_INCLUDED
