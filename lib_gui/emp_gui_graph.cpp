//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_gui_graph.h"

#include "emp_gui_button.h"
#include "emp_gui_function.h"
#include "emp_gui_icon.h"
#include "emp_gui_label.h"
#include "emp_gui_manager.h"
#include "emp_gui_mdi.h"
#include "emp_gui_toolbar.h"
#include "emp_gui_nurbs.h"
#include "emp_gui_popup.h"
#include "emp_gui_property.h"
#include "emp_gui_registrar_i.h"
#include "emp_gui_resolution.h"
#include "emp_gui_skin.h"
#include "emp_gui_slider.h"
#include "emp_gui_spacer.h"
#include "emp_gui_splitter.h"
#include "emp_gui_text.h"
#include "emp_gui_window.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_event.h"

#include "emp_fct_function.h"

#include "emp_gfx_nurbs.h"

#include "emp_mat_nurbs.h"

#include "emp_rfx_factory.h"

#include "emp_snd_envelope.h"
#include "emp_snd_soft_synth.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::tt;
using namespace emp::rfx;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
node_function_c::node_function_c(void):
    function()
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
node_function_c::~node_function_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::create(void)
{
    destroy();

    // @@0  skin
    // @@0  : remove global

    // @@0  skin
    function.skin_(EMP_GUI_SKIN_(function, "fct."));
    function.state().dock_(dock_e::Fill);
    function.transformation().size_(0.3f);
    function.show_coordinates_(true);
    function.frame.data
        .show_position_(false)
        .show_basis_(false)
        .show_axis_(false)
        .show_cursor_(false)
        .show_spike_(false)
        .show_line_(false)
        .scale_(0.08f);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::destroy(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* node_function_c::control(void)
{ return &function; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0  skin
void node_function_c::set_skin(cpc_graph_skin_s)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::set_function(emp::mat::function_i<precision_d>* const a_pValue, c_bool EMP_SILENT(a_bNew))
{ function.function_(a_pValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::center(void)
{ function.frame.center_origin(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void update_info_imp(c_frame_data_s& a_rFrom, info_s& a_rInfo);
EMP_INTERN void update_info_imp(c_frame_data_s& a_rFrom, info_s& a_rInfo)
{
    a_rInfo
        .scale_(a_rFrom.scale)
        .xscale_(a_rFrom.xscale)
        .yscale_(a_rFrom.yscale)
        .xlog_(a_rFrom.xlog)
        .ylog_(a_rFrom.ylog);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void set_info_imp(c_info_s& a_rInfo, frame_data_s& a_rTo);
EMP_INTERN void set_info_imp(c_info_s& a_rInfo, frame_data_s& a_rTo)
{
    a_rTo
        .scale_(a_rInfo.scale)
        .xscale_(a_rInfo.xscale)
        .yscale_(a_rInfo.yscale)
        .xlog_(a_rInfo.xlog)
        .ylog_(a_rInfo.ylog);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::update(emp::snd::soft_synth_c* const a_pSynth, c_ui32 EMP_SILENT(a_ui32Function))
{ function.cash_(a_pSynth->cash()); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::update_info(info_s& a_rInfo) const
{ update_info_imp(function.frame.data, a_rInfo); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::set_info(c_info_s& a_rInfo)
{ set_info_imp(a_rInfo, function.frame.data); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::resolution_changed(resolution_c const* const a_pSender)
{ function.frame.set_origin(v2f::mul(function.frame.origin(), a_pSender->size_ratio())); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_function_c::set_current_frequency(float const a_fValue)
{ function.current_frequency_(a_fValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
envelope_function_c::envelope_function_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
envelope_function_c::~envelope_function_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::create(void)
{
    destroy();

    nurbs.skin_(EMP_GUI_SKIN_(nurbs, "nrb."));
    nurbs.transformation().size_(0.3f, 0.3f);
    nurbs.state().dock_(dock_e::Fill);
    nurbs.frame.data.show_position_(false)
        .show_basis_(false)
        .show_axis_(false)
        .show_cursor_(true)
        .show_spike_(true)
        .show_line_(true)
        .min_(v2f::zero());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::destroy(void)
{
    nurbs.destroy();
    envelope = nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::set_function(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew)
{
    nurbs.nurbs_(nullptr);
    envelope = nullptr;
    if (a_pValue->operation() == static_cast<emp::mat::operation_e>(emp::mat::sound_operation_e::Envelope))
    {
        envelope = static_cast<emp::mat::envelope_t<precision_d>*>(a_pValue);
        emp::mat::nurbs_c* const pNurbs = envelope->nurbs();
        nurbs.nurbs_(pNurbs);
        if (a_bNew)
        {
            pNurbs->destroy();
            pNurbs->add_point({ 0, 0, 0, 1 }, false);
            pNurbs->add_point({ 0.02f, 1, 0, 1 }, false);
            pNurbs->add_point({ 0.1f, 0.1f, 0, 1 }, false);
            pNurbs->add_point({ 1, 0, 0, 1 }, false);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN control_i* envelope_function_c::control(void)
{ return &nurbs; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::update(emp::snd::soft_synth_c* const a_pSynth, c_ui32 a_ui32Function)
{
    if (a_ui32Function < a_pSynth->function_count())
    {
        emp::snd::function_c& rFunction = a_pSynth->get_function(a_ui32Function);
        emp::mat::nurbs_c const& rNurbs = *nurbs.nurbs;

        bool bChanged = false;
        c_size stCount = rNurbs.points().count();
        if (stCount != rFunction.params().count())
        { bChanged = true; }
        else
        {
            c_size stParamCount = rFunction.params().count();
            for (size_t stIndex = 0; stIndex < stParamCount; ++stIndex)
            {
                c_v2f v0 = v4f::xy(rNurbs.points().val(stIndex));
                c_v2f v1 = v4f::xy(rFunction.params().ref(stIndex));
                if (bool_equal(v0, v1) == false)
                {
                    bChanged = true;
                    break;
                }
            }
        }

        rFunction.params().resize(stCount);
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_v4f v4Point = rNurbs.points().val(stIndex);
            rFunction.params().set(stIndex, v4Point);
        }

        if (bChanged && envelope)
        {
            EMP_LOG_INFO("BUILD");
            envelope->envelope()->build();
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::update_info(info_s& a_rInfo) const
{ update_info_imp(nurbs.frame.data, a_rInfo); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::set_info(c_info_s& a_rInfo)
{ set_info_imp(a_rInfo, nurbs.frame.data); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void envelope_function_c::resolution_changed(resolution_c const* const a_pSender)
{ nurbs.frame.set_origin(v2f::mul(nurbs.frame.origin(), a_pSender->size_ratio())); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
node_c::node_c(void):
    designer(4u)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
node_c::~node_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::create(void)
{
    destroy();

    // @@0 create output.create();
    output.state().dock_(dock_e::Left).index_(0).focusable_(true).autosize.xy(true);
    output.set_text(U8LITERAL("G"));
    output.on_push.function_(::function(this, &node_c::show_output));

    remove.state().dock_(dock_e::Right).index_(1).focusable_(true).autosize.xy(true);
    remove.on_push.function_(::function(this, &node_c::remove_node));

    header.state().dock_(dock_e::Top).autosize.y_(true);

    header.add_control(&output);
    header.add_control(&remove);

    spacer_0.state().dock_(dock_e::Top);

    property.expand_last_column_(true)
        .state().dock_(dock_e::Top).focusable_(true).index_(0).autosize.y_(true);
    EMP_RET_FALSE_IF_NOT(property.set_object(nullptr));

    reflection = EMP_ALLOCATE(type_t<astring>, fct::function(this, &node_c::get_function_value),
        fct::function(this, &node_c::set_function_value));

    emp::loc::string_s sName(ALITERAL("f(*)"), U8LITERAL("f(*)"));
    property.add_object(emp::rfx::field_s(reflection, sName.key, sName, sName));

    spacer_1.state().visible_(false).dock_(dock_e::Top);

    input_container.state().dock_(dock_e::Left);

    input_spacer.state().dock_(dock_e::Left).autosize.y_(true);

    input_property.expand_last_column_(true).designer_(&designer)
        .state().dock_(dock_e::Fill).focusable_(true).index_(0).autosize.y_(true);

    input_docker.state().dock_(dock_e::Top).index_(1).autosize.y_(true);
    input_docker.add_control(&input_container);
    input_docker.add_control(&input_spacer);
    input_docker.add_control(&input_property);

    output_docker.state().dock_(dock_e::Bottom).autosize.y_(true);

    docker.state().index_(0).dock_(dock_e::Fill);

    main.state().index_(0);
    main.add_control(&header);
    main.add_control(&docker);

    button.control_(&main);

    set_control(&button);

    format.m_tParams.precision(6);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::autosize(void)
{
    output_docker.state().dock_(dock_e::Top);

    docker.state().dock_(dock_e::Top).autosize.xy(true);

    m_pFunction->control()->state().dock_(dock_e::Top);
    m_pFunction->control()->transformation().height_(0.4f);

    main.state().autosize.xy(true);
    button.state().autosize.xy(true);
    state().autosize.xy(true);

    EMP_RET_FALSE_IF_NOT(resize());

    output_docker.state().dock_(dock_e::Bottom);

    docker.state().dock_(dock_e::Fill).autosize.xy(false);

    m_pFunction->control()->state().dock_(dock_e::Fill);

    main.state().autosize.xy(false);
    button.state().autosize.xy(false);
    state().autosize.xy(false);

    m_pFunction->center();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::resize_function(void)
{
    //if (m_pFunction->control()->state().visible)
    { return autosize(); }
    //else
    //{ return autoresize(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::update_visibility(void)
{
    m_pFunction->control()->state().visible_(output.state().check);
    spacer_1.state().visible_(output.state().check);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::autoresize(void)
{
    output_docker.state().dock_(dock_e::Top);
    docker.state().dock_(dock_e::Top).autosize.xy(true);
    m_pFunction->control()->state().dock_(dock_e::Top);
    main.state().autosize.xy(true);
    button.state().autosize.xy(true);
    state().autosize.xy(true);

    EMP_RET_FALSE_IF_NOT(resize()); // @@0 bug
    EMP_RET_FALSE_IF_NOT(resize());

    output_docker.state().dock_(dock_e::Bottom);
    docker.state().dock_(dock_e::Fill).autosize.xy(false);
    m_pFunction->control()->state().dock_(dock_e::Fill);
    main.state().autosize.xy(false);
    button.state().autosize.xy(false);
    state().autosize.xy(false);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::set_node_function(node_function_i* const a_pValue)
{
    m_pFunction = a_pValue;

    docker.clear_controls();

    docker.add_control(&property);
    docker.add_control(&spacer_0);
    docker.add_control(&input_docker);
    docker.add_control(&spacer_1);

    m_pFunction->control()->state().dock_(dock_e::Fill);

    docker.add_control(m_pFunction->control());
    docker.add_control(&output_docker);

    on_resolution_changed = emp::fct::function(a_pValue, &node_function_i::resolution_changed);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::expression_changed(c_astring& a_sValue)
{
    synth->set_function_expression(function, a_sValue);
    EMP_RET_FALSE_IF_NOT(pinout_changed(synth->function(function), false));
    return resize_function();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN item_i* new_slider(type_i* const a_pReflection, fct_on_edit_d const& EMP_SILENT(a_rFctOn),
    cpc_list_skin_c a_pSkin, c_size a_stCount);
EMP_INTERN EMP_RETURN item_i* new_slider(type_i* const a_pReflection, fct_on_edit_d const& EMP_SILENT(a_rFctOn),
    cpc_list_skin_c a_pSkin, c_size a_stCount)
{
    type_t<astring>* const pReflection = EMP_FACTORY.cast<astring>(a_pReflection);
    item_t<slider_c>* const pSlider = new_item<slider_c>(a_pSkin ? a_pSkin->slider : nullptr, a_pReflection, emp::ast::cast<ui32>(a_stCount));
    pSlider->control().set_text(pReflection);
    pSlider->control().state().focusable_(true);
    return pSlider;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::pinout_changed(emp::mat::function_i<precision_d>* const a_pValue, c_bool a_bNew)
{
    m_pFunction->set_function(a_pValue, a_bNew);

    EMP_RET_FALSE_IF_NOT(input_property.set_object(nullptr));

    input_container.clear_controls();
    output_docker.clear_controls();

    m_stInputCount = 0;
    m_stOutputCount = 0;

    if (a_pValue)
    {
        c_size stInputCount = a_pValue->input_count();
        c_size stOutputCount = a_pValue->result();

        m_vfValues.resize(stInputCount);
        m_vsValues.resize(stInputCount);

        for (size_t stIndex = 0; stIndex < stInputCount; ++stIndex)
        {
            emp::mat::input_t<precision_d> const& rInput = *a_pValue->get_input(stIndex);

            type_i* pReflection = nullptr;
            picture_c* pPicture = nullptr;
            if (stIndex < reflections.count())
            {
                pReflection = reflections.val(stIndex);
                pPicture = input_pictures.val(stIndex);
            }
            else
            {
                type_t<astring>* const pClass = EMP_ALLOCATE(type_t<astring>,
                    bind(::function(this, &node_c::get_value), stIndex),
                    bind(::function(this, &node_c::set_value), stIndex));

                reflections.add(pClass);

                pReflection = pClass;

                auto& rDsnClass = designer.push<astring>(4u);
                rDsnClass.field.expanded_(false).on_new_item_(emp::fct::function(&new_slider));
                rDsnClass.push(rInput.name()).on_enability_(bind(::function(this, &node_c::get_enable), stIndex));

                // @@0  lib_str allocation
                pPicture = EMP_ALLOCATE(picture_c);
                input_pictures.add(pPicture);

                pPicture->state().focusable_(true).autosize.x_(true);
                pPicture->skin_(skin ? skin->pin_picture : nullptr).model_(skin ? skin->gfx_pin : nullptr);
            }

            emp::loc::string_s sName(rInput.name(), u8string(rInput.name()));
            input_property.add_object(emp::rfx::field_s(pReflection, sName.key, sName, sName));
            input_container.add_control(pPicture);
            ++m_stInputCount;
        }

        EMP_RET_FALSE_IF_NOT(input_property.populate());

        for (size_t stIndex = 0; stIndex < stOutputCount; ++stIndex)
        {
            icon_c* pIcon = nullptr;
            if (stIndex < output_icons.count())
            { pIcon = output_icons.val(stIndex); }
            else
            {
                // @@0  lib_str allocation
                pIcon = EMP_ALLOCATE(icon_c);
                output_icons.add(pIcon);

                // @@0 create pIcon->create();
                pIcon->splitter.orientation_(dock_e::Right);
                pIcon->state().dock_(dock_e::Top).autosize.y_(true);
                pIcon->picture.state().focusable_(true);
                pIcon->skin_(skin ? skin->pout_icon : nullptr);
                // @@0  lib_str allocation
                pIcon->label.set_text(STR_U8FORMAT("output #0",stIndex));
                pIcon->picture_(skin ? skin->gfx_pout : nullptr);
            }

            pIcon->state().visible_(a_pValue->user_result(stIndex));
            output_docker.add_control(pIcon);
            ++m_stOutputCount;
        }
    }

    m_pFunction->center();
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::set_registrar(registrar_i* const a_pValue)
{
    input_property.registrar_(a_pValue);
    property.registrar_(a_pValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::set_synth(emp::snd::soft_synth_c* const a_pValue)
{ synth = a_pValue; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::set_skin(cpc_graph_skin_s a_pValue)
{
    header.skin_(a_pValue ? a_pValue->header : nullptr);
    output.skin_(a_pValue? a_pValue->header_picture : nullptr);
    remove.skin_(a_pValue? a_pValue->remove_picture : nullptr);
    remove.model_(a_pValue ? a_pValue->gfx_remove : nullptr);

    property.skin_(a_pValue ? a_pValue->property : nullptr);

    docker.skin_(a_pValue ? a_pValue->docker : nullptr);
    main.skin_(a_pValue ? a_pValue->main : nullptr);
    button.skin_(a_pValue ? a_pValue->button : nullptr);
    // @@0 spacer spacer_0.skin_(a_pValue ? a_pValue->spacer : nullptr);
    // @@0 spacer spacer_1.skin_(a_pValue ? a_pValue->spacer : nullptr);
    input_property.skin_(a_pValue ? a_pValue->property : nullptr);
    input_docker.skin_(a_pValue ? a_pValue->input_docker : nullptr);
    // @@0 spacer input_spacer.skin_(a_pValue ? a_pValue->spacer : nullptr);
    output_docker.skin_(a_pValue ? a_pValue->input_docker : nullptr);
    input_container.skin_(a_pValue ? a_pValue->pin_container : nullptr);

    { c_size stCount = input_pictures.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        picture_c* const pPicture = input_pictures.val(stIndex);
        pPicture->skin_(a_pValue ? a_pValue->pin_picture :nullptr);
        pPicture->model_(a_pValue ? a_pValue->gfx_pin : nullptr);
    } }

    { c_size stCount = output_icons.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        icon_c* const pIcon = output_icons.val(stIndex);
        pIcon->skin_(a_pValue ? a_pValue->pout_icon : nullptr);
        pIcon->picture_(a_pValue ? a_pValue->gfx_pout: nullptr);
    } }

    skin = a_pValue;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::destroy(void)
{
    // @@0 create output.destroy();
 // @@0 create   property.destroy();

    if (reflection != nullptr)
    { EMP_DEALLOCATE(reflection); }
    reflection = nullptr;

    m_pFunction = nullptr;

   // @@0 create  input_property.destroy();

    emp::cnt::foreach(output_icons, emp::mem::safe_delete_reset_t());
    output_icons.resize(0);

    emp::cnt::foreach(input_pictures, emp::mem::safe_delete_reset_t());
    input_pictures.resize(0);

    emp::cnt::foreach(reflections, emp::mem::safe_delete_reset_t());
    reflections.resize(0);

    on_function_removed = fct_function_removed_d();

    synth = nullptr;

    m_stInputCount = 0;
    m_stOutputCount = 0;

    designer.destroy();

    EMP_TT_MAX_VAR(function);

    skin = nullptr;

    m_vfValues.resize(0);
    m_vsValues.resize(0);

    on_expression_changed = fct_expression_changed_d();
    expression = astring();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::update(emp::tim::clock_c const& a_rClock)
{
    EMP_RET_FALSE_IF_NOT(tbase1::update(a_rClock));
    m_pFunction->update(synth, function);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::update_info(info_s& a_rInfo) const
{
    c_v2f v2InvRatio = EMP_GUI_SKIN.resolution.inv_ratio();
    c_v2f v2Position = v2f::mul(transformation().position, v2InvRatio);
    c_v2f v2Size = v2f::mul(transformation().size, v2InvRatio);

    a_rInfo.position_(v2Position).size_(v2Size).show_output_(output.state().check);
    m_pFunction->update_info(a_rInfo);
    if (m_pFunction->control()->state().visible)
    { a_rInfo.function_size_(m_pFunction->control()->transformation().size); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::set_info(c_info_s& a_rInfo)
{
    c_v2f v2Ratio = EMP_GUI_SKIN.resolution.ratio();
    c_v2f v2Position = v2f::mul(a_rInfo.position, v2Ratio);
    c_v2f v2Size = v2f::mul(a_rInfo.size, v2Ratio);

    transformation().position_(v2Position).size_(v2Size);

    output.state().check_(a_rInfo.show_output);
    m_pFunction->set_info(a_rInfo);

    m_pFunction->control()->transformation().size_(a_rInfo.function_size);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::resize(void)
{
    if (state().visible == false)
    { return true; }

    input_container.transformation().size_(0);
    input_spacer.transformation().size_(0);
    if (input_docker.state().visible)
    {
        float fWidth = 0;
        float fHeight = 0;
        c_size stCount = m_stInputCount;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            picture_c* const pPicture = input_pictures.val(stIndex);
            line_c const* const pLine = input_property.lines().val(stIndex);
            pPicture->transformation().height_(pLine->transformation().size.y);
            pPicture->transformation().y_(pLine->transformation().y());
            EMP_RET_FALSE_IF_NOT(pPicture->resize());
            fWidth = emp::tt::maxof(fWidth, pPicture->transformation().width());
            fHeight += pPicture->transformation().height();
        }
        input_container.transformation().size_(fWidth, fHeight);
    }

    update_visibility();

    return tbase1::resize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::contains(cpc_control_i a_pButton, ui32& a_rui32Pin, ui32& a_rui32Pout) const
{
    EMP_TT_MAX_VAR(a_rui32Pin);
    EMP_TT_MAX_VAR(a_rui32Pout);

    {
        c_size stCount = m_stOutputCount;
        for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
        {
            if (output_icons.val(ui32Index) == a_pButton)
            {
                a_rui32Pout = ui32Index;
                return;
            }
        }
    }

    {
        c_size const stCount = m_stInputCount;
        for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
        {
            if (input_pictures.val(ui32Index) == a_pButton)
            {
                a_rui32Pin = ui32Index;
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::pin_check(c_size a_stIndex, c_bool a_bValue)
{ input_pictures.val(a_stIndex)->state().check_(a_bValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::pout_check(c_size a_stIndex, c_bool a_bValue)
{ output_icons.val(a_stIndex)->picture.state().check_(a_bValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v3f node_c::get_pin_info(c_size a_stIn) const
{
    cpc_control_i pPicture = input_pictures.val(a_stIn);
    c_transformation_data_s& rTransform = pPicture->transformation();
    return v3f::all(gui_to_gl(rTransform.matrix, { 0, -rTransform.size.y * 0.5f }), depth(transformation().fore_depth, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN v3f node_c::get_pout_info(c_size a_stOut) const
{
    cpc_control_i pIcon = output_icons.val(a_stOut);
    c_transformation_data_s& rTransform = pIcon->transformation();
    return v3f::all(gui_to_gl(rTransform.matrix, { rTransform.size.x, -rTransform.size.y * 0.5f }), depth(transformation().back_depth, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN picture_c const* node_c::get_pin_button(c_size a_stIndex) const
{ return input_pictures.val(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN icon_c const* node_c::get_pout_button(c_size a_stIndex) const
{ return output_icons.val(a_stIndex); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::get_value(c_size a_stIndex, astring& a_rsValue)
{
    emp::snd::function_c const& rFunction = synth->get_function(function);

    emp::snd::link_c const& rLink = rFunction.inputs().ref(a_stIndex);
    if (emp::tt::is_max(rLink.function()))
    {
        a_rsValue = ALITERAL("unbound");
        return true;
    }
    else if (rLink.function() == function)
    {
        c_float fValue = m_vfValues.val(a_stIndex);
        c_float fNewValue = synth->get_value(function, a_stIndex);

        if (emp::tt::equal_(fValue, fNewValue) == false)
        {
            astring& sValue = m_vsValues.ref(a_stIndex);

            format.set_value(fNewValue);
            sValue.clear();
            sValue = STR_FORMATA(sValue, "#0", format);
            a_rsValue = sValue;
            m_vfValues.set(a_stIndex, fNewValue);
        }
        else
        { a_rsValue = m_vsValues.ref(a_stIndex); }
        return true;
    }
    else
    {
        a_rsValue = ALITERAL("bound");
        return true;
    }
// @@0 unreachable    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::get_function_value(astring& a_rsValue)
{
    a_rsValue = expression;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::set_function_value(c_astring& a_rsValue)
{
    expression = a_rsValue;
    if (on_expression_changed.is_valid())
    { on_expression_changed(a_rsValue); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::set_value(c_size a_stIndex, c_astring& a_rsValue)
{
    float fValue = 0;
    c_bool bReturn = emp::str::to(a_rsValue, fValue);
    if (bReturn)
    {
        m_vsValues.set(a_stIndex, a_rsValue);
        return set_float_value(a_stIndex, fValue);
    }
    return bReturn;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::set_float_value(c_size a_stIndex, c_float a_fValue)
{
    emp::snd::function_c const& rFunction = synth->get_function(function);

    emp::snd::link_c const& rLink = rFunction.inputs().ref(a_stIndex);
    if (emp::tt::is_not_max(rLink.function()) &&
        rLink.function() == function)
    {
        synth->set_value(function, a_stIndex, a_fValue);
        m_vfValues.set(a_stIndex, a_fValue);
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void node_c::default_value(c_ui32 a_ui32Index, c_float a_fValue)
{
    if (function < synth->function_count())
    {
        emp::snd::function_c const& rFunction = synth->get_function(function);
        if (a_ui32Index < rFunction.inputs().count())
        {
            synth->link(function, a_ui32Index, function, a_ui32Index, true);
            EMP_ASSERT_BOOL_CALL(set_float_value(a_ui32Index, a_fValue));
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool node_c::get_enable(c_size a_stIndex)
{
    emp::snd::function_c const& rFunction = synth->get_function(function);

    emp::snd::link_c const& rLink = rFunction.inputs().ref(a_stIndex);
    return rLink.function() == function;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e node_c::remove_node(cpc_control_i, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        synth->remove_function(function);
        if (on_function_removed.is_valid())
        { on_function_removed(function); }
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e node_c::show_output(cpc_control_i, c_bool a_bValue, c_change_e)
{
    if (a_bValue)
    {
        EMP_LOG_INFO("SHOW_OUTPUT");
        c_bool bNewCheck = output.state().check == false;
        output.state().check_(bNewCheck);
        update_visibility();
        EMP_RET_X_IF_NOT(return_e::Error, resize_function());
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
graph_c::graph_c(void):
    popup(),
    m_vpNodes(),
    m_vpNodeFunctions(),
    m_ui32CurrentFunction(),
    m_ui32CurrentPin(),
    m_ui32CurrentPout(),
    m_v2CurrentPosition(),
    m_stLines(),
    m_vpImNurbs(),
    m_pImNurbs(),
    m_pRegistrar(),
    synth(),
    skin(nullptr)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
graph_c::~graph_c(void)
{ destroy(); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::create(registrar_i* const a_pRegistrar)
{
    destroy();

    m_pRegistrar = a_pRegistrar;

    popup.create(m_pRegistrar, this);
    popup.add_item(ALITERAL("emp::gui::graph_c.new_function"), function(this, &graph_c::on_new_function));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_scale"   ), function(this, &graph_c::on_new_scale));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_envelope"), function(this, &graph_c::on_new_envelope));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_spectrum"), function(this, &graph_c::on_new_spectrum));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_biquad"  ), function(this, &graph_c::on_new_biquad));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_delay"   ), function(this, &graph_c::on_new_delay));
    popup.add_item(ALITERAL("emp::gui::graph_c.new_reverb"  ), function(this, &graph_c::on_new_reverb));

    m_pImNurbs->create();
    m_pImNurbs->set_iteration(100);
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());
    m_pImNurbs->add_point(v3f::zero());

    scroller.state().overlap_(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::destroy(void)
{
    scroller.clear_controls();

    emp::cnt::foreach(m_vpImNurbs, emp::mem::safe_delete_reset_t());
    m_vpImNurbs.resize(0);

    m_pImNurbs->destroy();

    popup.destroy();

    m_pRegistrar = nullptr;

    skin = nullptr;

    set_synth(nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void update_nurbs(gfx::gl_nurbs_c& a_rNurbs, c_v3f& a_v3In, c_v3f& a_v3Out,
    c_bool a_bLine, c_float a_fRatioX, c_float EMP_SILENT(a_fRatioY));
EMP_INTERN void update_nurbs(gfx::gl_nurbs_c& a_rNurbs, c_v3f& a_v3In, c_v3f& a_v3Out,
    c_bool a_bLine, c_float a_fRatioX, c_float EMP_SILENT(a_fRatioY))
{
    c_float fInX = a_v3In.x;
    c_float fInY = -a_v3In.y;

    c_float fOutX = a_v3Out.x;
    c_float fOutY = -a_v3Out.y;

    c_float fDX = (fOutX - fInX);
    c_float fDY = (fOutY - fInY);

    c_float fDelta = -0.08f * a_fRatioX;

    c_float f1X = 0.2f;// / a_fRatioX;
    c_float f2X = 0.4f;// / a_fRatioX;
    c_float f3X = 0.5f;

    c_float f1Y = 0.01f;// / a_fRatioY;
    c_float f2Y = 0.2f;// / a_fRatioY;
    c_float f3Y = 0.5f;

    v2f v0;
    v2f v1;
    v2f v2;
    v2f v3;
    v2f v4;
    v2f v5;
    v2f v6;
    v2f v7;

    if (a_bLine)
    {
        c_float fSX = fDX * 0.5f / 3.0f;
        c_float fSY = fDY * 0.5f / 3.0f;

        v0 = { fInX + 0 * fSX, fInY + 0 * fSY };
        v1 = { fInX + 1 * fSX, fInY + 1 * fSY };
        v2 = { fInX + 2 * fSX, fInY + 2 * fSY };
        v3 = { fInX + 3 * fSX, fInY + 3 * fSY };

        v4 = { fOutX - 3 * fSX, fOutY - 3 * fSY };
        v5 = { fOutX - 2 * fSX, fOutY - 2 * fSY };
        v6 = { fOutX - 1 * fSX, fOutY - 1 * fSY };
        v7 = { fOutX - 0 * fSX, fOutY - 0 * fSY };
    }
    else if (fDX < fDelta)
    {
        c_float f1DX = f1X * fDX;
        c_float f1DY = f1Y * fDY;

        c_float f2DX = f2X * fDX;
        c_float f2DY = f2Y * fDY;

        c_float f3DX = f3X * fDX;
        c_float f3DY = f3Y * fDY;

        v0 = { fInX, fInY };
        v1 = { fInX + f1DX, fInY + f1DY };
        v2 = { fInX + f2DX, fInY + f2DY };
        v3 = { fInX + f3DX, fInY + f3DY };

        v4 = { fOutX - f3DX, fOutY - f3DY };
        v5 = { fOutX - f2DX, fOutY - f2DY };
        v6 = { fOutX - f1DX, fOutY - f1DY };
        v7 = { fOutX, fOutY };
    }
    else if (fDX < 0.0f)
    {
        c_float fOffX = fDelta;
        c_float f1DX = f1X * fOffX;
        c_float f1DY = f1Y * fDY;

        c_float f2DX = f2X * fOffX;
        c_float f2DY = f2Y * fDY;

        c_float f3DX = f3X * fDX;
        c_float f3DY = f3Y * fDY;

        v0 = { fInX, fInY };
        v1 = { fInX + f1DX, fInY + f1DY };
        v2 = { fInX + f2DX, fInY + f2DY };
        v3 = { fInX + f3DX, fInY + f3DY };

        v4 = { fOutX - f3DX, fOutY - f3DY };
        v5 = { fOutX - f2DX, fOutY - f2DY };
        v6 = { fOutX - f1DX, fOutY - f1DY };
        v7 = { fOutX, fOutY };
    }
    else
    {
        c_float fOffX = -fDelta;
        c_float f1DX = f1X * fOffX;
        c_float f1DY = f1Y * fDY;

        c_float f2DX = f2X * fOffX;
        c_float f2DY = f2Y * fDY;

        c_float f3DX = f3X * fDX;
        c_float f3DY = f3Y * fDY;

        v0 = { fInX, fInY };
        v1 = { fInX - f1DX, fInY + f1DY };
        v2 = { fInX - f2DX, fInY + f2DY };
        v3 = { fInX + f3DX, fInY + f3DY };

        v4 = { fOutX - f3DX, fOutY - f3DY };
        v5 = { fOutX + f2DX, fOutY - f2DY };
        v6 = { fOutX + f1DX, fOutY - f1DY };
        v7 = { fOutX, fOutY };
    }


    c_float fDeltaDepth = a_v3Out.z - a_v3In.z;

    a_rNurbs.set_point(0, v3f::all(v2f::mul(v0, {1, -1}), a_v3In.z + fDeltaDepth * 0.0f / 7.0f));
    a_rNurbs.set_point(1, v3f::all(v2f::mul(v1, {1, -1}), a_v3In.z + fDeltaDepth * 1.0f / 7.0f));
    a_rNurbs.set_point(2, v3f::all(v2f::mul(v2, {1, -1}), a_v3In.z + fDeltaDepth * 2.0f / 7.0f));
    a_rNurbs.set_point(3, v3f::all(v2f::mul(v3, {1, -1}), a_v3In.z + fDeltaDepth * 3.0f / 7.0f));
    a_rNurbs.set_point(4, v3f::all(v2f::mul(v4, {1, -1}), a_v3In.z + fDeltaDepth * 4.0f / 7.0f));
    a_rNurbs.set_point(5, v3f::all(v2f::mul(v5, {1, -1}), a_v3In.z + fDeltaDepth * 5.0f / 7.0f));
    a_rNurbs.set_point(6, v3f::all(v2f::mul(v6, {1, -1}), a_v3In.z + fDeltaDepth * 6.0f / 7.0f));
    a_rNurbs.set_point(7, v3f::all(v2f::mul(v7, {1, -1}), a_v3In.z + fDeltaDepth * 7.0f / 7.0f));

    a_rNurbs.update();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN node_c* get_node(emp::cnt::vector<node_c*> const& a_rvpNodes, c_ui32 a_ui32Function);
EMP_INTERN EMP_RETURN node_c* get_node(emp::cnt::vector<node_c*> const& a_rvpNodes, c_ui32 a_ui32Function)
{
    c_size stCount = a_rvpNodes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        node_c* const pNode = a_rvpNodes.val(stIndex);
        if (pNode->function == a_ui32Function)
        { return pNode; }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool graph_c::transform(stack_c& a_rStack)
{
    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    m_stLines = 0;

    c_m44f& rMatrix = transformation().matrix;
    c_v4f v4InColor = (skin && skin->pin_line) ? skin->pin_line->rgba : v4f::all(0);
    c_v4f v4OutColor = (skin && skin->pout_line) ? skin->pout_line->rgba : v4f::all(0);

    EMP_RET_FALSE_IF(synth == nullptr);

    { c_size stCount = m_vpNodes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        node_c* const pNode = m_vpNodes.val(stIndex);
        info_s& rInfo = synth->get_info(stIndex);

        pNode->update_info(rInfo);
        pNode->set_info(rInfo);

        for (size_t stPin = 0; stPin < pNode->input_count(); ++stPin)
        { pNode->pin_check(stPin, false); }
        for (size_t stPout = 0; stPout < pNode->output_count(); ++stPout)
        { pNode->pout_check(stPout, false); }
    } }

    // @@0  reorder nodes
    { c_size stCount = synth->function_count();
    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        emp::snd::function_c const& rFunction = synth->get_function(ui32Index);

        node_c* const pNode = get_node(m_vpNodes, ui32Index);

        c_size stInCount = rFunction.inputs().count();
        for (ui32  ui32In = 0; ui32In < stInCount; ++ui32In)
        {
            emp::snd::link_c const& rLink = rFunction.inputs().ref(ui32In);
            c_ui32 ui32OutFunction = rLink.function();
            c_ui32 ui32Out = rLink.index();

            if (emp::tt::is_max(ui32OutFunction))
            { continue; }

            pNode->pin_check(ui32In, true);

            if (ui32OutFunction == ui32Index)
            { continue; }

            node_c* const pPoutNode = get_node(m_vpNodes, ui32OutFunction);
            pPoutNode->pout_check(ui32Out, true);

            c_v3f v3Pin = pNode->get_pin_info(ui32In);
            c_v3f v3PinPosition = v3f::all(gl_to_gui(v3f::xy(v3Pin), rMatrix), v3Pin.z);

            c_v3f v3Pout = pPoutNode->get_pout_info(ui32Out);
            c_v3f v3PoutPosition = v3f::all(gl_to_gui(v3f::xy(v3Pout), rMatrix), v3Pout.z);

            emp::gfx::gl_nurbs_c* pNurbs = nullptr;
            if (m_stLines < m_vpImNurbs.count())
            { pNurbs = m_vpImNurbs.val(m_stLines); }
            else
            {
                pNurbs = EMP_ALLOCATE(emp::gfx::gl_nurbs_c);
                pNurbs->create();
                // @@0 fixme emp::gfx::material_c const* const pMaterial = skin->pin_line()->pointer();
                // pNurbs->set_material(pMaterial);
                // pNurbs->set_color(emp::mat::v4f::one());
                pNurbs->set_in_color(v4InColor);
                pNurbs->set_out_color(v4OutColor);
                pNurbs->set_iteration(100);
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                pNurbs->add_point(v3f::zero());
                m_vpImNurbs.add(pNurbs);
            }

            ++m_stLines;
            update_nurbs(*pNurbs, v3PinPosition, v3PoutPosition, false,
                EMP_GUI_SKIN.resolution.ratio().x,
                EMP_GUI_SKIN.resolution.ratio().y);
        }
    } }

    if (emp::tt::is_not_max(m_ui32CurrentFunction))
    {
        m_pImNurbs->set_in_color(v4InColor);
        m_pImNurbs->set_out_color(v4OutColor);

        c_float fDepth = depth(transformation().fore_depth, 0);

        if (emp::tt::is_not_max(m_ui32CurrentPin))
        {
            EMP_ASSERT(emp::tt::is_max(m_ui32CurrentPout));
            node_c const* const pNode = get_node(m_vpNodes, m_ui32CurrentFunction);
            c_v3f v3Pin = pNode->get_pin_info(m_ui32CurrentPin);
            update_nurbs(*m_pImNurbs, v3f::all(gl_to_gui(v3f::xy(v3Pin), rMatrix), v3Pin.z),
                v3f::all(m_v2CurrentPosition, fDepth), true, EMP_GUI_SKIN.resolution.ratio().x, EMP_GUI_SKIN.resolution.ratio().y);
        }
        else if (emp::tt::is_not_max(m_ui32CurrentPout))
        {
            EMP_ASSERT(emp::tt::is_max(m_ui32CurrentPin));
            node_c const* const pNode = get_node(m_vpNodes, m_ui32CurrentFunction);
            c_v3f v3Pout = pNode->get_pout_info(m_ui32CurrentPout);
            update_nurbs(*m_pImNurbs, v3f::all(m_v2CurrentPosition, fDepth), v3f::all(gl_to_gui(v3f::xy(v3Pout), rMatrix), v3Pout.z),
                true, EMP_GUI_SKIN.resolution.ratio().x, EMP_GUI_SKIN.resolution.ratio().y);
        }
        else
        { EMP_ASSERT_UNREACHABLE(); }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool graph_c::render(void)
{
    if (state().visible == false ||
        state().renderable == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::render());

    c_m44f& rMatrix = transformation().matrix;

    {
        c_size stCount = m_stLines;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { m_vpImNurbs.val(stIndex)->render(rMatrix); }
    }

    if (emp::tt::is_not_max(m_ui32CurrentFunction))
    { m_pImNurbs->render(rMatrix); }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false)
    { return return_e::Unused; }

    EMP_GUI_RETURN_IF_USED(tbase1::process_mouse(a_rEvent));

    auto const& rMouse = a_rEvent.mouse;
    auto const& rKeyboard = a_rEvent.keyboard;

    emp::app::c_button_e eButton = rMouse.m_eButton;

    c_bool bPressed = a_rEvent.mouse_pressed();
    c_bool bReleased = a_rEvent.mouse_released();

    if (state().hover &&
        rKeyboard.modified() == false &&
        eButton == emp::app::button_e::Left)
    {
        c_size stCount = m_vpNodes.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            node_c const* const pNode = m_vpNodes.val(stIndex);

            c_size stPinCount = pNode->input_count();
            for (size_t stPin = 0; stPin < stPinCount; ++stPin)
            {
                cpc_control_i pPinButton = pNode->get_pin_button(stPin);
                if (pPinButton->state().hover)
                { return on_pinout_push(pPinButton, bPressed, bReleased); }
            }

            c_size stPoutCount = pNode->output_count();
            for (size_t stPout = 0; stPout < stPoutCount; ++stPout)
            {
                cpc_control_i pPoutButton = pNode->get_pout_button(stPout);
                if (pPoutButton->state().hover)
                { return on_pinout_push(pPoutButton, bPressed, bReleased); }
            }
        }
    }

    if (bReleased &&
        eButton == emp::app::button_e::Left)
    {
        EMP_TT_MAX_VAR(m_ui32CurrentFunction);
        EMP_TT_MAX_VAR(m_ui32CurrentPin);
        EMP_TT_MAX_VAR(m_ui32CurrentPout);
    }

    if (state().hover)
    {
        m_v2CurrentPosition = gl_to_gui(rMouse.m_v2fPosition, transformation().matrix);

        if (bPressed &&
            rKeyboard.modified() == false &&
            eButton == emp::app::button_e::Right)
        {
            popup.show(m_v2CurrentPosition, change_e::Mouse);
        }
    }

    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* graph_c::add_function(c_astring& a_sExpression, c_info_s& a_rInfo)
{
    c_ui32 ui32Function = synth->add_function();
    synth->set_function_expression(ui32Function, a_sExpression);
    synth->get_info(ui32Function) = a_rInfo;
    return add_node(ui32Function, new_node_function(ui32Function), true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* graph_c::add_envelope(c_info_s& a_rInfo)
{
    c_ui32 ui32Function = synth->add_function();
    synth->set_function_expression(ui32Function, ALITERAL("envelope()"));
    synth->get_info(ui32Function) = a_rInfo;
    return add_node(ui32Function, new_envelope_function(ui32Function), true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* graph_c::add_spectrum(c_info_s& a_rInfo)
{
    c_ui32 ui32Function = synth->add_function();
    synth->set_function_expression(ui32Function, ALITERAL("spectrum(time,signal)"));
    synth->get_info(ui32Function) = a_rInfo;
    synth->get_info(ui32Function).show_output_(true).xlog_(true);
    return add_node(ui32Function, new_spectrum_function(ui32Function), true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* graph_c::add_biquad(c_info_s& a_rInfo)
{
    c_ui32 ui32Function = synth->add_function();
    synth->set_function_expression(ui32Function, ALITERAL("biquad(time,signal,110,1,0)"));
    synth->get_info(ui32Function) = a_rInfo;
    synth->get_info(ui32Function).show_output_(true).xlog_(true);
    return add_node(ui32Function, new_biquad_function(ui32Function), true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_function_i* graph_c::new_node_function(c_ui32 EMP_SILENT(a_ui32Function))
{
    node_function_c* const pNodeFunction = EMP_ALLOCATE(node_function_c);
    pNodeFunction->create();
    pNodeFunction->set_skin(skin);
    return pNodeFunction;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_function_i* graph_c::new_envelope_function(c_ui32 EMP_SILENT(a_ui32Function))
{
    envelope_function_c* const pNurbsFunction = EMP_ALLOCATE(envelope_function_c);
    pNurbsFunction->create();
    return pNurbsFunction;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_function_i* graph_c::new_spectrum_function(c_ui32 EMP_SILENT(a_ui32Function))
{
    node_function_c* const pSpectrumFunction = EMP_ALLOCATE(node_function_c);
    pSpectrumFunction->create();
    return pSpectrumFunction;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_function_i* graph_c::new_biquad_function(c_ui32 EMP_SILENT(a_ui32Function))
{
    node_function_c* const pBiquadFunction = EMP_ALLOCATE(node_function_c);
    pBiquadFunction->create();
    return pBiquadFunction;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* graph_c::add_node(c_ui32 a_ui32Function, node_function_i* const a_pNodeFunction, c_bool a_bNew)
{
    // @@0  lib_str allocation
    node_c* const pNode = EMP_ALLOCATE(node_c);

    m_vpNodeFunctions.add(a_pNodeFunction);

    EMP_RET_NULL_IF_NOT(pNode->create());
    pNode->set_registrar(m_pRegistrar);
    pNode->set_synth(synth);
    pNode->set_skin(skin);
    pNode->state().focusable_(true).navigable_(true).index_(emp::ast::cast<ui32>(m_vpNodes.count()));
    pNode->function_(a_ui32Function);
    pNode->expression_(synth->get_function_expression(a_ui32Function));
    pNode->set_node_function(a_pNodeFunction);
    pNode->set_info(synth->get_info(a_ui32Function));
    pNode->on_function_removed = function(this, &graph_c::function_removed);
    pNode->on_expression_changed = function(pNode, &node_c::expression_changed);
    m_vpNodes.add(pNode);

    EMP_RET_NULL_IF_NOT(pNode->pinout_changed(synth->function(a_ui32Function), a_bNew));

    pNode->update_visibility();
    EMP_RET_NULL_IF_NOT(pNode->resize_function());

    scroller.add_control(pNode);

    return pNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//void graph_c::set_full(c_bool a_bValue)
//{ m_pMdi->scroller().full_(a_bValue); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::set_skin(cpc_graph_skin_s a_pValue)
{
    skin = a_pValue;
    popup.skin_(a_pValue ? a_pValue->toolbar : nullptr);
    tbase1::skin_(a_pValue ? a_pValue->panel : nullptr);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::set_synth(emp::snd::soft_synth_c* const a_pValue)
{
    emp::cnt::foreach(m_vpNodes, emp::mem::safe_delete_reset_t());
    m_vpNodes.resize(0);

    emp::cnt::foreach(m_vpNodeFunctions, emp::mem::safe_delete_reset_t());
    m_vpNodeFunctions.resize(0);

    EMP_TT_MAX_VAR(m_ui32CurrentFunction);
    EMP_TT_MAX_VAR(m_ui32CurrentPin);
    EMP_TT_MAX_VAR(m_ui32CurrentPout);

    m_v2CurrentPosition = emp::mat::v2f::zero();
    m_stLines = 0;

    synth = a_pValue;

    if (synth != nullptr)
    {
        c_size stCount = synth->function_count();
        for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
        {
            emp::mat::function_i<precision_d>* pFunction = synth->function(ui32Index);
            emp::mat::c_operation_e eOperation = pFunction->operation();

            node_function_i* pNodeFunction = nullptr;

            if (eOperation == static_cast<emp::mat::operation_e>(emp::mat::sound_operation_e::Envelope))
            { pNodeFunction = new_envelope_function(ui32Index); }
            else if (eOperation == static_cast<emp::mat::operation_e>(emp::mat::sound_operation_e::Spectrum))
            { pNodeFunction = new_spectrum_function(ui32Index); }
            else if (eOperation == static_cast<emp::mat::operation_e>(emp::mat::sound_operation_e::Biquad))
            { pNodeFunction = new_biquad_function(ui32Index); }
            else
            { pNodeFunction = new_node_function(ui32Index); }
            EMP_ASSERT_BOOL_CALL(add_node(ui32Index, pNodeFunction, false) != nullptr);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::set_current_frequency(float const a_fValue)
{
    c_size stCount = m_vpNodeFunctions.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { m_vpNodeFunctions.ref(stIndex)->set_current_frequency(a_fValue); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN info_s graph_c::get_info(void)
{
    popup.hide();

    c_v2f v2PopupPosition = popup.position();

    c_float fX = v2PopupPosition.x;
    c_float fY = v2PopupPosition.y;

    c_v2f v2Position = v2f::mul({ fX, transformation().height() + fY }, EMP_GUI_SKIN.resolution.inv_ratio());

    c_v2f v2Size = { 0.4f, 0.5f };

    info_s oInfo;
    return oInfo.position_(v2Position).size_(v2Size).scale_(1).xscale_(1).yscale_(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_function(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_function(astring(), get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_scale(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_function(ALITERAL("time*scale(440, 12, 2, index)"), get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_envelope(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_envelope(get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_spectrum(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_spectrum(get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_biquad(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_biquad(get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_delay(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_function(ALITERAL("delay(signal, false, 100, 0.38)"), get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_new_reverb(c_size, c_bool a_bValue, c_change_e EMP_SILENT(a_eChange))
{
    if (a_bValue)
    {
        EMP_ASSERT_BOOL_CALL(add_function(ALITERAL("reverb(signal, false, 12, 100, 0.38)"), get_info()) != nullptr);
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN void get_function(cpc_control_i a_pSender, emp::cnt::vector<node_c*> const& a_rvpNodes,
    ui32& a_rui32Function, ui32& a_rui32Pin, ui32& a_rui32Pout);
EMP_INTERN void get_function(cpc_control_i a_pSender, emp::cnt::vector<node_c*> const& a_rvpNodes,
    ui32& a_rui32Function, ui32& a_rui32Pin, ui32& a_rui32Pout)
{
    EMP_TT_MAX_VAR(a_rui32Function);
    EMP_TT_MAX_VAR(a_rui32Pin);
    EMP_TT_MAX_VAR(a_rui32Pout);

    c_size stCount = a_rvpNodes.count();
    for (ui32 ui32Index = 0; ui32Index < stCount; ++ui32Index)
    {
        node_c const* const pNode = a_rvpNodes.val(ui32Index);
        ui32 EMP_TT_MAX_VAR(ui32Pin);
        ui32 EMP_TT_MAX_VAR(ui32Pout);
        pNode->contains(a_pSender, ui32Pin, ui32Pout);
        if (emp::tt::is_not_max(ui32Pin) ||
            emp::tt::is_not_max(ui32Pout))
        {
            a_rui32Function = pNode->function;
            a_rui32Pin = ui32Pin;
            a_rui32Pout = ui32Pout;
            return;
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN return_e graph_c::on_pinout_push(cpc_control_i a_pSender, c_bool a_bPressed, c_bool a_bReleased)
{
    ui32 EMP_TT_MAX_VAR(ui32Pin);
    ui32 EMP_TT_MAX_VAR(ui32Pout);
    ui32 EMP_TT_MAX_VAR(ui32Function);

    get_function(a_pSender, m_vpNodes, ui32Function, ui32Pin, ui32Pout);

    if (emp::tt::is_max(ui32Function))
    {
        EMP_TT_MAX_VAR(m_ui32CurrentFunction);
        EMP_TT_MAX_VAR(m_ui32CurrentPin);
        EMP_TT_MAX_VAR(m_ui32CurrentPout);
        EMP_ASSERT(emp::tt::is_max(ui32Pin) && emp::tt::is_max(ui32Pout));
        return return_e::Unused;
    }

    EMP_ASSERT(emp::tt::is_max(ui32Pin) != emp::tt::is_max(ui32Pout));

    if (a_bPressed)
    {
        if (emp::tt::is_not_max(ui32Pin))
        {
            emp::snd::link_c const& rLink = synth->get_function(ui32Function).inputs().ref(ui32Pin);

            c_ui32 ui32OutFunction = rLink.function();
            c_ui32 ui32Out = rLink.index();

            if (emp::tt::is_not_max(ui32OutFunction))
            {
                synth->unlink(ui32Function, ui32Pin, true);
                if (ui32OutFunction != ui32Function)
                {
                    m_ui32CurrentFunction = ui32OutFunction;
                    m_ui32CurrentPout = ui32Out;
                    EMP_TT_MAX_VAR(m_ui32CurrentPin);
                    return EMP_GUI_RETURNE(return_e::Consumed);
                }
           }
        }
        m_ui32CurrentFunction = ui32Function;
        m_ui32CurrentPin = ui32Pin;
        m_ui32CurrentPout = ui32Pout;
        return EMP_GUI_RETURNE(return_e::Consumed);
    }
    else if (a_bReleased && emp::tt::is_not_max(m_ui32CurrentFunction))
    {
        if (ui32Function == m_ui32CurrentFunction &&
            ui32Pin == m_ui32CurrentPin &&
            emp::tt::is_not_max(ui32Pin))
        {
            synth->link(ui32Function, ui32Pin, ui32Function, ui32Pin, true);

            EMP_TT_MAX_VAR(m_ui32CurrentFunction);
            EMP_TT_MAX_VAR(m_ui32CurrentPin);
            EMP_TT_MAX_VAR(m_ui32CurrentPout);
            return EMP_GUI_RETURNE(return_e::Consumed);
        }
        else if (ui32Function != m_ui32CurrentFunction)
        {
            if (emp::tt::is_not_max(m_ui32CurrentPin) &&
                emp::tt::is_not_max(ui32Pout))
            {
                synth->link(m_ui32CurrentFunction, m_ui32CurrentPin, ui32Function, ui32Pout, true);

                EMP_TT_MAX_VAR(m_ui32CurrentFunction);
                EMP_TT_MAX_VAR(m_ui32CurrentPin);
                EMP_TT_MAX_VAR(m_ui32CurrentPout);
                return EMP_GUI_RETURNE(return_e::Consumed);
            }
            else if (emp::tt::is_not_max(m_ui32CurrentPout) &&
                     emp::tt::is_not_max(ui32Pin))
            {
                synth->link(ui32Function, ui32Pin, m_ui32CurrentFunction, m_ui32CurrentPout, true);

                EMP_TT_MAX_VAR(m_ui32CurrentFunction);
                EMP_TT_MAX_VAR(m_ui32CurrentPin);
                EMP_TT_MAX_VAR(m_ui32CurrentPout);
                return EMP_GUI_RETURNE(return_e::Consumed);
            }
        }
    }

    EMP_TT_MAX_VAR(m_ui32CurrentFunction);
    EMP_TT_MAX_VAR(m_ui32CurrentPin);
    EMP_TT_MAX_VAR(m_ui32CurrentPout);
    return return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void graph_c::function_removed(c_ui32 a_ui32Function)
{
    for (size_t stIndex = 0; stIndex < m_vpNodes.count();)
    {
        node_c* const pNode = m_vpNodes.val(stIndex);
        c_ui32 ui32Function = pNode->function;

        if (ui32Function == a_ui32Function)
        {
            EMP_GUI_MANAGER->deactivate_focus();
            scroller.remove_control(pNode);
            EMP_DEALLOCATE(m_vpNodes.val(stIndex));
            EMP_DEALLOCATE(m_vpNodeFunctions.val(stIndex));
            m_vpNodes.remove(stIndex);
            m_vpNodeFunctions.remove(stIndex);
            continue;
        }

        if (ui32Function > a_ui32Function &&
            emp::tt::is_not_max(ui32Function))
        { pNode->function_(ui32Function - 1); }
        ++stIndex;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

