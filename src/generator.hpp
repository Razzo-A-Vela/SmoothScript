#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <optional>

#include "util.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "nodes.hpp"


class Generator {
public:
  Generator(Node::Base _node) : node(_node) {}

  void genFunc(const Node::Func* funcNode) {
    Func func = getFunc(funcNode->name).value();

    for (Node::Expr* expr : funcNode->exprs) {
      genExpr(expr);
      push("rax");
    }

    output << "  call " << func.label << "\n";
    pop("", funcNode->exprs.size() * 8);
  }


  Node::Stmt* genInsideScope(std::string breakLabel, std::string continueLabel) {
    index++;
    Node::Stmt* stmt;
    int scopeDepth = -1;
    for (; index < node.stmts.size(); index++) {
      stmt = node.stmts.at(index);

      if (std::holds_alternative<Node::StmtScope*>(stmt->var)) {
        Node::StmtScope* scopeStmt = std::get<Node::StmtScope*>(stmt->var);
        int depth = scopes.size();
        
        if (scopeStmt->close && depth == scopeDepth) break;
        else if (scopeDepth == -1) scopeDepth = depth + 1;

      } else if (scopeDepth == -1) scopeDepth = -2;
      
      genStmt(stmt, breakLabel, continueLabel);
      if (scopeDepth == -2) return NULL;
    }
    return stmt;
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
        gen->push("", 40);
        gen->output << "  call getchar\n";
        if (reg != "rax") gen->output << "  mov " << reg << ", rax\n";
        gen->pop("", 40);
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


  void genStmt(Node::Stmt* stmt, std::string breakLabel_ = "", std::string continueLabel_ = "") {
    struct Visitor {
      Generator* gen;
      std::string breakLabel;
      std::string continueLabel;
      Visitor(Generator* _gen, std::string _breakLabel, std::string _continueLabel) : gen(_gen), breakLabel(_breakLabel), continueLabel(_continueLabel) {}

      void operator()(const Node::StmtExit* exitNode) {
        gen->genExpr(exitNode->expr);
        gen->exit();

        gen->output << "  ret\n";
      }


      void operator()(const Node::StmtVar* varNode) {
        if (varNode->expr.has_value()) gen->genExpr(varNode->expr.value());
        else gen->output << "  mov rax, 0\n";

        if (varNode->reAssign) {
          Var var = gen->getVar(varNode->name).value();
          gen->output << "  mov [rsp + " << gen->stackPoint - var.stackOffset << "], rax\n";
          
          return;
        }

        if (gen->getVar(varNode->name, false).has_value()) err("Identifier '" + varNode->name + "' alredy exists");
        gen->push("rax");
        gen->vars.push_back({ .name = varNode->name, .stackOffset = gen->stackPoint });
      }


      void operator()(const Node::StmtScope* scopeNode) {
        if (scopeNode->close) {
          Scope scope;
          try {
            scope = gen->scopes.at(gen->scopes.size() - 1);
          } catch (std::out_of_range const& e) {
            err("Expected '{' before '}'");
          }
          gen->scopes.pop_back();

          while (gen->vars.size() > scope.varOffset)
            gen->vars.pop_back();
          gen->pop("", gen->stackPoint - scope.stackOffset);
          return;
        }

        gen->scopes.push_back({ .stackOffset = gen->stackPoint, .varOffset = gen->vars.size() });
      }


      void operator()(const Node::StmtIf* ifNode) {
        std::string endIfLabel = gen->getLabel("end_if");

        gen->genExpr(ifNode->expr);
        gen->jumpIfFalse(endIfLabel);

        Node::Stmt* scopeStmt = gen->genInsideScope(breakLabel, continueLabel);
        gen->output << endIfLabel << ":\n";

        if (scopeStmt != NULL) gen->genStmt(scopeStmt);
      }


      void operator()(const Node::StmtWhile* whileNode) {        
        std::string whileLoopLabel = gen->getLabel("while_loop");
        std::string endWhileLabel = gen->getLabel("end_while");

        gen->output << whileLoopLabel << ":\n";
        gen->genExpr(whileNode->expr);
        gen->jumpIfFalse(endWhileLabel);
        
        Node::Stmt* scopeStmt = gen->genInsideScope(endWhileLabel, whileLoopLabel);
        gen->output << "  jmp " << whileLoopLabel << "\n";
        gen->output << endWhileLabel << ":\n";

        if (scopeStmt != NULL) gen->genStmt(scopeStmt);
      }


      void operator()(const Node::StmtBreak* breakNode) {
        if (breakLabel == "") err("Cannot use 'break' while not in a loop");
        gen->output << "  jmp " << breakLabel << "\n";
      }


      void operator()(const Node::StmtContinue* continueNode) {
        if (continueLabel == "") err("Cannot use 'continue' while not in a loop");
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

        Node::Stmt* scopeStmt = gen->genInsideScope(endforLabel, forLoopLabel);
        gen->output << "  jmp " << forLoopLabel << "\n";
        gen->output << endforLabel << ":\n";

        if (scopeStmt != NULL) gen->genStmt(scopeStmt);
      }

      
      void operator()(const Node::StmtPut* putNode) {
        gen->genExpr(putNode->expr, "rcx");
        gen->push("", 40);
        gen->output << "  call putchar\n";
        gen->pop("", 40);
      }


      void operator()(const Node::StmtMain* mainNode) {
        if (gen->hasMain) err("Main alredy defined");
        gen->output << "main:\n";
        gen->hasMain = true;
      }


      void operator()(const Node::StmtDefFunc* defFuncNode) {
        if (gen->hasMain) err("Cannot define a function inside main");
        if (gen->getFunc(defFuncNode->name, false).has_value()) err("Trying to redefine function '" + defFuncNode->name + "'");
        if (gen->currentFuncName != "") err("Cannot define a function inside a function");
        else gen->currentFuncName = defFuncNode->name;
        std::string label = gen->getLabel(defFuncNode->name);
        gen->funcs.push_back({ .name = defFuncNode->name, .label = label, .params = defFuncNode->params });

        for (int i = 0; i < defFuncNode->params.size(); i++) {
          gen->stackPoint += 8;
          gen->vars.push_back({ .name = defFuncNode->params.at(i), .stackOffset = gen->stackPoint });
        }

        gen->stackPoint += 8;
        gen->output << label << ":\n";
        Node::Stmt* scopeStmt = gen->genInsideScope("", "");
        if (scopeStmt == NULL) err("Expected scope after function declaration of '" + defFuncNode->name + "'");
        gen->genStmt(scopeStmt);
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
        if (gen->currentFuncName == "") err("Return can only be used in functions");
        Scope scope = gen->scopes.at(gen->scopes.size() - 1);
        Func func = gen->getFunc(gen->currentFuncName).value();

        if (returnNode->expr.has_value()) gen->genExpr(returnNode->expr.value());
        else gen->output << "  mov rax, 0";
        
        gen->pop("", gen->stackPoint - scope.stackOffset);
        gen->output << "  ret\n";
      }


      void operator()(const Node::Func* funcNode) {
        gen->genFunc(funcNode);
      }
    };

    Visitor visitor(this, breakLabel_, continueLabel_);
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

    output << " stmt ;;;;;;;;;;;;;\n\n";
  }

  std::string generate() {
    output << "global main\nextern putchar\nextern getchar\nsection .text\n\n";

    for (; index < node.stmts.size(); index++) {
      genStmt(node.stmts.at(index));
    }
    if (!hasMain) err("Expected main");
    if (scopes.size() != 0) err("Expected '}'");
    exit();

    output << "  mov rax, 0\n  ret\n";
    return output.str();
  }

private:
  void push(std::string reg, int byteSize = 8) {
    stackPoint += byteSize;

    output << "  sub rsp, " << byteSize << "\n";
    if (reg != "") output << "  mov [rsp + 0], " << reg << "\n";
    output << "; push " << reg << "\n";
  }

  void pop(std::string reg, int byteSize = 8) {
    stackPoint -= byteSize;

    if (reg != "") output << "  mov " << reg << ", [rsp + 0]\n";
    output << "  add " << "rsp, " << byteSize << "\n";
    output << "; pop " << reg << "\n";
  }

  void exit() {
    pop("", stackPoint);
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


  struct Var {
    std::string name;
    int stackOffset;
  };

  struct Scope {
    int stackOffset;
    size_t varOffset;
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
    
    if (doThrow) err("No variable with identifier '" + name + "' found");
    return {};
  }

  std::optional<Func> getFunc(std::string name, bool doThrow = true) {
    for (Func func : funcs) {
      if (func.name == name)
        return func;
    }
    
    if (doThrow) err("No function with identifier '" + name + "' found");
    return {};
  }


  Node::Base node;
  std::stringstream output;
  std::vector<Var> vars;
  std::vector<Scope> scopes;
  std::vector<Func> funcs;
  std::string currentFuncName = "";
  int stackPoint = 0;
  int labelIndex = 0;
  int index = 0;
  bool hasMain = false;
};
