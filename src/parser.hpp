#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <optional>

#include "util.hpp"
#include "tokenizer.hpp"
#include "nodes.hpp"


class Parser {
public:
  Parser(const std::vector<Token> _tokens) : tokens(_tokens), allocator(1024 * 1024 * 5) /* 5MB */ {}


  Node::Expr* parseExpr(bool parenExpr = false, bool parseBin = true) {
    Node::Expr* ret;
    if (!peek().has_value()) err("Syntax error", peek(-1).value().line);

    if (try_consume(TokenType::open_paren).has_value())
      ret = parseExpr(true);

    else {
      ret = allocator.alloc<Node::Expr>();
  
      if (peek().value().type == TokenType::int_lit) {
        Node::ExprIntLit* intLitNode = allocator.alloc<Node::ExprIntLit>();
        intLitNode->value = consume().value.value();
        ret->var = intLitNode;


      } else if (peek().value().type == TokenType::ident) {
        parseExprIdent(consume(), ret);


      } else if (peek().value().type == TokenType::char_lit) {
        Node::ExprCharLit* charLitNode = allocator.alloc<Node::ExprCharLit>();
        std::stringstream value;
        value << ((int) consume().value.value().at(0));
        charLitNode->value = value.str();
        ret->var = charLitNode;

        
      } else if (peek().value().type == TokenType::get) {
        consume();
        try_consume(TokenType::open_paren, "Expected '('");
        try_consume(TokenType::closed_paren, "Expected ')'");
        
        Node::ExprGet* getNode = allocator.alloc<Node::ExprGet>();
        ret->var = getNode;


      } else
        err("Syntax error", peek(-1).value().line);
    }
    

    if (parseBin && peek().has_value() && isBinOp(peek().value().type)) {
      int prev_prec = getBinExprPrec(getBinType(peek().value().type).value());

      while (peek().has_value() && isBinOp(peek().value().type)) {
        ExprBinType opType = getBinType(consume().type).value();
        Node::Expr* right = parseExpr(false, false);
        Node::ExprBin* binExpr = allocator.alloc<Node::ExprBin>();
        int prec = getBinExprPrec(opType);
        binExpr->type = opType;

        if (prec > prev_prec) {
          if (!std::holds_alternative<Node::ExprBin*>(ret->var)) err("How?");
          Node::ExprBin* binExprNode = std::get<Node::ExprBin*>(ret->var);

          binExpr->left = binExprNode->right;
          Node::Expr* right_expr = allocator.alloc<Node::Expr>();
          right_expr->var = binExpr;
          binExprNode->right = right_expr;
          binExpr->right = right;

        } else {
          binExpr->left = ret;
          binExpr->right = right;

          Node::Expr* expr = allocator.alloc<Node::Expr>();
          expr->var = binExpr;
          ret = expr;
        }

        prev_prec = prec;
      }
    }

    if (parenExpr) try_consume(TokenType::closed_paren, "Expected ')'");
    return ret;
  }


