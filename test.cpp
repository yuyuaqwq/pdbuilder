#include <iostream>
#include <fstream>

#include <Geek/wow64ext/wow64ext.h>
#include <Geek/File/file.hpp>


#include <sezz/sezz.hpp>
#include <sezz/stl/optional.hpp>
#include <sezz/stl/map.hpp>
#include <sezz/stl/string.hpp>
#include <sezz/stl/vector.hpp>

#define private public
#include <pdbuilder/pdbuilder.hpp>
#undef private
#include <pdbuilder/downloader.hpp>



namespace sezz{

template <class Archive>
void Serialize(Archive& ar, pdbuilder::Pdber& pdber) {
    ar.Save(pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
}

template <class Archive>
void Serialize(Archive& ar, symbolic_access::BitfieldData& data) {
    ar.Save(data.Length, data.Position);
}

template <class Archive>
void Serialize(Archive& ar, symbolic_access::Member& member) {
    ar.Save(member.Bitfield, member.Name, member.Offset);
}



template <class T, class Archive>
    requires std::is_same_v<std::decay_t<T>, pdbuilder::Pdber>
T Deserialize(Archive& ar) {
    pdbuilder::Pdber pdber;
    ar.Load(pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
    return pdber;
}

template <class T, class Archive>
    requires std::is_same_v<T, symbolic_access::BitfieldData>
T Deserialize(Archive& ar) {
    symbolic_access::BitfieldData data;
    ar.Load(data.Length, data.Position);
    return data;
}

template<class T, class Archive>
    requires std::is_same_v<T, symbolic_access::Member>
T Deserialize(Archive& ar) {
    symbolic_access::Member member;
    ar.Load(member.Bitfield, member.Name, member.Offset);
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

    sezz::BinaryArchive<std::iostream> ar(fs);

    pdbuilder::Pdber pdber{ pdbuilder::FileStream(LR"(ntdll.pdb)") };
    ar.Save(pdber);

    fs.seekp(0);

    auto pdber2 = ar.Load<pdbuilder::Pdber>();

    PEB64 peb{ 0 };
    auto pdber_peb = pdber2.Struct(&peb)["_PEB"];
    pdber_peb["OSMajorVersion"]->u32() = 0xfe;

    auto sub_struct = pdber_peb["OSMajorVersion"].SubStruct();

    auto offset = pdber2.Struct()["_PEB"]["OSMajorVersion"].Offset();
    auto rva = pdber.Symbol()["NtSuspendProcess"].Rva();
    auto rva2 = pdber2.Symbol()["NtSuspendProcess"].Rva();

    std::cout << "Hello World!\n";
}
