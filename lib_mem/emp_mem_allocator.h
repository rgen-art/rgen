//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MEM_ALLOCATOR_H
#define EMP_MEM_ALLOCATOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_util.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tim_time.h"

#include "emp_tt_finally.h"

#include "emp_hh_new.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@info -fsanitize=address

#ifdef EMP_XX_TRACE_LEAKS
#define EMP_TRACE_LEAKS_ARG EMP_TRACE_ARG
#define EMP_TRACE_LEAKS_ARG_COMMA EMP_TRACE_ARG_COMMA
#define EMP_TRACE_LEAKS_PARAM EMP_TRACE_PARAM
#define EMP_TRACE_LEAKS_PARAM_COMMA EMP_TRACE_PARAM_COMMA
#define EMP_TRACE_LEAKS_HERE EMP_TRACE_HERE
#define EMP_TRACE_LEAKS_HERE_COMMA EMP_TRACE_HERE_COMMA
#else
#define EMP_TRACE_LEAKS_ARG
#define EMP_TRACE_LEAKS_ARG_COMMA
#define EMP_TRACE_LEAKS_PARAM
#define EMP_TRACE_LEAKS_PARAM_COMMA
#define EMP_TRACE_LEAKS_HERE
#define EMP_TRACE_LEAKS_HERE_COMMA
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct alloc_s
{
#if defined EMP_XX_TRACE_LEAKS
    size_t m_stIndex = 0;
    alloc_s* m_pPrevious = nullptr;
    alloc_s* m_pNext = nullptr;
    void* m_pAllocator = nullptr;
    emp::ast::trace_s m_Trace;
#endif

    size_t m_stOffset;
    size_t m_stRawSize;
    size_t m_stReqSize;

    alloc_s(void) = delete;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, allocator_c)
private:
    static allocator_c* s_pInstance;

public:
    #define EMP_ALLOCATOR emp::mem::allocator_c::instance()
    static void set_instance(allocator_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN allocator_c& instance(void) { return *s_pInstance; }

private:
    EMP_XX_IF_PROFILE(
        emp::tim::watch_c m_AllocationWatch;
        emp::tim::watch_c m_DeallocationWatch;
        )

#if defined EMP_XX_TRACE_LEAKS
        size_t m_stIndex = 0;
        alloc_s* m_pLeaks = nullptr;
        alloc_s* m_pLeakStart = nullptr;
#endif

        profile_s m_RawProfile{};
        profile_s m_RequestedProfile{};

public:
    allocator_c(void);
    ~allocator_c(void);
    void destroy(void);

#ifdef EMP_XX_PROFILE
public:
    EMP_INLINE EMP_RETURN emp::tim::watch_c const& alloc(void) const { return m_AllocationWatch; }
    EMP_INLINE EMP_RETURN emp::tim::watch_c const& dealloc(void) const { return m_DeallocationWatch; }
#endif

public:
    EMP_RETURN profile_s raw(void) const { return m_RawProfile; }
    EMP_RETURN profile_s req(void) const { return m_RequestedProfile; }

    EMP_RETURN size_t current_bytes(void) const;
    EMP_RETURN size_t current_count(void) const;

public:
#if defined EMP_XX_TRACE_LEAKS
    EMP_INLINE void leak_start(void) { m_pLeakStart = m_pLeaks; }
    EMP_INLINE void leak_stop(void) { EMP_ASSERT_BOOL_CALL(leak_check(m_pLeakStart)); }
#endif

public:
    template <typename T, typename ...P>
    EMP_INLINE EMP_RETURN T* allocate(EMP_TRACE_LEAKS_PARAM_COMMA P const&... a_Params)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_AllocationWatch);)
        return private_allocate<T, P...>(EMP_TRACE_LEAKS_ARG_COMMA a_Params...);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* allocate_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_AllocationWatch);)
        return private_allocate_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* allocate_pod_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_AllocationWatch);)
        return private_allocate_pod_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* allocate_aligned_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount, c_size a_stAlignment)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_AllocationWatch);)
        return private_allocate_aligned_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount, a_stAlignment);
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* allocate_aligned_pod_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount, c_size a_stAlignment)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_AllocationWatch);)
        return private_allocate_aligned_pod_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount, a_stAlignment);
    }

public:
    template <typename T>
    EMP_INLINE void deallocate(T* const a_pPointer)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_DeallocationWatch);)
        private_deallocate<T>(a_pPointer);
    }

    template <typename T>
    EMP_INLINE void deallocate_array(T* const a_pPointer)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_DeallocationWatch);)
        private_deallocate_array<T>(a_pPointer);
    }

    template <typename T>
    EMP_INLINE void deallocate_pod_array(T* const a_pPointer)
    {
        EMP_XX_IF_PROFILE(auto const oScope = emp::tt::scope(m_DeallocationWatch);)
        private_deallocate_pod<T>(a_pPointer);
    }

