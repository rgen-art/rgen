//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_GRAPH_H
#define EMP_GUI_GRAPH_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_button.h"
#include "emp_gui_function.h"
#include "emp_gui_mdi.h"
#include "emp_gui_nurbs.h"
#include "emp_gui_picture.h"
#include "emp_gui_popup.h"
#include "emp_gui_property.h"
#include "emp_gui_resolution.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { template <typename> class envelope_t; } }

namespace emp { namespace snd { EMP_FORWARD(class, soft_synth_c) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef float precision_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef emp::fct::function_t<bool(*)(c_astring&)> fct_expression_changed_d;
typedef emp::fct::function_t<void(*)(c_ui32)> fct_function_removed_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(graph_skin_s)
    EMP_FIELD( c_panel_skin_s*          , panel             )
    EMP_FIELD( c_toolbar_skin_s*        , toolbar           )
    EMP_FIELD( c_sizer_skin_s*          , main              )
    EMP_FIELD( c_button_skin_s*         , button            )
    EMP_FIELD( c_sizer_skin_s*          , header            )
    EMP_FIELD( c_label_skin_s*          , header_picture    )
    EMP_FIELD( c_picture_skin_s*        , remove_picture    )
    EMP_FIELD( c_sizer_skin_s*          , docker            )
    EMP_FIELD( c_function_skin_s*       , function          )
    // @@0 spacer EMP_FIELD( c_container_skin_s*      , spacer            )
    EMP_FIELD( c_list_skin_c*           , property          )
    EMP_FIELD( c_sizer_skin_s*          , input_docker      )
    EMP_FIELD( c_container_skin_s*      , pin_container     )
    EMP_FIELD( c_picture_skin_s*        , pin_picture       )
    EMP_FIELD( c_icon_skin_s*           , pout_icon         )
    EMP_FIELD( c_color_s*               , pin_line          )
    EMP_FIELD( c_color_s*               , pout_line         )
    EMP_FIELD( emp::gfx::model_c const* , gfx_pin           )
    EMP_FIELD( emp::gfx::model_c const* , gfx_remove        )
    EMP_FIELD( emp::gfx::model_c const* , gfx_expanded      )
    EMP_FIELD( emp::gfx::model_c const* , gfx_collapsed     )
    EMP_FIELD( emp::gfx::model_c const* , gfx_pout          )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, node_function_i)
public:
    node_function_i(void) {}
    virtual ~node_function_i(void) = 0;

public:
    //>> node_function_i
    virtual void set_function(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew) = 0;
    virtual EMP_RETURN control_i* control(void) = 0;
    virtual void center(void) = 0;
    virtual void update(emp::snd::soft_synth_c* const a_pSynth, c_ui32 a_ui32Function) = 0;
    virtual void update_info(info_s& a_rInfo) const = 0;
    virtual void set_info(c_info_s& a_rInfo) = 0;
    virtual void resolution_changed(cpc_resolution_c a_pSender) = 0;
    virtual void set_current_frequency(c_float a_fValue) = 0;
    //<< node_function_i
};
inline node_function_i::~node_function_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, node_function_c, public, node_function_i)
private:
    function_c function;

public:
    node_function_c(void);
    virtual ~node_function_c(void);

public:
    void create(void);
    void destroy(void);

public:
    //>> node_function_i
    void set_function(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew);
    EMP_RETURN control_i* control(void);
    void center(void);
    void update(emp::snd::soft_synth_c* const, c_ui32);
    void update_info(info_s& a_rInfo) const;
    void set_info(c_info_s& a_rInfo);
    void resolution_changed(cpc_resolution_c a_pSender);
    void set_current_frequency(c_float a_fValue);
    //<< node_function_i

public:
    void set_skin(cpc_graph_skin_s a_pSkin);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, envelope_function_c, public, node_function_i)
private:
    nurbs_c nurbs{};
    emp::mat::envelope_t<precision_d>* envelope = nullptr;

public:
    envelope_function_c(void);
    virtual ~envelope_function_c(void);

public:
    void create(void);
    void destroy(void);

public:
    //>> node_function_i
    void set_function(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew);
    EMP_RETURN control_i* control(void);
    void center(void) {}
    void update(emp::snd::soft_synth_c* const a_pSynth, c_ui32 a_ui32Function);
    void update_info(info_s& a_rInfo) const;
    void set_info(c_info_s& a_rInfo);
    void resolution_changed(cpc_resolution_c a_pSender);
    void set_current_frequency(c_float EMP_SILENT(a_fValue)) {}
    //<< node_function_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class graph_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, node_c, public, resolution_c)
public:
    EMP_FIELD(ui32, function, 0);
    EMP_FIELD(astring, expression, {});

    fct_expression_changed_d on_expression_changed{};
    fct_function_removed_d on_function_removed{};

private:
    container_c input_container{};

    docker_c main{};
    docker_c header{};
    docker_c output_docker{};
    docker_c docker{};

    spacer_c spacer_0{};
    spacer_c spacer_1{};
    docker_c input_docker{};
    spacer_c input_spacer{};

    button_c button{};

    label_c output{};

    picture_c remove{};

    property_c input_property{};
    property_c property{};

    designer_c designer;

    emp::rfx::type_t<astring>* reflection = nullptr;
    node_function_i* m_pFunction = nullptr;
    emp::snd::soft_synth_c* synth = nullptr;
    c_graph_skin_s* skin = nullptr;

    vector<float> m_vfValues{};
    vector<astring> m_vsValues{};


    vector<icon_c*> output_icons{};
    vector<picture_c*> input_pictures{};
    vector<emp::rfx::type_i*> reflections{};


    size_t m_stInputCount = 0;
    size_t m_stOutputCount = 0;

    emp::str::formater_t<emp::str::real_format_s, float> format{};

