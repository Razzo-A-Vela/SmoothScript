#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>

#include "util.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "nodes.hpp"


class Generator {
public:
  Generator(Node::Base _node, bool _mainFile = true, int _labelIndex = 0, std::string _labelName = "", std::string _alias = "") : node(_node), mainFile(_mainFile), labelIndex(_labelIndex), labelName(_labelName), alias(_alias) {}

  void genFunc(const Node::Func* funcNode) {
    Func func = getFunc(funcNode->name).value();

    for (Node::Expr* expr : funcNode->exprs) {
      genExpr(expr);
      push("rax");
    }

    output << "  call " << func.label << "\n";
    pop("", funcNode->exprs.size() * 8);
  }


  void genScope(const Node::StmtScope* scopeNode, std::string breakLabel = "", std::string continueLabel = "") {
    Scope scope { .varOffset = vars.size(), .stackOffset = stackPoint, .type = scopeNode->type, .breakLabel = breakLabel, .continueLabel = continueLabel };
    scopes.push_back(scope);
    for (Node::Stmt* stmt : scopeNode->stmts) genStmt(stmt);

    scopes.pop_back();
    while (vars.size() > scope.varOffset)
      vars.pop_back();
    popVariables(scope.stackOffset);
  }


  void genBinExpr(const Node::ExprBin* binExprNode, std::string reg) {
    genExpr(binExprNode->left);
    push("rax");
    output << "\n";
    genExpr(binExprNode->right, "rbx");
    pop("rax");

    if (binExprNode->type == ExprBinType::add)
      output << "  add rax, rbx\n";

    else if (binExprNode->type == ExprBinType::sub)
      output << "  sub rax, rbx\n";

    else if (binExprNode->type == ExprBinType::mult)
      output << "  mul rbx\n";

    else if (binExprNode->type == ExprBinType::div) {
      output << "  mov rdx, 0\n";
      output << "  div rbx\n";

    } else if (binExprNode->type == ExprBinType::mod) {
      output << "  mov rdx, 0\n";
      output << "  div rbx\n";
      output << "  mov rax, rdx\n";

    } else if (isBoolBinExpr(binExprNode->type)) {
      std::string binExprTrueLabel = getLabel("bool_bin_expr_true");
      std::string binExprEndLabel = getLabel("bool_bin_expr_end");
      output << "  cmp rax, rbx\n";
      
      if (binExprNode->type == ExprBinType::eq)
        output << "  je " << binExprTrueLabel << "\n";

      else if (binExprNode->type == ExprBinType::not_eq_)
        output << "  jne " << binExprTrueLabel << "\n";

      else if (binExprNode->type == ExprBinType::less_eq) {
        output << "  je " << binExprTrueLabel << "\n";
        output << "  jl " << binExprTrueLabel << "\n";

      } else if (binExprNode->type == ExprBinType::greater_eq) {
        output << "  je " << binExprTrueLabel << "\n";
        output << "  jg " << binExprTrueLabel << "\n";

      } else if (binExprNode->type == ExprBinType::greater) {
        output << "  jg " << binExprTrueLabel << "\n";

      } else if (binExprNode->type == ExprBinType::less) {
        output << "  jl " << binExprTrueLabel << "\n";
      }
      
      output << "  mov rax, 0\n";
      output << "  jmp " << binExprEndLabel << "\n";
      output << binExprTrueLabel << ":\n";
      output << "  mov rax, 1\n";
      output << binExprEndLabel << ":\n";

    }

    if (reg != "rax") output << "  mov " << reg << ", rax\n";
  }


  void genExpr(Node::Expr* expr, std::string reg_ = "rax") {
    struct Visitor {
      Generator* gen;
      std::string reg;
      Visitor(Generator* _gen, std::string _reg) : gen(_gen), reg(_reg) {}

      void operator()(const Node::ExprIntLit* intLitNode) {
        gen->output << "  mov " << reg << ", ";
        gen->output << intLitNode->value << "\n";
      }


      void operator()(const Node::ExprVar* varNode) {
        gen->output << "  mov " << reg << ", ";
        gen->output << "[rsp + " << gen->stackPoint - gen->getVar(varNode->name).value().stackOffset << "]\n";
      }


      void operator()(const Node::ExprBin* binExprNode) {
        gen->genBinExpr(binExprNode, reg);
      }


      void operator()(const Node::ExprCharLit* charLitNode) {
        gen->output << "  mov " << reg << ", ";
        gen->output << charLitNode->value << "\n";
      }


      void operator()(const Node::ExprGet* getNode) {
        gen->call("getchar", reg);
      }


      void operator()(const Node::Func* funcNode) {
        gen->genFunc(funcNode);
        if (reg != "rax") gen->output << "  mov " << reg << ", rax\n";
      }
    };

    Visitor visitor(this, reg_);
    std::visit(visitor, expr->var);
    output << ";;;;;;;;;;;;; ";

    if (std::holds_alternative<Node::ExprIntLit*>(expr->var)) output << "intLit";
    else if (std::holds_alternative<Node::ExprVar*>(expr->var)) output << "var";
    else if (std::holds_alternative<Node::ExprBin*>(expr->var)) output << "bin";
    else if (std::holds_alternative<Node::ExprCharLit*>(expr->var)) output << "charLit";
    else if (std::holds_alternative<Node::ExprGet*>(expr->var)) output << "get";
    else if (std::holds_alternative<Node::Func*>(expr->var)) output << "func";

    output << " expr ;;;;;;;;;;;;;\n";
  }


