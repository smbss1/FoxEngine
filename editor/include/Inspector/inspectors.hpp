//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_INSPECTORS_HPP
#define FOXENGINE_INSPECTORS_HPP

#include <rttr/variant.h>
#include "inspector.hpp"

rttr::variant get_meta_empty(const rttr::variant& other);
bool inspect_ui_var(rttr::variant& var, bool skip_custom = false, bool read_only = false,
                 const inspector::meta_getter& get_metadata = get_meta_empty);
bool inspect_track_var(rttr::variant& var, bool skip_custom = false, bool read_only = false,
                    const inspector::meta_getter& get_metadata = get_meta_empty);

bool inspect_array(rttr::variant& var, bool read_only = false,
                   const inspector::meta_getter& get_metadata = get_meta_empty);
bool inspect_associative_container(rttr::variant& var, bool read_only = false);
bool inspect_enum(rttr::variant& var, rttr::enumeration& data, bool read_only = false);


#endif //FOXENGINE_INSPECTORS_HPP
