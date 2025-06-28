#pragma once

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
using TokenFloat = float;
using TokenIdent = std::string;
class TokenNewline { };

using Token = std::variant<TokenVertex, TokenFloat, TokenIdent, TokenNewline>;

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

    [[nodiscard]] Token next() {
        char c = m_src.peek();
        // std::print("{}", c);

        switch (c) {
            case ' ':
            case '\t':
                // m_src.get();
                return next();
                break;
            default: {

                auto ident = read_while(m_src, isalpha);

                // auto num = read_while(m_src, isdigit);

                if (ident.has_value()) {
                    std::println("{}", ident.value());
                    return ident.value();
                }

                // if (num.has_value()) {
                //     std::println("{}", num.value());
                //     return num.value();
                // }



            } break;
        }
    }

private:
    static std::optional<std::string> read_while(
        std::istringstream &stream,
        std::function<bool(char)> predicate
    ) {

        if (!predicate(stream.peek())) return { };

        char c;
        std::string acc;

        while (predicate(c = stream.get())) {
            acc.push_back(c);
        }

        return acc;
    }

};
