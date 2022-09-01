//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_cnt_string.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@info #include <iconv.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace str {

template <typename T>
EMP_NOINSTANCE(struct, string_data_t) };

template <>
EMP_NOINSTANCE(struct, string_data_t<astring>)
    typedef astring_data<void> type;
};

template <>
EMP_NOINSTANCE(struct, string_data_t<u8string>)
    typedef u8string_data<void> type;
};

template <typename T>
using string_data = typename string_data_t<T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef EMP_XX_COMPILER_LLVM
EMP_PRAGMA_PUSH_IGNORE(EMP_W_EXIT_TIME_DESTRUCTORS)
#endif

EMP_PRAGMA_PUSH_IGNORE(EMP_W_4592)
template <>
enum_array_of(astring, base_e) astring_data<void>::s_asBases(
{
    ALITERAL("01"),
    ALITERAL("01234567"),
    ALITERAL("0123456789"),
    ALITERAL("0123456789abcdef"),
});
EMP_PRAGMA_POP_IGNORE(EMP_W_4592)

template <>
enum_array_of((emp::cnt::array<ui8, 16, true>), base_e) u8string_data<void>::s_asBases( // @@0 copy
{
    emp::cnt::array<ui8, 16, true>({ '0', '1' }),
    emp::cnt::array<ui8, 16, true>({ '0', '1', '2', '3', '4', '5', '6', '7' }),
    emp::cnt::array<ui8, 16, true>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }),
    emp::cnt::array<ui8, 16, true>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' })
});

EMP_PRAGMA_PUSH_IGNORE(EMP_W_4592)
emp::cnt::array<ui8, u8string::s_stLengthCount, true>  u8string::s_astLengths(
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,4,4,4,4,5,5,0,0
});
EMP_PRAGMA_POP_IGNORE(EMP_W_4592)

EMP_PRAGMA_PUSH_IGNORE(EMP_W_4592)
emp::cnt::array<u8string::utf8_lookup_s, u8string::s_stLookupCount, true> u8string::s_astLookups(
{
    { 0x00000000, 0x00000000 },
    { 0x00000080, 0x00003080 },
    { 0x00000800, 0x000E2080 },
    { 0x00010000, 0x03C82080 },
    { 0x00200000, 0xFA082080 },
    { 0x04000000, 0x82082080 },
} );
EMP_PRAGMA_POP_IGNORE(EMP_W_4592)

EMP_PRAGMA_PUSH_IGNORE(EMP_W_4592)
emp::cnt::array<ui32, u8string::s_stInvalidBitCount, true> u8string::s_astInvalidBits(
    {1, 1, 0, 1} );
EMP_PRAGMA_POP_IGNORE(EMP_W_4592)

EMP_PRAGMA_PUSH_IGNORE(EMP_W_4592)
emp::cnt::array<ui32, u8string::s_stInvalidOffsetCount, true> u8string::s_astInvalidOffsets(
    { 0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5 } );
EMP_PRAGMA_POP_IGNORE(EMP_W_4592)

#ifdef EMP_XX_COMPILER_LLVM
EMP_PRAGMA_POP_IGNORE(EMP_W_EXIT_TIME_DESTRUCTORS)
#endif

//m_aInvalidBmp.resize(st_invalidbmp_count);
//for (size_t stIndex = 0; stIndex < st_invalidbmp_count; ++stIndex)
//{
//    m_aInvalidBmp.set(stIndex >> 1,
//        IsSurrogate(c) || IsNonCharacter(c) || IsReserved(c));
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 remove
template <typename T>
EMP_RETURN bool cmp(T const& a_sLeft, T const& a_sRight)
{
    c_size stLeftLen = a_sLeft.len();
    return stLeftLen == a_sRight.len() ? ncmp(a_sLeft, a_sRight, stLeftLen) : false;
}

