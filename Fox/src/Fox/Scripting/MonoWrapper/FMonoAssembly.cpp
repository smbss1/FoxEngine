#include "FMonoAssembly.hpp"
#include "FMonoDomain.hpp"
#include "FMonoException.hpp"

#include "FMonoType.hpp"

#include <sstream>

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>

namespace fox::mono
{

FMonoAssembly::FMonoAssembly(const FMonoDomain& domain, const std::string& path)
{
	assembly_ = mono_domain_assembly_open(domain.GetInternalPtr(), path.c_str());

	if(!assembly_)
		throw FMonoException("NATIVE::Could not open assembly with path : " + path);

	image_ = mono_assembly_get_image(assembly_);
}

auto FMonoAssembly::GetType(const std::string& name) const -> FMonoType
{
	return FMonoType(image_, name);
}

auto FMonoAssembly::GetType(const std::string& name_space, const std::string& name) const -> FMonoType
{
	return FMonoType(image_, name_space, name);
}

auto FMonoAssembly::DumpReferences() const -> std::vector<std::string>
{
	std::vector<std::string> refs;
	/* Get a pointer to the AssemblyRef metadata table */
	auto table_info = mono_image_get_table_info(image_, MONO_TABLE_ASSEMBLYREF);

	/* Fetch the number of rows available in the table */
	int rows = mono_table_info_get_rows(table_info);

	refs.reserve(size_t(rows));
	/* For each row, print some of its values */
	for(int i = 0; i < rows; i++)
	{
		/* Space where we extract one row from the metadata table */
		uint32_t cols[MONO_ASSEMBLYREF_SIZE];

		/* Extract the row into the array cols */
		mono_metadata_decode_row(table_info, i, cols, MONO_ASSEMBLYREF_SIZE);

		std::stringstream s;
		s << i + 1;
		s << " Version=";
		s << cols[MONO_ASSEMBLYREF_MAJOR_VERSION];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_MINOR_VERSION];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_BUILD_NUMBER];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_REV_NUMBER];
		s << "\n\tName=";
		s << mono_metadata_string_heap(image_, cols[MONO_ASSEMBLYREF_NAME]);

		refs.emplace_back(s.str());
	}

	return refs;
}

} // namespace fox::mono