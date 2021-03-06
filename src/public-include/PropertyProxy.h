//
// Created by Thomas Willson on 2021-08-25.
//

#pragma once

#include "Property.h"

/**
 * The ProxyProperty classes serve as base classes for a property
 * implementations that function over a communications bus.
 */

namespace PropertySystem {
template <typename T>
class ReadOnlyProxyProperty : virtual public ITypedReadOnlyProperty<T> {
   public:
    T get() const override {
        T t;
        get(as_writable_bytes(t));
        return t;
    }

    using IReadOnlyProperty::get;

    gsl::czstring<> name() const override { return n; }

   protected:
    explicit ReadOnlyProxyProperty([[maybe_unused]] gsl::czstring<> name)
        : n{name} {}

   private:
    gsl::czstring<> n;
};

template <typename T>
class ProxyProperty : public ReadOnlyProxyProperty<T>,
                      virtual public ITypedProperty<T> {
   public:
    void set(const T& value) override { this->set(as_bytes(value)); }

    using IProperty::set;
    using ITypedProperty<T>::operator=;

   protected:
    explicit ProxyProperty(gsl::czstring<> name)
        : ReadOnlyProxyProperty<T>{name} {}
};
}  // namespace PropertySystem
