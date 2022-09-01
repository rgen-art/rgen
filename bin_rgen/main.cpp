//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
//#include "main.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "ase_cor_emphase.h"

#include "ase_sol_music.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_argument.h"
#include "emp_app_configuration.h"
#include "emp_app_event.h"
#include "emp_app_platform.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_skin.h"

#include "emp_rfx_factory.h"



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_reflection.rfx.h"
#include "emp_gfx_reflection.rfx.h"
#include "emp_gui_reflection.rfx.h"
#include "emp_loc_reflection.rfx.h"
#include "emp_mat_reflection.rfx.h"
#include "emp_snd_reflection.rfx.h"
#include "emp_ttf_reflection.rfx.h"

#include "ase_cor_reflection.rfx.h"
#include "ase_mod_reflection.rfx.h"


#include "emp_ose_compiler.h"
#include "emp_ose_processor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::app;
using namespace emp::cnt;
using namespace emp::fct;
using namespace emp::log;
using namespace emp::str;
using namespace emp::tt;

using namespace ase::sol;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, main_c)
private:
    argument_t<bool> m_Clear{};
    argument_t<bool> m_Silent{};
    argument_t<bool> m_Vsync{};
    argument_t<bool> m_Fullscreen{};
    argument_t<bool> m_Profile{};
    argument_t<bool> m_Fps{};
    argument_t<si32> m_Width{};
    argument_t<si32> m_Height{};
    argument_t<si32> m_Log{};
    argument_t<si32> m_Verbose{};
    argument_t<float> m_Pixel{};
    argument_t<astring> m_Language{};
    argument_t<astring> m_Save{};
    argument_t<astring> m_Mapping{};
    argument_t<astring> m_Layout{};
    argument_t<astring> m_Data{};
#if defined EMP_UNIT_TEST
    argument_t<ui32> m_Unit{};
    argument_t<bool> m_UnitKill{};
    argument_t<bool> m_UnitLog{};
    argument_t<si32> m_UnitSkip{};
    argument_t<astring> m_UnitFile{};
#endif
    bool m_bTested = false;

public:
    configuration_s m_Config{};
    platform_i* m_pPlatform = nullptr;
    std::recursive_mutex m_oMutex{};

