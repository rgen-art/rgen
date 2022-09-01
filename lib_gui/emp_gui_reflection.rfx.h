//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_GUI_REFLECTION_RFX_H
#define EMP_GUI_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_button.h"
#include "emp_gui_container.h"
#include "emp_gui_control_i.h"
#include "emp_gui_docker.h"
#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_menu.h"
#include "emp_gui_sizer.h"
#include "emp_gui_skin.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"

#include "emp_gfx_reflection.rfx.h"
#include "emp_mat_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::handler_i
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::control_i
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::window_i
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::skin_data_t<emp::gui::window_skin_s>
#define type_fields\
    enum_value_ex( astring , ( EMP_RFX_B , name , name ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::skin_data_t<emp::gui::button_skin_s>
#define type_fields\
    enum_value_ex( astring , ( EMP_RFX_B , name , name ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::skin_data_t<emp::gui::label_skin_s>
#define type_fields\
    enum_value_ex( astring , ( EMP_RFX_B , name , name ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::transformation_data_s
#define type_fields\
    enum_value_ex( emp::mat::v2f , ( EMP_RFX_B , position , position ) )\
    enum_value_ex( emp::mat::v2f , ( EMP_RFX_B , size     , size     ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::autosize_data_s
#define type_fields\
    enum_value_ex( bool , ( EMP_RFX_B , x , x ) )\
    enum_value_ex( bool , ( EMP_RFX_B , y , y ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::EMP_GUI_DOCK_E_NAME
#define type_fields EMP_GUI_DOCK_E_VALUES
#include "emp_rfx_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::EMP_GUI_ALIGNMENT_E_NAME
#define type_fields EMP_GUI_ALIGNMENT_E_VALUES
#include "emp_rfx_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::state_data_s
#define type_fields\
    enum_value_ex( bool                      , ( EMP_RFX_B , visible    , visible    ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , enable     , enable     ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , hover      , hover      ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , focus      , focus      ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , select     , select     ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , check      , check      ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , navigable  , navigable  ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , focusable  , focusable  ) )\
    enum_value_ex( bool                      , ( EMP_RFX_B , renderable , renderable ) )\
    enum_value_ex( ui32                      , ( EMP_RFX_B , index      , index      ) )\
    enum_value_ex( emp::gui::dock_e          , ( EMP_RFX_B , dock       , dock       ) )\
    enum_value_ex( emp::gui::autosize_data_s , ( EMP_RFX_B , autosize   , autosize   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::container_c
#define type_fields\
    enum_value_ex( emp::gui::state_data_s                 , ( EMP_RFX_B , state     , m_State      ) )\
    enum_value_ex( emp::gui::transformation_data_s             , ( EMP_RFX_B , transform , m_Transform  ) )\
    enum_value_ex( emp::cnt::vector<emp::gui::control_i*> , ( EMP_RFX_B , controls  , m_vpControls ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::label_c
#define type_fields\
    enum_value_ex( emp::gui::skin_data_t<emp::gui::label_skin_s> , ( EMP_RFX_B , skin      , skin      ) )\
    enum_value_ex( emp::gui::alignment_e                         , ( EMP_RFX_B , alignment , alignment ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::button_c
#define type_fields\
    enum_value_ex( emp::gui::skin_data_t<emp::gui::button_skin_s> , ( EMP_RFX_B , skin , skin     ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::sizer_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::docker_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::window_c
#define type_fields\
    enum_value_ex( emp::gui::skin_data_t<emp::gui::window_skin_s> , ( EMP_RFX_B , skin   , skin   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::menu_c
#define type_fields
#define class_pointer
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::color_s
#define type_fields\
    enum_value_ex( emp::mat::v4f, ( EMP_RFX_B , rgba , rgba ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::info_s
#define type_fields\
    enum_value_ex( emp::mat::v2f   , ( EMP_RFX_B , position      , position      ) )\
    enum_value_ex( emp::mat::v2f   , ( EMP_RFX_B , size          , size          ) )\
    enum_value_ex( emp::mat::v2f   , ( EMP_RFX_B , function_size , function_size ) )\
    enum_value_ex( float           , ( EMP_RFX_B , scale         , scale         ) )\
    enum_value_ex( float           , ( EMP_RFX_B , scale_x       , xscale        ) )\
    enum_value_ex( float           , ( EMP_RFX_B , scale_y       , yscale        ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , expand_input  , expand_input  ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , expand_output , expand_output ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , show_input    , show_input    ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , show_output   , show_output   ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , show_current  , show_output   ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , log_x         , xlog          ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , log_y         , ylog          ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::skin_data_s
#define type_to_string a_rValue.name
#define type_fields\
    enum_value_ex( astring             , ( EMP_RFX_B , name              , name             ))\
    enum_value_ex( emp::gfx::font_c*   , ( EMP_RFX_B , font_title        , font_title       ))\
    enum_value_ex( emp::gfx::font_c*   , ( EMP_RFX_B , font_text         , font_text        ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_text    , font_size_text   ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_button  , font_size_button ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_column  , font_size_column ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_toolbar , font_size_toolbar))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_menu    , font_size_menu   ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_h4      , font_size_h4     ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_h3      , font_size_h3     ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_h2      , font_size_h2     ))\
    enum_value_ex( float               , ( EMP_RFX_B , font_size_h1      , font_size_h1     ))\
    enum_value_ex( emp::mat::v2f       , ( EMP_RFX_B , spacing           , spacing          ))\
    enum_value_ex( bool                , ( EMP_RFX_B , random_link       , random_link      ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , a20               , a20              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , a25               , a25              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , a38               , a38              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , a50               , a50              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , a75               , a75              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , dark              , dark             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , light             , light            ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , control           , control          ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , text              , text             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , window            , window           ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , link              , link             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , black             , black            ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , zero              , zero             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , white             , white            ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , red               , red              ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , green             , green            ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , blue              , blue             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , yellow            , yellow           ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , pink              , pink             ))\
    enum_value_ex( emp::mat::v4f       , ( EMP_RFX_B , orange            , orange           ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_minus         , gfx_minus        ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_plus          , gfx_plus         ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_up            , gfx_up           ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_down          , gfx_down         ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_left          , gfx_left         ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_right         , gfx_right        ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_scroll        , gfx_scroll       ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_on            , gfx_on           ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_off           , gfx_off          ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_edit          , gfx_edit         ))\
    enum_value_ex( emp::gfx::model_c*  , ( EMP_RFX_B , gfx_check         , gfx_check        ))
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::skin_c
#define type_fields\
    enum_value_ex( emp::gui::skin_data_s* , ( EMP_RFX_B , data , data ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::gui::manager_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

