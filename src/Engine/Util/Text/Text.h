#pragma once
#include <string>

inline std::string normalizeBackslashes(std::string path) {
    // La subcadena que queremos buscar (dos barras invertidas literales)
    // En C++, para representar '\\', necesitas escribir '\\\\'.
    const std::string search_str = "\\\\";

    // La subcadena por la que queremos reemplazarla (una barra invertida literal)
    // En C++, para representar '\', necesitas escribir '\\'.
    const std::string replace_str = "\\";

    size_t pos = path.find(search_str); // Busca la primera ocurrencia de "\\\\"

    // Itera mientras se sigan encontrando ocurrencias
    while (pos != std::string::npos) {
        // Reemplaza la ocurrencia encontrada
        path.replace(pos, search_str.length(), replace_str);

        // Busca la siguiente ocurrencia, comenzando desde la posición
        // donde terminamos el reemplazo (pos + replace_str.length())
        // Esto es crucial para manejar casos como "\\\\" (cuatro barras), convirtiéndolo a "\"
        pos = path.find(search_str, pos + replace_str.length());
    }
    return path;
}

inline const wchar_t* ConvertFromString(const std::string str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    const wchar_t* wide_cstr = wstr.c_str();
    return wide_cstr;
}

inline std::string EscapeCppStringLiteral(const std::string& input) {
    std::string result;
    result.reserve(input.size() * 2); // Optimista

    for (char c : input) {
        switch (c) {
        case '\\': result += "\\\\"; break;
        case '"':  result += "\\\""; break;
        case '\n': result += "\\n";  break;
        case '\r': result += "\\r";  break;
        case '\t': result += "\\t";  break;
        case '\0': result += "\\0";  break;
        default:
            if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) > 0x7E) {
                // Escapar como secuencia hexadecimal
                char buf[5];
                std::snprintf(buf, sizeof(buf), "\\x%02X", static_cast<unsigned char>(c));
                result += buf;
            }
            else {
                result += c;
            }
        }
    }

    return result;
}