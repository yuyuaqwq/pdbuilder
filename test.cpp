// pdbrain.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>

#include <Geek/File/file.hpp>

#define private public
#include <pdbuilder/pdbuilder.hpp>
#include <pdbuilder/downloader.hpp>
#undef private

#include <sezz/sezz.hpp>

namespace sezz{

void Serialize(std::ostream& os, pdbuilder::Pdber& pdber) {
    Serialize(os, pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
}

template<>
pdbuilder::Pdber Deserialize<pdbuilder::Pdber>(std::istream& is) {
    pdbuilder::Pdber pdber;
    Deserialize(is, pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
    return pdber;
}

template<>
void Serialize/* <symbolic_access::Member> */(std::ostream& os, symbolic_access::Member& member) {
    Serialize(os, member.Bitfield, member.Name, member.Offset);
}
template<>
symbolic_access::Member Deserialize<symbolic_access::Member>(std::istream& is) {
    symbolic_access::Member member;
    Deserialize(is, member.Bitfield, member.Name, member.Offset);
    return member;
}
}


int main()
{

    if (!Geek::File::FileExists(L"ntdll.pdb")) {
        pdbuilder::Downloader downloader;
        downloader.DownloadPdb(downloader.GetPdbInfoFromImageBuf((uint8_t*)GetModuleHandleA("ntdll.dll")));
    }

    std::fstream fs;
    fs.open("ntdll.bin", std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

    pdbuilder::Pdber pdber{ pdbuilder::FileStream(LR"(ntdll.pdb)") };
    sezz::Serialize(fs, pdber);

    fs.seekg(0);

    auto pdber2 = sezz::Deserialize<pdbuilder::Pdber>(fs);

    auto offset = pdber2.Struct()["_PEB"]["OSMajorVersion"].Offset();
    auto rva = pdber2.Symbol()["NtSuspendProcess"].Rva();

    std::cout << "Hello World!\n";
}
