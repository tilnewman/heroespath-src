#ifndef SFMLUTIL_GUI_CONDITIONIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_GUI_CONDITIONIMAGEMANAGER_HPP_INCLUDED
//
// condition-image-manager.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include "game/creature/condition-enum.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <memory>


namespace sfml_util
{
namespace gui
{

    //types required by the singleton implementation
    class ConditionImageManager;
    using ConditionImageManagerSPtr_t = std::shared_ptr<ConditionImageManager>;


    //Loads images and delivers sfml_util::TextureSPtr_ts to them on demand.
    class ConditionImageManager
    {
        //prevent copy construction
        ConditionImageManager(const ConditionImageManager &);

        //prevent copy assignment
        ConditionImageManager & operator=(const ConditionImageManager &);

        //prevent non-singleton construction
        ConditionImageManager();

    public:
        virtual ~ConditionImageManager();

        static ConditionImageManagerSPtr_t Instance();
        inline static void SetImagesDirectory(const std::string & S)    { conditionImagesDirectory_ = S; }
        inline static float Dimmension()                                { return 256.0f; }
        static bool Test();

        sfml_util::TextureSPtr_t Get(const game::creature::Conditions::Enum) const;

    private:
        const std::string MakeFilename(const game::creature::Conditions::Enum) const;
        const boost::filesystem::path MakeFilepath(const game::creature::Conditions::Enum) const;

    private:
        static ConditionImageManagerSPtr_t instance_;
        static std::string conditionImagesDirectory_;
        static const std::string filenameExtension_;
    };
}
}

#endif //SFMLUTIL_GUI_CONDITIONIMAGEMANAGER_HPP_INCLUDED
