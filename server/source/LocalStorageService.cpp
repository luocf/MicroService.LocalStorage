//
// Created by luocf on 2019/6/13.
//
#include <cstring>
#include <future>
#include <stack>
#include <iostream>
#include <ctime>

using namespace std;
#include <thread>
#include <chrono>
#include <Tools/Log.hpp>
#include "Json.hpp"
#include <ThirdParty/CompatibleFileSystem.hpp>
#include "Command/LocalStorageCmd.hpp"
#include "LocalStorageService.h"
#include "ErrCode.h"
namespace micro_service {
    /***********************************************/
    /***** static function implement ***************/
    /***********************************************/
    LocalStorageService::LocalStorageService(const std::string& path)
            : mPath(path){
        mConnector = new Connector(LocalStorageService_TAG);
        mDatabaseProxy = new DatabaseProxy();
        mDatabaseProxy->startDb(path.c_str());
        this->Start();
    }
    
    LocalStorageService::~LocalStorageService() {
        this->Stop();
    }

    int LocalStorageService::Start() {
        if (mConnector == NULL) return -1;
        printf("Service start!\n");
        std::shared_ptr<PeerListener::MessageListener> message_listener = std::make_shared<LocalStorageMessageListener>(this);
        mConnector->SetMessageListener(message_listener);
        int status = PeerNode::GetInstance()->GetStatus();
        printf("LocalStorageService Start status: %d\n",status);
        std::shared_ptr<ElaphantContact::UserInfo> user_info = mConnector->GetUserInfo();
        if (user_info.get() != NULL) {
            user_info->getHumanCode(mOwnerHumanCode);
            printf("Service start mOwnerHumanCode:%s\n", mOwnerHumanCode.c_str());
        }
        return 0;
    }

    int LocalStorageService::Stop() {
        mDatabaseProxy->closeDb();
        if (mConnector == NULL) return -1;
        printf("Service stop!\n");
        return 0;
    }
    std::time_t LocalStorageService::getTimeStamp() {
        return time(0);
    }

    void LocalStorageService::receiveMessage(const std::string& friend_id, const std::string& message, std::time_t send_time) {
        std::string errMsg;
        std::string msg = message;
        std::string pre_cmd = msg + " " + friend_id;//Pretreatment cmd
        LocalStorageCmd::Do(this, pre_cmd, errMsg);
    }

    int LocalStorageService::acceptFriend(const std::string& friendid) {
        mConnector->AcceptFriend(friendid);
        mDatabaseProxy->createTable(friendid);
        return 0;
    }

    void LocalStorageService::helpCmd(const std::vector<std::string> &args, const std::string &message) {
        if (args.size() >= 2) {
            const std::string friend_id = args[1];
            Json respJson;
            respJson["serviceName"] = LocalStorageService_TAG;
            respJson["type"] = "textMsg";
            respJson["content"] = message;
            int ret = mConnector->SendMessage(friend_id, respJson.dump());
            if (ret != 0) {
                Log::I(LocalStorageService_TAG,
                       "helpCmd .c_str(): %s errno:(0x%x)",
                       message.c_str());
            }
        }
    }

    void LocalStorageService::SetValueCmd(const std::vector<std::string> &args) {
        if (args.size() >= 4) {
            const std::string friend_id = args[3];
            const std::string key = args[1];
            const std::string value = args[2];

            mDatabaseProxy->setValue(friend_id,key,value, getTimeStamp());
            Json respJson;
            respJson["serviceName"] = LocalStorageService_TAG;
            respJson["type"] = "textMsg";
            Json key_value;
            key_value["cmd"] = "set";
            key_value["key"] = key;
            key_value["value"] = value;
            respJson["content"] = key_value;
            int ret = mConnector->SendMessage(friend_id, respJson.dump());
            if (ret != 0) {
                Log::I(LocalStorageService_TAG,
                       "SetValueCmd key.c_str(): %s, value.c_str(): %s errno:(0x%x)",
                       key.c_str(),value.c_str());
            }
        }
    }

    void LocalStorageService::GetValueCmd(const std::vector<std::string> &args) {
        if (args.size() >= 3) {
            const std::string friend_id = args[2];
            const std::string key = args[1];
            std::string value = mDatabaseProxy->getValue(friend_id, key);
            Json respJson;
            respJson["serviceName"] = LocalStorageService_TAG;
            respJson["type"] = "textMsg";
            Json key_value;
            key_value["cmd"] = "get";
            key_value["key"] = key;
            key_value["value"] = value;
            respJson["content"] = key_value;
            int ret = mConnector->SendMessage(friend_id, respJson.dump());
            if (ret != 0) {
                Log::I(LocalStorageService_TAG,
                       "GetValueCmd key.c_str(): %s errno:(0x%x)",
                       key.c_str());
            }
        }
    }

    LocalStorageMessageListener::LocalStorageMessageListener(LocalStorageService* service) {
        mLocalStorageService = service;
    }
    LocalStorageMessageListener::~LocalStorageMessageListener() {

    }
    void LocalStorageMessageListener::onEvent(ContactListener::EventArgs& event) {
        Log::W(LocalStorageService_TAG, "onEvent type: %d\n", event.type);
        switch (event.type) {
            case ElaphantContact::Listener::EventType::FriendRequest: {
                auto friendEvent = dynamic_cast<ElaphantContact::Listener::RequestEvent*>(&event);
                Log::W(LocalStorageService_TAG, "FriendRequest from: %s\n", friendEvent->humanCode.c_str());
                mLocalStorageService->acceptFriend(friendEvent->humanCode);
                break;
            }
            case ElaphantContact::Listener::EventType::StatusChanged: {
                auto statusEvent = dynamic_cast<ElaphantContact::Listener::StatusEvent*>(&event);
                Log::I(LocalStorageService_TAG, "StatusChanged from: %s, statusEvent->status:%d\n", statusEvent->humanCode.c_str(), static_cast<int>(statusEvent->status));
                break;
            }
            case ElaphantContact::Listener::EventType::HumanInfoChanged:{
                auto infoEvent = dynamic_cast<ElaphantContact::Listener::InfoEvent*>(&event);
                Log::I(LocalStorageService_TAG, "HumanInfoChanged from: %s\n", infoEvent->humanCode.c_str());
                break;
            }
            default: {
                break;
            }
        }
    };

    void LocalStorageMessageListener::onReceivedMessage(const std::string& humanCode, ContactChannel channelType,
                                                     std::shared_ptr<ElaphantContact::Message> msgInfo) {

        auto text_data = dynamic_cast<ElaphantContact::Message::TextData*>(msgInfo->data.get());
        std::string content = text_data->toString();
        try {
            Json json = Json::parse(content);
            std::string msg_content = json["content"];
            printf("LocalStorageMessageListener onReceivedMessage humanCode: %s,msg_content:%s \n", humanCode.c_str(), msg_content.c_str());
            mLocalStorageService->receiveMessage(humanCode,
                                          msg_content, mLocalStorageService->getTimeStamp());
        } catch (const std::exception& e) {
            printf("LocalStorageMessageListener parse json failed\n");
        }
    }

    extern "C" {
    micro_service::LocalStorageService* CreateService(const char* path) {
        return new micro_service::LocalStorageService(path);
    }
    void DestroyService(micro_service::LocalStorageService* service) {
        if (service) {
            delete service;
        }
    }
    }
}