//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_COR_REFLECTION_RFX_H
#define ASE_COR_REFLECTION_RFX_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"
#include "ase_cor_selection.h"
#include "ase_cor_game.h"
#include "ase_cor_option.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_rfx_factory.h"
#include "ase_mod_reflection.rfx.h"

#include "emp_app_reflection.rfx.h"
#include "emp_ose_reflection.rfx.h"
#include "emp_gfx_reflection.rfx.h"
#include "emp_gui_reflection.rfx.h"
#include "emp_snd_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::ASE_COR_MOD_E_NAME
#define type_fields ASE_COR_MOD_E_VALUES
#include "emp_rfx_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::ASE_COR_POOL_E_NAME
#define type_fields ASE_COR_POOL_E_VALUES
#include "emp_rfx_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::pool_c
#define type_to_string a_rValue.name
#define type_fields\
    enum_value_ex( astring            , ( EMP_RFX_B , id     , id          ) )\
    enum_value_ex( u8string           , ( EMP_RFX_B , name   , name        ) )\
    enum_value_ex( vector<astring>    , ( EMP_RFX_B , files  , m_vsOse     ) )\
    enum_value_ex( ase::cor::pool_e   , ( EMP_RFX_B , type   , type        ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::pool_list_c
#define type_fields\
    enum_value_ex( vector<ase::cor::pool_c*> , ( EMP_RFX_B , pools, pools ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::score_option_s
#define type_fields\
    enum_value_ex( bool  , ( EMP_RFX_B  , visible    , visible    ) )\
    enum_value_ex( bool  , ( EMP_RFX_RW , letters    , letters    ) )\
    enum_value_ex( bool  , ( EMP_RFX_RW , durations  , durations  ) )\
    enum_value_ex( float , ( EMP_RFX_RW , scale      , scale      ) )\
    enum_value_ex( float , ( EMP_RFX_RW , time_scale , time_scale ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::mapping_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( astring  , ( EMP_RFX_B , name    , m_sName      ) )\
    enum_value_ex( u8string , ( EMP_RFX_B , mapping , m_su8Mapping ) )
#include "emp_rfx_class.def.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::keyboard_option_s
#define type_fields\
    enum_value_ex( bool                 , ( EMP_RFX_B  , visible   , visible    ) )\
    enum_value_ex( bool                 , ( EMP_RFX_RW , letters   , m_bLetters ) )\
    enum_value_ex( float                , ( EMP_RFX_RW , scale     , m_fScale   ) )\
    enum_value_ex( ase::cor::mapping_c* , ( EMP_RFX_B  , mapping   , mapping    ) )\
    enum_value_ex( emp::snd::layout_i*  , ( EMP_RFX_B  , layout    , layout     ) )\
    enum_value_ex( emp::snd::scale_i*   , ( EMP_RFX_B  , snd_scale , m_pScale   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::emphase_option_s
#define type_fields\
    enum_value_ex( ase::cor::mod_e              , ( EMP_RFX_B , mod      , mod      ) )\
    enum_value_ex( float                        , ( EMP_RFX_B , bpm      , bpm      ) )\
    enum_value_ex( bool                         , ( EMP_RFX_B , shuffle  , shuffle  ) )\
    enum_value_ex( ase::cor::score_option_s     , ( EMP_RFX_B , score    , score    ) )\
    enum_value_ex( ase::cor::keyboard_option_s  , ( EMP_RFX_B , keyboard , keyboard ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::evolution_option_s
#define type_fields\
    enum_value_ex( si64                   , ( EMP_RFX_B  , left      , left      ) )\
    enum_value_ex( si64                   , ( EMP_RFX_B  , max       , max       ) )\
    enum_value_ex( ui64                   , ( EMP_RFX_B  , consumed  , consumed  ) )\
    enum_value_ex( emp::ose::mutation_s   , ( EMP_RFX_B  , mutation  , mutation  ) )\
    enum_value_ex( emp::ose::crossover_s  , ( EMP_RFX_B  , crossover , crossover ) )
 #include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::selection_option_s
#define type_fields\
    enum_value_ex( ase::cor::pool_c*      , ( EMP_RFX_B  , pool          , pool          ) )\
    enum_value_ex( ase::cor::pool_list_c* , ( EMP_RFX_B  , pools         , pools         ) )\
    enum_value_ex( ui32                   , ( EMP_RFX_B  , gallery_width , gallery_width ) )\
    enum_value_ex( bool                   , ( EMP_RFX_B  , edition       , edition       ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::option_s
#define type_fields\
    enum_value_ex( ase::cor::evolution_option_s , ( EMP_RFX_B , evolution , evolution ) )\
    enum_value_ex( ase::cor::emphase_option_s   , ( EMP_RFX_B , emphase   , emphase   ) )\
    enum_value_ex( ase::cor::selection_option_s , ( EMP_RFX_B , selection , selection ) )\
    enum_value_ex( float                        , ( EMP_RFX_B , scaling   , scale     ) )\
    enum_value_ex( emp::gui::skin_data_s*       , ( EMP_RFX_B , skin      , skin      ) )\
    enum_value_ex( emp::loc::locale_c*          , ( EMP_RFX_B , locale    , locale    ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::user_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( u8string                    , ( EMP_RFX_B , name     , m_su8Name) )\
    enum_value_ex( ase::cor::option_s          , ( EMP_RFX_B , options  , options  ) )\
    enum_value_ex( ase::mod::mode_i*           , ( EMP_RFX_B , init_mod , init_mod ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::performance_c
#define type_fields\
    enum_value_ex( emp::cnt::vector<ase::cor::completion_bucket_c*> , ( EMP_RFX_B , buckets   , m_vpBuckets ) )\
    enum_value_ex( emp::cnt::vector<ase::cor::user_data_s>          , ( EMP_RFX_B , user_data , m_vUserData ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::random_music_c
#define type_fields\
    enum_value_ex( u8string                , ( EMP_RFX_B , name              , m_su8Name             ) )\
    enum_value_ex( float                   , ( EMP_RFX_B , beat_per_minute   , m_fBeatPerMinute      ) )\
    enum_value_ex( ui32                    , ( EMP_RFX_B , max_touches       , m_ui32MaxTouches      ) )\
    enum_value_ex( ui32                    , ( EMP_RFX_B , max_left_touches  , m_ui32MaxLeftTouches  ) )\
    enum_value_ex( ui32                    , ( EMP_RFX_B , max_right_touches , m_ui32MaxRightTouches ) )\
    enum_value_ex( float                   , ( EMP_RFX_B , max_duration      , m_fMaxDuration        ) )\
    enum_value_ex( bool                    , ( EMP_RFX_B , full_time         , m_bFullTime           ) )\
    enum_value_ex( ase::cor::performance_c , ( EMP_RFX_B , performance       , m_Performance         ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::option_shell_c
#define type_fields\
    enum_value_ex( ase::cor::user_c*            , ( EMP_RFX_RW , user     , __no_value ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::music_shell_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( u8string         , ( EMP_RFX_R , name               , __no_value ) )\
    enum_value_ex( emp::tim::time_c , ( EMP_RFX_R , duration           , __no_value ) )\
    enum_value_ex( float            , ( EMP_RFX_R , beat_per_minute    , __no_value ) )\
    enum_value_ex( ui32             , ( EMP_RFX_R , rating             , __no_value ) )\
    enum_value_ex( ui32             , ( EMP_RFX_R , played             , __no_value ) )\
    enum_value_ex( u8string         , ( EMP_RFX_R , highest_user       , __no_value ) )\
    enum_value_ex( float            , ( EMP_RFX_R , highest_completion , __no_value ) )\
    enum_value_ex( emp::tim::time_c , ( EMP_RFX_R , highest_time       , __no_value ) )\
    enum_value_ex( float            , ( EMP_RFX_R , highest_success    , __no_value ) )\
    enum_value_ex( float            , ( EMP_RFX_R , highest_failure    , __no_value ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::music_i
#define type_fields
#define type_to_string a_rValue.name()
#define type_interface
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::game_c
#define type_fields\
    enum_value_ex( ase::cor::user_c*                   , ( EMP_RFX_B , user          , m_pUser         ) )\
    enum_value_ex( ase::mod::selection_c*              , ( EMP_RFX_B , mod_selection , m_pModSelection ) )\
    enum_value_ex( ase::mod::evolution_c*              , ( EMP_RFX_B , mod_evolution , m_pModEvolution ) )\
    enum_value_ex( ase::mod::emphase_c*                , ( EMP_RFX_B , mod_emphase   , m_pModEmphase   ) )\
    enum_value_ex( ase::mod::improvize_c*              , ( EMP_RFX_B , mod_improvize , m_pModImprovize ) )\
    enum_value_ex( emp::cnt::vector<ase::mod::mode_i*> , ( EMP_RFX_B , modes         , m_vpMods        ) )\
    enum_value_ex( emp::cnt::vector<ase::mod::mode_i*> , ( EMP_RFX_B , debug_modes   , m_vpDebugMods   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::composition_music_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( emp::snd::composition_c* , ( EMP_RFX_B , compo       , m_pComposition ) )\
    enum_value_ex( ase::cor::performance_c  , ( EMP_RFX_B , performance , m_Performance  ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::completion_option_s
#define type_fields\
    enum_value_ex( ase::cor::mod_e , ( EMP_RFX_B , mod              , mod              ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , shuffle          , shuffle          ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , score_visible    , score_visible    ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , score_letters    , score_letters    ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , score_durations  , score_durations  ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , keyboard_visible , keyboard_visible ) )\
    enum_value_ex( bool            , ( EMP_RFX_B , keyboard_letters , keyboard_letters ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::completion_bucket_c
#define type_fields\
    enum_value_ex( ase::cor::completion_option_s                 , ( EMP_RFX_B , options , m_Options ) )\
    enum_value_ex( emp::cnt::vector<ase::cor::completion_data_s> , ( EMP_RFX_B , data    , m_vData   ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::completion_data_s
#define type_fields\
    enum_value_ex( ase::cor::user_c* , ( EMP_RFX_B , user    , user    ) )\
    enum_value_ex( float             , ( EMP_RFX_B , score   , score   ) )\
    enum_value_ex( ui64              , ( EMP_RFX_B , time    , time    ) )\
    enum_value_ex( float             , ( EMP_RFX_B , success , success ) )\
    enum_value_ex( float             , ( EMP_RFX_B , failure , failure ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::user_data_s
#define type_fields\
    enum_value_ex( ase::cor::user_c* , ( EMP_RFX_B , user   , user   ) )\
    enum_value_ex( ui32              , ( EMP_RFX_B , played , played ) )\
    enum_value_ex( ui32              , ( EMP_RFX_B , rating , rating ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::album_c
#define type_to_string a_rValue.name()
#define type_fields\
    enum_value_ex( u8string                              , ( EMP_RFX_B , name   , m_su8Name   ) )\
    enum_value_ex( emp::gfx::model_c*                    , ( EMP_RFX_B , cover  , m_pGfxCover ) )\
    enum_value_ex( emp::cnt::vector<ase::cor::music_i* > , ( EMP_RFX_B , musics , m_vpMusics  ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define type_name ase::cor::album_list_c
#define type_fields\
    enum_value_ex( emp::cnt::vector<ase::cor::album_c*>  , ( EMP_RFX_B , albums , m_vpAlbums ) )
#include "emp_rfx_class.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

