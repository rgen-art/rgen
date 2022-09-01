//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_SND_SOFT_SYNTH_H
#define EMP_SND_SOFT_SYNTH_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_snd_sound_i.h"
#include "emp_snd_synth_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_gui_control_i.h"
#include "emp_mat_evaluation.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace evl { class bind_c; } }
namespace emp { namespace evl { template <typename> class compiler_t; } }
namespace emp { namespace evl { template <typename> class function_t; } }
namespace emp { namespace evl { template <typename> class input_t; } }

namespace emp { namespace snd { class envelope_c; } }

namespace emp { namespace mat { class nurbs_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class spectrum_t;
template <typename> class delay_t;
template <typename> class reverb_t;

typedef emp::mat::precision_c::type precision_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name biquad_e
#define enum_values\
    enum_value( LowPass  )\
    enum_value( HighPass )\
    enum_value( BandPass )\
    enum_value( BandStop )\
    enum_value( AllPass  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, biquad_t)
private:
    //>> inputs
    biquad_e m_eType;
    T m_tF0;
    T m_tQ;
    T m_tSampleRate;
    //<< inputs

    //>> outputs
    T m_tOutputGain;
    T m_tOutputAmplitude;
    T m_tOutputPhase;
    T m_tOutputSignal;
    //<< outputs

    //>> tmp
    T m_ta0;
    T m_ta1;
    T m_ta2;

    T m_tb0;
    T m_tb1;
    T m_tb2;

    T m_tW0;

    T m_tSinW0;
    T m_tCosW0;

    T m_tAlpha;

    T m_tSamplePeriod;
    T m_tNyquist;
    T m_tToW;
    T m_tNegToW;
    //<< tmp

    T m_tZ1;
    T m_tZ2;

public:
    biquad_t(void);
    ~biquad_t(void);

public:
    void update(c_biquad_e a_eType,
        T const a_tF0, T const a_tQ,
        T const a_tSampleRate);

public:
    void compute_signal(T const a_tx0, T const a_tZ1, T const a_tZ2);
    void compute_transfer(T const a_tFrequency);

public:
    EMP_RETURN T output_gain(void) const
    { return m_tOutputGain; }
    EMP_RETURN T output_amplitude(void) const
    { return m_tOutputAmplitude; }
    EMP_RETURN T output_phase(void) const
    { return m_tOutputPhase; }
    EMP_RETURN T output_signal(void) const
    { return m_tOutputSignal; }

    EMP_RETURN T output_z1(void) const
    { return m_tZ1; }
    EMP_RETURN T output_z2(void) const
    { return m_tZ2; }

public:
    void dump_info(void) const;

private:
    void lowpass(void);
    void highpass(void);
    void bandpass(void);
    void bandstop(void);
    void allpass(void);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef precision_c::type precision_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 static_cast
#define enum_name sound_operation_e
#define enum_type
#define enum_values\
    enum_value( Delay    , static_cast<ui32>(operation_e::User) + 0)\
    enum_value( Reverb   , static_cast<ui32>(operation_e::User) + 1)\
    enum_value( Envelope , static_cast<ui32>(operation_e::User) + 2)\
    enum_value( Spectrum , static_cast<ui32>(operation_e::User) + 3)\
    enum_value( Biquad   , static_cast<ui32>(operation_e::User) + 4)\
    enum_value( Max      , static_cast<ui32>(operation_e::User) + 5)
#include "emp_tt_enum.def.h"

EMP_EVL_OP(sound_operation_e::Delay, 1, 4, delay_d    , noprec_d::value)
EMP_EVL_OP(sound_operation_e::Reverb  , 1, 5, reverb_d   , noprec_d::value)
EMP_EVL_OP(sound_operation_e::Envelope, 1, 0, envelope_d , noprec_d::value)
EMP_EVL_OP(sound_operation_e::Spectrum, 2, 2, spectrum_d , noprec_d::value)
EMP_EVL_OP(sound_operation_e::Biquad  , 4, 5, biquad_d   , noprec_d::value)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,envelope_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::mat::function_t<T> m_Function;
    EMP_POINTER(emp::mat::nurbs_c) m_pNurbs;
    EMP_POINTER(emp::snd::envelope_c) m_pEnvelope;
    vector_d* m_pv4Volumes;
    size_t m_stResult;
    size_t m_stSize;

public:
    envelope_t(void);
    ~envelope_t(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_Function.result(); }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return static_cast<operation_e>(operation_value); }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack);

    EMP_RETURN size_t input_count(void) const { return m_Function.input_count(); }
    EMP_RETURN size_t get_input(c_astring& a_sName) const { return m_Function.get_input(a_sName); }
    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_Function.get_input(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_Function.get_input(a_stIndex); }

    void link(void)
    {
        m_Function.link();
        m_stSize = arity() * precision_c::array_stride + m_Function.size();
    }

    EMP_RETURN function_t<T>* implementation(void) { return &m_Function; }
    EMP_RETURN bool user_result(c_size a_stIndex) const { return a_stIndex < result(); }
    //<< function_i

