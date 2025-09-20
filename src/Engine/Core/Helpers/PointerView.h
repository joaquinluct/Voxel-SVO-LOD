#pragma once

#include <vector>
#include <memory>
#include <ranges>
#include <algorithm>

template<typename T>
class PointerView {
public:
    using SharedList = std::vector<std::shared_ptr<T>>;
    using RawView = decltype(std::declval<SharedList>() | std::views::transform([](const std::shared_ptr<T>& ptr) { return ptr.get(); }));

    PointerView(const SharedList& source) : m_source(source) { 
    }

    auto View() const {
        return m_source | std::views::transform([](const std::shared_ptr<T>& ptr) { return ptr.get(); });
    }

    virtual std::vector<T*> ToVector() const {
        std::vector<T*> result;
        result.reserve(m_source.size());
        std::transform(m_source.begin(), m_source.end(), std::back_inserter(result),
            [](const std::shared_ptr<T>& ptr) { return ptr.get(); });
        return result;
    }

protected:
    const SharedList& m_source;
};
