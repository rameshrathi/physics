//
// Created by ramesh on 19/05/25.
//

#pragma once

#ifdef ALWAYS_INLINE
#    undef ALWAYS_INLINE
#endif
#define ALWAYS_INLINE __attribute__((always_inline)) inline