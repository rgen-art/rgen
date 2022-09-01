//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_frame.h"

#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_gl_device.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::cnt;
using namespace emp::gfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float frame_data_s::final_scale_x(void) const
{ return scale * xscale * EMP_GUI_SKIN.resolution.scaled_ratio().x; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN float frame_data_s::final_scale_y(void) const
{ return scale * yscale * EMP_GUI_SKIN.resolution.scaled_ratio().y; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
frame_c::frame_c(void):
    on_render(),
    data(),
    m_v2Origin(),
    m_bDrag(false),
    m_v2DragOrigin(),
    m_v2DragDelta(),
    position_x(),
    position_y(),
    m_pOriginUnitX(),
    m_pOriginUnitY(),
    m_pOriginDecadeX(),
    m_pOriginDecadeY(),
    m_pCursorX(),
    m_pCursorY(),
    m_pAxisX(),
    m_pAxisY(),
    m_pMesh(),
    m_vUnitX(),
    m_vUnitY(),
    m_mModel()
{
    // @@0 create position_x.create(); // @@0  static
// @@0 create     position_y.create(); // @@0  static

    add_control(&position_x);
    add_control(&position_y);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
frame_c::~frame_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool frame_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    position_x.state().visible_(data.show_position);
    position_y.state().visible_(data.show_position);

    c_v4f v4Origin = { 0, 0, 0, 1 };
    c_v4f v4TransformedOrigin = m44f::mul_v4(transformation().matrix, v4Origin);
    c_v2f v2Position = v2f::sub(m_v2Position, v4f::xy(v4TransformedOrigin));

    m_v2ClientPosition = v2Position;

    c_v2f v2FinalPosition = client_to_frame(m_v2ClientPosition);

    position_x.state().autosize.xy(true);
    position_x.m_su8Text.clear();
    position_x.set_text(STR_FORMATU8(position_x.m_su8Text, "#0", v2FinalPosition.x));

    position_y.state().autosize.xy(true);
    position_y.m_su8Text.clear();
    position_y.set_text(STR_FORMATU8(position_y.m_su8Text, "#0", v2FinalPosition.y));

    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    c_v2f v2MinSize = v2f::max(position_x.transformation().size, position_y.transformation().size);

    c_float fPositionWidth = v2MinSize.x;
    c_float fPositionHeight = v2MinSize.y;

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();

    position_x.transformation().position_(fWidth - fPositionWidth, 0).size_(v2MinSize);
    position_x.state().autosize.xy(false);

    position_y.transformation().position_(fWidth - fPositionWidth, -fPositionHeight).size_(v2MinSize);
    position_y.state().autosize.xy(false);

    c_v2f v2Origin = move_point(m_v2Origin, m_v2DragDelta);

    auto const l_resize_cell = [](c_bool a_bLog, c_float a_fScale, c_float a_fRatio, c_float a_fCenter, c_float a_fSize,
        vector<spike_c>& a_rvSpikes, int& a_rsiDomain, float& a_rfAlpha, c_bool EMP_SILENT(a_bX))
    {
        c_float fMinDelta = 0.01f * a_fRatio;

        int siDomain = 0;
        while (a_fScale * emp::mat::pow(10.0f, static_cast<float>(siDomain)) > fMinDelta)
        { --siDomain; }
        while (a_fScale * emp::mat::pow(10.0f, static_cast<float>(siDomain)) < fMinDelta)
        { ++siDomain; }

        if (a_bLog)
        { siDomain = emp::tt::minof(siDomain, -1); }

        a_rsiDomain = siDomain;

        c_float fDomain = emp::mat::pow(10.0f, static_cast<float>(siDomain));

        c_float fMinorAlpha = 0.1f * a_fScale * fDomain / fMinDelta;

        a_rfAlpha = 1 - fMinorAlpha;

        a_rvSpikes.resize(0);

        c_float fMin = 0.0f - a_fCenter;
        c_float fMax = a_fSize - a_fCenter;

        c_float fMinExponent = fMin / a_fScale;
        c_float fMaxExponent = fMax / a_fScale;

        if (a_bLog)
        {
            c_float fMinValue = emp::mat::pow(10.0f, fMinExponent);
            c_float fMaxValue = emp::mat::pow(10.0f, fMaxExponent);

            c_int siMinExponent = emp::ast::cast<int>(fMinExponent);
            c_int siMaxExponent = emp::ast::cast<int>(fMaxExponent);

            c_int siStartExponent = siMinExponent - (fMin < 0 ? 1 : 0);
            c_int siEndExponent = siMaxExponent + (fMax > 0 ? 1 : 0) - 1;

            c_float fMinBase = emp::mat::pow(10.0f, emp::ast::cast<float>(siStartExponent));
            c_float fMaxBase = emp::mat::pow(10.0f, emp::ast::cast<float>(siEndExponent));

            c_int siMinCell = emp::ast::cast<int>((fMinValue - fMinBase) / (fDomain * 10.0f * fMinBase));
            c_int siMaxCell = emp::ast::cast<int>((fMaxValue - fMaxBase) / (fDomain * 10.0f * fMaxBase));

            c_float fCellCount = 0.9f / fDomain;
            c_int siCellCount = emp::ast::cast<int>(fCellCount);
            c_int siExponentCount = siEndExponent - siStartExponent;

            c_int siStartCell = siMinCell;
            c_int siEndCell = siMaxCell + siExponentCount * siCellCount;

            for (int i = siStartCell; i <= siEndCell; ++i)
            {
                c_int siExponent = siStartExponent + i / siCellCount;
                c_int siIndex = i % siCellCount;

                c_float fExponent = emp::ast::cast<float>(siExponent);
                c_float fIndex = emp::ast::cast<float>(siIndex);

                c_float fValue = emp::mat::pow(10, fExponent) * (1 + fIndex * fDomain * 10);

                c_float fPosition = a_fCenter + log10f(fValue) * a_fScale;

                c_bool bDecade = (siIndex%10) == 0;
                c_float fAlpha = bDecade ? 1.0f : fMinorAlpha;

                spike_c oSpike;
                oSpike.value = fValue;
                oSpike.position = fPosition;
                oSpike.decade = bDecade;

                oSpike.size = fAlpha;
                oSpike.alpha = emp::tt::minof(1.0f, fAlpha * fAlpha * 12.0f);
                oSpike.label = oSpike.size;
                oSpike.visible = fPosition >= 0 && fPosition <= a_fSize;
                a_rvSpikes.add(oSpike);
            }
        }
        else
        {
            c_int siMinCell = emp::ast::cast<int>(fMinExponent / fDomain);
            c_int siMaxCell = emp::ast::cast<int>(fMaxExponent / fDomain);

            c_int siStartCell = siMinCell - (fMin < 0 ? 1 : 0);
            c_int siEndCell = siMaxCell + (fMax > 0 ? 1 : 0) - 1;

            for (int i = siStartCell; i <= siEndCell; ++i)
            {
                c_float fValue = emp::ast::cast<float>(i) * fDomain;
                c_float fPosition = a_fCenter + fValue * a_fScale;
                c_bool bDecade = (i%10) == 0;
                c_float fAlpha = bDecade ? 1.0f : fMinorAlpha;
                
                spike_c oSpike;
                oSpike.value = fValue;
                oSpike.position = fPosition;
                oSpike.decade = bDecade;
                oSpike.alpha = fAlpha;
                oSpike.size = emp::tt::minof(1.0f, fAlpha * fAlpha * fAlpha * 9);
                oSpike.label = oSpike.size;
                oSpike.visible = fPosition >= 0 && fPosition <= a_fSize;
                a_rvSpikes.add(oSpike);
            }
        }
    };

    l_resize_cell(data.xlog, data.final_scale_x(), EMP_GUI_SKIN.resolution.scaled_ratio().x, v2Origin.x,
        fWidth, m_vUnitY, m_siDomainX, m_fAlphaDecadeX, true);

    l_resize_cell(data.ylog, data.final_scale_y(), EMP_GUI_SKIN.resolution.scaled_ratio().y, v2Origin.y,
        fHeight, m_vUnitX, m_siDomainY, m_fAlphaDecadeY, false);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool frame_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    c_float fScaleX = data.final_scale_x();
    c_float fScaleY = data.final_scale_y();

    {
        matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);
        c_size stCount = controls().count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(controls().val(stIndex)->transform(a_rStack)); }
        tbase1::update_colors_current(a_rStack.enabled.current());

        c_v3f v3Scale = { fScaleX, fScaleY, 1.0f };
        c_v2f v2Position = move_point(m_v2Origin, m_v2DragDelta);

        c_float fX = v2Position.x;
        c_float fY = v2Position.y;// - transformation().height();

        c_v3f v3Position = { fX, fY, 0.0f };

        m44f mModel;
        m44f::identity(mModel);
        m44f::translate(mModel, v3Position, mModel);
        m44f::scale(mModel, v3Scale, mModel);

        matrix_stacker_c oModelStacker(a_rStack, mModel, state().enable);
        m_mModel = a_rStack.matrices.current();
    }

    c_float fWidth = transformation().width();
    c_float fHeight = transformation().height();
    c_float fDepth = depth(transformation().fore_depth, 0);

    c_v2f v2Origin = move_point(m_v2Origin, m_v2DragDelta);

    c_float fCenterX = v2Origin.x;
    c_float fCenterY = v2Origin.y;

    c_float fAxisX = emp::tt::maxof(0.0f, emp::tt::minof(fWidth, fCenterX));
    c_float fAxisY = emp::tt::maxof(0.0f, emp::tt::minof(fHeight, fCenterY));

    // @@0 glcoord c_float fSpikeSize = 0.01f;
    // @@0 glcoord c_float fSpikeSizeX = fSpikeSize * skin->resolution()->scaled_ratio_x();
    // @@0 glcoord c_float fSpikeSizeY = fSpikeSize * skin->resolution()->scaled_ratio_y();

    c_v4f v4LineColor = (skin && skin->unit_line) ? skin->unit_line->rgba : v4f::all(0);
    // @@0 glcoord c_v4f v4SpikeColor = skin->unit_spike()->rgba();

    m_pMesh->create(emp::gfx::usage_e::Dynamic);

    // y lines
    {
        c_size stCount = m_vUnitX.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            spike_c const& oSpike = m_vUnitX.ref(stIndex);
            if (oSpike.visible == false)
            { continue; }
            //if (oSpike.decade == false)
            //{ continue; }

            c_v4f v4Alpha = { 1, 1, 1, oSpike.decade ? 1.0f : oSpike.alpha };

            c_float fY = oSpike.position;

            if (data.show_line)
            {
                c_v4f v4Color = v4f::mul(v4LineColor, v4Alpha);

                m_pMesh->add({ 0     , -fY , fDepth }, v4Color);
                m_pMesh->add({ fWidth, -fY , fDepth }, v4Color);
            }

            /* @@0 glcoord if (data.show_spike())
            {
                c_float fSpike = fSpikeSizeX * (oSpike.decade ? 1.0f : oSpike.alpha);
                c_float fSSub = emp::tt::maxof(0.0f, emp::tt::minof(fWidth, fAxisX - fSpike));
                c_float fSAdd = emp::tt::maxof(0.0f, emp::tt::minof(fWidth, fAxisX + fSpike));

                c_v4f v4Color = v4f::mul(v4SpikeColor, v4Alpha);

                m_pMesh->add({ fSSub, -fY }, v4Color);
                m_pMesh->add({ fSAdd, -fY }, v4Color);
            }*/
        }
    }

    // x lines
    {
        c_size stCount = m_vUnitY.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            spike_c const& oSpike = m_vUnitY.ref(stIndex);
            if (oSpike.visible == false)
            { continue; }
            //if (oSpike.decade == false)
            //{ continue; }

            c_v4f v4Alpha = { 1, 1, 1, oSpike.decade ? 1.0f : oSpike.alpha };

            c_float fX = oSpike.position;

            if (data.show_line)
            {
                c_v4f v4Color = v4f::mul(v4LineColor, v4Alpha);

                m_pMesh->add({ fX,        0 , fDepth }, v4Color);
                m_pMesh->add({ fX, -fHeight , fDepth }, v4Color);
            }

            /* @@0 glcoord if (data.show_spike())
            {
                c_float fSpike = fSpikeSizeY * (oSpike.decade ? 1.0f : oSpike.alpha);
                c_float fSSub = emp::tt::maxof(0.0f, emp::tt::minof(fHeight, fAxisY - fSpike));
                c_float fSAdd = emp::tt::maxof(0.0f, emp::tt::minof(fHeight, fAxisY + fSpike));

                c_v4f v4Color = v4f::mul(v4SpikeColor, v4Alpha);

                m_pMesh->add({ fX, fSSub }, v4Color);
                m_pMesh->add({ fX, fSAdd }, v4Color);
            }*/
        }
    }

    if (data.show_basis)
    {
        // @@0  frame
        /*float const fDivX = emp::ast::cast<float>(data.div_x());
        float const fDivY = emp::ast::cast<float>(data.div_y());

        //decade
float const fOriginDecadeX = fScaleX * emp::mat::pow(10, m_siDomainX) * 100;
float const fOriginDecadeY = fScaleY * emp::mat::pow(10, m_siDomainY) * 100;

        //unit
        float const fOriginUnitX = fOriginDecadeX / fDivX;
        float const fOriginUnitY = fOriginDecadeY / fDivY;

        // x origin
        { float const fOSubX = emp::tt::maxof(0.0f, emp::tt::minof(
                    fWidth, fAxisX - fOriginDecadeX));
        float const fOAddX = emp::tt::maxof(0.0f, emp::tt::minof(
                    fWidth, fAxisX + fOriginDecadeX));
        m_OriginDecadeX.update(
                v2f::make(fOSubX, fAxisY),
                v2f::make(fOAddX, fAxisY)); }

        { float const fOSubX = emp::tt::maxof(0.0f, emp::tt::minof(
                    fWidth, fAxisX - fOriginUnitX));
        float const fOAddX = emp::tt::maxof(0.0f, emp::tt::minof(
                    fWidth, fAxisX + fOriginUnitX));
        m_OriginUnitX.update(
                v2f::make(fOSubX, fAxisY),
                v2f::make(fOAddX, fAxisY)); }

        // y origin
        { float const fOSubY = emp::tt::maxof(0.0f, emp::tt::minof(
                    fHeight, fAxisY - fOriginDecadeY));
        float const fOAddY = emp::tt::maxof(0.0f, emp::tt::minof(
                    fHeight, fAxisY + fOriginDecadeY));
        m_OriginDecadeY.update(
                v2f::make(fAxisX, fOSubY),
                v2f::make(fAxisX, fOAddY)); }

        { float const fOSubY = emp::tt::maxof(0.0f, emp::tt::minof(
                    fHeight, fAxisY - fOriginUnitY));
        float const fOAddY = emp::tt::maxof(0.0f, emp::tt::minof(
                    fHeight, fAxisY + fOriginUnitY));
        m_OriginUnitY.update(
                v2f::make(fAxisX, fOSubY),
                v2f::make(fAxisX, fOAddY)); }*/
    }

    m_pCursorX->create(emp::gfx::usage_e::Dynamic);
    m_pCursorY->create(emp::gfx::usage_e::Dynamic);

    if (data.show_cursor)
    {
        c_float fPositionY = m_v2ClientPosition.y;
        c_float fClampedPositionY = emp::tt::maxof(0.0f, emp::tt::minof(fHeight, fPositionY));
        m_pCursorX->add({ 0     , -fClampedPositionY , fDepth });
        m_pCursorX->add({ fWidth, -fClampedPositionY , fDepth });

        c_float fPositionX = m_v2ClientPosition.x;
        c_float fClampedPositionX = emp::tt::maxof(0.0f, emp::tt::minof(fWidth, fPositionX));
        m_pCursorY->add({ fClampedPositionX,        0, fDepth });
        m_pCursorY->add({ fClampedPositionX, -fHeight, fDepth });
    }

    m_pAxisX->create(emp::gfx::usage_e::Dynamic);
    m_pAxisY->create(emp::gfx::usage_e::Dynamic);

    if (data.show_axis)
    {
        m_pAxisX->add({ 0     , -fAxisX , fDepth });
        m_pAxisX->add({ fWidth, -fAxisY , fDepth });

        m_pAxisY->add({ fAxisX,        0, fDepth });
        m_pAxisY->add({ fAxisX, -fHeight, fDepth });
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool frame_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    c_m44f& rMatrix = transformation().matrix;

    color().background.raster.render(transformation(), depth(transformation().back_depth, 0.001f));

    auto& rDevice = EMP_GRAPHICS;

    rDevice.set_depth_test(true); // @@0 depth
    rDevice.set_depth_mask(true);

    if (m_pMesh->count() > 0)
    {
        rDevice.set_vertex_buffer(&*m_pMesh);
        rDevice.set_index_buffer(rDevice.line_index_buffer(m_pMesh->count() * 2));
        rDevice.set_program(rDevice.color_attribute_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    if (data.show_axis)
    {
        rDevice.set_program(rDevice.color_uniform_program());

        rDevice.set_vertex_buffer(&*m_pAxisX);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, skin->cursor_x->rgba);
        rDevice.draw(emp::gfx::primitive_e::Lines);

        rDevice.set_vertex_buffer(&*m_pAxisY);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, skin->cursor_y->rgba);
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    on_render(*this);

    EMP_RET_FALSE_IF_NOT(position_x.render());
    EMP_RET_FALSE_IF_NOT(position_y.render());

    if (data.show_cursor)
    {
        if (skin && skin->cursor_x)
        {
            rDevice.set_vertex_buffer(&*m_pCursorX);
            rDevice.set_index_buffer(rDevice.line_index_buffer(2));
            rDevice.set_program(rDevice.color_uniform_program());
            rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
            rDevice.set_uniform(uniform_e::v4Color, skin->cursor_x->rgba);
            rDevice.draw(emp::gfx::primitive_e::Lines);
        }

        if (skin && skin->cursor_y)
        {
            rDevice.set_vertex_buffer(&*m_pCursorY);
            rDevice.set_index_buffer(rDevice.line_index_buffer(2));
            rDevice.set_program(rDevice.color_uniform_program());
            rDevice.set_uniform(uniform_e::m44Transform, rMatrix);
            rDevice.set_uniform(uniform_e::v4Color, skin->cursor_y->rgba);
            rDevice.draw(emp::gfx::primitive_e::Lines);
        }
    }

    if (data.show_basis)
    {
        rDevice.set_program(rDevice.color_uniform_program());
        rDevice.set_uniform(uniform_e::m44Transform, rMatrix);

        c_v4f v4OriginX = skin->origin_x->rgba;
        c_v4f v4OriginY = skin->origin_y->rgba;

        rDevice.set_vertex_buffer(&*m_pOriginUnitX);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, v4OriginX);
        rDevice.draw(emp::gfx::primitive_e::Lines);

        rDevice.set_vertex_buffer(&*m_pOriginUnitY);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, v4OriginY);
        rDevice.draw(emp::gfx::primitive_e::Lines);

        c_v4f v4AlphaDecadeX = { 1, 1, 1, m_fAlphaDecadeX };
        c_v4f v4AlphaDecadeY = { 1, 1, 1, m_fAlphaDecadeY };

        rDevice.set_vertex_buffer(&*m_pOriginDecadeX);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, v4f::mul(v4OriginX, v4AlphaDecadeX));
        rDevice.draw(emp::gfx::primitive_e::Lines);

        rDevice.set_vertex_buffer(&*m_pOriginDecadeY);
        rDevice.set_index_buffer(rDevice.line_index_buffer(2));
        rDevice.set_uniform(uniform_e::v4Color, v4f::mul(v4OriginY, v4AlphaDecadeY));
        rDevice.draw(emp::gfx::primitive_e::Lines);
    }

    color().background.border.render(transformation(), depth(transformation().fore_depth, 0.002f));

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f frame_c::get_frame_size(v2f::tparam a_v2Client) const
{
    c_float fScaleX = data.final_scale_x();
    c_float fScaleY = data.final_scale_y();

    c_v2f v2Scale = { fScaleX, fScaleY };
    c_v2f v2Size = v2f::div(a_v2Client, v2Scale);
    return v2Size;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f frame_c::move_point(v2f::tparam a_v2Point, v2f::tparam a_v2Delta) const
{
    /* @@0 glcoord
    c_v2f v2Scale = { data.final_scale_x(), data.final_scale_y() };

    c_v2f v2Min = data.min_domain();
    c_v2f v2Max = data.max_domain();

    c_v2f v2MinSize = v2f::mul(v2Scale, v2Min);
    c_v2f v2MaxSize = v2f::mul(v2Scale, v2Max);

    c_v2f v2Size = transformation().m_v2fSize;

    c_float fMinX = v2MinSize.x;
    c_float fMinY = v2MinSize.y;
    c_float fMaxX = v2MaxSize.x;
    c_float fMaxY = v2MaxSize.y;*/

    c_v2f v2Final = v2f::add(a_v2Point, a_v2Delta);
    float fFinalX = v2Final.x;
    float fFinalY = v2Final.y;

    /* @@0 glcoord
    c_float fWidth = v2Size.x;
    if (fFinalX + fMaxX < fWidth)
    { fFinalX = fWidth - fMaxX; }
    if (fFinalX + fMinX > 0)
    { fFinalX = -fMinX; }

    c_float fHeight = v2Size.y;
    if (fFinalY + fMaxY < fHeight)
    { fFinalY = fHeight - fMaxY; }
    if (fFinalY + fMinY > 0)
    { fFinalY = -fMinY; }*/

    c_v2f v2NewPoint = { fFinalX, fFinalY };

    return v2NewPoint;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f frame_c::client_to_frame(v2f::tparam a_v2Client) const
{
    c_v2f v2Scale = { data.final_scale_x(), data.final_scale_y() };
    c_v2f v2Origin = move_point(m_v2Origin, m_v2DragDelta);
    c_v2f v2ScaledOrigin = v2f::div(v2Origin, v2Scale);
    c_v2f v2ScaledPosition = v2f::div(a_v2Client, v2Scale);
    c_v2f v2Chart = v2f::sub(v2ScaledPosition, v2ScaledOrigin);

    c_float fChartX = v2Chart.x;
    c_float fChartY = v2Chart.y;

    c_v2f v2Frame =
    {
        data.xlog ? emp::mat::pow(10.0f, fChartX) : fChartX,
        data.ylog ? emp::mat::pow(10.0f, fChartY) : fChartY
    };

    return v2Frame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN c_v2f frame_c::frame_to_client(v2f::tparam a_v2Chart) const
{
    c_v2f v2Scale = { data.final_scale_x(), data.final_scale_y() };
    c_v2f v2Origin = move_point(m_v2Origin, m_v2DragDelta);
    c_v2f v2ScaledOrigin = v2f::div(v2Origin, v2Scale);

    c_float fChartX = a_v2Chart.x;
    c_float fChartY = a_v2Chart.y;

    c_v2f v2Chart =
    {
        data.xlog ? log10f(fChartX) : fChartX,
        data.ylog ? log10f(fChartY) : fChartY
    };

    c_v2f v2ScaledClient = v2f::add(v2Chart, v2ScaledOrigin);
    c_v2f v2Client = v2f::mul(v2ScaledClient, v2Scale);

    return v2Client;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e frame_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    if (state().enable == false)
    { return return_e::Unused; }

    auto& rMouse = a_rEvent.mouse;
    auto& rKeyboard = a_rEvent.keyboard;

    c_v2f v2fPosition = rMouse.m_v2fPosition;
    c_v2f v2fRelPosition = rMouse.m_v2fPosition;//v2f::add(v2f::one(), v2fPosition);

    c_bool bContains = state().hover;
    //contains(transformation().matrix(), transformation().size(), v2fPosition);
    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();
    c_bool bDragged = a_rEvent.mouse_dragged();

    c_bool bModified = rKeyboard.modified();
    c_size stTapCount = rMouse.m_stTapCount;

    // @@0  consumed ?
    if (bContains)
    { m_v2Position = v2fPosition; }

    // @@0  ?
    if (bModified )
    { return return_e::Unused; }

    emp::app::c_button_e eButton = rMouse.m_eButton;
    if (eButton == emp::app::button_e::Left)
    {
        if (stTapCount == 1)
        {
            if (bContains)
            {
                if (bPressed)
                {
                    if (bDragged == false)
                    {
                        m_bDrag = true;
                        m_v2DragOrigin = v2fRelPosition;
                        m_v2DragDelta = v2f::zero();
                    }

                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
                else if (bReleased)
                {
                    if (m_bDrag)
                    {
                        m_bDrag = false;
                        move_origin(m_v2DragDelta);
                        m_v2DragOrigin = v2f::zero();
                        m_v2DragDelta = v2f::zero();
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
                else if (bDragged)
                {
                    if (m_bDrag)
                    {
                        m_v2DragDelta = v2f::sub(v2fRelPosition, m_v2DragOrigin);
                        return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                    }
                }
            }
            else
            {
                if (m_bDrag)
                {
                    m_bDrag = false;
                    move_origin(m_v2DragDelta);
                    m_v2DragOrigin = v2f::zero();
                    m_v2DragDelta = v2f::zero();
                    return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
                }
            }
        }
        else if (bPressed && bContains)
        {
        }
    }
    else if (eButton == emp::app::button_e::WheelUp)
    {
        if (bContains && bPressed && bModified == false)
        {
            scale(true, true, true);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    else if (eButton == emp::app::button_e::WheelDown)
    {
        if (bContains && bPressed && bModified == false)
        {
            scale(false, true, true);
            return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void frame_c::scale(c_bool a_bUp, c_bool a_bX, c_bool a_bY)
{
    c_v2f v2OldBaseScale = { data.final_scale_x(), data.final_scale_y() };

    if (a_bX && a_bY)
    {
        if (a_bUp)
        { data.scale_up(); }
        else
        { data.scale_down(); }
    }
    else if (a_bX)
    {
        if (a_bUp)
        { data.xscale_up(); }
        else
        { data.xscale_down(); }
    }
    else if (a_bY)
    {
        if (a_bUp)
        { data.yscale_up(); }
        else
        { data.yscale_down(); }
    }

    c_v2f v2NewBaseScale = { data.final_scale_x(), data.final_scale_y() };
    c_v2f v2Tmp = v2f::sub(m_v2ClientPosition, m_v2Origin);
    c_v2f v2Delta = v2f::mul(v2f::all(-1), v2f::div(v2f::mul(v2Tmp, v2f::sub(v2NewBaseScale, v2OldBaseScale)), v2OldBaseScale));

    move_origin(v2Delta);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
frame_c& frame_c::skin_(cpc_frame_skin_s a_pValue)
{
    skin = a_pValue;

    tbase1::skin_(a_pValue ? a_pValue->container : nullptr);

    position_x.skin_(a_pValue ? a_pValue->position_x : nullptr);
    position_y.skin_(a_pValue ? a_pValue->position_y : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

