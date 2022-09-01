//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_SKIN_H
#define EMP_GUI_SKIN_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gfx { EMP_FORWARD(class, model_c) } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_FORWARD(struct, button_skin_s)
EMP_FORWARD(struct, color_s)
EMP_FORWARD(struct, color_data_s)
EMP_FORWARD(struct, container_skin_s)
EMP_FORWARD(struct, control_skin_s)
EMP_FORWARD(struct, coordinate_skin_s)
EMP_FORWARD(struct, dialog_skin_s)
EMP_FORWARD(struct, font_data_s)
EMP_FORWARD(struct, frame_skin_s)
EMP_FORWARD(struct, function_skin_s)
EMP_FORWARD(struct, glyph_skin_s)
EMP_FORWARD(struct, graph_skin_s)
EMP_FORWARD(struct, icon_skin_s)
EMP_FORWARD(struct, label_skin_s)
EMP_FORWARD(struct, line_skin_s)
EMP_FORWARD(class , list_skin_c)
EMP_FORWARD(struct, menu_skin_s)
EMP_FORWARD(struct, nurbs_skin_s)
EMP_FORWARD(struct, outline_skin_s)
EMP_FORWARD(struct, padding_data_s)
EMP_FORWARD(struct, panel_skin_s)
EMP_FORWARD(struct, picture_skin_s)
EMP_FORWARD(struct, plot_skin_s)
EMP_FORWARD(struct, scroller_skin_s)
EMP_FORWARD(struct, sizer_skin_s)
EMP_FORWARD(struct, slider_skin_s)
EMP_FORWARD(struct, splitter_skin_s)
EMP_FORWARD(struct, state_data_s)
EMP_FORWARD(struct, text_skin_s)
EMP_FORWARD(struct, toggle_skin_s)
EMP_FORWARD(struct, toolbar_skin_s)
EMP_FORWARD(struct, window_skin_s)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(skin_data_s)
public:
    astring name{};

    emp::gfx::font_c* font_title = nullptr;
    emp::gfx::font_c* font_text = nullptr;

    float font_size_text = 1;
    float font_size_button = 1;
    float font_size_column = 1;
    float font_size_menu = 1;
    float font_size_toolbar = 1;
    float font_size_h4 = 1;
    float font_size_h3 = 1;
    float font_size_h2 = 1;
    float font_size_h1 = 1;

    bool random_link = 0;

    v4f a20{};
    v4f a25{};
    v4f a38{};
    v4f a50{};
    v4f a75{};
    v4f dark{};
    v4f light{};
    v4f control{};
    v4f text{};
    v4f window{};
    v4f link{};
    v4f black{};
    v4f zero{};
    v4f white{};
    v4f red{};
    v4f green{};
    v4f blue{};
    v4f yellow{};
    v4f pink{};
    v4f orange{};

    ui32 ui32_link = 0;

    emp::gfx::model_c* gfx_minus = nullptr;
    emp::gfx::model_c* gfx_plus = nullptr;
    emp::gfx::model_c* gfx_up = nullptr;
    emp::gfx::model_c* gfx_down = nullptr;
    emp::gfx::model_c* gfx_left = nullptr;
    emp::gfx::model_c* gfx_right = nullptr;
    emp::gfx::model_c* gfx_scroll = nullptr;
    emp::gfx::model_c* gfx_on = nullptr;
    emp::gfx::model_c* gfx_off = nullptr;
    emp::gfx::model_c* gfx_edit = nullptr;
    emp::gfx::model_c* gfx_check = nullptr;

    v2f spacing{};

public:
    EMP_RETURN bool post_load(void);

    EMP_RETURN v4f dark_dark(void) const { return v4f::mul(dark, dark); }
    EMP_RETURN v4f light_light(void) const { return v4f::mul(light, light); }

    EMP_RETURN v4f control_l1(void) const { return v4f::mul(control, light); }
    EMP_RETURN v4f control_l2(void) const { return v4f::mul(control_l1(), light); }
    EMP_RETURN v4f control_l3(void) const { return v4f::mul(control_l2(), light); }

    EMP_RETURN v4f link_d1(void) const { return v4f::mul(link, dark); }
    EMP_RETURN v4f link_d1_a20(void) const { return v4f::mul(link_d1(), a20); }
    EMP_RETURN v4f link_d1_a38(void) const { return v4f::mul(link_d1(), a38); }

    EMP_RETURN v4f link_d2(void) const { return v4f::mul(link, dark_dark()); }

    EMP_RETURN v4f link_d3(void) const { return v4f::mul(link_d1(), link_d2()); }

    EMP_RETURN v4f link_l1(void) const { return v4f::mul(link, light); }
    EMP_RETURN v4f link_l2(void) const { return v4f::mul(link, light_light()); }

    EMP_RETURN v4f link_a20(void) const { return v4f::mul(link, a20); }
    EMP_RETURN v4f link_a38(void) const { return v4f::mul(link, a38); }
    EMP_RETURN v4f link_a50(void) const { return v4f::mul(link, a50); }

    EMP_RETURN v4f gray_10_a75(void) const { return v4f::mul(gray_x(0.1f), a75); }

    EMP_RETURN v4f black_a50(void) const { return v4f::mul(black, a50); }

    EMP_RETURN v4f gray_x(c_float a_fPercent) const { return v4f::mul(white, {a_fPercent, a_fPercent, a_fPercent, 1.0f}); }

    EMP_RETURN v4f red_a25(void) const { return v4f::mul(red, a25); }
    EMP_RETURN v4f red_a50(void) const { return v4f::mul(red, a50); }

    EMP_RETURN v4f green_a25(void) const { return v4f::mul(green, a25); }
    EMP_RETURN v4f green_a50(void) const { return v4f::mul(green, a50); }

    EMP_RETURN v4f blue_a50(void) const { return v4f::mul(blue, a50); }

    EMP_RETURN v4f yellow_a50(void) const { return v4f::mul(yellow, a50); }

    EMP_RETURN v4f orange_a50(void) const { return v4f::mul(orange, a50); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, skin_t)
