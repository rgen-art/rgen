//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_PROPERTY_H
#define EMP_GUI_PROPERTY_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_list.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* @@0 remove EMP_STRUCT(property_skin_s)
public:
    EMP_FIELD(c_list_skin_c*  , list      , nullptr)
    EMP_FIELD(c_label_skin_s* , label     , nullptr)
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, property_c, public, list_c)
public:
    EMP_FIELD(bool, expanded, false);

private:
    emp::rfx::type_i* root = nullptr;
    map_t<emp::rfx::type_i*, line_c*>* used = nullptr;
    map_t<emp::rfx::type_i*, line_c*>* unused = nullptr;
    size_t reflection = 0;
    bool changed = false;

    vector<emp::rfx::field_s> fields;
    vector<line_c*> m_vpLines;
    vector<class_c const*> stack;

    map_t<emp::rfx::type_i*, line_c*> line_0;
    map_t<emp::rfx::type_i*, line_c*> line_1;
    map_t<emp::rfx::type_i*, bool> expanded_lines;

    emp::rfx::reflect_f on_reflect;

public:
    property_c(void);
    virtual ~property_c(void);

public:
    //> control_i
    EMP_RETURN bool resize(void);
    //< control_i

    property_c& skin_(cpc_list_skin_c a_pValue);

    EMP_RETURN bool populate(void);

    EMP_RETURN bool set_object(emp::rfx::type_i* const a_pValue);

    void add_object(emp::rfx::field_s const& a_rField);

    EMP_RETURN vector<line_c*> const& lines(void) const { return m_vpLines; }

private:
    EMP_RETURN bool reflect(emp::rfx::type_i* const a_pParent, c_size a_stDepth, emp::rfx::field_s const& a_rField);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

