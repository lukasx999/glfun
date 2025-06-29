#pragma once

#include "vertex.hh"
#include <cstdlib>
#include <functional>
#include <print>
#include <array>
#include <sstream>
#include <variant>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class TokenVertex { };
class TokenNormal { };
class TokenTexture { };
class TokenFace { };
using TokenFloat = float;
using TokenIdent = std::string;
class TokenSlash { };
class TokenNewline { };
class TokenInvalid { };

using Token = std::variant<TokenVertex, TokenNormal, TokenTexture, TokenFace, TokenFloat, TokenIdent, TokenSlash, TokenNewline, TokenInvalid>;

template <>
struct std::formatter<Token> : std::formatter<std::string> {
    auto format(Token p, format_context &ctx) const {

        std::string fmt;

        if (std::holds_alternative<TokenFloat>(p)) {
            fmt = std::format("Float({})", std::get<TokenFloat>(p));

        } else if (std::holds_alternative<TokenIdent>(p)) {
            fmt = std::format("Ident({})", std::get<TokenIdent>(p));

        } else if (std::holds_alternative<TokenVertex>(p)) {
            fmt = "Vertex";

        } else if (std::holds_alternative<TokenNormal>(p)) {
            fmt = "Normal";

        } else if (std::holds_alternative<TokenTexture>(p)) {
            fmt = "TokenTexture";

        } else if (std::holds_alternative<TokenFace>(p)) {
            fmt = "TokenFace";

        } else if (std::holds_alternative<TokenSlash>(p)) {
            fmt = "TokenSlash";

        } else if (std::holds_alternative<TokenNewline>(p)) {
            fmt = "TokenNewline";

        } else if (std::holds_alternative<TokenInvalid>(p)) {
            fmt = "TokenInvalid";

        } else {
            throw std::runtime_error("token has no string representation");
        }

        return formatter<string>::format(fmt, ctx);
    }
};

class Lexer {
    std::istringstream m_src;
    Token m_tok = TokenInvalid{};

public:
    Lexer(std::string src)
    : m_src(std::move(src))
    { }

    [[nodiscard]] Token peek() const {
        return m_tok;
    }

    Token next() {
        Token old = m_tok;
        m_tok = impl_next();
        return old;
    }

    void skip_to_newline() {
        read_while([](char c) { return c != '\n'; });
        next();
    }

    [[nodiscard]] bool is_at_end() const {
        return m_src.eof();
    }

private:
    Token impl_next() {
        char c = m_src.peek();

        switch (c) {
            case ' ':
            case '\t':
                m_src.ignore();
                return impl_next();
                break;

            case '\n':
                m_src.ignore();
                return TokenNewline{};
                break;

            case '/':
                m_src.ignore();
                return TokenSlash{};
                break;

            case '#':
                skip_to_newline();
                return peek();
                break;

            default: {
                auto ident = read_while(isalpha);

                if (ident.has_value()) {
                    auto str = ident.value();

                    if (str == "v")
                        return TokenVertex();

                    else if (str == "vn")
                        return TokenNormal();

                    else if (str == "vt")
                        return TokenTexture();

                    else if (str == "f")
                        return TokenFace();

                    else
                        return str;
                }

                auto num = tokenize_float();

                if (num.has_value()) {
                    return num.value();
                }

                return TokenInvalid{};

            } break;
        }
    }

    std::optional<float> tokenize_float() {

        float is_negative = false;
        if (m_src.peek() == '-') {
            is_negative = true;
            m_src.ignore();
        }

        auto num = read_while(isdigit);

        if (!num.has_value())
            return { };

        std::optional<std::string> num2;

        if (m_src.peek() == '.') {
            m_src.ignore();
            num2 = read_while(isdigit);

            if (!num2.has_value())
                return { };
        }

        auto str = num.value();

        if (num2.has_value())
            str += '.' + num2.value();

        if (is_negative)
            str.insert(str.begin(), '-');

        return atof(str.c_str());

    }

