//
// Created by samuel on 13/07/2021.
//

#ifndef FOXENGINE_INSPECTOR_HPP
#define FOXENGINE_INSPECTOR_HPP

#include <Reflection.hpp>

struct inspector
{
    REFLECTABLEV(inspector)

    using meta_getter = std::function<rttr::variant(const rttr::variant&)>;

    virtual ~inspector() = default;

    virtual bool inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata) = 0;
    virtual bool inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata) = 0;
};

struct property_layout
{
    property_layout(const rttr::property& prop, bool columns = true, float columnWidth = 100);

    property_layout(const std::string& name, bool columns = true, float columnWidth = 100);

    property_layout(const std::string& name, const std::string& tooltip, bool columns = true, float columnWidth = 100);

    ~property_layout();

    std::string m_strName;
};

REFLECT_INLINE(inspector)
    {
        rttr::registration::class_<inspector>("inspector");
    }
#define INSPECTOR_REFLECT(inspector_type, inspected_type)                                                    \
	REFLECT_INLINE(inspector_type)                                                                           \
	{                                                                                                        \
		rttr::registration::class_<inspector_type>(#inspector_type)(                                         \
			rttr::metadata("inspected_type", rttr::type::get<inspected_type>()))                             \
			.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                         \
	}

#define DECLARE_INSPECTOR(inspector_type, inspected_type)                                                    \
	struct inspector_type : public inspector                                                                 \
	{                                                                                                        \
		REFLECTABLEV(inspector_type, inspector)                                                              \
		bool inspect_imgui(rttr::variant& var, bool read_only, const meta_getter& get_metadata) override;     \
		bool inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata) override;   \
	};                                                                                                       \
	INSPECTOR_REFLECT(inspector_type, inspected_type)

#define INSPECTOR_TRACK_FUNCTION(inspector_type, inspected_type)                                                                               \
    bool inspector_type::inspect_track(rttr::variant& var, bool read_only, const meta_getter& get_metadata) {  \
        Track<inspected_type>* new_track = new Track<inspected_type>();                                    \
        var = new_track;                                                                                    \
        return true;                                                                                        \
    }

#endif //FOXENGINE_INSPECTOR_HPP