private:
    template <typename T, typename ...P>
    EMP_INLINE void ctor(T* const a_Address, P const&... a_Params)
    {
        EMP_ASSERT(a_Address != nullptr);
        new (a_Address)T(a_Params...);
    }

    template <typename T>
    EMP_INLINE void ctor_array(T* const a_Address, c_size a_stCount)
    {
        EMP_ASSERT(a_Address != nullptr);
        EMP_ASSERT(a_stCount > 0);

        T* pIterator = a_Address;
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        { ctor(pIterator + stIndex); }
    }

private:
    EMP_PRAGMA_PUSH_IGNORE(EMP_W_UNREFERENCED_PARAMETER) // @@0 unreferenced ??
    template <typename T>
    EMP_INLINE void dtor(T* const a_Address)
    {
        EMP_ASSERT(a_Address != nullptr);
        a_Address->~T();
    }
    EMP_PRAGMA_POP_IGNORE(EMP_W_UNREFERENCED_PARAMETER)

    template <typename T>
    EMP_INLINE void dtor_array(T* const a_Address, c_size a_stCount)
    {
        EMP_ASSERT(a_Address != nullptr);
        EMP_ASSERT(a_stCount > 0);
        for (size_t stIndex = 0; stIndex < a_stCount; ++stIndex)
        { dtor(a_Address + stIndex); }
    }

private:
    template <typename T, typename ...P>
    EMP_INLINE EMP_RETURN T* private_allocate(EMP_TRACE_LEAKS_PARAM_COMMA P const&... a_Params)
    {
        T* pReturn = this->private_allocate_pod<T>(EMP_TRACE_LEAKS_ARG);
        EMP_ASSERT(pReturn != nullptr);
        ctor<T, P...>(pReturn, a_Params...);
        return pReturn;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* private_allocate_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount)
    {
        EMP_ASSERT(a_stCount > 0);
        T* pReturn = private_allocate_pod_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount);
        EMP_ASSERT(pReturn != nullptr);
        ctor_array(pReturn, a_stCount);
        return pReturn;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* private_allocate_pod(EMP_TRACE_LEAKS_PARAM)
    {
        c_size stSize = sizeof(T);

        T* pReturn = reinterpret_cast<T*>(allocate(EMP_TRACE_LEAKS_ARG_COMMA stSize, alignof(T)));
        EMP_ASSERT(pReturn != nullptr);
        return pReturn;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* private_allocate_pod_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount)
    {
        EMP_ASSERT(a_stCount > 0);

        c_size stSize = sizeof(T) * a_stCount;

        T* pReturn = reinterpret_cast<T*>(allocate(EMP_TRACE_LEAKS_ARG_COMMA stSize, alignof(T)));
        EMP_ASSERT(pReturn != nullptr);
        return pReturn;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* private_allocate_aligned_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount, c_size a_stAlignment)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stAlignment > 0);
        T* pReturn = private_allocate_aligned_pod_array<T>(EMP_TRACE_LEAKS_ARG_COMMA a_stCount, a_stAlignment);
        EMP_ASSERT(pReturn != nullptr);
        ctor_array(pReturn, a_stCount);
        return pReturn;
    }

    template <typename T>
    EMP_INLINE EMP_RETURN T* private_allocate_aligned_pod_array(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stCount, c_size a_stAlignment)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stAlignment > 0);

        c_size stSize = sizeof(T) * a_stCount;

        T* pReturn = reinterpret_cast<T*>(allocate(EMP_TRACE_LEAKS_ARG_COMMA stSize, a_stAlignment));
        EMP_ASSERT(pReturn != nullptr);
        return pReturn;
    }

private:
    ENABLE_IF_T(false == std::is_polymorphic<T>::value)
    static EMP_INLINE EMP_RETURN void* get_address(T* const a_pPointer) { return a_pPointer; }

    EMP_PRAGMA_PUSH_IGNORE(EMP_W_4541)
    ENABLE_IF_T(std::is_polymorphic<T>::value)
    static EMP_INLINE EMP_RETURN void* get_address(T* const a_pPointer) { return dynamic_cast<void*>(a_pPointer); }
    EMP_PRAGMA_POP_IGNORE(EMP_W_4541)

    ENABLE_IF_T(false == std::is_polymorphic<T>::value)
    static EMP_INLINE EMP_RETURN void const* get_address(T const* const a_pPointer) { return a_pPointer; }

    EMP_PRAGMA_PUSH_IGNORE(EMP_W_4541)
    ENABLE_IF_T(std::is_polymorphic<T>::value)
    static EMP_INLINE EMP_RETURN void const* get_address(T const* const a_pPointer) { return dynamic_cast<void const*>(a_pPointer); }
    EMP_PRAGMA_POP_IGNORE(EMP_W_4541)

    template <typename T>
    EMP_INLINE void private_deallocate(T* const a_pPointer)
    {
        void* pAddress = get_address(a_pPointer);
        alloc_s* const pAlloc = get_alloc(pAddress);

        dtor(a_pPointer);
        deallocate(pAddress, pAlloc);
    }

    template <typename T>
    EMP_INLINE void private_deallocate_array(T* const a_pPointer)
    {
        void* const pAddress = get_address(a_pPointer);
        alloc_s* const pAlloc = get_alloc(pAddress);

        c_size stRequestedSize = pAlloc->m_stReqSize;

        EMP_ASSERT(stRequestedSize > 0);
        EMP_ASSERT((stRequestedSize % sizeof(T)) == 0);

        c_size stCount = stRequestedSize / sizeof(T);
        EMP_ASSERT(stCount > 0);

        dtor_array(a_pPointer, stCount);
        deallocate(pAddress, pAlloc);
    }

    template <typename T>
    EMP_INLINE void private_deallocate_pod(T* const a_pPointer)
    {
        void* const pAddress = get_address(a_pPointer);
        alloc_s* const pAlloc = get_alloc(pAddress);
        deallocate(pAddress, pAlloc);
    }

