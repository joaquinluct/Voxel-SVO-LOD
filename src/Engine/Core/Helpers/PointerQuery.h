#pragma once

#include <vector>
#include <memory>
#include <ranges>
#include <algorithm>
#include <functional>
#include <Core/Helpers/PointerView.h>

template<typename T>
class PointerQuery : public PointerView<T> {
public:
    using typename PointerView<T>::SharedList;
    using typename PointerView<T>::RawView;

    PointerQuery(const SharedList& source)
        : PointerView<T>(source) {}

    PointerQuery& Filter(std::function<bool(T*)> predicate) {
        m_filters.push_back(std::move(predicate));
        return *this;
    }

    PointerQuery& Sort(std::function<bool(T*, T*)> comparator) {
        m_sorter = std::move(comparator);
        return *this;
    }

    RawView View() const override {
        auto base = PointerView<T>::View();
        for (const auto& f : m_filters) {
            base = base | std::views::filter(f);
        }
        return base;
    }

    std::vector<T*> ToVector() const override {
        std::vector<T*> result(View().begin(), View().end());
        if (m_sorter) {
            std::sort(result.begin(), result.end(), m_sorter);
        }
        return result;
    }

private:
    std::vector<std::function<bool(T*)>> m_filters;
    std::function<bool(T*, T*)> m_sorter;
};