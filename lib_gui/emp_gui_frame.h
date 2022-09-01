//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_FRAME_H
#define EMP_GUI_FRAME_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_container.h"
#include "emp_gui_label.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { template <typename...> class buffer_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct spike_c
{
    float position;
    float value;
    float alpha;
    float label;
    float size;
    bool decade;
    bool visible;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(frame_data_s)
public:
    EMP_FIELD( float               , scale         , 1.0f             )
    EMP_FIELD( float               , xscale        , 1.0f             )
    EMP_FIELD( float               , yscale        , 1.0f             )
    EMP_FIELD( v2f                 , min           , { -1024, -1024 } )
    EMP_FIELD( v2f                 , max           , {  1024,  1024 } )
    EMP_FIELD( bool                , show_position , true             )
    EMP_FIELD( bool                , show_basis    , true             )
    EMP_FIELD( bool                , show_axis     , true             )
    EMP_FIELD( bool                , show_cursor   , true             )
    EMP_FIELD( bool                , show_spike    , true             )
    EMP_FIELD( bool                , show_line     , true             )
    EMP_FIELD( bool                , xlog          , false            )
    EMP_FIELD( bool                , ylog          , false            )

public:
    EMP_RETURN float final_scale_x(void) const;
    EMP_RETURN float final_scale_y(void) const;

public:
    static constexpr float delta_scale(void) { return 0.03f; }
    static constexpr float min_scale(void) { return 0.0001f; }

    void scale_up(void) { scale += scale * delta_scale(); }
    void scale_down(void)
    {
        scale -= scale * delta_scale();
        scale = tt::maxof(min_scale(), scale);
    }

    void xscale_up(void) { xscale += xscale * delta_scale(); }
    void xscale_down(void)
    {
        xscale -= xscale * delta_scale();
        xscale = tt::maxof(min_scale(), xscale);
    }

    void yscale_up(void) { yscale += yscale * delta_scale(); }
    void yscale_down(void)
    {
        yscale -= yscale * delta_scale();
        yscale = emp::tt::maxof(min_scale(), yscale);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(frame_skin_s)
public:
    EMP_FIELD( c_container_skin_s*    , container   , nullptr       )
    EMP_FIELD( c_color_s*             , unit_line   , nullptr       )
    EMP_FIELD( c_color_s*             , unit_spike  , nullptr       )
    EMP_FIELD( c_color_s*             , origin_x    , nullptr       )
    EMP_FIELD( c_color_s*             , origin_y    , nullptr       )
    EMP_FIELD( c_color_s*             , cursor_x    , nullptr       )
    EMP_FIELD( c_color_s*             , cursor_y    , nullptr       )
    EMP_FIELD( c_color_s*             , axis_x      , nullptr       )
    EMP_FIELD( c_color_s*             , axis_y      , nullptr       )
    EMP_FIELD( c_label_skin_s*        , position_x   ,{}               )
    EMP_FIELD( c_label_skin_s*        , position_y    ,{}              )
    EMP_FIELD( float                  , unit_x           , 1        )
    EMP_FIELD( float                  , unit_y           , 1        )
    EMP_FIELD( float                  , unit_size_x      , 0.06f    )
    EMP_FIELD( float                  , unit_size_y      , 0.06f    )
    EMP_FIELD( v2f                    , min_domain       , {-5, -5} )
    EMP_FIELD( v2f                    , max_domain       , { 5,  5} )
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, frame_c, public, container_c)
public:
    EMP_FIELD(on_render_f<frame_c const&>, on_render);
    EMP_FIELD(frame_data_s, data)

private:
    v2f m_v2Origin;

    bool m_bDrag = false;
    v2f m_v2DragOrigin;
    v2f m_v2DragDelta;

    label_c position_x{};
    label_c position_y{};

    v2f m_v2Position{};
    v2f m_v2ClientPosition{};

    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pOriginUnitX;
    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pOriginUnitY;
    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pOriginDecadeX;
    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pOriginDecadeY;
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pCursorX;
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pCursorY;
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pAxisX;
    EMP_POINTER(emp::gfx::buffer_t<v3f>) m_pAxisY;
    EMP_POINTER(emp::gfx::buffer_t<v3f, v4f>) m_pMesh;

    vector<spike_c> m_vUnitX;
    vector<spike_c> m_vUnitY;

    m44f m_mModel;

    int m_siDomainX = 0;
    int m_siDomainY = 0;

    float m_fAlphaDecadeX = 1;
    float m_fAlphaDecadeY = 1;

    c_frame_skin_s* skin = nullptr;

public:
    frame_c(void);
    virtual ~frame_c(void);

public:
    //>> control_i
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

public:
    //>> handler_i
    EMP_RETURN return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    void scale(c_bool a_bUp, c_bool a_bX, c_bool a_bY);

    EMP_RETURN vector<spike_c> const& units_x(void) const { return m_vUnitX; }
    EMP_RETURN vector<spike_c> const& units_y(void) const { return m_vUnitY; }

    EMP_RETURN c_m44f& model(void) const { return m_mModel; }

public:
    EMP_RETURN c_v2f client_position(void) const { return m_v2ClientPosition; }
    EMP_RETURN c_v2f client_to_frame(c_v2f& a_v2Value) const;
    EMP_RETURN c_v2f frame_to_client(c_v2f& a_v2Value) const;
    EMP_RETURN c_v2f origin(void) const { return m_v2Origin; }

    void move_origin(c_v2f& a_v2Delta) { m_v2Origin = move_point(m_v2Origin, a_v2Delta); }

    void set_origin(c_v2f& a_v2Value)
    {
        // @@0  : va + vb ?
        move_origin(emp::mat::v2f::neg(m_v2Origin));
        move_origin(a_v2Value);
    }

    void center_origin(void) { set_origin(v2f::half(transformation().size)); }

    EMP_RETURN c_v2f min_visible(void) const { return client_to_frame(v2f::zero()); }
    EMP_RETURN c_v2f max_visible(void) const { return client_to_frame(transformation().size); }

private:
    EMP_RETURN c_v2f move_point(c_v2f& a_v2Point, c_v2f& a_v2Delta) const;
    EMP_RETURN c_v2f get_point_size(void) const { return get_frame_size(emp::mat::v2f::all(0.02f)); }
    EMP_RETURN c_v2f get_frame_size(c_v2f& a_v2Client) const;
    EMP_RETURN c_v2f get_frame_delta(c_size a_stIndex, c_v2f& a_v2Delta);

public:
    frame_c& skin_(cpc_frame_skin_s a_pValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

