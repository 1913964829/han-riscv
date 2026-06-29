#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace han {

[[noreturn]] void fail(const std::string& message) {
    throw std::runtime_error(message);
}

std::string read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        fail("无法打开输入文件: " + path);
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void write_file(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        fail("无法打开输出文件: " + path);
    }
    out << content;
}

enum class TokenKind {
    Eof,
    KwInt,      // 整
    KwReturn,   // 返
    Ident,
    Number,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Semicolon,
    Plus,
    Minus,
    Star,
    Slash,
};

struct Token {
    TokenKind kind{};
    std::string text;
    std::int64_t number = 0;
    std::size_t line = 1;
    std::size_t col = 1;
};

bool is_ascii_ident_start(unsigned char c) {
    return std::isalpha(c) || c == '_';
}

bool is_ascii_ident_continue(unsigned char c) {
    return std::isalnum(c) || c == '_';
}

std::size_t utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            Token tok = next_token();
            tokens.push_back(tok);
            if (tok.kind == TokenKind::Eof) {
                break;
            }
        }
        return tokens;
    }

private:
    Token next_token() {
        skip_space_and_comments();

        Token tok;
        tok.line = line_;
        tok.col = col_;

        if (pos_ >= source_.size()) {
            tok.kind = TokenKind::Eof;
            tok.text = "<eof>";
            return tok;
        }

        unsigned char c = static_cast<unsigned char>(source_[pos_]);

        if (std::isdigit(c)) {
            return lex_number();
        }

        if (is_ident_start(c)) {
            return lex_ident_or_keyword();
        }

        advance_one();
        switch (c) {
            case '(':
                tok.kind = TokenKind::LParen;
                tok.text = "(";
                return tok;
            case ')':
                tok.kind = TokenKind::RParen;
                tok.text = ")";
                return tok;
            case '{':
                tok.kind = TokenKind::LBrace;
                tok.text = "{";
                return tok;
            case '}':
                tok.kind = TokenKind::RBrace;
                tok.text = "}";
                return tok;
            case ';':
                tok.kind = TokenKind::Semicolon;
                tok.text = ";";
                return tok;
            case '+':
                tok.kind = TokenKind::Plus;
                tok.text = "+";
                return tok;
            case '-':
                tok.kind = TokenKind::Minus;
                tok.text = "-";
                return tok;
            case '*':
                tok.kind = TokenKind::Star;
                tok.text = "*";
                return tok;
            case '/':
                tok.kind = TokenKind::Slash;
                tok.text = "/";
                return tok;
            default:
                fail(location(tok) + "无法识别字符: " + std::string(1, static_cast<char>(c)));
        }
    }

    void skip_space_and_comments() {
        while (pos_ < source_.size()) {
            unsigned char c = static_cast<unsigned char>(source_[pos_]);
            if (c == ' ' || c == '\t' || c == '\r') {
                advance_one();
                continue;
            }
            if (c == '\n') {
                advance_newline();
                continue;
            }
            if (c == '/' && peek_byte(1) == '/') {
                while (pos_ < source_.size() && source_[pos_] != '\n') {
                    advance_one();
                }
                continue;
            }
            break;
        }
    }

    Token lex_number() {
        Token tok;
        tok.kind = TokenKind::Number;
        tok.line = line_;
        tok.col = col_;

        std::size_t start = pos_;
        while (pos_ < source_.size() && std::isdigit(static_cast<unsigned char>(source_[pos_]))) {
            advance_one();
        }
        tok.text = source_.substr(start, pos_ - start);
        tok.number = std::stoll(tok.text);
        return tok;
    }

    Token lex_ident_or_keyword() {
        Token tok;
        tok.line = line_;
        tok.col = col_;

        std::size_t start = pos_;
        while (pos_ < source_.size()) {
            unsigned char c = static_cast<unsigned char>(source_[pos_]);
            if (!is_ident_continue(c)) {
                break;
            }
            advance_utf8_char();
        }

        tok.text = source_.substr(start, pos_ - start);
        if (tok.text == "整") {
            tok.kind = TokenKind::KwInt;
        } else if (tok.text == "返") {
            tok.kind = TokenKind::KwReturn;
        } else {
            tok.kind = TokenKind::Ident;
        }
        return tok;
    }

    bool is_ident_start(unsigned char c) const {
        return is_ascii_ident_start(c) || c >= 0x80;
    }

    bool is_ident_continue(unsigned char c) const {
        return is_ascii_ident_continue(c) || c >= 0x80;
    }

    unsigned char peek_byte(std::size_t offset) const {
        if (pos_ + offset >= source_.size()) {
            return 0;
        }
        return static_cast<unsigned char>(source_[pos_ + offset]);
    }

    void advance_utf8_char() {
        unsigned char c = static_cast<unsigned char>(source_[pos_]);
        std::size_t len = utf8_char_len(c);
        for (std::size_t i = 0; i < len && pos_ < source_.size(); ++i) {
            advance_one();
        }
    }

    void advance_one() {
        ++pos_;
        ++col_;
    }

    void advance_newline() {
        ++pos_;
        ++line_;
        col_ = 1;
    }

    static std::string location(const Token& tok) {
        return "第 " + std::to_string(tok.line) + " 行，第 " + std::to_string(tok.col) + " 列：";
    }

    std::string source_;
    std::size_t pos_ = 0;
    std::size_t line_ = 1;
    std::size_t col_ = 1;
};

