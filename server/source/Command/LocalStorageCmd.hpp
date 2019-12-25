#ifndef _CHAt_GROUP_CMD_HPP_
#define _CHAt_GROUP_CMD_HPP_

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "LocalStorageService.h"
namespace micro_service {
    class LocalStorageCmd {
    public:
        /*** type define ***/

        /*** static function and variable ***/
        static int Do(void* context,
                      const std::string& cmdLine,
                      std::string& errMsg);

        /*** class function and variable ***/

    protected:
        /*** type define ***/

        /*** static function and variable ***/

        /*** class function and variable ***/

    private:
        /*** type define ***/
        struct CommandInfo {
            std::string mCmd;
            std::string mLongCmd;
            std::function<int(void* context, const std::vector<std::string>&, std::string&)> mFunc;
            std::string mUsage;
        };

        /*** static function and variable ***/
        static int Help(void* context,
                        const std::vector<std::string>& args,
                        std::string& errMsg);
        static int GetValue(void* context,
                               const std::vector<std::string>& args,
                               std::string& errMsg);
        static int SetValue(void* context,
                            const std::vector<std::string>& args,
                            std::string& errMsg);
        static const std::vector<CommandInfo> gCommandInfoList;

        /*** class function and variable ***/
        explicit LocalStorageCmd() = delete;
        virtual ~LocalStorageCmd() = delete;
    };
}
/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

#endif /* _CHAt_GROUP_CMD_HPP_ */

