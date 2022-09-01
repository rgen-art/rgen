//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_coordinate.h"

#include "emp_gui_label.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

#include "emp_hh_math.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
coordinate_c::coordinate_c(void):
    frame(),
    label_x(),
    label_y(),
    m_pLineMeshVertices(),
    m_pLineMeshIndices(),   
    m_vpUnitLabelX(),
    m_vpUnitLabelY(),
    m_vfLabelX(),
    m_vfLabelY()
{
    frame.state().index_(0).focusable_(true);

    label_x.state().focusable_(true);

    label_y.state().focusable_(true);

    add_control(&frame);
    add_control(&label_x);
    add_control(&label_y);

    // @@0 remove
    m_pLineMeshVertices->create(emp::gfx::usage_e::Dynamic);
    m_pLineMeshIndices->create(emp::gfx::usage_e::Dynamic);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
coordinate_c::~coordinate_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void coordinate_c::destroy(void)
{
    emp::cnt::foreach(m_vpUnitLabelX, emp::mem::safe_delete_reset_t());
    m_vpUnitLabelX.resize(0);

    emp::cnt::foreach(m_vpUnitLabelY, emp::mem::safe_delete_reset_t());
    m_vpUnitLabelY.resize(0);

    m_pLineMeshVertices->destroy();
    m_pLineMeshIndices->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN label_c* get_label(emp::cnt::vector<label_c*>& a_rvpLabels, c_size a_stIndex, cpc_label_skin_s a_pSkin);
EMP_INTERN EMP_RETURN label_c* get_label(emp::cnt::vector<label_c*>& a_rvpLabels, c_size a_stIndex, cpc_label_skin_s a_pSkin)
{
    c_size stCount = a_rvpLabels.count();
    if (a_stIndex < stCount)
    { return a_rvpLabels.val(a_stIndex); }

    // @@0  lib_str allocation
    label_c* const pLabel = EMP_ALLOCATE(label_c);

    EMP_ASSERT(pLabel != nullptr);
    // @@0 create pLabel->create();
    pLabel->skin_(a_pSkin).state().autosize.xy(true);
    a_rvpLabels.add(pLabel);
    return pLabel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool coordinate_c::resize_labels(c_bool a_bResize, emp::mat::v2f& a_rv2Out)
{
    if (a_bResize)
    {
        label_x.clear_controls();
        label_y.clear_controls();
    }

    if (show_coordinates == false)
    {
        a_rv2Out = v2f::zero();
        return true;
    }

    cpc_label_skin_s pUnitSkin = skin->unit;

    auto const l_do_resize = [pUnitSkin, a_bResize](vector<spike_c> const& a_rvSpikes, vector<label_c*>& a_rvpLabels,
        vector<float>& a_rvfLabels, container_c& a_rContainer, v2f& a_rv2XYOut) EMP_RETURN -> bool
    {
        float fMaxX = 0;
        float fMaxY = 0;

        c_size stCount = a_rvSpikes.count();
        a_rvfLabels.resize(emp::tt::maxof(a_rvfLabels.count(), stCount));
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            spike_c const& rSpike = a_rvSpikes.ref(stIndex);
            label_c* const pLabel = get_label(a_rvpLabels, stIndex, pUnitSkin);

            if (a_bResize)
            {
                c_float fScale = rSpike.decade ? 1.0f : emp::tt::maxof(0.3f, emp::tt::minof(1.0f, rSpike.size));
                c_float fAlpha = rSpike.decade ? 1.0f : emp::tt::maxof(0.0f, emp::tt::minof(1.0f, rSpike.label));

                if (emp::tt::equal_(a_rvfLabels.val(stIndex), rSpike.value) == false)
                {
                    pLabel->m_su8Text.clear();
                    emp::tt::silent(STR_FORMATU8(pLabel->m_su8Text, "#0", REAL_FMT_RE4(rSpike.value)));
                    pLabel->set_text(pLabel->m_su8Text);
                    a_rvfLabels.set(stIndex, rSpike.value);
                }

                pLabel->scale_(fScale).alpha_(fAlpha);
                a_rContainer.add_control(pLabel);
                EMP_RET_FALSE_IF_NOT(pLabel->resize());
            }

            fMaxX = emp::tt::maxof(fMaxX, pLabel->transformation().width());
            fMaxY = emp::tt::maxof(fMaxY, pLabel->transformation().height());
        }

        a_rv2XYOut = { fMaxX, fMaxY };
        return true;
    };

    v2f v2XSize;
    EMP_RET_FALSE_IF_NOT(l_do_resize(frame.units_x(), m_vpUnitLabelX, m_vfLabelX, label_x, v2XSize));
    c_float fMaxUnitX = v2XSize.x;

    v2f v2YSize;
    EMP_RET_FALSE_IF_NOT(l_do_resize(frame.units_y(), m_vpUnitLabelY, m_vfLabelY, label_y, v2YSize));
    c_float fMaxUnitY = v2YSize.y;

    c_float fMaxX = 2 * FEX() + fMaxUnitX;
    c_float fMaxY = 2 * FEY() + fMaxUnitY;

    a_rv2Out = { fMaxX, fMaxY };
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float coordinate_c::FEX(void) const
{ return 0.007f * EMP_GUI_SKIN.resolution.ratio().x; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float coordinate_c::FEY(void) const
{ return 0.007f * EMP_GUI_SKIN.resolution.ratio().y; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool coordinate_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    label_x.state().visible_(show_coordinates);
    label_y.state().visible_(show_coordinates);

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    c_float fEX = FEX();
    c_float fEY = FEY();

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    emp::mat::v2f v2Max;
    EMP_RET_FALSE_IF_NOT(resize_labels(true, v2Max));

    c_float fMaxX = v2Max.x;
    c_float fMaxY = v2Max.y;

    c_float fChartW = fWidth - fMaxX;
    c_float fChartH = fHeight - fMaxY;

    if (show_coordinates)
    {
        cpc_label_skin_s pUnitSkin = skin->unit;

        auto const l_do_position = [pUnitSkin, fMaxX, /*fChartH,*/ fEX, fEY](vector<spike_c> const& a_rvSpikes,
            vector<label_c*>& a_rvpLabels, c_bool a_bX)
        {
            c_size stCount = a_rvSpikes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                spike_c const& rSpike = a_rvSpikes.ref(stIndex);
                c_float fPos = rSpike.position;

                label_c* const pLabel = get_label(a_rvpLabels, stIndex, pUnitSkin);

                c_v2f v2Position = a_bX ?
                    v2f({
                        fMaxX - pLabel->transformation().width() - fEX,
//                        -(fChartH - fPos - pLabel->transformation().height() - fEY)
                            fPos
                    }) :
                    v2f({ fPos + fEX, -fEY });
                pLabel->transformation().position_(v2Position);
            }
        };

        l_do_position(frame.units_x(), m_vpUnitLabelX, true);
        l_do_position(frame.units_y(), m_vpUnitLabelY, false);
    }

    label_x.transformation().size_(fMaxX, fChartH).position_(0);
    label_y.transformation().size_(fChartW, fMaxY).position_(fMaxX, -fChartH);
    frame.transformation().size_(fChartW, fChartH).position_(fMaxX, 0);

    m_fMaxX = fMaxX;
    m_fMaxY = fMaxY;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool coordinate_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    if (show_coordinates)
    {
        cpc_resolution_data_s pResolution = &EMP_GUI_SKIN.resolution;

        c_float fSX = 0.02f * pResolution->ratio().x;
        c_float fSY = 0.02f * pResolution->ratio().y;

        m_pLineMeshVertices->create(emp::gfx::usage_e::Dynamic);
        m_pLineMeshIndices->create(emp::gfx::usage_e::Dynamic);

        c_v4f v4Color = skin->frame->unit_spike->rgba;

        c_float fDepth = depth(transformation().fore_depth, 0.0f);
        auto const lResizeSpikes = [this, &v4Color, fDepth](emp::cnt::vector<spike_c> const& a_rvSpikes, c_float EMP_SILENT(a_fMaxX), c_float EMP_SILENT(a_fMaxY),
            c_float a_fSize, c_bool a_bX, container_c const& a_rLabel) EMP_RETURN -> bool
        {
            c_size stCount = a_rvSpikes.count();
            EMP_RET_FALSE_IF((m_pLineMeshIndices->count() + stCount * 2) > emp::tt::max<short>::value);
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                spike_c const& rSpike = a_rvSpikes.ref(stIndex);
                if (rSpike.visible == false)
                { continue; }

                c_float fPos = rSpike.position;
                c_float fA = rSpike.alpha;

                c_float fX = a_rLabel.transformation().x();
                c_float fY = a_rLabel.transformation().y();

                c_float fV0X = fX + (a_bX ? -a_fSize : fPos);
                c_float fV0Y = fY + (a_bX ? fPos : 0);

                c_float fV1X = fX + (a_bX ? 0 : fPos);
                c_float fV1Y = fY + (a_bX ? fPos : -a_fSize);

                c_v4f v4C = v4f::mul_w(v4Color, fA);
                emp::gfx::emit_line(m_pLineMeshVertices->count(), *m_pLineMeshIndices);
                m_pLineMeshVertices->add({ fV0X, fV0Y, fDepth },  v4C);
                m_pLineMeshVertices->add({ fV1X, fV1Y, fDepth }, v4C);
            }
            return true;
        };

        EMP_RET_FALSE_IF_NOT(lResizeSpikes(frame.units_x(), m_fMaxX, m_fMaxY, fSX, true, label_x));
        EMP_RET_FALSE_IF_NOT(lResizeSpikes(frame.units_y(), m_fMaxX, transformation().height(), fSY, false, label_y));
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool coordinate_c::render(void)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    if (show_coordinates)
    {
        auto & rDevice = EMP_GRAPHICS;

        rDevice.set_depth_test(true); // @@0 depth
        rDevice.set_depth_mask(true);
        rDevice.set_vertex_buffer(&*m_pLineMeshVertices);
        rDevice.set_index_buffer(&*m_pLineMeshIndices);
        rDevice.set_program(rDevice.color_attribute_program());
        rDevice.set_uniform(emp::gfx::uniform_e::m44Transform, transformation().matrix);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e coordinate_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    if (state().enable == false)
    { return return_e::Unused; }

    if (a_rEvent.keyboard.modified() == false &&
        state().hover)
    {
        emp::app::c_mouse_event_s& rMouse = a_rEvent.mouse;
        if (a_rEvent.mouse_pressed())
        {
            emp::app::c_button_e eButton = rMouse.m_eButton;
            bool bUp = eButton == emp::app::button_e::WheelUp;
            bool bY = label_x.state().hover;
            bool bX = label_y.state().hover;
            if (bUp || (eButton == emp::app::button_e::WheelDown))
            {
                frame.scale(bUp, bX, bY);
                return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
            }
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool coordinate_c::auto_resize(emp::mat::v2f::tparam a_v2Value)
{
    emp::mat::v2f v2Max;
    EMP_RET_FALSE_IF_NOT(resize_labels(false, v2Max));
    c_float fMaxX = v2Max.x;
    c_float fMaxY = v2Max.y;

    c_float fChartW = a_v2Value.x;
    c_float fChartH = a_v2Value.y;

    label_x.transformation().size_(fMaxX, fChartH).position_(0);
    label_y.transformation().size_(fChartW, fMaxY).position_(fMaxX, -fChartH);
    frame.transformation().size_(fChartW, fChartH).position_(fMaxX, 0);

    transformation().size_(fChartW + fMaxX, fChartH + fMaxY);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
coordinate_c& coordinate_c::skin_(cpc_coordinate_skin_s a_pValue)
{
    skin = a_pValue;

    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);
    frame.skin_(a_pValue ? a_pValue->frame : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

