#include <iostream>
#include <fstream>

#include <Geek/wow64ext/wow64ext.h>
#include <Geek/File/file.hpp>

#include <sezz/sezz.hpp>

#define private public
#include <pdbuilder/pdbuilder.hpp>
#undef private
#include <pdbuilder/downloader.hpp>



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





int main(){
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

    PEB64 peb{ 0 };
    auto pdber_pdb = pdber2.Struct(&peb)["_PEB"];
    pdber_pdb["OSMajorVersion"].Value<DWORD>() = 0xfe;


    auto offset = pdber2.Struct()["_PEB"]["OSMajorVersion"].Offset();
    auto rva = pdber2.Symbol()["NtSuspendProcess"].Rva();

    std::cout << "Hello World!\n";
}
