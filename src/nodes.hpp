#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>


enum class ExprBinType {
  add, sub, mult, div, mod, eq, not_eq_, less_eq, greater_eq, greater, less
};

namespace Node {
  struct Expr;

  struct Func {
    std::string name;
    std::vector<Expr*> exprs;
  };


  struct ExprGet {};

  struct ExprCharLit {
    std::string value;
  };

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
    std::variant<ExprIntLit*, ExprVar*, ExprBin*, ExprCharLit*, ExprGet*, Func*> var;
  };


  struct Stmt;

  struct StmtExtend {
    std::string fileName;
    std::string alias = "";
  };

  struct StmtReturn {
    std::optional<Expr*> expr;
  };

  struct StmtDefFunc {
    std::string name;
    std::vector<std::string> params;
  };

  struct StmtMain {};

  struct StmtPut {
    Expr* expr;
  };

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
    std::variant<StmtExit*, StmtVar*, StmtScope*, StmtIf*, StmtWhile*, StmtBreak*, StmtContinue*, StmtFor*, StmtPut*, StmtMain*, StmtDefFunc*, StmtReturn*, Func*, StmtExtend*> var;
  };


  struct Base {
    std::vector<Stmt*> stmts;
  };
}
