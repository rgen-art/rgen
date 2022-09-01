//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_MOD_REFLECTION_RFX_H
#define ASE_MOD_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_mod_mode_i.h"
#include "ase_mod_mode_a.h"
#include "ase_mod_compose.h"
#include "ase_mod_debug.h"
#include "ase_mod_demo.h"
#include "ase_mod_emphase.h"
#include "ase_mod_selection.h"
#include "ase_mod_evolution.h"
#include "ase_mod_improvize.h"
#include "ase_mod_music.h"
#include "ase_mod_synthesize.h"
#include "ase_mod_text.h"
#include "ase_mod_rgen.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_reflection.rfx.h"
#include "emp_loc_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::mode_i
#define type_interface
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::mode_a
#define type_interface
#define type_fields\
    enum_value_ex( emp::gui::window_c* , ( EMP_RFX_B , window   , m_pWindow   ) )\
    enum_value_ex( emp::loc::string_s  , ( EMP_RFX_B , name     , m_peName    ) )\
    enum_value_ex( emp::loc::string_s  , ( EMP_RFX_B , desc     , m_peDesc    ) )\
    enum_value_ex( float               , ( EMP_RFX_B , duration , m_fDuration ) )\
    enum_value_ex( ase::mod::mode_i*   , ( EMP_RFX_B , next     , m_pNextMod  ) )\
    enum_value_ex( ase::mod::mode_i*   , ( EMP_RFX_B , prev     , m_pPrevMod  ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::text_c
#define type_fields\
    enum_value_ex( emp::gui::label_c* , ( EMP_RFX_B , label , m_pLabel ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::demo_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::evolution_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::rgen_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::emphase_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::selection_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::music_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::improvize_c
#define type_fields\
    enum_value_ex( ase::mod::music_c* , ( EMP_RFX_B , mod_music , m_pModMusic ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::compose_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::synthesize_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::mod::debug_c
#define type_fields
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