public:
    static const sound_operation_e operation_value = sound_operation_e::Envelope;
    static EMP_RETURN function_i<T>* allocate(void)
    { return EMP_ALLOCATE(envelope_t); }

public:
    void set_volume(c_size a_stStride, vector_d const a_fValue)
    {
        EMP_ASSERT(a_stStride < precision_c::array_stride);
        m_pv4Volumes[a_stStride] = a_fValue;
    }

public:
    EMP_RETURN emp::snd::envelope_c* envelope(void) { return m_pEnvelope; }
    EMP_RETURN emp::snd::envelope_c const* envelope(void) const { return m_pEnvelope; }

public:
    EMP_RETURN emp::mat::nurbs_c* nurbs(void) { return m_pNurbs; }
    EMP_RETURN emp::mat::nurbs_c const* nurbs(void) const { return m_pNurbs; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,spectrum_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::mat::function_t<T> m_Function;
    emp::snd::spectrum_t<T>* m_pData;
    size_t m_stResult;
    size_t m_stSize;

public:
    spectrum_t(void);
    ~spectrum_t(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_Function.result(); }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return static_cast<operation_e>(operation_value); }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack);

    EMP_RETURN size_t input_count(void) const { return m_Function.input_count(); }
    EMP_RETURN size_t get_input(c_astring& a_sName) const { return m_Function.get_input(a_sName); }
    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_Function.get_input(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_Function.get_input(a_stIndex); }

    void link(void)
    {
        m_Function.link();
        m_stSize = arity() * precision_c::array_stride + m_Function.size();
    }

    EMP_RETURN function_t<T>* implementation(void) { return &m_Function; }
    EMP_RETURN bool user_result(c_size a_stIndex) const { return a_stIndex == 1; }
    //<< function_i

public:
    static const sound_operation_e operation_value = sound_operation_e::Spectrum;
    static EMP_RETURN function_i<T>* allocate(void)
    { return EMP_ALLOCATE(spectrum_t); }

public:
    void set_data(emp::snd::spectrum_t<T>* const a_pValue)
    { m_pData = a_pValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,biquad_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::mat::function_t<T> m_Function;
    emp::snd::biquad_t<T> m_Biquad;
    size_t m_stResult;
    size_t m_stSize;

public:
    biquad_t(void);
    ~biquad_t(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_Function.result(); }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return static_cast<operation_e>(operation_value); }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack);

    EMP_RETURN size_t input_count(void) const { return m_Function.input_count(); }
    EMP_RETURN size_t get_input(c_astring& a_sName) const { return m_Function.get_input(a_sName); }
    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_Function.get_input(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_Function.get_input(a_stIndex); }

    void link(void)
    {
        m_Function.link();
        m_stSize = arity() * precision_c::array_stride + m_Function.size();
    }

    EMP_RETURN function_t<T>* implementation(void) { return &m_Function; }
    EMP_RETURN bool user_result(c_size a_stIndex) const { return a_stIndex == 1; }
    //<< function_i

