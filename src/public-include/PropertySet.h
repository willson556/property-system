//
// Created by Thomas Willson on 2021-09-04.
//

#pragma once

#include <vector>
#include "gsl/gsl"

#include "Property.h"

namespace PropertySystem {

class NestedPropertySet;

class PropertySet {
   public:
    PropertySet(std::initializer_list<NestedPropertySet> sets,
                std::initializer_list<IReadOnlyProperty*> properties)
        : s{sets}, p{properties} {}

    PropertySet(std::initializer_list<IReadOnlyProperty*> properties)
        : p{properties} {}

    std::vector<NestedPropertySet>& sets() { return s; }

    const std::vector<IReadOnlyProperty*>& properties() { return p; }

   private:
    std::vector<NestedPropertySet> s;
    const std::vector<IReadOnlyProperty*> p;
};

class NestedPropertySet {
   public:
    NestedPropertySet([[maybe_unused]] gsl::czstring<> name,
                      PropertySet nested_set)
        : nested_set{nested_set}, n{name} {}

    PropertySet& nested() { return nested_set; }

    gsl::czstring<> name() const { return n; }

   private:
    PropertySet nested_set;
    gsl::czstring<> n;
};

class FlattenedPropertyWrapper {
   public:
    FlattenedPropertyWrapper(std::string name, IReadOnlyProperty& prop)
        : prop{prop}, n{name} {}

    IReadOnlyProperty& property() const { return prop; }

    const std::string& name() const { return n; }

   private:
    IReadOnlyProperty& prop;
    std::string n;
};

namespace Impl {
void flatten(std::string prefix,
             std::vector<FlattenedPropertyWrapper>& output,
             PropertySet* set) {
    auto adjusted_prefix{prefix.empty() ? "" : prefix + "."};

    for (auto& nested_set : set->sets()) {
        auto name{adjusted_prefix + nested_set.name()};
        flatten(name, output, &nested_set.nested());
    }

    for (auto property : set->properties()) {
        auto name{adjusted_prefix + property->name()};
        output.emplace_back(name, *property);
    }
}
}  // namespace Impl

std::vector<FlattenedPropertyWrapper> flatten(PropertySet property_set) {
    std::vector<FlattenedPropertyWrapper> flat_list;

    Impl::flatten("", flat_list, &property_set);

    return flat_list;
}

struct IPropertyStruct {
    virtual PropertySet get_property_set() = 0;
};
}  // namespace PropertySystem
