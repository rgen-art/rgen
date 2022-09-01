//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_LOC_STRING_H
#define EMP_LOC_STRING_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace loc {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(string_s)

    astring key{};
    u8string value{};

    explicit string_s(astring const& a_sKey) : key(a_sKey), value() {}
    string_s(astring const& a_sKey, u8string const& a_sValue) : key(a_sKey), value(a_sValue) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif