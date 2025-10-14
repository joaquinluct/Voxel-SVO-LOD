// Minimal drop-in shim for nlohmann::json used in this project
// Only implements the small API used by UIRenderer for atlas parsing.
#pragma once

#include <string>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <cstdlib>

namespace nlohmann {

class json {
public:
    enum class Type { Null, Object, Number, String };

    json() : m_type(Type::Null), m_number(0) {}

    static json parse(const std::string& s) {
        json root;
        root.m_type = Type::Object;
        // simple regex-based parser for the specific atlas_ui.json shape
        std::smatch m;
        std::regex reTex("\"textureSize\"\s*:\s*(\\d+)");
        if (std::regex_search(s, m, reTex)) {
            int v = std::atoi(m[1].str().c_str());
            json j; j.m_type = Type::Number; j.m_number = v;
            root.m_object["textureSize"] = j;
        }
        std::regex reCell("\"cellSize\"\s*:\s*(\\d+)");
        if (std::regex_search(s, m, reCell)) {
            int v = std::atoi(m[1].str().c_str());
            json j; j.m_type = Type::Number; j.m_number = v;
            root.m_object["cellSize"] = j;
        }
        // find glyphs block
        std::regex reGlyphsBlock("\\\"glyphs\\\"\\s*:\\s*\\{(.*)\\}\\s*\\Z", std::regex::icase);
        if (!std::regex_search(s, m, reGlyphsBlock)) {
            // fallback: find first { after "glyphs"
            std::size_t pos = s.find("\"glyphs\"");
            if (pos != std::string::npos) {
                std::size_t brace = s.find('{', pos);
                if (brace != std::string::npos) {
                    int depth = 1;
                    std::size_t i = brace + 1;
                    for (; i < s.size(); ++i) {
                        if (s[i] == '{') ++depth;
                        else if (s[i] == '}') { --depth; if (depth == 0) break; }
                    }
                    if (i < s.size()) {
                        std::string block = s.substr(brace + 1, i - brace - 1);
                        // parse entries inside block
                        parseGlyphEntries(block, root);
                    }
                }
            }
        } else {
            std::string block = m[1].str();
            parseGlyphEntries(block, root);
        }

        return root;
    }

    bool contains(const std::string& key) const {
        if (m_type != Type::Object) return false;
        return m_object.find(key) != m_object.end();
    }

    bool is_object() const { return m_type == Type::Object; }
    bool is_number_integer() const { return m_type == Type::Number; }

    const json& operator[](const std::string& key) const {
        static json empty;
        auto it = m_object.find(key);
        if (it == m_object.end()) return empty;
        return it->second;
    }

    json& operator[](const std::string& key) {
        m_type = Type::Object;
        return m_object[key];
    }

    template<typename T>
    T get() const {
        // only int supported in this shim
        return (T)m_number;
    }

    // items() support: return vector of pairs
    std::vector<std::pair<std::string, json>> items() const {
        std::vector<std::pair<std::string, json>> out;
        if (m_type != Type::Object) return out;
        for (auto& kv : m_object) out.emplace_back(kv.first, kv.second);
        return out;
    }

private:
    Type m_type;
    int m_number;
    std::string m_string;
    std::map<std::string, json> m_object;

    static void parseGlyphEntries(const std::string& block, json& root) {
        // regex to find entries like "65": { ... }
        std::regex reEntry("\"(\\d+)\"\s*:\s*\\{([^}]*)\\}");
        auto begin = std::sregex_iterator(block.begin(), block.end(), reEntry);
        auto end = std::sregex_iterator();
        json glyphsObj; glyphsObj.m_type = Type::Object;
        for (auto it = begin; it != end; ++it) {
            std::smatch m = *it;
            std::string key = m[1].str();
            std::string inside = m[2].str();
            json entry; entry.m_type = Type::Object;
            // find numbers x,y,w,h,advance
            std::regex reNum("\"(x|y|w|h|advance)\"\s*:\s*(\\d+)");
            auto b2 = std::sregex_iterator(inside.begin(), inside.end(), reNum);
            for (auto it2 = b2; it2 != end; ++it2) {
                std::smatch m2 = *it2;
                std::string name = m2[1].str();
                int val = std::atoi(m2[2].str().c_str());
                json v; v.m_type = Type::Number; v.m_number = val;
                entry.m_object[name] = v;
            }
            glyphsObj.m_object[key] = entry;
        }
        root.m_object["glyphs"] = glyphsObj;
    }
};

} // namespace nlohmann
