//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_mem_allocator.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mem {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
allocator_c* allocator_c::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
allocator_c::allocator_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
allocator_c::~allocator_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void allocator_c::destroy(void)
{
#if defined EMP_XX_TRACE_LEAKS
    // TODO_RGEN: EMP_ASSERT_BOOL_CALL(leak_check(nullptr));
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t allocator_c::current_bytes(void) const
{
    c_size stAlloc = m_RequestedProfile.m_AllocBytes.m_stCount;
    c_size stDealloc = m_RequestedProfile.m_DeallocBytes.m_stCount;
    EMP_ASSERT(stAlloc >= stDealloc);
    return stAlloc > stDealloc ? stAlloc - stDealloc : 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t allocator_c::current_count(void) const
{
    c_size stAlloc = m_RequestedProfile.m_stAllocCount;
    c_size stDealloc = m_RequestedProfile.m_stDeallocCount;
    EMP_ASSERT(stAlloc > stDealloc);
    return stAlloc > stDealloc ? stAlloc - stDealloc : 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_TRACE_LEAKS
EMP_RETURN bool allocator_c::leak_check(alloc_s const* const a_pStart) const
{
    if (m_pLeaks == nullptr)
    {
        emp::ast::output_pchar("no leaks.... \\o/");
        return true;
    }

    size_t stLeakCount = 0;

    emp::ast::output_pchar(EMP_XSZ_S0 EMP_XSZ_NEWLINE);
    emp::ast::output_pchar("// LEAKING.....\n");
    emp::ast::output_pchar(EMP_XSZ_S0 EMP_XSZ_NEWLINE);

    alloc_s const* pElement = m_pLeaks;
    while (pElement && (pElement != a_pStart))
    {
        size_t stFileLen = 0;
        cpc_char szFile = pElement->m_Trace.m_szFile;
        while (szFile != nullptr && szFile[stFileLen] != '\0')
        { ++stFileLen; }

        size_t stNameLen = stFileLen - (stFileLen > 0 ? 1 : 0);
        while (szFile[stNameLen] != '/' && stNameLen > 0)
        { --stNameLen; }

        emp::ast::output_pchar("// L | ");
        emp::ast::output_pchar(szFile + stNameLen);
        emp::ast::output_pchar(":");
        emp::ast::output_pchar(pElement->m_Trace.m_szLine);
        emp::ast::output_pchar(" | ");
        emp::ast::output_pchar(pElement->m_Trace.m_szSignature);
        emp::ast::output_pchar(EMP_XSZ_NEWLINE);
        pElement = pElement->m_pPrevious;
        ++stLeakCount;
    }

    emp::ast::output_pchar(EMP_XSZ_S0 EMP_XSZ_NEWLINE);
    emp::ast::output_pchar("// LEAK COUNT : ");
    size_t stDigit = 0;
    size_t stCount = stLeakCount;
    char cLeakCount[64];
    do
    {
        cLeakCount[stDigit++] = emp::ast::cast<char>(stCount % 10 + '0');
        stCount /= 10;
    }
    while (stCount > 0);
    cLeakCount[stDigit] = '\0';

    c_size stHalfLen = stDigit / 2;
    for (size_t stIndex = 0; stIndex < stHalfLen; ++stIndex)
    {
        char c = cLeakCount[stIndex];
        size_t stLast = stDigit - stIndex - 1;
        cLeakCount[stIndex] = cLeakCount[stLast];
        cLeakCount[stLast] = c;
    }
    emp::ast::output_pchar(cLeakCount);
    emp::ast::output_pchar(("\n"));
    emp::ast::output_pchar(EMP_XSZ_S0 EMP_XSZ_NEWLINE);
    EMP_ASSERT(false);

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void allocator_c::alloc_hook(alloc_s const* const a_pAlloc)
{
    if (a_pAlloc->m_stIndex == 14172)
    {
        int x = 0;
        ++x;
    }
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* allocator_c::allocate(EMP_TRACE_LEAKS_PARAM_COMMA c_size a_stSize, c_size a_stAlign)
{
    EMP_ASSERT(a_stSize > 0);
    EMP_ASSERT(a_stAlign > 0);

    c_size stAlign = a_stAlign > alignof(alloc_s) ? a_stAlign : alignof(alloc_s);

    c_size stSize = a_stSize + sizeof(alloc_s) + stAlign;

    void* pMemory = alloc(stSize);

#ifdef EMP_XX_ASSERT
    ui8* const pTemp = static_cast<ui8*>(pMemory);
    for (size_t stIndex = 0; stIndex < stSize; ++stIndex)
    { pTemp[stIndex] = 0; }
#endif

    uintptr_t const uiMemory = reinterpret_cast<uintptr_t>(pMemory);
    uintptr_t const uiBase = uiMemory + sizeof(alloc_s);
    uintptr_t const uiOffset = stAlign - (uiBase % stAlign);
    uintptr_t const uiUser = uiBase + uiOffset;
    uintptr_t const uiPrivate = uiUser - sizeof(alloc_s);

    alloc_s* const pAlloc = reinterpret_cast<alloc_s*>(uiPrivate);

#if defined EMP_XX_TRACE_LEAKS
    if (m_pLeaks)
    { m_pLeaks->m_pNext = pAlloc; }
    pAlloc->m_pPrevious = m_pLeaks;
    pAlloc->m_pNext = nullptr;
    pAlloc->m_pAllocator = this;
    pAlloc->m_Trace = a_rTrace;
    pAlloc->m_stIndex = m_stIndex++;
    m_pLeaks = pAlloc;
    alloc_hook(pAlloc);
#endif

    pAlloc->m_stOffset = uiOffset;
    pAlloc->m_stRawSize = stSize;
    pAlloc->m_stReqSize = a_stSize;

    m_RawProfile.m_AllocBytes.m_stCount += stSize;
    ++m_RawProfile.m_stAllocCount;

    m_RequestedProfile.m_AllocBytes.m_stCount += a_stSize;
    ++m_RequestedProfile.m_stAllocCount;

    EMP_ASSERT((uiUser % stAlign) == 0);
    return reinterpret_cast<void*>(uiUser);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void allocator_c::deallocate(void* const
#if defined EMP_XX_ASSERT || defined EMP_XX_TRACE_LEAKS
    a_Address
#endif
    , alloc_s* const a_pAlloc)
{
    EMP_ASSERT(a_Address != nullptr);
    EMP_ASSERT(a_pAlloc != nullptr);

    auto const& l_dealloc_check = [](profile_s& a_rProfile, c_size a_stSize)
    {
        EMP_ASSERT(a_stSize > 0);
        EMP_ASSERT(a_rProfile.m_AllocBytes.m_stCount >= (a_rProfile.m_DeallocBytes.m_stCount + a_stSize));
        EMP_ASSERT(a_rProfile.m_stAllocCount >= (a_rProfile.m_stDeallocCount + 1));
        a_rProfile.m_DeallocBytes.m_stCount += a_stSize;
        ++a_rProfile.m_stDeallocCount;
    };

    c_size stRawSize = a_pAlloc->m_stRawSize;

    l_dealloc_check(m_RawProfile, stRawSize);
    l_dealloc_check(m_RequestedProfile, a_pAlloc->m_stReqSize);

#ifdef EMP_XX_TRACE_LEAKS
    EMP_ASSERT(a_pAlloc->m_pAllocator == this);

    if (a_pAlloc->m_pPrevious)
    { a_pAlloc->m_pPrevious->m_pNext = a_pAlloc->m_pNext; }
    if (a_pAlloc->m_pNext)
    { a_pAlloc->m_pNext->m_pPrevious = a_pAlloc->m_pPrevious; }
    else
    {
        EMP_ASSERT(a_pAlloc == m_pLeaks);
        m_pLeaks = a_pAlloc->m_pPrevious;
    }
#endif

    void* pMemory = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(a_pAlloc) - a_pAlloc->m_stOffset);

#ifdef EMP_XX_ASSERT
    bool bFeed = true;
    ui8* const pTemp = static_cast<ui8*>(pMemory);
    for (size_t stIndex = 0; stIndex < stRawSize; ++stIndex)
    {
        if (pTemp[stIndex] != 0xFE)
        {
            pTemp[stIndex] = 0xFE;
            bFeed = false;
        }
    }
    EMP_ASSERT(bFeed == false);
#endif
    dealloc(pMemory);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* allocator_c::alloc(c_size a_stSize)
{ return malloc(a_stSize); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void allocator_c::dealloc(void* a_pAddress)
{ free(a_pAddress); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
