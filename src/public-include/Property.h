//
// Created by Thomas Willson on 2021-08-24.
//

#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>

#include "gsl/span"

namespace PropertySystem {

    template<typename T>
    constexpr gsl::span<std::byte> as_writable_bytes(T &t) {
        return gsl::span<std::byte>{reinterpret_cast<std::byte *>(&t), sizeof(t)};
    }

    template<typename T>
    constexpr gsl::span<const std::byte> as_bytes(const T &t) {
        return gsl::span<const std::byte>{reinterpret_cast<const std::byte *>(&t), sizeof(t)};
    }

    class IReadOnlyProperty {
    public:
        virtual size_t size() const = 0;
        virtual void get(gsl::span<std::byte> dst) const = 0;

    protected:
        void check_size(const gsl::span<const std::byte> &span) const {
            if (span.size() < this->size()) {
                throw std::runtime_error("Provided span is too small!");
            }
        }
    };

    class IProperty : virtual public IReadOnlyProperty {
    public:
        virtual void set(gsl::span<const std::byte> src) = 0;
    };

    template<typename T>
    class ITypedReadOnlyProperty : virtual public IReadOnlyProperty {
    public:
        size_t size() const override { return sizeof(T); }

        virtual T get() const = 0;
        using IReadOnlyProperty::get;
    };

    template<typename T>
    class ITypedProperty : virtual public ITypedReadOnlyProperty<T>, virtual public IProperty {
    public:
        virtual void set(const T &val) = 0;
        using IProperty::set;
    };

    namespace Impl {
        template<typename T, typename TStorage>
        class ReferenceProperty : virtual public ITypedReadOnlyProperty<T> {
        public:
            ReferenceProperty(TStorage &reference)
                    : reference{reference} {}

            T get() const override { return reference; }

            void get(gsl::span<std::byte> dst) const override {
                this->check_size(gsl::as_bytes(dst));
                *reinterpret_cast<T *>(dst.data()) = reference;
            }

        protected:
            TStorage &reference;
        };
    }

    template<typename T>
    class ReferenceReadOnlyProperty
            : public Impl::ReferenceProperty<T, const T>, virtual public ITypedReadOnlyProperty<T> {
    public:
        ReferenceReadOnlyProperty(const T &reference)
                : Impl::ReferenceProperty<T, const T>(reference) {}
    };

    template<typename T>
    class ReferenceProperty : public Impl::ReferenceProperty<T, T>, virtual public ITypedProperty<T> {
    public:
        ReferenceProperty(T &reference)
                : Impl::ReferenceProperty<T, T>{reference} {}

        void set(const T &val) override { this->reference = val; }

        void set(gsl::span<const std::byte> src) override {
            this->check_size(src);
            this->reference = *reinterpret_cast<const T *>(src.data());
        }
    };

}