public:
    static const sound_operation_e operation_value = sound_operation_e::Biquad;
    static EMP_RETURN function_i<T>* allocate(void)
    { return EMP_ALLOCATE(biquad_t); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,delay_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::mat::function_t<T> m_Function;
    emp::snd::delay_t<T>* m_pData;
    size_t m_stResult;
    size_t m_stSize;

public:
    delay_t(void);
    ~delay_t(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_Function.result(); }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return static_cast<operation_e>(operation_value); }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack);

    EMP_RETURN size_t input_count(void) const { return m_Function.input_count(); }
    EMP_RETURN size_t get_input(c_astring& a_sName) const { return m_Function.get_input(a_sName); }
    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_Function.get_input(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_Function.get_input(a_stIndex); }

    void link(void)
    {
        m_Function.link();
        m_stSize = arity() * precision_c::array_stride + m_Function.size();
    }

    EMP_RETURN function_t<T>* implementation(void) { return &m_Function; }
    EMP_RETURN bool user_result(c_size a_stIndex) const { return a_stIndex < result(); }
    //<< function_i

public:
    static const sound_operation_e operation_value = sound_operation_e::Delay;
    static EMP_RETURN function_i<T>* allocate(void)
    { return EMP_ALLOCATE(delay_t); }

public:
    void set_data(emp::snd::delay_t<T>* const a_pValue)
    { m_pData = a_pValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,reverb_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::mat::function_t<T> m_Function;
    emp::snd::reverb_t<T>* m_pData;
    size_t m_stResult;
    size_t m_stSize;

public:
    reverb_t(void);
    ~reverb_t(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_Function.result(); }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return static_cast<operation_e>(operation_value); }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rSTack);

    EMP_RETURN size_t input_count(void) const { return m_Function.input_count(); }
    EMP_RETURN size_t get_input(c_astring& a_sName) const { return m_Function.get_input(a_sName); }
    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_Function.get_input(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_Function.get_input(a_stIndex); }

    void link(void)
    {
        m_Function.link();
        m_stSize = arity() * precision_c::array_stride + m_Function.size();
    }

    EMP_RETURN function_t<T>* implementation(void) { return &m_Function; }
    EMP_RETURN bool user_result(c_size a_stIndex) const { return a_stIndex < result(); }
    //<< function_i

public:
    static const sound_operation_e operation_value = sound_operation_e::Reverb;
    static EMP_RETURN function_i<T>* allocate(void)
    { return EMP_ALLOCATE(reverb_t); }

public:
    void set_data(emp::snd::reverb_t<T>* const a_pValue)
    { m_pData = a_pValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::mat;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool get_unbound_variables(vector<emp::mat::input_t<T>*>* a_rvpInputs,
    emp::mat::function_i<T>* const a_pFunction, c_astring& a_sName);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, link_c)
private:
    ui32 m_ui32Function;
    ui32 m_ui32Index;
    size_t m_stBind;
    float m_fValue;

public:
    link_c(void):
        m_ui32Function(),
        m_ui32Index(),
        m_stBind(),
        m_fValue()
    { destroy(); }

    link_c(link_c const& a_rFrom):
        m_ui32Function(),
        m_ui32Index(),
        m_stBind(),
        m_fValue()
    { copy(a_rFrom); }

    ~link_c(void)
    {}

public:
    link_c& operator=(link_c const& a_rFrom)
    {
        copy(a_rFrom);
        return *this;
    }