struct Expr {
    enum class Kind { Number, Binary } kind{};
    std::int64_t number = 0;
    char op = 0;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
};

struct Function {
    std::string source_name;
    std::unique_ptr<Expr> return_expr;
};

struct Program {
    std::vector<Function> functions;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    Program parse_program() {
        Program program;
        while (!check(TokenKind::Eof)) {
            program.functions.push_back(parse_function());
        }
        return program;
    }

private:
    Function parse_function() {
        expect(TokenKind::KwInt, "需要函数返回类型「整」");
        Token name = expect(TokenKind::Ident, "需要函数名");
        expect(TokenKind::LParen, "需要「(」");
        expect(TokenKind::RParen, "第一版暂时只支持无参数函数，需要「)」");
        expect(TokenKind::LBrace, "需要「{」");
        expect(TokenKind::KwReturn, "第一版函数体只支持返回语句，需要「返」");
        auto expr = parse_expr();
        expect(TokenKind::Semicolon, "需要「;」");
        expect(TokenKind::RBrace, "需要「}」");

        Function fn;
        fn.source_name = name.text;
        fn.return_expr = std::move(expr);
        return fn;
    }

    std::unique_ptr<Expr> parse_expr() {
        return parse_additive();
    }

    std::unique_ptr<Expr> parse_additive() {
        auto lhs = parse_multiplicative();
        while (match(TokenKind::Plus) || match(TokenKind::Minus)) {
            Token op = previous();
            auto rhs = parse_multiplicative();
            auto node = std::make_unique<Expr>();
            node->kind = Expr::Kind::Binary;
            node->op = (op.kind == TokenKind::Plus) ? '+' : '-';
            node->lhs = std::move(lhs);
            node->rhs = std::move(rhs);
            lhs = std::move(node);
        }
        return lhs;
    }

    std::unique_ptr<Expr> parse_multiplicative() {
        auto lhs = parse_primary();
        while (match(TokenKind::Star) || match(TokenKind::Slash)) {
            Token op = previous();
            auto rhs = parse_primary();
            auto node = std::make_unique<Expr>();
            node->kind = Expr::Kind::Binary;
            node->op = (op.kind == TokenKind::Star) ? '*' : '/';
            node->lhs = std::move(lhs);
            node->rhs = std::move(rhs);
            lhs = std::move(node);
        }
        return lhs;
    }

    std::unique_ptr<Expr> parse_primary() {
        if (match(TokenKind::Number)) {
            auto node = std::make_unique<Expr>();
            node->kind = Expr::Kind::Number;
            node->number = previous().number;
            return node;
        }

        if (match(TokenKind::LParen)) {
            auto expr = parse_expr();
            expect(TokenKind::RParen, "需要「)」");
            return expr;
        }

        fail(here() + "需要表达式，目前只支持整数、括号和四则运算");
    }

