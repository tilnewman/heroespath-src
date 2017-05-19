//
// condition-image-manager.cpp
//
#include "condition-image-manager.hpp"

#include "sfml-util/loaders.hpp"
#include "utilz/boost-string-includes.hpp"

#include "game/log-macros.hpp"
#include "utilz/assertlogandthrow.hpp"
#include "game/loop-manager.hpp"

#include <boost/filesystem.hpp>


namespace sfml_util
{
namespace gui
{

    ConditionImageManagerSPtr_t ConditionImageManager::instance_;
    std::string                 ConditionImageManager::conditionImagesDirectory_("");
    const std::string           ConditionImageManager::filenameExtension_       (".png");


    ConditionImageManager::ConditionImageManager()
    {}


    ConditionImageManager::~ConditionImageManager()
    {}


    ConditionImageManagerSPtr_t ConditionImageManager::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new ConditionImageManager );

        return instance_;
    }


    bool ConditionImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ConditionImageManager::Test()  Starting Tests...");
        }

        ConditionImageManagerSPtr_t cimSPtr{ ConditionImageManager::Instance() };

        static auto condIndex{ 0 };
        if (condIndex < game::creature::Conditions::Count)
        {
            auto const ENUM{ static_cast<game::creature::Conditions::Enum>(condIndex) };
            auto const ENUM_STR{ game::creature::Conditions::ToString(ENUM) };
            auto const TEXTURE_SPTR{ cimSPtr->Get(ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ConditionImageManager::Test()  Get(\"" << ENUM_STR << "\") returned a nullptr.");
            game::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            game::LoopManager::Instance()->TestingStrAppend("ConditionImageManager Tested " + ENUM_STR);
            ++condIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ConditionImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    sfml_util::TextureSPtr_t ConditionImageManager::Get(const game::creature::Conditions::Enum ENUM) const
    {
        sfml_util::TextureSPtr_t textureSPtr{ nullptr };
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, MakeFilepath(ENUM).string());
        return textureSPtr;
    }


    const std::string ConditionImageManager::MakeFilename(const game::creature::Conditions::Enum ENUM) const
    {
        return boost::algorithm::to_lower_copy(game::creature::Conditions::ToString(ENUM)) + filenameExtension_;
    }


    const boost::filesystem::path ConditionImageManager::MakeFilepath(const game::creature::Conditions::Enum ENUM) const
    {
        namespace bfs = boost::filesystem;
        return bfs::system_complete( bfs::path(conditionImagesDirectory_) / bfs::path(MakeFilename(ENUM)) );
    }

}
}