  void genStmt(Node::Stmt* stmt) {
    struct Visitor {
      Generator* gen;
      Visitor(Generator* _gen) : gen(_gen) {}

      void operator()(const Node::StmtExit* exitNode) {
        gen->genExpr(exitNode->expr, "rcx");
        gen->popVariables(0, false);
        gen->call("exit");
      }


      void operator()(const Node::StmtVar* varNode) {
        if (varNode->expr.has_value()) gen->genExpr(varNode->expr.value());
        else gen->output << "  mov rax, 0\n";

        if (varNode->reAssign) {
          Var var = gen->getVar(varNode->name).value();
          gen->output << "  mov [rsp + " << gen->stackPoint - var.stackOffset << "], rax\n";
          
          return;
        }

        if (gen->getVar(varNode->name, false).has_value()) gen->genErr("Identifier '" + varNode->name + "' alredy exists");
        gen->push("rax");
        gen->vars.push_back({ .name = varNode->name, .stackOffset = gen->stackPoint });
      }


      void operator()(const Node::StmtScope* scopeNode) {
        gen->genScope(scopeNode);
      }


      void operator()(const Node::StmtIf* ifNode) {
        std::string endIfLabel = gen->getLabel("end_if");

        gen->genExpr(ifNode->expr);
        gen->jumpIfFalse(endIfLabel);
        gen->genScope(ifNode->scope, gen->getLastBreak(), gen->getLastContinue());
        gen->output << endIfLabel << ":\n";
      }


      void operator()(const Node::StmtWhile* whileNode) {        
        std::string whileLoopLabel = gen->getLabel("while_loop");
        std::string endWhileLabel = gen->getLabel("end_while");

        gen->output << whileLoopLabel << ":\n";
        gen->genExpr(whileNode->expr);
        gen->jumpIfFalse(endWhileLabel);
        
        gen->genScope(whileNode->scope, endWhileLabel, whileLoopLabel);
        gen->output << "  jmp " << whileLoopLabel << "\n";
        gen->output << endWhileLabel << ":\n";
      }


      void operator()(const Node::StmtBreak* breakNode) {
        std::string breakLabel = gen->getLastBreak();
        if (breakLabel == "") gen->genErr("Cannot use 'break' while not in a loop");

        Scope scope = gen->getLastScope(ScopeType::loop);
        gen->popVariables(scope.stackOffset, false);
        gen->output << "  jmp " << breakLabel << "\n";
      }


      void operator()(const Node::StmtContinue* continueNode) {
        std::string continueLabel = gen->getLastContinue();
        if (continueLabel == "") gen->genErr("Cannot use 'continue' while not in a loop");

        Scope scope = gen->getLastScope(ScopeType::loop);
        gen->popVariables(scope.stackOffset, false);
        gen->output << "  jmp " << continueLabel << "\n";
      }


      void operator()(const Node::StmtFor* forNode) {
        std::string forLoopLabel = gen->getLabel("for_loop");
        std::string endforLabel = gen->getLabel("end_for");

        gen->genStmt(forNode->once);
        gen->output << forLoopLabel << ":\n";
        gen->genExpr(forNode->condition);
        gen->jumpIfFalse(endforLabel);
        gen->genStmt(forNode->repeat);

        gen->genScope(forNode->scope, endforLabel, forLoopLabel);
        gen->output << "  jmp " << forLoopLabel << "\n";
        gen->output << endforLabel << ":\n";
      }

      
      void operator()(const Node::StmtPut* putNode) {
        gen->genExpr(putNode->expr, "rcx");
        gen->call("putchar");
      }


      void operator()(const Node::StmtMain* mainNode) {
        if (gen->hasMain) gen->genErr("Main alredy defined");
        if (gen->mainFile) gen->output << "main:\n";
        else gen->output << gen->labelName + ":\n";
        if (!gen->mainFile && mainNode->enforceMain) gen->stop = true;
        
        for (std::string extend : gen->extends) gen->output << "  call " << extend << "\n";
        gen->hasMain = true;
      }


      void operator()(const Node::StmtDefFunc* defFuncNode) {
        if (gen->hasMain) gen->genErr("Cannot define a function inside main");
        if (gen->getFunc(defFuncNode->name, false).has_value()) gen->genErr("Trying to redefine function '" + defFuncNode->name + "'");
        if (gen->currentFuncName != "") gen->genErr("Cannot define a function inside a function");
        else gen->currentFuncName = defFuncNode->name;

        std::string labelName;
        if (gen->alias == "") labelName = defFuncNode->name;
        else labelName = gen->alias + "_" + defFuncNode->name;
        std::string label = gen->getLabel(labelName);
        gen->funcs.push_back({ .name = defFuncNode->name, .label = label, .params = defFuncNode->params });

        for (int i = 0; i < defFuncNode->params.size(); i++) {
          gen->stackPoint += 8;
          gen->vars.push_back({ .name = defFuncNode->params.at(i), .stackOffset = gen->stackPoint });
        }

        gen->stackPoint += 8;
        gen->output << label << ":\n";
        gen->genScope(defFuncNode->scope);
        gen->stackPoint -= 8;

        for (int i = 0; i < defFuncNode->params.size(); i++) {
          gen->vars.pop_back();
          gen->stackPoint -= 8;
        }

        gen->output << "  mov rax, 0\n";
        gen->output << "  ret\n";
        gen->currentFuncName = "";
      }


      void operator()(const Node::StmtReturn* returnNode) {
        if (gen->currentFuncName == "") gen->genErr("Return can only be used in functions");
        Scope scope = gen->getLastScope(ScopeType::func);
        Func func = gen->getFunc(gen->currentFuncName).value();

        if (returnNode->expr.has_value()) gen->genExpr(returnNode->expr.value());
        else gen->output << "  mov rax, 0";
        
        gen->popVariables(scope.stackOffset, false);
        gen->output << "  ret\n";
      }


      void operator()(const Node::Func* funcNode) {
        gen->genFunc(funcNode);
      }


      void operator()(const Node::StmtExtend* extendNode) {
        if (gen->hasMain) gen->genErr("Cannot extend a file after main");
        std::string contents;
        {
          std::fstream fileStream(extendNode->fileName, std::ios::in);
          std::stringstream stream;
          stream << fileStream.rdbuf();
          contents = stream.str();
        }
        std::string mainLabel = gen->getLabel(extendNode->fileName.substr(0, extendNode->fileName.size() - 4) + "_main");
        gen->extends.push_back(mainLabel);

        Tokenizer tokenizer(contents);
        Parser parser(tokenizer.tokenize());
        Generator generator(parser.parse(), false, gen->labelIndex, mainLabel, extendNode->alias);
        gen->output << generator.generate() << "\n";
        gen->output << ";;;;;;;;;;;;; " << extendNode->fileName << " ;;;;;;;;;;;;;\n";
        gen->addFromExtend(&generator, extendNode->alias);
      }
    };

    Visitor visitor(this);
    std::visit(visitor, stmt->var);
    output << ";;;;;;;;;;;;; ";

    if (std::holds_alternative<Node::StmtExit*>(stmt->var)) output << "exit";
    else if (std::holds_alternative<Node::StmtVar*>(stmt->var)) output << "var";
    else if (std::holds_alternative<Node::StmtScope*>(stmt->var)) output << "scope";
    else if (std::holds_alternative<Node::StmtIf*>(stmt->var)) output << "if";
    else if (std::holds_alternative<Node::StmtWhile*>(stmt->var)) output << "while";
    else if (std::holds_alternative<Node::StmtBreak*>(stmt->var)) output << "break";
    else if (std::holds_alternative<Node::StmtContinue*>(stmt->var)) output << "continue";
    else if (std::holds_alternative<Node::StmtFor*>(stmt->var)) output << "for";
    else if (std::holds_alternative<Node::StmtPut*>(stmt->var)) output << "put";
    else if (std::holds_alternative<Node::StmtMain*>(stmt->var)) output << "main";
    else if (std::holds_alternative<Node::StmtDefFunc*>(stmt->var)) output << "defFunc";
    else if (std::holds_alternative<Node::StmtReturn*>(stmt->var)) output << "return";
    else if (std::holds_alternative<Node::Func*>(stmt->var)) output << "func";
    else if (std::holds_alternative<Node::StmtExtend*>(stmt->var)) output << "extend";

    output << " stmt ;;;;;;;;;;;;;\n\n";
  }

