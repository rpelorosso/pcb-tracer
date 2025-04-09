#ifndef COMMUNICATION_HUB_H
#define COMMUNICATION_HUB_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <any>

enum class HubEvent {
    NODE_MADE_SINGLE_LINK,
    NODE_MADE_MULTIPLE_LINKS,
    NODE_DELETED,
    COMPONENT_CREATED,
    COMPONENT_DELETED,
    NOTE_CREATED,
    NOTE_DELETED,
    SCENE_CLEAN
};

class CommunicationHub {
    public:
        using Callback = std::function<void(HubEvent, const void*)>;
    
        static CommunicationHub& instance() {
            static CommunicationHub instance;
            return instance;
        }
    
        void subscribe(HubEvent event, Callback callback) {
            subscribers[event].push_back(std::move(callback));
        }
    
        void publish(HubEvent event, const void* data) {
            auto it = subscribers.find(event);
            if (it != subscribers.end()) {
                for (auto& callback : it->second) {
                    callback(event, data);
                }
            }
        }
    
    private:
        CommunicationHub() = default;
        std::unordered_map<HubEvent, std::vector<Callback>> subscribers;
    };

#endif // COMMUNICATION_HUB_H