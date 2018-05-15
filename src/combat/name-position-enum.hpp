// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_NAMEPOSITIONENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_NAMEPOSITIONENUM_HPP_INCLUDED
//
// name-position-enum.hpp
//
#include <string>
#include <tuple>

namespace heroespath
{
namespace combat
{

    struct NamePosition
    {
        enum Enum
        {
            NoName = 0,
            SourceBefore,
            SourceAfter,
            SourceThenTarget,
            TargetBefore,
            TargetAfter,
            TargetThenSource,
            Count
        };

        static const std::string ToString(const Enum);
    };

    class ContentAndNamePos
    {
    public:
        explicit ContentAndNamePos(
            const std::string & PRE_STR = "",
            const std::string & CONTENT_STR = "",
            const std::string & POST_STR = "",
            const NamePosition::Enum NAME_POS_ENUM = NamePosition::NoName);

        ContentAndNamePos(const std::string & CONTENT_STR, const NamePosition::Enum NAME_POS_ENUM);

        const std::string Pre() const { return pre_; }
        const std::string Content() const { return content_; }
        const std::string Post() const { return post_; }
        NamePosition::Enum NamePos() const { return posEnum_; }

        const std::string
            Compose(const std::string & SOURCE_NAME, const std::string & TARGET_NAME) const;

        friend bool operator==(const ContentAndNamePos & L, const ContentAndNamePos & R);
        friend bool operator<(const ContentAndNamePos & L, const ContentAndNamePos & R);

    private:
        std::string pre_;
        std::string content_;
        std::string post_;
        NamePosition::Enum posEnum_;
    };

    inline bool operator==(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return std::tie(L.pre_, L.content_, L.post_, L.posEnum_)
            == std::tie(R.pre_, R.content_, R.post_, R.posEnum_);
    }

    inline bool operator!=(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return !(L == R);
    }

    inline bool operator<(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return std::tie(L.pre_, L.content_, L.post_, L.posEnum_)
            < std::tie(R.pre_, R.content_, R.post_, R.posEnum_);
    }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_NAMEPOSITIONENUM_HPP_INCLUDED
