#pragma once
#include <string>
#include <filesystem>
#include <WinNls.h>

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

inline std::string roundFloat(float value, int numDecimals = 2) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(numDecimals) << value;
    return ss.str();
}

inline void StringReplace(std::string& s, const std::string& oldSubstr, const std::string newSubstr, bool all = true) {
    // Si la subcadena a buscar está vacía, no tiene sentido hacer nada.
    if (oldSubstr.empty()) {
        return;
    }

    size_t pos = 0; // Posición actual para iniciar la búsqueda
    while ((pos = s.find(oldSubstr, pos)) != std::string::npos) {
        // Encontramos una ocurrencia de oldSubstr
        // Reemplazamos oldSubstr por newSubstr
        s.replace(pos, oldSubstr.length(), newSubstr);

        // Avanzamos la posición para la siguiente búsqueda.
        // Es crucial avanzar por la longitud de newSubstr para evitar bucles infinitos
        // si newSubstr contiene oldSubstr (ej. reemplazar "a" con "aa").
        pos += newSubstr.length();

        if (!all) {
            // Si no queremos reemplazar todas las ocurrencias, salimos después del primer reemplazo.
            break;
		}
    }
}

inline std::string ParseInt(int number) {
    std::stringstream ss;
    ss << std::fixed << number;
    return ss.str();
}

inline std::string ParseFloat(float number) {
    std::stringstream ss;
    ss << std::fixed << number;
    return ss.str();
}

inline std::string StrToLower(std::string s) {
    // std::transform aplica una operación a cada elemento de un rango.
    // s.begin() y s.end() definen el rango (toda la cadena).
    // s.begin() como tercer argumento indica que el resultado se guarda en la misma cadena.
    // [](unsigned char c){ return std::tolower(c); } es una lambda expression.
    // Convierte cada carácter 'c' a su equivalente en minúscula.
    // Es importante usar unsigned char con std::tolower para evitar problemas con valores de caracteres negativos.
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

// Converts std::string (UTF-8 assumed) to std::wstring (UTF-16)
inline std::wstring StringToWstring(const std::string& str) {
    if (str.empty()) {
        return L"";
    }

    // Determine the size needed for the wide string
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    if (size_needed == 0) {
        // Handle error, e.g., GetLastError()
        return L"";
    }

    std::vector<wchar_t> wstr_buf(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr_buf[0], size_needed);

    return std::wstring(wstr_buf.begin(), wstr_buf.end());
}

// Converts std::wstring (UTF-16 assumed) to std::string (UTF-8)
inline std::string WstringToString(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }

    // Determine the size needed for the multi-byte string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    if (size_needed == 0) {
        // Handle error, e.g., GetLastError()
        return "";
    }

    std::vector<char> str_buf(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &str_buf[0], size_needed, NULL, NULL);

    return std::string(str_buf.begin(), str_buf.end());
}