public:
    node_c(void);
    virtual ~node_c(void);

public:
    EMP_RETURN bool create(void);
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    //<< control_i

public:
    EMP_RETURN size_t output_count(void) const { return m_stOutputCount; }
    EMP_RETURN v3f get_pout_info(c_size a_stOut) const;
    EMP_RETURN icon_c const* get_pout_button(c_size a_stOut) const;

    EMP_RETURN size_t input_count(void) const { return m_stInputCount; }
    EMP_RETURN v3f get_pin_info(c_size a_stIn) const;
    EMP_RETURN picture_c const* get_pin_button(c_size a_stIn) const;

    void contains(cpc_control_i a_pButton, ui32& a_rui32Pin, ui32& a_rui32Pout) const;

    void link(c_size a_stIndex, node_c* a_pNode, c_size a_stOut);
    void unlink(c_size a_stIndex);
    void relink(c_size a_stIndex);

    void pin_check(c_size a_stIndex, c_bool a_bValue);
    void pout_check(c_size a_stIndex, c_bool a_bValue);

public:
    EMP_RETURN bool autosize(void);
    EMP_RETURN bool autoresize(void);

    EMP_RETURN bool resize_function(void);
    void update_visibility(void);

public:
    void set_node_function(node_function_i* const a_pValue);

public:
    EMP_RETURN bool expression_changed(c_astring& a_sValue);

public:
    EMP_RETURN bool pinout_changed(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew);

public:
    void set_skin(cpc_graph_skin_s a_pValue);
    void set_registrar(registrar_i* const a_pValue);
    void set_synth(emp::snd::soft_synth_c* const a_pValue);

    void update_info(info_s& a_rInfo) const;
    void set_info(c_info_s& a_rInfo);

public:
    void default_value(c_ui32 a_ui32Index, c_float a_fValue);

public:
    EMP_RETURN bool get_value(c_size a_stIndex, astring& a_rsValue);
    EMP_RETURN bool set_value(c_size a_stIndex, c_astring& a_rsValue);
    EMP_RETURN bool get_enable(c_size a_stIndex);

    EMP_RETURN bool get_function_value(astring& a_rsValue);
    EMP_RETURN bool set_function_value(c_astring& a_rsValue);

public:
    EMP_RETURN bool set_float_value(c_size a_stIndex, c_float a_fValue);

public:
    EMP_RETURN return_e remove_node(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e show_output(cpc_control_i a_pSender, c_bool a_bValue, c_change_e a_eChange);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, graph_c, public, mdi_c)
private:
    popup_c popup;

    vector<node_c*> m_vpNodes;
    vector<node_function_i*> m_vpNodeFunctions;

    ui32 m_ui32CurrentFunction;
    ui32 m_ui32CurrentPin;
    ui32 m_ui32CurrentPout;
    v2f m_v2CurrentPosition;

    size_t m_stLines;

    vector<emp::gfx::gl_nurbs_c*> m_vpImNurbs;

    EMP_POINTER(emp::gfx::gl_nurbs_c) m_pImNurbs;

    registrar_i* m_pRegistrar;

    emp::snd::soft_synth_c* synth = nullptr;

    c_graph_skin_s* skin = nullptr;

public:
    graph_c(void);
    virtual ~graph_c(void);

    void create(registrar_i* const a_pRegistrar);
    void destroy(void);

public:
    //>> control_i
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void set_skin(cpc_graph_skin_s a_pValue);

    void set_synth(emp::snd::soft_synth_c* const a_pValue);

    void set_current_frequency(c_float a_fValue);

public:
    EMP_RETURN node_c* add_node(c_ui32 a_ui32Function, node_function_i* const a_pNodeFunction, c_bool a_bNew);

public:
    EMP_RETURN node_c* add_function(c_astring& a_sExpression, c_info_s& a_rInfo);
    EMP_RETURN node_c* add_envelope(c_info_s& a_rInfo);
    EMP_RETURN node_c* add_spectrum(c_info_s& a_rInfo);
    EMP_RETURN node_c* add_biquad(c_info_s& a_rInfo);

public:
    EMP_RETURN node_function_i* new_node_function(c_ui32 a_ui32Function);
    EMP_RETURN node_function_i* new_envelope_function(c_ui32 a_ui32Function);
    EMP_RETURN node_function_i* new_spectrum_function(c_ui32 a_ui32Function);
    EMP_RETURN node_function_i* new_biquad_function(c_ui32 a_ui32Function);

private:
    EMP_RETURN info_s get_info(void);

public:
    EMP_RETURN return_e on_new_function(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_scale(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_envelope(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_spectrum(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_biquad(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_delay(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);
    EMP_RETURN return_e on_new_reverb(c_size a_stIndex, c_bool a_bValue, c_change_e a_eChange);

public:
    EMP_RETURN return_e on_pinout_push(cpc_control_i a_pSender, c_bool a_bPressed, c_bool a_bReleased);

public:
    EMP_RETURN registrar_i* registrar(void) { return m_pRegistrar; }

public:
    void function_removed(c_ui32 a_ui32Function);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

