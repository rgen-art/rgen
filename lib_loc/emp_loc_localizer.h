//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_LOC_LOCALIZER_H
#define EMP_LOC_LOCALIZER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace loc {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class locale_c;
struct string_s;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, localizer_c)
private:
    static localizer_c* s_pInstance;
public:
#ifdef EMP_XX_LOCALIZE
#define EMP_LOCALIZER emp::loc::localizer_c::instance()
#define EMP_LOCALIZE(x_szString) EMP_LOCALIZER.get_string(x_szString)
#else
#define EMP_LOCALIZE(x_szString) x_szString
#endif
    static void set_instance(localizer_c* const a_pValue) { s_pInstance = a_pValue; }
    static EMP_INLINE EMP_RETURN localizer_c& instance(void) { return *s_pInstance; }

private:
    astring m_sLocale;
    emp::loc::locale_c* m_pLocale;
    EMP_POINTER(u8string) m_psu8NoTranslation;

public:
    localizer_c(void);
    ~localizer_c(void);

    EMP_RETURN bool create(locale_c* const a_pLocale);
    EMP_RETURN bool create(c_astring const& a_sLocale);
    void destroy(void);

public:
    // @@0 remove void refresh(void)
    //{ set_locale(m_sLocale); }

public:
    EMP_RETURN bool is_valid_language(c_astring& a_sValue);

    EMP_RETURN c_u8string& get_string(string_s const& a_rString) const;

    EMP_RETURN c_astring& name(void) const { return m_sLocale; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
