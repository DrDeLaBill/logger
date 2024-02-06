/* Copyright © 2024 Georgy E. All rights reserved. */

#ifndef _USB_HID_TUPLE_H_
#define _USB_HID_TUPLE_H_


#include <memory>

#include "Modules/Exceptions/app_exception.h"
#include "Modules/Utils/utils/Utils/variables.h"


struct HIDTupleBase {};

template<class type_t, class getter_f = void>
struct HIDTuple : HIDTupleBase
{
    static_assert(!std::is_same<getter_f, void>::value, "Tuple getter functor must be non void");

    type_t* target(unsigned index = 0)
    {
        type_t* value = getter_f{}();
        if (!value) {
            throw new exceptions::TemplateErrorException();
        }
        return value + index;
    }

    type_t deserialize(const uint8_t* src)
    {
        if (!src) {
            throw new exceptions::TemplateErrorException();
        }
        return utl::deserialize<type_t>(src)[0];
    }

    std::shared_ptr<uint8_t[]> serialize(unsigned index = 0)
    {
        if (!target()) {
            throw new exceptions::TemplateErrorException();
        }
        return utl::serialize<type_t>(target(index));
    }
};


#endif
