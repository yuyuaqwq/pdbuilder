#ifndef PDBUILDER_DOWNLOADER_HPP_
#define PDBUILDER_DOWNLOADER_HPP_

#include <optional>
#include <string>
#include <format>
#include <tuple>

#include <Geek/network/http.hpp>
#include <Geek/file/file.hpp>


#include <iostream>
#include <Windows.h>

namespace pdbuilder {

class Downloader {
private:
	inline static std::wstring server_url_ = L"https://msdl.microsoft.com";

public:
	struct PdbInfo {
		ULONG Signature;
		GUID Guid;
		ULONG Age;
		char PdbFileName[1];
	};

    std::tuple<std::string, std::string> GetPdbInfoFromImageBuf(uint8_t* base) {
		const auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
		const auto ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dosHeader->e_lfanew);
		// get debug dir
		const auto dbgDir = reinterpret_cast<IMAGE_DEBUG_DIRECTORY*>(base +
			ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress);
		const auto pdbInfo = reinterpret_cast<PdbInfo*>(base + dbgDir->AddressOfRawData);

		// get guid(md5) md5 size of 32
		auto guid = std::format("{:08x}{:04x}{:04x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:x}",
			pdbInfo->Guid.Data1, pdbInfo->Guid.Data2, pdbInfo->Guid.Data3,
			pdbInfo->Guid.Data4[0], pdbInfo->Guid.Data4[1], pdbInfo->Guid.Data4[2], pdbInfo->Guid.Data4[3],
			pdbInfo->Guid.Data4[4], pdbInfo->Guid.Data4[5], pdbInfo->Guid.Data4[6], pdbInfo->Guid.Data4[7],
			pdbInfo->Age
		);

		// get pdb name
		std::string name(pdbInfo->PdbFileName);

		return std::make_tuple(guid, name);
    }

	bool DownloadPdb(std::tuple<std::string, std::string> pdb_info) {
		auto& [guid, name] = pdb_info;
		auto name_w = Geek::String::AnsiToUtf16le(name);
		auto guid_w = Geek::String::AnsiToUtf16le(guid);

		Geek::http::Connect cli{ server_url_ };

		auto res = cli.Open(L"/download/symbols/" + name_w + L"/" + guid_w + L"/" + name_w);
		cli.Send(true);


		auto code = cli.GetStatusCode();

		std::wcout << cli.GetResponseHeaders().value()->Print();

		auto data = cli.GetResponseContent();

		Geek::File::WriteFile(name_w.data(), data.value().data(), data.value().size());
		return true;
	}

private:
    
};

}  // namespace pdbuilder 


#endif // PDBUILDER_DOWNLOADER_HPP_