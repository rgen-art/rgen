//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_PLOT_H
#define EMP_GUI_PLOT_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_coordinate.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(plot_skin_s)
public:
    EMP_FIELD(c_coordinate_skin_s* , coordinate  , nullptr)
    EMP_FIELD(c_color_s*           , line        , nullptr)
    EMP_FIELD(c_color_s*           , plot_hover  , nullptr)
    EMP_FIELD(c_color_s*           , plot_normal , nullptr)
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, plot_c, public, coordinate_c)
public:
    typedef emp::fct::function_t<size_t (*)(c_size, v2f::tparam)> on_point_moved_d;
    typedef emp::fct::function_t<void (*)(c_size, v2f::tparam)> on_point_added_d;
    typedef emp::fct::function_t<void (*)(c_size)> on_point_removed_d;
    typedef emp::fct::function_t<void (*)(c_size)> on_point_right_clicked_d;

    on_render_f<plot_c const&> on_render{};
    on_point_moved_d on_point_moved{};
    on_point_added_d on_point_added{};
    on_point_removed_d on_point_removed{};
    on_point_right_clicked_d on_point_right_clicked{};

    vector<v2f::type> points{};

private:
    size_t EMP_TT_MAX_VAR(m_stPlot);
    v2f m_v2PointDelta{};
    v2f m_v2PointOrigin{};

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pQuadMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pQuadMeshIndices;

    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pLineMeshVertices;
    EMP_POINTER(emp::gfx::buffer_t<ui16>) m_pLineMeshIndices;
    

    c_plot_skin_s* skin = nullptr;

public:
    plot_c(void);
    virtual ~plot_c(void);

public:
    //> control_i
    EMP_RETURN bool transform(stack_c& a_rStack);
    //< control_i

    //> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //< handler_i

public:
    EMP_RETURN bool auto_resize(void);

private:
    void render_plot(frame_c const& a_pSender);

private:
    EMP_RETURN size_t move_point(c_size a_stIndex, v2f::tparam a_v2Delta);
    void add_point(c_size a_stIndex, v2f::tparam a_v2Position);
    void remove_point(c_size a_stIndex);

    label_c* get_label(c_size a_stIndex, c_bool a_bX);

public:
    EMP_RETURN c_v2f point_size(void) const;

private:
    EMP_RETURN c_v2f get_point_size(void) const;
    EMP_RETURN c_v2f get_point_size(v2f::tparam a_v2Client) const;
    EMP_RETURN c_v2f get_point_delta(c_size a_stIndex, v2f::tparam a_v2Delta);

public:
    plot_c& skin_(cpc_plot_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

