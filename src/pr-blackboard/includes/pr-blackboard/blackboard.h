#ifndef __MEGA_BLACKBOARD_H__
#define __MEGA_BLACKBOARD_H__

#include "behaviortree_cpp_v3/bt_factory.h"

namespace mega{
template <typename T>
class BlackBoard{
public:
    static BlackBoard& instance() {
        static BlackBoard instance;
        return instance;
    }

    BlackBoard(const BlackBoard&) = delete;
    BlackBoard& operator=(const BlackBoard&) = delete;

    void setOutput(const std::string& key, const T& value) {
        blackboard_->set<T>(key, value);
    }

    void setOutput(const std::string& key, const std::shared_ptr<T>& value){
        blackboard_->set<std::shared_ptr<T>>(key, value);
    }


    // 支持对象
    bool getInput(const std::string& key, T& value){
        return blackboard_->get<T>(key,value);
    }

    // 支持 shared_ptr
    bool getInput(const std::string& key, std::shared_ptr<T>& value){
        return blackboard_->get<std::shared_ptr<T>>(key,value);
    }

    BT::Blackboard::Ptr raw(){
        return blackboard_;
    }

private:
    BlackBoard(){
        blackboard_ = BT::Blackboard::create();
    }

    ~BlackBoard() = default;

    BT::Blackboard::Ptr blackboard_;
};

} // namespace mega
#endif