#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <Util/StringUtils.hpp>

namespace Generator {
    struct ASMRegister {
        /* 
            FLAGS: XX AA BBBB
            
            A: size[ 0=char(1 byte),  1=short(2 byte),  2=int(4 byte),  3=long(8 byte) ];
            B: register[  0-7(rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp),  8-15(r8-r15)  ];
            X: UNUSED
        */
        char register_flags;
        static const char* registerStrs[];

        void print(std::ostream& out);
        static ASMRegister* fromString(std::string from);
    };

    enum class ASMStatementParamType {
        REGISTER, INT_LIT, LABEL
    };

    struct ASMStatementParam {
        ASMStatementParamType type;
        union {
            ASMRegister* register_;
            unsigned int int_lit;
            char* label;
        } u;

        void print(std::ostream& out);
        static ASMStatementParam* register_(std::string from);
        static ASMStatementParam* int_lit(unsigned int from);
        static ASMStatementParam* label(std::string from);
    };
    
    enum class ASMStatementType {
        MOV, RET, PUSH, POP, SPACE_, CALL
    };

    struct ASMStatement {
        ASMStatementType type;
        ASMStatementParam* param1;
        ASMStatementParam* param2;

        void print(std::ostream& out);
        static ASMStatement* mov(ASMStatementParam* dest, ASMStatementParam* from);
        static ASMStatement* call(ASMStatementParam* toCall);
        static ASMStatement* push(ASMStatementParam* toPush);
        static ASMStatement* pop(ASMStatementParam* toPop);
        static ASMStatement* ret();
        static ASMStatement* space_();
    };

    struct ASMFunction {
        std::string name;
        std::vector<ASMStatement*> statements;

        void print(std::ostream& out);
    };

    struct ASMGlobalStatement {
        std::vector<ASMFunction*> functions;
        std::string entryFunctionName;

        void print(std::ostream& out);
    };
}