#if defined EMP_XX_TRACE_LEAKS
    EMP_RETURN bool leak_check(alloc_s const* const a_pStart) const;
    void alloc_hook(alloc_s const* const a_pAlloc);
#endif

    static EMP_RETURN void* alloc(c_size a_stSize);
    static void dealloc(void* a_pAddress);

    EMP_RETURN void* allocate(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stSize, c_size a_stAlign);
    void deallocate(void* const
#if defined EMP_XX_ASSERT || defined EMP_XX_TRACE_LEAKS
        a_Address
#endif
        , alloc_s* const a_pAlloc);

private:
    EMP_INLINE EMP_RETURN alloc_s* get_alloc(void* const a_pAddress) const
    {
        EMP_ASSERT(a_pAddress != nullptr);
        return (static_cast<alloc_s*>(a_pAddress) - 1);
    }

    EMP_INLINE EMP_RETURN alloc_s const* get_alloc(void const* const a_pAddress) const
    {
        EMP_ASSERT(a_pAddress != nullptr);
        return (static_cast<alloc_s const*>(a_pAddress) - 1);
    }

public:
    template <typename T>
    EMP_INLINE EMP_RETURN size_t requested_size(T const* const a_pPointer)
    { return get_alloc(get_address(a_pPointer))->m_stReqSize; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_ALLOCATE_1(...)                  EMP_ALLOCATOR.allocate<EMP_PP_TRY_SOLVE(__VA_ARGS__)>(EMP_TRACE_LEAKS_HERE)
#define EMP_ALLOCATE_0(x_Type, ...)          EMP_ALLOCATOR.allocate<EMP_PP_TRY_SOLVE(x_Type)>(EMP_TRACE_LEAKS_HERE_COMMA __VA_ARGS__)

#define EMP_ALLOCATE(...) EMP_PP_FORWARD(EMP_PP_CAT(EMP_ALLOCATE_, EMP_PP_EQUAL(1, EMP_VAARGS_COUNT(__VA_ARGS__)))(__VA_ARGS__))
#define EMP_ALLOCATE_POD_ARRAY(x_Type, x_Count) EMP_ALLOCATOR.allocate_pod_array<EMP_PP_TRY_SOLVE(x_Type)>(EMP_TRACE_LEAKS_HERE_COMMA x_Count)
#define EMP_ALLOCATE_ARRAY(x_Type, x_Count)     EMP_ALLOCATOR.allocate_array<EMP_PP_TRY_SOLVE(x_Type)>(EMP_TRACE_LEAKS_HERE_COMMA x_Count)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_ALLOCATE_ALIGNED_POD_ARRAY(x_Type, x_Count, x_Alignment)\
    EMP_ALLOCATOR.allocate_aligned_pod_array<EMP_PP_TRY_SOLVE(x_Type)>(EMP_TRACE_LEAKS_HERE_COMMA x_Count, x_Alignment)

#define EMP_ALLOCATE_ALIGNED_ARRAY(x_Type, x_Count, x_Alignment)\
    EMP_ALLOCATOR.allocate_aligned_array<EMP_PP_TRY_SOLVE(x_Type)>(EMP_TRACE_LEAKS_HERE_COMMA x_Count, x_Alignment)

#define EMP_DEALLOCATE_ALIGNED_POD_ARRAY(x_Address) EMP_ALLOCATOR.deallocate_pod_array(x_Address)
#define EMP_DEALLOCATE_ALIGNED_ARRAY(x_Address) EMP_ALLOCATOR.deallocate_array(x_Address)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_DEALLOCATE(x_Address)           EMP_ALLOCATOR.deallocate(x_Address)
#define EMP_DEALLOCATE_POD_ARRAY(x_Address) EMP_ALLOCATOR.deallocate_pod_array(x_Address)
#define EMP_DEALLOCATE_ARRAY(x_Address)     EMP_ALLOCATOR.deallocate_array(x_Address)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
