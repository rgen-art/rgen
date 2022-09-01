##-----------------------------------------------------------------------------
## rgen.art
##-----------------------------------------------------------------------------
INCLUDES    += -I$(LIBXML2_H_DIR)

INCLUDES    += -I$(LIBPNG_H_DIR)

ifeq ($(TARGET_OS), lin)
INCLUDES    += -I$(X11_ROOT)
endif

ifeq ($(FREETYPE2_ROOT),)
else
INCLUDES    += -I$(FREETYPE2_ROOT)
endif

INCLUDES    += -I$(OPENAL_H_DIR)
INCLUDES    += -I$(OPENGL_H_DIR)
//INCLUDES    += -I$(LIBZ_H_DIR)

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
ifeq ($(LIBPNG_L_DIR),)
else
LIBRARIES += -L$(LIBPNG_L_DIR)
endif
LIBRARIES += $(LIBPNG_LIB)

ifeq ($(LIBXML2_L_DIR),)
else
LIBRARIES   += -L$(LIBXML2_L_DIR)
endif
LIBRARIES   += $(LIBXML2_LIB)

ifeq ($(OPENAL_L_DIR),)
else
LIBRARIES   += -L$(OPENAL_L_DIR)
endif
LIBRARIES   += $(OPENAL_LIB)

ifeq ($(OPENGL_L_DIR),)
else
LIBRARIES   += -L$(OPENGL_L_DIR)
endif
LIBRARIES   += $(OPENGL_LIB)

# ifeq ($(LIBZ_L_DIR),)
# else
# LIBRARIES += -L$(LIBZ_L_DIR)
# endif
# LIBRARIES += $(LIBZ_LIB)


ifeq ($(LIBJPEG_L_DIR),)
else
LIBRARIES += -L$(LIBJPEG_L_DIR)
endif
LIBRARIES += $(LIBJPEG_LIB)



ifeq ($(TARGET_OS), lin)
LIBRARIES   += -lX11
LIBRARIES   += -lXrandr
LIBRARIES   += -lXext
LIBRARIES   += -lpthread

##LIBRARIES   += -lopenal
LIBRARIES   += -lfftw3
endif

ifeq ($(TARGET_OS), osx)
LIBRARIES   += $(COCOA_LIB)
endif

ifeq ($(TARGET_OS), win)
#LIBRARIES   += -lwsock32
#LIBRARIES   += -liconv
LIBRARIES   += -lgdi32
#INCLUDES    += -I/mingw64/include/c++/5.4.0/
#INCLUDES    += -I/mingw64/include/c++/5.4.0/x86_64-w64-mingw32/
#LIBRARIES   += -lpthread
endif

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
SOURCES	?=

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
SOURCES +=  lib_xx/emp_xx_call.cpp

SOURCES +=  lib_ast/emp_ast_assert.cpp
SOURCES +=  lib_ast/emp_ast_errno.cpp

SOURCES +=  lib_mem/emp_mem_allocator.cpp
SOURCES	+=	lib_mem/emp_mem_util.cpp

SOURCES	+=	lib_cnt/emp_cnt_string.cpp

SOURCES	+=	lib_log/emp_log_logger.cpp

SOURCES +=  lib_prf/emp_prf_profiler.cpp

SOURCES	+=	lib_loc/emp_loc_locale.cpp
SOURCES	+=	lib_loc/emp_loc_localizer.cpp
SOURCES	+=	lib_loc/emp_loc_string.cpp

SOURCES +=  lib_rfx/emp_rfx_factory.cpp

SOURCES	+=	lib_tim/emp_tim_time.cpp

SOURCES	+=	lib_fs/emp_fs_file.cpp

SOURCES	+=	lib_xml/emp_xml_reader.cpp
SOURCES	+=	lib_xml/emp_xml_writer.cpp

SOURCES += 	lib_png/emp_png_util.cpp
#SOURCES +=  lib_png/spng.c


SOURCES +=  lib_mat/emp_mat_algebra.cpp
SOURCES +=  lib_mat/emp_mat_evaluation.cpp
SOURCES +=  lib_mat/emp_mat_glyph.cpp
SOURCES +=  lib_mat/emp_mat_nurbs.cpp

SOURCES	+=	lib_gfx/emp_gfx_gl_device.cpp
SOURCES +=  lib_gfx/emp_gfx_icon.cpp
SOURCES +=  lib_gfx/emp_gfx_model.cpp
SOURCES	+=	lib_gfx/emp_gfx_nurbs.cpp
SOURCES	+=	lib_fnt/emp_gfx_text.cpp

