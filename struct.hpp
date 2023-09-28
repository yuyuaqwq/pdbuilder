#ifndef PDBUILDER_STRUCT_HPP_
#define PDBUILDER_STRUCT_HPP_

#include <memory>

#include <fustd/generic/varient.hpp>

namespace pdbuilder {
namespace internal {
    
class StructMap;

class StructMember {
public:
    StructMember(std::string_view struct_name, std::string_view member_name, symbolic_access::ModuleExtender* module_extender, void* struct_ptr) :
        struct_name_{ struct_name },
        member_name_{ member_name },
        module_extender_{ module_extender },
        struct_ptr_{ struct_ptr }, 
        varient { nullptr }
    {

    }
    ~StructMember() {

    }

    std::optional<size_t> Offset() const {
        return module_extender_->GetOffset(struct_name_, member_name_);
    }

    fustd::Varient* operator->() {
        auto offset = Offset();
        varient.Reset(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(struct_ptr_) + offset.value()));
        return &varient;
    }

    template<typename T=StructMap>
    T SubStruct() {
        auto offset = Offset();
        return T{ module_extender_, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(struct_ptr_) + offset.value()) };
    }

private:
    fustd::Varient varient;
    void* struct_ptr_;
    std::string_view struct_name_;
    std::string_view member_name_;
    symbolic_access::ModuleExtender* module_extender_;
};

class Struct {
public:
    Struct(std::string_view struct_name, symbolic_access::ModuleExtender* module_extender, void* struct_ptr) : 
        struct_name_{ struct_name }, 
        module_extender_{ module_extender },
        struct_ptr_{ struct_ptr }
    {

    }
    ~Struct() {

    }

    StructMember operator[](std::string_view member_name) {
        return StructMember{ struct_name_ , member_name, module_extender_, struct_ptr_ };
    }

private:
    void* struct_ptr_;
    std::string_view struct_name_;
    symbolic_access::ModuleExtender* module_extender_;
};

class StructMap {
public:
    StructMap(symbolic_access::ModuleExtender* module_extender, void* struct_ptr) : 
        module_extender_{ module_extender },
        struct_ptr_{ struct_ptr }
    {

    }
    ~StructMap() {

    }

    Struct operator[](std::string_view struct_name) {
        return Struct{ struct_name , module_extender_, struct_ptr_ };
    }

private:
    symbolic_access::ModuleExtender* module_extender_;
    void* struct_ptr_;
};

}  // namespace internal

}  // namespace symbol 



#endif // PDBUILDER_STRUCT_HPP_