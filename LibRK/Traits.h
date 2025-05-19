//
// Created by ramesh on 19/05/25.
//

#pragma once

template<class T>
constexpr bool IsLvalueReference = false;

template<class T>
constexpr bool IsLvalueReference<T&> = true;

template<typename T, typename U>
constexpr bool IsSame = false;

template<typename T>
constexpr bool IsSame<T, T> = true;