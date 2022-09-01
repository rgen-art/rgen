//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_snd_soft_synth.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_foreach.h"

#include "emp_hh_complex.h"
#include "emp_hh_math.h"

#include "emp_mat_nurbs.h"

#include "emp_mem_pointer.h"

#include "emp_snd_envelope.h"
#include "emp_snd_quality.h"
#include "emp_snd_media.h"
#include "emp_snd_scale_i.h"

#include "emp_tim_time.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
biquad_t<T>::biquad_t(void):
    m_eType(),
    m_tF0(),
    m_tQ(),
    m_tSampleRate(44100),
    m_tOutputGain(),
    m_tOutputAmplitude(),
    m_tOutputPhase(),
    m_tOutputSignal(),
    m_ta0(),
    m_ta1(),
    m_ta2(),
    m_tb0(),
    m_tb1(),
    m_tb2(),
    m_tW0(),
    m_tSinW0(),
    m_tCosW0(),
    m_tAlpha(),
    m_tSamplePeriod(),
    m_tNyquist(),
    m_tToW(),
    m_tNegToW(),
    m_tZ1(),
    m_tZ2()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
biquad_t<T>::~biquad_t(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::update(c_biquad_e a_eType,
    T const a_tF0, T const a_tQ, T const a_tSampleRate)
{
    m_tNyquist = a_tSampleRate / 2;
    T const tF0 = emp::tt::minof(m_tNyquist, a_tF0);

    T const tEps = 0.00000000001f;
    T const tQ = emp::mat::abs(a_tQ) < tEps ? tEps : a_tQ;

    if (m_eType != a_eType ||
        emp::tt::equal_<T>(m_tF0, tF0) == false ||
        emp::tt::equal_<T>(m_tQ, tQ) == false ||
        emp::tt::equal_<T>(m_tSampleRate, a_tSampleRate) == false)
    {
        m_eType = a_eType;
        m_tF0 = tF0;
        m_tQ = tQ;
        m_tSampleRate = a_tSampleRate;

        m_tSamplePeriod = 1 / m_tSampleRate;
        m_tToW = 2 * emp::mat::pi<T>() * m_tSamplePeriod;
        m_tNegToW = -m_tToW;

        m_tW0 = m_tToW * m_tF0;
        m_tSinW0 = emp::mat::sin(m_tW0);
        m_tCosW0 = emp::mat::cos(m_tW0);

        m_tAlpha = m_tSinW0 / (2 * m_tQ);

        switch (m_eType)
        {
            case biquad_e::LowPass:
            { lowpass(); break; }
            case biquad_e::HighPass:
            { highpass(); break; }
            case biquad_e::BandPass:
            { bandpass(); break; }
            case biquad_e::BandStop:
            { bandstop(); break; }
            case biquad_e::AllPass:
            { allpass(); break; }
            default:
            { EMP_ASSERT_UNREACHABLE(); return; }
        }

        m_tb0 /= m_ta0;
        m_tb1 /= m_ta0;
        m_tb2 /= m_ta0;
        m_ta1 /= m_ta0;
        m_ta2 /= m_ta0;

        //dump_info();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::compute_signal(T const a_x0, T const a_tZ1, T const a_tZ2)
{
    /*m_tOutputSignal =
        m_tb0 * a_x0 +
        m_tb1 * a_x1 +
        m_tb2 * a_x2 -
        m_ta1 * a_y1 -
        m_ta2 * a_y2;*/

    //out = in * a0 + z1;
    m_tZ1 = a_tZ1;
    m_tZ2 = a_tZ2;

    m_tOutputSignal = a_x0 * m_tb0 + m_tZ1;

    //z1 = in * a1 + z2 - b1 * out;
    m_tZ1 = a_x0 * m_tb1 + m_tZ2 - m_ta1 * m_tOutputSignal;

    //z2 = in * a2 - b2 * out;    m_tOutputSignal =
    m_tZ2 = a_x0 * m_tb2 - m_ta2 * m_tOutputSignal;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::compute_transfer(T const a_tFrequency)
{
    T const tF = emp::tt::minof(m_tNyquist, a_tFrequency);
    T const tW = m_tNegToW * tF;

    std::complex<T> const oZ1 = std::polar<T>(1, tW);
    std::complex<T> const oZ2 = std::polar<T>(1, 2 * tW);

    //num
    std::complex<T> oNum2(oZ2);
    oNum2*= m_tb2;

    std::complex<T> oNum1(oZ1);
    oNum1*= m_tb1;

    std::complex<T> oNum(m_tb0);
    oNum += oNum1;
    oNum += oNum2;

    //den
    std::complex<T> oDen2(oZ2);
    oDen2*= m_ta2;

    std::complex<T> oDen1(oZ1);
    oDen1*= m_ta1;

    std::complex<T> oDen(1.0);
    oDen+= oDen1;
    oDen+= oDen2;

    //res
    std::complex<T> oRes(oNum);
    oRes/=oDen;

    m_tOutputAmplitude = std::abs(oRes);
    EMP_ASSERT(emp::mat::is_valid(static_cast<float>(m_tOutputAmplitude)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::dump_info(void) const
{
    EMP_LOG_INFO("------------------------------------");
    EMP_LOG_INFO("Filter Description");
    EMP_LOG_INFO("    Type");
    switch (m_eType)
    {
        case emp::snd::biquad_e::LowPass:
        { EMP_LOG_INFO("        lowpass"); break; }
        case emp::snd::biquad_e::HighPass:
        { EMP_LOG_INFO("        highpass"); break; }
        case emp::snd::biquad_e::BandPass:
        { EMP_LOG_INFO("        bandpass"); break; }
        case emp::snd::biquad_e::BandStop:
        { EMP_LOG_INFO("        bandstop"); break; }
        case emp::snd::biquad_e::AllPass:
        { EMP_LOG_INFO("        allpass"); break; }
        default:
        { EMP_LOG_INFO("        unknown"); break; }
    }

    EMP_LOG_INFO("    Coeficients");
    EMP_LOG_INFO("        b0              = #0", m_tb0);
    EMP_LOG_INFO("        b1              = #0", m_tb1);
    EMP_LOG_INFO("        b2              = #0", m_tb2);
    EMP_LOG_INFO("        a0              = #0", m_ta0);
    EMP_LOG_INFO("        a1              = #0", m_ta1);
    EMP_LOG_INFO("        a2              = #0", m_ta2);
    EMP_LOG_INFO("    Parameters");
    EMP_LOG_INFO("        Q               = #0", m_tQ);
    EMP_LOG_INFO("        Center Freq. Hz = #0", m_tF0);
    EMP_LOG_INFO("        SampleRate      = #0", m_tSampleRate);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::lowpass(void)
{
    T const cosw0 = m_tCosW0;
    T const alpha = m_tAlpha;

    m_tb0 =  (1 - cosw0) / 2;
    m_tb1 =   1 - cosw0;
    m_tb2 =  (1 - cosw0) / 2;
    m_ta0 =   1 + alpha;
    m_ta1 =  -2 * cosw0;
    m_ta2 =   1 - alpha;

    /*T const K = emp::mat::tan(emp::mat::pi<T>() * m_tF0);
    T const Q = m_tQ;
    T const norm = 1 / (1 + K / Q + K * K);
    m_tb0 = K * K * norm;
    m_tb1 = 2 * m_tb0;
    m_tb2 = m_tb0;
    m_ta1 = 2 * (K * K - 1) * norm;
    m_ta2 = (1 - K / Q + K * K) * norm;*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::highpass(void)
{
    T const cosw0 = m_tCosW0;
    T const alpha = m_tAlpha;

    m_tb0 =  (1 + cosw0) / 2;
    m_tb1 = -(1 + cosw0);
    m_tb2 =  (1 + cosw0) / 2;
    m_ta0 =   1 + alpha;
    m_ta1 =  -2 * cosw0;
    m_ta2 =   1 - alpha;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::bandpass(void)
{
    T const cosw0 = m_tCosW0;
    T const sinw0 = m_tSinW0;
    T const alpha = m_tAlpha;

    m_tb0 =   sinw0 / 2;
    m_tb1 =   0;
    m_tb2 =  -sinw0 / 2;
    m_ta0 =   1 + alpha;
    m_ta1 =  -2 * cosw0;
    m_ta2 =   1 - alpha;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::bandstop(void)
{
    T const cosw0 = m_tCosW0;
    T const alpha = m_tAlpha;

    m_tb0 =   1;
    m_tb1 =  -2 * cosw0;
    m_tb2 =   1;
    m_ta0 =   1 + alpha;
    m_ta1 =  -2 * cosw0;
    m_ta2 =   1 - alpha;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
void biquad_t<T>::allpass(void)
{
    T const cosw0 = m_tCosW0;
    T const alpha = m_tAlpha;

    m_tb0 =   1 - alpha;
    m_tb1 =  -2*cosw0;
    m_tb2 =   1 + alpha;
    m_ta0 =   1 + alpha;
    m_ta1 =  -2*cosw0;
    m_ta2 =   1 - alpha;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
envelope_t<T>::envelope_t(void):
    m_Function(),
    m_pNurbs(),
    m_pEnvelope(m_pNurbs),
    m_pv4Volumes(),
    m_stResult(),
    m_stSize()
{ m_pv4Volumes = EMP_ALLOCATE_ALIGNED_POD_ARRAY(vector_d, precision_c::array_stride, 16); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
envelope_t<T>::~envelope_t(void)
{ EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Volumes); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool envelope_t<T>::evaluate(emp::mat::eval_stack_t<T>& a_rStack)
{
    a_rStack.push(operation_e::Assign, arity());

    vector_d* const pOutput = a_rStack.node_output();

#ifdef EMP_EVL_AOS
    c_size ui32Arity = a_rStack.node_arity();
#endif

#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
    pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = m_pv4Volumes[x_Index];
    EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE

    c_size res = operation_d<enum_from_(operation_e, enum_to__(sound_operation_e::Envelope))>::result;
    a_rStack.flush_node_output(res);
    a_rStack.pop();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
spectrum_t<T>::spectrum_t(void):
    m_Function(),
    m_pData(),
    m_stResult(),
    m_stSize()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
spectrum_t<T>::~spectrum_t(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool spectrum_t<T>::evaluate(eval_stack_t<T>& a_rStack)
{
    a_rStack.push(operation_e::Assign, arity());

    vector_d* const pOutput = a_rStack.node_output();
    vector_d const* const pInput = a_rStack.input();

    c_bool bCompute = a_rStack.compute();
    emp::snd::spectrum_t<T>* const pData = m_pData;

#ifdef EMP_EVL_AOS
    c_size ui32Arity = a_rStack.node_arity();
#endif

    if (m_Function.evaluate(a_rStack) == false)
    { return false; }

    if (pData)
    {
        if (bCompute)
        {
            for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
            {
                vector_d const vFrequency = pInput[EMP_EVL_STRIDE(stStride, 0, spectrum_d::arity)];
                vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 1, spectrum_d::arity)];

                pData->add_sample(vSignal);
                vector_d const vResult = pData->amplitude(vFrequency);

               pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
               pOutput[EMP_EVL_STRIDE(stStride,1,ui32Arity)] = vSignal;
            }
        }
        else
        {
            for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
            {
                vector_d const vFrequency = pInput[EMP_EVL_STRIDE(stStride, 0, spectrum_d::arity)];
                vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 1, spectrum_d::arity)];
                vector_d const vResult = pData->amplitude(vFrequency);

               pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
               pOutput[EMP_EVL_STRIDE(stStride,1,ui32Arity)] = vSignal;
            }
        }
    }
    else
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        {
            vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 1, spectrum_d::arity)];
            vector_d const vResult = tool_d::zero();

           pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vResult;
           pOutput[EMP_EVL_STRIDE(stStride,1,ui32Arity)] = vSignal;
        }
    }

    a_rStack.flush_node_output(spectrum_d::result);
    a_rStack.pop();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
biquad_t<T>::biquad_t(void):
    m_Function(),
    m_Biquad(),
    m_stResult(),
    m_stSize()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
biquad_t<T>::~biquad_t(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool biquad_t<T>::evaluate(eval_stack_t<T>& a_rStack)
{
    a_rStack.push(operation_e::Assign, arity());

    vector_d* const pOutput = a_rStack.node_output();
    vector_d const* const pInput = a_rStack.input();

    c_bool bCompute = a_rStack.compute();

    emp::snd::biquad_t<T>* const pBiquad = &m_Biquad;

#ifdef EMP_EVL_AOS
    c_size ui32Arity = a_rStack.node_arity();
#endif

    if (m_Function.evaluate(a_rStack) == false)
    { return false; }

    vector_d vOutputZ1 = tool_d::zero();
    vector_d vOutputZ2 = tool_d::zero();

    if (a_rStack.first_passed())
    {
        c_size stOutputOffset2 = EMP_EVL_STRIDE(precision_c::array_stride - 1, 2, ui32Arity);
        c_size stOutputOffset3 = EMP_EVL_STRIDE(precision_c::array_stride - 1, 3, ui32Arity);

        vOutputZ1 = a_rStack.get_output(1, stOutputOffset2);
        vOutputZ2 = a_rStack.get_output(1, stOutputOffset3);

    }

    precision_d dOutputZ1 = tool_d::get_w(vOutputZ1);
    precision_d dOutputZ2 = tool_d::get_w(vOutputZ2);

    precision_d const dRate = 44100;

    if (bCompute)
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        {
            vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 1, biquad_d::arity)];
            vector_d const vF0 = pInput[EMP_EVL_STRIDE(stStride, 2, biquad_d::arity)];
            vector_d const vQ = pInput[EMP_EVL_STRIDE(stStride, 3, biquad_d::arity)];
            vector_d const vType = pInput[EMP_EVL_STRIDE(stStride, 4, biquad_d::arity)];

            vector_d vOutput = vSignal;
            vector_d vAmplitude = tool_d::zero();

            //x
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_x(vType)));
                precision_d const dF0 = tool_d::get_x(vF0);
                precision_d const dQ = tool_d::get_x(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                precision_d const dSignal = tool_d::get_x(vSignal);
                pBiquad->compute_signal(dSignal, dOutputZ1, dOutputZ2);
                precision_d const dOutput = pBiquad->output_signal();
                dOutputZ1 = pBiquad->output_z1();
                dOutputZ2 = pBiquad->output_z2();

                vOutput = tool_d::set_x(vOutput, dOutput);
                vOutputZ1 = tool_d::set_x(vOutputZ1, dOutputZ1);
                vOutputZ2 = tool_d::set_x(vOutputZ2, dOutputZ2);
            }

            //y
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_y(vType)));
                precision_d const dF0 = tool_d::get_y(vF0);
                precision_d const dQ = tool_d::get_y(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                precision_d const dSignal = tool_d::get_y(vSignal);
                pBiquad->compute_signal(dSignal, dOutputZ1, dOutputZ2);
                precision_d const dOutput = pBiquad->output_signal();
                dOutputZ1 = pBiquad->output_z1();
                dOutputZ2 = pBiquad->output_z2();

                vOutput = tool_d::set_y(vOutput, dOutput);
                vOutputZ1 = tool_d::set_y(vOutputZ1, dOutputZ1);
                vOutputZ2 = tool_d::set_y(vOutputZ2, dOutputZ2);
            }

            //z
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_z(vType)));
                precision_d const dF0 = tool_d::get_z(vF0);
                precision_d const dQ = tool_d::get_z(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                precision_d const dSignal = tool_d::get_z(vSignal);
                pBiquad->compute_signal(dSignal, dOutputZ1, dOutputZ2);
                precision_d const dOutput = pBiquad->output_signal();
                dOutputZ1 = pBiquad->output_z1();
                dOutputZ2 = pBiquad->output_z2();

                vOutput = tool_d::set_z(vOutput, dOutput);
                vOutputZ1 = tool_d::set_z(vOutputZ1, dOutputZ1);
                vOutputZ2 = tool_d::set_z(vOutputZ2, dOutputZ2);
            }

            //w
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_w(vType)));
                precision_d const dF0 = tool_d::get_w(vF0);
                precision_d const dQ = tool_d::get_w(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                precision_d const dSignal = tool_d::get_w(vSignal);
                pBiquad->compute_signal(dSignal, dOutputZ1, dOutputZ2);
                precision_d const dOutput = pBiquad->output_signal();
                dOutputZ1 = pBiquad->output_z1();
                dOutputZ2 = pBiquad->output_z2();

                vOutput = tool_d::set_w(vOutput, dOutput);
                vOutputZ1 = tool_d::set_w(vOutputZ1, dOutputZ1);
                vOutputZ2 = tool_d::set_w(vOutputZ2, dOutputZ2);
            }

            pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vAmplitude;
            pOutput[EMP_EVL_STRIDE(stStride,1,ui32Arity)] = vOutput;
            pOutput[EMP_EVL_STRIDE(stStride,2,ui32Arity)] = vOutputZ1;
            pOutput[EMP_EVL_STRIDE(stStride,3,ui32Arity)] = vOutputZ2;
        }
    }
    else
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        {
            vector_d const vTime = pInput[EMP_EVL_STRIDE(stStride, 0, biquad_d::arity)];
            vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 1, biquad_d::arity)];
            vector_d const vF0 = pInput[EMP_EVL_STRIDE(stStride, 2, biquad_d::arity)];
            vector_d const vQ = pInput[EMP_EVL_STRIDE(stStride, 3, biquad_d::arity)];
            vector_d const vType = pInput[EMP_EVL_STRIDE(stStride, 4, biquad_d::arity)];

            vector_d vOutput = vSignal;
            vector_d vAmplitude = tool_d::zero();

            //x
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_x(vType)));
                precision_d const dF0 = tool_d::get_x(vF0);
                precision_d const dQ = tool_d::get_x(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                T const dTime = tool_d::get_x(vTime);
                pBiquad->compute_transfer(dTime);
                vAmplitude = tool_d::set_x(vAmplitude, pBiquad->output_amplitude());
            }

            //y
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_y(vType)));
                precision_d const dF0 = tool_d::get_y(vF0);
                precision_d const dQ = tool_d::get_y(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                T const dTime = tool_d::get_y(vTime);
                pBiquad->compute_transfer(dTime);
                vAmplitude = tool_d::set_y(vAmplitude, pBiquad->output_amplitude());
            }

            //z
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_z(vType)));
                precision_d const dF0 = tool_d::get_z(vF0);
                precision_d const dQ = tool_d::get_z(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                T const dTime = tool_d::get_z(vTime);
                pBiquad->compute_transfer(dTime);
                vAmplitude = tool_d::set_z(vAmplitude, pBiquad->output_amplitude());
            }

            //w
            {
                emp::snd::c_biquad_e eType = static_cast<emp::snd::biquad_e>(emp::ast::cast<size_t>(tool_d::get_w(vType)));
                precision_d const dF0 = tool_d::get_w(vF0);
                precision_d const dQ = tool_d::get_w(vQ);
                pBiquad->update(eType, dF0, dQ, dRate);

                T const dTime = tool_d::get_w(vTime);
                pBiquad->compute_transfer(dTime);
                vAmplitude = tool_d::set_w(vAmplitude, pBiquad->output_amplitude());
            }

            pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vAmplitude;
            pOutput[EMP_EVL_STRIDE(stStride,1,ui32Arity)] = vOutput;
            pOutput[EMP_EVL_STRIDE(stStride,2,ui32Arity)] = vOutputZ1;
            pOutput[EMP_EVL_STRIDE(stStride,3,ui32Arity)] = vOutputZ2;
        }
    }

    a_rStack.flush_node_output(biquad_d::result);
    a_rStack.pop();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
delay_t<T>::delay_t(void):
    m_Function(),
    m_pData(),
    m_stResult(),
    m_stSize()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
delay_t<T>::~delay_t(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool delay_t<T>::evaluate(emp::mat::eval_stack_t<T>& a_rStack)
{
    a_rStack.push(operation_e::Assign, arity());

    vector_d* const pOutput = a_rStack.node_output();
    vector_d const* const pInput = a_rStack.input();

    c_bool bCompute = a_rStack.compute();
    emp::snd::delay_t<T>* const pData = m_pData;

#ifdef EMP_EVL_AOS
    c_size ui32Arity = a_rStack.node_arity();
#endif

    if (m_Function.evaluate(a_rStack) == false)
    { return false; }

    if (bCompute && pData)
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        {
            vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 0, delay_d::arity)];
            vector_d const vFeedBack = pInput[EMP_EVL_STRIDE(stStride, 1, delay_d::arity)];
            vector_d const vDelay = pInput[EMP_EVL_STRIDE(stStride, 2, delay_d::arity)];
            vector_d const vDecay = pInput[EMP_EVL_STRIDE(stStride, 3, delay_d::arity)];
            vector_d vOutput = vSignal;

            // @@0 simd
            T const t0 = pData->update(tool_d::get_x(vSignal), tool_d::get_x(vDelay), tool_d::get_x(vDecay), tool_d::get_x(vFeedBack) > 0);
            T const t1 = pData->update(tool_d::get_y(vSignal), tool_d::get_y(vDelay), tool_d::get_y(vDecay), tool_d::get_y(vFeedBack) > 0);
            T const t2 = pData->update(tool_d::get_z(vSignal), tool_d::get_z(vDelay), tool_d::get_z(vDecay), tool_d::get_z(vFeedBack) > 0);
            T const t3 = pData->update(tool_d::get_w(vSignal), tool_d::get_w(vDelay), tool_d::get_w(vDecay), tool_d::get_w(vFeedBack) > 0);

            vOutput = tool_d::set(t0, t1, t2, t3);
            pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = vOutput;
        }
    }
    else
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        { pOutput[EMP_EVL_STRIDE(stStride,0,ui32Arity)] = tool_d::zero(); }
    }

    a_rStack.flush_node_output(delay_d::result);
    a_rStack.pop();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
reverb_t<T>::reverb_t(void):
    m_Function(),
    m_pData(),
    m_stResult(),
    m_stSize()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
reverb_t<T>::~reverb_t(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool reverb_t<T>::evaluate(emp::mat::eval_stack_t<T>& a_rStack)
{
    a_rStack.push(operation_e::Assign, arity());

    vector_d* const pOutput = a_rStack.node_output();
    vector_d const* const pInput = a_rStack.input();

#ifdef EMP_EVL_AOS
    c_size ui32Arity = a_rStack.node_arity();
#endif

    if (m_Function.evaluate(a_rStack) == false)
    { return false; }

    for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
    {
        vector_d const vSignal = pInput[EMP_EVL_STRIDE(stStride, 0, reverb_d::arity)];
        pOutput[EMP_EVL_STRIDE(stStride, 0, ui32Arity)] = vSignal;
    }
    a_rStack.flush_node_output(reverb_d::result);
    a_rStack.pop();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace snd {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
soft_synth_c::soft_synth_c(void):
    m_sName(),
    m_vpSounds(),
    m_pQuality(),
    m_vFunctions(),
    m_vInfo(),
    m_Data(),
    m_vpFunctions(),
    m_pCompiler(),
    m_pStack(),
    m_vpRelease(),
    m_stCellCount()
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
soft_synth_c::~soft_synth_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::destroy(void)
{
    m_sName = astring();

    emp::cnt::foreach(m_vpSounds, emp::mem::safe_delete_reset_t());
    m_vpSounds.resize(0);

    m_pQuality = nullptr;

    m_vFunctions.resize(0);

    m_vInfo.resize(0);

    emp::cnt::foreach(m_vpFunctions, emp::mem::safe_delete_reset_t());
    m_vpFunctions.resize(0);

    emp::cnt::foreach(m_vpRelease, emp::mem::safe_delete_reset_t());
    m_vpRelease.resize(0);

    m_stCellCount = 0;

    m_Data.destroy();
    m_pStack->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_INTERN EMP_RETURN emp::mat::function_i<T>* build_function(c_astring& a_sExpression,
    emp::mat::compiler_t<T>* const a_pCompiler, emp::snd::function_c const& a_rFunction,
    c_bool EMP_SILENT(a_bPostLoad), soft_synth_data_c& a_rData);
template <typename T>
EMP_INTERN EMP_RETURN emp::mat::function_i<T>* build_function(c_astring& a_sExpression,
    emp::mat::compiler_t<T>* const a_pCompiler, emp::snd::function_c const& a_rFunction,
    c_bool EMP_SILENT(a_bPostLoad), soft_synth_data_c& a_rData)
{
    emp::cnt::vector<emp::mat::function_i<T>*> vpFunctions;

    if (a_pCompiler->build(vpFunctions, a_sExpression) == false)
    { return nullptr; }

    c_size stCount = vpFunctions.count();
    if ((stCount > 0) == false)
    { return nullptr; }

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::function_i<T>* const pFunction = vpFunctions.val(stIndex);
        emp::mat::c_operation_e eOperation = pFunction->operation();

        if (eOperation == static_cast<emp::mat::operation_e>(emp::mat::envelope_t<T>::operation_value))
        {
            emp::mat::envelope_t<T>* const pEnvelope = static_cast<emp::mat::envelope_t<T>*>(pFunction);

            c_size stParamCount = a_rFunction.params().count();
            for (size_t stParam = 0; stParam < stParamCount; ++stParam)
            { pEnvelope->nurbs()->insert_point(stParam, v4f::all(v4f::xy(a_rFunction.params().ref(stParam)), 0, 1)); }
            pEnvelope->envelope()->build();
            a_rData.envelopes().add(pEnvelope);
        }
        else if (
            eOperation == static_cast<emp::mat::operation_e>(
            emp::mat::spectrum_t<T>::operation_value))
        { a_rData.spectrums().add(static_cast<emp::mat::spectrum_t<T>*>(pFunction)); }
        else if (
            eOperation == static_cast<emp::mat::operation_e>(
            emp::mat::delay_t<T>::operation_value))
        { a_rData.delays().add(static_cast<emp::mat::delay_t<T>*>(pFunction)); }
        else if (
            eOperation == static_cast<emp::mat::operation_e>(
            emp::mat::reverb_t<T>::operation_value))
        { a_rData.reverbs().add(static_cast<emp::mat::reverb_t<T>*>(pFunction)); }
    }

    return vpFunctions.last_val();
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool soft_synth_c::post_load(void)
{
    using namespace emp::evl;
    if (m_pCompiler->create(enum_to__(sound_operation_e::Max)) == false)
    { return false; }




#define XTMP_REG(x_Operation, x_Name)\
{\
    typedef emp::mat::x_Operation<precision_d> sound_d;\
    constexpr c_sound_operation_e eSound = sound_d::operation_value;\
    constexpr c_size stSound = enum_to__(eSound);\
    constexpr c_operation_e eOperation = enum_from_(operation_e, stSound);\
    typedef operation_d<eOperation> op_d;\
    if (m_pCompiler->regop(eOperation, operation_t<precision_d>(\
        ALITERAL(x_Name), eOperation, op_d::precedence, op_d::result, op_d::arity, tool_d::zero(), &sound_d::allocate)) == false)\
    { return false; }\
}

    XTMP_REG( delay_t    , "delay"    )
    XTMP_REG( reverb_t   , "reverb"   )
    XTMP_REG( envelope_t , "envelope" )
    XTMP_REG( spectrum_t , "spectrum" )
    XTMP_REG( biquad_t   , "biquad"   )

#undef XTMP_REG

    c_size stCount = m_vFunctions.count();
    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        function_c const& rFunction = m_vFunctions.ref(ui32Index);

        auto* const pFunction = build_function<precision_d>(rFunction.expression(), m_pCompiler, rFunction, true, m_Data);
        if (pFunction == nullptr)
        { return false; }
        m_vpFunctions.add(pFunction);
    }

    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        function_c const& rFunction = m_vFunctions.ref(ui32Index);
        c_size stInCount = rFunction.inputs().count();
        for (ui32 ui32In = 0; ui32In < stInCount; ++ui32In)
        {
            link_c const& rLink = rFunction.inputs().ref(ui32In);
            if (rLink.function() != emp::tt::max<ui32>::value &&
                rLink.index() != emp::tt::max<ui32>::value)
            {
                link(ui32Index, ui32In, rLink.function(), rLink.index(),false);
                set_value(ui32Index, ui32In, rLink.value());
            }
        }
    }

    build();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool soft_synth_c::load(c_size EMP_SILENT(a_stIndex))
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN bool soft_synth_c::scale(void) const
{
    c_astring sIndex("frequency");

    c_size stCount = m_vpFunctions.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::function_i<precision_d> const* const pFunction = m_vpFunctions.val(stIndex);
        c_size stInputCount = pFunction->input_count();
        for (size_t stInput = 0; stInput < stInputCount; ++stInput)
        {
            emp::mat::input_t<precision_d> const* const pInput = pFunction->get_input(stInput);
            if (emp::str::cmp(pInput->name(), sIndex))
            { return true; }
        }
    }
    return false;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN sound_i* soft_synth_c::sound(float const a_fFrequency)
{
    if (get_output() == nullptr)
    { return nullptr; }

    soft_sound_c* pSound = nullptr;
    c_size stCount = m_vpSounds.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        soft_sound_c* const pCurrent = m_vpSounds.val(stIndex);
        if (pCurrent->released())
        {
            pSound = pCurrent;
            break;
        }
    }

    if (pSound == nullptr)
    {
        pSound = EMP_ALLOCATE(soft_sound_c);
        m_vpSounds.add(pSound);
    }

    pSound->set_frequency(a_fFrequency);
    pSound->set_synth(this, m_pStack);
    return pSound;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::set_function_expression(c_ui32 a_ui32Function, c_astring& a_sValue)
{
    c_astring& sExpression = m_vFunctions.ref(a_ui32Function).expression();
    if (emp::str::cmp(sExpression, a_sValue))
    { return; }

    emp::mat::function_i<precision_d>* pFunction = m_vpFunctions.val(a_ui32Function);

    release_function(pFunction);

    function_c& rFunction = m_vFunctions.ref(a_ui32Function);

    pFunction = build_function<precision_d>(a_sValue, m_pCompiler, rFunction, false, m_Data);

    m_vpFunctions.set(a_ui32Function, pFunction);

    rFunction.set_expression(a_sValue);

    c_size stInCount = rFunction.inputs().count();
    c_size stNewInCount = pFunction ? pFunction->input_count() : 0;

    c_size stRelinkCount = emp::tt::minof(stInCount, stNewInCount);
    for (ui32 ui32Index = 0; ui32Index < stRelinkCount; ++ui32Index)
    { relink(a_ui32Function, ui32Index, false); }

    for (size_t stIndex = stRelinkCount; stIndex < stInCount; ++stIndex)
    { unlink(a_ui32Function, stIndex, false); }

    rFunction.inputs().resize(stNewInCount);

    for (size_t stIndex = stInCount; stIndex < stNewInCount; ++stIndex)
    { rFunction.inputs().ref(stIndex).destroy(); }

    c_size stNewOutCount = pFunction ? pFunction->result() : 0;

    { c_size stCount = m_vFunctions.count();
    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        function_c& rCurrentFunction = m_vFunctions.ref(ui32Index);
        c_size stCurrentInCount = rCurrentFunction.inputs().count();
        for (ui32 ui32In = 0; ui32In < stCurrentInCount; ++ui32In)
        {
            link_c const& rLink = rCurrentFunction.inputs().ref(ui32In);
            if (rLink.function() == a_ui32Function)
            {
                if (rLink.index() < stNewOutCount)
                { relink(ui32Index, ui32In, false); }
                else
                { unlink(ui32Index, ui32In, false); }
            }
        }
    } }
    build();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::mat::function_i<precision_d>* soft_synth_c::get_output(void)
{
    c_astring sOutput(ALITERAL("left,right"));
    c_size stCount = m_vFunctions.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (emp::str::cmp(sOutput, m_vFunctions.ref(stIndex).expression()))
        { return m_vpFunctions.val(stIndex); }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN ui32 soft_synth_c::add_function(void)
{
    c_ui32 ui32Function = static_cast<ui32>(m_vFunctions.count());
    m_vFunctions.add(function_c());
    m_vpFunctions.add(nullptr);
    m_vInfo.add(emp::gui::info_s());
    return ui32Function;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::release_function(emp::mat::function_i<precision_d>* a_pValue)
{
    m_Data.remove(a_pValue);
    if (a_pValue)
    { m_vpRelease.add(a_pValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::remove_function(c_ui32 a_ui32Index)
{
    release_function(m_vpFunctions.val(a_ui32Index));

    m_vpFunctions.remove(a_ui32Index);
    m_vFunctions.remove(a_ui32Index);
    m_vInfo.remove(a_ui32Index);

    c_size stCount = m_vFunctions.count();
    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        function_c& rFunction = m_vFunctions.ref(ui32Index);
        emp::mat::function_i<precision_d>* const pFunction = m_vpFunctions.val(ui32Index);
        c_size stInCount = rFunction.inputs().count();
        for (ui32 ui32In = 0; ui32In < stInCount; ++ui32In)
        {
            link_c& rLink = rFunction.inputs().ref(ui32In);
            ui32 ui32Function = rLink.function();
            if (ui32Function == a_ui32Index)
            {
                rLink.destroy();
                emp::mat::input_t<precision_d>* const pInput = pFunction->get_input(ui32In);
                pInput->reset();
            }
            else if (
                ui32Function > a_ui32Index &&
                ui32Function != emp::tt::max<ui32>::value)
            {
                rLink.set_function(ui32Function - 1);
                relink(ui32Index, ui32In, false);
            }
        }
    }
    build();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::link(c_ui32 a_ui32InFunction, c_ui32 a_ui32In, c_ui32 a_ui32OutFunction, c_ui32 a_ui32Out, c_bool a_bBuild)
{
    function_c& rFunction = m_vFunctions.ref(a_ui32InFunction);
    link_c& rLink = rFunction.inputs().ref(a_ui32In);
    rLink.set_function(a_ui32OutFunction);
    rLink.set_index(a_ui32Out);

    emp::mat::function_i<precision_d>* const pRoot = m_vpFunctions.val(a_ui32InFunction);
    emp::mat::input_t<precision_d>* const pInput = pRoot->get_input(a_ui32In);
    if (a_ui32InFunction != a_ui32OutFunction)
    { pInput->set_function(*m_pStack, a_ui32Out, m_vpFunctions.val(a_ui32OutFunction)); }
    else
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        { pInput->set_value(*m_pStack, stStride, tool_d::zero()); }
    }

    if (a_bBuild)
    { build(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::unlink(c_size a_stInFunction, c_size a_stIn, c_bool a_bBuild)
{
    function_c& rFunction = m_vFunctions.ref(a_stInFunction);
    link_c& rLink = rFunction.inputs().ref(a_stIn);
    rLink.destroy();

    emp::mat::function_i<precision_d>* const pFunction = m_vpFunctions.val(a_stInFunction);
    if (pFunction && a_stIn < pFunction->input_count())
    { pFunction->get_input(a_stIn)->reset(); }
    if (a_bBuild)
    { build(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::relink(c_ui32 a_ui32InFunction, c_ui32 a_ui32In, c_bool a_bBuild)
{
    function_c const& rFunction = m_vFunctions.ref(a_ui32InFunction);
    link_c const& rLink = rFunction.inputs().ref(a_ui32In);
    c_ui32 ui32Function = rLink.function();
    if (ui32Function != emp::tt::max<ui32>::value)
    { link(a_ui32InFunction, a_ui32In, ui32Function, rLink.index(), a_bBuild);}
    else
    { unlink(a_ui32InFunction, a_ui32In, a_bBuild); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float soft_synth_c::get_value(c_size a_stInFunction, c_size a_stIn) const
{
    function_c const& rFunction = m_vFunctions.ref(a_stInFunction);
    link_c const& rLink = rFunction.inputs().ref(a_stIn);
    if (rLink.function() == a_stInFunction &&
        rLink.index() == a_stIn)
    {
        emp::mat::function_i<precision_d>* const pRoot = m_vpFunctions.val(a_stInFunction);
        emp::mat::input_t<precision_d>* const pInput = pRoot->get_input(a_stIn);
        return tool_d::get_x(pInput->cash()->result(0));
    }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::set_value(c_size a_stInFunction, c_size a_stIn, c_float a_fValue)
{
    function_c& rFunction = m_vFunctions.ref(a_stInFunction);
    link_c& rLink = rFunction.inputs().ref(a_stIn);
    if (rLink.function() == a_stInFunction &&
        rLink.index() == a_stIn)
    {
        emp::mat::function_i<precision_d>* const pRoot = m_vpFunctions.val(a_stInFunction);
        emp::mat::input_t<precision_d>* const pInput = pRoot->get_input(a_stIn);
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        { pInput->set_value(*m_pStack, stStride, tool_d::set(a_fValue)); }
        build();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_RETURN bool get_unbound_variables(emp::cnt::vector<emp::mat::input_t<T>*>* a_rvpInputs,
    emp::mat::function_i<T>* const a_pFunction, c_astring& a_sName)
{
    if (a_pFunction == nullptr)
    { return false; }

    bool bFound = false;

    c_size stCount = a_pFunction->input_count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::input_t<T>* const pInput = a_pFunction->get_input(stIndex);
        if (emp::tt::is_max(pInput->bind()) &&
            (a_sName.len() == 0 || emp::str::cmp(a_sName, pInput->name())))
        {
            if (a_rvpInputs)
            { a_rvpInputs->add(pInput); }
            bFound = true;
        }
    }

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::input_t<T>* const pInput = a_pFunction->get_input(stIndex);
        if (pInput->cash() && pInput->cash()->function())
        {
            if (get_unbound_variables(a_rvpInputs, pInput->cash()->function(), a_sName))
            { bFound = true; }
        }
    }

    return bFound;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_synth_c::build(void)
{
    m_stCellCount = get_unbound_variables<precision_d>(nullptr, get_output(), ALITERAL("frequency")) ? emp::tt::max<size_t>::value : 1;
    /*c_size stCount = m_vpFunctions.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::function_i* const pFunction = m_vpFunctions.val(stIndex);
        if (pFunction)
        {
            c_size stFunctionSize = pFunction->size();
            EMP_LOG_INFO("FUNCTION SIZE #0", stFunctionSize);
        }
    }*/
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
soft_sound_c::soft_sound_c(void):
    m_pfSamples(),
    m_vpVarX(),
    m_vpVarIndex(),
    m_pOutput(),
    m_pStack(),
    m_pCash(),
    m_stChannelCount(),
    m_stRate(),
    m_fFrequency(),
    m_bReleased(true),
    m_Data()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
soft_sound_c::~soft_sound_c(void)
{
    if (m_pfSamples)
    { EMP_DEALLOCATE_ARRAY(m_pfSamples); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_sound_c::set_synth(soft_synth_c* const a_pValue, emp::mat::eval_stack_t<precision_d>* const a_pCash)
{
    m_pCash = a_pCash;
    m_pOutput = a_pValue->get_output();
    c_bool bChanged = a_pValue->quality()->channels() > m_stChannelCount;
    m_stChannelCount = a_pValue->quality()->channels();
    m_stRate = a_pValue->quality()->rate();

    m_Data.copy(a_pValue->data());
    m_vpVarIndex.resize(0);
    m_vpVarX.resize(0);
    emp::tt::silent(get_unbound_variables(&m_vpVarX, m_pOutput, astring()));

    c_astring sXVariable(ALITERAL("time"));
    c_astring sIndexVariable(ALITERAL("frequency"));

    bool bUnbound = false;
    c_size stCount = m_vpVarX.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        emp::mat::input_t<precision_d> const* pInput=m_vpVarX.val(stIndex);
        if (emp::str::cmp(sXVariable, pInput->name()) == false &&
            emp::str::cmp(sIndexVariable, pInput->name()) == false)
        {
            EMP_LOG_WARNING("Unbound variable #0", pInput->name());
            bUnbound = true;
        }
    }

    m_vpVarX.resize(0);
    m_vpVarIndex.resize(0);

    if (m_pOutput)
    {
        m_pOutput->link();
        m_pStack->create(64, m_pOutput->size() + m_pOutput->result() * emp::mat::precision_c::array_stride);
    }

    if (bUnbound == false)
    {
        emp::tt::silent(get_unbound_variables(&m_vpVarX, m_pOutput, sXVariable));
        emp::tt::silent(get_unbound_variables(&m_vpVarIndex, m_pOutput, sIndexVariable));
    }

    if (m_stChannelCount > 0 && (bChanged || m_pfSamples == nullptr))
    {
        if (m_pfSamples)
        { EMP_DEALLOCATE_ARRAY(m_pfSamples); }
        m_pfSamples = EMP_ALLOCATE_ARRAY(float, m_stChannelCount * emp::mat::precision_c::data_stride);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void soft_sound_c::prepare_samples(c_size a_stIndex)
{
    c_size stXVarCount = m_vpVarX.count();
    c_size stIndexVarCount = m_vpVarIndex.count();

   for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
   {
        c_size stOffset = a_stIndex + stStride * emp::mat::precision_c::vector_stride;

        vector_d const vSample = tool_d::set(
            static_cast<precision_d>(stOffset + 0),
            static_cast<precision_d>(stOffset + 1),
            static_cast<precision_d>(stOffset + 2),
            static_cast<precision_d>(stOffset + 3));

        vector_d const vRate = tool_d::set(static_cast<precision_d>(m_stRate));
        vector_d const vTime = tool_d::div(vSample, vRate);

        for (size_t stIndex = 0; stIndex < stXVarCount; ++stIndex)
        { m_vpVarX.val(stIndex)->set_value(*m_pCash, stStride, vTime); }

        vector_d const vIndex = tool_d::set(m_fFrequency);

        for (size_t stIndex = 0; stIndex < stIndexVarCount; ++stIndex)
        { m_vpVarIndex.val(stIndex)->set_value(*m_pCash, stStride, vIndex); }
    }

    size_t stSampleCount = 0;
    size_t stChannel = 0;

    m_pCash->clean();
    m_pStack->begin_frame(m_pOutput->arity());
    c_bool bResult = m_pOutput->evaluate(*m_pStack);

    c_size stResultCount = m_pOutput->result();

    if (bResult)
    {
        for (; stChannel < stResultCount; ++stChannel)
        {
            for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
            {
                vector_d const vOutput = m_pStack->get_input(EMP_EVL_STRIDE(stStride, stChannel, stResultCount));
                m_pfSamples[stSampleCount] = tool_d::get_x(vOutput);
                ++stSampleCount;
                m_pfSamples[stSampleCount] = tool_d::get_y(vOutput);
                ++stSampleCount;
                m_pfSamples[stSampleCount] = tool_d::get_z(vOutput);
                ++stSampleCount;
                m_pfSamples[stSampleCount] = tool_d::get_w(vOutput);
                ++stSampleCount;
            }
        }
        m_pStack->end_frame();
    }

    for (size_t stIndex = 0; stIndex < stXVarCount; ++stIndex)
    { m_vpVarX.val(stIndex)->reset(); }

    for (size_t stIndex = 0; stIndex < stIndexVarCount; ++stIndex)
    { m_vpVarIndex.val(stIndex)->reset(); }

    for (; stChannel < m_stChannelCount; ++stChannel)
    {
        for (size_t stStride = 0; stStride < emp::mat::precision_c::array_stride; ++stStride)
        {
            m_pfSamples[stSampleCount] = 0.0f;
            ++stSampleCount;
            m_pfSamples[stSampleCount] = 0.0f;
            ++stSampleCount;
            m_pfSamples[stSampleCount] = 0.0f;
            ++stSampleCount;
            m_pfSamples[stSampleCount] = 0.0f;
            ++stSampleCount;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

