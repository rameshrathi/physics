//
// Created by ramesh on 29/05/25.
//

#pragma once

namespace LIB
{
    template<typename T>
    static inline T atomic_exchange(T volatile* var, T desired, MemoryOrder order = memory_order_seq_cst) noexcept
    {
        return __atomic_exchange_n(var, desired, order);
    }

    template<typename T, typename V = RemoveVolatile<T>>
    static inline V* atomic_exchange(T volatile** var, V* desired, MemoryOrder order = memory_order_seq_cst) noexcept
    {
        return __atomic_exchange_n(var, desired, order);
    }

    template<typename T, typename V = RemoveVolatile<T>>
    static inline V* atomic_exchange(T volatile** var, nullptr_t, MemoryOrder order = memory_order_seq_cst) noexcept
    {
        return __atomic_exchange_n(const_cast<V**>(var), nullptr, order);
    }
} // Lib

template<typename T>
class AtomicRefCount {
public:
    AtomicRefCount() = default;
    ~AtomicRefCount() = default;

private:
};