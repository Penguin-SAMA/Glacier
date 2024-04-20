#pragma once

#include <memory>

namespace Glacier {

namespace {

template <class T, class X, int N>
T& getInstanceX() {
    static T v;
    return v;
}

template <class T, class X, int N>
std::shared_ptr<T> getInstancePtr() {
    static std::shared_ptr<T> v(new T);
    return v;
}
} // namespace

// 单例模式封装类
template <class T, class X = void, int N = 0>
class Singleton
{
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

// 单例模式智能指针封装类
template <class T, class X = void, int N = 0>
class SingletonPtr
{
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

} // namespace Glacier
