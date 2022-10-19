#pragma once

namespace eva01 {

template <typename T>
class Singleton {
public:
    static T* Get() {
        static T v;
        return &v;
    }
};
}
