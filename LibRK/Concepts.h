//
// Created by ramesh on 19/05/25.
//

#pragma once

#include "Traits.h"

template<typename T, typename U>
concept SameAs = IsSame<T, U>;