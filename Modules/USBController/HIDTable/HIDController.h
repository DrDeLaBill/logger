/* Copyright © 2024 Georgy E. All rights reserved. */

#ifndef HIDCONTROLLER_H
#define HIDCONTROLLER_H


#include <variant>
#include <cstring>
#include <unordered_map>

#include "hid_defs.h"
#include "HIDTable.h"
#include "HIDTuple.h"
#ifdef USE_HAL_DRIVER
#include "bmacro.h"
#else
#include "app_exception.h"
#endif


template<class Table>
struct HIDController
{
private:
    static_assert(std::is_base_of<HIDTableBase, Table>::value, "Template class must be HIDTable");

    using tuple_p = typename Table::tuple_p;
    using tuple_v = typename Table::tuple_v;

    using tuple_t = std::unordered_map<
        uint16_t,
        tuple_v
    >;

    uint16_t currID;

    void details(const uint16_t key, const uint8_t index)
    {
#ifdef USE_HAL_DRIVER
        gprint("Details: key = %u; index = %u; max key = %lu\n", key, index, maxKey());
#endif
    }

    template<class... TuplePacks>
    void set_table(utl::simple_list_t<TuplePacks...>)
    {
        (set_tuple(utl::getType<TuplePacks>{}), ...);
        currID--;
    }

    template<class TuplePack>
    void set_tuple(utl::getType<TuplePack> tuplePack)
    {
        using tuple_t = typename decltype(tuplePack)::TYPE;

        characteristics.insert({currID++, tuple_t{}});
    }

public:
    tuple_t characteristics;

    HIDController(const uint16_t startKey = HID_FIRST_KEY)
    {
        currID = startKey;
        set_table(tuple_p{});
    }

    void setValue(const uint16_t key, const uint8_t* value, const uint8_t index = 0)
    {
        auto it = characteristics.find(key);
        if (it == characteristics.end()) {
#ifdef USE_HAL_DRIVER
        	BEDUG_ASSERT(false, "HID table not found error");
        	details(key, index);
        	return;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }

        auto lambda = [&] (auto& tuple) {
            tuple.set(tuple.deserialize(value), index);
        };

        std::visit(lambda, it->second);
    }

    void getValue(const uint16_t key, uint8_t* dst, const uint8_t index = 0)
    {
    	auto it = characteristics.find(key);
        if (it == characteristics.end()) {
#ifdef USE_HAL_DRIVER
        	BEDUG_ASSERT(false, "HID table not found error");
        	details(key, index);
        	return;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }

        auto lambda = [&] (auto& tuple) {
            memcpy_s(dst, sizeof(uint32_t), tuple.serialize(index).get(), tuple.size());
        };

        std::visit(lambda, it->second);
    }

    bool isUpdated(const uint16_t key)
    {
        auto it = characteristics.find(key);
        if (it == characteristics.end()) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "HID table not found error");
            details(key, index);
            return;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }

        bool result = false;
        auto lambda = [&] (auto& tuple) {
            result = tuple.isUpdated();
        };
        std::visit(lambda, it->second);

        return result;
    }

    void resetUpdated(const uint16_t key)
    {
        auto it = characteristics.find(key);
        if (it == characteristics.end()) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "HID table not found error");
            details(key, index);
            return;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }

        auto lambda = [&] (auto& tuple) {
            tuple.resetUpdated();
        };
        std::visit(lambda, it->second);
    }

    constexpr unsigned characteristicLength(uint16_t characteristic_id)
    {
        auto it = characteristics.find(characteristic_id);
        if (it == characteristics.end()) {
#ifdef USE_HAL_DRIVER
            BEDUG_ASSERT(false, "HID table not found error");
        	details(characteristic_id, 0);
            return 0;
#else
            throw new exceptions::TemplateErrorException();
#endif
        }

        unsigned result = 0;
        auto lambda = [&] (const auto& tuple) {
            result = tuple.length();
        };

        std::visit(lambda, it->second);

        return result;
    }

    static constexpr unsigned count()
    {
        return Table::count();
    }

};


#endif // HIDCONTROLLER_H
