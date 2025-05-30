//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <RK/Types.h>
#include <RK/Atomic.h>

template<typename T>
class AtomicRefCounted : public Atomic<T> {
public:
    ~AtomicRefCounted() = default;

    explicit AtomicRefCounted(T value) : Atomic<T>(value) {};

    UInt32 refCount() const noexcept { return _ref_count; }

private:
    UInt32 _ref_count = 0;
};
