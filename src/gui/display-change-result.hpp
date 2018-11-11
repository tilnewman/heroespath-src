// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_DISPLAY_CHANGE_RESULT_HPP_INCLUDED
#define HEROESPATH_GUI_DISPLAY_CHANGE_RESULT_HPP_INCLUDED
//
// display-change-enum.hpp
//

namespace heroespath
{
namespace gui
{

    struct DisplayChangeResult
    {
        bool Success() const { return (is_res_intended && is_aa_intended); }

        bool SuccessAndChanged() const { return (Success() && Changed()); }
        bool SuccessButIgnored() const { return (Success() && !Changed()); }

        bool Changed() const { return (did_res_change || did_aa_change); }

        bool FailedButReverted() const { return (!Success() && !Changed()); }
        bool FailedButOnlyAA() const { return (!Success() && is_res_intended && !is_aa_intended); }
        bool FailedAndRevertFailed() const { return (!Success() && Changed()); }

        bool did_res_change = false;
        bool did_aa_change = false;
        bool is_res_intended = false;
        bool is_aa_intended = false;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_DISPLAY_CHANGE_RESULT_HPP_INCLUDED
