//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_LITERAL_H
#define EMP_TT_LITERAL_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_assert.h"
#include "emp_tt_limit.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class aliteral
{
public:
    char const* m_szLiteral = nullptr;
    size_t m_stSize = 0;
    size_t m_stLen = 0;

public:
    explicit constexpr aliteral(void):
        m_szLiteral(nullptr),
        m_stSize(0),
        m_stLen(0)
    {}

    template <size_t t_stSize>
    explicit constexpr EMP_INLINE aliteral(char const (&a_szLiteral)[t_stSize], c_size a_stLen) :
        m_szLiteral(a_szLiteral),
        m_stSize(t_stSize),
        m_stLen(a_stLen)
    {
        static_assert(t_stSize >= 1, "");
        const_assert(a_stLen < t_stSize);
        const_assert(aliteral::len(a_szLiteral) == a_stLen);
        //const_assert(a_stLen < t_stSize);
        //const_assert(literal_t::len(a_szLiteral) == a_stLen);
    }

    constexpr EMP_INLINE EMP_RETURN char operator[](c_size a_stIndex) const
    {
#if defined EMP_XX_CPP_11
#else
        const_assert(a_stIndex < m_stSize);
#endif
        return m_szLiteral[a_stIndex];
    }

    constexpr EMP_INLINE EMP_RETURN size_t size(void) const { return m_stSize; }
    constexpr EMP_INLINE EMP_RETURN size_t len(void) const { return m_stLen; }
    constexpr EMP_INLINE EMP_RETURN char const* cstr(void) const { return m_szLiteral; }

public:
#if defined EMP_XX_CPP_11
    template <size_t t_stSize>
    static constexpr EMP_RETURN size_t len_impl(char const(&a_szLiteral)[t_stSize], c_size a_stLen, c_size a_stCount)
    {
        const_assert(a_stLen < t_stSize);
        return a_szLiteral[a_stLen] != '\0' ? 1 + (a_stCount < 1 ? 0 : len_impl<t_stSize>(a_szLiteral, a_stLen + 1, a_stCount - 1)) : 0;
    }

    //template <size_t t_stSize>
    //struct len_impl<t_stSize, 1>
    //{
    //    static constexpr EMP_INLINE EMP_RETURN size_t apply(char const (&a_szLiteral)[t_stSize])
    //    {
    //        return a_szLiteral[t_st]
    //    }
    //};
    template <size_t t_stSize>
    static constexpr EMP_INLINE EMP_RETURN size_t len(char const (&a_szLiteral)[t_stSize])
    { return len_impl<t_stSize>(a_szLiteral, 0, t_stSize); }

#else
    template <size_t t_stSize>
    static constexpr EMP_INLINE EMP_RETURN size_t len(char const (&a_szLiteral)[t_stSize] )
    {
        size_t stLen = 0;
        const_assert(stLen < t_stSize);
        while (a_szLiteral[stLen] != '\0')
        {
            const_assert(stLen < t_stSize);
            ++stLen;
        }
        return stLen;
    }
#endif
};

EMP_TYPEDEF(aliteral)
#define ALITERAL(x_Cstr) emp::tt::aliteral(x_Cstr, emp::tt::integral<size_t, emp::tt::aliteral::len(x_Cstr)>::value)

