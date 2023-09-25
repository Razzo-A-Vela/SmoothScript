#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <variant>


enum class ExprBinType {
  add, sub, mult, div, mod, eq, not_eq_
};

namespace Node {
  struct Expr;

  struct ExprBin {
    Expr* left;
    Expr* right;
    ExprBinType type;
  };

  struct ExprVar {
    std::string name;
  };

  struct ExprIntLit {
    std::string value;
  };

  struct Expr {
    std::variant<ExprIntLit*, ExprVar*, ExprBin*> var;
  };


  struct Stmt;

  struct StmtFor {
    Stmt* once;
    Expr* condition;
    Stmt* repeat;
  };

  struct StmtContinue {};

  struct StmtBreak {};

  struct StmtWhile {
    Expr* expr;
  };

  struct StmtIf {
    Expr* expr;
  };

  struct StmtScope {
    bool close = false;
  };

  struct StmtVar {
    std::string name;
    std::optional<Expr*> expr;
    bool reAssign = false;
  };

  struct StmtExit {
    Expr* expr;
  };

  struct Stmt {
    std::variant<StmtExit*, StmtVar*, StmtScope*, StmtIf*, StmtWhile*, StmtBreak*, StmtContinue*, StmtFor*> var;
  };


  struct Base {
    std::vector<Stmt*> stmts;
  };
}
