#ifndef etkf_static_vector_hpp
#define etkf_static_vector_hpp

#include "keys.hpp"

namespace etkf {
    template <class T, size_t N>
    class static_vector {
    public:
        static_vector() : m_storage{}, m_count{0} {
            for (auto& e : m_storage) {
                e = T{};
            }
        }

        static_vector(const static_vector& other)
            : m_count{other.size()}
        {
            for (auto i=0u; i < other.m_count; ++i) {
                m_storage[i] = other.m_storage[i];
            }
        }

        auto begin() { return m_storage; }
        auto begin() const { return m_storage; }
        auto end() { return m_storage + m_count; }
        auto end() const { return m_storage + m_count; }

        void push_back (T t) {
            if (m_count < N) {
                m_storage[m_count++] = t;
            }
        }

        T& operator[] (size_t n) { return m_storage[n]; }
        T operator[] (size_t n) const { return m_storage[n]; }

        auto size() const { return m_count; }

    private:
        T m_storage[N];
        size_t m_count;
    };
}

#endif
