//
// spell-image-manager.cpp
//
#include "spell-image-manager.hpp"

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

    SpellImageManagerSPtr_t SpellImageManager::instance_;
    std::string             SpellImageManager::spellImagesDirectory_("");
    const std::string       SpellImageManager::filenameExtension_   (".gif");


    SpellImageManager::SpellImageManager()
    {}


    SpellImageManager::~SpellImageManager()
    {}


    SpellImageManagerSPtr_t SpellImageManager::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new SpellImageManager );

        return instance_;
    }


    bool SpellImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::SpellImageManager::Test() Starting Tests...");
        }

        SpellImageManagerSPtr_t simSPtr{ SpellImageManager::Instance() };

        static auto spellIndex{ 0 };
        if (spellIndex < game::spell::Spells::Count)
        {
            auto const ENUM{ static_cast<game::spell::Spells::Enum>(spellIndex) };
            auto const ENUM_STR{ game::spell::Spells::ToString(ENUM) };
            auto const TEXTURE_SPTR{ simSPtr->Get(ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::SpellImageManager::Test()  Get(\"" << ENUM_STR << "\") returned a nullptr.");
            game::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            game::LoopManager::Instance()->TestingStrAppend("SpellImageManager Tested " + ENUM_STR);
            ++spellIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::SpellImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    sfml_util::TextureSPtr_t SpellImageManager::Get(const game::spell::Spells::Enum ENUM) const
    {
        sfml_util::TextureSPtr_t textureSPtr{ nullptr };
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, MakeFilepath(ENUM).string());
        return textureSPtr;
    }


    const std::string SpellImageManager::MakeFilename(const game::spell::Spells::Enum ENUM) const
    {
        return boost::algorithm::to_lower_copy(game::spell::Spells::ToString(ENUM)) + filenameExtension_;
    }


    const boost::filesystem::path SpellImageManager::MakeFilepath(const game::spell::Spells::Enum ENUM) const
    {
        namespace bfs = boost::filesystem;
        return bfs::system_complete( bfs::path(spellImagesDirectory_) / bfs::path(MakeFilename(ENUM)) );
    }

}
}