private:
    astring name;
    emp::cnt::map_t<astring, T*> skins;

public:
    skin_t(c_astring& a_sName);
    ~skin_t(void);

public:
    void destroy(void);

    EMP_RETURN T* allocate(c_astring& a_sName);
    EMP_RETURN T* get(c_astring& a_sName, c_bool a_bFail = true) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
skin_t<T>::skin_t(c_astring& a_sName) :
    name(a_sName),
    skins(64)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
skin_t<T>::~skin_t(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void skin_t<T>::destroy(void)
{
    for (auto it = skins.begin(); it.not_end(); ++it)
    { EMP_DEALLOCATE(*it.value()); }
    skins.clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN T* skin_t<T>::allocate(c_astring& a_sName)
{
    {
        T* const pSkin = get(a_sName, false);
        if (pSkin != nullptr)
        {
            return pSkin;
        }
    }

    EMP_RET_NULL_IF(get(a_sName, false) != nullptr, "Duplicate skin #0.#1", name, a_sName);
    T* const pSkin = EMP_ALLOCATE(T);
    skins.set(a_sName, pSkin);
    return pSkin;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN T* skin_t<T>::get(c_astring& a_sName, c_bool a_bFail) const
{
    auto const& it = skins.get(a_sName);
    if (it.end())
    {
        if (a_bFail && a_sName.len() > 0)
        {
            EMP_LOG_ERROR("Skin not found '#0::#1'", name, a_sName);
            EMP_XX_HOOK()
            return nullptr;
        }
        return nullptr;
    }
    return *it.value();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, skin_c)
public:
    skin_data_s* data = nullptr;

    resolution_data_s resolution{};

    skin_t<button_skin_s> button;
    skin_t<color_s> color;
    skin_t<container_skin_s> container;
    skin_t<control_skin_s> control;
    skin_t<coordinate_skin_s> coordinate;
    skin_t<dialog_skin_s> dialog;
    skin_t<font_data_s> font;
    skin_t<frame_skin_s> frame;
    skin_t<function_skin_s> function;
    skin_t<glyph_skin_s> glyph;
    skin_t<graph_skin_s> graph;
    skin_t<icon_skin_s> icon;
    skin_t<label_skin_s> label;
    skin_t<line_skin_s> line;
    skin_t<list_skin_c> list;
    skin_t<menu_skin_s> menu;
    skin_t<toolbar_skin_s> toolbar;
    skin_t<nurbs_skin_s> nurbs;
    skin_t<outline_skin_s> outline;
    skin_t<padding_data_s> padding;
    skin_t<panel_skin_s> panel;
    skin_t<picture_skin_s> picture;
    skin_t<plot_skin_s> plot;
    skin_t<scroller_skin_s> scroller;
    skin_t<sizer_skin_s> sizer;
    skin_t<slider_skin_s> slider;
    skin_t<splitter_skin_s> splitter;
    skin_t<text_skin_s> text;
    skin_t<toggle_skin_s> toggle;
    skin_t<window_skin_s> window;

public:
    skin_c(void);
    ~skin_c(void);

public:
    EMP_RETURN bool post_load(void);

public:
    EMP_RETURN bool create(pc_skin_data_s a_pData);

public:
    template <typename T>
    EMP_RETURN T* get(c_astring& a_sName) const;

    EMP_RETURN float spacing_x(void) const;
    EMP_RETURN float spacing_y(void) const;

    EMP_RETURN float padding_x(c_float a_fValue, c_bool a_bScale) const;
    EMP_RETURN float padding_y(c_float a_fValue, c_bool a_bScale) const;

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_label_color(c_label_skin_s& a_rSkin, c_state_data_s& a_rState, c_bool a_bEnable, color_s& a_rMaterial);
void compute_picture_color(c_picture_skin_s& a_rSkin, c_state_data_s& a_rState, c_bool a_bEnable, color_s& a_rColor);
void compute_container_color(c_container_skin_s& a_rSkin, c_state_data_s& a_rState, c_state_data_s& a_rChild, c_bool a_bEnable, color_data_s& a_rColor);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN skin_c& skin(void);
#define EMP_GUI_SKIN emp::gui::skin()
#define EMP_GUI_SKIN_(x_Type, x_Name) EMP_GUI_SKIN.x_Type.get(ALITERAL(x_Name))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

