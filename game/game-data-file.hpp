#ifndef GAME_GAMEDATAFILE_INCLUDED
#define GAME_GAMEDATAFILE_INCLUDED
//
// game-data-file.hpp
//  A singleton config file that contains game data
//
#include "configbase/configbase.hpp"
#include "utilz/assertlogandthrow.hpp"

#include <boost/type_index.hpp>//for boost::typeindex::type_id<T>().pretty_name()

#include <string>
#include <memory>


namespace game
{

    //Singleton class that manages a simple configuration file
    class GameDataFile : public appbase::ConfigBase
    {
        //prevent copy construction
        GameDataFile(const GameDataFile &) =delete;

        //prevent copy assignment
        GameDataFile & operator=(const GameDataFile &) =delete;

        //prevent non-singleton construction
        GameDataFile();

    public:
        virtual ~GameDataFile();
        static GameDataFile * Instance();
        static void Acquire();
        static void Release();

        template<typename T>
        T GetCopyDataFile(const std::string & KEY, const T DEFAULT = T()) const
        {
            T temp(DEFAULT);
            Get<T>(temp, KEY);//this throws if the KEY is not found or if the value could not be boost::lexical_cast<T>().
            return temp;
        }

        virtual const std::string GetCopyStr(const std::string & KEY, const std::string & DEFAULT = "") const;
        virtual int               GetCopyInt(const std::string & KEY, const int DEFAULT = 0) const;
        virtual std::size_t       GetCopySizet(const std::string & KEY, const std::size_t DEFAULT = 0) const;
        virtual bool              GetCopyBool(const std::string & KEY, const bool DEFAULT = false) const;
        virtual float             GetCopyFloat(const std::string & KEY, const float DEFAULT = 0.0f) const;
        virtual double            GetCopyDouble(const std::string & KEY, const double DEFAULT = 0.0) const;

        const std::string GetMediaPath(const std::string & KEY) const;
        const std::string CreateMediaPath(const std::string & PATH) const;
        const std::string GetMediaBasePathStr() const;

        bool WillRunSystemTests() const;

    private:
        static std::unique_ptr<GameDataFile> instanceUPtr_;
    };

}
#endif //GAME_GAMEDATAFILE_INCLUDED