namespace {

static_assert(aliteral::len("abc") == 3, "");
static_assert(ALITERAL("abs").len() == 3, "");
constexpr c_char toto = ALITERAL("abs")[2];

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class u8literal
{
public:
    char const* m_szLiteral = nullptr;
    size_t m_stSize = 0;
    size_t m_stLen = 0;
    size_t m_stByteLen = 0;

public:
    u8literal(void):
        m_szLiteral(nullptr),
        m_stSize(0),
        m_stLen(0),
        m_stByteLen(0)
    {}

    template <size_t t_stSize>
    explicit constexpr EMP_INLINE u8literal(char const (&a_szLiteral)[t_stSize], c_size a_stLen, c_size a_stByteLen) :
        m_szLiteral(a_szLiteral), // @@0 ascii test
        m_stSize(t_stSize),
        m_stLen(a_stLen),
        m_stByteLen(a_stByteLen)
    {
        static_assert(t_stSize >= 1, "");
        const_assert(a_stLen < t_stSize);
        const_assert(u8literal::len(a_szLiteral) == a_stLen);
        const_assert(u8literal::byte_len(a_szLiteral) == a_stByteLen);
        //const_assert(a_stLen < t_stSize);
        //const_assert(literal_t::len(a_szLiteral) == a_stLen);
    }

    constexpr EMP_INLINE EMP_RETURN char operator[](c_size a_stIndex) const
    {
#if defined EMP_XX_CPP_11
#else
        const_assert(a_stIndex < m_stSize);
#endif
        return m_szLiteral[a_stIndex];
    }

    constexpr EMP_INLINE EMP_RETURN size_t size(void) const { return m_stSize; }
    constexpr EMP_INLINE EMP_RETURN size_t len(void) const { return m_stLen; }
    constexpr EMP_INLINE EMP_RETURN size_t byte_len(void) const { return m_stByteLen; }
    constexpr EMP_INLINE EMP_RETURN char const* cstr(void) const { return m_szLiteral; }

public:
    template <size_t t_stSize>
    static constexpr EMP_RETURN size_t byte_len_impl(char const (&a_szLiteral)[t_stSize], c_size a_stByteLen)
    {
        const_assert(a_stByteLen < t_stSize);
        return a_szLiteral[a_stByteLen] != '\0' ? byte_len_impl(a_szLiteral, a_stByteLen + 1) : a_stByteLen;
    }

    template <size_t t_stSize>
    static constexpr EMP_INLINE EMP_RETURN size_t byte_len(char const (&a_szLiteral)[t_stSize])
    {
        return byte_len_impl(a_szLiteral, 0);
        /*size_t stByteLen = 0;
        const_assert(stByteLen < t_stSize);
        while (a_szLiteral[stByteLen] != '\0')
        {
            const_assert(stByteLen < t_stSize);
            ++stByteLen;
        }
        return stByteLen;*/
    }

    template <size_t t_stSize>
    static constexpr EMP_RETURN size_t len_impl(char const (&a_szLiteral)[t_stSize], c_size a_stByteLen, c_size a_stLen)
    {
        const_assert(a_stByteLen < t_stSize);
        return (a_szLiteral[a_stByteLen] == 0) ? 
            a_stLen :
            (a_szLiteral[a_stByteLen] & 0xC0) != 0x80 ?
                len_impl(a_szLiteral, a_stByteLen + 1, a_stLen + 1) :
                len_impl(a_szLiteral, a_stByteLen + 1, a_stLen);
    }

    template <size_t t_stSize>
    static constexpr EMP_INLINE EMP_RETURN size_t len(char const (&a_szLiteral)[t_stSize])
    {
        return len_impl(a_szLiteral, 0, 0);
        /*size_t stLen = 0;
        size_t stByteLen = 0;
        const_assert(stByteLen < t_stSize);
        while (true)
        {
            const_assert(stByteLen < t_stSize);
            c_char cChar = a_szLiteral[stByteLen];
            if (cChar == 0)
            { break; }
            if ((cChar & 0xC0) != 0x80)
            { ++stLen; }
            ++stByteLen;
        }
        return stLen;*/
    }
};

EMP_TYPEDEF(u8literal)
#define U8LITERAL(x_Cstr) emp::tt::u8literal(x_Cstr,\
    emp::tt::integral<size_t, emp::tt::u8literal::len(x_Cstr)>::value,\
    emp::tt::integral<size_t, emp::tt::u8literal::byte_len(x_Cstr)>::value)

namespace {

// @@0 static_assert

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, size_t t_stArgs, size_t t_stExpected>
struct format_validator_t
{
    static_assert(t_stArgs == t_stExpected, "invalid format");
    typedef T type;
};

template <typename T, size_t t_stArgs, size_t t_stExpected>
using format_validator = typename format_validator_t<T, t_stArgs, t_stExpected>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_COMPILER_MSC
#define XFORMAT(x_Format, x_Literal, x_Cstr, x_Count) emp::tt::format_validator\
    <x_Format, x_Count, x_Count>(x_Literal(x_Cstr), x_Count)
#warning unchecked format.
#else
#define XFORMAT(x_Format, x_Literal, x_Cstr, x_Count) emp::tt::format_validator\
    <x_Format, x_Count, x_Format::args(x_Literal(x_Cstr))>(x_Literal(x_Cstr), x_Count)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
class format_t
{
public:
    T m_Literal;
    size_t m_stArgs;

public:
    explicit constexpr format_t(void):
        m_Literal(),
        m_stArgs(0)
    {}

