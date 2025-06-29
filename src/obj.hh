#pragma once

#include <cstdlib>
#include <functional>
#include <print>
#include <array>
#include <sstream>
#include <variant>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


struct ObjVertex {
    glm::vec3 m_v;
};

struct ObjNormal {
    glm::vec3 m_vn;
};

struct ObjTexture {
    glm::vec2 m_vt;
};

struct ObjFace {
    std::array<unsigned int, 3> m_vertex_idx;
    std::array<unsigned int, 3> m_uv_idx;
    std::array<unsigned int, 3> m_normal_idx;
};

using Data = std::variant<ObjVertex, ObjNormal, ObjTexture, ObjFace>;

class TokenVertex { };
class TokenNormal { };
class TokenTexture { };
class TokenFace { };
using TokenFloat = float;
using TokenIdent = std::string;
class TokenNewline { };
class TokenInvalid { };

using Token = std::variant<TokenVertex, TokenNormal, TokenTexture, TokenFace, TokenFloat, TokenIdent, TokenNewline, TokenInvalid>;

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

        } else if (std::holds_alternative<TokenNewline>(p)) {
            fmt = "TokenNewline";

        } else if (std::holds_alternative<TokenInvalid>(p)) {
            fmt = "TokenInvalid";
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
        std::println("Token: {}", old);
        return old;
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

public:
    Parser(std::string src)
    : m_lexer(std::move(src))
    {
        m_lexer.next();
    }

    void parse() {

        if (std::holds_alternative<TokenVertex>(m_lexer.peek())) {
            parse_vertex();
        }

        if (std::holds_alternative<TokenNormal>(m_lexer.peek())) {
            parse_normal();
        }

        if (std::holds_alternative<TokenTexture>(m_lexer.peek())) {
            parse_texture();
        }

    }

private:
    void parse_texture() {
        auto tok = m_lexer.next();
        assert(std::holds_alternative<TokenTexture>(tok));

        auto u = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(u));

        auto v = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(v));

        auto nl = m_lexer.next();
        assert(std::holds_alternative<TokenNewline>(nl));

        m_uvs.push_back(
            {
                std::get<TokenFloat>(u),
                std::get<TokenFloat>(v)
            }
        );
    }

    void parse_normal() {
        auto tok = m_lexer.next();
        assert(std::holds_alternative<TokenNormal>(tok));

        auto x = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(x));

        auto y = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(y));

        auto z = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(z));

        auto nl = m_lexer.next();
        assert(std::holds_alternative<TokenNewline>(nl));

        m_normals.push_back(
            {
                std::get<TokenFloat>(x),
                std::get<TokenFloat>(y),
                std::get<TokenFloat>(z)
            }
        );
    }

    void parse_vertex() {
        auto tok = m_lexer.next();
        assert(std::holds_alternative<TokenVertex>(tok));

        auto x = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(x));

        auto y = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(y));

        auto z = m_lexer.next();
        assert(std::holds_alternative<TokenFloat>(z));

        auto nl = m_lexer.next();
        assert(std::holds_alternative<TokenNewline>(nl));

        m_vertices.push_back(
            {
                std::get<TokenFloat>(x),
                std::get<TokenFloat>(y),
                std::get<TokenFloat>(z)
            }
        );

    }

};
