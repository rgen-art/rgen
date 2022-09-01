//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_STR_STRING_H
#define EMP_STR_STRING_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//@@0 bug if non formated log contains '#' char eg : EMP_LOG_ERROR(szLog)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_xx_string.h"

#include "emp_cnt_array.h"
#include "emp_cnt_container.h"
#include "emp_cnt_shell.h"

#include "emp_mat_algebra.h"

#include "emp_mem_immutable.h"
#include "emp_mem_util.h"

#include "emp_tt_logical.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace str {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_STR_CONSTANT(x_Name, x_Value)\
template <typename T>\
EMP_NOINSTANCE(struct, x_Name)\
    static const T value = x_Value;\
};\
template <typename T>\
EMP_RETURN bool EMP_PP_CAT(is_, x_Name)(T const a_tValue)\
{ return x_Name<T>::value == a_tValue; }\
template <typename T>\
EMP_RETURN bool EMP_PP_CAT(is_not_, x_Name)(T const a_tValue)\
{ return EMP_PP_CAT(is_, x_Name)(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTMP_STR_CONSTANT(null              , EMP_XC_NULL)
XTMP_STR_CONSTANT(newline           , EMP_XC_NEWLINE)
XTMP_STR_CONSTANT(tabulation        , EMP_XC_TABULATION)
XTMP_STR_CONSTANT(space             , EMP_XC_SPACE)
XTMP_STR_CONSTANT(not_              , EMP_XC_NOT)
XTMP_STR_CONSTANT(double_quote      , EMP_XC_DOUBLE_QUOTE)
XTMP_STR_CONSTANT(sharp             , EMP_XC_SHARP)
XTMP_STR_CONSTANT(dollar            , EMP_XC_DOLLAR)
XTMP_STR_CONSTANT(mod               , EMP_XC_MOD)
XTMP_STR_CONSTANT(and_              , EMP_XC_AND)
XTMP_STR_CONSTANT(quote             , EMP_XC_QUOTE)
XTMP_STR_CONSTANT(left_parenthesis  , EMP_XC_LEFT_PARENTHESIS)
XTMP_STR_CONSTANT(right_parenthesis , EMP_XC_RIGHT_PARENTHESIS)
XTMP_STR_CONSTANT(mul               , EMP_XC_MUL)
XTMP_STR_CONSTANT(add               , EMP_XC_ADD)
XTMP_STR_CONSTANT(comma             , EMP_XC_COMMA)
XTMP_STR_CONSTANT(sub               , EMP_XC_SUB)
XTMP_STR_CONSTANT(dot               , EMP_XC_DOT)
XTMP_STR_CONSTANT(div               , EMP_XC_DIV)
XTMP_STR_CONSTANT(colon             , EMP_XC_COLON)
XTMP_STR_CONSTANT(semi_colon        , EMP_XC_SEMI_COLON)
XTMP_STR_CONSTANT(less              , EMP_XC_LESS)
XTMP_STR_CONSTANT(equal             , EMP_XC_EQUAL)
XTMP_STR_CONSTANT(more              , EMP_XC_MORE)
XTMP_STR_CONSTANT(question          , EMP_XC_QUESTION)
XTMP_STR_CONSTANT(at                , EMP_XC_AT)
XTMP_STR_CONSTANT(left_bracket      , EMP_XC_LEFT_BRACKET)
XTMP_STR_CONSTANT(back_slash        , EMP_XC_BACK_SLASH)
XTMP_STR_CONSTANT(right_bracket     , EMP_XC_RIGHT_BRACKET)
XTMP_STR_CONSTANT(xor_              , EMP_XC_XOR)
XTMP_STR_CONSTANT(underscore        , EMP_XC_UNDERSCORE)
XTMP_STR_CONSTANT(back_quote        , EMP_XC_BACK_QUOTE)
XTMP_STR_CONSTANT(left_brace        , EMP_XC_LEFT_BRACE)
XTMP_STR_CONSTANT(or_               , EMP_XC_OR)
XTMP_STR_CONSTANT(right_brace       , EMP_XC_RIGHT_BRACE)
XTMP_STR_CONSTANT(tilde             , EMP_XC_TILDE)

XTMP_STR_CONSTANT(lower_alpha_min   , EMP_XC_lower_a)
XTMP_STR_CONSTANT(lower_alpha_max   , EMP_XC_lower_z)
XTMP_STR_CONSTANT(upper_alpha_min   , EMP_XC_UPPER_A)
XTMP_STR_CONSTANT(upper_alpha_max   , EMP_XC_UPPER_Z)

XTMP_STR_CONSTANT(lower_hexa_min    , EMP_XC_lower_a)
XTMP_STR_CONSTANT(lower_hexa_max    , EMP_XC_lower_f)
XTMP_STR_CONSTANT(upper_hexa_min    , EMP_XC_UPPER_A)
XTMP_STR_CONSTANT(upper_hexa_max    , EMP_XC_UPPER_F)

XTMP_STR_CONSTANT(numeric_min       , EMP_XC_0)
XTMP_STR_CONSTANT(numeric_max       , EMP_XC_9)

XTMP_STR_CONSTANT(zero              , EMP_XC_0)
XTMP_STR_CONSTANT(one               , EMP_XC_1)
XTMP_STR_CONSTANT(two               , EMP_XC_2)
XTMP_STR_CONSTANT(three             , EMP_XC_3)
XTMP_STR_CONSTANT(four              , EMP_XC_4)
XTMP_STR_CONSTANT(five              , EMP_XC_5)
XTMP_STR_CONSTANT(six               , EMP_XC_6)
XTMP_STR_CONSTANT(sever             , EMP_XC_7)
XTMP_STR_CONSTANT(eight             , EMP_XC_8)
XTMP_STR_CONSTANT(nine              , EMP_XC_9)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef XTMP_STR_CONSTANT

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_inside(T const a_tValue, T const a_tMin, T const a_tMax)
{ return  a_tValue >= a_tMin && a_tValue <= a_tMax; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_lower_alpha(T const a_tValue)
{ return is_inside(a_tValue, lower_alpha_min<T>::value, lower_alpha_max<T>::value); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_upper_alpha(T const a_tValue)
{ return is_inside(a_tValue, upper_alpha_min<T>::value, upper_alpha_max<T>::value); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_lower_hexa(T const a_tValue)
{ return is_inside(a_tValue, lower_hexa_min<T>::value, lower_hexa_max<T>::value); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_upper_hexa(T const a_tValue)
{ return is_inside(a_tValue, upper_hexa_min<T>::value, upper_hexa_max<T>::value); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_hexa(T const a_tValue)
{ return is_lower_hexa(a_tValue) || is_upper_hexa(a_tValue); }

template <typename T>
EMP_INLINE EMP_RETURN bool is_not_hexa(T const a_tValue)
{ return is_hexa(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_alpha(T const a_tValue)
{ return  is_lower_alpha(a_tValue) || is_upper_alpha(a_tValue); }

template <typename T>
EMP_INLINE EMP_RETURN bool is_not_alpha(T const a_tValue)
{ return is_alpha(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_numeric(T const a_tValue)
{ return is_inside(a_tValue, numeric_min<T>::value, numeric_max<T>::value); }

template <typename T>
EMP_INLINE EMP_RETURN bool is_not_numeric(T const a_tValue)
{ return is_numeric(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_whitespace(T const a_tValue)
{ return is_space(a_tValue) || is_tabulation(a_tValue); }

template <typename T>
EMP_INLINE EMP_RETURN bool is_not_whitespace(T const a_tValue)
{ return is_whitespace(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INLINE EMP_RETURN bool is_operator(T const a_tValue)
{
    return
        is_not_(a_tValue) ||
        is_double_quote(a_tValue) ||
        is_sharp(a_tValue) ||
        is_dollar(a_tValue) ||
        is_mod(a_tValue) ||
        is_and_(a_tValue) ||
        is_quote(a_tValue) ||
        is_left_parenthesis(a_tValue) ||
        is_right_parenthesis(a_tValue) ||
        is_mul(a_tValue) ||
        is_add(a_tValue) ||
        is_comma(a_tValue) ||
        is_sub(a_tValue) ||
        is_dot(a_tValue) ||
        is_div(a_tValue) ||
        is_colon(a_tValue) ||
        is_semi_colon(a_tValue) ||
        is_less(a_tValue) ||
        is_equal(a_tValue) ||
        is_more(a_tValue) ||
        is_question(a_tValue) ||
        is_at(a_tValue) ||
        is_left_bracket(a_tValue) ||
        is_back_slash(a_tValue) ||
        is_right_bracket(a_tValue) ||
        is_xor_(a_tValue) ||
        is_underscore(a_tValue) ||
        is_back_quote(a_tValue) ||
        is_left_brace(a_tValue) ||
        is_or_(a_tValue) ||
        is_right_brace(a_tValue) ||
        is_tilde(a_tValue);
}

template <typename T>
EMP_INLINE EMP_RETURN bool is_not_operator(T const a_tValue)
{ return is_not_operator(a_tValue) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name base_e
#define enum_type
#define enum_values\
    enum_value( Bin )\
    enum_value( Oct )\
    enum_value( Dec )\
    enum_value( Hex )
    #include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct u8string;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(astring)
public:
    friend struct u8string;
    typedef char tchar;
    typedef emp::tt::aliteral tliteral;


public:
    emp::cnt::container_t<char, emp::mem::immutable_t<char>> m_Container{};
    size_t m_stLen = 0;

public:
    ~astring(void) {}

    explicit EMP_INLINE astring(c_char a_cFrom):
        m_Container(),
        m_stLen(0)
    { append_char(a_cFrom); }

    /*template <size_t t_stSize>
    constexpr EMP_INLINE astring(c_char(&a_szFrom)[t_stSize]):
        m_Container(emp::mem::bulk_t<char const>(a_szFrom, t_stSize, t_stSize)),
        m_stLen(ALITERAL(a_szFrom).len())
    {
        //static_assert(aliteral::len(a_szFrom) > 0, "");
    }*/

    EMP_INLINE astring(tt::c_aliteral& a_sFrom):
        m_Container(emp::mem::bulk_t<char const>(a_sFrom.cstr(), a_sFrom.size(), true)),
        m_stLen(a_sFrom.len())
    {}

    EMP_INLINE astring(tt::c_aformat_c& a_sFrom):
        m_Container(emp::mem::bulk_t<char const>(a_sFrom.literal().cstr(), a_sFrom.literal().size(), true)),
        m_stLen(a_sFrom.literal().len())
    {}

private:
    ENABLE_IF_T(emp::tt::equal<T, emp::tt::true_>::value)
    explicit EMP_INLINE astring(cpc_char a_szFrom, T const* const):
        m_Container(),
        m_stLen(0)
    { append_cstr(a_szFrom); }

    ENABLE_IF_T(emp::tt::equal<T, emp::tt::false_>::value)
    explicit EMP_INLINE astring(cpc_char a_szFrom, T const* const):
        m_Container(emp::mem::bulk_t<c_char>(a_szFrom, a_szFrom == nullptr ? 0 : emp::mem::len(a_szFrom) + 1, true)),
        m_stLen(emp::tt::maxof<size_t>(1, m_Container.count()) - 1) // @@0 bulletproof ?
    { }

public:
    static EMP_INLINE EMP_RETURN astring dup(cpc_char a_szSource)
    { return astring(a_szSource, static_cast<emp::tt::true_*>(nullptr)); }

    static EMP_INLINE EMP_RETURN astring ref(cpc_char a_szSource)
    { return astring(a_szSource, static_cast<emp::tt::false_*>(nullptr)); }

    static EMP_INLINE EMP_RETURN bool from_u8string(astring& a_rsDest, u8string const& a_u8Source);

private:

    explicit EMP_INLINE astring(cpc_char a_szFrom, c_size a_stSize, c_size a_stLen):
        m_Container(emp::mem::bulk_t<char const>(a_szFrom, a_stSize, true)),
        m_stLen(a_stLen)
    { EMP_ASSERT((a_szFrom == nullptr && a_stLen == 0) || (a_szFrom != nullptr && (emp::mem::len(a_szFrom) == a_stLen))); }

    EMP_RETURN bool share(char* const a_szFrom, c_size a_stSize, c_size a_stLen, size_t* const a_pstReferenceCount)
    {
        if (m_Container.share(a_szFrom, a_stSize, a_pstReferenceCount) == false)
        { return false; }
        m_stLen = a_stLen;
        return true;
    }

    /* @@0 remove explicit EMP_INLINE astring(char* const a_szFrom, c_size a_stSize, c_size a_stLen):
        m_Container(emp::mem::bulk_t<char>(a_szFrom, a_stSize, true)),
        m_stLen(a_stLen)
    { EMP_ASSERT((a_szFrom == nullptr && a_stLen == 0) || (a_szFrom != nullptr && (emp::mem::len(a_szFrom) == a_stLen))); }*/

public:
    EMP_INLINE EMP_RETURN size_t len(void) const { return m_stLen; }
    EMP_INLINE EMP_RETURN size_t byte_len(void) const { return m_stLen; }

    //EMP_INLINE EMP_RETURN size_t count(void) const { return m_Container.count(); } // @@0 remove

public:
    EMP_INLINE EMP_RETURN bool is_empty(void) const { return len() == 0; }
    EMP_INLINE EMP_RETURN bool is_cstr(void) const { return m_stLen > 0 && m_Container.last_val() == null<char>::value; }
    EMP_INLINE EMP_RETURN c_char* cstr(void)
    {
        if (is_cstr() == false)
        {
            append_char(null<char>::value);
            return m_Container.first_ptr();
        }
        return m_Container.first_const_ptr();
    }

public:
    EMP_INLINE EMP_RETURN char get_char(c_size a_stIndex) const { return m_Container.val(a_stIndex); }
    EMP_INLINE EMP_RETURN char get_first_char(void) const { return m_Container.first_val(); }
    EMP_INLINE EMP_RETURN char get_last_char(void) const
    {
        EMP_ASSERT(m_stLen > 0);
        return m_Container.val(m_stLen - 1);
    }

    EMP_INLINE EMP_RETURN char* get_first_char_ptr(void) { return m_Container.first_ptr(); }

public:
    void clear(void)
    {
        m_Container.clear();
        m_stLen = 0;
    }

public:
    void append_char(c_char a_cChar)
    {
        c_size stNewLen = m_stLen + (a_cChar == null<char>::value ? 0 : 1);
        c_size stNewCount = stNewLen + 1;
        m_Container.resize(stNewCount);
        m_Container.set_last(null<char>::value);
        m_Container.set(m_stLen, a_cChar);
        m_stLen = stNewLen;
    }

    /*template <size_t t_stSize>
    void append_literal(c_char(&a_szFrom)[t_stSize]) //@@0 better ?
    { append_string(astring(a_szFrom)); }*/

    void append_literal(tt::c_aliteral& a_sFrom) //@@0 better ?
    { append_string(astring(a_sFrom)); }

    void append_cstr(cpc_char a_szFrom) //@@0 better ?
    { append_string(astring::ref(a_szFrom)); }

public:
    void append_string(astring const& a_sValue)
    {
        c_size stNewLen = m_stLen + a_sValue.len();
        c_size stNewCount = stNewLen + 1;
        m_Container.resize(stNewCount);
        m_Container.set_last(null<char>::value);
        emp::mem::bulk_copy_from_start(a_sValue.m_Container.bulk<char const>(), m_Container.bulk<char>(m_stLen), a_sValue.len());
        m_stLen = stNewLen;
    }

public:
    EMP_INLINE void set_byte(c_size a_stIndex, c_char a_cChar)
    { set_char(a_stIndex, a_cChar); }

    EMP_INLINE char get_byte(c_size a_stIndex)
    { return get_char(a_stIndex); }

    EMP_INLINE void set_char(c_size a_stIndex, c_char a_cChar)
    {
        c_char cPrevious = get_char(a_stIndex);
        m_Container.resize(m_Container.size());
        m_Container.set(a_stIndex, a_cChar);
        if (is_null(a_cChar))
        { m_stLen = emp::tt::minof(m_stLen, a_stIndex); }
        else if (is_null(cPrevious) &&
            a_stIndex < m_stLen)
        { m_stLen = a_stIndex + emp::mem::nlen(m_Container.first_ptr() + a_stIndex, m_Container.count() - a_stIndex); }
    }

    EMP_INLINE void insert(c_size a_stIndex, c_char a_cChar)
    {
        m_Container.insert(a_stIndex, a_cChar);
        if (is_null(a_cChar))
        { m_stLen = emp::tt::minof(m_stLen, a_stIndex); }
        else if (a_stIndex < m_stLen)
        { ++m_stLen; }
    }

    void remove(c_size a_stIndex)
    {
        c_char cChar = m_Container.val(a_stIndex);
        m_Container.remove(a_stIndex);
        if (is_null(cChar))
        { m_stLen = emp::tt::minof(m_stLen, a_stIndex); }
        else if (a_stIndex < m_stLen && m_stLen > 0) //-V560
        { --m_stLen; }
    }

    EMP_INLINE void resize(c_size a_stSize)
    {
        m_Container.resize(a_stSize);
        m_stLen = emp::mem::nlen(m_Container.first_ptr(), a_stSize);
    }

public:
    EMP_INLINE void reverse_bytes(c_size a_stIndex = 0)
    {
        EMP_ASSERT(a_stIndex <= m_stLen);
        c_size stLen = m_stLen - a_stIndex;
        c_size stHalfLen = stLen / 2;
        for (size_t stIndex = 0; stIndex < stHalfLen; ++stIndex)
        {
            c_size stOffset = a_stIndex + stIndex;
            c_size stLast = m_stLen - stIndex - 1;
            c_char cLast = get_char(stLast);
            EMP_ASSERT(cLast != 0);
            set_char(stLast, get_char(stOffset));
            set_char(stOffset, cLast);
        }
    }

public:
    EMP_INLINE void trim_left(c_size a_stCount = 1)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stCount <= m_stLen);
        m_Container.trim_left(a_stCount);
        m_stLen -= a_stCount;
    }

    EMP_INLINE void trim_right(c_size a_stCount = 1)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stCount <= m_stLen);
        c_size stTrimCount = a_stCount + (is_cstr() ? 1 : 0);
        m_Container.trim_right(stTrimCount);
        m_stLen -= a_stCount;
        if (m_Container.is_owner())
        { append_char(null<char>::value); }
    }

    EMP_INLINE void trim_index(c_size a_stLeft, c_size a_stRight)
    {
        trim_left(a_stLeft);
        trim_right(a_stRight);
    }

    EMP_INLINE void trim_count(c_size a_stLeft, c_size a_stCount)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stCount <= m_stLen);
        EMP_ASSERT(a_stLeft + a_stCount <= m_stLen);
        EMP_ASSERT(a_stLeft + a_stCount <= m_Container.count());
        EMP_ASSERT(a_stLeft + a_stCount <= m_Container.size());

        trim_left(a_stLeft);
        trim_right(m_stLen -  a_stCount);
    }
};
EMP_TYPEDEF(astring)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename Dummy = void>
EMP_NOINSTANCE(struct, astring_data)
public:
    static enum_array_of(astring, base_e) s_asBases;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename Dummy = void>
EMP_NOINSTANCE(struct, u8string_data)
public:
    static enum_array_of((emp::cnt::array<ui8, 16, true>), base_e) s_asBases;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, ascii) static constexpr T const value = 0x80; };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_INLINE EMP_RETURN bool is_ascii(c_ui32 a_ui32Code);
EMP_INLINE EMP_RETURN bool is_ascii(c_ui32 a_ui32Code)
{ return a_ui32Code < ascii<c_ui32>::value; }

// @@0 factor
EMP_INLINE EMP_RETURN bool is_ascii(c_ui8 a_ui8Code);
EMP_INLINE EMP_RETURN bool is_ascii(c_ui8 a_ui8Code)
{ return a_ui8Code < ascii<c_ui8>::value; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_in_range(c_ui32 c, c_ui32 lo, c_ui32 hi);
EMP_INLINE EMP_RETURN bool is_in_range(c_ui32 c, c_ui32 lo, c_ui32 hi)
{ return (c - lo) < (hi - lo + 1); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_surrogate(c_ui32 c);
EMP_INLINE EMP_RETURN bool is_surrogate(c_ui32 c)
{ return is_in_range(c, 0xd800, 0xdfff); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_non_character(c_ui32 c);
EMP_INLINE EMP_RETURN bool is_non_character(c_ui32 c)
{ return is_in_range( c, 0xfdd0, 0xfdef); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_reserved(c_ui32 c);
EMP_INLINE EMP_RETURN bool is_reserved(c_ui32 c)
{ return (c & 0xfffe) == 0xfffe; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_out_of_range(c_ui32 c);
EMP_INLINE EMP_RETURN bool is_out_of_range(c_ui32 c)
{ return c > 0x10ffff; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_not_valid_utf32(c_ui32 a_ui32Code);
EMP_INLINE EMP_RETURN bool is_not_valid_utf32(c_ui32 a_ui32Code)
{
    return  is_surrogate(a_ui32Code) || is_non_character(a_ui32Code) ||
            is_reserved(a_ui32Code) || is_out_of_range(a_ui32Code);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool is_valid_utf32(c_ui32 a_ui32Code);
EMP_INLINE EMP_RETURN bool is_valid_utf32(c_ui32 a_ui32Code)
{ return is_not_valid_utf32(a_ui32Code) == false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(u8string)
public:
    typedef ui64 tchar;
    typedef emp::tt::u8literal tliteral;

public:
    emp::cnt::container_t<ui8, emp::mem::immutable_t<ui8>> m_Container{};
    size_t m_stLen = 0;
    size_t m_stByteLen = 0;
    bool m_bAscii = true;


public:
    explicit EMP_INLINE u8string(c_astring& a_sFrom):
        m_Container(emp::mem::bulk_t<c_ui8>(reinterpret_cast<c_ui8*>(a_sFrom.m_Container.size() > 0 ? a_sFrom.m_Container.first_const_ptr() : nullptr), a_sFrom.m_Container.size(), true)),
        m_stLen(a_sFrom.len()),
        m_stByteLen(a_sFrom.len()),
        m_bAscii(true)
    {
        static_assert(((char)-1) == ((char)~0), ""); // @@0 two' complement ??
    }

public:
    EMP_INLINE u8string(tt::c_u8literal& a_sFrom):
        m_Container(emp::mem::bulk_t<c_ui8>(reinterpret_cast<c_ui8*>(a_sFrom.cstr()), a_sFrom.size(), true)),
        m_stLen(a_sFrom.len()),
        m_stByteLen(a_sFrom.byte_len()),
        m_bAscii(false) // @@0 m_bAscii(a_sFrom.ascii())
    {
        static_assert(((char)-1) == ((char)~0), ""); // @@0 two' complement ??
    }

    EMP_INLINE u8string(tt::c_u8format_c& a_sFrom):
        m_Container(emp::mem::bulk_t<c_ui8>(reinterpret_cast<c_ui8*>(a_sFrom.literal().cstr()), a_sFrom.literal().size(), true)),
        m_stLen(a_sFrom.literal().len()),
        m_stByteLen(a_sFrom.literal().byte_len()),
        m_bAscii(false) // @@0 m_bAscii(a_sFrom.ascii())
    {
        static_assert(((char)-1) == ((char)~0), ""); // @@0 two' complement ??
    }
    
public:
    //static EMP_INLINE EMP_RETURN u8string dup(cpc_ui8 a_ui8Source)
    //{ return u8string(a_ui8Source, static_cast<emp::tt::true_*>(nullptr)); }

    static EMP_INLINE EMP_RETURN bool dup(cpc_char a_szSource, u8string& a_rsOut)
    {
        static_assert(((char)-1) == ((char)~0), "");
        u8string sTmp;
        if (u8string::ref(reinterpret_cast<c_ui8*>(a_szSource), sTmp) == false)
        { return false; }
        a_rsOut.append_string(sTmp);  // @@0 optim
        return true;
    }

    static EMP_INLINE EMP_RETURN bool ref(cpc_ui8 a_ui8Source, u8string& a_rsOut)
    {
        if (len(a_ui8Source, a_rsOut.m_stLen, a_rsOut.m_bAscii, a_rsOut.m_stByteLen) == false)
        { return false; }
        a_rsOut.m_Container = emp::cnt::container_t<ui8, emp::mem::immutable_t<ui8>>(
            emp::mem::bulk_t<c_ui8>(a_ui8Source, a_rsOut.m_stByteLen + 1, true));
        return true;
    }

    static EMP_INLINE EMP_RETURN bool own(ui8* const a_ui8Source, u8string& a_rsOut)
    {
        if (len(a_ui8Source, a_rsOut.m_stLen, a_rsOut.m_bAscii, a_rsOut.m_stByteLen) == false)
        { return false; }
        a_rsOut.m_Container = emp::cnt::container_t<ui8, emp::mem::immutable_t<ui8>>(
            emp::mem::bulk_t<ui8>(a_ui8Source, a_rsOut.m_stByteLen + 1, true));
        return true;
    }

public:
    EMP_INLINE void set_byte(c_size a_stIndex, c_ui8 a_ui8Char)
    { m_Container.set(a_stIndex, a_ui8Char); }

    EMP_INLINE ui8 get_byte(c_size a_stIndex)
    { return m_Container.val(a_stIndex); }

    //EMP_INLINE EMP_RETURN ui32 get_char(c_size EMP_SILENT(a_stIndex)) const
    //{ EMP_ASSERT_NOT_IMPLEMENTED(); return 0; }


    EMP_INLINE EMP_RETURN ui64 get_first_char(void) const
    {
        if ((m_stLen > 0) == false || (m_stByteLen > 0) == false || (m_Container.count() > 0) == false)
        { return 0; }

        size_t stExpectedLen = 0;
        ui64 ui64Code = m_Container.val(0);

        if      ((ui64Code & 0b10000000) == 0b00000000) return ui64Code;
        else if ((ui64Code & 0b11100000) == 0b11000000) stExpectedLen = 2;
        else if ((ui64Code & 0b11110000) == 0b11100000) stExpectedLen = 3;
        else if ((ui64Code & 0b11111000) == 0b11110000) stExpectedLen = 4;
        else if ((ui64Code & 0b11111100) == 0b11111000) stExpectedLen = 5;
        else if ((ui64Code & 0b11111110) == 0b11111100) stExpectedLen = 6;
        else    return 0;

        for (size_t stIndex = 1; stIndex < stExpectedLen; ++stIndex)
        {
            if (m_Container.is_valid_index(stIndex) == false)
            { return 0; }

            c_ui8 ui8Code = m_Container.val(stIndex);
            if ((ui8Code & 0b11000000) != 0b10000000)
            { return 0; }

            ui64Code = ui64Code | emp::ast::cast<ui64>(ui8Code << emp::ast::cast<ui64>(8 * stIndex));
        }

        return ui64Code;
    }

    static EMP_INLINE EMP_RETURN ui32 utf8_to_codepoint(c_ui64 a_ui64Value)
    {
        c_ui8 ui8Code = a_ui64Value & 0xFF;
        if (ui8Code == null<ui8>::value)
        { return 0; }

        if (emp::str::is_ascii(ui8Code))
        { return ui8Code; }

        ui64 ui64Value = a_ui64Value >> 8;

        ui32 ui32Code = ui8Code;

        c_ui32 ui32Length = s_astLengths.val(ui8Code);
        if (ui32Length == 0 || ui32Length > 3)
        { return 0; }

        ui32 ui32Mask = 0;
        for (size_t stIndex = 0; stIndex < ui32Length; ++stIndex)
        {
            c_ui8 ui8Current = ui64Value & 0xFF;
            ui32Code = (ui32Code << 6) + ui8Current;
            ui32Mask = (ui32Mask << 1) | s_astInvalidBits.val(emp::ast::cast<ui32>(ui8Current) >> 6);
            ui64Value >>= 8;
        }

        if (ui32Mask != 0)
        { return 0; }

        utf8_lookup_s const& rLookup = s_astLookups.ref(ui32Length);
        ui32Code -= rLookup.sub;

        if (ui32Code < rLookup.min ||
            is_null(ui32Code) ||
            is_not_valid_utf32(ui32Code))
        { return 0; }

        return ui32Code;
    }

    /*EMP_INLINE EMP_RETURN ui64 get_first_char(void) const
    {
        c_ui8 ui8Code = m_Container.val(0);
        if (ui8Code == null<ui8>::value)
        { return 0; }

        if (emp::str::is_ascii(ui8Code))
        { return ui8Code; }

        ui64 ui64Code = ui8Code;

        c_ui32 ui32Length = s_astLengths.val(ui8Code);
        if (ui32Length == 0 || ui32Length > 3)
        { return 0; }

        ui32 ui32Mask = 0;
        for (size_t stIndex = 0; stIndex < ui32Length; ++stIndex)
        {
            c_ui8 ui8Current = m_Container.val(stIndex + 1);
            ui64Code = (ui64Code << 6) + ui8Current;
            ui32Mask = (ui32Mask << 1) | s_astInvalidBits.val(emp::ast::cast<ui32>(ui8Current) >> 6);
        }

        if (ui32Mask != 0)
        { return 0; }

        utf8_lookup_s const& rLookup = s_astLookups.ref(ui32Length);
        ui64Code -= rLookup.sub;

        if (ui64Code < rLookup.min ||
            is_null(ui64Code) ||
            is_not_valid_utf32(ui64Code))
        { return 0; }

        return ui32Code;
    }*/

    //EMP_INLINE EMP_RETURN ui32 get_last_char(void) const
    //{ EMP_ASSERT_NOT_IMPLEMENTED(); return 0; }

    EMP_INLINE void trim_left(c_size a_stCount = 1)
    {
        size_t stTrim = a_stCount;
        while (stTrim > 0 && m_stLen > 0 && m_stByteLen > 0)
        {
            do
            {
                m_Container.trim_left();
                --m_stByteLen;
                if (m_stByteLen > 0)
                {
                    auto const cChar = m_Container.first_val();
                    auto const cResult = cChar & 0xC0;
                    if (cResult == 0x80)
                    {
                        int x = 0;
                        ++x;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            while (m_stByteLen > 0 && ((m_Container.first_val() & 0xC0) == 0x80));
            --stTrim;
            --m_stLen;
        }

    }

    EMP_INLINE void trim_right(c_size a_stCount = 1)
    {
        if (is_cstr())
        { m_Container.trim_right(); }

        size_t stTrim = a_stCount;
        while (stTrim > 0 && m_stLen > 0 && m_stByteLen > 0)
        {
            do
            {
                m_Container.trim_right();
                --m_stByteLen;
            }
            while (m_stByteLen > 0 && (m_Container.first_val() & 0xC0) == 0x80);
            --stTrim;
            --m_stLen;
        }

        if (m_Container.is_owner())
        { append_char(null<char>::value); }
    }

    EMP_INLINE void trim_count(c_size a_stLeft, c_size a_stCount)
    {
        EMP_ASSERT(a_stCount > 0);
        EMP_ASSERT(a_stCount <= m_stLen);
        EMP_ASSERT(a_stLeft + a_stCount <= m_stLen);
        EMP_ASSERT(a_stLeft + a_stCount <= m_Container.count());
        EMP_ASSERT(a_stLeft + a_stCount <= m_Container.size());

        trim_left(a_stLeft);
        trim_right(m_stLen -  a_stCount);
    }

    EMP_INLINE void reverse_bytes(c_size a_stIndex = 0)
    {
        EMP_ASSERT(byte_len() >= a_stIndex);
        emp::mem::bulk_reverse(m_Container.bulk<ui8>(a_stIndex, byte_len() - a_stIndex));
    }

    EMP_INLINE void slow_insert(c_size EMP_SILENT(a_stIndex), c_ui64 EMP_SILENT(a_cChar))
    {
        m_Container.insert(a_stIndex, (uint8_t)a_cChar);
        ++m_stByteLen;
        ++m_stLen;
    }

    EMP_INLINE void slow_remove(c_size EMP_SILENT(a_stIndex))
    {
        m_Container.remove(a_stIndex);
        --m_stByteLen;
        --m_stLen;
    }

public:
    void append_literal(tt::c_u8literal& EMP_SILENT(a_sFrom)) //@@0 better ?
    { append_string(u8string(a_sFrom)); }

    void append_string(u8string const& a_sValue)
    {
        c_size stNewLen = m_stLen + a_sValue.m_stLen;
        c_size stSourceByteLen = a_sValue.m_stByteLen;
        c_size stNewByteLen = m_stByteLen + stSourceByteLen;
        m_Container.resize(stNewByteLen + 1);
        m_Container.set(stNewByteLen, null<ui8>::value);
        emp::mem::bulk_copy_from_start(a_sValue.m_Container.bulk<c_ui8>(), m_Container.bulk<ui8>(m_stLen), stSourceByteLen);
        m_stLen = stNewLen;
        m_stByteLen = stNewByteLen;
        m_bAscii = m_bAscii && a_sValue.m_bAscii;
    }

    void append_string(c_astring& a_sValue)
    { append_string(u8string(a_sValue)); }

private:

    constexpr static c_size s_stLengthCount = 256;
    static emp::cnt::array<ui8, s_stLengthCount, true>  s_astLengths;

    struct utf8_lookup_s
    {
        ui32 min;
        ui32 sub;
    };

    constexpr static c_size s_stLookupCount = 6;
    static emp::cnt::array<utf8_lookup_s, s_stLookupCount, true> s_astLookups;

    constexpr static c_size s_stInvalidBitCount = 4;
    static emp::cnt::array<ui32, s_stInvalidBitCount, true> s_astInvalidBits;

    constexpr static c_size s_stInvalidOffsetCount = 32;
    static emp::cnt::array<ui32, s_stInvalidOffsetCount, true> s_astInvalidOffsets;

    //static c_size st_invalidbmp_count = 10000);
    //emp::cnt::array<bool, st_invalidbmp_count>  m_aInvalidBmp;

    //static bool IsInvalidBMP(c_ui32 c)
    //{ return m_InvalidBmp.val(c >> 1); }

public:
    void clear(void)
    {
        m_Container.clear();
        m_stLen = 0;
        m_stByteLen = 0;
    }

public:
    static EMP_RETURN bool len(c_ui8* a_pui8Source, size_t& a_rstLen, bool& a_rbAscii, size_t& a_rstSize)
    {
        if (a_pui8Source == nullptr)
        { return false; }

        bool bAscii = true;
        size_t stSize = 0;
        size_t stLen = 0;

        while (true)
        {
            c_ui8 ui8Code = *a_pui8Source;
            if (ui8Code == null<ui8>::value)
            { break; }

            /* @@0 utf8
            EMP_ASSERT(is_valid_utf8(a_pui8Source));
            if (is_not_valid_utf8(a_pui8Source))
            {
                bAscii = false;
                stSize = 0;
                stLen = 0;
                return false;
            }*/

            ++a_pui8Source;
            ++stSize;

            if (false == emp::str::is_ascii(ui8Code))
            {
                bAscii = false;
                ui32 ui32Code = ui8Code;

                c_ui32 ui32Length = s_astLengths.val(ui8Code);
                if (ui32Length == 0 || ui32Length > 3)
                { return false; }
                // @@0 utf8 EMP_RET_FALSE_IF(ui32Length > a_pui8Source size)

                ui32 ui32Mask = 0;
                for (size_t stIndex = 0; stIndex < ui32Length; ++stIndex)
                {
                    c_ui8 ui8Current = a_pui8Source[stIndex];
                    ui32Code = (ui32Code << 6) + ui8Current;
                    ui32Mask = (ui32Mask << 1) | s_astInvalidBits.val(emp::ast::cast<ui32>(ui8Current) >> 6);
                }

                stSize += ui32Length;
                a_pui8Source += ui32Length - s_astInvalidOffsets.val(ui32Mask);
                if (ui32Mask != 0)
                { return false; }

                utf8_lookup_s const& rLookup = s_astLookups.ref(ui32Length);
                ui32Code -= rLookup.sub;

                if (ui32Code < rLookup.min ||
                    is_null(ui32Code) ||
                    is_not_valid_utf32(ui32Code))
                { return false; }
            }

            ++stLen;
        }

        a_rstLen = stLen;
        a_rbAscii = bAscii;
        a_rstSize = stSize;
        return true;
    }


public:
    EMP_INLINE EMP_RETURN size_t len(void) const { return m_stLen; }
    EMP_INLINE EMP_RETURN size_t byte_len(void) const { return m_stByteLen; }
    //EMP_INLINE EMP_RETURN size_t count(void) const { return m_Container.count(); } // @@0 remove

public:
    EMP_INLINE EMP_RETURN bool is_empty(void) const { return len() == 0; }
    EMP_INLINE EMP_RETURN bool is_cstr(void) const { return m_stLen > 0 && m_Container.last_val() == null<char>::value; }
    EMP_INLINE EMP_RETURN bool is_ascii(void) const { return m_bAscii; }
    EMP_INLINE EMP_RETURN c_ui8* cstr(void)
    {
        if (is_cstr() == false)
        {
            append_char(null<char>::value);
            return m_Container.first_ptr();
        }
        return m_Container.first_const_ptr();
    }

    /* @@0 uncomment void append_char(c_ui8 a_ui8Char)
    {
        EMP_ASSERT(is_ascii(a_ui8Char));
        c_size stNewLen = m_stLen + (a_ui8Char == null<ui32>::value ? 0 : 1);
        c_size stNewCount = stNewLen + 1;
        m_Container.resize(stNewCount);
        m_Container.set_last(null<char>::value);
        m_Container.set(m_stByteLen, a_ui8Char);
        m_stLen = stNewLen;
        ++m_stByteLen;
    }*/


    void append_char(c_ui64 a_ui64Char)
    {
        //EMP_XX_IF_ASSERT(c_bool bValid = is_valid_utf32(a_ui64Char);) // @@0 valid utf8
        //EMP_ASSERT(bValid);
        ui64 ui64Char = a_ui64Char;//bValid ? a_ui32Char : 'X'; // @@0 append something ?
        size_t stCount = 0;
        while (ui64Char & 0xFF)
        {
            ++stCount;
            ui64Char >>= 8;
        }

        c_size stNewLen = m_stLen + emp::tt::minof<size_t>(stCount, 1u);
        c_size stNewCount = m_stByteLen + stCount + emp::tt::maxof<size_t>(stCount, 1);
        m_Container.resize(stNewCount);
        m_Container.set_last(null<char>::value);

        ui64Char = a_ui64Char;
        stCount = 0;
        while (ui64Char & 0xFF)
        {
            m_Container.set(m_stByteLen + stCount, ui64Char & 0xFF);
            ++stCount;
            ui64Char >>= 8;
        }
        m_stLen = stNewLen;
        m_stByteLen += stCount;
    }
};
EMP_TYPEDEF(u8string)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INLINE EMP_RETURN bool astring::from_u8string(astring& a_rsDest, c_u8string& a_u8Source)
{
    if (a_u8Source.is_ascii())
    {
        if (a_u8Source.m_Container.is_const() == false)
        {
            size_t* pstReferenceCount = nullptr;
            emp::mem::bulk_t<ui8> oSource(a_u8Source.m_Container.retain_bulk<ui8>(pstReferenceCount));
            if (a_rsDest.share(reinterpret_cast<char*>(oSource.first_ptr()), oSource.size(), a_u8Source.len(), pstReferenceCount) == false)
            { return false; }
        }
        else
        {
            emp::mem::bulk_t<c_ui8> const oSource(a_u8Source.m_Container.bulk<c_ui8>());
            a_rsDest = astring(reinterpret_cast<char const*>(oSource.first_ptr()), oSource.size(), a_u8Source.len());
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_RETURN bool cmp(  T const& a_sLeft, T const& a_sRight);
template <typename T> EMP_RETURN bool ncmp( T const& a_sLeft, T const& a_sRight, c_size a_stCount);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename t_Left, typename t_Right>
EMP_NOINSTANCE_STRUCT(str_cmp)
public:
    static EMP_INLINE EMP_RETURN bool apply(const_param<t_Left> a_Left, const_param<t_Right> a_Right)
    { return emp::str::cmp(a_Left, a_Right); }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//template <typename T>
//EMP_RETURN size_t copy(T* const a_szDest, c_size a_stDestSize, T const* const a_szSrc);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, formater_i)
public:
    EMP_INLINE formater_i(void) {}
    virtual EMP_INLINE ~formater_i(void) = 0;

public:
    //>> formater_i
    virtual void operator()(astring& a_rsOutput) const = 0;
    virtual void operator()(u8string& a_rsOutput) const = 0;
    //<< formater_i
};
EMP_INLINE formater_i::~formater_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, formater_a, public, formater_i)
public:
    EMP_INLINE formater_a(void) {}
    virtual EMP_INLINE ~formater_a(void) = 0;

public:
    //>> formater_a
    virtual void set_value(T const& a_rtValue) = 0;
    //<< formater_a
};
template <typename T> EMP_INLINE formater_a<T>::~formater_a(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename t_Params, typename t_Value>
EMP_NOCOPY_BASE(class, formater_t, public, formater_a<emp::tt::try_remove_reference<t_Value>>)
public:
    t_Params m_tParams;

private:
    emp::tt::try_remove_reference<t_Value> const* m_ptValue;

public:
    EMP_INLINE formater_t(void):
        m_tParams(),
        m_ptValue()
    {}

    explicit EMP_INLINE formater_t(t_Params const& a_rtParams):
        m_tParams(a_rtParams),
        m_ptValue(nullptr)
    {}

    EMP_INLINE formater_t(t_Params const& a_rtParams, t_Value const& a_rtValue):
        m_tParams(a_rtParams), m_ptValue(&a_rtValue)
    {}



public:
    //>> formater_i
    EMP_INLINE void operator()(astring& a_rsOutput) const { m_tParams(*m_ptValue, a_rsOutput); }
    EMP_INLINE void operator()(u8string& a_rsOutput) const { m_tParams(*m_ptValue, a_rsOutput); }
    EMP_INLINE EMP_RETURN astring to_astring(void) const { astring sOutput; m_tParams(*m_ptValue, sOutput);  return sOutput; }
    EMP_INLINE EMP_RETURN u8string to_u8string(void) const { u8string sOutput; m_tParams(*m_ptValue, sOutput); return sOutput; }
    //<< formater_i

public:
    //>> formater_a
    EMP_INLINE void set_value(t_Value const& a_rtValue) { m_ptValue = &a_rtValue; }
    //<< formater_a
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(string_format_s)
public:
    template <typename T> void operator()(T const& a_tValue, astring& a_rsOutput) const;
    template <typename T> void operator()(T const& a_tValue, u8string& a_rsOutput) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(real_format_s)
public:
    size_t m_stDecimal = 0;
    bool m_bRound = false;
    bool m_bPadding = false;
    bool m_bScientific = false;
    bool m_bPercent = false;

public:
    real_format_s& precision(c_size a_stValue)
    {
        m_stDecimal = a_stValue;
        return *this;
    }

    real_format_s& round(c_bool a_bValue)
    {
        m_bRound = a_bValue;
        return *this;
    }

    real_format_s& zero(c_bool a_bValue)
    {
        m_bPadding = a_bValue;
        return *this;
    }

    real_format_s& scientific(c_bool a_bValue)
    {
        m_bScientific = a_bValue;
        return *this;
    }

    real_format_s& percent(c_bool a_bValue)
    {
        m_bPercent = a_bValue;
        return *this;
    }
public:
    void operator()(c_float& a_tValue, astring& a_rsOutput) const;
    void operator()(c_float& a_tValue, u8string& a_rsOutput) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(integral_format_s)
public:
    bool m_bPositiveSign = false;
    bool m_bZeroPadding = false;
    size_t m_stCount = 0;
    base_e m_eBase = base_e::Dec;

public:
    integral_format_s& sign(c_bool a_bValue)
    {
        m_bPositiveSign = a_bValue;
        return *this;
    }

    integral_format_s& zero(c_bool a_bValue)
    {
        m_bZeroPadding = a_bValue;
        return *this;
    }

    integral_format_s& pad(c_size a_stValue)
    {
        m_stCount = a_stValue;
        return *this;
    }

    integral_format_s& base(c_base_e a_eValue)
    {
        m_eBase = a_eValue;
        return *this;
    }
public:
    template <typename S, typename T> void operator()(T const& a_tValue, S& a_rsOutput) const;
};

template <typename T> using int_fmt = emp::str::formater_t<emp::str::integral_format_s, T>;
template <typename T> using real_fmt = emp::str::formater_t<emp::str::real_format_s, T>;

#define STR_INT_FMT(x_Value, x_Format) emp::str::int_fmt<decltype(x_Value)>(emp::str::integral_format_s x_Format, x_Value)
#define STR_REAL_FMT(x_Value, x_Format) emp::str::real_fmt<decltype(x_Value)>(emp::str::real_format_s x_Format, x_Value)

#define REAL_FMT_R_(x_Precision, x_Value) STR_REAL_FMT(x_Value, ().round(true).precision(x_Precision))
#define REAL_FMT_RE_(x_Precision, x_Value) STR_REAL_FMT(x_Value, ().round(true).scientific(true).precision(x_Precision))
#define REAL_FMT_R0_(x_Precision, x_Value) STR_REAL_FMT(x_Value, ().zero(true).round(true).precision(x_Precision))

#define REAL_FMT_R5(x_Value) REAL_FMT_R_(5, x_Value)
#define REAL_FMT_R6(x_Value) REAL_FMT_R_(6, x_Value)
#define REAL_FMT_R7(x_Value) REAL_FMT_R_(7, x_Value)
#define REAL_FMT_R10(x_Value) REAL_FMT_R_(10, x_Value)

#define REAL_FMT_RE4(x_Value) REAL_FMT_RE_(4, x_Value)

#define REAL_FMT_R02(x_Value) REAL_FMT_R0_(2, x_Value)

#define INT_FMT_D_(x_Precision, x_Value) STR_INT_FMT(x_Value, ().pad(x_Precision))
#define INT_FMT_0D_(x_Precision, x_Value) STR_INT_FMT(x_Value, ().zero(true).pad(x_Precision))
#define INT_FMT_H_(x_Precision, x_Value) STR_INT_FMT(x_Value, ().pad(x_Precision).base(emp::str::base_e::Hex))
#define INT_FMT_0H_(x_Precision, x_Value) STR_INT_FMT(x_Value, ().zero(true).pad(x_Precision).base(emp::str::base_e::Hex))

#define INT_FMT_D3(x_Value)  INT_FMT_D_(  3, x_Value)

#define INT_FMT_0D3(x_Value) INT_FMT_0D_( 3, x_Value)
#define INT_FMT_0D4(x_Value) INT_FMT_0D_( 4, x_Value)

#define INT_FMT_0H(x_Value)  INT_FMT_0H_( 0, x_Value)
#define INT_FMT_0H4(x_Value) INT_FMT_0H_( 4, x_Value)

#define XTMP_STR_DECLARE_INT_FMT(x_Name, x_Sign, x_Padding, x_Count, x_Base)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename F> S& fromx(S& a_rsOutput, F const& a_rtFrom);
template <typename S, typename F> S xfrom(F const& a_rtFrom);

template <typename F> astring& from(astring& a_rsOutput, F const& a_rtFrom)
{ return fromx(a_rsOutput, a_rtFrom); }

template <typename F> u8string& from(u8string& a_rsOutput, F const& a_rtFrom)
{ return fromx(a_rsOutput, a_rtFrom); }

template <typename F> EMP_RETURN astring afrom(F const& a_rtFrom)
{ return xfrom<astring>(a_rtFrom); }

template <typename F> EMP_RETURN u8string u8from(F const& a_rtFrom)
{ return xfrom<u8string>(a_rtFrom); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T> EMP_RETURN bool to_wrap(S const& a_sFrom, T& a_rTo, S* const a_prsEnd);

template <typename T> EMP_RETURN bool to(c_astring& a_sFrom, T& a_rTo, astring* const a_prsEnd = nullptr)
{ return to_wrap(a_sFrom, a_rTo, a_prsEnd); }

template <typename T> EMP_RETURN bool to(c_u8string& a_sFrom, T& a_rTo, u8string* const a_prsEnd = nullptr)
{ return to_wrap(a_sFrom, a_rTo, a_prsEnd); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(struct, resolve_t)
};

#define XTMP_RESOLVE(...)\
template <>\
EMP_NOINSTANCE(struct, (resolve_t<__VA_ARGS__>))\
    template <typename S>\
    static EMP_INLINE void apply(S& a_rsOutput, __VA_ARGS__ const& a_tParam)\
    { emp::str::from(a_rsOutput, a_tParam); }\
};

XTMP_RESOLVE(unsigned long long int)
XTMP_RESOLVE(unsigned long)
XTMP_RESOLVE(unsigned int)
XTMP_RESOLVE(unsigned short)
XTMP_RESOLVE(long long)
XTMP_RESOLVE(long)
XTMP_RESOLVE(int)
XTMP_RESOLVE(double)
XTMP_RESOLVE(float)
XTMP_RESOLVE(bool)
XTMP_RESOLVE(emp::tim::time_c)
XTMP_RESOLVE(emp::mem::size_s)

#undef XTMP_RESOLVE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_T(tt::is_base<formater_i, T>::value)
EMP_INLINE void resolve(astring& a_rsOutput, T const& a_tParam)
{ a_tParam(a_rsOutput); }
ENABLE_IF_T(false == tt::is_base<formater_i, T>::value)
EMP_INLINE void resolve(astring& a_rsOutput, T const& a_tParam)
{ resolve_t<T>::apply(a_rsOutput, a_tParam); }

EMP_INLINE void resolve(astring& a_rsOutput, tt::c_aliteral& a_sParam);
EMP_INLINE void resolve(astring& a_rsOutput, tt::c_aliteral& a_sParam)
{ a_rsOutput.append_string(astring(a_sParam)); }

EMP_INLINE void resolve(astring& a_rsOutput, c_astring& a_sParam);
EMP_INLINE void resolve(astring& a_rsOutput, c_astring& a_sParam)
{ a_rsOutput.append_string(a_sParam); }

EMP_INLINE void resolve(astring& a_rsOutput, c_char& a_cParam);
EMP_INLINE void resolve(astring& a_rsOutput, c_char& a_cParam)
{ a_rsOutput.append_char(a_cParam); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_T(tt::is_base<formater_i, T>::value)
EMP_INLINE void resolve(u8string& a_rsOutput, T const& a_tParam)
{ a_tParam(a_rsOutput); }
ENABLE_IF_T(false == tt::is_base<formater_i, T>::value)
EMP_INLINE void resolve(u8string& a_rsOutput, T const& a_tParam)
{ resolve_t<T>::apply(a_rsOutput, a_tParam); }

EMP_INLINE void resolve(u8string& a_rsOutput, c_u8string& a_sParam);
EMP_INLINE void resolve(u8string& a_rsOutput, c_u8string& a_sParam)
{ a_rsOutput.append_string(a_sParam); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S>
EMP_RETURN size_t get_next_parameter(S& a_rsOutput, S& a_rsFormat);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename SO, typename SF, typename P>
EMP_INLINE EMP_RETURN size_t format_impl(SO& a_rsOutput, SF& a_rsFormat, P const& a_rtParam);
template <typename SO, typename SF, typename P>
EMP_INLINE EMP_RETURN size_t format_impl(SO& a_rsOutput, SF& a_rsFormat, P const& a_rtParam)
{
    c_size stReturn = get_next_parameter(a_rsOutput, a_rsFormat);
    if (tt::is_not_max(stReturn))
    { resolve(a_rsOutput, a_rtParam); }
    return stReturn;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename SO, typename SF, typename F, typename... P>
EMP_INLINE EMP_RETURN size_t format_impl(SO& a_rsOutput, SF& a_sFormat, F const& a_rtFirst, P const&... a_rtParams);
template <typename SO, typename SF, typename F, typename... P>
EMP_INLINE EMP_RETURN size_t format_impl(SO& a_rsOutput, SF& a_sFormat, F const& a_rtFirst, P const&... a_rtParams)
{
    EMP_ASSERT_BOOL_CALL(tt::is_not_max(format_impl(a_rsOutput, a_sFormat, a_rtFirst)));
    return format_impl(a_rsOutput, a_sFormat, a_rtParams...);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename... P> EMP_INLINE EMP_RETURN S& formatx_impl(S& a_rsOutput, S const& a_sFormat, P const&... a_rtParams);
template <typename S, typename... P> EMP_INLINE EMP_RETURN S& formatx_impl(S& a_rsOutput, S const& a_sFormat, P const&... a_rtParams)
{
    S sFormat(a_sFormat);
    if (tt::is_not_max(format_impl(a_rsOutput, sFormat, a_rtParams...)))
    { EMP_ASSERT_BOOL_CALL(tt::is_max(get_next_parameter(a_rsOutput, sFormat))); }
    return a_rsOutput;
}


template <typename S, typename... P> EMP_INLINE EMP_RETURN S xformat_impl(S const& a_sFormat, P const&... a_rtParams); // @@0 alloc
template <typename S, typename... P> EMP_INLINE EMP_RETURN S xformat_impl(S const& a_sFormat, P const&... a_rtParams)
{
    S sOutput;
    emp::tt::silent(formatx_impl(sOutput, a_sFormat, a_rtParams...));
    return sOutput;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename... P> EMP_INLINE EMP_RETURN astring& formatx(astring& a_rsOutput, c_astring& a_sFormat, P const&... a_rtParams);
template <typename... P> EMP_INLINE EMP_RETURN astring& formatx(astring& a_rsOutput, c_astring& a_sFormat, P const&... a_rtParams)
{ return formatx_impl(a_rsOutput, a_sFormat, a_rtParams...); }

template <typename... P> EMP_INLINE EMP_RETURN u8string& formatx(u8string& a_rsOutput, c_u8string& a_sFormat, P const&... a_rtParams);
template <typename... P> EMP_INLINE EMP_RETURN u8string& formatx(u8string& a_rsOutput, c_u8string& a_sFormat, P const&... a_rtParams)
{ return formatx_impl(a_rsOutput, a_sFormat, a_rtParams...); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename... P> EMP_INLINE EMP_RETURN astring xformat(c_astring& a_sFormat, P const&... a_rtParams);
template <typename... P> EMP_INLINE EMP_RETURN astring xformat(c_astring& a_sFormat, P const&... a_rtParams)
{ return xformat_impl(a_sFormat, a_rtParams...); }

template <typename... P> EMP_INLINE EMP_RETURN u8string xformat(c_u8string& a_sFormat, P const&... a_rtParams);
template <typename... P> EMP_INLINE EMP_RETURN u8string xformat(c_u8string& a_sFormat, P const&... a_rtParams)
{ return xformat_impl(a_sFormat, a_rtParams...); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define STR_FORMAT_XLITERAL(x_Literal, x_Format, ...)\
    EMP_PP_FORWARD(x_Literal(x_Format, EMP_VAARGS_COUNT(__VA_ARGS__))).literal()

#define STR_FORMATX(x_Literal, x_Output, x_Format, ...)\
    emp::str::formatx(x_Output, EMP_PP_FORWARD(STR_FORMAT_XLITERAL(x_Literal, x_Format, __VA_ARGS__)), __VA_ARGS__)

#define STR_XFORMAT(x_Literal, x_Format, ...)\
    emp::str::xformat(EMP_PP_FORWARD(STR_FORMAT_XLITERAL(x_Literal, x_Format, __VA_ARGS__)), __VA_ARGS__)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define STR_FORMATA(x_Output, x_Format, ...) EMP_PP_FORWARD(STR_FORMATX(AFORMAT, x_Output, x_Format, __VA_ARGS__))
#define STR_AFORMAT(x_Format, ...) EMP_PP_FORWARD(STR_XFORMAT(AFORMAT, x_Format, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define STR_FORMATU8(x_Output, x_Format, ...) EMP_PP_FORWARD(STR_FORMATX(U8FORMAT, x_Output, x_Format, __VA_ARGS__))
#define STR_U8FORMAT(x_Format, ...) EMP_PP_FORWARD(STR_XFORMAT(U8FORMAT, x_Format, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void extract_E(c_astring& a_sFrom, astring& a_sExtension);
void extract_F(c_astring& a_sFrom, astring& a_sFilename);
void extract_PF(c_astring& a_sFrom, astring& a_sPath, astring& a_sFilename);
void extract_PFE(c_astring& a_sFrom, astring& a_sPath, astring& a_sFilename, astring& a_sExtension);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
