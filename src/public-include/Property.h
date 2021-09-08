//
// Created by Thomas Willson on 2021-08-24.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "gsl/gsl"

namespace PropertySystem {

template <typename T>
constexpr gsl::span<std::byte> as_writable_bytes(T& t) {
    return gsl::span<std::byte>{reinterpret_cast<std::byte*>(&t), sizeof(t)};
}

template <typename T>
constexpr gsl::span<const std::byte> as_bytes(const T& t) {
    return gsl::span<const std::byte>{reinterpret_cast<const std::byte*>(&t),
                                      sizeof(t)};
}

class IReadOnlyProperty {
   public:
    [[nodiscard]] virtual size_t size() const = 0;

    virtual void get(gsl::span<std::byte> dst) const = 0;

    virtual gsl::czstring<> name() const = 0;

   protected:
    void check_size(const gsl::span<const std::byte>& span) const {
        if (span.size() < this->size()) {
            throw std::runtime_error("Provided span is too small!");
        }
    }
};

class IProperty : virtual public IReadOnlyProperty {
   public:
    virtual void set(gsl::span<const std::byte> src) = 0;
};

template <typename T>
class ITypedReadOnlyProperty : virtual public IReadOnlyProperty {
   public:
    [[nodiscard]] size_t size() const override { return sizeof(T); }

    virtual T get() const = 0;

    using IReadOnlyProperty::get;

    operator T() const { return get(); }
};

template <typename T>
class ITypedProperty : virtual public ITypedReadOnlyProperty<T>,
                       virtual public IProperty {
   public:
    virtual void set(const T& val) = 0;

    T operator=(const T& val) {
        set(val);
        return static_cast<ITypedReadOnlyProperty<T>*>(this)->get();
    }  // NOLINT(cppcoreguidelines-c-copy-assignment-signature) Can't always
       // return a reference

    using IProperty::set;
};

namespace Impl {
template <typename T, typename TStorage>
class ReferenceProperty : virtual public ITypedReadOnlyProperty<T> {
   public:
    ReferenceProperty([[maybe_unused]] gsl::czstring<> name,
                      TStorage& reference)
        : reference{reference}, n{name} {}

    T get() const override { return reference; }

    void get(gsl::span<std::byte> dst) const override {
        this->check_size(gsl::as_bytes(dst));
        *reinterpret_cast<T*>(dst.data()) = reference;
    }

    gsl::czstring<> name() const override { return n; }

   protected:
    TStorage& reference;  // NOLINT: memory optimization in a simple class.
   private:
    gsl::czstring<> n;
};
}  // namespace Impl

template <typename T>
class ReferenceReadOnlyProperty : public Impl::ReferenceProperty<T, const T>,
                                  virtual public ITypedReadOnlyProperty<T> {
   public:
    ReferenceReadOnlyProperty(gsl::czstring<> name, const T& reference)
        : Impl::ReferenceProperty<T, const T>{name, reference} {}
};

template <typename T>
class ReferenceProperty : public Impl::ReferenceProperty<T, T>,
                          virtual public ITypedProperty<T> {
   public:
    ReferenceProperty(gsl::czstring<> name, T& reference)
        : Impl::ReferenceProperty<T, T>{name, reference} {}

    void set(const T& val) override { this->reference = val; }

    void set(gsl::span<const std::byte> src) override {
        this->check_size(src);
        this->reference = *reinterpret_cast<const T*>(src.data());
    }

    using ITypedProperty<T>::operator=;
};

template <typename TGetter,
          typename TProp = decltype(std::declval<TGetter>()())>
class LambdaReadOnlyProperty : virtual public ITypedReadOnlyProperty<TProp> {
   public:
    LambdaReadOnlyProperty([[maybe_unused]] gsl::czstring<> name,
                           TGetter getter)
        : getter{getter}, n{name} {}

    TProp get() const override { return getter(); }

    void get(gsl::span<std::byte> dst) const override {
        this->check_size(dst);
        auto value{get()};
        *reinterpret_cast<TProp*>(dst.data()) = value;
    }

    gsl::czstring<> name() const override { return n; }

   private:
    TGetter getter;

    gsl::czstring<> n;
};

template <typename TGetter,
          typename TSetter,
          typename TProp = decltype(std::declval<TGetter>()())>
class LambdaProperty : public LambdaReadOnlyProperty<TGetter, TProp>,
                       virtual public ITypedProperty<TProp> {
   public:
    LambdaProperty(gsl::czstring<> name, TGetter getter, TSetter setter)
        : LambdaReadOnlyProperty<TGetter, TProp>{name, getter},
          setter{setter} {}

    void set(const TProp& val) override { setter(val); }

    void set(gsl::span<const std::byte> src) override {
        this->check_size(src);
        set(*reinterpret_cast<const TProp*>(src.data()));
    }

    using ITypedProperty<TProp>::operator=;

   private:
    TSetter setter;
};

template <typename T>
class StorageProperty : virtual public ITypedProperty<T> {
   public:
    explicit StorageProperty([[maybe_unused]] gsl::czstring<> name) : n{name} {}

    StorageProperty([[maybe_unused]] gsl::czstring<> name,
                    const T& initial_value)
        : value{initial_value}, n{name} {}

    T get() const override { return value; }

    void get(gsl::span<std::byte> dst) const override {
        this->check_size(gsl::as_bytes(dst));
        *reinterpret_cast<T*>(dst.data()) = value;
    }

    void set(const T& val) override { value = val; }

    void set(gsl::span<const std::byte> src) override {
        this->check_size(src);
        value = *reinterpret_cast<const T*>(src.data());
    }

    using ITypedProperty<T>::operator=;

    gsl::czstring<> name() const override { return n; }

   private:
    T value;
    gsl::czstring<> n;
};
}  // namespace PropertySystem