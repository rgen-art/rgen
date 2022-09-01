//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_demo.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"

#include "emp_fs_file.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_container.h"
#include "emp_gui_skin.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, demo_c, public, emp::gui::container_c)
private:
    vector<astring> m_vsFiles{};
    vector<astring> m_vsDemo{};
    size_t m_stDemo = 0;
    emp::tim::time_c time{};
    emp::tim::time_c duration{};
    vector<float> durations{};
    size_t current_duration = 0;
    ui32 EMP_TT_MAX_VAR(previous_texture);
    ui32 EMP_TT_MAX_VAR(current_texture);
    bool m_bNext = true;
    emp::mat::random_c m_Random{};

public:
    demo_c(void) {}
    ~demo_c(void)
    {
        EMP_ASSERT_BOOL_CALL(stop());
    }

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock)
    {
        EMP_RET_FALSE_IF_NOT(tbase1::update(a_rClock));
        if (m_bNext)
        {
            time = emp::tim::time_c();
            if (m_vsDemo.is_valid_index(m_stDemo))
            {
                c_astring& sFile = m_vsDemo.ref(m_stDemo);
                bool bExists = false;
                EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sFile, bExists));
                if (bExists)
                {
                    if (emp::tt::is_not_max(previous_texture))
                    { EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.unlock_texture(previous_texture)); }

                    previous_texture = current_texture;
                    EMP_TT_MAX_VAR(current_texture);
                    EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.lock_texture(sFile, current_texture, EMP_GUI_SKIN.data->ui32_link, true));
                }
                m_bNext = false;
            }
            ++m_stDemo;
            ++current_duration;
        }

        if (durations.is_valid_index(current_duration))
        {
            time = emp::tim::time_c::add(time, a_rClock.delta());
            duration = emp::tim::time_c::from_seconds(durations.val(current_duration));
        }
        else
        { duration = emp::tim::time_c(); }
        if (emp::tim::time_c::more(time, duration))
        {
            m_bNext = true;
        }

        return true;
    }
    EMP_RETURN bool resize(void)
    {
        return tbase1::resize();
    }
    EMP_RETURN bool transform(emp::gui::stack_c& a_rStack)
    {
        return tbase1::transform(a_rStack);
    }
    EMP_RETURN bool render(void)
    {
        if (duration == emp::tim::time_c())
        { return true; }

        auto& rDevice = EMP_GRAPHICS;

        m44f m44Identity;
        emp::mat::m44f::identity(m44Identity);

        c_float fTime = emp::tt::minof(1.0f, 10 * emp::tim::time_c::to_seconds(time) / emp::tim::time_c::to_seconds(duration));

        EMP_RET_FALSE_IF_NOT(tbase1::render());
        if (emp::tt::is_not_max(previous_texture) &&
            emp::tt::is_not_max(current_texture))
        {

            EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 1 }));
            rDevice.set_blend(true);
            rDevice.set_depth_test(false);
            rDevice.set_depth_mask(false);
            rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
            rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
            rDevice.set_index_buffer(rDevice.quad_index_buffer());
            rDevice.set_program(rDevice.transition_programs(0));
            rDevice.set_uniform(uniform_e::m44Transform, m44Identity);

            rDevice.set_uniform(uniform_e::t2Texture_0, 0, previous_texture);
            rDevice.set_uniform(uniform_e::t2Texture_1, 1, current_texture);


            rDevice.set_uniform(uniform_e::fProgress, (emp::mat::saturate(fTime)));
            rDevice.draw(emp::gfx::primitive_e::Triangles);
            return true;
        }
        else if (emp::tt::is_max(previous_texture) &&
                 emp::tt::is_not_max(current_texture))
        {
            EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 1 }));

            rDevice.set_blend(true);
            rDevice.set_depth_test(false);
            rDevice.set_depth_mask(false);
            rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
            rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
            rDevice.set_index_buffer(rDevice.quad_index_buffer());
            rDevice.set_program(rDevice.texture_program());
            rDevice.set_uniform(uniform_e::m44Transform, m44Identity);
            rDevice.set_uniform(uniform_e::v4Color, v4f({1, 1, 1, 1}));

            rDevice.set_uniform(uniform_e::t2Texture_0, 0, current_texture);
            rDevice.draw(emp::gfx::primitive_e::Triangles);
            return true;
        }
        return true;
    }
    //<< control_i

    //>> handler_i
    EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s& a_rEvent)
    { return tbase1::process_keyboard(a_rEvent); }

    EMP_RETURN emp::gui::return_e process_mouse(emp::app::c_event_s& a_rEvent)
    { return tbase1::process_mouse(a_rEvent); }
    //<< handler_i

    EMP_RETURN bool create(void)
    {
        durations.resize(0);
        durations.add(1.1f);
        durations.add(1.0f);
        durations.add(0.9f);
        durations.add(0.8f);
        durations.add(0.7f);
        durations.add(0.6f);
        durations.add(0.5f);
        durations.add(0.4f);
        durations.add(0.3f);
        durations.add(0.2f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);
        durations.add(0.1f);

        return true;
    }

    EMP_RETURN bool start(void)
    {
        m_stDemo = 0;
        current_duration = 0;

        m_vsFiles.resize(0);
        m_vsDemo.resize(0);

        astring sPath = STR_AFORMAT("#0ose/", EMP_APPLICATION.resource_path());
        EMP_RET_FALSE_IF_NOT(emp::fs::list(sPath, ALITERAL("*.ose"), m_vsFiles, false));

        astring sThumb;

        c_size stCount = m_vsFiles.count();
        EMP_RET_FALSE_IF_NOT(stCount > 0);

        size_t stDurationCount = durations.count();


        for (size_t stIndex = 0; stIndex < stDurationCount; ++stIndex)
        {
            c_size stFile = m_Random.rand<size_t>(0, stCount);

            sThumb.clear();
            sThumb = m_vsFiles.ref(stFile);
            sThumb.trim_right(4);

            sPath.clear();
            emp::tt::silent(STR_FORMATA(sPath, "#0png/#1.png", EMP_APPLICATION.resource_path(), sThumb));

            bool bExists = false;
            EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sPath, bExists));
            if (bExists)
            { m_vsDemo.add(sPath); }
        }
        return true;
    }

    EMP_RETURN bool stop(void)
    {
        if (emp::tt::is_not_max(previous_texture))
        {
            EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.unlock_texture(previous_texture));
            EMP_TT_MAX_VAR(previous_texture);
        }

        if (emp::tt::is_not_max(current_texture))
        {
            EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.unlock_texture(current_texture));
            EMP_TT_MAX_VAR(current_texture);
        }

        return true;
    }

    EMP_RETURN bool ended(void) const
    {
        return m_stDemo >= m_vsDemo.count();
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
demo_c::demo_c(void):
demo{}
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
demo_c::~demo_c(void)
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool demo_c::public_create(void)
{
    EMP_RET_FALSE_IF_NOT(demo->create());

    demo->transformation().position_(0).size_(2);
    demo->state().dock_(emp::gui::dock_e::Fill);

    window().add_control(demo);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void demo_c::public_destroy(void)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool demo_c::public_start(void)
{
    set_pause(false);
    EMP_RET_FALSE_IF_NOT(demo->start());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void demo_c::public_stop(void)
{
    set_pause(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool demo_c::public_update(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool demo_c::condition_satisfied(void) const
{ return demo->ended(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

