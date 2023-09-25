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
    if (!peek().has_value()) err("Syntax error", peek().value().line);

    if (peek().value().type == TokenType::open_paren) {
      consume();
      ret = parseExpr(true);

    } else {
      ret = allocator.alloc<Node::Expr>();
  
      if (peek().value().type == TokenType::int_lit) {
        Node::ExprIntLit* intLitNode = allocator.alloc<Node::ExprIntLit>();
        intLitNode->value = consume().value.value();
        ret->var = intLitNode;

      } else if (peek().value().type == TokenType::ident) {
        Node::ExprVar* varNode = allocator.alloc<Node::ExprVar>();
        varNode->name = consume().value.value();
        ret->var = varNode;

      } else if (peek().value().type == TokenType::char_lit) {
        Node::ExprCharLit* charLitNode = allocator.alloc<Node::ExprCharLit>();
        std::stringstream value;
        value << ((int) consume().value.value().at(0));
        charLitNode->value = value.str();
        ret->var = charLitNode;
        
      } else
        err("Syntax error", peek().value().line);
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
    if (!peek().has_value()) err("Syntax error", peek().value().line);


    if (peek().value().type == TokenType::exit) {
      consume();
      try_consume(TokenType::open_paren, "Expected '('");

      Node::StmtExit* exitNode = allocator.alloc<Node::StmtExit>();
      exitNode->expr = parseExpr(true);
      ret->var = exitNode;

      try_consume(TokenType::semi, "Expected ';'");

    } else if (peek().value().type == TokenType::var) {
      consume();
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

    } else if (peek().value().type == TokenType::ident) {
      Token ident = consume();
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

    } else if (peek().value().type == TokenType::if_) {
      consume();
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtIf* ifStmt = allocator.alloc<Node::StmtIf>();
      Node::Expr* expr = parseExpr(true);
      ifStmt->expr = expr;
      ret->var = ifStmt;

    } else if (peek().value().type == TokenType::while_) {
      consume();
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtWhile* stmtWhile = allocator.alloc<Node::StmtWhile>();
      Node::Expr* expr = parseExpr(true);
      stmtWhile->expr = expr;
      ret->var = stmtWhile;

    } else if (peek().value().type == TokenType::break_) {
      consume();
      try_consume(TokenType::semi, "Expected ';'");
      Node::StmtBreak* stmtBreak = allocator.alloc<Node::StmtBreak>();
      ret->var = stmtBreak;

    } else if (peek().value().type == TokenType::continue_) {
      consume();
      try_consume(TokenType::semi, "Expected ';'");
      Node::StmtContinue* stmtContinue = allocator.alloc<Node::StmtContinue>();
      ret->var = stmtContinue;

    } else if (peek().value().type == TokenType::for_) {
      consume();
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtFor* stmtFor = allocator.alloc<Node::StmtFor>();

      stmtFor->once = parseStmt();
      stmtFor->condition = parseExpr();
      try_consume(TokenType::semi, "Expected ';'");
      stmtFor->repeat = parseStmt();
      try_consume(TokenType::closed_paren, "Expected ')'");
      ret->var = stmtFor;

    } else if (peek().value().type == TokenType::put) {
      consume();
      try_consume(TokenType::open_paren, "Expected '('");
      Node::StmtPut* putStmt = allocator.alloc<Node::StmtPut>();
      putStmt->expr = parseExpr(true);
      
      ret->var = putStmt;
      try_consume(TokenType::semi, "Expected ';'");

    } else if (peek().value().type == TokenType::open_curly || peek().value().type == TokenType::closed_curly) {
      Node::StmtScope* scopeStmt = allocator.alloc<Node::StmtScope>();
      scopeStmt->close = consume().type == TokenType::closed_curly;
      ret->var = scopeStmt;

    } else
      err("Syntax error", peek().value().line);

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
    if (!optional.has_value()) err(msg, peek().value().line);
    return optional;
  }

  const std::vector<Token> tokens;
  Allocator allocator;
  std::size_t index = 0;
};
