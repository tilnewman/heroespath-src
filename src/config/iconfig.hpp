// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CONFIG_ICONFIG_HPP_INCLUDED
#define HEROESPATH_CONFIG_ICONFIG_HPP_INCLUDED
//
// iconfig.hpp
//
#include <string>

namespace heroespath
{
namespace config
{

    //
    // IConfigBase Class
    //  Usage interface
    //
    class IConfigBase
    {
    public:
        virtual ~IConfigBase() = default;
        //
        virtual bool Load() = 0;

        // Pass a string to override the default which uses the member fileNameStr.
        virtual bool Save(const std::string & FILENAME = "") const = 0;
        //
        virtual const std::string
            GetWithDefaultStr(const std::string & KEY, const std::string & DEFAULT = "") const = 0;
        virtual int GetWithDefaultInt(const std::string & KEY, const int DEFAULT = 0) const = 0;
        virtual size_t
            GetWithDefaultSizet(const std::string & KEY, const size_t DEFAULT = 0) const = 0;
        virtual bool
            GetWithDefaultBool(const std::string & KEY, const bool DEFAULT = false) const = 0;
        virtual float
            GetWithDefaultFloat(const std::string & KEY, const float DEFAULT = 0.0f) const = 0;
        virtual double
            GetWithDefaultDouble(const std::string & KEY, const double DEFAULT = 0.0) const = 0;

        // Sets val to value stored at KEY.
        // If KEY does not exist or cannot be cast to the desired type, then val is not changed and
        // an exception is thrown.
        virtual void GetStr(std::string & val, const std::string & KEY) const = 0;
        virtual void GetInt(int & val, const std::string & KEY) const = 0;
        virtual void GetSizet(size_t & val, const std::string & KEY) const = 0;
        virtual void GetBool(bool & val, const std::string & KEY) const = 0;
        virtual void GetFloat(float & val, const std::string & KEY) const = 0;
        virtual void GetDouble(double & val, const std::string & KEY) const = 0;

        virtual const std::string GetCopyStr(const std::string & KEY) const = 0;
        virtual int GetCopyInt(const std::string & KEY) const = 0;
        virtual std::size_t GetCopySizet(const std::string & KEY) const = 0;
        virtual float GetCopyFloat(const std::string & KEY) const = 0;
        virtual double GetCopyDouble(const std::string & KEY) const = 0;
        virtual bool GetCopyBool(const std::string & KEY) const = 0;

        // Don't bother with a templated version of these functions,
        // becuase this interface class is required.
        virtual void SetStr(const std::string & KEY, const std::string & VALUE) = 0;
        virtual void SetInt(const std::string & KEY, const int VALUE) = 0;
        virtual void SetSizet(const std::string & KEY, const size_t VALUE) = 0;
        virtual void SetBool(const std::string & KEY, const bool VALUE) = 0;
        virtual void SetFloat(const std::string & KEY, const float VALUE) = 0;
        virtual void SetDouble(const std::string & KEY, const double VALUE) = 0;

        virtual void Dump(std::ostream &) = 0;

        virtual const std::string GetFileNameStr() const = 0;
        virtual const std::string GetFileNameFullPathStr() const = 0;
        virtual const std::string GetSeparatorStr() const = 0;
        virtual const std::string GetCommentStr() const = 0;
    };
} // namespace config
} // namespace heroespath

#endif // HEROESPATH_CONFIG_ICONFIG_HPP_INCLUDED
