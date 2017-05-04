//
// game-data-file.cpp
//
#include "game-data-file.hpp"

#include "utilz/platform.hpp"
#include "utilz/boost-string-includes.hpp"

#include "game/log-macros.hpp"


namespace game
{

    GameDataFileSPtr_t GameDataFile::instanceSPtr_;


    GameDataFile::GameDataFile()
    :
        ConfigBase("game-data.txt", "=", "#")
    {
        Load();
    }


    GameDataFile::~GameDataFile()
    {}


    GameDataFileSPtr_t GameDataFile::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new GameDataFile() );

        return instanceSPtr_;
    }


    const std::string GameDataFile::GetCopyStr(const std::string & KEY, const std::string & DEFAULT) const
    {
        std::string temp(DEFAULT);
        GetStr(temp, KEY);//throws if KEY is not found
        return temp;
    }


    int GameDataFile::GetCopyInt(const std::string & KEY, const int DEFAULT) const
    {
        return GetCopyDataFile<int>(KEY, DEFAULT);
    }


    std::size_t GameDataFile::GetCopySizet(const std::string & KEY, const std::size_t DEFAULT) const
    {
        return GetCopyDataFile<std::size_t>(KEY, DEFAULT);
    }


    bool GameDataFile::GetCopyBool(const std::string & KEY, const bool DEFAULT) const
    {
        //handle case where file contains true/false or yes/no, instead of the number
        namespace ba = boost::algorithm;
        const std::string STR_VERSION( ba::trim_copy(ba::to_lower_copy(GetCopyStr(KEY, ""))) );
        if ((STR_VERSION == "true") || (STR_VERSION == "yes"))
            return true;
        else
            if ((STR_VERSION == "false") || (STR_VERSION == "no"))
                return false;

        return GetCopyDataFile<bool>(KEY, DEFAULT);
    }


    float GameDataFile::GetCopyFloat(const std::string & KEY, const float DEFAULT) const
    {
        return GetCopyDataFile<float>(KEY, DEFAULT);
    }


    double GameDataFile::GetCopyDouble(const std::string & KEY, const double DEFAULT) const
    {
        return GetCopyDataFile<double>(KEY, DEFAULT);
    }


    const std::string GameDataFile::GetMediaPath(const std::string & KEY) const
    {
        const std::string PATH(GetCopyStr(KEY, ""));
        M_ASSERT_OR_LOGANDTHROW_SS((PATH.empty() == false), "GameDataFile::GetMediaPath(\"" << KEY << "\") failed to find that key in the config file.");

        std::ostringstream ss;
        ss << GetMediaBasePathStr() << PATH;
        return ss.str();
    }


    const std::string GameDataFile::CreateMediaPath(const std::string & PATH) const
    {
        std::ostringstream ss;
        ss << GetMediaBasePathStr() << PATH;
        return ss.str();
    }


    const std::string GameDataFile::GetMediaBasePathStr() const
    {
        const std::string MEDIA_BASE_PATH_KEY_STR((utilz::Platform::Instance()->IsWindows()) ? "system-media-dir-win" : "system-media-dir-linux");
        const std::string MEDIA_BASE_PATH_STR( GetCopyStr(MEDIA_BASE_PATH_KEY_STR, "") );
        M_ASSERT_OR_LOGANDTHROW_SS((MEDIA_BASE_PATH_STR.empty() == false), "GameDataFile::GetMediaBasePathStr(key=\"" << MEDIA_BASE_PATH_KEY_STR << "\") failed to find that key in the config file.");
        return MEDIA_BASE_PATH_STR;
    }


    bool GameDataFile::WillRunSystemTests() const
    {
        return GetCopyBool("system-will-execute-tests", false);
    }

}
