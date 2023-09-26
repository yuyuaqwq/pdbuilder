#ifndef PDBUILDER_SYMBOL_HPP_
#define PDBUILDER_SYMBOL_HPP_

namespace pdbuilder {
namespace internal {

class Symbol {
public:
    Symbol(std::string_view symbol, symbolic_access::ModuleExtender* module_extender) : symbol_{ symbol }, module_extender_{ module_extender } {

    }

    ~Symbol() {

    }

    std::optional<uint32_t> Rva() {
        return reinterpret_cast<uint32_t>(module_extender_->GetPointer<void>(symbol_));
    }
private:
    std::string_view symbol_;
    symbolic_access::ModuleExtender* module_extender_;
};

class SymbolMap {
public:
public:
    SymbolMap(symbolic_access::ModuleExtender* module_extender) : module_extender_{ module_extender } {

    }
    ~SymbolMap() {

    }

    Symbol operator[](std::string_view symbol) {
        return Symbol{ symbol, module_extender_ };
    }

private:
    symbolic_access::ModuleExtender* module_extender_;
};



}  // namespace internal

}  // namespace pdbuilder 


#endif // PDBUILDER_SYMBOL_HPP_