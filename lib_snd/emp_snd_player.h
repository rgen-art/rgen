//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_PLAYER_H
#define EMP_SND_PLAYER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#define EMP_SND_PROFILE_MEDIA

#ifdef EMP_SND_PROFILE_MEDIA
#include "emp_mem_pointer.h"
namespace emp { namespace prf { class tim_profiler_c; } }
#endif

/*    emp::mat::sse_v4f::type const vtest =
        emp::mat::sse_v4f::set(0, 1, 2, 3);
    float const fx = emp::mat::sse_v4f::get_x(vtest);
    float const fy = emp::mat::sse_v4f::get_y(vtest);
    float const fz = emp::mat::sse_v4f::get_z(vtest);
    float const fw = emp::mat::sse_v4f::get_w(vtest);
    if (fx > 0){}
    if (fy > 0){}
    if (fz > 0){}
    if (fw > 0){}

    emp::mat::sse_v4f::type vnew = vtest;
    vnew = emp::mat::sse_v4f::set_x(vnew, 4);
    vnew = emp::mat::sse_v4f::set_y(vnew, 5);
    vnew = emp::mat::sse_v4f::set_z(vnew, 6);
    vnew = emp::mat::sse_v4f::set_w(vnew, 7);

    EMP_LOG_INFO("NEW #0, #1, #2, #3",
        emp::mat::sse_v4f::get_x(vnew),
        emp::mat::sse_v4f::get_y(vnew),
        emp::mat::sse_v4f::get_z(vnew),
        emp::mat::sse_v4f::get_w(vnew));*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace tim { class manager_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class media_i;
class mixer_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, player_c)
private:
    emp::cnt::vector<emp::snd::media_i*> m_vpMedias;
    emp::cnt::vector<emp::snd::media_i*> m_vpRelease;
#ifdef EMP_SND_PROFILE_MEDIA
    EMP_POINTER(emp::tim::profiler_c) m_pProfiler;
#endif
public:
    player_c(void);
    ~player_c(void);

    void create(void);
    void destroy(void);

    void update(void);
    void render(emp::snd::mixer_c& a_rMixer);

    void play(emp::snd::media_i* const a_pValue);
    void stop(emp::snd::media_i* const a_pValue);

    void release(emp::snd::media_i* const a_pValue);

public:
    EMP_RETURN emp::cnt::vector<emp::snd::media_i*> const& medias(void) const
    { return m_vpMedias; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN player_c& player(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

