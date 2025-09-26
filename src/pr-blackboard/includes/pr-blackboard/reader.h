
#pragma once
#include <memory>
#include <string>
#include "pr-blackboard/blackboard.h"

namespace mega {
template <typename T> class BlackBoard;
template <typename T>
class Reader {
public:
    Reader(const std::string& topic)
        : topic_(topic) {}
    virtual ~Reader() = default;
    bool Read(std::shared_ptr<T>& value){
        return BlackBoard<T>::instance().getInput(topic_, value);
    }
    bool Read(T& value){
        return BlackBoard<T>::instance().getInput(topic_, value);
    }
private:
    std::string topic_;
};
} // namespace mega