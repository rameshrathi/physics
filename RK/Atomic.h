//
// Created by ramesh on 30/05/25.
//

#pragma once

#include <atomic>

template <class T>
class Atomic
{
public:
    ~Atomic() = default;

    explicit Atomic(T value) : _value(value) {}
private:
    std::atomic<T> _value;
};