    std::optional<std::string> read_while(std::function<bool(char)> predicate) {

        char c;
        std::string acc;

        while (predicate(c = m_src.get())) {
            acc.push_back(c);
        }

        m_src.unget();

        if (acc.empty()) return { };

        return acc;

    }

};

class Parser {
    Lexer m_lexer;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_vertex_indices;
    std::vector<unsigned int> m_texture_indices;
    std::vector<unsigned int> m_normal_indices;

public:
    Parser(std::string src) : m_lexer(std::move(src))
    {
        m_lexer.next();
    }

    std::vector<Vertex> parse() {

        while (!std::holds_alternative<TokenInvalid>(m_lexer.peek())) {
            parse_line();
        }

        std::vector<Vertex> verts;

        for (auto &v : m_vertex_indices) {
            verts.push_back(m_vertices[v-1]);
        }

        return verts;

    }

private:
    void parse_line() {

        auto tok = m_lexer.peek();

        if (std::holds_alternative<TokenVertex>(tok)) {
            parse_vertex();

        } else if (std::holds_alternative<TokenNormal>(tok)) {
            parse_normal();

        } else if (std::holds_alternative<TokenTexture>(tok)) {
            parse_texture();

        } else if (std::holds_alternative<TokenFace>(tok)) {
            parse_face();

        } else if (std::holds_alternative<TokenIdent>(tok)) {
            std::println(stderr, "> Obj Parser Warning:");
            std::println(stderr, "Unsupported Instruction: `{}`", std::get<TokenIdent>(tok));
            m_lexer.skip_to_newline();
        }

        auto nl = m_lexer.next();
        expect<TokenNewline>(nl);

    }

    void parse_face() {
        expect<TokenFace>(m_lexer.next());
        parse_face_index_triple();
        parse_face_index_triple();
        parse_face_index_triple();
    }

    void parse_face_index_triple() {
        auto vert_idx = m_lexer.next();
        expect<TokenFloat>(vert_idx);
        m_vertex_indices.push_back(std::get<TokenFloat>(vert_idx));

        if (!std::holds_alternative<TokenSlash>(m_lexer.peek())) return;
        m_lexer.next();

        auto tex_idx = m_lexer.next();
        expect<TokenFloat>(tex_idx);
        m_texture_indices.push_back(std::get<TokenFloat>(tex_idx));

        if (!std::holds_alternative<TokenSlash>(m_lexer.peek())) return;
        m_lexer.next();

        auto norm_idx = m_lexer.next();
        expect<TokenFloat>(norm_idx);
        m_normal_indices.push_back(std::get<TokenFloat>(norm_idx));
    }

    void parse_texture() {
        expect<TokenTexture>(m_lexer.next());

        auto u = m_lexer.next();
        expect<TokenFloat>(u);

        auto v = m_lexer.next();
        expect<TokenFloat>(v);

        m_uvs.push_back({
            std::get<TokenFloat>(u),
            std::get<TokenFloat>(v),
        });
    }

    void parse_normal() {
        expect<TokenNormal>(m_lexer.next());

        auto x = m_lexer.next();
        expect<TokenFloat>(x);

        auto y = m_lexer.next();
        expect<TokenFloat>(y);

        auto z = m_lexer.next();
        expect<TokenFloat>(z);

        m_normals.push_back({
            std::get<TokenFloat>(x),
            std::get<TokenFloat>(y),
            std::get<TokenFloat>(z)
        });
    }

    void parse_vertex() {
        expect<TokenVertex>(m_lexer.next());

        auto x = m_lexer.next();
        expect<TokenFloat>(x);

        auto y = m_lexer.next();
        expect<TokenFloat>(y);

        auto z = m_lexer.next();
        expect<TokenFloat>(z);

        m_vertices.push_back({
            std::get<TokenFloat>(x),
            std::get<TokenFloat>(y),
            std::get<TokenFloat>(z)
        });
    }

    template <class TokenType>
    static inline void expect(const Token &tok) {
        assert(std::holds_alternative<TokenType>(tok));
    }

};
