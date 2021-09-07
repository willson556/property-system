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

#ifdef PROPERTY_SYSTEM_INCLUDE_NAMES
    gsl::czstring<> name() override { return n; }
#endif
   protected:
    explicit ReadOnlyProxyProperty([[maybe_unused]] gsl::czstring<> name)
#ifdef PROPERTY_SYSTEM_INCLUDE_NAMES
        : n {
        name
    }
#endif
    {}

   private:
#ifdef PROPERTY_SYSTEM_INCLUDE_NAMES
    gsl::czstring<> n;
#endif
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
