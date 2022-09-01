//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_COMPOSITION_H
#define EMP_SND_COMPOSITION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class note_c;
class track_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, composition_c)
private:
    u8string m_su8Name;
    vector<track_c*> m_vpTracks;

public:
    composition_c(void);
    ~composition_c(void);

public:
    void destroy(void);

public:
    void merge(track_c& a_rTrack);

public:
    void set_name(c_u8string& a_su8Value) { m_su8Name = a_su8Value; }
    EMP_RETURN c_u8string& name(void) const { return m_su8Name; }

public:
    void add_track(track_c* const a_pValue) { m_vpTracks.add(a_pValue); }
    void remove_track(c_size a_stIndex) { m_vpTracks.remove(a_stIndex); }
    void swap_tracks(c_size a_st0, c_size a_st1) { m_vpTracks.swap(a_st0, a_st1); }
    EMP_RETURN size_t track_count(void) const { return m_vpTracks.count(); }

    EMP_RETURN track_c* track(c_size a_stIndex) { return m_vpTracks.val(a_stIndex); }
    EMP_RETURN track_c const* track(c_size a_stIndex) const { return m_vpTracks.val(a_stIndex); }

public:
    EMP_RETURN float beat_per_minute(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

