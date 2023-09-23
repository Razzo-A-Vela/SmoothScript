#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>

#include "util.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"


class Generator {
public:
  Generator(Node::Base _node) : node(_node) {}

  Node::Stmt* genInsideScope(std::string breakLabel, std::string continueLabel) {
    index++;
    Node::Stmt* stmt;
    int scopeDepth = -1;
    for (; index < node.stmts.size(); index++) {
      stmt = node.stmts.at(index);

      try {
        Node::StmtScope* scopeStmt = std::get<Node::StmtScope*>(stmt->var);
        int depth = scopes.size();
        
        if (scopeStmt->close && depth == scopeDepth) break;
        else if (scopeDepth == -1) scopeDepth = depth + 1;
      } catch (std::bad_variant_access const& e) {}
      
      genStmt(stmt, breakLabel, continueLabel);
      output << "\n\n";
    }
    output.seekp(-2, output.cur);

    return stmt;
  }

  void genBinExpr(const Node::ExprBin* binExprNode, std::string reg) {
    genExpr(binExprNode->left);
    push("rax");
    output << "\n";
    genExpr(binExprNode->right, "rbx");
    pop("rax");

    if (binExprNode->type == ExprBinType::add)
      output << "  add rax, rbx\n\n";

    else if (binExprNode->type == ExprBinType::sub)
      output << "  sub rax, rbx\n\n";

    else if (binExprNode->type == ExprBinType::mult)
      output << "  mul rbx\n\n";

    else if (binExprNode->type == ExprBinType::div) {
      output << "  mov rdx, 0\n";
      output << "  div rbx\n\n";

    } else if (binExprNode->type == ExprBinType::mod) {
      output << "  mov rdx, 0\n";
      output << "  div rbx\n";
      output << "  mov rax, rdx\n\n";

    } else if (isBoolBinExpr(binExprNode->type)) {
      std::string binExprTrueLabel = getLabel("bool_bin_expr_true");
      std::string binExprFalseLabel = getLabel("bool_bin_expr_false");
      
      if (binExprNode->type == ExprBinType::eq) {
        output << "  cmp rax, rbx\n";
        output << "  je " << binExprTrueLabel << "\n";

      } else if (binExprNode->type == ExprBinType::not_eq_) {
        output << "  cmp rax, rbx\n";
        output << "  jne " << binExprTrueLabel << "\n";
      }
      
      output << "  mov rax, 0\n";
      output << "  jmp " << binExprFalseLabel << "\n";
      output << binExprTrueLabel << ":\n";
      output << "  mov rax, 1\n";
      output << binExprFalseLabel << ":\n\n";

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
        gen->output << "[rsp + " << (gen->stackPoint - gen->getVar(varNode->name).value().stackOffset) * 8 << "]\n";
      }

      void operator()(const Node::ExprBin* binExprNode) {
        gen->genBinExpr(binExprNode, reg);
      }
    };

    Visitor visitor(this, reg_);
    std::visit(visitor, expr->var);
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
          gen->output << "  mov [rsp + " << (gen->stackPoint - var.stackOffset) * 8 << "], rax\n";
          
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

          while (gen->stackPoint > scope.stackOffset) {
            gen->pop("rbx");
            gen->vars.pop_back();
          }
          return;
        }

        gen->scopes.push_back({ .stackOffset = gen->stackPoint });
      }

      void operator()(const Node::StmtIf* ifNode) {
        std::string endIfLabel = gen->getLabel("end_if");
        gen->genExpr(ifNode->expr);

        gen->output << "  mov rbx, 0\n";
        gen->output << "  cmp rax, rbx\n";
        gen->output << "  je " << endIfLabel << "\n";

        Node::Stmt* scopeStmt = gen->genInsideScope(breakLabel, continueLabel);
        gen->output << endIfLabel << ":\n";

        gen->genStmt(scopeStmt);
      }

      void operator()(const Node::StmtWhile* whileNode) {        
        std::string whileLoopLabel = gen->getLabel("while_loop");
        std::string endWhileLabel = gen->getLabel("end_while");
        gen->output << whileLoopLabel << ":\n";
        gen->genExpr(whileNode->expr);

        gen->output << "  mov rbx, 0\n";
        gen->output << "  cmp rax, rbx\n";
        gen->output << "  je " << endWhileLabel << "\n";
        
        Node::Stmt* scopeStmt = gen->genInsideScope(endWhileLabel, whileLoopLabel);
        gen->output << "  jmp " << whileLoopLabel << "\n";
        gen->output << endWhileLabel << ":\n";

        gen->genStmt(scopeStmt);
      }

      void operator()(const Node::StmtBreak* breakNode) {
        if (breakLabel == "") err("Cannot use 'break' while not in a loop");
        gen->output << "  jmp " << breakLabel << "\n";
      }

      void operator()(const Node::StmtContinue* continueNode) {
        if (continueLabel == "") err("Cannot use 'continue' while not in a loop");
        gen->output << "  jmp " << continueLabel << "\n";
      }
    };

    Visitor visitor(this, breakLabel_, continueLabel_);
    std::visit(visitor, stmt->var);
  }

  std::string generate() {
    output << "global main\nsection .text\n\nmain:\n";

    for (; index < node.stmts.size(); index++) {
      genStmt(node.stmts.at(index));
      output << "\n\n";
    }
    if (scopes.size() != 0) err("Expected '}'");
    exit();

    output << "  mov rax, 0\n  ret\n";
    return output.str();
  }

private:
  void push(std::string reg, bool update = true) {
    if (update) stackPoint++;
    output << "  push " << reg << "\n";
  }

  void pop(std::string reg, bool update = true) {
    if (update) stackPoint--;
    output << "  pop " << reg << "\n";
  }

  void exit() {
    for (int i = stackPoint; i > 0; i--) pop("rbx", false);
  }

  std::string getLabel(std::string name) {
    std::stringstream out;
    out << name << "__" << labelIndex++;
    return out.str();
  }

  struct Var {
    std::string name;
    int stackOffset;
  };

  struct Scope {
    int stackOffset;
  };

  std::optional<Var> getVar(std::string name, bool doThrow = true) {
    for (Var var : vars) {
      if (var.name == name)
        return var;
    }
    if (doThrow) err("No variable with identifier '" + name + "' found");
    return {};
  }


  Node::Base node;
  std::stringstream output;
  std::vector<Var> vars;
  std::vector<Scope> scopes;
  int stackPoint = 0;
  int labelIndex = 0;
  int index = 0;
};
