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

#include <chrono>

size_t string_count = 0;
size_t other_count = 0;

namespace sezz{


template <class Archive>
void Serialize(Archive& ar, pdbuilder::Pdber& pdber) {
    ar.Save(size_t{ pdber.module_extender_.m_Structs.size()});
    for (auto& struct_ : pdber.module_extender_.m_Structs) {
        ++string_count;
        ar.Save(struct_.first, struct_.second);
    }
    ar.Save(size_t{ pdber.module_extender_.m_Symbols.size() });
    for (auto& symbol : pdber.module_extender_.m_Symbols) {
        ++string_count;
        ++other_count;
        ar.Save(symbol.first, symbol.second);
    }
    //ar.Save(pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
}

template <class Archive>
void Serialize(Archive& ar, symbolic_access::BitfieldData& data) {
    other_count+=2;
    ar.Save(data.Length, data.Position);
}

template <class Archive>
void Serialize(Archive& ar, symbolic_access::Member& member) {
    ++other_count;
    ++string_count;
    ar.Save(member.Bitfield, member.Name, member.Offset);
}



template <class T, class Archive>
    requires std::is_same_v<std::decay_t<T>, pdbuilder::Pdber>
T Deserialize(Archive& ar) {
    pdbuilder::Pdber pdber;
    auto size_members = ar.Load<size_t>();
    for (size_t i = 0; i < size_members; i++) {
        auto string = ar.Load<std::string>();
        auto members = ar.Load<symbolic_access::StructMembers>();
    }
    auto size_symbols = ar.Load<size_t>();
    for (size_t i = 0; i < size_symbols; i++) {
        auto string = ar.Load<std::string>();
        auto size = ar.Load<size_t>();
    }
    //ar.Load(pdber.module_extender_.m_Structs, pdber.module_extender_.m_Symbols);
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

    pdbuilder::Pdber pdber{ pdbuilder::FileStream(LR"(ntdll.pdb)") };


    //sezz::MemoryIoStream fs_raw(1024);
    //sezz::BinaryArchive<sezz::MemoryIoStream, sezz::ArchiveMode::kRaw> ar_raw(fs_raw);
    //for (int i = 0; i < 4000; i++) {
    //    ar_raw.Save(pdber);
    //}
    //std::cout << "raw size: " << fs_raw.tellp() << "bytes" << std::endl;

    //

    //std::stringstream fs;

    //fs.open("ntdll.bin", std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    sezz::MemoryIoStream fs(1028878000);

    sezz::BinaryOutputArchive oar(fs);


    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < 4000; i++) {
        //fs.seekp(0);
        oar.Save(pdber);
    }
    auto t2 = std::chrono::steady_clock::now();

    double dr_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::cout << string_count << std::endl << other_count << std::endl;

    std::cout << "Serialization statistics: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " ms" << std::endl;

    std::cout << "Serialized size: " << fs.tellp() << "bytes" << std::endl;

    fs.seekg(0);
    sezz::BinaryInputArchive iar(fs);


    t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < 4000; i++) {
        //fs.seekp(0);
        auto pdber2 = iar.Load<pdbuilder::Pdber>();
        //PEB64 peb{ 0 };
        //auto pdber_peb = pdber2.Struct(&peb)["_PEB"];
        //pdber_peb["OSMajorVersion"]->u32() = 0xfe;

        //auto sub_struct = pdber_peb["OSMajorVersion"].SubStruct();

        //auto offset = pdber2.Struct()["_PEB"]["OSMajorVersion"].Offset();
        //auto rva = pdber.Symbol()["NtSuspendProcess"].Rva();
        //auto rva2 = pdber2.Symbol()["NtSuspendProcess"].Rva();
    }
    //auto pdber2 = ar.Load<pdbuilder::Pdber>();
    t2 = std::chrono::steady_clock::now();

    std::cout << "Deserialization statistics: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        << " ms" << std::endl;

}