  Node::Stmt* parseStmt() {
    Node::Stmt* ret = allocator.alloc<Node::Stmt>();
    if (!peek().has_value()) err("Syntax error", peek(-1).value().line);


    if (try_consume(TokenType::exit).has_value()) {
      try_consume(TokenType::open_paren, "Expected '('");

      Node::StmtExit* exitNode = allocator.alloc<Node::StmtExit>();
      exitNode->expr = parseExpr(true);
      ret->var = exitNode;

      try_consume(TokenType::semi, "Expected ';'");


    } else if (try_consume(TokenType::var).has_value()) {
      if (!peek().has_value()) err("Syntax error", peek(-1).value().line);
      Token ident = consume();
      if (ident.type != TokenType::ident) err("Syntax error", ident.line);

      Node::StmtVar* varNode = allocator.alloc<Node::StmtVar>();
      varNode->name = ident.value.value();

      if (peek().value().type == TokenType::semi)
        varNode->expr = {};
      else {
        try_consume(TokenType::eq, "Expected '='");
        Node::Expr* expr = parseExpr();
        varNode->expr = expr;
      }
      try_consume(TokenType::semi, "Expected ';'");
      ret->var = varNode;


    } else if (try_consume(TokenType::if_).has_value()) {
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtIf* ifStmt = allocator.alloc<Node::StmtIf>();
      Node::Expr* expr = parseExpr(true);
      ifStmt->expr = expr;
      ifStmt->scope = parseScope(ScopeType::generic);
      ret->var = ifStmt;


    } else if (try_consume(TokenType::while_).has_value()) {
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtWhile* stmtWhile = allocator.alloc<Node::StmtWhile>();
      Node::Expr* expr = parseExpr(true);
      stmtWhile->expr = expr;
      stmtWhile->scope = parseScope(ScopeType::loop);
      ret->var = stmtWhile;


    } else if (try_consume(TokenType::break_).has_value()) {
      try_consume(TokenType::semi, "Expected ';'");
      Node::StmtBreak* stmtBreak = allocator.alloc<Node::StmtBreak>();
      ret->var = stmtBreak;


    } else if (try_consume(TokenType::continue_).has_value()) {
      try_consume(TokenType::semi, "Expected ';'");
      Node::StmtContinue* stmtContinue = allocator.alloc<Node::StmtContinue>();
      ret->var = stmtContinue;


    } else if (try_consume(TokenType::for_).has_value()) {
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtFor* stmtFor = allocator.alloc<Node::StmtFor>();

      stmtFor->once = parseStmt();
      stmtFor->condition = parseExpr();
      try_consume(TokenType::semi, "Expected ';'");
      stmtFor->repeat = parseStmt();
      try_consume(TokenType::closed_paren, "Expected ')'");
      stmtFor->scope = parseScope(ScopeType::loop);
      ret->var = stmtFor;


    } else if (try_consume(TokenType::put).has_value()) {
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtPut* putStmt = allocator.alloc<Node::StmtPut>();
      putStmt->expr = parseExpr(true);
      
      ret->var = putStmt;
      try_consume(TokenType::semi, "Expected ';'");


    } else if (try_consume(TokenType::dollar).has_value()) {
      try_consume(TokenType::main, "Expected 'main' after '$'");
      Node::StmtMain* mainNode = allocator.alloc<Node::StmtMain>();
      
      if (try_consume(TokenType::question).has_value()) mainNode->enforceMain = false;
      else mainNode->enforceMain = true;

      ret->var = mainNode;


    } else if (try_consume(TokenType::return_).has_value()) {
      Node::StmtReturn* returnNode = allocator.alloc<Node::StmtReturn>();
      if (!try_consume(TokenType::semi).has_value()) {
        returnNode->expr = parseExpr();
        try_consume(TokenType::semi, "Expected ';'");
      }
      ret->var = returnNode;


    } else if (try_consume(TokenType::extend).has_value()) {
      Node::StmtExtend* extendNode = allocator.alloc<Node::StmtExtend>();
      Token file = consume();
      try_consume(TokenType::dot, "Expected .smt file");
      Token ext = consume();

      std::string alias = "";
      if (try_consume(TokenType::as).has_value()) {
        Token al = consume();
        if (al.type != TokenType::ident) err("Expected identifier after 'as'");
        alias = al.value.value();
      }
      try_consume(TokenType::semi, "Expected ';'");

      if (file.type != TokenType::ident || ext.type != TokenType::ident) err("Expected .smt file", file.line);
      if (ext.value.value() != "smt") err("Expected .smt file", ext.line);
      extendNode->fileName = file.value.value() + "." + ext.value.value();
      extendNode->alias = alias;
      ret->var = extendNode;


    } else if (peek().value().type == TokenType::ident) {
      Token ident = consume();

      if (try_consume(TokenType::colon).has_value()) {
        try_consume(TokenType::open_paren, "Expected '('");
        Node::StmtDefFunc* defFuncNode = allocator.alloc<Node::StmtDefFunc>();
        defFuncNode->name = ident.value.value();

        while (!try_consume(TokenType::closed_paren).has_value()) {
          Token t = consume();
          if (t.type != TokenType::ident) err("Expected parameter name", t.line);
          defFuncNode->params.push_back(t.value.value());

          if (peek().has_value() && peek().value().type != TokenType::closed_paren) try_consume(TokenType::comma, "Expected ','");
        }

        defFuncNode->scope = parseScope(ScopeType::func, "Expected '{' after function declaration of '" + defFuncNode->name + "'");
        ret->var = defFuncNode;

      } else parseStmtIdent(ident, ret);


    } else if (peek().value().type == TokenType::open_curly) {
      ret->var = parseScope(ScopeType::generic);


    } else err("Syntax error", peek(-1).value().line);

    return ret;
  }


  Node::Base parse() {
    Node::Base ret;

    while (peek().has_value()) {
      ret.stmts.push_back(parseStmt());
    }

    return ret;
  }

private:
  std::optional<Token> peek(int offset = 0) {
    if (index + offset >= tokens.size()) {
      return {};
    }
    return tokens.at(index + offset);
  }

  Token consume() {
    return tokens.at(index++);
  }

  std::optional<Token> try_consume(TokenType type) {
    if (peek().has_value() && peek().value().type == type) {
      return consume();
    }
    return {};
  }

  std::optional<Token> try_consume(TokenType type, std::string msg) {
    std::optional<Token> optional = try_consume(type);
    if (!optional.has_value()) err(msg, peek(-1).value().line);
    return optional;
  }


