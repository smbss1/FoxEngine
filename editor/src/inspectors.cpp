// //
// // Created by samuel on 13/07/2021.
// //

// #include <imgui.h>
// #include "Inspector/inspectors.hpp"

// rttr::variant get_meta_empty(const rttr::variant& other)
// {
//     return rttr::variant();
// }

// struct inspector_registry
// {
//     inspector_registry()
//     {
//         auto inspector_types = rttr::type::get<inspector>().get_derived_classes();
//         for(auto& inspector_type : inspector_types)
//         {
//             auto inspected_type_var = inspector_type.get_metadata("inspected_type");
//             if(inspected_type_var)
//             {
//                 auto inspected_type = inspected_type_var.get_value<rttr::type>();
//                 auto inspector_var = inspector_type.create();
//                 if(inspector_var)
//                 {
//                     type_map[inspected_type] = inspector_var.get_value<std::shared_ptr<inspector>>();
//                 }
//             }
//         }
//     }
//     std::unordered_map<rttr::type, std::shared_ptr<inspector>> type_map;
// };

// std::shared_ptr<inspector> get_inspector(rttr::type type)
// {
//     static inspector_registry registry;
//     return registry.type_map[type];
// }

// bool inspect_var(rttr::variant& var, bool skip_custom, bool read_only,
//                  const inspector::meta_getter& get_metadata)
// {
//     rttr::instance object = var;
//     auto type = object.get_derived_type();
//     auto properties = type.get_properties();

//     bool changed = false;

//     auto inspector = get_inspector(type);
//     if(!skip_custom && inspector)
//     {
//         changed |= inspector->inspect(var, read_only, get_metadata);
//     }
// //    else if(properties.empty())
// //    {
// //        if(type.is_enumeration())
// //        {
// //            auto enumeration = type.get_enumeration();
// //            changed |= inspect_enum(var, enumeration, read_only);
// //        }
// //    }
//     else
//     {
//         for(auto& prop : properties)
//         {
//             bool prop_changed = false;
//             auto prop_var = prop.get_value(object);
//             bool is_readonly = prop.is_readonly();
//             bool is_array = prop_var.is_sequential_container();
//             bool is_associative_container = prop_var.is_associative_container();
//             bool is_enum = prop.is_enumeration();
//             rttr::instance prop_object = prop_var;
//             bool has_inspector = !!get_inspector(prop_object.get_derived_type());
//             bool details = !has_inspector && !is_enum;
//             property_layout layout(prop);
//             bool open = true;
//             if(details)
//             {
//                 ImGui::AlignTextToFramePadding();
//                 open = ImGui::TreeNode("details");
//             }

//             if(open)
//             {
//                 auto get_meta = [&prop](const rttr::variant& name) -> rttr::variant {
//                     return prop.get_metadata(name);
//                 };
// //                if(is_array)
// //                {
// //                    prop_changed |= inspect_array(prop_var, is_readonly, get_meta);
// //                }
// //                else if(is_associative_container)
// //                {
// //                    prop_changed |= inspect_associative_container(prop_var, is_readonly);
// //                }
// //                else if(is_enum)
// //                {
// //                    auto enumeration = prop.get_enumeration();
// //                    prop_changed |= inspect_enum(prop_var, enumeration, is_readonly);
// //                }
// //                else
// //                {
//                 prop_changed |= inspect_var(prop_var, false, is_readonly, get_meta);
// //                }

//                 if(details && open)
//                     ImGui::TreePop();
//             }

//             if(prop_changed && !is_readonly)
//             {
//                 prop.set_value(object, prop_var);
//             }

//             changed |= prop_changed;
//         }
//     }

//     return changed;
// }