    bool check(TokenKind kind) const {
        return current().kind == kind;
    }

    bool match(TokenKind kind) {
        if (!check(kind)) {
            return false;
        }
        ++pos_;
        return true;
    }

    Token expect(TokenKind kind, const std::string& message) {
        if (!check(kind)) {
            fail(here() + message + "，实际看到「" + current().text + "」");
        }
        return tokens_[pos_++];
    }

    const Token& current() const {
        if (pos_ >= tokens_.size()) {
            return tokens_.back();
        }
        return tokens_[pos_];
    }

    const Token& previous() const {
        return tokens_[pos_ - 1];
    }

    std::string here() const {
        const Token& tok = current();
        return "第 " + std::to_string(tok.line) + " 行，第 " + std::to_string(tok.col) + " 列：";
    }

    std::vector<Token> tokens_;
    std::size_t pos_ = 0;
};

class RiscvBackend {
public:
    std::string emit(const Program& program) {
        out_ << "    .text\n";
        for (const auto& fn : program.functions) {
            emit_function(fn);
            out_ << "\n";
        }
        return out_.str();
    }

private:
    void emit_function(const Function& fn) {
        std::string label = mangle_function_name(fn.source_name);

        out_ << "    .globl " << label << "\n";
        out_ << label << ":\n";
        out_ << "    addi sp, sp, -16\n";
        out_ << "    sd ra, 8(sp)\n";
        out_ << "    sd s0, 0(sp)\n";
        out_ << "    addi s0, sp, 16\n";

        emit_expr(*fn.return_expr);

        out_ << "    ld ra, 8(sp)\n";
        out_ << "    ld s0, 0(sp)\n";
        out_ << "    addi sp, sp, 16\n";
        out_ << "    ret\n";
    }

    void emit_expr(const Expr& expr) {
        switch (expr.kind) {
            case Expr::Kind::Number:
                out_ << "    li a0, " << expr.number << "\n";
                return;
            case Expr::Kind::Binary:
                emit_expr(*expr.lhs);
                out_ << "    addi sp, sp, -8\n";
                out_ << "    sd a0, 0(sp)\n";
                emit_expr(*expr.rhs);
                out_ << "    ld t0, 0(sp)\n";
                out_ << "    addi sp, sp, 8\n";
                switch (expr.op) {
                    case '+':
                        out_ << "    add a0, t0, a0\n";
                        break;
                    case '-':
                        out_ << "    sub a0, t0, a0\n";
                        break;
                    case '*':
                        out_ << "    mul a0, t0, a0\n";
                        break;
                    case '/':
                        out_ << "    div a0, t0, a0\n";
                        break;
                    default:
                        fail("未知二元运算符");
                }
                return;
        }
    }

    std::string mangle_function_name(const std::string& name) {
        if (name == "主" || name == "main") {
            return "main";
        }

        auto it = function_labels_.find(name);
        if (it != function_labels_.end()) {
            return it->second;
        }

        std::string label = "han_fn_" + std::to_string(function_labels_.size());
        function_labels_[name] = label;
        return label;
    }

    std::ostringstream out_;
    std::unordered_map<std::string, std::string> function_labels_;
};

std::string compile_to_riscv_asm(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    Program program = parser.parse_program();

    RiscvBackend backend;
    return backend.emit(program);
}

}  // namespace han

int main(int argc, char** argv) {
    try {
        if (argc != 2 && argc != 4) {
            std::cerr << "用法: hancc 输入.han [-o 输出.s]\n";
            return 2;
        }

        std::string input_path = argv[1];
        std::string output_path;

        if (argc == 4) {
            std::string flag = argv[2];
            if (flag != "-o") {
                std::cerr << "未知参数: " << flag << "\n";
                return 2;
            }
            output_path = argv[3];
        }

        std::string source = han::read_file(input_path);
        std::string asm_text = han::compile_to_riscv_asm(source);

        if (output_path.empty()) {
            std::cout << asm_text;
        } else {
            han::write_file(output_path, asm_text);
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "编译失败: " << e.what() << "\n";
        return 1;
    }
}