  void parseStmtIdent(Token ident, Node::Stmt* ret) {
    if (ident.type != TokenType::ident) err("Expected identifier", ident.line);

    if (try_consume(TokenType::open_paren).has_value()) {
      Node::Func* funcNode = allocator.alloc<Node::Func>();
      funcNode->name = ident.value.value();
      
      while (!try_consume(TokenType::closed_paren).has_value()) {
        funcNode->exprs.push_back(parseExpr());

        if (peek().has_value() && peek().value().type != TokenType::closed_paren) try_consume(TokenType::comma, "Expected ','");
      }
      ret->var = funcNode;
      try_consume(TokenType::semi, "Expected ';'");


    } else if (try_consume(TokenType::dot).has_value()) {
      Node::Stmt* stmt = allocator.alloc<Node::Stmt>();
      parseStmtIdent(consume(), stmt);

      if (std::holds_alternative<Node::Func*>(stmt->var)) {
        Node::Func* funcNode = std::get<Node::Func*>(stmt->var);
        funcNode->name = ident.value.value() + "#" + funcNode->name;
        ret->var = funcNode;

      } else if (std::holds_alternative<Node::StmtVar*>(stmt->var)) {
        Node::StmtVar* varNode = std::get<Node::StmtVar*>(stmt->var);
        varNode->name = ident.value.value() + "#" + varNode->name;
        ret->var = varNode;

      } else err("Syntax error", ident.line);

    } else {      
      Node::StmtVar* varNode = allocator.alloc<Node::StmtVar>();
      varNode->name = ident.value.value();
      varNode->reAssign = true;

      TokenType op = peek().value().type;
      if (op == TokenType::plus || op == TokenType::minus) {
        try_consume(op, "Syntax error");
        try_consume(op, "Syntax error");

        Node::Expr* expr = allocator.alloc<Node::Expr>();
        Node::ExprBin* binExpr = allocator.alloc<Node::ExprBin>();
        Node::ExprVar* varExpr = allocator.alloc<Node::ExprVar>();
        Node::ExprIntLit* intExpr = allocator.alloc<Node::ExprIntLit>();

        varExpr->name = varNode->name;
        Node::Expr* var_expr = allocator.alloc<Node::Expr>();
        var_expr->var = varExpr;
        
        intExpr->value = "1";
        Node::Expr* int_expr = allocator.alloc<Node::Expr>();
        int_expr->var = intExpr;

        binExpr->left = var_expr;
        binExpr->right = int_expr;
        binExpr->type = getBinType(op).value();
        expr->var = binExpr;
        varNode->expr = expr;

      } else {
        try_consume(TokenType::eq, "Expected '='");
        Node::Expr* expr = parseExpr();
        varNode->expr = expr;
      }

      ret->var = varNode;
      try_consume(TokenType::semi, "Expected ';'");
    }
  }

  void parseExprIdent(Token ident, Node::Expr* ret) {
    if (ident.type != TokenType::ident) err("Expected identifier", ident.line);

    if (try_consume(TokenType::open_paren).has_value()) {
      Node::Func* funcNode = allocator.alloc<Node::Func>();
      funcNode->name = ident.value.value();
      
      while (!try_consume(TokenType::closed_paren).has_value()) {
        funcNode->exprs.push_back(parseExpr());

        if (peek().has_value() && peek().value().type != TokenType::closed_paren) try_consume(TokenType::comma, "Expected ','");
      }
      ret->var = funcNode;


    } else if (try_consume(TokenType::dot).has_value()) {
      Node::Expr* expr = allocator.alloc<Node::Expr>();
      parseExprIdent(consume(), expr);

      if (std::holds_alternative<Node::Func*>(expr->var)) {
        Node::Func* funcNode = std::get<Node::Func*>(expr->var);
        funcNode->name = ident.value.value() + "#" + funcNode->name;
        ret->var = funcNode;

      } else if (std::holds_alternative<Node::ExprVar*>(expr->var)) {
        Node::ExprVar* varNode = std::get<Node::ExprVar*>(expr->var);
        varNode->name = ident.value.value() + "#" + varNode->name;
        ret->var = varNode;

      } else err("Syntax error", ident.line);

    } else {
      Node::ExprVar* varNode = allocator.alloc<Node::ExprVar>();
      varNode->name = ident.value.value();
      ret->var = varNode;
    }
  }

  
  Node::StmtScope* parseScope(ScopeType type, std::string needsCurlyErr = "") {
    Node::StmtScope* ret = allocator.alloc<Node::StmtScope>();
    ret->type = type;
    bool foundCurly = true;
    if (peek().value().type == TokenType::open_curly) consume();
    else foundCurly = false;
    if (!foundCurly && needsCurlyErr != "") err(needsCurlyErr, peek(-1).value().line);

    do {
      ret->stmts.push_back(parseStmt());
    } while (foundCurly && !try_consume(TokenType::closed_curly).has_value());

    return ret;
  }


  const std::vector<Token> tokens;
  Allocator allocator;
  std::size_t index = 0;
};