    explicit constexpr EMP_INLINE format_t(T const& a_Literal, c_size a_stArgs) :
        m_Literal(a_Literal),
        m_stArgs(a_stArgs)
    {
#ifndef EMP_XX_COMPILER_MSC
        const_assert(args(a_Literal) == m_stArgs);
#endif
    }

public:
    constexpr EMP_RETURN size_t args(void) const
    { return m_stArgs; }

    constexpr EMP_RETURN T const& literal(void) const
    { return m_Literal; }

public:
#if defined EMP_XX_CPP_11

    static constexpr EMP_INLINE EMP_RETURN size_t arg_stub(T const& a_Literal, c_size a_stIndex, c_size a_stCount, c_ui64 a_uiArg, c_size a_stOffset)
    { return a_stIndex < a_Literal.len() ? (a_stOffset % 2 == 1 ? arg_index(a_Literal, a_stIndex + a_stOffset, a_stCount, a_uiArg) : args(a_Literal, a_stIndex +(a_stOffset > 0 ? a_stOffset : 1))) : 0; }

    static constexpr EMP_INLINE EMP_RETURN size_t args(T const& a_Literal, c_size a_stIndex, c_size a_stCount = 0)
    { return a_stIndex < a_Literal.len() ? arg_stub(a_Literal, a_stIndex, a_stCount, 0, is_arg(a_Literal, a_stIndex)) : 0; }

    static constexpr EMP_INLINE EMP_RETURN bool is_arg(T const& a_Literal, c_size a_stIndex)
    { return a_stIndex < a_Literal.len() ? (a_Literal[a_stIndex] == '#' ? 1 + is_arg(a_Literal, a_stIndex + 1) : 0) : 0; }

    static constexpr EMP_INLINE EMP_RETURN size_t arg_index(T const& a_Literal, c_size a_stIndex, c_size a_stCount, c_ui64 a_uiArg)
    { return a_stIndex < a_Literal.len() ?
            ((a_Literal[a_stIndex] >= '0' && a_Literal[a_stIndex] <= '9') ?
                arg_index(a_Literal, a_stIndex, a_stCount, a_uiArg * 10 + a_Literal[a_stIndex] - '0') :
                args(a_Literal, a_stIndex, a_stCount + 1) + (a_stCount == a_uiArg ? 1 : 0)) : 0;
    }
#else
#ifndef EMP_XX_COMPILER_MSC
    static constexpr EMP_INLINE EMP_RETURN size_t args(T const& a_Literal)
    {
        size_t stRank = 0;
        size_t stSharp = 0;
        c_size stLen = a_Literal.len();
        size_t stIndex = 0;
        while (stIndex < stLen)
        {
            auto const& cChar = a_Literal[stIndex];
            if (cChar == '#')
            {
                ++stSharp;
                ++stIndex;
            }
            else
            {
                c_bool isArg = (stSharp % 2) == 1;
                stSharp = 0;
                if (isArg)
                {
                    size_t EMP_TT_MAX_VAR(stValue);
                    while (stIndex < stLen)
                    {
                        auto const& cNum = a_Literal[stIndex];
                        if (cNum >= '0' && cNum <= '9')
                        {
                            if (emp::tt::is_max(stValue))
                            { stValue = 0; }
                            stValue = stValue * 10 + static_cast<size_t>(cNum - '0');
                            ++stIndex;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (emp::tt::is_max(stValue) ||
                        stValue != stRank)
                    { return 0; }
                    else
                    { ++stRank; }
                }
                else
                {
                    ++stIndex;
                }
            }
        }
        return stRank;
    }
#endif
#endif
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using aformat_c = format_t<aliteral>;
EMP_TYPEDEF(aformat_c)
#define AFORMAT(x_Cstr, x_Count) XFORMAT(emp::tt::aformat_c, ALITERAL, x_Cstr, x_Count)

using u8format_c = format_t<u8literal>;
EMP_TYPEDEF(u8format_c)
#define U8FORMAT(x_Cstr, x_Count) XFORMAT(emp::tt::u8format_c, U8LITERAL, x_Cstr, x_Count)

// @@0 static_assert

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

