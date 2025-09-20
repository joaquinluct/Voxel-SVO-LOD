#pragma once

#include <algorithm>
#include <functional>
#include <ranges>
#include <vector>

template<typename T>
class PointerQueryRaw {
public:
    using RawList = std::vector<T*>;

    // El constructor ahora toma la lista por valor y la mueve, garantizando
    // que la clase posea una copia válida de los datos.
    PointerQueryRaw(RawList source)
        : m_source(std::move(source)) {
    }

    PointerQueryRaw& Filter(std::function<bool(T*)> predicate) {
        m_filters.push_back(std::move(predicate));
        return *this;
    }

    PointerQueryRaw& Sort(std::function<bool(T*, T*)> comparator) {
        m_sorter = std::move(comparator);
        return *this;
    }

    auto View() const {
        return m_source | std::views::filter([&](T* item) {
            for (const auto& f : m_filters) {
                if (!f(item)) {
                    return false;
                }
            }
            return true;
            });
    }

    std::vector<T*> ToVector() const {
        std::vector<T*> result;
        for (T* item : View()) {
            result.push_back(item);
        }
        if (m_sorter) {
            std::sort(result.begin(), result.end(), m_sorter);
        }
        return result;
    }

private:
    // Ya no es una referencia, sino un vector de verdad.
    RawList m_source;
    std::vector<std::function<bool(T*)>> m_filters;
    std::function<bool(T*, T*)> m_sorter;
};
