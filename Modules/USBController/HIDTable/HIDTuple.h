/* Copyright © 2024 Georgy E. All rights reserved. */

#ifndef _USB_HID_TUPLE_H_
#define _USB_HID_TUPLE_H_


#include <memory>

#ifdef USE_HAL_DRIVER
#   include "bmacro.h"
#   include "variables.h"
#else
#   include "app_exception.h"
#   include "variables.h"
#endif



struct HIDTupleBase {};

template<class type_t, class getter_f = void, unsigned LENGTH = 1>
struct HIDTuple : HIDTupleBase
{
    static_assert(!std::is_same<getter_f, void>::value, "Tuple getter functor must be non void");

    type_t* target(unsigned index = 0)
    {
        type_t* value = getter_f{}();
        if (!value || index >= length()) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "Value must not be null");
        	return nullptr;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }
        return value + index;
    }

    type_t deserialize(const uint8_t* src)
    {
        if (!src) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "Source must not be null");
        	return 0;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }
        return utl::deserialize<type_t>(src)[0];
    }

    std::shared_ptr<uint8_t[]> serialize(unsigned index = 0)
    {
        if (!target() || index >= length()) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "Target must not be null");
        	return nullptr;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }
        return utl::serialize<type_t>(target(index));
    }

    constexpr unsigned size() const // TODO: find references
    {
        return sizeof(type_t) * length();
    }

    constexpr unsigned length() const
    {
        return LENGTH;
    }
};


#endif
