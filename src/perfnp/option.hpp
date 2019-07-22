// Copyright (c) 2019 Locksley.CZ s.r.o.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PERFNP_OPTION_H_
#define PERFNP_OPTION_H_

#include <memory>

namespace perfnp {

template<class T>
class Optional {

    std::unique_ptr<T> m_object;

public:

    bool is_empty() const
    {
        return !static_cast<bool>(m_object);
    }

    T& operator*() const
    {
        return *m_object;
    }

    T* operator->() const
    {
        return m_object.get();
    }

    bool operator==(const Optional<T>& rhs) const
    {
        if (!bool(m_object) && !bool(rhs.m_object)) {
            return true;
        }

        if (bool(m_object) && bool(rhs.m_object)) {
            return true;
        }

        return *m_object == *rhs.m_object;
    }

    bool operator==(const T& rhs) const
    {
        return bool(m_object) && *m_object == rhs;
    }

    template<class... A>
    static Optional<T> make(A&&... a)
    {
        Optional<T> out;
        out.m_object.reset(new T(std::forward<T>(a...)));
        return out;
    }

    static Optional<T> empty()
    {
        return Optional<T>();
    }
}; // Optional
} // perfnp NS
#endif
