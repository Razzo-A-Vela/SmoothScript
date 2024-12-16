#include "Generator.hpp"

namespace Generator {
    void Generator::addStatement(ASMStatement* asmStatement) {
        currFunction->statements.push_back(asmStatement);
    }

    void Generator::pushStackFrame() {
        addStatement(ASMStatement::push(ASMStatementParam::register_("rbp")));
        addStatement(ASMStatement::mov(ASMStatementParam::register_("rbp"), ASMStatementParam::register_("rsp")));
    }

    void Generator::popStackFrame() {
        addStatement(ASMStatement::mov(ASMStatementParam::register_("rsp"), ASMStatementParam::register_("rbp")));
        addStatement(ASMStatement::pop(ASMStatementParam::register_("rbp")));
    }

    std::string Generator::getLabelFromFuncDecl(FunctionDeclaration* funcDecl) {
        //TODO: Parameters
        return funcDecl->name + std::string("§§") + dataTypeToString(funcDecl->returnType);
    }

    std::string Generator::dataTypeToString(DataType* dataType) {
        switch (dataType->type) {
            case DataTypeT::VOID :
                return std::string("v");

            case DataTypeT::BYTE_TYPE :
                char* buf = new char[16];
                itoa(dataType->u.integer, buf, 10);
                return std::string(buf);
        }

        Utils::error("Internal Error", "Invalid dataType (how?)");
    }


    void Generator::processExpression(Expression* expression) {
        if (expression == NULL)
            return;
        
        if (expression->type == ExpressionType::LITERAL) {
            if (expression->u.literal.type != Tokenizer::LiteralType::integer)
                Utils::error("Internal Error", "Expected integer literal");

            addStatement(ASMStatement::mov(ASMStatementParam::register_("eax"), ASMStatementParam::int_lit(expression->u.literal.u.integer)));
        
        } else if (expression->type == ExpressionType::FUNC_CALL) {
            //TODO: Make the calling convention be customizable (maybe in the config?)
            //TODO: Parameters
            addStatement(ASMStatement::call(ASMStatementParam::label(getLabelFromFuncDecl(expression->u.funcDecl))));
        }
    }

    void Generator::processStatement(Statement* statement) {
        ASMStatement* ret = new ASMStatement();

        if (statement->type == StatementType::SCOPE) {
            for (Statement* currStatement : statement->u.scope->statements)
                processStatement(currStatement);
            return;
        
        } else if (statement->type == StatementType::RETURN) {
            processExpression(statement->u.expression);
            popStackFrame();
            addStatement(ASMStatement::ret());
        
        } else if (statement->type == StatementType::EXPRESSION) {
            processExpression(statement->u.expression);
        }

        addStatement(ASMStatement::space_());
    }

    void Generator::processFunc(Function* func) {
        if (!func->hasDefinition)
            Utils::error("Internal Error", "Generator should only see Defined functions");
        currFunction = new ASMFunction();

        currFunction->name = getLabelFromFuncDecl(func->funcDecl);
        pushStackFrame();
        addStatement(ASMStatement::space_());
        
        processStatement(func->scopeStatement);

        popStackFrame();
        addStatement(ASMStatement::ret());
    }

    void Generator::process() {
        ASMGlobalStatement asmGlobalStatement;

        while (hasPeek()) {
            GlobalStatement globalStatement = consume().value();
            
            if (globalStatement.type == GlobalStatementType::FUNC) {
                processFunc(globalStatement.u.func);
                asmGlobalStatement.functions.push_back(currFunction);
            }
        }

        asmGlobalStatement.entryFunctionName = "main§§4";
        addToOutput(asmGlobalStatement);
    }

    void Generator::print(std::ostream& out) {
        ASMGlobalStatement output = getSingleOutput();
        output.print(out);
    }
}