  std::string generate() {
    if (mainFile) output << "global main\nextern putchar\nextern getchar\nextern exit\nsection .text\n\n";

    for (Node::Stmt* stmt : node.stmts) {
      genStmt(stmt);
      if (stop) break;
    }
    if (!hasMain) genErr("Expected main");
    if (scopes.size() != 0) genErr("Expected '}'");
    popVariables();

    output << "  mov rax, 0\n  ret\n";
    return output.str();
  }

private:
  void push(std::string reg, int byteSize = 8, bool update = true) {
    if (update) stackPoint += byteSize;

    output << "  sub rsp, " << byteSize << "\n";
    if (reg != "") output << "  mov [rsp + 0], " << reg << "\n";
    output << "; push " << reg << "\n";
  }

  void pop(std::string reg, int byteSize = 8, bool update = true) {
    if (update) stackPoint -= byteSize;

    if (reg != "") output << "  mov " << reg << ", [rsp + 0]\n";
    output << "  add " << "rsp, " << byteSize << "\n";
    output << "; pop " << reg << "\n";
  }

  void popVariables(int offset = 0, bool update = true) {
    pop("", stackPoint - offset, update);
  }

  void genErr(std::string msg) {
    if (mainFile) err(msg);

    std::string name = labelName;
    std::size_t start = name.find("_main__");
    name.replace(start, name.size() - start, ".smt");
    err(msg + " in file: " + name);
  }

