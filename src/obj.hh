#pragma once

#include <cstdlib>
#include <functional>
#include <print>
#include <array>
#include <sstream>

#include <glm/glm.hpp>
#include <variant>
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
        return formatter<string>::format(
            std::format("Token: {}", p.index()), ctx);
    }
};

class Lexer {
    std::istringstream m_src;

public:
    Lexer(std::string src)
    : m_src(std::move(src))
    { }

    Token next() {
        char c = m_src.peek();

        switch (c) {
            case ' ':
            case '\t':
                m_src.ignore();
                return next();
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

private:
    std::optional<float> tokenize_float() {

        float negative = false;
        if (m_src.peek() == '-') {
            negative = true;
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

        if (negative)
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