public:
    void destroy(void)
    {
        EMP_TT_MAX_VAR(m_ui32Function);
        EMP_TT_MAX_VAR(m_ui32Index);
        EMP_TT_MAX_VAR(m_stBind);
        m_fValue = 0;
    }

    void copy(link_c const& a_rFrom)
    {
        m_ui32Function = a_rFrom.m_ui32Function;
        m_ui32Index = a_rFrom.m_ui32Index;
        m_stBind = a_rFrom.m_stBind;
        m_fValue = a_rFrom.m_fValue;
    }

public:
    void set_function(c_ui32 a_ui32Value) { m_ui32Function = a_ui32Value; }
    EMP_RETURN ui32 function(void) const { return m_ui32Function; }

    void set_index(c_ui32 a_ui32Value) { m_ui32Index = a_ui32Value; }
    EMP_RETURN ui32 index(void) const { return m_ui32Index; }

    void set_bind(c_size a_stValue) { m_stBind = a_stValue; }
    EMP_RETURN size_t bind(void) const { return m_stBind; }

    void set_value(c_float a_fValue) { m_fValue = a_fValue; }
    EMP_RETURN float value(void) const { return m_fValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, function_c)
private:
    astring m_sExpression;
    vector<link_c> m_vInputs;
    vector<v4f> m_vv4Params;

public:
    function_c(void):
        m_sExpression(),
        m_vInputs(),
        m_vv4Params()
    { destroy(); }

    function_c(function_c const& a_rFrom):
        m_sExpression(),
        m_vInputs(),
        m_vv4Params()
    { copy(a_rFrom); }

    ~function_c(void)
    {}

public:
    function_c& operator=(function_c const& a_rFrom)
    {
        copy(a_rFrom);
        return *this;
    }

public:
    void destroy(void)
    {
        m_sExpression = astring();
        m_vInputs.resize(0);
        m_vv4Params.resize(0);
    }

    void copy(function_c const& a_rFrom)
    {
        m_sExpression = a_rFrom.m_sExpression;

        { c_size stCount = a_rFrom.m_vInputs.count();
        m_vInputs.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vInputs.set(stIndex, a_rFrom.m_vInputs.val(stIndex)); } }

        { c_size stCount = a_rFrom.m_vv4Params.count();
        m_vv4Params.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vv4Params.set(stIndex, a_rFrom.m_vv4Params.val(stIndex)); } }
    }

public:
    void set_expression(c_astring& a_sValue) { m_sExpression = a_sValue; }
    EMP_RETURN c_astring& expression(void) const { return m_sExpression; }

    EMP_RETURN vector<link_c> const& inputs(void) const { return m_vInputs; }
    EMP_RETURN vector<link_c>& inputs(void) { return m_vInputs; }

    EMP_RETURN vector<v4f> const& params(void) const { return m_vv4Params; }
    EMP_RETURN vector<v4f>& params(void) { return m_vv4Params; }
};