public:
    main_c(void)
    {}

    EMP_RETURN bool run(c_int a_siArgc, cpc_char* const a_aszArgv)
    {
        emp::tim::clock_c oClock;
        emp::tim::clock_c::set_instance(&oClock);

        emp::mem::allocator_c oAllocator;
        emp::mem::allocator_c::set_instance(&oAllocator);

        emp::log::logger_c   ::set_instance(EMP_ALLOCATE(emp::log::logger_c));
        emp::loc::localizer_c::set_instance(EMP_ALLOCATE(emp::loc::localizer_c));
        emp::xml::parser_c   ::set_instance(EMP_ALLOCATE(emp::xml::parser_c));
        emp::rfx::factory_c  ::set_instance(EMP_ALLOCATE(emp::rfx::factory_c));
        emp::gfx::gl_device_c::set_instance(EMP_ALLOCATE(emp::gfx::gl_device_c));
        application_c        ::set_instance(EMP_ALLOCATE(emp::app::application_c));

        m_pPlatform = emp::app::new_platform();

        m_Config.argc           = a_siArgc;
        m_Config.argv           = a_aszArgv;
        m_Config.title          = ALITERAL("rgen");
        m_Config.centered       = true;
        m_Config.maximized      = false;
        m_Config.x              = 0;
        m_Config.y              = 0;

        m_Config.buffer_size    = 32;
        m_Config.red_bpp        = 8;
        m_Config.green_bpp      = 8;
        m_Config.blue_bpp       = 8;
        m_Config.alpha_bpp      = 8;
        m_Config.depth_bpp      = 24;
        m_Config.stencil_bpp    = 0;
        m_Config.sample_buffers = 0;
        m_Config.samples        = 0;
        m_Config.stereo         = false;
        m_Config.double_buffer  = true;
        m_Config.fps            = true;

        m_Config.on_create      = on_create_f      (function( this, &main_c::on_create      ));
        m_Config.on_parse       = on_parse_f       (function( this, &main_c::on_parse       ));
        m_Config.on_reflect     = on_reflect_f     (function( this, &main_c::on_reflect     ));
        m_Config.on_init        = on_init_f        (function( this, &main_c::on_init        ));
        m_Config.on_deserialize = on_deserialize_f (function( this, &main_c::on_deserialize ));
        m_Config.on_exit        = on_exit_f        (function( this, &main_c::on_exit        ));
        m_Config.on_unit_test   = on_unit_test_f   (function( this, &main_c::on_unit_test   ));
        return EMP_APPLICATION.run(m_Config, m_pPlatform, m_oMutex);
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_create(configuration_s& a_rConfiguration)
    {
#define XTMP_ARG(x_Name, ...)\
        {\
            x_Name.init(__VA_ARGS__);\
            EMP_APPLICATION.add_argument(&x_Name);\
        }

        XTMP_ARG( m_Clear      , 'c' , ALITERAL("clear"     ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_Silent     , 't' , ALITERAL("silent"    ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_Vsync      , 'v' , ALITERAL("vsync"     ), ALITERAL(""), true                                  )
        XTMP_ARG( m_Fullscreen , 'f' , ALITERAL("fullscreen"), ALITERAL(""), false, true                           )
        XTMP_ARG( m_Profile    , 'x' , ALITERAL("profile"   ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_Fps        , 'r' , ALITERAL("fps"       ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_Width      , 'w' , ALITERAL("width"     ), ALITERAL(""), 1024                                  )
        XTMP_ARG( m_Height     , 'h' , ALITERAL("height"    ), ALITERAL(""), 768                                   )
        XTMP_ARG( m_Log        , '!' , ALITERAL("log"       ), ALITERAL(""), enum_to__(log_e::none)                )
        XTMP_ARG( m_Verbose    , '*' , ALITERAL("verbose"   ), ALITERAL(""), enum_to__(verbose_e::Type)            )
        XTMP_ARG( m_Pixel      , 'p' , ALITERAL("pixel"     ), ALITERAL(""), 1                                     )
        XTMP_ARG( m_Language   , '@' , ALITERAL("language"  ), ALITERAL(""), ALITERAL("emp_loc_locale_en_US.xml" ) )
        XTMP_ARG( m_Save       , 's' , ALITERAL("save"      ), ALITERAL("")                                        )
        XTMP_ARG( m_Mapping    , 'M' , ALITERAL("mapping"   ), ALITERAL("")                                        )
        XTMP_ARG( m_Layout     , 'y' , ALITERAL("layout"    ), ALITERAL("")                                        )
        XTMP_ARG( m_Data       , 'd' , ALITERAL("data"      ), ALITERAL(""), ALITERAL("data/"                    ) )
#if defined EMP_UNIT_TEST
        XTMP_ARG( m_Unit       , 'U' , ALITERAL("unit"      ), ALITERAL(""), 0,     1024                           )
        XTMP_ARG( m_UnitLog    , 'L' , ALITERAL("ulog"      ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_UnitKill   , 'K' , ALITERAL("ukill"     ), ALITERAL(""), false, true                           )
        XTMP_ARG( m_UnitSkip   , 'S' , ALITERAL("uskip"     ), ALITERAL(""), 0, 0                           )
        XTMP_ARG( m_UnitFile   , 'F' , ALITERAL("ufile"     ), ALITERAL("")                                        )
#endif
#undef XTMP_ARG

        // @@3 constexpr check
        a_rConfiguration.usage = ALITERAL(
            "(((clear|save)>silent)|(unit>(ukill|ufile|ulog))|language|log|verbose|width|height|fullscreen|mapping|layout|vsync|pixel|profile|fps|data)");

        return true;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_parse(configuration_s& a_rConfiguration)
    {
        a_rConfiguration.vsync          = m_Vsync.value();
#if defined(EMP_XX_OS_OSX) || defined(EMP_XX_OS_LIN) || defined(EMP_XX_OS_WIN)
        a_rConfiguration.width          = m_Width.value();
        a_rConfiguration.height         = m_Height.value();
#endif
        a_rConfiguration.fullscreen     = m_Fullscreen.value();
        a_rConfiguration.silent         = m_Silent.value();

        c_log_e eLog = static_cast<log_e>(m_Log.value());
        EMP_RET_FALSE_IF_NOT(enum_is_valid(eLog), "Invalid log level : #0", m_Log.value());
        EMP_LOGGER.m_eLogLevel = eLog;

        c_verbose_e eVerbose = static_cast<verbose_e>(m_Verbose.value()); // @@0 enum_cast
        EMP_RET_FALSE_IF_NOT(enum_is_valid(eVerbose), "Invalid verbose level : #0", m_Verbose.value());
        EMP_LOGGER.m_eVerboseLevel = eVerbose;

        a_rConfiguration.data = m_Data.value();

        return true;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_reflect(void)
    {
#define XTMP_ADD_TYPE(...)\
        {\
            typedef __VA_ARGS__ type;\
            EMP_RET_FALSE_IF(EMP_STORE(type) == nullptr, "Failed to add type #0", ALITERAL(EMP_PP_STRING(__VA_ARGS__)));\
        }

        XTMP_ADD_TYPE( emp::loc::locale_c            );
        XTMP_ADD_TYPE( emp::loc::string_s            );
        XTMP_ADD_TYPE( emp::mat::glyph_s             );
        XTMP_ADD_TYPE( emp::mat::outline_s           );
        XTMP_ADD_TYPE( emp::gfx::buffer_t<ui8>       );
        XTMP_ADD_TYPE( emp::gfx::buffer_t<ui16>      );
        XTMP_ADD_TYPE( emp::gfx::buffer_t<v2f, v4f>  );
        XTMP_ADD_TYPE( emp::gfx::buffer_t<v4f, v4f>  );
        XTMP_ADD_TYPE( emp::gfx::mesh_c              );
        XTMP_ADD_TYPE( emp::gfx::model_c             );
        XTMP_ADD_TYPE( emp::gfx::font_c              );
        XTMP_ADD_TYPE( emp::gfx::icon_c              );
        XTMP_ADD_TYPE( emp::gui::skin_data_s         );
        XTMP_ADD_TYPE( emp::gui::skin_c              );
        XTMP_ADD_TYPE( emp::gui::manager_c           );
        XTMP_ADD_TYPE( emp::gui::button_c            );
        XTMP_ADD_TYPE( emp::gui::label_c             );
        XTMP_ADD_TYPE( emp::gui::color_s             );
        XTMP_ADD_TYPE( emp::gui::docker_c            );
        XTMP_ADD_TYPE( emp::gui::window_c            );
        XTMP_ADD_TYPE( emp::gui::menu_c              );
        XTMP_ADD_TYPE( emp::mat::nurbs_c             );
        XTMP_ADD_TYPE( emp::snd::scale_c             );
        XTMP_ADD_TYPE( emp::snd::soft_synth_c        );
        XTMP_ADD_TYPE( emp::snd::quality_c           );
        XTMP_ADD_TYPE( emp::snd::envelope_c          );
        XTMP_ADD_TYPE( emp::snd::sound_c             );
        XTMP_ADD_TYPE( emp::snd::sample_c            );
        XTMP_ADD_TYPE( emp::snd::layout_c            );
        XTMP_ADD_TYPE( emp::snd::track_c             );
        XTMP_ADD_TYPE( emp::snd::composition_c       );
        XTMP_ADD_TYPE( ase::cor::pool_c              );
        XTMP_ADD_TYPE( ase::cor::pool_list_c         );
        XTMP_ADD_TYPE( ase::cor::user_c              );
        XTMP_ADD_TYPE( ase::cor::album_c             );
        XTMP_ADD_TYPE( ase::cor::album_list_c        );
        XTMP_ADD_TYPE( ase::cor::composition_music_c );
        XTMP_ADD_TYPE( ase::cor::random_music_c      );
        XTMP_ADD_TYPE( ase::cor::completion_bucket_c );
        XTMP_ADD_TYPE( ase::cor::mapping_c           );
        XTMP_ADD_TYPE( ase::mod::text_c              );
        XTMP_ADD_TYPE( ase::mod::demo_c              );
        XTMP_ADD_TYPE( ase::mod::debug_c             );
        XTMP_ADD_TYPE( ase::mod::emphase_c           );
        XTMP_ADD_TYPE( ase::mod::selection_c         );
        XTMP_ADD_TYPE( ase::mod::evolution_c         );
        XTMP_ADD_TYPE( ase::mod::music_c             );
        XTMP_ADD_TYPE( ase::mod::improvize_c         );
        XTMP_ADD_TYPE( ase::mod::synthesize_c        );
        XTMP_ADD_TYPE( ase::mod::compose_c           );
        XTMP_ADD_TYPE( ase::mod::rgen_c              );
        XTMP_ADD_TYPE( ase::cor::game_c              );

#undef XTMP_ADD_TYPE

        return true;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_init(void)
    {
        emp::gui::skin_c* const pSkin = EMP_STORE(emp::gui::skin_c)->get_object(ALITERAL("emp_gui_skin.xml"));
        EMP_RET_FALSE_IF(pSkin == nullptr, "failed to bind gui skin");
        EMP_APPLICATION.gui_skin(pSkin);

        emp::gui::manager_c* const pGuiManager = EMP_STORE(emp::gui::manager_c)->get_object(ALITERAL("emp_gui_manager.xml"));
        EMP_RET_FALSE_IF(pGuiManager == nullptr, "failed to bind gui manager");
        EMP_APPLICATION.gui_manager(pGuiManager);

        EMP_LOG_INFO("binding game instance");
        ase::cor::game_c* const pGame = EMP_STORE(ase::cor::game_c)->get_object(ALITERAL("ase_cor_game.xml"));
        EMP_RET_FALSE_IF(pGame == nullptr, "failed to bind game instance");
        EMP_APPLICATION.game(pGame);

        ase::cor::game_c::set_instance(pGame);

        return true;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_deserialize(void)
    {
        // @@0 remove
        //if (m_Ose.value().len() > 0)
        //{
        //    EMP_LOG_INFO("Loading #0", m_Ose.value());
        //    ase::mod::improvize_c* const pModImprovize = EMP_STORE(ase::mod::improvize_c)->get_object(ALITERAL("ase_mod_improvize.xml"));
        //    EMP_RET_FALSE_IF(pModImprovize == nullptr, "Failed to load #0", m_Ose.value());
        //    pModImprovize->set_ose(m_Ose.value(), m_Profile.value());
        //    pModImprovize->set_pixel(m_Pixel.value());
        //}

        if (m_Mapping.value().len() > 0)
        {
            ase::cor::mapping_c*const pMapping =EMP_STORE(ase::cor::mapping_c)->get_object(m_Mapping.value());
            EMP_RET_FALSE_IF(pMapping == nullptr, "Failed to load mapping #0", m_Mapping.value());
            ASE_GAME.user().options.emphase.keyboard.mapping = pMapping;
        }

        if (m_Layout.value().len() > 0)
        {
            emp::snd::layout_c* const pLayout = EMP_STORE(emp::snd::layout_c)->get_object(m_Layout.value());
            EMP_RET_FALSE_IF(pLayout == nullptr, "Failed to load layout #0", m_Layout.value());
            ASE_GAME.track()->set_layout(pLayout);
        }

        c_astring& sLanguage(m_Language.value());
        EMP_RET_FALSE_IF_NOT(EMP_LOCALIZER.is_valid_language(sLanguage), "Invalid language : #0", m_Language.value());
        EMP_RET_FALSE_IF_NOT(EMP_LOCALIZER.create(sLanguage));

        return true;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_exit(c_bool a_bError)
    {
        EMP_LOG_INFO("exiting...");
        if (a_bError)
        {
            if (m_pPlatform != nullptr)
            {
                auto* const pPlatform = m_pPlatform;
                m_pPlatform = nullptr;

                m_Config = emp::app::configuration_s();

                m_Language.destroy();
                m_Save.destroy();
                m_Mapping.destroy();
                m_Layout.destroy();
                m_Data.destroy();

                EMP_DEALLOCATE(&EMP_FACTORY);

                EMP_RET_FALSE_IF_NOT(pPlatform->current_context());
                if (EMP_GRAPHICS.valid())
                { EMP_RET_FALSE_IF_NOT(EMP_APPLICATION.bind_platform()); }

                EMP_DEALLOCATE(&EMP_GRAPHICS);
                EMP_DEALLOCATE(pPlatform);

                EMP_DEALLOCATE(&EMP_APPLICATION);

                EMP_DEALLOCATE(&EMP_XML_PARSER);
                EMP_DEALLOCATE(&EMP_LOCALIZER);
                EMP_DEALLOCATE(&EMP_LOGGER);
                EMP_ALLOCATOR.destroy();
            }

            return true;
        }
        else
        {
            if (m_Clear.value())
            {
                EMP_LOG_INFO("Clearing...");
                EMP_STORE(ase::cor::composition_music_c)->touch(); //@@0 working ?
                EMP_STORE(ase::cor::random_music_c)->touch(); // @@0 working ?
            }

            c_bool bAll = emp::str::cmp(m_Save.value(), astring(ALITERAL("all"))); // @@0 explicit
            c_bool bSave = m_Save.is_active() && (m_Save.value().len() == 0 || bAll);

            bool bReturn = true;
            if (bSave)
            {
                EMP_LOG_INFO("Writing emp::rfx::factory_c : #0", bAll ? ALITERAL("ALL") : ALITERAL("changes"));
                bReturn = EMP_FACTORY.write(bAll);

                if (bReturn == false)
                { EMP_LOG_ERROR("Failed to write emp::rfx::factory_c"); }

                // @@0  : some HC here alc_context destruction before sound destruction
            }
            else
            {
                EMP_LOG_WARNING("Readonly factory");
            }
            
            return bReturn;
        }
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EMP_RETURN bool on_unit_test(void)
    {
        if (m_bTested == false)
        {
            if (m_Unit.value() > 1)
            {
                EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().m_pCompiler->unit_test(EMP_APPLICATION.resource_path(), m_UnitKill.value(), m_UnitLog.value(), m_UnitFile.value()));
            }

            if (m_Unit.value() > 0)
            {
                EMP_RET_FALSE_IF_NOT(ASE_GAME.ose_renderer().unit_test(EMP_APPLICATION.resource_path(), 1.0f/*EMP_GUI_SKIN.resolution.current()*/, m_UnitKill.value(), m_UnitLog.value(), m_UnitFile.value(), m_UnitSkip.value()));
            }
            m_bTested = true;
        }
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(c_int a_siArgc, cpc_char* const a_aszArgv);
int main(c_int a_siArgc, cpc_char* const a_aszArgv)
{ return main_c().run(a_siArgc, a_aszArgv); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

