#ifndef etkf_array_hpp
#define etkf_array_hpp

namespace etkf {
    template <class T, size_t N>
    class array {
    public:
        T& operator[] (size_t n) { return m_storage[n]; }
        T operator[] (size_t n) const { return m_storage[n]; }

        auto size() const { return N; }

        auto begin() { return m_storage; }
        auto begin() const { return m_storage; }
        auto end() { return m_storage + N; }
        auto end() const { return m_storage + N; }

        T m_storage[N];
    };
}

#endif
