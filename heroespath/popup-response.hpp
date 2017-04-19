#ifndef APPBASE_SFMLUTIL_POPUPRESPONSE_INCLUDED
#define APPBASE_SFMLUTIL_POPUPRESPONSE_INCLUDED
//
// popup-response.hpp
//  Encapsulates everything about a popup window's result with a response from the user
//
#include "sfml-util/response-enum.hpp"

#include "heroespath/popup-info.hpp"


namespace heroespath
{
namespace callback
{

    //encapsulates everything about a popup window's response
    class PopupResponse
    {
    public:
        PopupResponse(const heroespath::PopupInfo &   POPUP_INFO,
                      const sfml_util::Response::Enum RESPONSE,
                      const std::size_t               SELECTION)
        :
            info_     (POPUP_INFO),
            response_ (RESPONSE),
            selection_(SELECTION)
        {}

        virtual ~PopupResponse() {}

        inline const PopupInfo Info() const                 { return info_; }
        inline sfml_util::Response::Enum Response() const   { return response_; }
        inline std::size_t Selection() const                { return selection_; }

        friend bool operator==(const PopupResponse & L, const PopupResponse & R);

    private:
        PopupInfo info_;
        sfml_util::Response::Enum response_;
        std::size_t selection_;
    };


    inline bool operator==(const PopupResponse & L, const PopupResponse & R)
    {
        return ((L.info_ == R.info_) &&
                (L.response_ == R.response_) &&
                (L.selection_ == R.selection_));
    }


    inline bool operator!=(const PopupResponse & L, const PopupResponse & R)
    {
        return ! (L == R);
    }

}
}
#endif //APPBASE_SFMLUTIL_POPUPRESPONSE_INCLUDED
