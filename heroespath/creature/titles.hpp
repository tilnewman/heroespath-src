#ifndef HEROESPATH_CREATURE_TITLE_TITLES_HPP_INCLUDED
#define HEROESPATH_CREATURE_TITLE_TITLES_HPP_INCLUDED
//
// titles.hpp
//
#include "heroespath/creature/title-enum.hpp"

#include <memory>
#include <vector>


namespace heroespath
{
namespace creature
{
    //forward declarations
    class Title;
    using TitlePtr_t = Title *;
    using TitleCPtr_t = const Title *;
    using TitlePtrC_t = Title * const;
    using TItleCPtrC_t = const Title * const;
    using TitleUPtr_t = std::unique_ptr<Title>;
    using TitleUVec_t = std::vector<TitleUPtr_t>;

namespace title
{

    class TitleWarehouse
    {
    public:
        static void Setup();
        static TitlePtr_t Get(const title::Enum);
        static bool Test();

    private:
        static TitleUVec_t titleUPtrVec_;
    };
}
}
}

#endif //HEROESPATH_CREATURE_TITLE_TITLES_HPP_INCLUDED
