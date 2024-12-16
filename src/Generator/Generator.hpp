#pragma once

#include "GeneratorNodes.hpp"

#include <Processor/Processor.hpp>
#include <Tokenizer/Literal.hpp>
#include <Parser/ParseNodes.hpp>

namespace Generator {
    using namespace Parser;

    class Generator : public Processor<GlobalStatement, ASMGlobalStatement> {
    private:
        std::vector<GlobalStatement> globalStatements;
        ASMFunction* currFunction;

    public:
        Generator(std::vector<GlobalStatement> globalStatements) : globalStatements(globalStatements), Processor(globalStatements.size()) {}
        virtual void process();
        virtual void print(std::ostream& out);

    protected:
        virtual GlobalStatement get(int index) { return globalStatements.at(index); }
        void processFunc(Function* function);
        void processStatement(Statement* statement);
        void processExpression(Expression* expression);

        std::string dataTypeToString(DataType* dataType);
        std::string getLabelFromFuncDecl(FunctionDeclaration* funcDecl);
        void popStackFrame();
        void pushStackFrame();
        void addStatement(ASMStatement* asmStatement);
    };
}
