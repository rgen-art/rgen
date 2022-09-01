//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_QUALIFIER_H
#define EMP_TT_QUALIFIER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tt {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE_STRUCT(remove_qualifier_t)
private:
    typedef try_remove_const<try_remove_volatile<try_remove_reference<
        try_remove_const<try_remove_volatile<try_remove_pointer<T>>>>>> tdestination;

public:
    typedef if_else<equal<T, tdestination>::value, T, typename remove_qualifier_t<tdestination>::type> type;
};

template <typename T> using remove_qualifier = typename remove_qualifier_t<T>::type;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

