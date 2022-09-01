//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_label.h"

#include "emp_gui_button.h"
#include "emp_gui_manager.h"
#include "emp_gui_skin.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_gfx_text.h"

#include "emp_rfx_type_i.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
label_c::label_c(void):
    m_mModel(),
    skin(),
    m_pText(),
    m_peString(),
    m_sText(),
    m_pReflection(),
    m_bRenderText(true),
    m_Color()
{
    on_push.reset(this, false);
    child_(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
label_c::~label_c(void)
{
    if (m_peString != nullptr)
    {
        EMP_DEALLOCATE(m_peString);
        m_peString = nullptr;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool label_c::post_load(void)
{
    skin.data = EMP_GUI_SKIN.label.get(skin.name);
    // @@0 uncomment EMP_RET_FALSE_IF(skin.data == nullptr, "Failed to load skin '#0'", skin.name);
    skin_(skin.data);
    // @@0 create create();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void label_c::create(void)
{
    EMP_ASSERT_BOOL_CALL(m_pText->create()); // @@0 return
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void label_c::destroy(void)
{
    // @@0 legit remove ? if ((&*m_pOnPush) != nullptr)
    {
        on_push.reset(false);
    }

    if ((&*m_pText) != nullptr)
    {
        m_pText->destroy();
    }

    m44f::identity(m_mModel);

    if (m_peString != nullptr)
    {
        EMP_DEALLOCATE(m_peString);
        m_peString = nullptr;
    }

    m_pReflection = nullptr;

    m_bRenderText = true;

    m_sText = u8string();
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* label_c::got_focus(c_focus_e EMP_SILENT(a_eFocus), c_bool EMP_SILENT(a_bSelect), c_bool a_bPush, c_change_e a_eChange)
{
    if (state().focusable)
    {
        if (push_on_focus && a_bPush)
        {
            if (on_push.is_valid())
            {
                emp::tt::silent(on_push.set_state(true, a_eChange));
                emp::tt::silent(on_push.set_state(false, a_eChange));
            }
        }

        if (emp::tt::is_not_max(state().index))
        { return this; }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool label_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    c_bool bLeft =
        alignment == alignment_e::TopLeft ||
        alignment == alignment_e::MiddleLeft ||
        alignment == alignment_e::BottomLeft;

    c_bool bRight =
        alignment == alignment_e::TopRight ||
        alignment == alignment_e::MiddleRight ||
        alignment == alignment_e::BottomRight;

    m_pText->set_alignment(bLeft ? -1 : bRight ? 1 : 0);

    c_v2f v2FontSize = compute_font_size();
    m_pText->set_font_size_x(v2FontSize.x);
    m_pText->set_font_size_y(v2FontSize.y);

    if (m_peString != nullptr)
    { m_pText->set_text(EMP_LOCALIZE(*m_peString)); }
    else if (m_pReflection != nullptr)
    {
        m_su8Text.clear();
        c_bool bResult = m_pReflection->to_string(formater, m_su8Text);
        EMP_ASSERT(bResult);
        if (bResult == true)
        { m_pText->set_text(m_su8Text); }
    }

    c_bool bAutoSizeX = state().autosize.x;
    c_bool bAutoSizeY = state().autosize.y;
    c_bool bAutoScale = (skin.data != nullptr && skin.data->font != nullptr) ? skin.data->font->autoscale : false;

    if (m_pText->font() == nullptr)
    {
        if (bAutoScale)
        {
            // @@0  skin
            //m_Font.set_size(0);
            //m_pText->set_font_size_x(0);
            //m_pText->set_font_size_y(0);
            EMP_ASSERT_UNREACHABLE();
        }
        else if (bAutoSizeX || bAutoSizeY)
        { transformation().size_(0); }
    }
    else
    {
        c_v2f v2Zero = v2f::zero();
        c_v2f v2AutoSize = { bAutoSizeX ? 1.0f : 0.0f, bAutoSizeY ? 1.0f : 0.0f };

        cpc_padding_data_s pPadding = (skin.data && skin.data->button && skin.data->button->padding) ?
            skin.data->button->padding : nullptr;
        c_float fPaddingX = pPadding ? pPadding->x() * scale_x : 0;
        c_float fPaddingY = pPadding ? pPadding->y() * scale_y : 0;

        c_v2f v2Padding = { fPaddingX, fPaddingY };
        c_v2f v2FreeSize = v2f::sub(transformation().size, v2Padding);
        c_v2f v2Bounds = v2f::select(v2AutoSize, v2Zero, v2FreeSize);

        if (bAutoScale)
        {
             // @@0 fixme skin
             /*m_pText->set_bounds(v2f::select(v2AutoSize, v2Zero, v2FreeSize));
             m_pText->refresh();
             c_v2f v2FontSize = v2f::div(v2FreeSize, v2f::div(m_pText->autosize(), v2FontSize));
             c_float fNewFontSize = emp::tt::minof(v2f::get_x(v2FontSize), v2f::get_y(v2FontSize));
             m_Font.set_size(fNewFontSize);
             m_pText->set_font_size_x(fNewFontSize);
             m_pText->set_font_size_y(fNewFontSize);*/
            EMP_ASSERT_UNREACHABLE();
        }
        else if (bAutoSizeX || bAutoSizeY)
        {
            if ((v2Bounds.x > 0 || bAutoSizeX) &&
                (v2Bounds.y > 0 || bAutoSizeY))
            {
                m_pText->set_bounds(v2Bounds);
                EMP_RET_FALSE_IF_NOT(m_pText->refresh());
                c_v2f v2NewSize = v2f::select(v2AutoSize, m_pText->autosize(), v2FreeSize);
                transformation().size_(v2f::add(v2Padding, v2NewSize));
            }
        }
        else
        {
            if (v2Bounds.x > 0 &&
                v2Bounds.y > 0)
            {
                m_pText->set_bounds(v2f::select(v2AutoSize, v2Zero, v2FreeSize));
                EMP_RET_FALSE_IF_NOT(m_pText->refresh());
            }
        }
    }

    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool label_c::transform(stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);

    if (skin.data)
    { compute_label_color(*skin.data, state(), a_rStack.enabled.current(), m_Color); }

    emp::gfx::font_c const* const pFont = m_pText->font();
    if (pFont != nullptr)
    {
        cpc_padding_data_s pPadding = skin.data->button ? skin.data->button->padding : nullptr;

        c_float fPaddingX = pPadding ? pPadding->x() * scale_x : 0;
        c_float fPaddingY = pPadding ? pPadding->y() * scale_y : 0;

        c_float fPaddingLeft = pPadding ? pPadding->left() * scale_x : 0;
        c_float fPaddingRight = pPadding ? pPadding->right() * scale_x : 0;
        c_float fPaddingTop = pPadding ? pPadding->top() * scale_y : 0;
        c_float fPaddingBottom = pPadding ? pPadding->bottom() * scale_y : 0;

        c_float fFontSizeY = m_pText->font_size_y();
        c_v3f v3FontSize = { m_pText->font_size_x(), fFontSizeY, 1.0f };

        c_v2f v2TextSize = m_pText->autosize();
        c_float fControlWidth = v2TextSize.x;
        c_float fControlHeight = v2TextSize.y;

        c_v2f v2ControlSize = transformation().size;
        c_float fWidth = v2ControlSize.x;
        c_float fHeight = v2ControlSize.y;

        c_bool bLeft =
            alignment == alignment_e::TopLeft ||
            alignment == alignment_e::MiddleLeft ||
            alignment == alignment_e::BottomLeft;

        c_bool bCenter =
            alignment == alignment_e::TopCenter ||
            alignment == alignment_e::MiddleCenter ||
            alignment == alignment_e::BottomCenter;

        c_bool bRight =
            alignment == alignment_e::TopRight ||
            alignment == alignment_e::MiddleRight ||
            alignment == alignment_e::BottomRight;

        c_bool bTop =
            alignment == alignment_e::TopLeft ||
            alignment == alignment_e::TopCenter ||
            alignment == alignment_e::TopRight;

        c_bool bMiddle =
            alignment == alignment_e::MiddleLeft ||
            alignment == alignment_e::MiddleCenter ||
            alignment == alignment_e::MiddleRight;

        c_bool bBottom =
            alignment == alignment_e::BottomLeft ||
            alignment == alignment_e::BottomCenter ||
            alignment == alignment_e::BottomRight;

        float fOffsetX = 0;
        if (bLeft)
        { fOffsetX += fPaddingLeft; }
        else if (bCenter)
        { fOffsetX += fPaddingLeft + (fWidth - fPaddingX - fControlWidth)*0.5f;}
        else if (bRight)
        { fOffsetX += fWidth - fPaddingRight - fControlWidth; }

        float fOffsetY = 0;
        if (bTop)
        { fOffsetY -= fHeight - fPaddingTop - fControlHeight; }
        else if (bMiddle)
        { fOffsetY -= fPaddingBottom + (fHeight - fPaddingY - fControlHeight) * 0.5f; }
        else if (bBottom)
        { fOffsetY -= fPaddingBottom; }

        c_v3f v3Position = { fOffsetX, -fOffsetY + pFont->line_under_height() * fFontSizeY - transformation().height(), -0.001f };

        m44f mModel;
        m44f::identity(mModel);
        m44f::translate(mModel, v3Position, mModel);
        m44f::scale(mModel, v3FontSize, mModel);

        matrix_stacker_c oModelStacker(a_rStack, mModel, state().enable);
        m_mModel = a_rStack.matrices.current();
    }

    c_size stCount = controls().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(controls().val(stIndex)->transform(a_rStack)); }
    update_colors_current(a_rStack.enabled.current());

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e label_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_keyboard(a_rEvent));

    auto const& rKeyboard = a_rEvent.keyboard;

    if (state().enable &&
        rKeyboard.modified() == false &&
        (push_on_repeat || rKeyboard.m_bRepeat == false))
    {
        emp::app::c_key_e eKey = rKeyboard.m_eKey;
        if (eKey == emp::app::key_e::Enter ||
            eKey == emp::app::key_e::Space)
        {
            if (focus_in_hierarchy(this))
            {
                c_change_e eChange = change_e::Keyboard;
                EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, false, eChange);

                c_bool bPressed = rKeyboard.m_bPressed;
                if ((bPressed || on_push.value) &&
                    on_push.is_valid())
                {
                    return on_push.set_state(bPressed, eChange);
                }
            }
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e label_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    if (state().hover &&
        state().enable &&
        parent_enable(this))
    {
        auto const& rMouse= a_rEvent.mouse;
        if (a_rEvent.keyboard.modified() == false &&
            rMouse.m_eButton == emp::app::button_e::Left)
        {
            c_change_e eChange = change_e::Mouse;
            if (a_rEvent.mouse_pressed())
            {
                if (focus_in_hierarchy(this) == false)
                { EMP_GUI_MANAGER->activate_focus(this, focus_e::First, true, false, eChange); }

                if (push_on_double_click || rMouse.m_stTapCount == 1)
                {
                    if (on_push.is_valid())
                    { return on_push.set_state(true, eChange); }
                }
            }
            else if (a_rEvent.mouse_released())
            {
                if (on_push.value)
                {
                    if (push_on_double_click || rMouse.m_stToucheCount == 1)
                    {
                        if (on_push.is_valid())
                        { return on_push.set_state(false, eChange); }
                    }
                }
            }
        }
    }

    return return_e::Unused;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool label_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    /*color_data_s oColor;
    oColor.background().raster().set(EMP_GAME().gui_skin().data()->material_color(), v4f::make(0, 1, 1, 1));
    oColor.background().update(transformation().size());
    oColor.background().render(transformation().matrix(), alpha);*/

    // @@0 remove alpha
    if (m_pText->can_render() &&
        render_text)
    {
        bool bEmpty = false;
        if (scissor)
        { EMP_GUI_MANAGER_CLIP(oScissor, false, transformation(), bEmpty, 0.5f, -1.0f, 0, 0); }
        if (bEmpty == false)
        { m_pText->draw(v4f::mul(m_Color.rgba, alpha), m_mModel, transformation().fore_depth); }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void label_c::set_text(c_u8string& a_sValue)
{
    if (m_peString != nullptr)
    {
        EMP_DEALLOCATE(m_peString);
        m_peString = nullptr;
    }
    m_pReflection = nullptr;

    m_sText = a_sValue;
    m_pText->set_text(a_sValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void label_c::set_text(emp::loc::string_s const& a_eValue)
{
    if (m_peString == nullptr)
    { m_peString = EMP_ALLOCATE(emp::loc::string_s, a_eValue); } //-V747
    else if (emp::str::cmp(m_peString->key, a_eValue.key) == false)
    { *m_peString = a_eValue; }
    m_pReflection = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void label_c::set_text(emp::rfx::type_i* const a_pValue)
{
    if (m_peString != nullptr)
    {
        EMP_DEALLOCATE(m_peString);
        m_peString = nullptr;
    }
    m_pReflection = a_pValue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::cnt::vector<v2f::type> const& label_c::positions(void) const
{ return m_pText->positions(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
label_c& label_c::skin_(cpc_label_skin_s a_pValue)
{
    skin.data = a_pValue;
    tbase1::skin_((a_pValue && a_pValue->button && a_pValue->button->container) ? a_pValue->button->container : nullptr);
    m_pText->set_font((a_pValue && a_pValue->font && a_pValue->font->pointer) ? a_pValue->font->pointer : nullptr);
    return *this;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v2f::tret label_c::compute_font_size(void) const
{
    cpc_label_skin_s pLabel = skin.data;
    if (pLabel == nullptr)
    { return { 0.0f, 0.0f}; }

    emp::gui::cpc_font_data_s pFontData = pLabel->font;
    if (pFontData == nullptr)
    { return { 0.0f, 0.0f}; }

    auto const& rResolution = EMP_GUI_SKIN.resolution;
    c_float fFontSize = pFontData->size * rResolution.scale();
    c_float fScaleX = fFontSize * scale_x;
    c_float fScaleY = fFontSize * scale_y;
    c_float fFontSizeX = fScaleX * rResolution.ratio().x;
    c_float fFontSizeY = fScaleY * (m_pText->font()->aspect_ratio() ? rResolution.ratio().y : rResolution.ratio_xy());
    return { fFontSizeX, fFontSizeY };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

