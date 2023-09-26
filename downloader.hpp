#ifndef PDBUILDER_SYMBOL_HPP_
#define PDBUILDER_SYMBOL_HPP_

#include <optional>
#include <string>
#include <format>
#include <tuple>

#include <Windows.h>

namespace pdbuilder {

class Downloader {
public:
	struct PdbInfo {
		ULONG Signature;
		GUID Guid;
		ULONG Age;
		char PdbFileName[1];
	};

    std::tuple<std::string, std::string> GetGuidFromImageBuf(uint8_t* base) {
		const auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
		const auto ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dosHeader->e_lfanew);
		//get debug dir
		const auto dbgDir = reinterpret_cast<IMAGE_DEBUG_DIRECTORY*>(base +
			ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress);
		const auto pdbInfo = reinterpret_cast<PdbInfo*>(base + dbgDir->AddressOfRawData);

		//get guid(md5) md5 size of 32
		auto guid = std::format("{8x}{4x}{4x}{2x}{2x}{2x}{2x}{2x}{2x}{2x}{2x}{x}",
			pdbInfo->Guid.Data1, pdbInfo->Guid.Data2, pdbInfo->Guid.Data3,
			pdbInfo->Guid.Data4[0], pdbInfo->Guid.Data4[1], pdbInfo->Guid.Data4[2], pdbInfo->Guid.Data4[3],
			pdbInfo->Guid.Data4[4], pdbInfo->Guid.Data4[5], pdbInfo->Guid.Data4[6], pdbInfo->Guid.Data4[7],
			pdbInfo->Age
		);

		//get pdb name
		std::string name(pdbInfo->PdbFileName);

		return std::make_tuple(guid, name);
    }

private:
    
};

}  // namespace pdbuilder 


#endif // PDBUILDER_SYMBOL_HPP_