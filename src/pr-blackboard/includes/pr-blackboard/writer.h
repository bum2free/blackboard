
#pragma once
#include <memory>
#include <string>

#include "pr-blackboard/blackboard.h"

namespace mega {

//template <typename T> class BlackBoard;
template <typename T>
class Writer {
public:
    explicit Writer(const std::string& topic)
        : topic_(topic) {}
    virtual ~Writer() = default;
    void Write(const std::shared_ptr<T>& value){
        BlackBoard<T>::instance().setOutput(topic_, value);
    }
    void Write(const T& value){
        BlackBoard<T>::instance().setOutput(topic_, value);
    }
private:
    std::string topic_;
};

} // namespace mega