#ifndef PDBUILDER_PDBUILDER_HPP_
#define PDBUILDER_PDBUILDER_HPP_

#include <SymbolicAccess/Pdb/Extractors/StructExtractor.h>
#include <SymbolicAccess/Pdb/Extractors/SymbolExtractor.h>
#include <SymbolicAccess/ModuleExtender/ModuleExtender.h>
#include <SymbolicAccess/ModuleExtender/ModuleExtenderFactory.h>

#include <pdbuilder/struct.hpp>
#include <pdbuilder/symbol.hpp>

#pragma comment(lib, "SymbolicAccess/lib/x64/SymbolicAccessUM.lib")

namespace pdbuilder {

class Pdber {
public:
    Pdber() : symbol_map_{ &module_extender_ } {
        ::new(&module_extender_) symbolic_access::ModuleExtender(0, {}, {});
    }
    Pdber(symbolic_access::FileStream pdb_stream) : symbol_map_{ &module_extender_ }{
        symbolic_access::MsfReader msf_reader_{ std::move(pdb_stream) };
        msf_reader_.Initialize();

        symbolic_access::SymbolsExtractor symbols_extractor(msf_reader_);
        symbolic_access::StructExtractor structs_extractor(msf_reader_);

        ::new(&module_extender_) symbolic_access::ModuleExtender(0, structs_extractor.Extract(), symbols_extractor.Extract());
    }
    Pdber(Pdber&& other) noexcept : symbol_map_{ &module_extender_ } {
        ::new(&module_extender_) symbolic_access::ModuleExtender(0, {}, {});

        module_extender_ = std::move(other.module_extender_);
    }

    ~Pdber() {

    }

    internal::StructMap Struct() {
        return internal::StructMap{ &module_extender_, nullptr };
    }

    internal::StructMap Struct(void* struct_ptr) {
        return internal::StructMap{ &module_extender_, struct_ptr };
    }

    internal::SymbolMap& Symbol() {
        return symbol_map_;
    }

    void SetBase() {

    }

private:
    union {
        symbolic_access::ModuleExtender module_extender_;
    };
    internal::SymbolMap symbol_map_;
};

using FileStream = symbolic_access::FileStream;

} // pdbuilder

#endif