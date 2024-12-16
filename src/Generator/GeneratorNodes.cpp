#include "GeneratorNodes.hpp"

namespace Generator {
    const char* ASMRegister::registerStrs[] = { "ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
                                                "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };

    void ASMRegister::print(std::ostream& out) {
        char size = (register_flags & 0b00110000) >> 4;
        char register_ = register_flags & 0b00001111;
        std::string registerStr = std::string(registerStrs[register_]);

        if (register_ < 8) {            // rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp
            if (size == 3)              // 64 bits  (8 bytes)
                out << 'r' << registerStr;
            else if (size == 2)         // 32 bits  (4 bytes)
                out << 'e' << registerStr;
            else if (size == 1)         // 16 bits  (2 bytes)
                out << registerStr;
            else {                      // 8 bits   (1 byte)
                if (register_ < 4)
                    out << registerStr.at(0);
                else 
                    out << registerStr;
                out << 'l';
            }
            
        } else {                        // r(number)
            if (size == 3)              // 64 bits  (8 bytes)
                out << registerStr;
            else if (size == 2)         // 32 bits  (4 bytes)
                out << registerStr << 'd';
            else if (size == 1)         // 16 bits  (2 bytes)
                out << registerStr << 'w';
            else                        // 8 bits   (1 byte)
                out << registerStr << 'b';
        }
    }

    ASMRegister* ASMRegister::fromString(std::string from) {
        if (from == "al") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000000;
            return ret;

        } else if (from == "bl") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000001;
            return ret;
        
        } else if (from == "cl") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000010;
            return ret;
        
        } else if (from == "dl") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000011;
            return ret;

        } else if (from == "sil") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000100;
            return ret;
        
        } else if (from == "dil") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000101;
            return ret;
            
        } else if (from == "bpl") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000110;
            return ret;
        
        } else if (from == "spl") {
            ASMRegister* ret = new ASMRegister();
            ret->register_flags = 0b00000111;
            return ret;
        }
        
        std::string register_ = from.substr(1);
        for (int i = 0; i < 8; i++) {
            if (register_ == std::string(registerStrs[i])) {
                ASMRegister* ret = new ASMRegister();
                ret->register_flags = i;
                ret->register_flags |= (from.at(0) == 'r' ? 3 : (from.at(0) == 'e' ? 2 : 1)) << 4;
                return ret;
            }
        }

        register_ = from.substr(0, 2);
        for (int i = 8; i < 10; i++) {
            if (register_ == std::string(registerStrs[i])) {
                ASMRegister* ret = new ASMRegister();
                ret->register_flags = i;
                ret->register_flags |= (from.ends_with("b") ? 0 : (from.ends_with("w") ? 1 : (from.ends_with("d") ? 2 : 3))) << 4;
                return ret;
            }
        }

        register_ = from.substr(0, 3);
        for (int i = 10; i < 16; i++) {
            if (register_ == std::string(registerStrs[i])) {
                ASMRegister* ret = new ASMRegister();
                ret->register_flags = i;
                ret->register_flags |= (from.ends_with("b") ? 0 : (from.ends_with("w") ? 1 : (from.ends_with("d") ? 2 : 3))) << 4;
                return ret;
            }
        }

        return NULL;
    }


    void ASMStatementParam::print(std::ostream& out) {
        switch (type) {
            case ASMStatementParamType::INT_LIT :
                out << u.int_lit;
                break;
            
            case ASMStatementParamType::REGISTER :
                u.register_->print(out);
                break;
            
            case ASMStatementParamType::LABEL :
                out << std::string(u.label);
                break;
        }
    }

    ASMStatementParam* ASMStatementParam::register_(std::string from) {
        ASMStatementParam* ret = new ASMStatementParam();
        ret->type = ASMStatementParamType::REGISTER;
        ret->u.register_ = ASMRegister::fromString(from);
        return ret;
    }

    ASMStatementParam* ASMStatementParam::int_lit(unsigned int from) {
        ASMStatementParam* ret = new ASMStatementParam();
        ret->type = ASMStatementParamType::INT_LIT;
        ret->u.int_lit = from;
        return ret;
    }

    ASMStatementParam* ASMStatementParam::label(std::string from) {
        ASMStatementParam* ret = new ASMStatementParam();
        ret->type = ASMStatementParamType::LABEL;
        ret->u.label = Utils::stringToCString(from);
        return ret;
    }

    
    void ASMStatement::print(std::ostream& out) {
        switch (type) {
            case ASMStatementType::MOV :
                out << "mov ";
                param1->print(out);
                out << ", ";
                param2->print(out);
                break;
            
            case ASMStatementType::PUSH :
                out << "push ";
                param1->print(out);
                break;
            
            case ASMStatementType::POP :
                out << "pop ";
                param1->print(out);
                break;

            case ASMStatementType::RET :
                out << "ret";
                break;
            
            case ASMStatementType::SPACE_ :
                break;
            
            case ASMStatementType::CALL :
                out << "call ";
                param1->print(out);
                break;
        }
    }

    ASMStatement* ASMStatement::mov(ASMStatementParam* dest, ASMStatementParam* from) {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::MOV;
        ret->param1 = dest;
        ret->param2 = from;
        return ret;
    }

    ASMStatement* ASMStatement::call(ASMStatementParam* toCall) {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::CALL;
        ret->param1 = toCall;
        ret->param2 = NULL;
        return ret;
    }

    ASMStatement* ASMStatement::push(ASMStatementParam* toPush) {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::PUSH;
        ret->param1 = toPush;
        ret->param2 = NULL;
        return ret;
    }

    ASMStatement* ASMStatement::pop(ASMStatementParam* toPop) {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::POP;
        ret->param1 = toPop;
        ret->param2 = NULL;
        return ret;
    }

    ASMStatement* ASMStatement::ret() {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::RET;
        ret->param1 = NULL;
        ret->param2 = NULL;
        return ret;
    }

    ASMStatement* ASMStatement::space_() {
        ASMStatement* ret = new ASMStatement();
        ret->type = ASMStatementType::SPACE_;
        ret->param1 = NULL;
        ret->param2 = NULL;
        return ret;
    }


    void ASMFunction::print(std::ostream& out) {
        out << name << ":\n";
        for (ASMStatement* asmStatement : statements) {
            out << "  ";
            asmStatement->print(out);
            out << '\n';
        }
    }


    void ASMGlobalStatement::print(std::ostream& out) {
        out << "global main\nsection .text\n\n";
        
        for (ASMFunction* asmFunction : functions) {
            asmFunction->print(out);
            out << '\n';
        }

        out << "main:\n  push rbp\n  mov rbp, rsp\n\n";
        out << "  call " << entryFunctionName << "\n";
        out << "\n  mov rsp, rbp\n  pop rbp\n  ret\n";
    }
}