template <typename V, typename T> void remove_in_vector(V& a_rVector, T const* const a_pValue);
template <typename V, typename T> void remove_in_vector(V& a_rVector, T const* const a_pValue)
{
    for (size_t stIndex = 0; stIndex < a_rVector.count();)
    {
        if (a_rVector.val(stIndex) == a_pValue)
        {
            a_rVector.remove(stIndex);
            continue;
        }
        ++stIndex;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class soft_sound_c;

EMP_NOCOPY(class, soft_synth_data_c)
private:
    typedef emp::cnt::vector<emp::mat::envelope_t<precision_d>*> envelopes_d;
    envelopes_d m_vpEnvelopes;

    typedef emp::cnt::vector<emp::mat::spectrum_t<precision_d>*> spectrums_d;
    spectrums_d m_vpSpectrums;

    typedef emp::cnt::vector<emp::mat::delay_t<precision_d>*> delays_d;
    delays_d m_vpDelays;

    typedef emp::cnt::vector<emp::mat::reverb_t<precision_d>*> reverbs_d;
    reverbs_d m_vpReverbs;

public:
    soft_synth_data_c(void):
        m_vpEnvelopes(),
        m_vpSpectrums(),
        m_vpDelays(),
        m_vpReverbs()
    {}
    ~soft_synth_data_c(void)
    {}

public:
    void destroy(void)
    {
        m_vpEnvelopes.resize(0);
        m_vpSpectrums.resize(0);
        m_vpDelays.resize(0);
        m_vpReverbs.resize(0);
    }

public:
    void copy(soft_synth_data_c const& a_rFrom)
    {
        { c_size stCount = a_rFrom.m_vpEnvelopes.count();
        m_vpEnvelopes.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpEnvelopes.set(stIndex, a_rFrom.m_vpEnvelopes.val(stIndex)); } }

        { c_size stCount = a_rFrom.m_vpSpectrums.count();
        m_vpSpectrums.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpSpectrums.set(stIndex, a_rFrom.m_vpSpectrums.val(stIndex)); } }

        { c_size stCount = a_rFrom.m_vpDelays.count();
        m_vpDelays.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpDelays.set(stIndex, a_rFrom.m_vpDelays.val(stIndex)); } }

        { c_size stCount = a_rFrom.m_vpReverbs.count();
        m_vpReverbs.resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpReverbs.set(stIndex, a_rFrom.m_vpReverbs.val(stIndex)); } }
    }

    void remove(emp::mat::function_i<precision_d>* a_pValue)
    {
        remove_in_vector(m_vpEnvelopes, a_pValue);
        remove_in_vector(m_vpSpectrums, a_pValue);
        remove_in_vector(m_vpDelays, a_pValue);
        remove_in_vector(m_vpReverbs, a_pValue);
    }

    EMP_RETURN envelopes_d& envelopes(void) { return m_vpEnvelopes; }
    EMP_RETURN spectrums_d& spectrums(void) { return m_vpSpectrums; }
    EMP_RETURN delays_d& delays(void) { return m_vpDelays; }
    EMP_RETURN reverbs_d& reverbs(void) { return m_vpReverbs; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,soft_synth_c, public, synth_i)
private:
    using tool_d = emp::mat::simd_v4<precision_d>;
    using vector_d = typename tool_d::type;

private:
    astring m_sName;
    vector<soft_sound_c*> m_vpSounds;
    quality_c* m_pQuality;
    vector<function_c> m_vFunctions;
    vector<emp::gui::info_s> m_vInfo;

private:
    soft_synth_data_c m_Data;
    vector<emp::mat::function_i<precision_d>*> m_vpFunctions;
    EMP_POINTER(emp::mat::compiler_t<precision_d>) m_pCompiler;
    EMP_POINTER(emp::mat::eval_stack_t<precision_d>) m_pStack;

    vector<emp::mat::function_i<precision_d>*> m_vpRelease;

    size_t m_stCellCount;

public:
    soft_synth_c(void);
    ~soft_synth_c(void);

public:
    void destroy(void);

public:
    EMP_RETURN bool post_load(void);

public:
    //>> layout_i
    EMP_RETURN c_astring& name(void) const { return m_sName; }

    EMP_RETURN size_t cell_count(void) const { return m_stCellCount; }
    EMP_RETURN cell_c const cell(c_size a_stIndex)
    {
        // @@0  : remove static_cast
        return cell_c(static_cast<ui32>(a_stIndex), this);
    }
    //<< layout_i

    //>> synth_i
    EMP_RETURN bool load(c_size EMP_SILENT(a_stIndex));
    EMP_RETURN sound_i* sound(c_float a_fFrequency);
    EMP_RETURN quality_c const* quality(void) const { return m_pQuality; }
//    EMP_RETURN bool scale(void) const;
    //<< synth_i

public:
    void set_name(c_astring& a_rsValue) { m_sName = a_rsValue; }
    void set_quality(emp::snd::quality_c* const a_pValue) { m_pQuality = a_pValue; }

