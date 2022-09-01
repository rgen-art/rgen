//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_REFLECTION_RFX_H
#define EMP_SND_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_composition.h"
#include "emp_snd_envelope.h"
#include "emp_snd_layout_i.h"
#include "emp_snd_layout.h"
#include "emp_snd_note.h"
#include "emp_snd_quality.h"
#include "emp_snd_sample.h"
#include "emp_snd_scale_i.h"
#include "emp_snd_scale.h"
#include "emp_snd_soft_synth.h"
#include "emp_snd_sound_i.h"
#include "emp_snd_sound.h"
#include "emp_snd_synth_i.h"
#include "emp_snd_track.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gfx_reflection.rfx.h"
#include "emp_gui_reflection.rfx.h"
#include "emp_mat_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::layout_i
#define type_to_string a_rValue.name()
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::synth_i
#define type_to_string a_rValue.name()
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::sound_i
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::sound_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring   , ( EMP_RFX_B , name  , m_sName  ) )\
    enum_value_ex( emp::mat::nurbs_c*  , ( EMP_RFX_B , nurbs , m_pNurbs ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::link_c
#define type_fields\
    enum_value_ex( ui32  , ( EMP_RFX_B , function , m_ui32Function ) )\
    enum_value_ex( ui32  , ( EMP_RFX_B , index    , m_ui32Index    ) )\
    enum_value_ex( float , ( EMP_RFX_B , value    , m_fValue       ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::function_c
#define type_fields\
    enum_value_ex( emp::str::astring                  , ( EMP_RFX_B , expression , m_sExpression ) )\
    enum_value_ex( emp::cnt::vector<emp::snd::link_c> , ( EMP_RFX_B , inputs     , m_vInputs     ) )\
    enum_value_ex( emp::cnt::vector<emp::mat::v4f>    , ( EMP_RFX_B , params     , m_vv4Params   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::quality_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring , ( EMP_RFX_B , name      , m_sName        ) )\
    enum_value_ex( ui32              , ( EMP_RFX_B , rate      , m_ui32Rate     ) )\
    enum_value_ex( ui32              , ( EMP_RFX_B , depth     , m_ui32Depth    ) )\
    enum_value_ex( ui32              , ( EMP_RFX_B , channels  , m_ui32Channels ) )\
    enum_value_ex( float             , ( EMP_RFX_B , volume    , m_fVolume      ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::soft_synth_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring                      , ( EMP_RFX_B , name      , m_sName      ) )\
    enum_value_ex( emp::snd::quality_c*                   , ( EMP_RFX_B , quality   , m_pQuality   ) )\
    enum_value_ex( emp::cnt::vector<emp::snd::function_c> , ( EMP_RFX_B , functions , m_vFunctions ) )\
    enum_value_ex( emp::cnt::vector<emp::gui::info_s>     , ( EMP_RFX_B , info      , m_vInfo      ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::scale_i
#define type_to_string a_rValue.name()
#define type_fields
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::scale_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring        , ( EMP_RFX_B , name   , m_sName   ) )\
    enum_value_ex( float                    , ( EMP_RFX_B , origin , m_fOrigin ) )\
    enum_value_ex( float                    , ( EMP_RFX_B , tones  , m_fTones  ) )\
    enum_value_ex( float                    , ( EMP_RFX_B , ratio  , m_fRatio  ) )\
    enum_value_ex( float                    , ( EMP_RFX_B , offset , m_fOffset ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::sample_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring      , ( EMP_RFX_B , name     , m_sName     ) )\
    enum_value_ex( emp::str::astring      , ( EMP_RFX_B , filename , m_sFilename ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::note_c
#define type_fields\
    enum_value_ex( ui32 , ( EMP_RFX_B , index    , index    ) )\
    enum_value_ex( ui64 , ( EMP_RFX_B , time     , time     ) )\
    enum_value_ex( ui64 , ( EMP_RFX_B , duration , duration ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::cell_c
#define type_fields\
    enum_value_ex( ui32                , ( EMP_RFX_B , index  , index  ) )\
    enum_value_ex( emp::snd::synth_i*  , ( EMP_RFX_B , synth  , synth  ) )\
    enum_value_ex( emp::snd::scale_i*  , ( EMP_RFX_B , scale  , scale  ) )\
    enum_value_ex( emp::snd::layout_i* , ( EMP_RFX_B , layout , layout ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::layout_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring                  , ( EMP_RFX_B , name  , m_sName  ) )\
    enum_value_ex( emp::cnt::vector<emp::snd::cell_c> , ( EMP_RFX_B , cells , m_vCells ) )
#include "emp_rfx_class.def.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::envelope_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::astring  , ( EMP_RFX_B , name  , m_sName  ) )\
    enum_value_ex( emp::mat::nurbs_c* , ( EMP_RFX_B , nurbs , m_pNurbs ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::composition_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::u8string                   , ( EMP_RFX_B , name   , m_su8Name  ) )\
    enum_value_ex( emp::cnt::vector<emp::snd::track_c*> , ( EMP_RFX_B , tracks , m_vpTracks ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name emp::snd::track_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::str::u8string                 , ( EMP_RFX_B , name          , m_su8Name          ) )\
    enum_value_ex( ui64                               , ( EMP_RFX_B , beat_duration , m_ui64BeatDuration ) )\
    enum_value_ex( emp::snd::scale_i*                 , ( EMP_RFX_B , scale         , m_pScale           ) )\
    enum_value_ex( emp::snd::layout_i*                , ( EMP_RFX_B , layout        , m_pLayout          ) )\
    enum_value_ex( bool                               , ( EMP_RFX_B , loop          , m_bLoop            ) )\
    enum_value_ex( emp::cnt::vector<emp::snd::note_c> , ( EMP_RFX_B , notes         , m_vNotes           ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

