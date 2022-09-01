//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_skin.h"

#include "emp_gui_container.h"
#include "emp_gui_coordinate.h"
#include "emp_gui_dialog.h"
#include "emp_gui_frame.h"
#include "emp_gui_function.h"
#include "emp_gui_glyph.h"
#include "emp_gui_graph.h"
#include "emp_gui_icon.h"
#include "emp_gui_list.h"
#include "emp_gui_manager.h"
#include "emp_gui_nurbs.h"
#include "emp_gui_menu.h"
#include "emp_gui_outline.h"
#include "emp_gui_panel.h"
#include "emp_gui_picture.h"
#include "emp_gui_plot.h"
#include "emp_gui_resolution.h"
#include "emp_gui_scroller.h"
#include "emp_gui_slider.h"
#include "emp_gui_spacer.h"
#include "emp_gui_splitter.h"
#include "emp_gui_text.h"
#include "emp_gui_toggle.h"
#include "emp_gui_toolbar.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h" // @@0 cyclic

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN EMP_INTERN emp::mat::v3f::tret rgb_to_hsv(emp::mat::v3f::tparam a_v3Value);
EMP_RETURN EMP_INTERN emp::mat::v3f::tret rgb_to_hsv(emp::mat::v3f::tparam a_v3Value)
{
    c_float fR = a_v3Value.x;
    c_float fG = a_v3Value.y;
    c_float fB = a_v3Value.z;

    c_float fMax = emp::tt::maxof(fR, emp::tt::maxof(fG, fB));
    c_float fMin = emp::tt::minof(fR, emp::tt::minof(fG, fB));

    float fH = 0.0f;
    float fS = 0.0f;
    float const fV = fMax;

    if (emp::tt::not_equal_(fMax, 0.0f))
    { fS = (fMax - fMin) / fMax; }
    else
    { fS = 0.0f; }

    if (emp::tt::equal_(fS, 0.0f))
    {
        // fH = 0.0f; // @@0 leggit ?
        return emp::mat::v3f::zero();
    }

    float const fDelta = fMax - fMin;

    if (emp::tt::equal_(fR, fMax))
    { fH = (fG - fB) / fDelta; }
    else if (emp::tt::equal_(fG, fMax))
    { fH = 2.0f + (fB - fR) / fDelta; }
    else if (emp::tt::equal_(fB, fMax))
    { fH = 4.0f + (fR - fG) / fDelta; }
    fH = fH * 60.0f;
    if (fH < 0.0)
    { fH += 360.0f; }
    return { fH, fS, fV };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN EMP_INTERN emp::mat::v3f::tret hsv_to_rgb(emp::mat::v3f::tparam a_v3Value);
EMP_RETURN EMP_INTERN emp::mat::v3f::tret hsv_to_rgb(emp::mat::v3f::tparam a_v3Value)
{
    float fH = a_v3Value.x;
    c_float fS = a_v3Value.y;
    c_float fV = a_v3Value.z;

    float fR = 0.0f;
    float fG = 0.0f;
    float fB = 0.0f;

    if(emp::tt::equal_(fS, 0.0f))
    {
        fR = fV;
        fG = fV;
        fB = fV;
    }
    else
    {
        fH /= 60;
        c_int i = emp::ast::cast<int>(floor(fH));
        c_float f = fH - emp::ast::cast<float>(i);
        c_float p = fV * (1 - fS);
        c_float q = fV * (1 - fS * f);
        c_float t = fV * (1 - fS * (1 - f));
        switch (i)
        {
            case 0:
                fR = fV;
                fG = t;
                fB = p;
            break;
            case 1:
                fR = q;
                fG = fV;
                fB = p;
            break;
            case 2:
                fR = p;
                fG = fV;
                fB = t;
            break;
            case 3:
                fR = p;
                fG = q;
                fB = fV;
            break;
            case 4:
                fR = t;
                fG = p;
                fB = fV;
            break;
            default:
                fR = fV;
                fG = p;
                fB = q;
            break;
        }
    }
    return { fR, fG, fB };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v4f color_s::light(emp::mat::v4f::tparam a_v4Value)
{
    emp::mat::v3f v3Color = emp::mat::v4f::xyz(a_v4Value);
/*    EMP_LOG_INFO("-------------------------------------");
    EMP_LOG_INFO("RGB #0, #1, #2",
        emp::mat::v3f::get_x(v3Color),
        emp::mat::v3f::get_y(v3Color),
        emp::mat::v3f::get_z(v3Color));*/

    v3Color = rgb_to_hsv(v3Color);
/*    EMP_LOG_INFO("HSV #0, #1, #2",
        emp::mat::v3f::get_x(v3Color),
        emp::mat::v3f::get_y(v3Color),
        emp::mat::v3f::get_z(v3Color));*/

    c_float fLight = 0.5f;

    c_float fOldS = v3Color.y;
    c_float fOldV = v3Color.z;

    c_float fH = v3Color.x;
    c_float fV = emp::tt::minof(1.0f, fOldV + fLight);
    c_float fS = emp::tt::maxof(0.0f, fOldS - (fLight - (fV - fOldV)));

    v3Color = { fH, fS, fV };
/*    EMP_LOG_INFO("HSV #0, #1, #2",
        emp::mat::v3f::get_x(v3Color),
        emp::mat::v3f::get_y(v3Color),
        emp::mat::v3f::get_z(v3Color));*/

   v3Color = hsv_to_rgb(v3Color);
/*    EMP_LOG_INFO("RGB #0, #1, #2",
        emp::mat::v3f::get_x(v3Color),
        emp::mat::v3f::get_y(v3Color),
        emp::mat::v3f::get_z(v3Color));*/

    return emp::mat::v4f::all(v3Color, 1.0f);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool skin_data_s::post_load(void)
{
    if (random_link)
    {
        random_c oRandom;
        while (true)
        {
            c_float fR = oRandom.rand<float>(0, 1);
            c_float fG = oRandom.rand<float>(0, 1);
            c_float fB = oRandom.rand<float>(0, 1);

            if (emp::mat::abs(fR - fG) < 0.2f &&
                emp::mat::abs(fG - fB) < 0.2f)
            { continue; }

            float const fM = (fR + fG + fB);
            if (fM < 1.20f)
            { continue; }

            if (fM > 2.0f)
            { continue; }

            link =  { fR, fG, fB, 1.0f };
            break;
        }

        ui32_link = emp::ast::cast<ui32>(
            ((emp::ast::cast<si64>(link.w * 255) & 0xFF) << 24) |
            ((emp::ast::cast<si64>(link.x * 255) & 0xFF) << 16) |
            ((emp::ast::cast<si64>(link.y * 255) & 0xFF) <<  8) |
            ((emp::ast::cast<si64>(link.z * 255) & 0xFF) <<  0));

//m_v4Link = v4f::make(0.6523437, 0.7773437, 0.1250000, 1.0000000);
//m_v4Link = v4f::make(0.8984375, 0.0273437, 0.2187500, 1.0000000);
//m_v4Link = v4f::make(0.4062500, 0.7734375, 0.1953125, 1.0000000);
//m_v4Link = v4f::make(0.4570312, 0.0195312, 0.5820312, 1.0000000);
//m_v4Link = v4f::make(0.3164062, 0.1210937, 0.5390625, 1.0000000);
//m_v4Link = v4f::make(0.3632812, 0.5898437, 0.0507812, 1.0000000);
//m_v4Link = v4f::make(0.6757812, 0.3750000, 0.1328125, 1.0000000);
//m_v4Link = v4f::make(0.3281250, 0.7773437, 0.0976562, 1.0000000);
//m_v4Link = v4f::make(0.9921875, 0.0625000, 0.1445312, 1.0000000);
//m_v4Link = v4f::make(0.2421875, 0.8437500, 0.7382812, 1.0000000);
//m_v4Link = v4f::make(0.0234375, 0.0976562, 0.8281250, 1.0000000);
//m_v4Link = v4f::make(0.9062500, 0.4062500, 0.5625000, 1.0000000);
//m_v4Link = v4f::make(0.6093750, 0.0898437, 0.0546875, 1.0000000);
//m_v4Link = v4f::make(0.1835937, 0.9179687, 0.1796875, 1.0000000);
//m_v4Link = v4f::make(0.8007812, 0.8398437, 0.0273437, 1.0000000);
//m_v4Link = v4f::make(0.9687500, 0.3398437, 0.6523437, 1.0000000);
//m_v4Link = v4f::make(0.4296875, 0.9414062, 0.1328125, 1.0000000);
//m_v4Link = v4f::make(0.9765625, 0.3867187, 0.0234375, 1.0000000);
//m_v4Link = v4f::make(0.6953125, 0.8242187, 0.0664062, 1.0000000);
//m_v4Link = v4f::make(0.8359375, 0.2070312, 0.2539062, 1.0000000);
//m_v4Link = v4f::make(0.9101562, 0.6054687, 0.0625000, 1.0000000);
    EMP_LOG_INFO("m_v4Link = v4f::make(#0, #1, #2, #3);", link.x, link.y, link.z, link.w);
    }

    //m_v4Link = m_v4Orange;
    return true;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
skin_c::skin_c( void):
    button(ALITERAL("emp::gui::button_skin_s")),
    color(ALITERAL("emp::gui::color_skin_c")),
    container(ALITERAL("emp::gui::container_skin_s")),
    control(ALITERAL("emp::gui::control_skin_s")),
    coordinate(ALITERAL("emp::gui::coordinate_skin_s")),
    dialog(ALITERAL("emp::gui::dialog_skin_s")),
    font(ALITERAL("emp::gui::font_data_s")),
    frame(ALITERAL("emp::gui::frame_skin_s")),
    function(ALITERAL("emp::gui::function_skin_s")),
    glyph(ALITERAL("emp::gui::glyph_skin_s")),
    graph(ALITERAL("emp::gui::graph_skin_s")),
    icon(ALITERAL("emp::gui::icon_skin_s")),
    label(ALITERAL("emp::gui::label_skin_s")),
    line(ALITERAL("emp::gui::line_skin_s")),
    list(ALITERAL("emp::gui::list_skin_c")),
    menu(ALITERAL("emp::gui::menu_skin_s")),
    toolbar(ALITERAL("emp::gui::toolbar_skin_s")),
    nurbs(ALITERAL("emp::gui::nurbs_skin_s")),
    outline(ALITERAL("emp::gui::outline_skin_s")),
    padding(ALITERAL("emp::gui::padding_data_s")),
    panel(ALITERAL("emp::gui::panel_skin_s")),
    picture(ALITERAL("emp::gui::picture_skin_s")),
    plot(ALITERAL("emp::gui::plot_skin_s")),
    scroller(ALITERAL("emp::gui::scroller_skin_s")),
    sizer(ALITERAL("emp::gui::sizer_skin_s")),
    slider(ALITERAL("emp::gui::slider_skin_s")),
    splitter(ALITERAL("emp::gui::splitter_skin_s")),
    text(ALITERAL("emp::gui::text_skin_s")),
    toggle(ALITERAL("emp::gui::toggle_skin_s")),
    window(ALITERAL("emp::gui::window_skin_s"))
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
skin_c::~skin_c(void)
{ }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN float scale(c_float a_fValue);
EMP_INTERN EMP_RETURN float scale(c_float a_fValue)
{ return a_fValue < 1 ? a_fValue : 1 + logf(a_fValue) * 0.5f; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float skin_c::spacing_x(void) const
{ return padding_x(data->spacing.x, false); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float skin_c::spacing_y(void) const
{ return padding_y(data->spacing.y, false); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float skin_c::padding_x(c_float a_fValue, c_bool a_bScale) const
{ return a_fValue * resolution.ratio().x * (a_bScale ? scale(resolution.scale()) : 1.0f); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float skin_c::padding_y(c_float a_fValue, c_bool a_bScale) const
{ return a_fValue * resolution.ratio().y * (a_bScale ? scale(resolution.scale()) : 1.0f); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool skin_c::post_load(void)
{ return create(data); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool skin_c::create(pc_skin_data_s a_pValue)
{
    data = a_pValue;
    EMP_RET_FALSE_IF(data == nullptr);

#define XTMP_UI_(x_Store, x_Name) x_Store.allocate(ALITERAL(x_Name))
#define XTMP_UI(x_Var, x_Store, x_Name) auto* const x_Var = XTMP_UI_(x_Store, x_Name); (*x_Var)

    //-----------------------------------------------------------------------------
    //color
    //-----------------------------------------------------------------------------
    #define XTMP_COL(x_Name, x_Rgba) XTMP_UI_(color, x_Name)->rgba_(x_Rgba);

    XTMP_COL( "col.black_a50"  , data->black_a50()   ) //-V522
    XTMP_COL( "col.control"    , data->control       )
    XTMP_COL( "col.control_l1" , data->control_l1()  )
    XTMP_COL( "col.control_l2" , data->control_l2()  )
    XTMP_COL( "col.control_l3" , data->control_l3()  )
    XTMP_COL( "col.gray10"     , data->gray_x(0.1f)  )
    XTMP_COL( "col.gray20"     , data->gray_x(0.2f)  )
    XTMP_COL( "col.gray30"     , data->gray_x(0.3f)  )
    XTMP_COL( "col.gray40"     , data->gray_x(0.4f)  )
    XTMP_COL( "col.gray50"     , data->gray_x(0.5f)  )
    XTMP_COL( "col.green"      , data->green         )
    XTMP_COL( "col.green_a25"  , data->green_a25()   )
    XTMP_COL( "col.link"       , data->link          )
    XTMP_COL( "col.link_l1"    , data->link_l1()     )
    XTMP_COL( "col.link_a20"   , data->link_a20()    )
    XTMP_COL( "col.link_a38"   , data->link_a38()    )
    XTMP_COL( "col.link_a50"   , data->link_a50()    )
    XTMP_COL( "col.link_d1"    , data->link_d1()     )
    XTMP_COL( "col.link_d1_a20", data->link_d1_a20() )
    XTMP_COL( "col.link_d1_a38", data->link_d1_a38() )
    XTMP_COL( "col.link_d2"    , data->link_d2()     )
    XTMP_COL( "col.link_d3"    , data->link_d3()     )
    XTMP_COL( "col.pink"       , data->pink          )
    XTMP_COL( "col.red"        , data->red           )
    XTMP_COL( "col.red_a25"    , data->red_a25()     )
    XTMP_COL( "col.red_a50"    , data->red_a50()     )
    XTMP_COL( "col.text"       , data->text          )
    XTMP_COL( "col.window"     , data->window        )
    XTMP_COL( "col.yellow_a50" , data->yellow_a50()  )

    //-----------------------------------------------------------------------------
    //control
    //-----------------------------------------------------------------------------
    #define XTMP_CTRL(x_Name, x_Raster, x_Border) XTMP_UI_(control, x_Name)->raster_(color.get(ALITERAL(x_Raster))).border_(color.get(ALITERAL(x_Border)));

    XTMP_CTRL("ctl.window"      , "col.window"    , "col.control_l3" )
    XTMP_CTRL("ctl.control"     , "col.control"   , "col.control_l3" )
    XTMP_CTRL("ctl.focus"       , "col.link_d2"   , "col.link_d1"    )
    XTMP_CTRL("ctl.select"      , "col.link_d3"   , "col.link_d2"    )
    XTMP_CTRL("ctl.hover"       , "col.link_a20"  , "col.link_a38"   )
    XTMP_CTRL("ctl.check"       , ""              , ""               )
    XTMP_CTRL("ctl.modal"       , "col.black_a50" , ""               )
    XTMP_CTRL("ctl.debug"       , "col.yellow_a50", "col.pink"       )
    XTMP_CTRL("ctl.border"      , ""              , "col.control_l3" )
    XTMP_CTRL("ctl.window_hover", ""              , "col.link_a38"   )
    #undef XTMP_CTRL
    //-----------------------------------------------------------------------------
    //container
    //-----------------------------------------------------------------------------
    //#define XTMP_CNT(x_Var, x_Name, x_Normal) XTMP_UI(x_Var, container, x_Name).enabled_background.normal_(x_Normal);
    /*
    #define XTMP_CNT_(x_Var, x_Name, x_EnBaNormal, x_EnBaFocus, x_EnBaSelect, x_EnFoHover, x_EnFoFocus, x_EnFoSelect)\
        XTMP_CNT(x_Var, x_Name, x_EnBaNormal);\
        x_Var->enabled_background.focus_(x_EnBaFocus).select_(x_EnBaSelect);\
        x_Var->enabled_foreground.hover_(x_EnFoHover).focus_(x_EnFoFocus).select_(x_EnFoSelect);
*/
    #define XTMP_CNT(x_Name, x_EnBaNormal, x_EnBaFocus, x_EnBaSelect, x_EnBaHover, x_EnFoNormal, x_EnFoFocus, x_EnFoSelect, x_EnFoHover)\
        {\
            auto * const pCtrl = XTMP_UI_(container, x_Name);\
            pCtrl->enabled_background\
                .normal_( control.get(ALITERAL(x_EnBaNormal)))\
                .focus_(  control.get(ALITERAL(x_EnBaFocus)))\
                .select_( control.get(ALITERAL(x_EnBaSelect)))\
                .hover_(  control.get(ALITERAL(x_EnBaHover)));\
            pCtrl->enabled_foreground\
                .normal_( control.get(ALITERAL(x_EnFoNormal)))\
                .focus_(  control.get(ALITERAL(x_EnFoFocus)))\
                .select_( control.get(ALITERAL(x_EnFoSelect)))\
                .hover_(  control.get(ALITERAL(x_EnFoHover)));\
        }

XTMP_CNT("cnt.debug"        ,"ctl.debug"  ,""          ,""          ,"" ,"",""          ,""                 ,""          )
XTMP_CNT("cnt.modal"        ,"ctl.modal"  ,""          ,""          ,"" ,"",""          ,""                 ,""          )
XTMP_CNT("cnt.window"       ,"ctl.window" ,""          ,""          ,"" ,"",""          ,""                 ,"" )
XTMP_CNT("cnt.column"       ,"ctl.control","ctl.focus" ,"ctl.select","" ,"",""          ,""                 ,"ctl.hover" )
XTMP_CNT("cnt.border"       ,"ctl.border" ,""          ,""          ,"" ,"",""          ,""                 ,""          )

//XTMP_CNT("cnt.header"       ,""                  ,""                    ,"ctl.link_a38."        ,"ctl.select_a38."          ,"",""          ,""                 ,"")//"ctl.hover_a20.hover_a38"   )
//XTMP_CNT("cnt..border"      ,"ctl..border"       ,""                    ,""                     ,""                         ,"",""          ,""                 ,""                          )

/*
XTMP_CNT("cnt."             ,"ctl."              ,"ctl."               ,"ctl."                 ,"ctl."                   ,"ctl.","ctl."     ,"ctl."             ,"ctl."                      )
XTMP_CNT("cnt.edit"         ,"ctl.white.link"    ,"ctl."               ,"ctl."                 ,"ctl."                   ,"ctl.","ctl."     ,"ctl."             ,"ctl."                      )
XTMP_CNT("cnt.hover"        ,"ctl."              ,"ctl."               ,"ctl."                 ,"ctl."                   ,"ctl.","ctl."     ,"ctl."             ,"ctl.hover_a20.hover_a38"   )
XTMP_CNT("cnt.window.border","ctl.window."       ,"ctl."               ,"ctl."                 ,"ctl."                   ,"ctl.","ctl."     ,"ctl."             ,"ctl.hover_a20.hover_a38"   )

XTMP_CNT("cnt.keyboard"     ,"ctl.back.border"   ,"ctl."               ,"ctl."                 ,"ctl.hover_a20.hover_a38","ctl.","ctl."     ,"ctl.link_a38.link","ctl."                      )
XTMP_CNT("cnt.node_header"  ,"ctl."              ,"ctl."               ,"ctl..hover_a38"       ,"ctl."                   ,"ctl.","ctl."     ,"ctl."             ,"ctl."                      )
*/
//XTMP_CNT_(pCntGraphNodeHeader ,"cnt.graph.node.header" ,pCtrlGraphNodeHeader ,nullptr             ,nullptr             ,pCtrlZeroHoverA38    ,nullptr      ,nullptr         );
//XTMP_CNT_(pCntToolbar         ,"cnt.toolbar"           ,pCtrlControlBorder   ,pCtrlLinkA38Border,pCtrlSelectA38Border,pCtrlHoverA20HoverA38,pCtrlZeroLink,nullptr         );
//XTMP_CNT( pCntWindowBorder    ,"cnt.window.border"     ,pCtrlWindowBorder       );
//XTMP_CNT( pCntWindow          ,"cnt.window"            ,pCtrlWindowNull         );
//XTMP_CNT_(pCntKeyboard        ,"cnt.ase.keyboard"      ,pCtrlBackgroundBorder,nullptr             ,nullptr             ,pCtrlHoverA20HoverA38,nullptr      ,pCtrlLinkA38Link);


/*XTMP_CNT( pCntNull            ,"cnt.null"              ,nullptr                 );
XTMP_CNT( pCntBackA50Null     ,"cnt.background_a50"    ,pCtrlBackA50Null        );
XTMP_CNT( pCntBackgroundBlack ,"cnt.black"             ,pCtrlBackNull           );
XTMP_CNT( pCntBackgroundZero  ,"cnt.background"        ,pCtrlBackNull           );
XTMP_CNT( pCntBackgroundAlpha ,"cnt.background_alpha"  ,pCtrlBackA50Null        );
XTMP_CNT( pCntBackgroundBorder,"cnt.background.border" ,pCtrlBackBorder         );
XTMP_CNT( pCntDebug           ,"cnt.debug"             ,pCtrlGreenRed           );
XTMP_CNT( pCntRasterGreenA25  ,"cnt.raster_green_a25"  ,pCtrlGreenA25Null       );
XTMP_CNT( pCntRasterRedA25    ,"cnt.raster_red_a25"    ,pCtrlRedA25Null         );
XTMP_CNT( pCntEdit            ,"cnt.edit"              ,pCtrlWhiteLink          );
XTMP_CNT( pCntHeader          ,"cnt.header"            ,pCtrlNullBorder         );
XTMP_CNT( pCntBorder          ,"cnt.border"            ,pCtrlNullBorder         );
XTMP_CNT( pCntBottomBorderLink,"cnt.bottom.border.link",pCtrlNullLink           );
XTMP_CNT( pCntText            ,"cnt.text"              ,pCtrlNullBorder         );
XTMP_CNT_(pCntHover           ,"cnt.hover"             ,nullptr              ,nullptr             ,nullptr             ,pCtrlHoverA20HoverA38,nullptr      ,nullptr         );
XTMP_CNT_(pCntLineCombo       ,"cnt.line.combo"        ,nullptr              ,pCtrlLinkA38Null    ,pCtrlSelectA38Zero  ,pCtrlHoverA20HoverA38,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntBackgroundLink  ,"cnt.background.link"   ,pCtrlBackgroundBorder,pCtrlBackgroundLink ,nullptr             ,pCtrlZeroHoverA38    ,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntToolbar         ,"cnt.toolbar"           ,pCtrlControlBorder   ,pCtrlLinkA38Border,pCtrlSelectA38Border,pCtrlHoverA20HoverA38,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntGraphNode       ,"cnt.graph.node"        ,pCtrlGraphNode       ,nullptr             ,nullptr             ,pCtrlZeroHoverA38    ,nullptr      ,nullptr         );
XTMP_CNT_(pCntIcon            ,"cnt.icon"              ,pCtrlNull            ,pCtrlLinkA38Border  ,nullptr             ,pCtrlHoverA20HoverA38,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntZeroLink        ,"cnt.zero.link"         ,pCtrlNull            ,pCtrlZeroLink       ,nullptr             ,pCtrlZeroHoverA38    ,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntWindowLink      ,"cnt.window.link"       ,pCtrlWindowBorder    ,pCtrlWindowLink     ,nullptr             ,pCtrlZeroHoverA38    ,pCtrlZeroLink,nullptr         );
XTMP_CNT_(pCntTextHover       ,"cnt.text.hover"        ,pCtrlZeroRightBorder ,pCtrlLinkA38Border  ,nullptr             ,pCtrlHoverA20HoverA38,pCtrlZeroLink,nullptr         );
*/
    #undef XTMP_CNT
    #undef XTMP_CNT_
    //-----------------------------------------------------------------------------
    //spacing
    //-----------------------------------------------------------------------------
    c_float fSX = data->spacing.x;
    c_float fSY = data->spacing.y;

    //-----------------------------------------------------------------------------
    //spacer
    //-----------------------------------------------------------------------------
    #define XTMP_SPE(x_Name, x_Spacing, x_Container) XTMP_UI_(spacer, x_Name)->\
        spacing_(spacing.get(ALITERAL(x_Spacing))).container_(container.get(ALITERAL(x_Container)));

    #undef XTMP_SPE
    //-----------------------------------------------------------------------------
    //padding
    //-----------------------------------------------------------------------------
    #define XTMP_PAD(x_Name, x_Left, x_Right, x_Top, x_Bottom, x_Scale) XTMP_UI_(padding, x_Name)->\
        left_(fSX * x_Left).right_(fSX * x_Right).top_(fSY * x_Top).bottom_(fSY * x_Bottom).scale_(x_Scale);

    XTMP_PAD("pad.column"           , 0.5f, 0.5f, 0.5f, 0.5f, false)
    XTMP_PAD("pad.window"           , 1.0f, 1.0f, 1.0f, 1.0f, false)
    XTMP_PAD("pad.window_noleft"    , 0.0f, 1.0f, 1.0f, 1.0f, false)

    /*
    XTMP_PAD("pad."        , 0.0f, 0.0f, 0.0f, 0.0f)
    XTMP_PAD("pad.title"   , 1.2f, 1.2f, 1.0f, 1.0f)
    XTMP_PAD("pad.window"  , 1.0f, 1.0f, 1.0f, 1.0f)
    XTMP_PAD("pad.keyboard", 3.0f, 3.0f, 0.5f, 0.5f)
    XTMP_PAD("pad.text"    , 0.6f, 0.6f, 0.2f, 0.2f)
    XTMP_PAD("pad.scroller", 0.5f, 0.5f, 0.5f, 0.5f)
    */

//    XTMP_PAD(pPadText           , "text")              .xy_(fSX * 0.6f, fSY * 0.2f);
//    XTMP_PAD(pPadTextLeft       , "text_left")         .xy_(fSX * 0.6f, fSY * 0.2f);
//    XTMP_PAD(pPadNull           , "null")              ;
//    XTMP_PAD(pPad               , "default")           .xy_(fSX       , fSY);
//    XTMP_PAD(pPadTitle          , "title")             .xy_(fSX * 1.2f, fSY * 1.0f);
//    XTMP_PAD(pPadColumn         , "column")            .xy_(fSX * 0.7f, fSY * 0.7f);
//    XTMP_PAD(pPadKeyboard       , "ase.keyboard")      .xy_(fSX * 3.0f, fSY * 0.5f).scale_(true);

/*

    XTMP_PAD(pPadMenu           , "menu")              .xy_(fSX * 0.7f, fSY * 0.7f);
    XTMP_PAD(pPadToolbar        , "toolbar")           .xy_(fSX * 0.7f, fSY * 0.5f);
    XTMP_PAD(pPadScroller       , "scroller")          .xy_(fSX * 0.5f, fSY * 0.5f);
    XTMP_PAD(pPadCover          , "cover")             .xy_(fSX * 1.0f, fSY * 1.0f);
    XTMP_PAD(pPadTextY          , "text.y")            .y_(fSY * 0.2f);
    XTMP_PAD(pPadLeft           , "left")              .left_(fSX);
    XTMP_PAD(pPadTextNoPadLeft  , "text.nopad.left")   .right_(fSY * 0.6f).y_(fSY * 0.2f);
*/
    #undef XTMP_PAD
    //-----------------------------------------------------------------------------
    //sizer
    //-----------------------------------------------------------------------------
    #define XTMP_SZR(x_Name, x_Container, x_Padding) XTMP_UI_(sizer, x_Name)->\
        container_(container.get(ALITERAL(x_Container))).\
        padding_(padding.get(ALITERAL(x_Padding)));

    XTMP_SZR("szr.window"       , "cnt.window"      , "pad.window" )
    //XTMP_SZR("szr.modal"        , "cnt.modal"       , "pad.window" )
    XTMP_SZR("szr.column"       , "cnt.column"      , "pad.column"   )

    /*
    XTMP_SZR("szr."             , "cnt."            , "pad."       )
    XTMP_SZR("szr.window"       , "cnt.window"      , "pad.window" )
    XTMP_SZR("szr.node_header"  , "cnt.node_header" , "pad.text"   )
*/
//    XTMP_SZR(pSzrHeaderGraph      ,"header_graph"      ).container_(pCntGraphNodeHeader ).padding_(pPadText);
//    XTMP_SZR(pSzrWindow           ,"window"            ).container_(pCntWindow          ).padding_(pPadNull);
//    XTMP_SZR(pSzrWindowPadding    ,"window_padding"    ).container_(pCntWindow          ).padding_(pPad);

/*
    XTMP_SZR(pSzrMenu             ,"menu"              ).container_(pCntBackgroundA50   ).padding_(pPadMenu);
    XTMP_SZR(pSzrBackground       ,"background"        ).container_(pCntBackgroundAlpha ).padding_(pPadNull);
    XTMP_SZR(pSzrBlack            ,"black"             ).container_(pCntBackgroundBlack ).padding_(pPadNull);
    XTMP_SZR(pSzrBorderBackground ,"border.background" ).container_(pCntBackgroundBorder).padding_(pPadNull);
    XTMP_SZR(pSzrBackgroundLink   ,"background.link"   ).container_(pCntBackgroundLink  ).padding_(pPadNull);
    XTMP_SZR(pSzrBorder           ,"border"            ).container_(pCntBorder          ).padding_(pPadNull);
    XTMP_SZR(pSzrEdit             ,"edit"              ).container_(pCntEdit            ).padding_(pPadNull);
    XTMP_SZR(pSzrNull             ,"null"              ).container_(pCntNull            ).padding_(pPadNull);
    XTMP_SZR(pSzrZeroZeroPad      ,"zero.zero.pad"     ).container_(pCntNull            ).padding_(pPadMenu);
    XTMP_SZR(pSzrWindowLink       ,"window.link"       ).container_(pCntWindowLink      ).padding_(pPad);
*/
    #undef XTMP_SZR
    //-----------------------------------------------------------------------------
    //button
    //-----------------------------------------------------------------------------
    #define XTMP_BUT(x_Name, x_Container, x_Padding)\
        XTMP_UI_(button, x_Name)->container_(container.get(ALITERAL(x_Container))).padding_(padding.get(ALITERAL(x_Padding)));

    XTMP_BUT("but.column"       , "cnt.column"    , "pad.column"        )
    XTMP_BUT("but.modal"        , "cnt.modal"     , "pad.window"        )
    XTMP_BUT("but.modal_noleft" , "cnt.modal"     , "pad.window_noleft" )
    XTMP_BUT("but.debug"        , "cnt.debug"     , "pad.column"        )
    XTMP_BUT("but.padding"      , ""              , "pad.column"        )

/*
    XTMP_BUT("but."           , "cnt."           , "pad."         )
    XTMP_BUT("but.property"   , "cnt."           , "pad.text"     )
    XTMP_BUT("but.title"      , "cnt.hover"      , "pad.title"    )
    XTMP_BUT("but.column"     , "cnt.column"     , "pad.column"   )
    XTMP_BUT("but.keyboard"   , "cnt.keyboard"   , "pad.keyboard" )
    XTMP_BUT("but.node_header", "cnt.node_header", "pad.text"     )
    XTMP_BUT("but.scroller"   , "cnt.column"     , "pad.scroller" )
    XTMP_BUT("but.edit"       , "cnt.edit"       , "pad.text"     )
*/
//    XTMP_BUT(pButEdit                   , "edit"              , pCntEdit           , pPadText         ,MiddleRight );
//    XTMP_BUT(pButProperty               , "property"          , pCntNull           , pPadText         ,MiddleCenter);
//    XTMP_BUT(pButScroller               , "scroller"          , pCntToolbar        , pPadScroller     ,MiddleCenter);
//    XTMP_BUT(pButGraphNodeHeader        , "graph.node.header" , pCntGraphNodeHeader, pPadText         ,MiddleCenter);
//    XTMP_BUT(pButRightBorderFocus       , "right_border_focus", pCntToolbar        , pPadColumn       ,MiddleCenter);
//    XTMP_BUT(pButKeyboard               , "ase.keyboard"      , pCntKeyboard       , pPadKeyboard     ,MiddleCenter);


/*    XTMP_BUT(pButMenuToolbar            , "menu.toolbar"      , pCntBackgroundA50  , pPadMenu         ,MiddleLeft  );
    XTMP_BUT(pButBackgroundLink         , "background.link"   , pCntBackgroundLink , pPad             ,MiddleCenter);
    XTMP_BUT(pButDebug                  , "debug"             , pCntDebug          , pPadNull         ,MiddleCenter);
    XTMP_BUT(pButGraphNode              , "graph.node"        , pCntGraphNode      , pPadNull         ,MiddleCenter);
    XTMP_BUT(pButMenu                   , "menu"              , pCntHover          , pPadText         ,MiddleCenter);
    XTMP_BUT(pButTitleHover             , "title.hover"       , pCntHover          , pPadTitle        ,MiddleCenter);
    XTMP_BUT(pButGraphNodeHeaderPicture , "header"            , pCntHover          , pPadScroller     ,MiddleCenter);
    XTMP_BUT(pButIcon                   , "icon"              , pCntIcon           , pPadLeft         ,MiddleLeft  );
    XTMP_BUT(pButCover                  , "cover"             , pCntKeyboard       , pPadCover        ,MiddleCenter);
    XTMP_BUT(pButPout                   , "pout"              , pCntNull           , pPadNull         ,MiddleRight );
    XTMP_BUT(pButNull                   , "null"              , pCntNull           , pPadNull         ,MiddleCenter);
    XTMP_BUT(pButAlignLeft              , "left"              , pCntNull           , pPadNull         ,MiddleLeft  );
    XTMP_BUT(pButCompletion             , "ase.completion"    , pCntNull           , pPadNull         ,MiddleLeft  );
    XTMP_BUT(pButRight                  , "pout"              , pCntNull           , pPadTextY        ,MiddleRight );
    XTMP_BUT(pButCombo                  , "combo"             , pCntNull           , pPadToolbar      ,MiddleCenter);
    XTMP_BUT(pButGreenA25               , "green_a25"         , pCntRasterGreenA25 , pPadText         ,MiddleRight );
    XTMP_BUT(pButRedA25                 , "red_a25"           , pCntRasterRedA25   , pPadText         ,MiddleRight );
    XTMP_BUT(pButTextLeft               , "text_left"         , pCntText           , pPadTextLeft     ,MiddleLeft  );
    XTMP_BUT(pButText                   , "text"              , pCntTextHover      , pPadToolbar      ,MiddleRight );
    XTMP_BUT(pButRightBorderFocus       , "right_border_focus", pCntToolbar        , pPadColumn       ,MiddleCenter);
    XTMP_BUT(pButToolbar                , "toolbar"           , pCntToolbar        , pPadToolbar      ,MiddleCenter);
    XTMP_BUT(pButZeroLink               , "zero.link"         , pCntZeroLink       , pPadCover        ,MiddleCenter);
*/
    #undef XTMP_BUT
    //-----------------------------------------------------------------------------
    //font data
    //-----------------------------------------------------------------------------
    #define XTMP_FNT(x_Name, x_Pointer, x_Size) XTMP_UI_(font, x_Name)->pointer_(x_Pointer).size_(x_Size);

    XTMP_FNT("fnt.h2"     , data->font_title , data->font_size_h2        )
    XTMP_FNT("fnt.h4"     , data->font_title , data->font_size_h4        )
    XTMP_FNT("fnt.text"   , data->font_text  , data->font_size_text      )

/*
    XTMP_FNT("fnt.h46"    , data->font_title , data->font_size_h4 * 0.6f )
    XTMP_FNT("fnt.column" , data->font_text  , data->font_size_column    )
    XTMP_FNT("fnt.toolbar", data->font_text  , data->font_size_toolbar   )
*/
//  XTMP_FNT(pFontToolbar   , "toolbar"     , data->font_text  , data->font_size_toolbar);
//  XTMP_FNT(pFontH2        , "h2"          , data->font_title , data->font_size_h2);
//  XTMP_FNT(pFontH4        , "h4"          , data->font_title , data->font_size_h4);
//  XTMP_FNT(pFontH46       , "ase.keyboard", data->font_title , data->font_size_h4 * 0.6f);

/*    XTMP_FNT(pFontH1        , "h1"          , data->font_title , data->font_size_h1);
    XTMP_FNT(pFontH3        , "h3"          , data->font_title , data->font_size_h3);
    XTMP_FNT(pFontColumn    , "column"      , data->font_text  , data->font_size_column);
    XTMP_FNT(pFontText      , "text"        , data->font_text  , data->font_size_text);
    XTMP_FNT(pFontMenu      , "menu"        , data->font_text  , data->font_size_menu);
*/
    #undef XTMP_FNT
    //-----------------------------------------------------------------------------
    //label
    //-----------------------------------------------------------------------------
    #define XTMP_LBL(x_Name, x_Font, x_Button, x_EnNorm, x_EnHover, x_EnCheck, x_DiNorm)\
        {\
            auto *const pControl = XTMP_UI_(label, x_Name);\
            pControl->font_(font.get(ALITERAL(x_Font))).button_(button.get(ALITERAL(x_Button)));\
            pControl->enabled.normal_(color.get(ALITERAL(x_EnNorm))).hover_(color.get(ALITERAL(x_EnHover))).check_(color.get(ALITERAL(x_EnCheck)));\
            pControl->disabled.normal_(color.get(ALITERAL(x_DiNorm)));\
        }

    XTMP_LBL("lbl.title"      ,"fnt.h2"     , ""            , "col.link", ""            , ""    , ""            )
    XTMP_LBL("lbl.text"       ,"fnt.text"   , ""            , "col.text", "col.link_l1" , ""    , "col.control" )
    XTMP_LBL("lbl.text_padded","fnt.text"   , "but.padding" , "col.text", "col.link_l1" , ""    , "col.control" )
    XTMP_LBL("lbl.evolution"     ,"fnt.h4"     , ""            , "col.link", ""            , ""    , ""            )
    XTMP_LBL("lbl.h4"         ,"fnt.h4"     , ""            , "col.text", ""            , ""    , ""            )

/*
        XTMP_LBL(pLblevolutionBank     ,"ase.evolution.bank"   ,pFontH4     ,pColLink      ,nullptr       ,nullptr ,nullptr    ,pButCompletion      )

    XTMP_LBL("lbl.h4"         , "fnt.h4"     , "but."        , "col.fore", "col."    , "col."    , "col."        )
    XTMP_LBL("lbl.property"   , "fnt.text"   , "but.property", "col.fore", "col.link", "col."    , "col.control" )
    XTMP_LBL("lbl.column"     , "fnt.column" , "but.column"  , "col.fore", "col."    , "col."    , "col."        )
    XTMP_LBL("lbl.keyboard"   , "fnt.h46"    , "but.keyboard", "col.fore", "col.link", "col."    , "col."        )
    XTMP_LBL("lbl.edit"       , "fnt.text"   , "but.edit"    , "col.back", "col."    , "col."    , "col."        )
    XTMP_LBL("lbl.node_header", "fnt.toolbar", "but."        , "col.fore", "col.link", "col.link", "col.control" )
*/
//    XTMP_LBL(pLblGraphNodeHeader,"graph_node_header" ,pFontToolbar,pColForeground,pColLink      ,pColLink,pColControl,pButNull            )
//    XTMP_LBL(pLblEdit           ,"edit"              ,pFontText   ,pColBackground,nullptr       ,nullptr ,nullptr    ,pButEdit            )
//    XTMP_LBL(pLblProperty       ,"property"          ,pFontText   ,pColForeground,pColLink      ,nullptr ,pColControl,pButProperty        )
//    XTMP_LBL(pLblIcon           ,"icon"              ,pFontText   ,pColForeground,pColLink      ,nullptr ,pColControl,pButAlignLeft       )
//    XTMP_LBL(pLblH4             ,"h4"                ,pFontH4     ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButNull            )
//    XTMP_LBL(pLblKeyboard       ,"ase.keyboard"      ,pFontH46    ,pColLink      ,pColForeground,nullptr ,nullptr    ,pButKeyboard        )


/*    XTMP_LBL(pLblPositionY      ,"frame.position.y"  ,pFontColumn ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButGreenA25        )
    XTMP_LBL(pLblPositionX      ,"frame.position.x"  ,pFontColumn ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButRedA25          )
    XTMP_LBL(pLblColumn         ,"column"            ,pFontColumn ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButRightBorderFocus)
    XTMP_LBL(pLblTitleHover     ,"title.hover"       ,pFontH2     ,pColLink      ,nullptr       ,nullptr ,nullptr    ,pButTitleHover      )
    XTMP_LBL(pLblCompletion     ,"ase.completion"    ,pFontH3     ,pColLink      ,nullptr       ,nullptr ,nullptr    ,pButCompletion      )
    XTMP_LBL(pLblH4Left         ,"h4.left"           ,pFontH4     ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButAlignLeft       )
    XTMP_LBL(pLblevolutionBank     ,"ase.evolution.bank"   ,pFontH4     ,pColLink      ,nullptr       ,nullptr ,nullptr    ,pButCompletion      )
    XTMP_LBL(pLblH4Link         ,"h4.link"           ,pFontH4     ,pColLink      ,nullptr       ,nullptr ,nullptr    ,pButNull            )
    XTMP_LBL(pLblMenuHover      ,"menu.hover"        ,pFontMenu   ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButMenu            )
    XTMP_LBL(pLblCombo          ,"combo"             ,pFontText   ,pColBackground,nullptr       ,nullptr ,nullptr    ,pButCombo           )
    XTMP_LBL(pLblTextHover      ,"text.hover"        ,pFontText   ,pColForeground,pColLink      ,nullptr ,pColControl,pButText            )
    XTMP_LBL(pLblTextHoverLeft  ,"text.hover.left"   ,pFontText   ,pColForeground,pColLink      ,nullptr ,pColControl,pButText            ) // @@0 duplicate
    XTMP_LBL(pLblText           ,"text"              ,pFontText   ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButNull            )
    XTMP_LBL(pLblPout           ,"pout"              ,pFontText   ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButRight           )
    XTMP_LBL(pLblTextLeft       ,"text.left"         ,pFontText   ,pColForeground,nullptr       ,nullptr ,nullptr    ,pButTextLeft        )
    XTMP_LBL(pLblToolbar        ,"toolbar"           ,pFontToolbar,pColForeground,pColLink      ,pColLink,pColBorder ,pButToolbar         )
*/
    #undef XTMP_LBL
    //-----------------------------------------------------------------------------
    //picture
    //-----------------------------------------------------------------------------
    #define XTMP_PIC(x_Name, x_Button, x_EnNormal, x_EnHover, x_EnFocus, x_EnCheck, x_DiNormal)\
        {\
            auto *const pControl = XTMP_UI_(picture, x_Name);\
            pControl->button_(button.get(ALITERAL(x_Button)));\
            pControl->enabled\
                .normal_(color.get(ALITERAL(x_EnNormal)))\
                .hover_(color.get(ALITERAL(x_EnHover)))\
                .focus_(color.get(ALITERAL(x_EnFocus)))\
                .check_(color.get(ALITERAL(x_EnCheck)));\
            pControl->disabled.normal_(color.get(ALITERAL(x_DiNormal)));\
        }

    XTMP_PIC("pic."         , ""            , "col.link", "col.link_l1"       , "", "", "col.control")
    XTMP_PIC("pic.column"   , "but.column"  , "col.link", "col.link_l1"       , "", "", "col.control")

/*    XTMP_PIC("pic.scroller_center"   , 0.015f , "but.column", "col.link", "col.link_l1"       , "", "", "col.control")
    XTMP_PIC("pic.scroller_icon"     , 0.015f , ""          , "col.link", "col.link_l1"       , "", "", "col.control")
    XTMP_PIC("pic.collapser"         , 0.015f , ""          , "col.link", "col.link_l1"       , "", "", "col.control")
    XTMP_PIC("pic.icon"              , 0.026f , ""          , "col.link", "col.link_l1"       , "", "", "col.control")
    XTMP_PIC("pic.cover"             , 0.2f   , "but.column", "col.link", "col.link_l1"       , "", "", "col.control")
  */
/*
    XTMP_PIC("pic."           , 0.0f  , "but."           , "col."    , "col."       , "col.", "col.", "col.")
    XTMP_PIC("pic.node_header", 0.026f, "but.node_header", "col.link", "col.control", "col.", "col.", "col.")
    XTMP_PIC("pic.icon_small" , 0.016f, "but.node_header", "col.link", "col.control", "col.", "col.", "col.")
*/
//    XTMP_PIC(  pPicCover            , "cover"       , 0.2f  , pColLink  , nullptr    , pButCover    )
//    XTMP_PIC_( pPicIconSmall        , "icon.small"  , 0.016f, pColLink  , pColControl, pButGraphNodeHeaderPicture, pColLinkA38  , nullptr   , nullptr    )
//    XTMP_PIC_( pPicGraphNodeHeader  , "header"      , 0.026f, pColLink  , pColControl, pButGraphNodeHeaderPicture, pColLinkA38  , nullptr   , nullptr    )
//    XTMP_PIC(  pPicCollapser        , "collapser"   , 0.02f , pColLink  , nullptr    , pButNull     )
//    XTMP_PIC(  pPicScroller         , "scroller"    , 0.02f , pColLink  , nullptr    , pButScroller )
//    XTMP_PIC_( pPicIcon             , "icon"        , 0.026f, pColLink  , pColControl, pButNull                  , pColLinkA38  , nullptr   , nullptr    )

/*
    XTMP_PIC_( pPicPin              , "pin"         , 0.016f, pColBorder, pColControl, pButNull                  , pColGreen    , pColGreen , pColGreen  )
    XTMP_PIC_( pPicPout             , "pout"        , 0.016f, pColBorder, pColControl, pButNull                  , pColRed      , pColRed   , pColRed    )
*/
    #undef XTMP_PIC
    //-----------------------------------------------------------------------------
    //window
    //-----------------------------------------------------------------------------
    #define XTMP_WIN(x_Name, x_Sizer, x_Alpha, x_Fade, x_Speed)\
        XTMP_UI_(window, x_Name)->sizer_(sizer.get(ALITERAL(x_Sizer))).alpha_(x_Alpha).duration_(x_Fade).speed_(x_Speed);

    XTMP_WIN( "wnd.dialog" , "szr.window" , 1.0f, 0.3f, 1.0f)
    XTMP_WIN( "wnd.demo"   , "szr.window" , 1.0f, 0.3f, 100.0f)
    XTMP_WIN( "wnd.main"   , "szr.window" , 1.0f, 0.7f, 1.0f)
    XTMP_WIN( "wnd.menu"   , ""           , 0.1f, 0.3f, 1.0f)
    XTMP_WIN( "wnd.overlay", ""           , 0.01f, 0.3f, 1.0f)

/*
    XTMP_WIN( "wnd."       , "szr."       , 1.0f, 0.7f)
    XTMP_WIN( "wnd.modal"  , "szr."       , 0.0f, 0.8f)
    XTMP_WIN(pWinMenu          ,"menu"           ,pSzrNull          ,1.0f,0.5f);
*/
/*
    XTMP_WIN(pWinNull          ,"null"           ,pSzrNull          ,1.0f,0.7f);
    XTMP_WIN(pWinBlack         ,"black"          ,pSzrBlack         ,1.0f,0.7f);
    XTMP_WIN(pWinDialog        ,"dialog"         ,pSzrWindowLink    ,1.0f,0.4f);
    XTMP_WIN(pWinBackgroundLink,"background.link",pSzrBackgroundLink,1.0f,0.4f);
    XTMP_WIN(pWinModText       ,"mod_text"       ,pSzrWindow        ,1.0f,0.7f);
    XTMP_WIN(pWinModSelect     ,"mod_select"     ,pSzrWindowPadding ,1.0f,0.5f);
    XTMP_WIN(pWinGuiManager    ,"gui_manager"    ,pSzrBackground    ,0.0f,0.8f);
    XTMP_WIN(pWinEdit          ,"edit"           ,pSzrEdit          ,1.0f,0.3f);
*/
    #undef XTMP_WIN
    //-----------------------------------------------------------------------------
    //scroller
    //-----------------------------------------------------------------------------

    XTMP_UI_(scroller, "scr.")->
        container_      ( container.get ( ALITERAL(""                    ))).
        center_         ( container.get ( ALITERAL("cnt.border"          ))).
        picture_center_ ( picture.get   ( ALITERAL("pic.column" ))).
        picture_icon_   ( picture.get   ( ALITERAL("pic.column"   ))).
        gfx_up_         ( data->gfx_up       ).
        gfx_down_       ( data->gfx_down     ).
        gfx_left_       ( data->gfx_left     ).
        gfx_right_      ( data->gfx_right    ).
        gfx_scroll_     ( nullptr            );

    //-----------------------------------------------------------------------------
    //text
    //-----------------------------------------------------------------------------
    #define XTMP_TXT(x_Name, x_Label, x_Carret) XTMP_UI_(text, x_Name)->\
        label_(label.get(ALITERAL(x_Label))).carret_(color.get(ALITERAL(x_Carret)));

   // XTMP_TXT("txt.", "lbl.edit", "col.black");

 //   XTMP_TXT(pText, "default").label_(pLblEdit).carret_(pColBlack);

    #undef XTMP_TXT
    //-----------------------------------------------------------------------------
    //splitter
    //-----------------------------------------------------------------------------
    #define XTMP_SPT(x_Name, x_Container, x_Spacer) XTMP_UI_(splitter, x_Name)->\
        container_(container.get(ALITERAL(x_Container))).spacer_(container.get(ALITERAL(x_Spacer)))

    XTMP_SPT("spt.modal", "", "cnt.modal");

/*
    XTMP_SPT( pSplitterBlack    , "black"       ).container_(pCntBackgroundBlack)   .spacer_(pSpacer);
    XTMP_SPT( pSplitterBlackA50 , "black.a50"   ).container_(pCntBackgroundA50)     .spacer_(pSpacer);
    XTMP_SPT( pSplitter         , "default"     ).container_(pCntNull)              .spacer_(pSpacer);
    XTMP_SPT( pSplitterNull     , "null"        ).container_(pCntNull)              .spacer_(pSpacerNull);
*/

    #undef XTMP_SPT
    //-----------------------------------------------------------------------------
    //icon
    //-----------------------------------------------------------------------------
    #define XTMP_ICO(x_Name, x_Splitter, x_Picture, x_Label, x_Button)\
        XTMP_UI_(icon, x_Name)->\
            splitter_(splitter.get(ALITERAL(x_Splitter))).\
            picture_(picture.get(ALITERAL(x_Picture))).\
            label_(label.get(ALITERAL(x_Label))).\
            button_(button.get(ALITERAL(x_Button)));
            
    //XTMP_ICO("ico.", "spt.", "pic.icon", "lbl.text", "but.column")
//    XTMP_ICO(pIconMenu              , "menu"    , pSplitter, pPicIcon           , pLblIcon           , pButToolbar          );// @@0 duplicate
    XTMP_ICO("ico.toolbar", "", ""        , "lbl.text", "but.column")
    XTMP_ICO("ico.toggle" , "", "pic.", "lbl.text", "but.padding")

/*
    XTMP_ICO(pIconGraphNodeHeader   , "header"  , pSplitter, pPicGraphNodeHeader, pLblGraphNodeHeader, pButGraphNodeHeader  );
    XTMP_ICO(pIconToggle            , "toggle"  , pSplitter, pPicGraphNodeHeader, pLblIcon           , pButNull             );
    XTMP_ICO(pIcon                  , "default" , pSplitter, pPicIcon           , pLblIcon           , pButIcon             );
    XTMP_ICO(pIconProperty          , "property", pSplitter, pPicIcon           , pLblIcon           , pButProperty         );
    XTMP_ICO(pIconPout              , "pout"    , pSplitter, pPicPout           , pLblPout           , pButPout             );
    XTMP_ICO(pIconDialog            , "dialog"  , pSplitter, pPicIcon           , pLblIcon           , pButToolbar          );
*/
    #undef XTMP_ICO
    //-----------------------------------------------------------------------------
    //menu
    //-----------------------------------------------------------------------------
    #define XTMP_TBR(x_Name, x_Icon, x_Window, x_Container, x_Spaced, x_Adjust, x_Width)\
        XTMP_UI_(toolbar, x_Name)->icon_(icon.get(ALITERAL(x_Icon))).\
            window_(window.get(ALITERAL(x_Window))).\
            container_(container.get(ALITERAL(x_Container))).\
            spaced_(x_Spaced).adjust_(x_Adjust).min_width_(x_Width)

    XTMP_TBR("tbr.", "ico.toolbar", "", "", true, false, 0.0f);

    //XTMP_TBR("tbl.menu",
/*    XTMP_TOB(pToolbarNull   , "null"         , nullptr       , nullptr           , nullptr , nullptr, false, false, 0.0f );
    XTMP_TOB(pToolbarDialog , "dialog"       , pIconMenu     , pWinNull          , nullptr , pSpacer, true , true , 0.16f);
    XTMP_TOB(pToolbar       , "index.toolbar", pIconMenu     , pWinNull          , pCntNull, pSpacer, true , false, 0.0f );
    XTMP_TOB(pToolbar1      , "index.menu"   , pIconMenu     , pWinBackgroundLink, nullptr , pSpacer, false, true , 0.0f );
*/
    #undef XTMP_TBR
    //-----------------------------------------------------------------------------
    //resolution
    //-----------------------------------------------------------------------------
  /*  auto* const pResolution = resolution.allocate(ALITERAL("default"));
    pResolution->center_(false);
    pResolution->scale_(false);

    auto* const pResolutionScale = resolution.allocate(ALITERAL("scale"));
    pResolutionScale->center_(false);
    pResolutionScale->scale_(true);

    auto* const pResolutionDialog = resolution.allocate(ALITERAL("dialog"));
    pResolutionDialog->center_(true);
    pResolutionDialog->scale_(true);
*/
    //-----------------------------------------------------------------------------
    //dialog
    //-----------------------------------------------------------------------------
    #define XTMP_DLG(x_Name, x_Window, x_Splitter, x_Toolbar) XTMP_UI_(dialog, x_Name)->\
        window_(window.get(ALITERAL(x_Window))).splitter_(splitter.get(ALITERAL(x_Splitter))).\
            toolbar_(toolbar.get(ALITERAL(x_Toolbar)));
    XTMP_DLG("dlg.", "wnd.dialog", "spt.", "tbr.")

    //-----------------------------------------------------------------------------
    //toggle
    //-----------------------------------------------------------------------------
    #define XTMP_TGL(x_Name, x_Icon, x_GfxOn, x_GfxOff) XTMP_UI_(toggle, x_Name)->\
        icon_(icon.get(ALITERAL(x_Icon))).gfx_on_(x_GfxOn).gfx_off_(x_GfxOff);

    XTMP_TGL("tgl.enable", "ico.toggle", data->gfx_on    , data->gfx_off  )
    XTMP_TGL("tgl.expand", "ico.toggle", data->gfx_minus , data->gfx_plus )

  /*  auto* const pToggle = toggle.allocate(ALITERAL("default"));
    pToggle->icon_(pIcon);
    pToggle->gfx_on_(data->gfx_on);
    pToggle->gfx_off_(data->gfx_off);

    auto* const pToggleProperty = toggle.allocate(ALITERAL("property"));
    pToggleProperty->icon_(pIconProperty);
    pToggleProperty->gfx_on_(data->gfx_on);
    pToggleProperty->gfx_off_(data->gfx_off);

    auto* const pToggleEnabled = toggle.allocate(ALITERAL("toolbar.enabled"));
    pToggleEnabled->icon_(pIconToggle);
    pToggleEnabled->gfx_on_(data->gfx_on);
    pToggleEnabled->gfx_off_(data->gfx_off);

    auto* const pToggleExpand = toggle.allocate(ALITERAL("toolbar.expanded"));
    pToggleExpand->icon_(pIconToggle);
    pToggleExpand->gfx_on_(data->gfx_minus);
    pToggleExpand->gfx_off_(data->gfx_plus);
*/
    #undef XTMP_TGL

    //-----------------------------------------------------------------------------
    //panel
    //-----------------------------------------------------------------------------
    #define XTMP_PANEL(x_Name, x_Scroller, x_User) XTMP_UI_(panel, x_Name)->\
        scroller_(scroller.get(ALITERAL(x_Scroller))).\
        user_(container.get(ALITERAL(x_User)));

    XTMP_PANEL("pnl."      , "scr.", ""             )
    XTMP_PANEL("pnl.window", "scr.", "cnt.window"             )
    //XTMP_PANEL("pnl.window", "scr.", "spi.", "cnt.window.border")

/*
    XTMP_PANEL(pPanelRaw    , "raw"     ).scroller_(pScroller).spacing_(pSpacingNull);
    XTMP_PANEL(pPanelNull   , "null"    ).scroller_(pScroller).spacing_(pSpacing);
    XTMP_PANEL(pPanel       , "default" ).scroller_(pScroller).spacing_(pSpacing).user_(pCntBorder);
    XTMP_PANEL(pPanelCombo  , "combo"   ).scroller_(pScroller).spacing_(pSpacing).user_(pCntNull);
    XTMP_PANEL(pPanelWindow , "window"  ).scroller_(pScroller).spacing_(pSpacing).user_(pCntWindowBorder);
*/
    #undef XTMP_PANEL
    //-----------------------------------------------------------------------------
    //frame
    //-----------------------------------------------------------------------------
    frame.allocate(ALITERAL("frm."))->
        container_  ( EMP_GUI_SKIN_(container, "cnt.window"     )).//pCntBackgroundBorder);
        unit_line_  ( EMP_GUI_SKIN_(color    , "col.gray10"     )).
        unit_spike_ ( EMP_GUI_SKIN_(color    , "col.gray40"     )).
        origin_x_   ( EMP_GUI_SKIN_(color    , "col.red"        )).
        origin_y_   ( EMP_GUI_SKIN_(color    , "col.green"      )).
        cursor_x_   ( EMP_GUI_SKIN_(color    , "col.red_a25"    )).
        cursor_y_   ( EMP_GUI_SKIN_(color    , "col.green_a25"  )).
        axis_x_     ( EMP_GUI_SKIN_(color    , "col.gray40"     )).
        axis_y_     ( EMP_GUI_SKIN_(color    , "col.gray40"     )).
        position_x_ ( EMP_GUI_SKIN_(label    , ""               )).//pLblPositionX);
        position_y_ ( EMP_GUI_SKIN_(label    , ""               ));//pLblPositionY);

    //-----------------------------------------------------------------------------
    //coordinate
    //-----------------------------------------------------------------------------
    coordinate.allocate(ALITERAL("crd."))->
        container_  ( EMP_GUI_SKIN_(container   , "cnt.window"      )). // pCntBackgroundBorder
        frame_      ( EMP_GUI_SKIN_(frame       , "frm."            )).
        unit_       ( EMP_GUI_SKIN_(label       , "lbl.text_padded" ));

    //-----------------------------------------------------------------------------
    //function
    //-----------------------------------------------------------------------------
    function.allocate(ALITERAL("fct."))->
        coordinate_ ( EMP_GUI_SKIN_(coordinate  , "crd."        )).
        line_       ( EMP_GUI_SKIN_(color       , "col.link"    ));

    //-----------------------------------------------------------------------------
    //plot
    //-----------------------------------------------------------------------------
    plot.allocate(ALITERAL("plt."))->
        coordinate_ ( EMP_GUI_SKIN_(coordinate  , "crd."        )).
        line_       ( EMP_GUI_SKIN_(color       , "col.gray10"  )).
        plot_hover_ ( EMP_GUI_SKIN_(color       , "col.link"    )).
        plot_normal_( EMP_GUI_SKIN_(color       , "col.gray40"  ));

    //-----------------------------------------------------------------------------
    //nurbs
    //-----------------------------------------------------------------------------
    nurbs.allocate(ALITERAL("nrb."))->
        plot_( EMP_GUI_SKIN_(plot , "plt."      )).
        line_( EMP_GUI_SKIN_(color, "col.link"  ));

    //-----------------------------------------------------------------------------
    //glyph
    //-----------------------------------------------------------------------------
  /*  auto* const pGlyph = glyph.allocate(ALITERAL("default"));
    pGlyph->coordinate_(pCoordinate);
    pGlyph->glyph_(pColLink);
*/
    //-----------------------------------------------------------------------------
    //outline
    //-----------------------------------------------------------------------------
   /* auto* const pOutline = outline.allocate(ALITERAL("default"));
    pOutline->plot_(pPlot);
    pOutline->line_(pColLink);
    pOutline->control_(pColLink);
*/
    //-----------------------------------------------------------------------------
    //slider
    //-----------------------------------------------------------------------------
    #define XTMP_SDR(x_Name, x_Label, x_Color) XTMP_UI_(slider, x_Name)->label_(label.get(ALITERAL(x_Label))).value_(color.get(ALITERAL(x_Color)));

  /*  auto* const pSlider = slider.allocate(ALITERAL("default"));
    pSlider->label_(pLblProperty);
    pSlider->value_(pColLinkA38);
*/
    XTMP_SDR("sdr.", "lbl.text", "col.link_a38")

    #undef XTMP_SDR
    //-----------------------------------------------------------------------------
    //line
    //-----------------------------------------------------------------------------
    #define XTMP_LIN(x_Name, x_Container, x_Collapser, x_GfxCollapse, x_GfxExpand) XTMP_UI_(line, x_Name)->\
        container_(container.get(ALITERAL(x_Container))).\
        collapser_(picture.get(ALITERAL(x_Collapser))).\
        gfx_collapsed_(x_GfxCollapse).\
        gfx_expanded_(x_GfxExpand);

    XTMP_LIN("lin.column", "cnt.column", "", data->gfx_plus, data->gfx_minus)

/*    auto* const pHeader = line.allocate(ALITERAL("header"));
    pHeader->container_(pCntHeader);

    auto* const pLineCombo = line.allocate(ALITERAL("combo"));
    pLineCombo->container_(pCntLineCombo);
    pLineCombo->collapser_(pPicCollapser);
    pLineCombo->gfx_collapsed_(data->gfx_plus);
    pLineCombo->gfx_expanded_(data->gfx_minus);
*/
    #undef XTMP_LIN
    //-----------------------------------------------------------------------------
    //list
    //-----------------------------------------------------------------------------
    #define XTMP_LIST(x_Var, x_Name) XTMP_UI(x_Var, list, x_Name)

    {
        auto* const pControl = XTMP_UI_(list, "lst.");
            pControl->
            //spacer_         ( spacer .get(ALITERAL("spe."           ))).
            //sizer_          ( sizer  .get(ALITERAL("szr."           ))) //.
            panel_          ( panel  .get(ALITERAL("pnl.window"           ))).
            header_         ( line   .get(ALITERAL("lin.column"     ))).//.
            //header_color_   ( color  .get(ALITERAL("col.border"     ))).
            column_         ( label  .get(ALITERAL("lbl.text_padded"     ))).
            label_          ( label  .get(ALITERAL("lbl.text_padded"      ))).
            icon_           ( icon   .get(ALITERAL("ico.toggle"           ))).
            toggle_         ( toggle .get(ALITERAL("tgl.enable"           ))).
            slider_         ( slider .get(ALITERAL("sdr."           ))).
            combo_          ( list   .get(ALITERAL("lst."           ))).
        	//text_           ( text   .get(ALITERAL("txt."           ))).
            //window_         ( window .get(ALITERAL("wnd."           ))).
        	gfx_edit_       ( data->gfx_edit                          )
            ;


        c_size stCount = 10;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_float fDepth = emp::ast::cast<float>(stIndex);
            c_v4f v4Color = v4f::add(data->control, v4f::mul(v4f({0.04f, 0.04f, 0.04f, 1}), v4f::all(fDepth)));

            auto* const pColI = color.allocate(STR_AFORMAT("col.line_#0", stIndex));
            pColI->rgba_(v4f::all(v4f::xyz(v4Color), 1.0f));

            control_skin_s* pCtrlI = control.allocate(STR_AFORMAT("ctl.line_#0", stIndex));
            pCtrlI->raster_(pColI).border_(color.get(ALITERAL("col.control_l3")));

            auto* const pCntLine = container.allocate(STR_AFORMAT("cnt.line_#0", stIndex));
            pCntLine->enabled_background
                .normal_(pCtrlI)
                .focus_( EMP_GUI_SKIN_(control, "ctl.focus"))
                .select_(EMP_GUI_SKIN_(control, "ctl.select"))
                .check_( EMP_GUI_SKIN_(control, "ctl.check"));

            pCntLine->enabled_foreground
                .hover_(EMP_GUI_SKIN_(control, "ctl.hover"));

            auto* const pLine = line.allocate(STR_AFORMAT("lin.#0", stIndex));
            pLine->container_(pCntLine)
                .collapser_(EMP_GUI_SKIN_(picture, "pic."))
                .gfx_collapsed_(data->gfx_plus)
                .gfx_expanded_(data->gfx_minus);

            pControl->lines.add(pLine);
        }
    }
  /*  XTMP_LIST(pList, "default").
        spacer_         ( pSpacer           ).
        sizer_          ( pSzrNull          ).
        panel_          ( pPanelWindow      ).
        header_         ( pHeader           ).
        header_color_   ( pColBorder        ).
        column_         ( pLblColumn     ).
        label_          ( pLblTextHover  ).
        icon_           ( pIcon             ).
        toggle_         ( pToggle           ).
        slider_         ( pSlider           ).
        text_           ( pText             ).
        gfx_edit_       ( data->gfx_edit    ).
        window_         ( pWinEdit          );

    XTMP_LIST(pListDefaultNoColumn, "default.nocolumns").
        spacer_         (pSpacer).
        sizer_          (pSzrNull).
        panel_          (pPanelWindow).
        header_         (pHeader).
        header_color_   (pColBorder).
        column_         (pLblColumn).
        label_          (pLblTextHover).
        icon_           (pIcon).
        toggle_         (pToggle).
        slider_         (pSlider).
        text_           (pText).
        gfx_edit_       (data->gfx_edit).
        window_         (pWinEdit).
        show_columns_   (false).
        wrap_lines_     (false);
    pListDefaultNoColumn->lines.add(pLineCombo);
    pListDefaultNoColumn->lines_top.add(pLineCombo);
    pListDefaultNoColumn->lines_top_bottom.add(pLineCombo);
    pListDefaultNoColumn->lines_bottom.add(pLineCombo);


    XTMP_LIST(pListCombo, "combo")
        .spacer_(pSpacer)
        .sizer_(pSzrNull)
        .panel_(pPanelCombo)
        .header_(pHeader)
        .header_color_(pColBorder)
        .column_(pLblColumn)
        .label_(pLblCombo)
        .icon_(pIcon)
        .toggle_(pToggle)
        .slider_(pSlider)
        .text_(pText)
        .gfx_edit_(data->gfx_edit)
        .show_columns_(false)
        .wrap_lines_(false);

    pListCombo->lines.add(pLineCombo);
    pListCombo->lines_top.add(pLineCombo);
    pListCombo->lines_top_bottom.add(pLineCombo);
    pListCombo->lines_bottom.add(pLineCombo);

    XTMP_LIST(pListNoColumn, "nocolumn")
        .spacer_(pSpacer)
        .sizer_(pSzrNull)
        .panel_(pPanelWindow)
        .header_(pHeader)
        .header_color_(pColBorder)
        .column_(pLblColumn)
        .label_(pLblProperty)
        .icon_(pIconProperty)
        .toggle_(pToggleProperty)
        .slider_(pSlider)
        .text_(pText)
        .combo_(pListNoColumn)
        .gfx_edit_(data->gfx_edit)
        .window_(pWinEdit)
        .show_columns_(false)
        .wrap_lines_(false);

    XTMP_LIST(pListBackground, "background.nocolumn")
        .spacer_(pSpacer)
        .sizer_(pSzrBorderBackground)
        .panel_(pPanelWindow)
        .header_(pHeader)
        .header_color_(pColBorder)
        .column_(pLblColumn)
        .label_(pLblProperty)
        .icon_(pIconProperty)
        .toggle_(pToggleProperty)
        .slider_(pSlider)
        .text_(pText)
        .combo_(pListCombo)
        .gfx_edit_(data->gfx_edit)
        .window_(pWinEdit)
        .show_columns_(false)
        .wrap_lines_(false);

    c_size stCount = 10;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        // @@0 alloc
        auto* const pColI = color.allocate(STR_AFORMAT("color.#0", stIndex));

        c_float fDepth = emp::ast::cast<float>(stIndex);
        c_v4f v4Color = v4f::add(data->background, v4f::mul(data->depth, v4f::all(fDepth)));

        *pColI = color_s(v4f::all(v4f::xyz(v4Color), 1.0f));

        control_skin_s* pCtrlI = nullptr;
        if (stIndex != 0)
        {
            // @@0 alloc
            pCtrlI = control.allocate(STR_AFORMAT("default.#0", stIndex));
            pCtrlI->raster_(pColI);
        }

        control_skin_s* pCtrlTopI = nullptr;
        if (stIndex == 0)
        { pCtrlTopI = pCtrlZeroTopBorder; }
        if (stIndex != 0)
        {
            // @@0 alloc
            pCtrlTopI = control.allocate(STR_AFORMAT("top.#0", stIndex));
            pCtrlTopI->raster_(pColI).top_(pColBorder);
        }

        control_skin_s* pCtrlTopBottomI = nullptr;
        if (stIndex == 0)
        { pCtrlTopBottomI = pCtrlZeroTopBottomBorder; }
        else
        {
            // @@0 alloc
            pCtrlTopBottomI = control.allocate(STR_AFORMAT("top.bottom.#0", stIndex));
            pCtrlTopBottomI->raster_(pColI).top_(pColBorder).bottom_(pColBorder);
        }

        control_skin_s* pCtrlBottomI = nullptr;
        if (stIndex == 0)
        { pCtrlBottomI = pCtrlZeroBottomBorder; }
        else
        {
            // @@0 alloc
            pCtrlBottomI = control.allocate(STR_AFORMAT("bottom.#0", stIndex));
            pCtrlBottomI->raster_(pColI).bottom_(pColBorder);
        }

        // @@0 alloc
        auto* const pCntLine = container.allocate(STR_AFORMAT("line.default.#0", stIndex));
        pCntLine->enabled_background.normal_(pCtrlI).focus_(pCtrlLinkA38Zero).select_(pCtrlSelectA38Zero).check_(pCtrlCheckZero);
        pCntLine->enabled_foreground.focus_(pCtrlZeroLink).hover_(pCtrlHoverA20HoverA38);
        pCntLine->disabled_foreground.hover_(pCtrlNull);

        // @@0 alloc
        auto* const pCntLineTop = container.allocate(STR_AFORMAT("line.top.#0", stIndex));
        pCntLineTop->enabled_background.normal_(pCtrlTopI).focus_(pCtrlLinkA38TopBorder).select_(pCtrlSelectA38TopBorder).check_(pCtrlCheckTopBorder);
        pCntLineTop->enabled_foreground.focus_(pCtrlZeroLink).hover_(pCtrlHoverA20HoverA38);
        pCntLineTop->disabled_foreground.hover_(pCtrlNull);

        // @@0 alloc
        auto* const pCntLineTopBottom = container.allocate(STR_AFORMAT("line.top.bottom.#0", stIndex));
        pCntLineTopBottom->enabled_background.normal_(pCtrlTopBottomI).focus_(pCtrlLinkA38TopBottomBorder).select_(pCtrlSelectA38TopBottomBorder).check_(pCtrlCheckTopBottomBorder);

        pCntLineTopBottom->enabled_foreground.focus_(pCtrlZeroLink).hover_(pCtrlHoverA20HoverA38);
        pCntLineTopBottom->disabled_foreground.hover_(pCtrlNull);

        // @@0 alloc
        auto* const pCntLineBottom = container.allocate(STR_AFORMAT("line.bottom.#0", stIndex));
        pCntLineBottom->enabled_background.normal_(pCtrlBottomI).focus_(pCtrlLinkA38BottomBorder).select_(pCtrlSelectA38BottomBorder).check_(pCtrlCheckBottomBorder);
        pCntLineBottom->enabled_foreground.focus_(pCtrlZeroLink).hover_(pCtrlHoverA20HoverA38);
        pCntLineBottom->disabled_foreground.hover_(pCtrlNull);

        // @@0 alloc
        auto* const pLine = line.allocate(STR_AFORMAT("default.#0", stIndex));
        pLine->container_(pCntLine);
        pLine->collapser_(pPicCollapser);
        pLine->gfx_collapsed_(data->gfx_plus);
        pLine->gfx_expanded_(data->gfx_minus);

        // @@0 alloc
        auto* const pLineTop = line.allocate(STR_AFORMAT("top.#0", stIndex));
        pLineTop->container_(pCntLineTop);
        pLineTop->collapser_(pPicCollapser);
        pLineTop->gfx_collapsed_(data->gfx_plus);
        pLineTop->gfx_expanded_(data->gfx_minus);

        // @@0 alloc
        auto* const pLineTopBottom = line.allocate(STR_AFORMAT("top.bottom.#0", stIndex));
        pLineTopBottom->container_(pCntLineTopBottom);
        pLineTopBottom->collapser_(pPicCollapser);
        pLineTopBottom->gfx_collapsed_(data->gfx_plus);
        pLineTopBottom->gfx_expanded_(data->gfx_minus);

        // @@0 alloc
        auto* const pLineBottom = line.allocate(STR_AFORMAT("bottom.#0", stIndex));
        pLineBottom->container_(pCntLineBottom);
        pLineBottom->collapser_(pPicCollapser);
        pLineBottom->gfx_collapsed_(data->gfx_plus);
        pLineBottom->gfx_expanded_(data->gfx_minus);

        pList->lines.add(pLine);
        pList->lines_top.add(pLineTop);
        pList->lines_top_bottom.add(pLineTopBottom);
        pList->lines_bottom.add(pLineBottom);

        pListNoColumn->lines.add(pLine);
        pListNoColumn->lines_top.add(pLineTop);
        pListNoColumn->lines_top_bottom.add(pLineTopBottom);
        pListNoColumn->lines_bottom.add(pLineBottom);

        pListBackground->lines.add(pLine);
        pListBackground->lines_top.add(pLineTop);
        pListBackground->lines_top_bottom.add(pLineTopBottom);
        pListBackground->lines_bottom.add(pLineBottom);
    }
*/
    //-----------------------------------------------------------------------------
    //property
    //-----------------------------------------------------------------------------
    //#define XTMP_PTY(x_Name, x_List, x_Label) XTMP_UI_(property, x_Name)->list_(list.get(ALITERAL(x_List))).label_(label.get(ALITERAL(x_Label)));

    //XTMP_PTY("pty.", "lst.", "lbl.text")

/*    auto* const pProperty = property.allocate(ALITERAL("default"));
    pProperty->list = pListNoColumn;
    pProperty->label = pLblTextHoverLeft;

    auto* const pPropertyBackground = property.allocate(ALITERAL("background"));
    pPropertyBackground->list = pListBackground;
    pPropertyBackground->label = pLblTextHoverLeft;

    auto* const pPropertyPin = property.allocate(ALITERAL("pin"));
    pPropertyPin->list = pListNoColumn;
    pPropertyPin->label = pLblTextLeft;
*/
   // #undef XTMP_PTY
    //-----------------------------------------------------------------------------
    //graph
    //-----------------------------------------------------------------------------
    graph.allocate(ALITERAL("grp."))->
        panel_          ( EMP_GUI_SKIN_(panel       , "pnl.")).//pPanel;
        toolbar_        ( EMP_GUI_SKIN_(toolbar     , "tbr.")).//pToolbar;
        main_           ( EMP_GUI_SKIN_(sizer       , "")).//pSzrNull;
        button_         ( EMP_GUI_SKIN_(button      , "")).//pButGraphNode;
        header_         ( EMP_GUI_SKIN_(sizer       , "szr.column")).//pSzrHeaderGraph;
        header_picture_ ( EMP_GUI_SKIN_(label       , "lbl.text_padded")).//pLblGraphNodeHeader;
        remove_picture_ ( EMP_GUI_SKIN_(picture     , "pic.")).//pPicGraphNodeHeader;
        docker_         ( EMP_GUI_SKIN_(sizer       , "szr.window")).//pSzrZeroZeroPad;
        function_       ( EMP_GUI_SKIN_(function    , "fct.")).//pFunction;
        //spacer_         ( EMP_GUI_SKIN_(container, "")).//pSpacer;
        property_       ( EMP_GUI_SKIN_(list        , "lst.")).//pPropertyPin;
        input_docker_   ( EMP_GUI_SKIN_(sizer       , "")).//pSzrNull;
        pin_container_  ( EMP_GUI_SKIN_(container   , "")).//pCntNull;
        pin_picture_    ( EMP_GUI_SKIN_(picture     , "pic.")).//pPicPin;
        pout_icon_      ( EMP_GUI_SKIN_(icon        , "ico.toggle")).//pIconPout;
        pin_line_       ( EMP_GUI_SKIN_(color       , "col.green" )).//pColGreen;
        pout_line_      ( EMP_GUI_SKIN_(color       , "col.red"   )).//pColRed;
        gfx_expanded_   ( data->gfx_down ).
        gfx_collapsed_  ( data->gfx_right).
        gfx_remove_     ( data->gfx_check).
        gfx_pin_        ( data->gfx_right).
        gfx_pout_       ( data->gfx_right);

    //-----------------------------------------------------------------------------
    //menu
    //-----------------------------------------------------------------------------
    auto* const pMenu = menu.allocate(astring(ALITERAL("men.")));
    pMenu->
        window_(     EMP_GUI_SKIN_(window    , "wnd.menu")).
        toolbar_(    EMP_GUI_SKIN_(toolbar   , "tbr.")).
        button_tb_(  EMP_GUI_SKIN_(button    , "but.modal")).
        button_m_(   EMP_GUI_SKIN_(button    , "but.modal_noleft")).
        splitter_h_( EMP_GUI_SKIN_(splitter  , "spt.modal")).
        splitter_v_( EMP_GUI_SKIN_(splitter  , "spt."));

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INTERN T const* compute_layer_color(state_skin_t<T const> const& a_rLayers, c_state_data_s& a_rStates, c_state_data_s& a_rChild)
{
    c_bool bCheck = a_rStates.check || a_rChild.check;
    c_bool bFocus = a_rStates.focus || a_rChild.focus;
    c_bool bSelect = a_rStates.select || a_rChild.select;
    c_bool bHover = (a_rStates.hover || a_rChild.hover) &&
                    (a_rStates.focusable || a_rChild.focusable);

    T const* pLayer = nullptr;

    if (bCheck && a_rLayers.check)
    { pLayer = a_rLayers.check; }
    else if (bFocus && a_rLayers.focus)
    { pLayer = a_rLayers.focus; }
    else if (bSelect && a_rLayers.select)
    { pLayer = a_rLayers.select; }
    else if (bHover && a_rLayers.hover)
    { pLayer = a_rLayers.hover; }

    if (pLayer == nullptr)
    { pLayer = a_rLayers.normal; }
    return pLayer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void compute_state_color(state_skin_t<c_control_skin_s> const& a_rEnabled,
    state_skin_t<c_control_skin_s> const& a_rDisabled, state_data_s const& a_rStates, state_data_s const& a_rChild,
    c_bool a_bEnable, layer_data_s& a_rColor);
EMP_INTERN void compute_state_color(state_skin_t<c_control_skin_s> const& a_rEnabled,
    state_skin_t<c_control_skin_s> const& a_rDisabled, state_data_s const& a_rStates, state_data_s const& a_rChild,
    c_bool a_bEnable, layer_data_s& a_rColor)
{
    c_bool bEnable = a_rStates.enable && a_bEnable;

    c_control_skin_s* pLayerRaster = nullptr;
    c_control_skin_s* pLayerBorder = nullptr;
    if (bEnable == false)
    {
        pLayerRaster = compute_layer_color(a_rDisabled, a_rStates, a_rChild);
        pLayerBorder = compute_layer_color(a_rDisabled, a_rStates, a_rChild);
    }
    if (bEnable || pLayerRaster == nullptr)
    { pLayerRaster = compute_layer_color(a_rEnabled, a_rStates, a_rChild); }
    if (bEnable || pLayerBorder == nullptr)
    { pLayerBorder = compute_layer_color(a_rEnabled, a_rStates, a_rChild); }

    raster_data_s& rRaster = a_rColor.raster;
    border_data_s& rBorder = a_rColor.border;

    rRaster.color_(v4f::all(0));
    if (pLayerRaster != nullptr)
    {
        cpc_color_s pColRaster = pLayerRaster->raster;
        if (pColRaster != nullptr)
        { rRaster.color_(pColRaster->rgba); }
    }

    rBorder.color_(v4f::all(0));
    if (pLayerBorder != nullptr)
    {
        cpc_color_s pColBorder = pLayerBorder->border;
        if (pColBorder != nullptr)
        { rBorder.color_(pColBorder->rgba); }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename O>
EMP_INTERN void compute_layer_color(state_skin_t<T const> const& a_rEnabled, state_skin_t<T const> const& a_rDisabled,
    state_data_s const& a_rStates, state_data_s const& a_rChild, c_bool a_bEnable, O& a_rOut)
{
    c_bool bEnable = a_rStates.enable && a_bEnable;
    T const* pLayer = nullptr;
    if (bEnable == false)
    { pLayer = compute_layer_color(a_rDisabled, a_rStates, a_rChild); }
    if (bEnable || pLayer == nullptr)
    { pLayer = compute_layer_color(a_rEnabled, a_rStates, a_rChild); }
    if (pLayer != nullptr)
    { a_rOut = *pLayer; }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_label_color(c_label_skin_s& a_rSkin, c_state_data_s& a_rStates, c_bool a_bEnable, color_s& a_rColor)
{ compute_layer_color(a_rSkin.enabled, a_rSkin.disabled,a_rStates, a_rStates, a_bEnable, a_rColor); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_picture_color(c_picture_skin_s& a_rSkin, c_state_data_s& a_rStates, c_bool a_bEnable, color_s& a_rColor)
{ compute_layer_color(a_rSkin.enabled, a_rSkin.disabled, a_rStates, a_rStates, a_bEnable, a_rColor); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_container_color(c_container_skin_s& a_rSkin, c_state_data_s& a_rStates, c_state_data_s& a_rChild,
    c_bool a_bEnable, color_data_s& a_rColor)
{
    if (a_rStates.hover)
    {
        int x = 0;
        ++x;
    }
    compute_state_color(a_rSkin.enabled_background, a_rSkin.disabled_background, a_rStates, a_rChild, a_bEnable, a_rColor.background);
    compute_state_color(a_rSkin.enabled_foreground, a_rSkin.disabled_foreground, a_rStates, a_rChild, a_bEnable, a_rColor.foreground);
    /*//if (EMP_APPLICATION.gui_manager().m_bDebug)
    {
        //a_rColor.background.raster.color_({ 0, 0, 0, 0 });
        //a_rColor.background.border.color_({ 1, 0, 0, 1 }).border_(border_e::All);

        if (a_rChild.hover)
        {
            a_rColor.foreground.raster.color_({ 0, 0, 0, 0 });
            a_rColor.foreground.border.color_({ 1, 0, 0, 1 }).border_(border_e::All);
        }
        else
        {
            a_rColor.foreground.raster.color_({ 0, 0, 0, 0 });
            a_rColor.foreground.border.color_({ 0, 1, 0, 1 }).border_(border_e::All);
        }
    }*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN skin_c& skin(void)
{ return EMP_APPLICATION.gui_skin(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

