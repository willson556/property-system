//
// Created by Thomas Willson on 2021-08-25.
//

#pragma once

#include "Property.h"

namespace PropertySystem
{
    template<typename T>
    class ReadOnlyProxyProperty : virtual public ITypedReadOnlyProperty<T> {
    public:
        T get() const override {
            T t;
            IReadOnlyProperty::get(as_writable_bytes(t));
            return t;
        }

        using IReadOnlyProperty::get;
    };

    template<typename T>
    class ProxyProperty : public ReadOnlyProxyProperty<T>, virtual public ITypedProperty<T> {
    public:
        void set(const T &value) override {
            this->set(as_bytes(value));
        }

        using IProperty::set;
    };
}
