//
// Created by Thomas Willson on 2021-09-04.
//

#pragma once

#include <vector>
#include "gsl/gsl"

#include "Property.h"

namespace PropertySystem {

    class PropertySet {
    public:
        PropertySet(std::initializer_list<PropertySet *> sets,
                    std::initializer_list<IReadOnlyProperty *> properties)
                : s{sets}, p{properties}
        {}

        PropertySet(std::initializer_list<IReadOnlyProperty *> properties)
                : p{properties}
        {}

        const std::vector<PropertySet *> &sets() const { return s; }

        const std::vector<IReadOnlyProperty *> &properties() const { return p; }

    private:
        const std::vector<PropertySet *> s;
        const std::vector<IReadOnlyProperty *> p;
    };

#ifdef PROPERTY_SYSTEM_INCLUDE_NAMES

    class FlattenedPropertyWrapper {
    public:
        FlattenedPropertyWrapper(std::string name, IReadOnlyProperty &prop)
                : prop{prop}, n{name} {}

        IReadOnlyProperty &property() const { return prop; }

        const std::string& name() const { return n; }

    private:
        IReadOnlyProperty &prop;
        std::string n;
    };

    namespace Impl {
        void flatten(std::string prefix,
                     std::vector<FlattenedPropertyWrapper> &output,
                     PropertySet *set) {
            for (auto nested_set: set->sets()) {
                flatten(prefix + "." + set->name(), output, nested_set);
            }

            for (auto property: set->properties()) {
                output.emplace_back(prefix + property->name(), *property);
            }
        }
    }

    std::vector<FlattenedPropertyWrapper> flatten(std::vector<PropertySet *> property_sets) {
        std::vector<FlattenedPropertyWrapper> flat_list;

        for (auto set: property_sets) {
            Impl::flatten("", flat_list, set);
        }

        return flat_list;
    }
#endif

    struct IPropertyStruct {
        virtual const PropertySet get_property_set() = 0;
    };

#define STORAGE_PROPERTY(type, name) PropertySystem::StorageProperty<type> name{#name}
#define STORAGE_PROPERTY_INIT(type, name, initial_value) PropertySystem::StorageProperty<type> name{#name, (initial_value)}

}