SOURCES	+=	lib_gui/emp_gui_button.cpp
SOURCES	+=	lib_gui/emp_gui_combo.cpp
SOURCES	+=	lib_gui/emp_gui_container.cpp
SOURCES	+=	lib_gui/emp_gui_control_i.cpp
SOURCES	+=	lib_gui/emp_gui_coordinate.cpp
SOURCES	+=	lib_gui/emp_gui_dialog.cpp
SOURCES	+=	lib_gui/emp_gui_docker.cpp
SOURCES	+=	lib_gui/emp_gui_frame.cpp
SOURCES	+=	lib_gui/emp_gui_function.cpp
SOURCES	+=	lib_gui/emp_gui_glyph.cpp
SOURCES	+=	lib_gui/emp_gui_graph.cpp
SOURCES	+=	lib_gui/emp_gui_icon.cpp
SOURCES	+=	lib_gui/emp_gui_label.cpp
SOURCES	+=	lib_gui/emp_gui_list.cpp
SOURCES	+=	lib_gui/emp_gui_manager.cpp
SOURCES	+=	lib_gui/emp_gui_mdi.cpp
SOURCES	+=	lib_gui/emp_gui_menu.cpp
SOURCES	+=	lib_gui/emp_gui_nurbs.cpp
SOURCES	+=	lib_gui/emp_gui_outline.cpp
SOURCES	+=	lib_gui/emp_gui_panel.cpp
SOURCES	+=	lib_gui/emp_gui_picture.cpp
SOURCES	+=	lib_gui/emp_gui_plot.cpp
SOURCES	+=	lib_gui/emp_gui_popup.cpp
SOURCES	+=	lib_gui/emp_gui_property.cpp
SOURCES	+=	lib_gui/emp_gui_registrar.cpp
SOURCES	+=	lib_gui/emp_gui_resolution.cpp
SOURCES	+=	lib_gui/emp_gui_scroller.cpp
SOURCES	+=	lib_gui/emp_gui_sizer.cpp
SOURCES	+=	lib_gui/emp_gui_skin.cpp
SOURCES	+=	lib_gui/emp_gui_slider.cpp
SOURCES	+=	lib_gui/emp_gui_spacer.cpp
SOURCES	+=	lib_gui/emp_gui_splitter.cpp
SOURCES	+=	lib_gui/emp_gui_text.cpp
SOURCES	+=	lib_gui/emp_gui_toggle.cpp
SOURCES +=  lib_gui/emp_gui_toolbar.cpp
SOURCES	+=	lib_gui/emp_gui_window.cpp

SOURCES	+=	lib_snd/emp_snd_al_device.cpp
SOURCES	+=	lib_snd/emp_snd_alc_context.cpp
SOURCES	+=	lib_snd/emp_snd_composition.cpp
SOURCES	+=	lib_snd/emp_snd_envelope.cpp
SOURCES	+=	lib_snd/emp_snd_layout.cpp
SOURCES	+=	lib_snd/emp_snd_media.cpp
SOURCES	+=	lib_snd/emp_snd_mixer.cpp
SOURCES	+=	lib_snd/emp_snd_player.cpp
SOURCES	+=	lib_snd/emp_snd_sample.cpp
SOURCES	+=	lib_snd/emp_snd_scale.cpp
SOURCES	+=	lib_snd/emp_snd_soft_synth.cpp
SOURCES	+=	lib_snd/emp_snd_sound.cpp
SOURCES	+=	lib_snd/emp_snd_track.cpp

##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
SOURCES +=  lib_app/emp_app_application.cpp
SOURCES +=  lib_app/emp_app_argument.cpp

ifeq ($(TARGET_OS), lin)

SOURCES	+=   lib_app/emp_app_platform_lin.cpp

else
ifeq ($(TARGET_OS), osx)

SOURCES +=  lib_app/emp_app_platform_osx.mm

else
ifeq ($(TARGET_OS), win)

SOURCES +=  lib_app/emp_app_platform_win.cpp

endif
endif
endif


##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
SOURCES += lib_ose/emp_ose_compiler.cpp
SOURCES += lib_ose/emp_ose_lexeme.cpp
SOURCES += lib_ose/emp_ose_processor.cpp
SOURCES += lib_ose/emp_ose_program.cpp
SOURCES += lib_ose/emp_ose_renderer.cpp

SOURCES	+=	ase_cor_emphase.cpp
SOURCES	+=	ase_cor_option.cpp
SOURCES +=  ase_cor_selection.cpp

SOURCES	+=	ase_gui_score.cpp
SOURCES +=  ase_gui_skin.cpp

SOURCES +=  ase_mod_compose.cpp
SOURCES +=  ase_mod_debug.cpp
SOURCES +=  ase_mod_demo.cpp
SOURCES	+=	ase_mod_emphase.cpp
SOURCES +=  ase_mod_evolution.cpp
SOURCES +=  ase_mod_improvize.cpp
SOURCES	+=	ase_mod_mode_a.cpp
SOURCES +=  ase_mod_music.cpp
SOURCES +=  ase_mod_selection.cpp
SOURCES	+=	ase_mod_synthesize.cpp
SOURCES	+=	ase_mod_text.cpp
SOURCES +=  ase_mod_rgen.cpp

SOURCES	+=	ase_cor_game.cpp



SOURCES +=	bin_rgen/main.cpp
##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------

