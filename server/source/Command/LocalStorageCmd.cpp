#include <Command/LocalStorageCmd.hpp>

#include <iostream>
#include <iterator>
#include <sstream>
namespace micro_service {
/* =========================================== */
/* === variables initialize =========== */
/* =========================================== */
    const std::vector <LocalStorageCmd::CommandInfo> LocalStorageCmd::gCommandInfoList{
            {"h", "help",   LocalStorageCmd::Help,           "Print help usages."},
            {"g", "get",   LocalStorageCmd::GetValue,    "Get value."},
            {"s", "set",   LocalStorageCmd::SetValue,    "Set value."},
    };
    
    inline std::string trim(const std::string &s)
    {
       auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
       auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
       return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
    }
/* =========================================== */
/* === function implement ============= */
/* =========================================== */
    int LocalStorageCmd::Do(void *context,
                         const std::string &cmd_msg,
                         std::string &errMsg) {
        std::string trim_msg = cmd_msg;
        trim_msg = trim(trim_msg);
        if (trim_msg.find('/') != 0) {
            errMsg = "not command";
            return -10000;
        }

        const std::string &cmdLine = trim_msg.substr(1);
        auto wsfront = std::find_if_not(cmdLine.begin(), cmdLine.end(),
                                        [](int c) { return std::isspace(c); });
        auto wsback = std::find_if_not(cmdLine.rbegin(), cmdLine.rend(),
                                       [](int c) { return std::isspace(c); }).base();
        auto trimCmdLine = (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));

        std::istringstream iss(trimCmdLine);
        std::vector <std::string> args{std::istream_iterator < std::string > {iss},
                                       std::istream_iterator < std::string > {}};
        if (args.size() <= 0) {
            return 0;
        }
        const auto &cmd = args[0];

        for (const auto &cmdInfo : gCommandInfoList) {
            if (cmd.compare(0, 1, cmdInfo.mCmd) != 0
                && cmd != cmdInfo.mLongCmd) {
                continue;
            }

            int ret = cmdInfo.mFunc(context, args, errMsg);
            return ret;
        }

        errMsg = "Unknown command: " + cmd;
        return -10000;
    }

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */

    int LocalStorageCmd::Help(void *context,
                           const std::vector <std::string> &args,
                           std::string &errMsg) {
        std::cout << "Usage:" << std::endl;
        std::string msg = "";

        for (const auto &cmdInfo : gCommandInfoList) {
            msg += "" + cmdInfo.mCmd + " | " + cmdInfo.mLongCmd + " : " + cmdInfo.mUsage + "\n";
        }
        auto carrier_robot = reinterpret_cast<LocalStorageService *>(context);
        carrier_robot->helpCmd(args, msg);
        return 0;
    }
    
    int LocalStorageCmd::SetValue(void *context,
                                  const std::vector <std::string> &args,
                                  std::string &errMsg) {
        auto carrier_robot = reinterpret_cast<LocalStorageService *>(context);
        carrier_robot->SetValueCmd(args);
        return 0;
    }

    int LocalStorageCmd::GetValue(void *context,
                                  const std::vector <std::string> &args,
                                  std::string &errMsg) {
        auto carrier_robot = reinterpret_cast<LocalStorageService *>(context);
        carrier_robot->GetValueCmd(args);
        return 0;
    }
}