  std::string getLabel(std::string name) {
    std::stringstream out;
    out << name << "__" << labelIndex++;
    return out.str();
  }

  void jumpIfFalse(std::string label) {
    output << "  mov rbx, 0\n";
    output << "  cmp rax, rbx\n";
    output << "  je " << label << "\n";
  }

  void call(std::string name, std::string reg = "rax") {
    push("", 40);
    output << "  call " << name << "\n";
    if (reg != "rax") output << "  mov " << reg << ", rax\n";
    pop("", 40);
  }


  struct Var {
    std::string name;
    int stackOffset;
  };

  struct Scope {
    size_t varOffset;
    int stackOffset;
    ScopeType type;
    std::string breakLabel;
    std::string continueLabel;
  };

  struct Func {
    std::string name;
    std::string label;
    std::vector<std::string> params;
  };

  std::optional<Var> getVar(std::string name, bool doThrow = true) {
    for (Var var : vars) {
      if (var.name == name)
        return var;
    }
    
    if (doThrow) genErr("No variable with identifier '" + name + "' found");
    return {};
  }

  std::optional<Func> getFunc(std::string name, bool doThrow = true) {
    for (Func func : funcs) {
      if (func.name == name)
        return func;
    }
    
    if (doThrow) genErr("No function with identifier '" + name + "' found");
    return {};
  }

  void addFromExtend(Generator* gen, std::string alias) {
    for (Var var : gen->vars) {
      Var v = var;
      if (alias != "") v.name = alias + "#" + v.name;
      vars.push_back(v);
    }

    for (Func func : gen->funcs) {
      Func f = func;
      if (alias != "") f.name = alias + "#" + f.name;
      funcs.push_back(f);
    }
    labelIndex = gen->labelIndex;
  }

  Scope getLastScope(ScopeType type = ScopeType::generic, std::string err = "") {
    for (int i = scopes.size() - 1; i >= 0; i--) {
      if (type == ScopeType::generic) return scopes.at(i);
      else if (scopes.at(i).type == type) return scopes.at(i);
    }
    if (err != "") genErr(err);
    return { .stackOffset = -1 };
  }

  std::string getLastBreak() {
    Scope scope = getLastScope(ScopeType::loop);
    if (scope.stackOffset == -1) return "";
    return scope.breakLabel;
  }

  std::string getLastContinue() {
    Scope scope = getLastScope(ScopeType::loop);
    if (scope.stackOffset == -1) return "";
    return scope.continueLabel;
  }


  Node::Base node;
  std::stringstream output;
  std::vector<Var> vars;
  std::vector<Scope> scopes;
  std::vector<Func> funcs;
  std::vector<std::string> extends;
  std::string labelName;
  std::string alias;
  std::string currentFuncName = "";
  bool mainFile;
  int labelIndex;
  int stackPoint = 0;
  bool hasMain = false;
  bool stop = false;
};