template bool cmp(c_u8string&, c_u8string&);
template bool cmp(c_astring&, c_astring&);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 remove
template <typename T>
EMP_RETURN bool ncmp(T const& a_sLeft, T const& a_sRight, c_size a_stCount)
{
    EMP_ASSERT(a_stCount <= a_sLeft.len());
    EMP_ASSERT(a_stCount <= a_sRight.len());

    T sLeft(a_sLeft);
    T sRight(a_sRight);

    size_t stCount = emp::tt::minof(emp::tt::minof(sLeft.len(), sRight.len()), a_stCount);
    while (stCount-- > 0)
    {
        if (sLeft.get_first_char() != sRight.get_first_char())
        { return false; }
        sLeft.trim_left();
        sRight.trim_left();
    }

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void right_split_string(c_astring& a_sFrom, c_char a_cChar, astring& a_rsLeft, astring& a_rsRight);
void right_split_string(c_astring& a_sFrom, c_char a_cChar, astring& a_rsLeft, astring& a_rsRight)
{
    size_t EMP_TT_MAX_VAR(stRight);
    c_size stLen = a_sFrom.len();
    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        if (a_sFrom.get_char(stLen - stIndex - 1) == a_cChar)
        {
            stRight = stIndex;
            break;
        }
    }

    a_rsLeft = astring();
    a_rsRight = a_sFrom;
    if (emp::tt::is_not_max(stRight))
    {
        a_rsLeft = a_sFrom;
        a_rsLeft.trim_right(stRight);
        a_rsRight.trim_left(stLen - stRight);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void extract_PFE(c_astring& a_sFrom, astring& a_sPath, astring& a_sFilename, astring& a_sExtension)
{
    astring sFilename;
    right_split_string(a_sFrom, '.', sFilename, a_sExtension);
    right_split_string(sFilename, '/', a_sPath, a_sFilename);
    if (a_sFilename.len() > 0 && a_sFilename.get_last_char() == '.')
    { a_sFilename.trim_right(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void extract_PF(c_astring& a_sFrom, astring& a_sPath, astring& a_sFilename)
{ right_split_string(a_sFrom, '/', a_sPath, a_sFilename); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void extract_F(c_astring& a_sFrom, astring& a_sFilename) // @@0 remove left
{
    astring sPath;
    extract_PF(a_sFrom, sPath, a_sFilename);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void extract_E(c_astring& a_sFrom, astring& a_sExtension) // @@0 remove left
{
    astring sPath;
    astring sFilename;
    extract_PFE(a_sFrom, sPath, sFilename, a_sExtension);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
void from_real_impl(T const& a_rtValue, c_size a_stDecimal, c_bool a_bRound, c_bool a_bPadding, c_bool a_bScientific,
    c_bool a_bPercent, S& a_rsOutput);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void real_format_s::operator()(c_float& a_tValue, astring& a_rsOutput) const
{ from_real_impl(a_tValue, m_stDecimal, m_bRound, m_bPadding, m_bScientific, m_bPercent, a_rsOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void real_format_s::operator()(c_float& a_tValue, u8string& a_rsOutput) const
{ from_real_impl(a_tValue, m_stDecimal, m_bRound, m_bPadding, m_bScientific, m_bPercent, a_rsOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
void integral_format_s::operator()(T const& a_tValue, S& a_rsOutput) const
{ from_int_impl(a_tValue, m_bPositiveSign, m_stCount, m_bZeroPadding, a_rsOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S>
EMP_RETURN size_t get_next_parameter(S& a_rsOutput, S& a_rsFormat)
{
    while (true)
    {
        if ((a_rsFormat.len() > 0) == false)
        { break; }

        size_t stDelimCount = 0;
        while (true)
        {
            if ((a_rsFormat.len() > 0) == false)
            { break; }

            typename S::tchar cChar = a_rsFormat.get_first_char();
            if (is_sharp(cChar) == false)
            { break; }

            ++stDelimCount;
            a_rsFormat.trim_left();
        }

        auto const l_add_sharp = [&a_rsOutput](c_size a_stDelimCount)
        {
            c_size stCount = a_stDelimCount / 2;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { a_rsOutput.append_char(sharp<typename S::tchar>::value); }
        };

        if (stDelimCount % 2 == 1)
        {
            l_add_sharp(stDelimCount - 1);

            size_t stParameterIndex = 0;
            S sEnd;
            if (to(a_rsFormat, stParameterIndex, &sEnd))
            {
                c_size stFormatLen = a_rsFormat.len();
                c_size stEndLen = sEnd.len();
                if (stFormatLen > stEndLen)
                {
                    a_rsFormat.trim_left(stFormatLen - stEndLen);
                    return stParameterIndex;
                }
            }
            break;
        }
        else if (stDelimCount > 0)
        {
            l_add_sharp(stDelimCount);
        }

        while (true)
        {
            if ((a_rsFormat.len() > 0) == false)
            { break; }

            typename S::tchar cChar = a_rsFormat.get_first_char();
            if (is_sharp(cChar))
            { break; }

            a_rsOutput.append_char(cChar);
            a_rsFormat.trim_left();
        }
    }

    return emp::tt::max<size_t>::value;
}

template EMP_RETURN size_t get_next_parameter(astring&, astring&);
template EMP_RETURN size_t get_next_parameter(u8string&, u8string&);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 in math
template <typename T> EMP_RETURN T f_mod(T const a_tValue, T const a_tDivider);

//@@0:errno emp::mat
template <> EMP_RETURN float f_mod<float>(c_float a_tValue, c_float a_tDivider)
{ return fmodf(a_tValue, a_tDivider); }

//@@0:errno emp::mat
template <> EMP_RETURN double f_mod<double>(c_double a_tValue, c_double a_tDivider)
{ return fmod(a_tValue, a_tDivider); }

//@@0:errno emp::mat
template <> EMP_RETURN long double f_mod<long double>(long double const a_tValue, long double const a_tDivider)
{ return fmodl(a_tValue, a_tDivider); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 in math
template <typename T>
EMP_RETURN T f_break(T const a_tValue, T& a_rtIntegral);

//@@0:errno emp::mat
template <> EMP_RETURN float f_break<float>(c_float a_tValue, float& a_rtIntegral)
{ return modff(a_tValue, &a_rtIntegral); }

//@@0:errno emp::mat
template <> EMP_RETURN double f_break<double>(c_double a_tValue, double& a_rtIntegral)
{ return modf(a_tValue, &a_rtIntegral); }

//@@0:errno emp::mat
template <> EMP_RETURN long double f_break<long double>(long double const a_tValue, long double& a_rtIntegral)
{ return modfl(a_tValue, &a_rtIntegral); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_INLINE EMP_RETURN typename T::tliteral inf_literal(void);
template <> EMP_INLINE EMP_RETURN typename astring::tliteral inf_literal<astring>(void) { return ALITERAL("inf"); }
template <> EMP_INLINE EMP_RETURN typename u8string::tliteral inf_literal<u8string>(void) { return U8LITERAL("inf"); }

template <typename T> EMP_INLINE EMP_RETURN typename T::tliteral nan_literal(void);
template <> EMP_INLINE EMP_RETURN typename astring::tliteral nan_literal<astring>(void) { return ALITERAL("nan"); }
template <> EMP_INLINE EMP_RETURN typename u8string::tliteral nan_literal<u8string>(void) { return U8LITERAL("nan"); }

EMP_INLINE EMP_RETURN ui8 get_base_char(emp::cnt::array<ui8, 16, true> const& a_sBase, c_size a_stIndex)
{ return a_sBase.val(a_stIndex); }

EMP_INLINE EMP_RETURN char get_base_char(c_astring& a_sBase, c_size a_stIndex)
{ return a_sBase.get_char(a_stIndex); }

EMP_INLINE EMP_RETURN size_t get_base_len(emp::cnt::array<ui8, 16, true> const& a_sBase)
{ return a_sBase.count(); }

EMP_INLINE EMP_RETURN size_t get_base_len(c_astring& a_sBase)
{ return a_sBase.len(); }

template <typename T>
EMP_INLINE EMP_RETURN auto get_first_base_char(T const& a_sBase)
{ return get_base_char(a_sBase, 0); }

template <typename T>
EMP_INLINE EMP_RETURN auto get_last_base_char(T const& a_sBase)
{ return get_base_char(a_sBase, get_base_len(a_sBase) - 1); }

template <typename S, typename T>
void from_real_impl(T const& a_rtValue, c_size a_stDecimal, c_bool a_bRound, c_bool a_bPadding, c_bool a_bScientific,
    c_bool a_bPercent, S& a_rsOutput)
{
    static_assert(emp::tt::is_real<T>::value, "");

    typedef typename S::tchar tchar;

    if (emp::mat::is_inf(a_rtValue))
    {
        a_rsOutput.append_literal(inf_literal<S>());
        return;
    }
    else if (emp::mat::is_nan(a_rtValue))
    {
        a_rsOutput.append_literal(nan_literal<S>());
        return;
    }

    auto const& sBase = string_data<S>::s_asBases.template ref<enum_to__(base_e::Dec)>();
    c_size stBaseLen = get_base_len(sBase);

    // @@0 ::runtime_cast
    c_double tBase = emp::ast::cast<T>(stBaseLen);
    c_double tAbsValue = emp::mat::abs(a_rtValue);

    double tValue = 0;
    double tIntegral = 0;
    double tReal = f_break(tAbsValue, tIntegral);

    c_size stDecimalLen = a_rsOutput.byte_len();

    tValue = tReal;
    size_t stCount = 0;

    size_t stTrimCount = (a_bRound ? 1u : 0u);
    c_size stDecimal = a_stDecimal + stTrimCount;
    do
    {
        // @@0 :runtime_cast
        tValue *= tBase;

        double tModI = 0;
        double tModF = f_break(tValue, tModI);
        c_double fMod = f_mod(tValue, tBase);
        c_size stIndex = emp::ast::cast<size_t>(fMod);
        auto const cDigit = get_base_char(sBase, stIndex);
        a_rsOutput.append_char(cDigit);
        tValue = tModF;

        stCount++;
    }
    while (stCount < stDecimal);

    a_rsOutput.reverse_bytes(stDecimalLen);

    /*if (stCount > 1)
    {
        for (size_t stIndex = stCount; stIndex < stCount; ++stIndex)
        { sOutput.append_char(sBase.get_char(0)); }
    }*/

    //c_size stDotPosition = sOutput.len();
    a_rsOutput.append_char(dot<tchar>::value);

    //size_t stFractionalLen = a_rsOutput.byte_len();

    tValue = tIntegral;
    do
    {
        // @@0 :runtime_cast
        c_size stIndex = emp::ast::cast<size_t>(f_mod(tValue, tBase));
        tchar cDigit = get_base_char(sBase, stIndex);
        a_rsOutput.append_char(cDigit);
        tValue /= tBase;
    }
    while (tValue >= 1);

    EMP_ASSERT(emp::mat::is_negative(a_rtValue) == (a_rtValue < 0));
    if (emp::mat::is_negative(a_rtValue))
    { a_rsOutput.append_char(sub<tchar>::value); }

    tchar cDigit = a_rsOutput.get_first_char();
    c_size stBaseIndex = emp::ast::cast<size_t>(cDigit - get_first_base_char(sBase));
    c_bool bCanRound = stBaseIndex >= (get_base_len(sBase) / 2);

    if (a_bRound == false || bCanRound == false)
    {
    }
    else
    {
        size_t stCarry = 1;
        for (size_t stIndex = 1; stIndex < a_rsOutput.byte_len(); ++stIndex)
        {
            auto const cCurrentDigit = a_rsOutput.get_byte(stIndex);
            auto const cFirstBaseChar = get_first_base_char(sBase);
            if (cCurrentDigit >= cFirstBaseChar &&
                cCurrentDigit <= get_last_base_char(sBase))
            {
                c_size stCurrentIndex = emp::ast::cast<size_t>(cCurrentDigit - cFirstBaseChar);
                if (stCurrentIndex + stCarry >= stBaseLen)
                {
                    a_rsOutput.set_byte(stIndex, get_base_char(sBase, (stCurrentIndex + stCarry) - stBaseLen));
                    stCarry = 1;
                }
                else
                {
                    a_rsOutput.set_byte(stIndex, get_base_char(sBase, stCurrentIndex + stCarry));
                    stCarry = 0;
                    break;
                }
            }
        }
        if (stCarry)
        { a_rsOutput.append_char(get_base_char(sBase, 1)); }
    }

    S sTmp(a_rsOutput);
    if (stTrimCount > 0)
    { sTmp.trim_left(); }

    size_t stExponent = 0;
    if (a_bPadding == false)
    {
        bool bDecimal = true;
        while(sTmp.len())
        {
            tchar cChar = sTmp.get_first_char();
            if (cChar == zero<tchar>::value)
            {
                ++stTrimCount;
                sTmp.trim_left();
                //--stFractionalLen;
            }
            else if (cChar == dot<tchar>::value)
            {
                ++stTrimCount;
                sTmp.trim_left();
                bDecimal = false;
                //--stFractionalLen;
                break;
            }
            else
            {
                break;
            }
        }

        if (bDecimal == false && a_bScientific)
        {
            while (sTmp.len())
            {
                tchar cChar = sTmp.get_first_char();
                if (cChar == zero<ui32>::value)
                {
                    ++stExponent;
                    ++stTrimCount;
                    sTmp.trim_left();
                }
                else
                { break; }
            }
        }
    }

    a_rsOutput.reverse_bytes(stDecimalLen);
    if (stTrimCount > 0)
    { a_rsOutput.trim_right(stTrimCount); }
    if (a_bPercent)
    { a_rsOutput.append_char('%'); }
    else if (stExponent > 0)
    {
        a_rsOutput.append_char('E');
        a_rsOutput = emp::str::from(a_rsOutput, stExponent);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
void from_int_impl(T const& a_rtValue, c_bool a_bPositive, size_t a_stPad, c_bool a_bZeroPadding, S& a_rsOutput)
{
    auto const& sBase = string_data<S>::s_asBases.template ref<enum_to__(base_e::Dec)>();

    typedef typename S::tchar tchar;
    typedef tchar const c_tchar;

    // @@0 ::runtime_cast
    tt::unsigned_<T> const tBase = emp::ast::cast<tt::unsigned_<T>>(get_base_len(sBase));
    tt::unsigned_<T> tValue = emp::mat::uabs(a_rtValue);

    c_size stOldLen = a_rsOutput.byte_len();

    do
    {
        // @@0 :runtime_cast
        c_size stIndex = emp::ast::cast<size_t>(tValue % tBase);
        c_tchar cDigit = get_base_char(sBase, stIndex);
        a_rsOutput.append_char(cDigit);
        tValue /= tBase;
    }
    while (tValue);

    // @@0 remove size_t stOffset = 0;
    if (emp::mat::is_negative(a_rtValue))
    {
        a_rsOutput.append_char(sub<tchar>::value);
        // @@0 remove stOffset = 1;
    }
    else if (a_bPositive)
    {
        a_rsOutput.append_char(sub<tchar>::value);
        // @@0 remove stOffset = 1;
    }

    c_size stLen = a_rsOutput.len();
    if (a_stPad > stLen)
    {
        c_tchar cPadding = a_bZeroPadding ? zero<tchar>::value : space<tchar>::value;
        for (size_t stIndex = stLen; stIndex < a_stPad; ++stIndex)
        { a_rsOutput.append_char(cPadding); }
    }

    a_rsOutput.reverse_bytes(stOldLen); // @@0 do this inplace
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*ENABLE_IF_POINTER_T // @@0 implement
void from_impl(T const& EMP_SILENT(a_rtValue), c_base_e EMP_SILENT(a_eBase))
{
    static_assert(false, "");
    EMP_ASSERT(false);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_REAL_T
void from_impl(T const& a_rtValue, astring& a_rsOutput)
{ from_real_impl(a_rtValue, 10, false, true, false, false, a_rsOutput);}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ENABLE_IF_REAL_T
void from_impl(T const& a_rtValue, u8string& a_rsOutput)
{ from_real_impl(a_rtValue, 10, false, true, false, false, a_rsOutput);}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T, ENABLE_IF_INTEGRAL(T)>
void from_impl(T const& a_rtValue, S& a_rsOutput)
{ from_int_impl(a_rtValue, false, 0, false, a_rsOutput); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T, ENABLE_IF(emp::tt::equal<T, emp::tim::time_c>::value)>
void from_impl(T const& a_rValue, S& a_rsOutput)
{
    c_ui64 ui64Milisecond = 1000;
    c_ui64 ui64Second = ui64Milisecond * 1000;
    c_ui64 ui64Minute = ui64Second * 60;
    c_ui64 ui64Hour = ui64Minute * 60;
    c_ui64 ui64Day = ui64Hour * 24;

    c_ui64 ui64Time = a_rValue.usec();

    ui64 ui64Tmp = ui64Time;

    c_ui64 ui64Days = ui64Tmp / ui64Day;
    ui64Tmp -= ui64Days * ui64Day;

    c_ui64 ui64Hours = ui64Tmp / ui64Hour;
    ui64Tmp -= ui64Hours * ui64Hour;

    c_ui64 ui64Minutes = ui64Tmp / ui64Minute;
    ui64Tmp -= ui64Minutes * ui64Minute;

    c_ui64 ui64Seconds = ui64Tmp / ui64Second;
    ui64Tmp -= ui64Seconds * ui64Second;

    c_ui64 ui64Miliseconds = ui64Tmp / ui64Milisecond;
    ui64Tmp -= ui64Miliseconds * ui64Milisecond;

    c_ui64 ui64Microseconds = ui64Tmp;

    c_bool bD = ui64Days > 0;
    c_bool bDH = ui64Hours > 0 || bD;
    c_bool bDHM = ui64Minutes > 0 || bDH;
    c_bool bDHMS = ui64Seconds > 0 || bDHM;
    c_bool bDHMSI = ui64Miliseconds > 0 || bDHMS;

    if (bD)
    {
        emp::str::from(a_rsOutput, ui64Time);
        a_rsOutput.append_char('d');
    }

    if (bDH)
    {
        if (bD)
        {
            a_rsOutput.append_char(' ');
            if (ui64Hours < 10)
            { a_rsOutput.append_char('0'); }
        }

        emp::str::from(a_rsOutput, ui64Hours);
        a_rsOutput.append_char('h');
    }

    if (bDHM)
    {
        if (bDH)
        {
            a_rsOutput.append_char(' ');
            if (ui64Minutes < 10)
            { a_rsOutput.append_char('0'); }
        }

        emp::str::from(a_rsOutput, ui64Minutes);
        a_rsOutput.append_char('m');
    }

    if (bDHMS)
    {
        if (bDHM)
        {
            a_rsOutput.append_char(' ');
            if (ui64Seconds < 10)
            { a_rsOutput.append_char('0'); }
        }

        emp::str::from(a_rsOutput, ui64Seconds);
        a_rsOutput.append_char('s');
    }

    if (bDHMSI)
    {
        if (bDHMS)
        {
            a_rsOutput.append_char(' ');
            if (ui64Miliseconds < 10)
            { a_rsOutput.append_char('0'); }
            if (ui64Miliseconds < 100)
            { a_rsOutput.append_char('0'); }
        }

        emp::str::from(a_rsOutput, ui64Miliseconds);
        a_rsOutput.append_char('m');
        a_rsOutput.append_char('s');

        a_rsOutput.append_char(' ');
        if (ui64Microseconds < 10)
        { a_rsOutput.append_char('0'); }
        if (ui64Microseconds < 100)
        { a_rsOutput.append_char('0'); }
    }

    emp::str::from(a_rsOutput, ui64Microseconds);
    a_rsOutput.append_char('u');
    a_rsOutput.append_char('s');
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T, ENABLE_IF(emp::tt::equal<T, emp::mem::size_s>::value)>
void from_impl(T const& a_rValue, S& a_rsOutput)
{
    c_size cui64Bytes = 1 << 0;
    c_size cui64KBytes = cui64Bytes << 10;
    c_size cui64MBytes = cui64KBytes << 10;
    c_size cui64GBytes = cui64MBytes << 10;

    size_t ui64Tmp = a_rValue.m_stCount;

    c_size ui64GBytes = ui64Tmp / cui64GBytes;
    ui64Tmp -= ui64GBytes * cui64GBytes;

    c_size ui64MBytes = ui64Tmp / cui64MBytes;
    ui64Tmp -= ui64MBytes * cui64MBytes;

    c_size ui64KBytes = ui64Tmp / cui64KBytes;
    ui64Tmp -= ui64KBytes * cui64KBytes;

    c_size ui64Bytes = ui64Tmp;

    c_bool bG = ui64GBytes > 0;
    c_bool bGM = ui64MBytes > 0 || bG;
    c_bool bGMK = ui64KBytes > 0 || bGM;
    c_bool bGMKB = ui64Bytes > 0 || bGMK;

auto const& lZeroPadding = [&a_rsOutput](c_ui64 a_ui64Bytes)
{
    a_rsOutput.append_char(' ');
    if (a_ui64Bytes < 10)
    { a_rsOutput.append_char('0'); }
    if (a_ui64Bytes < 100)
    { a_rsOutput.append_char('0'); }

    //a_rsOutput.append_char(' ');
    /*si64 ui64TmpBytes = a_i64Bytes;
    while (ui64TmpBytes > 0)
    {
        a_rsOutput.append_char('0');
        ui64TmpBytes /= 10;
    }*/
};

    if (bG)
    {
        emp::str::from(a_rsOutput, ui64GBytes);
        a_rsOutput.append_char('g');
    }

    if (bGM)
    {
        if (bG)
        { lZeroPadding(ui64MBytes); }

        emp::str::from(a_rsOutput, ui64MBytes);
        a_rsOutput.append_char('m');
    }

    if (bGMK)
    {
        if (bGM)
        { lZeroPadding(ui64KBytes); }

        emp::str::from(a_rsOutput, ui64KBytes);
        a_rsOutput.append_char('k');
    }

    if (bGMKB)
    {
        if (bGMK)
        { lZeroPadding(ui64Bytes); }
    }

    emp::str::from(a_rsOutput, ui64Bytes);
    a_rsOutput.append_char('b');
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S>//, typename T, ENABLE_IF(emp::tt::equal<T, bool>::value)>
void from_impl(c_bool& a_rtValue, S& a_rsOutput)
{ a_rsOutput.append_char(get_base_char(string_data<S>::s_asBases.template ref<enum_to__(base_e::Bin)>(), a_rtValue ? 1u : 0u)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T, ENABLE_IF(emp::tt::equal<S, T>::value)>
void from_impl(S const& a_rsValue, T& a_rsOutput)
{ a_rsOutput.append_string(a_rsValue); }


void from_impl(c_astring& a_rsValue, u8string& a_rsOut);
void from_impl(c_astring& a_rsValue, u8string& a_rsOut)
{ a_rsOut.append_string(a_rsValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
S& fromx(S& a_rsOutput, T const& a_rtValue)
{
    from_impl(a_rtValue, a_rsOutput);
    return a_rsOutput;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
EMP_RETURN S xfrom(T const& a_rtValue)
{
    S sOutput;
    from_impl(a_rtValue, sOutput);
    return sOutput;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T, ENABLE_IF_INTEGRAL(T), ENABLE_IF_NOT_SIGNED(T)>
EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, T& a_rtTo)
{
    typedef typename S::tchar tchar;

    a_rtTo = 0;
    while(a_sFrom.len() && is_whitespace(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    while(a_sFrom.len() && is_add(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }

    S sHexa(a_sFrom);
    bool bHexa = false;
    if (sHexa.len() > 2 && sHexa.get_first_char() == '0')
    {
        sHexa.trim_left();
        if (sHexa.get_first_char() == 'x')
        {
            bHexa = true;
            a_sFrom.trim_left(2);
        }
    }

    if (a_sFrom.len() == 0)
    { return false; }

    {
        tchar const cChar = a_sFrom.get_first_char();
        if (bHexa)
        {
            if (is_not_numeric(cChar) && is_not_hexa(cChar))
            { return false; }
        }
        else
        {
            if (is_not_numeric(cChar))
            { return false; }
        }
    }


    while(a_sFrom.len() && is_zero(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }

    if (bHexa)
    {
        size_t stCount = 0;
        while(a_sFrom.len() && stCount < (sizeof(T) * 2))
        {
            tchar cChar = a_sFrom.get_first_char();
            if ((is_numeric(cChar) || is_hexa(cChar)) == false)
            { break; }

            // @@0  lib_str
            //a_rtTo <<= 4u;
            if (emp::mat::mul(a_rtTo, emp::ast::cast<T>(16), a_rtTo) == false)
            { EMP_ASSERT(false); return false; }
            tchar ui32Char = 0;
            if (is_numeric(cChar))
            { ui32Char = emp::ast::cast<tchar>(cChar - numeric_min<tchar>::value); }
            else if (is_lower_hexa(cChar))
            { ui32Char = emp::ast::cast<tchar>(10 + (cChar - lower_hexa_min<tchar>::value)); }
            else if (is_upper_hexa(cChar))
            { ui32Char = emp::ast::cast<tchar>(10 + (cChar - upper_hexa_min<tchar>::value)); }
            else
            { break; }
            if (emp::mat::add(a_rtTo, emp::ast::cast<T>(ui32Char), a_rtTo) == false)
            { EMP_ASSERT(false); return false; }
            a_sFrom.trim_left();
            ++stCount;
        }
    }
    else
    {
        while(a_sFrom.len())
        {
            tchar const cChar = a_sFrom.get_first_char();
            if (is_numeric(cChar) == false)
            { break; }

            if (emp::mat::mul(a_rtTo, emp::ast::cast<T>(10), a_rtTo) == false)
            { return false; }
            if (emp::mat::add(a_rtTo, emp::ast::cast<T>(cChar - numeric_min<char>::value), a_rtTo) == false)
            { return false; }
            a_sFrom.trim_left();
        }
    }

    return true;
}

template <typename S> EMP_RETURN bool to_impl(S& a_sFrom, bool& a_rtTo);
template <typename S> EMP_RETURN bool to_impl(S& a_sFrom, bool& a_rtTo)
{
    a_rtTo = false;
    while(a_sFrom.len() && is_whitespace(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    while(a_sFrom.len() && is_add(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    if (a_sFrom.len() == 0 || is_not_numeric(a_sFrom.get_first_char()))
    { return false; }
    while(a_sFrom.len() && is_zero(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    if (is_one(a_sFrom.get_first_char()))
    {
        a_rtTo = true;
        a_sFrom.trim_left();
    }
    if (a_sFrom.len() == 0 || is_not_numeric(a_sFrom.get_first_char()))
    { return true; }

    EMP_ASSERT(false);
    return false;
}

template <typename S, typename T, ENABLE_IF_INTEGRAL(T), ENABLE_IF_SIGNED(T)>
EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, T& a_rtTo)
{
    T tSign = 1;

    a_rtTo = 0;
    while(a_sFrom.len() && is_whitespace(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    while(a_sFrom.len())
    {
        typename S::tchar const cChar = a_sFrom.get_first_char();
        if (is_add(cChar))
        { }
        else if (is_sub(cChar))
        { tSign = emp::ast::cast<T>(tSign * -1); }
        else
        { break; }
        a_sFrom.trim_left();
    }

    if (a_sFrom.len() == 0 || is_not_numeric(a_sFrom.get_first_char()))
    { return false; }
    while(a_sFrom.len() && is_zero(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }

    while(a_sFrom.len() && is_numeric(a_sFrom.get_first_char()))
    {
        typename S::tchar const cChar = a_sFrom.get_first_char();
        EMP_RET_FALSE_IF_NOT(emp::mat::mul(a_rtTo, emp::ast::cast<T>(10), a_rtTo));

        T tDigit = emp::ast::cast<T>(cChar - zero<char>::value);
        EMP_RET_FALSE_IF_NOT(emp::mat::add(a_rtTo, tDigit, a_rtTo));

        a_sFrom.trim_left();
    }

    EMP_RET_FALSE_IF_NOT(emp::mat::mul(a_rtTo, tSign, a_rtTo));
    return true;
}

template <typename S>
EMP_RETURN bool to_impl(S& a_sFrom, char& a_rtTo)
{
    typename S::tchar const cChar = a_sFrom.get_first_char();
    a_rtTo = cChar;
    return true;
    //@@0
    /*if (emp::str::is_ascii(cChar))
    {
        a_rtTo = emp::ast::cast<char>(cChar);
        return true;
    }*/
    //a_rtTo = emp::str::null<char>::value;
    //return false;
}

template <typename S, typename T, ENABLE_IF_REAL(T)>
EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, T& a_rtTo)
{
    double fIntegral = 0;
    double fReal = 0;

    double tSign = 1;

    a_rtTo = 0;
    while(a_sFrom.len() && is_whitespace(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }
    while(a_sFrom.len())
    {
        typename S::tchar const cChar = a_sFrom.get_first_char();
        if (is_add(cChar))
        { }
        else if (is_sub(cChar))
        { tSign *= -1; }
        else
        { break; }
        a_sFrom.trim_left();
    }

    if (a_sFrom.len() == 0 ||
        (is_not_numeric(a_sFrom.get_first_char()) &&
         is_not_dot(a_sFrom.get_first_char())))
    {
        if (emp::str::cmp(a_sFrom, S(inf_literal<S>()))) // @@0 wtf : explicit utf32string
        {
           a_rtTo = emp::mat::inf<T>();
           return true;
        }
        else if (emp::str::cmp(a_sFrom, S(nan_literal<S>()))) // @@0 wtf : explicit utf32string
        {
            a_rtTo = emp::mat::nan<T>();
            return true;
        }
        return false;
    }
    while(a_sFrom.len() && is_zero(a_sFrom.get_first_char()))
    { a_sFrom.trim_left(); }

    while(a_sFrom.len() && is_numeric(a_sFrom.get_first_char()))
    {
        typename S::tchar const cChar = a_sFrom.get_first_char();
        double tDigit = emp::ast::cast<T>(cChar - zero<typename S::tchar>::value);
        if (emp::mat::mul(fIntegral, 10.0, fIntegral) == false)
        { EMP_ASSERT(false); return false; }
        if (emp::mat::add(fIntegral, tDigit, fIntegral) == false)
        { EMP_ASSERT(false); return false; }
        a_sFrom.trim_left();
    }

    if (a_sFrom.len() && is_dot(a_sFrom.get_first_char()))
    {
        a_sFrom.trim_left();
        double tOffset = 0.1;
        while(a_sFrom.len() && is_numeric(a_sFrom.get_first_char()))
        {
            auto cChar = a_sFrom.get_first_char();
            double tDigit = emp::ast::cast<T>(cChar - zero<typename S::tchar>::value);
            //printf("tdig : %f\n", tDigit);
            if (emp::mat::mul(tDigit, tOffset, tDigit) == false)
            { EMP_ASSERT(false); return false; }
            //printf("tdigoff : %f\n", tDigit);
            if (emp::mat::add(fReal, tDigit, fReal) == false)
            { EMP_ASSERT(false); return false; }
            //printf("treal : %f\n", fReal);
            if (emp::mat::mul(tOffset, 0.1, tOffset) == false)
            { EMP_ASSERT(false); return false; }
            a_sFrom.trim_left();
        }
    }

    double dResult = 0;
    if (emp::mat::add(fIntegral, fReal, dResult) == false)
    { EMP_ASSERT(false); return false; }
    //printf("abs : %f\n", a_rtTo);
    if (emp::mat::mul(dResult, tSign, dResult) == false)
    { EMP_ASSERT(false); return false; }
    //printf("fin : %f\n", a_rtTo);

    return emp::ast::try_cast(dResult, a_rtTo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
template <typename S>
EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, S& a_rtTo)
{
    a_rtTo = a_sFrom;
    c_size stLen = a_sFrom.len();
    if (stLen > 0)
    { a_sFrom.trim_left(stLen); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_INTERN EMP_RETURN bool to_impl(u8string& a_sFrom, astring&a_ru8To)
{
    if (astring::from_u8string(a_ru8To, a_sFrom) == false)
    { return false; }

    c_size stLen = a_sFrom.len();
    if (stLen > 0)
    { a_sFrom.trim_left(stLen); }
    return true;
}

EMP_INTERN EMP_RETURN bool to_impl(astring& a_sFrom, u8string& a_ru8To)
{
    a_ru8To = u8string(a_sFrom);
    c_size stLen = a_sFrom.len();
    if (stLen > 0)
    { a_sFrom.trim_left(stLen); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
template <typename S>
EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, emp::tim::time_c& a_rtTo)
{
    ui64 ui64Value = 0;
    EMP_RET_FALSE_IF_NOT(emp::str::to_impl(a_sFrom, ui64Value));
    a_rtTo = emp::tim::time_c(ui64Value);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
//template <typename S>
//EMP_INTERN EMP_RETURN bool to_impl(S& a_sFrom, emp::mem::size_s& a_rtTo)
//{
//    ui64 ui64Value = 0;
//    EMP_RET_FALSE_IF_NOT(emp::str::to_impl(a_sFrom, ui64Value));
//    a_rtTo = emp::mem::size_s(ui64Value);
//    return true;
//}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename S, typename T>
EMP_RETURN bool to_wrap(S const& a_sFrom, T& a_rtTo, S* const a_psEnd)
{
    S sFrom(a_sFrom);

    if (a_psEnd != nullptr)
    { *a_psEnd = sFrom; }

    c_bool bReturn = to_impl(sFrom, a_rtTo);

    if (a_psEnd != nullptr)
    { *a_psEnd = sFrom; }
    else
    { return bReturn && sFrom.len() == 0; }
    return bReturn;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template bool to_wrap(c_astring&, astring&, astring* const);
template bool to_wrap(c_astring&, u8string&, astring* const);

template bool to_wrap(c_astring&, bool&, astring* const);

template bool to_wrap(c_astring&, float&, astring* const);
template bool to_wrap(c_astring&, double&, astring* const);

template bool to_wrap(c_astring&, emp::tim::time_c&, astring* const);

template bool to_wrap(c_astring&, char&, astring* const);
template bool to_wrap(c_astring&, short&, astring* const);
template bool to_wrap(c_astring&, int&, astring* const);
template bool to_wrap(c_astring&, long&, astring* const);
template bool to_wrap(c_astring&, long long&, astring* const);

template bool to_wrap(c_astring&, unsigned char&, astring* const);
template bool to_wrap(c_astring&, unsigned short&, astring* const);
template bool to_wrap(c_astring&, uint&, astring* const);
template bool to_wrap(c_astring&, unsigned long&, astring* const);
template bool to_wrap(c_astring&, unsigned long long&, astring* const);


template bool to_wrap(c_u8string&, float&, u8string* const);
template bool to_wrap(c_u8string&, emp::tim::time_c&, u8string* const);
template bool to_wrap(c_u8string&, bool&, u8string* const);
template bool to_wrap(c_u8string&, ui8&, u8string* const);
template bool to_wrap(c_u8string&, ui16&, u8string* const);
template bool to_wrap(c_u8string&, unsigned int&, u8string* const);
template bool to_wrap(c_u8string&, unsigned long long&, u8string* const);
template bool to_wrap(c_u8string&, long long&, u8string* const);
template bool to_wrap(c_u8string&, astring&, u8string* const);
template bool to_wrap(c_u8string&, u8string&, u8string* const);
//template bool to_wrap(c_u8string&, ui16&, u8string* const);
//template bool to_wrap(c_u8string&, emp::mem::size_s&, u8string* const);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template emp::tt::enable_if<emp::tt::not_equal<unsigned long, unsigned long long>::value, void> integral_format_s::operator()(unsigned long long const&, astring&) const;
template emp::tt::enable_if<emp::tt::not_equal<unsigned long, unsigned long long>::value, void> integral_format_s::operator()(unsigned long long const&, u8string&) const;

template void integral_format_s::operator()(unsigned long const&, astring&) const;
template void integral_format_s::operator()(unsigned long const&, u8string&) const;

template void integral_format_s::operator()(c_uint&, astring&) const;
template void integral_format_s::operator()(c_uint&, u8string&) const;

template void integral_format_s::operator()(unsigned short const&, astring&) const;
template void integral_format_s::operator()(unsigned char const&, astring&) const;

template void integral_format_s::operator()(long const&, astring&) const;
template void integral_format_s::operator()(long const&, u8string&) const;

template void integral_format_s::operator()(c_int&, astring&) const;
template void integral_format_s::operator()(short const&, astring&) const;
template void integral_format_s::operator()(c_char&, astring&) const;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template astring& fromx(astring&, emp::tim::time_c const&);
template astring& fromx(astring&, emp::mem::size_s const&);

template astring& fromx(astring&, unsigned long long const&);

template astring& fromx(astring&, unsigned long const&);

template astring& fromx(astring&, c_uint&);
template astring& fromx(astring&, unsigned short const&);
template astring& fromx(astring&, unsigned char const&);

template astring& fromx(astring&, long long const&);
template astring& fromx(astring&, long const&);
template astring& fromx(astring&, c_int&);
template astring& fromx(astring&, short const&);
template astring& fromx(astring&, char const&);

template astring& fromx(astring&, c_bool&);

template astring& fromx(astring&, c_float&);

template astring& fromx(astring&, c_double&);


template u8string& fromx(u8string&, c_u8string&);
template u8string& fromx(u8string&, c_astring&);

template u8string& fromx(u8string&, emp::tim::time_c const&);
template u8string& fromx(u8string&, emp::mem::size_s const&);

template u8string& fromx(u8string&, unsigned long const&);
template u8string& fromx(u8string&, unsigned long long const&);

template u8string& fromx(u8string&, long long const&);

template u8string& fromx(u8string&, long const&);

template u8string& fromx(u8string&, int const&);

template u8string& fromx(u8string&, short const&);

template u8string& fromx(u8string&, char const&);

template u8string& fromx(u8string&, c_uint&);
template u8string& fromx(u8string&, c_ui16&);
template u8string& fromx(u8string&, c_ui8&);

template u8string& fromx(u8string&, c_bool&);

template u8string& fromx(u8string&, c_float&);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template void from_real_impl(c_float&, c_size, c_bool, c_bool, c_bool, c_bool, astring&);
template void from_real_impl(c_float&, c_size, c_bool, c_bool, c_bool, c_bool, u8string&);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template astring xfrom(emp::tim::time_c const&);
template astring xfrom(emp::mem::size_s const&);

template astring xfrom(c_uint&);

template astring xfrom(c_size&);
template astring xfrom(c_int&);
template astring xfrom(c_bool&);

template astring xfrom(c_float&);


template u8string xfrom(emp::tim::time_c const&);
template u8string xfrom(emp::mem::size_s const&);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN size_t manager_c::utf8_len(shell<c_ui8> const& a_sFrom) const
{
    size_t stLen = 0;

    shell<c_ui8> sFrom(a_sFrom);

    if (is_empty(sFrom))
    { return 0; }

    while (is_not_empty(sFrom) && is_not_null(sFrom.first_val()))
    {
        EMP_ASSERT(is_valid_utf8(sFrom));
        if (is_not_valid_utf8(sFrom))
        { EMP_ASSERT(false); return 0; }

        c_ui8 ui8Code = sFrom.first_val();
        sFrom.trim_left();

        ui32 ui32Code = ui8Code;
        if (is_ascii(ui8Code))
        {
        }
        else
        {
            c_ui32 ui32Length = m_aLengths.val(ui8Code);
            if (ui32Length == 0)
            { EMP_ASSERT(false); return 0; }

            if (ui32Length > 3)
            { EMP_ASSERT(false); return 0; }

            if (ui32Length > sFrom.count())
            { EMP_ASSERT(false); return 0; }

            ui32 ui32Mask = 0;
            c_size stCount = ui32Length;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_ui8 ui8Current = sFrom.val(stIndex);
                ui32Code = (ui32Code << 6) + ui8Current;
                ui32Mask = (ui32Mask << 1) | m_aInvalidBits.val(emp::ast::cast<ui32>(ui8Current) >> 6);
            }

            sFrom.trim_left(stCount - m_aInvalidOffsets.val(ui32Mask));
            if (ui32Mask)
            { EMP_ASSERT(false); return 0; }

            utf8_lookup_s const& rLookup = m_aLookups.ref(stCount);
            ui32Code -= rLookup.sub;
            if (ui32Code < rLookup.min)
            { EMP_ASSERT(false); return 0; }

            if (is_null(ui32Code))
            { EMP_ASSERT(false); return 0; }

            if (is_not_valid_utf32(ui32Code))
            { EMP_ASSERT(false); return 0; }
        }

        ++stLen;
    }

    return stLen;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::utf8_to_utf32(shell<c_ui8>& a_sFrom, shell<ui32>& a_sTo, size_t& a_rstCount, size_t& a_rstSize) const
{
    a_rstCount = 0;
    a_rstSize = 0;

    if (is_empty(a_sFrom))
    { EMP_ASSERT(false); return false; }

    while (is_not_empty(a_sFrom) && is_not_null(a_sFrom.first_val()))
    {
        //printf("THE CODE\n");
        EMP_ASSERT(is_valid_utf8(a_sFrom));
        if (is_not_valid_utf8(a_sFrom))
        { EMP_ASSERT(false); return false; }

        c_ui8 ui8Code = a_sFrom.first_val();
        ui32 ui32Code = ui8Code;

        a_sFrom.trim_left();
        if (is_ascii(ui8Code))
        {
        }
        else
        {
        c_ui32 ui32Length = m_aLengths.val(ui8Code);
        if (ui32Length == 0)
        { EMP_ASSERT(false); return false; }

        if (ui32Length > 3)
        { EMP_ASSERT(false); return false; }

        if (ui32Length > a_sFrom.count())
        { EMP_ASSERT(false); return false; }
            //printf("Code : %x\n", ui32Code);
//            ui32Code &= (emp::ast::cast<ui32>(0x3F) >> ui32Length);
            //printf("Code : %x\n", ui32Code);

        ui32 ui32Mask = 0;
        c_size stCount = ui32Length;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_ui8 ui8Current = a_sFrom.val(stIndex);
            //printf("Code : %x, %x\n", ui32Code, ui8Current);
            ui32Code = (ui32Code << 6) + ui8Current;
            ui32Mask = (ui32Mask << 1) | m_aInvalidBits.val(emp::ast::cast<ui32>(ui8Current) >> 6);
            //printf("Code : %x, %x\n", ui32Code, ui32Mask);
        }

        c_size stOffsetIndex = stCount - m_aInvalidOffsets.val(ui32Mask);

        //printf("offset : %u\n", stOffsetIndex);
        a_sFrom.trim_left(stOffsetIndex);
        if (ui32Mask)
        { EMP_ASSERT(false); return false; }

        utf8_lookup_s const& rLookup = m_aLookups.ref(stCount);
        //printf("Code : %x, %x\n", ui32Code, rLookup.sub);
        ui32Code -= rLookup.sub;
        //printf("Code : %x, %x\n", ui32Code, rLookup.sub);
        if (ui32Code < rLookup.min)
        { EMP_ASSERT(false); return false; }
        }

        if (is_empty(a_sTo))
        { EMP_ASSERT(false); return false; }

        if (is_null(ui32Code))
        { EMP_ASSERT(false); return false; }

        if (is_not_valid_utf32(ui32Code))
        { EMP_ASSERT(false); return false; }

        a_sTo.set_first(ui32Code);
        a_sTo.trim_left();
        ++a_rstCount;
        ++a_rstSize;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool manager_c::utf32_to_utf8(shell<c_ui32>& a_sFrom, shell<ui8>& a_sTo, size_t& a_rstCount, size_t& a_rstSize) const
{
    a_rstCount = 0;
    a_rstSize = 0;

    if (is_empty(a_sFrom))
    { EMP_ASSERT(false); return false; }

    while (is_not_empty(a_sFrom) && is_not_null(a_sFrom.first_val()))
    {
        c_ui32 ui32Code = a_sFrom.first_val();
        a_sFrom.trim_left();

        if (is_not_valid_utf32(ui32Code))
        { EMP_ASSERT(false);return false; }

        emp::cnt::shell<c_ui8> sutf8Code(a_sTo);

        size_t stCount = 0;
        ui8 ui8Char = 0;

        if (is_ascii(ui32Code))
        {
            ui8Char = emp::ast::cast<ui8>(ui32Code);
            stCount = 0;
        }
        else if (ui32Code < 0x800)
        {
            ui8Char = emp::ast::cast<ui8>(0xC0 | (0xFF & (ui32Code >> 6)));
            stCount = 1;
        }
        else if (ui32Code < 0x10000)
        {
            ui8Char = emp::ast::cast<ui8>(0xE0 | (0xFF & (ui32Code >> 12)));
            stCount = 2;
        }
        else if (ui32Code < 0x200000)
        {
            ui8Char = emp::ast::cast<ui8>(0xF0 | (0xFF & (ui32Code >> 18)));
            stCount = 3;
        }
        else
        {
            EMP_ASSERT_UNREACHABLE();
            return false;
        }

        if (is_empty(a_sTo))
        { EMP_ASSERT(false); return false; }

        a_sTo.set_first(ui8Char);
        a_sTo.trim_left();

        if (a_sTo.count() < stCount)
        { EMP_ASSERT(false); return false; }

        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_ui32 ui32Offset = emp::ast::cast<ui32>(stCount - stIndex - 1);
            c_ui8 ui8CurrentChar = emp::ast::cast<ui8>(ascii<ui32>::value | ((ui32Code >> (ui32Offset * 6u)) & 0x3F));

            a_sTo.set_first(ui8CurrentChar);
            a_sTo.trim_left();
        }

        EMP_ASSERT(is_valid_utf8(sutf8Code));
        if (is_not_valid_utf8(sutf8Code))
        { EMP_ASSERT(false); return false; }

        a_rstSize += stCount + 1;
        ++a_rstCount;
    }

    return true;
}
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_RETURN bool copy(shell<T const>& a_sFrom, shell<T>& a_sTo, size_t& a_rstCount, size_t& a_rstSize)
{
    a_rstCount = 0;
    a_rstSize = 0;

    while ( is_not_empty(a_sFrom) &&
            is_not_null(a_sFrom.first_val()) &&
            is_not_empty(a_sTo))
    {
        a_sTo.set_first(a_sFrom.first_val());
        a_sFrom.trim_left();
        a_sTo.trim_left();
        ++a_rstCount;
        ++a_rstSize;
    }

    bool bReturn = is_empty(a_sFrom) || (a_sFrom.count() == 1 && is_null(a_sFrom.first_val()));

    EMP_ASSERT(bReturn);
    return bReturn;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
