//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_FUNCTION_H
#define EMP_GUI_FUNCTION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_coordinate.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat
{
    template <typename> class eval_stack_t;
    template <typename> class function_i;
    template <typename> class input_t;
} }

namespace emp { namespace gfx
{
    template <typename...> class buffer_t;
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(function_skin_s)
public:
    EMP_FIELD(c_coordinate_skin_s* , coordinate , nullptr)
    EMP_FIELD(c_color_s*           , line       , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,function_c, public, coordinate_c)
public:
    typedef float precision_d;

public:
    EMP_FIELD(float                                 , current_frequency , 1.0f      );
    EMP_FIELD(emp::mat::function_i<precision_d>*    , function          , nullptr   );
    EMP_FIELD(emp::mat::eval_stack_t<precision_d>*  , cash              , nullptr   );

private:
    c_function_skin_s* skin = nullptr;

    EMP_POINTER(emp::mat::eval_stack_t<precision_d>) m_pStack;
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pImLineVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pImLineIndices;

    vector<emp::mat::input_t<precision_d>*> m_vpVarX;
    vector<emp::mat::input_t<precision_d>*> m_vpVarIndex;

public:
    function_c(void);
    virtual ~function_c(void);

public:
    //> control_i
    EMP_RETURN bool transform(stack_c& a_rStack);
    //< control_i

public:
    void auto_resize(void);

private:
    void render_function(frame_c const& a_rSender);

public:
    function_c& skin_(cpc_function_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

