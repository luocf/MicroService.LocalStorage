//
// Created by luocf on 2019/6/13.
//

#ifndef LOCAL_STORAGE_SERVICE_H
#define LOCAL_STORAGE_SERVICE_H

#include <stdlib.h>
#include <functional>
#include <memory> // std::unique_ptr
#include <ctime>
#include <thread>
#include <regex>
#include "DataBase/DatabaseProxy.h"
#include "Common/FileUtils.hpp"
#include "PeerNodeSDK/Connector.h"
#include "PeerNodeSDK/PeerListener.h"
#include "PeerNodeSDK/PeerNode.h"
#include "Elastos.SDK.Contact/Contact.hpp"
#include "Elastos.SDK.Contact/ContactListener.hpp"

using namespace elastos;

namespace micro_service {
    static const char *LocalStorageService_TAG = "LocalStorageService";
    class LocalStorageService:public std::enable_shared_from_this<LocalStorageService>{
    public:
        LocalStorageService(const std::string& path);
        ~LocalStorageService();
        int acceptFriend(const std::string& friendid);
        void receiveMessage(const std::string& friend_id, const std::string& message, std::time_t send_time);
        void helpCmd(const std::vector<std::string> &args, const std::string& message);
        void SetValueCmd(const std::vector<std::string> &args);
        void GetValueCmd(const std::vector<std::string> &args);
        std::time_t getTimeStamp();
        std::string mOwnerHumanCode;
    protected:
        std::string mPath;

    private:

        void getAddressFromFile(std::string& address);
        Connector* mConnector;
        DatabaseProxy* mDatabaseProxy;
        int Start();
        int Stop();
    };

    class LocalStorageMessageListener :public PeerListener::MessageListener{
    public:
        LocalStorageMessageListener( LocalStorageService* chatGroupService);
        ~LocalStorageMessageListener();
        void onEvent(ContactListener::EventArgs& event) override ;
        void onReceivedMessage(const std::string& humanCode, ContactChannel channelType,
                               std::shared_ptr<ElaphantContact::Message> msgInfo) override;
    private:
        LocalStorageService*mLocalStorageService;
    };

    extern "C" {
        micro_service::LocalStorageService* CreateService(const char* path);
        void DestroyService(micro_service::LocalStorageService* service);
    }
}

#endif //LOCAL_STORAGE_SERVICE_H
