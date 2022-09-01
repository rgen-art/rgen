//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_CNT_MAP_H
#define EMP_CNT_MAP_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"
#include "emp_cnt_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace cnt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K>
EMP_NOINSTANCE(struct, key_t)
    //@@0 tt::cast
    static EMP_RETURN uintptr_t compute(K const& a_kValue) { return static_cast<uintptr_t>(a_kValue); }
    static EMP_RETURN bool compare(K const& a_k0, K const& a_k1) { return a_k0 == a_k1; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K>
EMP_NOINSTANCE(struct, key_t<K*>)
    static EMP_RETURN uintptr_t compute(K const* const a_pkValue) { return reinterpret_cast<uintptr_t>(a_pkValue); }
    static EMP_RETURN bool compare(K const* const a_pk0, K const* const a_pk1) { return a_pk0 == a_pk1; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K>
EMP_NOINSTANCE(struct, key_t<K const*>)
static EMP_RETURN uintptr_t compute(K const* const a_pkValue) { return reinterpret_cast<uintptr_t>(a_pkValue); }
static EMP_RETURN bool compare(K const* const a_pk0, K const* const a_pk1) { return a_pk0 == a_pk1; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <> EMP_NOINSTANCE(struct, key_t<c_char*>)
    static EMP_RETURN uintptr_t compute(cpc_char a_szValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <> EMP_NOINSTANCE(struct, key_t<char*>)
    static EMP_RETURN uintptr_t compute(char* const a_szValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <> EMP_NOINSTANCE(struct, key_t<str::astring>)
    static EMP_RETURN uintptr_t compute(str::c_astring& a_sValue)
    {
        uintptr_t uiHash = 0;
        c_size stCount = a_sValue.len();

        auto const l_hash = [&uiHash, stCount, &a_sValue](c_size a_stIndex, c_size a_stCount)
        {
            uintptr_t uiSeed = 131;
            c_size stMax = emp::tt::minof(stCount, a_stCount);
            for (size_t stIndex = a_stIndex; stIndex < stMax; ++stIndex)
            { uiHash = (uiHash * uiSeed) + emp::ast::cast<uintptr_t>(a_sValue.get_char(stIndex)); }
        };

        l_hash(0u, 16u);
        return uiHash;
    }

    static EMP_RETURN bool compare(str::c_astring& a_rs0, str::c_astring& a_rs1) { return str::cmp(a_rs0, a_rs1); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <> EMP_NOINSTANCE(struct, key_t<size_t>)
    static EMP_RETURN uintptr_t compute(c_size a_stValue) { return a_stValue; }
    static EMP_RETURN bool compare(c_size a_st0, c_size a_st1) { return a_st0 == a_st1; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_COMPILER_MSC
#else
template <>
EMP_NOINSTANCE(struct, key_t<ui32>)
public:
    static EMP_RETURN uintptr_t compute(c_ui32 a_ui32Value) { return a_ui32Value; }
    static EMP_RETURN bool compare(c_ui32 a_ui320, c_ui32 a_ui321) { return a_ui320 == a_ui321; }
};
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K, typename V>
EMP_NOCOPY(class, map_t)
public:
    typedef V type;

    EMP_(struct, cell_c)
    private:
        K m_Key;
        V m_Value;
    public:
        cell_c(void):
            m_Key(),
            m_Value()
        {}

        cell_c(K const& a_rKey, V const& a_rValue):
            m_Key(a_rKey),
            m_Value(a_rValue)
        {}

        ~cell_c(void)
        {}

    public:
        cell_c(cell_c const& a_rFrom):
            m_Key(a_rFrom.m_Key),
            m_Value(a_rFrom.m_Value)
        {
        }

        cell_c& operator=(cell_c const& a_rFrom)
        {
            m_Key = a_rFrom.m_Key;
            m_Value = a_rFrom.m_Value;
            return *this;
        }

    public:
        EMP_RETURN K const& key(void) const { return m_Key; }
        EMP_RETURN V const value(void) const { return m_Value; }
        EMP_RETURN V& value(void) { return m_Value; }
    };

    EMP_(struct, iterator_c)
    private:
        friend class map_t;
        size_t m_stBucket;
        size_t m_stIndex;
        K const* m_pKey;
        V* m_pValue;
        map_t const* m_pMap;

    public:
        iterator_c(void):
            EMP_TT_MAX_CTOR(m_stBucket),
            EMP_TT_MAX_CTOR(m_stIndex),
            m_pKey(nullptr),
            m_pValue(nullptr),
            m_pMap()
        {}

        explicit iterator_c(map_t const* a_pMap):
            EMP_TT_MAX_CTOR(m_stBucket),
            EMP_TT_MAX_CTOR(m_stIndex),
            m_pKey(nullptr),
            m_pValue(nullptr),
            m_pMap(a_pMap)
        {}

        iterator_c(c_size a_stBucket, c_size a_stIndex,
            K const* const a_pKey, V* const a_pValue,
            map_t const* const a_pMap):
                m_stBucket(a_stBucket),
                m_stIndex(a_stIndex),
                m_pKey(a_pKey),
                m_pValue(a_pValue),
                m_pMap(a_pMap)
        {}

        ~iterator_c(void)
        {}

    public:
        iterator_c(iterator_c const& a_rFrom):
            m_stBucket(a_rFrom.m_stBucket),
            m_stIndex(a_rFrom.m_stIndex),
            m_pKey(a_rFrom.m_pKey),
            m_pValue(a_rFrom.m_pValue),
            m_pMap(a_rFrom.m_pMap)
        {}

        iterator_c& operator=(iterator_c const& a_rFrom)
        {
            m_stBucket = a_rFrom.m_stBucket;
            m_stIndex = a_rFrom.m_stIndex;
            m_pKey = a_rFrom.m_pKey;
            m_pValue = a_rFrom.m_pValue;
            m_pMap = a_rFrom.m_pMap;
            return *this;
        }

    public:
        iterator_c& operator++(void)
        {
            bool bFirst = false;
            while (m_pMap->m_vpBuckets.is_valid_index(m_stBucket))
            {
                vector<cell_c>* pBucket = m_pMap->m_vpBuckets.val(m_stBucket);
                if (pBucket)
                {
                    c_size stOffset = m_stIndex + (bFirst ? 0 : 1);
                    if (pBucket->is_valid_index(stOffset))
                    {
                        m_stIndex = stOffset;
                        m_pKey = &pBucket->ref(m_stIndex).key();
                        m_pValue = &pBucket->ref(m_stIndex).value();
                        return *this;
                    }
                    m_stIndex = 0;
                }

                bFirst = true;
                ++m_stBucket;
            }
            m_pKey = nullptr;
            m_pValue = nullptr;
            *this = m_pMap->end();
            return *this;
        }

    public:
        EMP_RETURN bool operator==(iterator_c const& a_rFrom) const
        {
            EMP_ASSERT(m_pMap == a_rFrom.m_pMap);
            return
                m_stBucket == a_rFrom.m_stBucket &&
                m_stIndex == a_rFrom.m_stIndex;
        }

        EMP_RETURN bool operator!=(iterator_c const& a_rFrom) const
        { return operator==(a_rFrom) == false; }

    public:
        EMP_RETURN bool is_valid(void) const
        {
            if (m_pMap->m_vpBuckets.is_valid_index(m_stBucket))
            {
                vector<cell_c>* pBucket = m_pMap->m_vpBuckets.val(m_stBucket);
                if (pBucket != nullptr)
                { return pBucket->is_valid_index(m_stIndex); }
            }
            return false;
        }

        EMP_RETURN bool end(void) const
        { return this->operator==(m_pMap->end()); }
    
        EMP_RETURN bool not_end(void) const
        { return this->operator!=(m_pMap->end()); }

    public:
        EMP_RETURN size_t bucket(void) const { return m_stBucket; }
        EMP_RETURN size_t index(void) const { return m_stIndex; }
        EMP_RETURN K const* key(void) const { return m_pKey; }
        EMP_RETURN V* value(void) const { return m_pValue; }
    };

private:
    iterator_c m_itEnd;
    vector<vector<cell_c>*> m_vpBuckets;
    size_t m_stCount = 0;

public:
    map_t(c_size a_stCount):
        m_itEnd(),
        m_vpBuckets(),
        m_stCount()
    {
        m_itEnd.m_pMap = this;
        m_vpBuckets.resize(a_stCount);
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        { m_vpBuckets.set(stIndex, nullptr); }
    }

    ~map_t(void)
    { destroy(); }

public:
    EMP_RETURN size_t seed(void) const { return m_vpBuckets.count(); }
public:
    void destroy(void)
    {
        c_size stCount = m_vpBuckets.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            vector<cell_c>* const pBucket = m_vpBuckets.val(stIndex);
            if (pBucket != nullptr)
            { EMP_DEALLOCATE(pBucket); }
        }
        m_vpBuckets.resize(0);
    }

    void resize(c_size a_stCount)
    {
        destroy();
        m_vpBuckets.resize(a_stCount);
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        { m_vpBuckets.set(stIndex, nullptr); }
    }

    EMP_RETURN iterator_c get(K const& a_rKey) const
    {
        if (m_stCount == 0)
        { return m_itEnd; }

        uintptr_t const iKey = key_t<K>::compute(a_rKey);

        c_size stCount = m_vpBuckets.count();
        c_size stBucket = iKey % stCount;
        vector<cell_c>* const pBucket = m_vpBuckets.val(stBucket);
        if (pBucket == nullptr)
        { return end(); }
        c_size stValueCount = pBucket->count();
        for (size_t stIndex = 0; stIndex < stValueCount; ++stIndex)
        {
            cell_c& rCell = pBucket->ref(stIndex);
            if (key_t<K>::compare(rCell.key(), a_rKey))
            { return iterator_c(stBucket, stIndex, &rCell.key(), &rCell.value(), this); }
        }
        return m_itEnd;
    }

    EMP_RETURN bool has(K const& a_rKey) const { return get(a_rKey) != end(); }
    EMP_RETURN bool has_not(K const& a_rKey) const { return has(a_rKey) == false; }

    EMP_INLINE void set(K const& a_rKey, V const& a_rValue)
    {
        uintptr_t const iKey = key_t<K>::compute(a_rKey);

        c_size stCount = m_vpBuckets.count();
        c_size stBucket = iKey % stCount;
        vector<cell_c>* pBucket = m_vpBuckets.val(stBucket);
        if (pBucket == nullptr)
        {
            pBucket = EMP_ALLOCATE(vector<cell_c>);
            m_vpBuckets.set(stBucket, pBucket);
        }

        c_size stValueCount = pBucket->count();
        for (size_t stIndex = 0; stIndex < stValueCount; ++stIndex)
        {
            cell_c const& rCell = pBucket->ref(stIndex);
            if (key_t<K>::compare(rCell.key(), a_rKey))
            {
                pBucket->set(stIndex, cell_c(a_rKey, a_rValue));
                return;
            }
        }
        pBucket->add(cell_c(a_rKey, a_rValue));
        ++m_stCount;
    }

    void remove(iterator_c const& a_rIt)
    {
        vector<cell_c>* const pBucket = m_vpBuckets.val(a_rIt.bucket());
        EMP_ASSERT(pBucket != nullptr);
        pBucket->remove_unordered(a_rIt.index());
        EMP_ASSERT(m_stCount > 0);
        --m_stCount;
    }

    void clear(void)
    {
        c_size stCount = m_vpBuckets.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            vector<cell_c>* const pBucket = m_vpBuckets.val(stIndex);
            if (pBucket)
            { pBucket->resize(0); }
        }
        m_stCount = 0;
    }

public:
    EMP_RETURN bool is_empty(void) const { return begin() == end(); }

    EMP_RETURN size_t count(void) const { return m_stCount; }

public:
    EMP_RETURN iterator_c begin(void) const
    {
        if (m_stCount == 0)
        { return m_itEnd; }

        c_size stBucketCount = m_vpBuckets.count();
        for (size_t stBucket = 0; stBucket < stBucketCount; ++stBucket)
        {
            vector<cell_c>* const pBucket = m_vpBuckets.val(stBucket);
            if (pBucket && pBucket->count())
            {
                cell_c& rCell = pBucket->ref(0);
                return iterator_c(stBucket, 0, &rCell.key(), &rCell.value(), this);
            }
        }
        return m_itEnd;
    }

    EMP_RETURN iterator_c const& end(void) const { return m_itEnd; }

    EMP_RETURN size_t bucket_count(void) const { return m_vpBuckets.count(); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K, typename V> void copy(emp::cnt::map_t<K, V> const& a_rFrom, emp::cnt::map_t<K, V>& a_rTo);
template <typename K, typename V> void copy(emp::cnt::map_t<K, V> const& a_rFrom, emp::cnt::map_t<K, V>& a_rTo)
{
    a_rTo.clear();
    append(a_rFrom, a_rTo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename K, typename V> void append(emp::cnt::map_t<K, V> const& a_rFrom, emp::cnt::map_t<K, V>& a_rTo);
template <typename K, typename V> void append(emp::cnt::map_t<K, V> const& a_rFrom, emp::cnt::map_t<K, V>& a_rTo)
{
    typedef emp::cnt::map_t<K, V> type;
    typedef typename type::iterator_c iterator;

    iterator const& rEnd = a_rFrom.end();
    for (iterator oIt = a_rFrom.begin(); oIt != rEnd; ++oIt)
    { a_rTo.set(*oIt.key(), *oIt.value()); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