public:
    EMP_RETURN size_t function_count(void) const { return m_vpFunctions.count(); }
    EMP_RETURN emp::mat::function_i<precision_d> const* function(c_size a_stIndex) const{ return m_vpFunctions.val(a_stIndex); }
    EMP_RETURN emp::mat::function_i<precision_d>* function(c_size a_stIndex) { return m_vpFunctions.val(a_stIndex); }

public:
    EMP_RETURN function_c const& get_function(c_size a_stIndex) const { return m_vFunctions.ref(a_stIndex); }
    EMP_RETURN function_c& get_function(c_size a_stIndex) { return m_vFunctions.ref(a_stIndex); }

    EMP_RETURN emp::gui::c_info_s& get_info(c_size a_stIndex) const { return m_vInfo.ref(a_stIndex); }
    EMP_RETURN emp::gui::info_s& get_info(c_size a_stIndex) { return m_vInfo.ref(a_stIndex); }

public:
    void set_function_expression(c_ui32 a_ui32Function, c_astring& a_sValue);
    EMP_RETURN c_astring& get_function_expression(c_size a_stIndex) const { return m_vFunctions.ref(a_stIndex).expression(); }

public:
    emp::mat::function_i<precision_d>* get_output(void);

public:
    EMP_RETURN ui32 add_function(void);
    void release_function(emp::mat::function_i<precision_d>* a_pValue);
    void remove_function(c_ui32 a_ui32Index);

    void link(c_ui32 a_ui32InFunction, c_ui32 a_ui32In, c_ui32 a_ui32OutFunction, c_ui32 a_ui32Out, c_bool a_bBuild);
    void unlink(c_size a_stInFunction, c_size a_stIn, c_bool a_bBuild);
    void relink(c_ui32 a_ui32InFunction, c_ui32 a_ui32In, c_bool a_bBuild);

    EMP_RETURN float get_value(c_size a_stInFunction, c_size a_stIn) const;

    void set_value(c_size a_stInFunction, c_size a_stIn, c_float a_fValue);

public:
    void build(void);

public:
    EMP_RETURN emp::mat::eval_stack_t<precision_d>* cash(void) { return m_pStack; }
    EMP_RETURN soft_synth_data_c& data(void) { return m_Data; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,soft_sound_c, public, sound_i)
private:
    using tool_d = emp::mat::simd_v4<precision_d>;
    using vector_d = typename tool_d::type;

private:
    float* m_pfSamples;
    vector<emp::mat::input_t<precision_d>*> m_vpVarX;
    vector<emp::mat::input_t<precision_d>*> m_vpVarIndex;
    emp::mat::function_i<precision_d>* m_pOutput;
    EMP_POINTER(emp::mat::eval_stack_t<precision_d>) m_pStack;
    emp::mat::eval_stack_t<precision_d>* m_pCash;
    size_t m_stChannelCount;
    size_t m_stRate;
    float m_fFrequency;

    bool m_bReleased;
    soft_synth_data_c m_Data;

public:
    soft_sound_c(void);
    ~soft_sound_c(void);

public:
    //>> sound_i
    void prepare_samples(c_size a_stIndex);
    EMP_RETURN size_t sample_count(void) const { return emp::tt::max<size_t>::value; }
    EMP_RETURN size_t channel_count(void) const { return m_stChannelCount; }
    EMP_RETURN float solve(c_size a_stChannel, c_size) const
    {
        EMP_ASSERT(a_stChannel < (m_stChannelCount * emp::mat::precision_c::data_stride));
        return m_pfSamples[a_stChannel];
    }
    void acquire(void) { m_bReleased = false; }
    void release(void) { m_bReleased = true; }
    EMP_RETURN bool released(void) const { return m_bReleased; }
    //<< sound_i

public:
    void set_synth(soft_synth_c* const a_pValue, emp::mat::eval_stack_t<precision_d>* const a_pStack);
    void set_frequency(float const a_fValue) { m_fFrequency = a_fValue; }

public:
    EMP_RETURN soft_synth_data_c* data(void) { return &m_Data; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

