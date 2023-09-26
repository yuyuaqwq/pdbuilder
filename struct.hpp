#ifndef PDBUILDER_STRUCT_HPP_
#define PDBUILDER_STRUCT_HPP_

namespace pdbuilder {
namespace internal {

class StructMember {
public:
    StructMember(std::string_view struct_name, std::string_view member_name, symbolic_access::ModuleExtender* module_extender) :
        struct_name_{ struct_name },
        member_name_{ member_name },
        module_extender_{ module_extender } {

    }
    ~StructMember() {

    }

    std::optional<size_t> Offset() {
        return module_extender_->GetOffset(struct_name_, member_name_);
    }

private:
    std::string_view struct_name_;
    std::string_view member_name_;
    symbolic_access::ModuleExtender* module_extender_;
};

class Struct {
public:
    Struct(std::string_view struct_name, symbolic_access::ModuleExtender* module_extender) : struct_name_{ struct_name }, module_extender_{ module_extender } {

    }
    ~Struct() {

    }

    StructMember operator[](std::string_view member_name) {
        return StructMember{ struct_name_ , member_name, module_extender_ };
    }

private:
    std::string_view struct_name_;
    symbolic_access::ModuleExtender* module_extender_;
};

class StructMap {
public:
    StructMap(symbolic_access::ModuleExtender* module_extender) : module_extender_{ module_extender } {

    }
    ~StructMap() {

    }

    Struct operator[](std::string_view struct_name) {
        return Struct{ struct_name , module_extender_ };
    }

private:
    symbolic_access::ModuleExtender* module_extender_;
};

}  // namespace internal

}  // namespace symbol 



#endif // PDBUILDER_STRUCT_HPP_