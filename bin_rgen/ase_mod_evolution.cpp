//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "ase_mod_evolution.h"

#include "ase_cor_game.h"
#include "ase_cor_selection.h"
#include "ase_cor_reflection.rfx.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_app_application.h"
#include "emp_app_event.h"

#include "emp_fct_function.h"

#include "emp_gfx_gl_device.h"

#include "emp_gui_label.h"
#include "emp_gui_slider.h"
#include "emp_gui_window.h"

#include "emp_rfx_factory.h"

#include "emp_ose_processor.h"
#include "emp_ose_program.h"
#include "emp_ose_renderer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace gui {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_PADDING_ADDED)
EMP_NOCOPY_BASE(class, evolution_c, public, docker_c)
public:
    EMP_FIELD(bool, profile, false);
    EMP_FIELD(float, min_scale, 0.1f);
    docker_c docker{};

private:
    emp::ose::best_s best{};

    spacer_c spacer_begin{};
    spacer_c spacer_end{};

    slider_c consumed{};
    button_c button_consumed{};
    spacer_c spacer_consumed{};
    
    slider_c left{};
    button_c button_left{};
    spacer_c spacer_left{};

    slider_c bank{};
    button_c button_bank{};
    spacer_c spacer_bank{};

    slider_c swipe{};
    button_c button_swipe{};
    spacer_c spacer_swipe{};

    slider_c process{};
    button_c button_process{};

    float m_fSwipe = 0;
    float m_fProcess = 0;
    emp::tim::time_c m_Stamp{};

    ui64 EMP_TT_MAX_VAR(m_uiTouchId);

    astring m_sMother{};
    astring m_sFather{};

    bool m_bRender = false;
    bool m_bChanged = false;
    bool m_bMutate = false;
    bool m_bSwitch = false;
    bool m_bLookup = false;
    bool m_bPng = false;
    bool m_bCleared = false;
    bool m_bNextFile = false;
    bool m_bDragLeft = false;
    bool m_bDragRight = false;
    bool m_bLeftPressed = false;
    bool m_bLeftReleased = false;
    bool m_bLeftDoubleClick = false;
    bool m_bRightPressed = false;
    bool m_bRightReleased = false;
    bool m_bRightDoubleClick = false;
    bool m_bTransitionEnded = false;
    bool m_bDragged = false;
    bool m_bLaunched = false;
    bool m_bFirstSign = false;
    bool m_bCancel = false;
    bool m_bDummy = true;
    bool m_bSaved = false;

    array<emp::ose::renderlet_c, emp::ose::renderer_c::sc_ui32SimuCount, true> m_aSimulated{};
    emp::ose::renderlet_c m_Rendered{};
    emp::ose::renderlet_c m_ToSave{};
    emp::ose::renderer_c* m_pRenderer = nullptr;

    size_t m_stCurrentSort = 0;
    array<vector<emp::ose::best_s>, 2, true> m_avBests{};
    vector<emp::ose::renderlet_c*> m_vpRenderlets{};

    size_t m_stNextTransition = 0;
    size_t m_stCurrentFile = 0;

    ase::cor::selection_option_s selection{};
    ase::cor::evolution_option_s evolution{};

    emp::mat::v2f m_v2DragOrigin{};
    emp::mat::v2f m_v2DragDelta{};

    ui32 EMP_TT_MAX_VAR(m_uiTarget);

    size_t m_stCurrentTarget =  0;
    float EMP_TT_MAX_VAR(m_fTransitionTime);

    float m_fTransitionDelta = 0;
    emp::mat::v2f m_v2fTransitionVelocity{};
    emp::mat::v2f m_v2fTransitionDirection{};
    float m_fTransitionDirection = 0;
    emp::tim::time_c m_DeltaTime{};

    vector<astring> m_vsTmpPool{};

public:
    //emp::fct::function_t<void(*)(c_astring& a_sFilename)> m_OnFileAdded;

private:
    EMP_XX_IF_PROFILE(EMP_POINTER(emp::prf::time_profiler_c) m_pProfiler;)

public:
    evolution_c(void);
    ~evolution_c(void);

public:
    EMP_RETURN bool create(emp::ose::renderer_c* const a_pRenderer);

public:
    void reset(c_bool a_bMain);

public:
    //>> control_i
    EMP_RETURN bool update(emp::tim::clock_c const& a_rClock);
    EMP_RETURN bool resize(void);
    EMP_RETURN bool transform(emp::gui::stack_c& a_rStack);
    EMP_RETURN bool render(void);
    //<< control_i

    //>> handler_i
    EMP_RETURN emp::gui::return_e process_keyboard(emp::app::c_event_s& a_rEvent);
    EMP_RETURN emp::gui::return_e process_mouse(emp::app::c_event_s& a_rEvent);
    //<< handler_i

public:
    EMP_RETURN size_t best_count(void) const { return m_avBests.ref(0).count(); }
    EMP_RETURN size_t best_max(void) const { return emp::ose::renderer_c::sc_ui32BestCount; }
    
public:
    // @@0 remove EMP_RETURN bool resize_buffers(c_f64 a_dWidth, c_f64 a_dHeight);
    void destroy_buffers(void);
    void set_file(c_astring& a_sFile);
};
EMP_PRAGMA_POP_IGNORE(EMP_W_PADDING_ADDED)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
evolution_c::evolution_c(void)
#if defined(EMP_XX_PROFILE)
 :
    m_pProfiler(ALITERAL("run"))
#endif
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
evolution_c::~evolution_c(void)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_PRAGMA_PUSH_IGNORE(EMP_W_DISABLED_OPTIMIZATION)
EMP_RETURN bool evolution_c::create(emp::ose::renderer_c* const a_pRenderer)
{
    m_stCurrentFile = 0;

    m_pRenderer = a_pRenderer;
    EMP_RET_FALSE_IF(m_pRenderer == nullptr);

    for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    {
        auto* pSimulated = m_aSimulated.ptr(stIndex);
        pSimulated->m_pImStack->create();
        m_vpRenderlets.add(pSimulated);
    }

    m_Rendered.m_pImStack->create();
    m_vpRenderlets.add(&m_Rendered);

    EMP_TT_MAX_VAR(m_fTransitionTime);

    docker.state().dock_(emp::gui::dock_e::Bottom).autosize.xy(false, true);
    docker.transformation().size_(2.0, 0);


    c_float fWidth = 0.38f;

    spacer_end.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_end);

    process.skin_(EMP_GUI_SKIN_(slider, "sdr.")).min_(0).max_(1).step_(0.1f).state().autosize.xy(false, true);
    process.transformation().size_(fWidth, 0);
    process.set_text(STR_U8FORMAT("#0", emp::tim::time_c()));
    button_process.skin_(EMP_GUI_SKIN_(button, "but.modal")).state().dock_(emp::gui::dock_e::Right).autosize.xy(false, true);
    //button_process->transformation().size_(0.2f, 0);
    button_process.control_(&process);
    docker.add_control(&button_process);

    spacer_swipe.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_swipe);

    swipe.skin_(EMP_GUI_SKIN_(slider, "sdr.")).min_(0).max_(1).step_(0.1f).state().autosize.xy(false, true);
    swipe.transformation().size_(fWidth, 0);
    swipe.set_text(u8string(U8LITERAL("#0%")));
    button_swipe.skin_(EMP_GUI_SKIN_(button, "but.modal")).state().dock_(emp::gui::dock_e::Right).autosize.xy(false, true);
    //button_swipe->transformation().size_(0.2f, 0);
    button_swipe.control_(&swipe);
    docker.add_control(&button_swipe);

    spacer_bank.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_bank);

    bank.skin_(EMP_GUI_SKIN_(slider, "sdr.")).min_(0).max_(1).step_(0.1f).state().autosize.xy(false, true);
    bank.transformation().size_(fWidth, 0);
    bank.set_text(STR_U8FORMAT("#0/#1", 0, best_max()));
    button_bank.skin_(EMP_GUI_SKIN_(button, "but.modal")).state().dock_(emp::gui::dock_e::Right).autosize.xy(false, true);
    //button_bank->transformation().size_(0.2f, 0);
    button_bank.control_(&bank);
    docker.add_control(&button_bank);

    spacer_left.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_left);

    left.skin_(EMP_GUI_SKIN_(slider, "sdr.")).min_(0).max_(1).step_(0.1f).state().autosize.xy(false, true);
    left.transformation().size_(fWidth, 0);
    left.set_text(STR_U8FORMAT("#0/#1", 0, best_max()));
    button_left.skin_(EMP_GUI_SKIN_(button, "but.modal")).state().dock_(emp::gui::dock_e::Right).autosize.xy(false, true);
    //button_bank->transformation().size_(0.2f, 0);
    button_left.control_(&left);
    docker.add_control(&button_left);

    spacer_consumed.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_consumed);

    consumed.skin_(EMP_GUI_SKIN_(slider, "sdr.")).min_(0).max_(1).step_(0.1f).state().autosize.xy(false, true);
    consumed.transformation().size_(fWidth, 0);
    consumed.set_text(STR_U8FORMAT("#0/#1", 0, best_max()));
    button_consumed.skin_(EMP_GUI_SKIN_(button, "but.modal")).state().dock_(emp::gui::dock_e::Right).autosize.xy(false, true);
    //button_bank->transformation().size_(0.2f, 0);
    button_consumed.control_(&consumed);
    docker.add_control(&button_consumed);

    spacer_begin.state().dock_(emp::gui::dock_e::Right);
    docker.add_control(&spacer_begin);

    return true;
}
EMP_PRAGMA_POP_IGNORE(EMP_W_DISABLED_OPTIMIZATION)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void evolution_c::reset(c_bool a_bMain)
{
    auto& rOptions = ASE_GAME.user().options;
    if (rOptions.evolution != evolution ||
        rOptions.selection != selection ||
        a_bMain)
    {
        evolution = rOptions.evolution;
        selection = rOptions.selection;
        auto& rCrossover = evolution.crossover;

        for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
        {
            auto& rSimulated = m_aSimulated.ref(stIndex);

            rSimulated.m_sMother = rCrossover.m_bXMother ? astring() : m_Rendered.m_sMother;
            if (evolution.crossover.m_bActive)
            { rSimulated.m_sFather = rCrossover.m_bXFather ? astring() : m_Rendered.m_sFather; }
            else
            { rSimulated.m_sFather = astring(); }

            if (rSimulated.m_pProgram != nullptr)
            {
                EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rSimulated.m_stProgram));
                rSimulated.m_pProgram = nullptr;
                EMP_TT_MAX_VAR(rSimulated.m_stProgram);
            }
            if (emp::tt::is_not_max(rSimulated.m_uiTarget0))
            {
                EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.unlock_framebuffer(rSimulated.m_uiTarget0));
                EMP_TT_MAX_VAR(rSimulated.m_uiTarget0);
            }
            if (emp::tt::is_not_max(rSimulated.m_uiTarget1))
            {
                EMP_ASSERT_BOOL_CALL(EMP_GRAPHICS.unlock_framebuffer(rSimulated.m_uiTarget1));
                EMP_TT_MAX_VAR(rSimulated.m_uiTarget1);
            }
        }

        {
            auto* const pToAdd = m_avBests.ptr(0);
            c_size stCount = pToAdd->count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                ose::best_s& rBest = pToAdd->ref(stIndex);
                if (rBest.m_pProgram != nullptr)
                {
                    EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rBest.m_stProgram));
                    rBest.m_pProgram = nullptr;
                    EMP_TT_MAX_VAR(rBest.m_stProgram);
                }
            }
            pToAdd->resize(0);
        }

        if (a_bMain)
        {
            m_bDummy = true;
            if (m_Rendered.m_pProgram != nullptr)
            {
                EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(m_Rendered.m_stProgram));
                m_Rendered.m_pProgram = nullptr;
                EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
            }

            m_Rendered.m_sMother = astring();
            m_Rendered.m_sFather = astring();
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::update(emp::tim::clock_c const& a_rClock)
{
    m_DeltaTime = a_rClock.delta();
    m_fTransitionDelta = emp::tt::minof(1 / 60.0f, emp::tim::time_c::to_seconds(m_DeltaTime));

    return tbase1::update(a_rClock);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::resize(void)
{
    auto const& revolution = ASE_GAME.user().options.evolution;

    docker.transformation().size_(2.0, 0);

    consumed.m_su8Text.clear();
    left.m_su8Text.clear();
    bank.m_su8Text.clear();
    process.m_su8Text.clear();
    swipe.m_su8Text.clear();

    if ((revolution.left > 0) == false)
    {
         auto* const pToAdd = m_avBests.ptr(0);
         pToAdd->resize(0);
    }

    bank    .set_text(STR_FORMATU8(bank     .m_su8Text, "#0/#1", best_count(), best_max()));
    consumed.set_text(STR_FORMATU8(consumed .m_su8Text, "#0", revolution.consumed));
    left    .set_text(STR_FORMATU8(left     .m_su8Text, "#0/#1", revolution.left >= 0 ? revolution.left : 0, revolution.max));
    swipe   .set_text(STR_FORMATU8(swipe    .m_su8Text, "#0%", revolution.left >= -1 ? emp::ast::cast<ui32>(m_fSwipe) : 0));
    process .set_text(STR_FORMATU8(process  .m_su8Text, "#0 - #1%",
        revolution.left >= 0 ? m_Rendered.m_Stamp : emp::tim::time_c(),
        revolution.left >= 0 ? emp::ast::cast<ui32>(m_fProcess) : 0));


    EMP_RET_FALSE_IF_NOT(tbase1::resize());

    process .slide(revolution.left >= 0 ? m_fProcess / 100.0f : 0);
    swipe   .slide(revolution.left >= -1 ? m_fSwipe / 100.0f : 0);
    bank    .slide(emp::ast::cast<f32>(best_count()) / emp::ast::cast<f32>(best_max()));
    left    .slide(emp::ast::cast<f32>(emp::tt::maxof(emp::ast::cast<f64>(revolution.left), 0.0) / emp::ast::cast<f64>(revolution.max)));
    consumed.slide(emp::ast::cast<f32>(emp::ast::cast<f64>(revolution.consumed) / emp::ast::cast<f64>(emp::tt::max<ui64>::value)));

    c_float fWidth = (docker.transformation().width() - 6 * spacer_left.transformation().width()) / 5.0f;

    button_consumed.transformation().width_(fWidth);
    button_left.transformation().width_(fWidth);
    button_bank.transformation().width_(fWidth);
    button_process.transformation().width_(fWidth);
    button_swipe.transformation().width_(fWidth);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::transform(emp::gui::stack_c& a_rStack)
{
    if (state().visible == false)
    { return true; }

    EMP_RET_FALSE_IF_NOT(tbase1::transform(a_rStack));

    reset(false);

    auto const& l_dump_bests = [](vector<emp::ose::best_s>* const a_pvFrom EMP_UNUSED)
    {
        EMP_XX_IF_LOG(
        EMP_LOG_INFO("// ----> SORTING <--- \\");
        for (size_t stIndex = 0; stIndex < a_pvFrom->count(); ++stIndex)
        {
            ose::best_s const& rBest = a_pvFrom->ref(stIndex);
            EMP_LOG_INFO("BEST #0 =  M #1, C #2, X #3, S #4",
                stIndex, rBest.m_stMutationCount, rBest.m_stCount,
                rBest.m_stCrossCount, emp::tim::time_c::to_seconds(rBest.m_Stamp));
        })
    };

    auto const& l_sort_bests = [](vector<emp::ose::best_s>* const a_pvFrom, vector<emp::ose::best_s>* const a_pvSorted, auto const& a_lTest, auto const& EMP_SILENT(a_lDump))
    {
        a_pvSorted->resize(0);
        for (size_t stFrom = 0; stFrom < a_pvFrom->count(); ++stFrom)
        {
            //EMP_LOG_INFO("BEFORE");
            //a_lDump(a_pvSorted);
            emp::ose::best_s rFrom = a_pvFrom->val(stFrom);
            bool bAdded = false;
            for (size_t stSort = 0; stSort < a_pvSorted->count(); ++stSort)
            {
                emp::ose::best_s rSort = a_pvSorted->val(stSort);
                if (a_lTest(rFrom, rSort))
                {
                    //EMP_LOG_INFO("INSERT #0/#1 : #2", stSort, a_pvSorted->count(), a_pvFrom->count());
                    a_pvSorted->insert(stSort, rFrom);
                    bAdded = true;
                    break;
                }
            }
            if (bAdded == false)
            {
                //EMP_LOG_INFO("ADD");
                a_pvSorted->add(rFrom);
            }

            //EMP_LOG_INFO("AFTER");
            //a_lDump(a_pvSorted);
        }
    };

    bool bBestChanged = false;

    EMP_LOGGER.m_eDisableLog = emp::log::log_e::Error;

    auto* const pToAdd = m_avBests.ptr(0);

    for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    {
        auto& rSimulated = m_aSimulated.ref(stIndex);
        if (rSimulated.m_bCompleted &&
            rSimulated.m_pProgram != nullptr &&
            emp::tt::is_not_max(rSimulated.m_uiTarget1))
        {
            emp::ose::best_s oBest;
            EMP_RET_FALSE_IF_NOT(m_pRenderer->is_beauty(rSimulated, oBest, false));
            if (oBest.m_bBeauty)
            {
                ++rSimulated.m_stBeautyCount;
            }
            else
            {
            }
            ++rSimulated.m_stAllCount;

            if (rSimulated.m_stAllCount >= 1)
            {
                //EMP_LOG_INFO("ALL COUNT");
                if (pToAdd->count() <= emp::ose::renderer_c::sc_ui32BestCount)
                {
                    //EMP_LOG_INFO("ADD COUNT");
                    if (rSimulated.m_stBeautyCount >= 1)
                    {
                        //EMP_LOG_INFO("BEATY COUNT");
                        oBest.m_stBeautyCount = rSimulated.m_stBeautyCount;
                        oBest.m_stAllCount = rSimulated.m_stAllCount;
                        oBest.m_stCrossCount = rSimulated.m_stCrossCount;
                        oBest.m_stMutationCount = rSimulated.m_stMutationCount;
                        oBest.m_Stamp = rSimulated.m_Stamp;

                        bool bDropped = pToAdd->count() < emp::ose::renderer_c::sc_ui32BestCount;
                        if (m_avBests.ptr(0)->count() >= emp::ose::renderer_c::sc_ui32BestCount)
                        {
                            ose::best_s& rBest = m_avBests.ref(0).last_ref();
                            if (oBest.m_stMutationCount > rBest.m_stMutationCount ||

                                (oBest.m_stMutationCount == rBest.m_stMutationCount &&
                                 oBest.m_stCount > rBest.m_stCount) ||

                                (oBest.m_stMutationCount == rBest.m_stMutationCount &&
                                 oBest.m_stCount == rBest.m_stCount &&
                                 oBest.m_stCrossCount > rBest.m_stCrossCount))
                            {
                                //EMP_LOG_INFO("DROP #0 =  M #1, C #2, X #3, S #4",
                                //stIndex, rBest.m_stMutationCount, rBest.m_stCount,
                                //    rBest.m_stCrossCount, emp::tim::time_c::to_seconds(rBest.m_Stamp));
                                if (rBest.m_pProgram != nullptr)
                                {
                                    EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rBest.m_stProgram));
                                    rBest.m_pProgram = nullptr;
                                    EMP_TT_MAX_VAR(rBest.m_stProgram);
                                }
                                m_avBests.ptr(0)->remove_last();
                                bDropped = true;
                            }
                        }

                        if (bDropped)
                        {
                            pToAdd->add(oBest);
                            EMP_LOG_INFO("BESTUP #0, BEAUTY #1/#2", m_avBests.ptr(0)->count(), oBest.m_stBeautyCount, oBest.m_stAllCount);
                            rSimulated.m_pProgram = nullptr;
                            EMP_TT_MAX_VAR(rSimulated.m_stProgram);
                            bBestChanged = true;
                        }
                    }
                    else
                    {
                        EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rSimulated.m_stProgram));
                        rSimulated.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(rSimulated.m_stProgram);
                    }
                }
            }
            else
            {
                auto* const pProgram = rSimulated.m_pProgram;
                c_size stProgram = rSimulated.m_stProgram;
                c_size stAll = rSimulated.m_stAllCount;
                c_size stBeauty = rSimulated.m_stBeautyCount;
                c_size stCross = rSimulated.m_stCrossCount;
                c_size stMutation = rSimulated.m_stMutationCount;
                emp::tim::time_c oStamp = rSimulated.m_Stamp;

                EMP_RET_FALSE_IF_NOT(rSimulated.reset(rSimulated.m_sMother, rSimulated.m_sFather, 1.0f/*EMP_GUI_SKIN.resolution.current()*/));

                rSimulated.m_pProgram = pProgram;
                rSimulated.m_stProgram = stProgram;
                rSimulated.m_bCompleted = false;
                rSimulated.m_pStack->copy(pProgram);
                rSimulated.m_stAllCount = stAll;
                rSimulated.m_stBeautyCount = stBeauty;
                rSimulated.m_stCrossCount = stCross;
                rSimulated.m_stMutationCount = stMutation;
                rSimulated.m_Stamp = oStamp;
            }
        }

        if (rSimulated.m_pProgram == nullptr && (ASE_GAME.user().options.evolution.left > 0))
        {
            #if defined EMP_OSE_RENDERING
            auto const& revolution = ASE_GAME.user().options.evolution;
            auto oFinally = emp::tt::finally([&rSimulated]
            {
                if (rSimulated.m_pProgram != nullptr)
                {
                    EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rSimulated.m_stProgram));
                    rSimulated.m_pProgram = nullptr;
                    EMP_TT_MAX_VAR(rSimulated.m_stProgram);
                }
            });
            if (m_pRenderer->load(rSimulated, 1.0f/*EMP_GUI_SKIN.resolution.current()*/, revolution.mutation, revolution.crossover, false))
            {
                c_astring sMother = rSimulated.m_sMother;
                c_astring sFather = rSimulated.m_sFather;
                c_size stMutation = rSimulated.m_stMutationCount;
                c_size stCrossover = rSimulated.m_stCrossCount;

                rSimulated.m_sMother = ALITERAL("fuck");
                rSimulated.m_sFather = astring();
                astring sFilename;
                m_pRenderer->get_filepath(rSimulated.m_sMother, sFilename);
                astring sBuffer;
                if (m_pRenderer->save(rSimulated, sFilename, sBuffer, false, true, true))
                {
                    oFinally.action();
                    emp::tt::silent(m_pRenderer->load(rSimulated, 1.0f/*EMP_GUI_SKIN.resolution.current()*/, false));
                }

                rSimulated.m_sMother = sMother;
                rSimulated.m_sFather = sFather;
                rSimulated.m_stMutationCount = stMutation;
                rSimulated.m_stCrossCount = stCrossover;
            }
            #endif
        }
    }
    EMP_LOGGER.m_eDisableLog = emp::log::log_e::none;

    if (bBestChanged)
    {
        bBestChanged = false;
        auto* pFrom = m_avBests.ptr(0);
        auto* pSort = m_avBests.ptr(1);

       l_sort_bests(pFrom, pSort,
            [](ose::best_s const& a_rFrom, ose::best_s const& a_rSort) EMP_RETURN -> bool
            { return emp::tim::time_c::more(a_rFrom.m_Stamp, a_rSort.m_Stamp); }, l_dump_bests);
        emp::mat::swap(pFrom, pSort);

        l_sort_bests(pFrom, pSort,
            [](ose::best_s const& a_rFrom, ose::best_s const& a_rSort) EMP_RETURN -> bool
            { return a_rFrom.m_stCrossCount > a_rSort.m_stCrossCount; }, l_dump_bests);
        emp::mat::swap(pFrom, pSort);

        l_sort_bests(pFrom, pSort,
            [](ose::best_s const& a_rFrom, ose::best_s const& a_rSort) EMP_RETURN -> bool
            { return a_rFrom.m_stCount > a_rSort.m_stCount; }, l_dump_bests);
        emp::mat::swap(pFrom, pSort);

        l_sort_bests(pFrom, pSort,
            [](ose::best_s const& a_rFrom, ose::best_s const& a_rSort) EMP_RETURN -> bool
            { return a_rFrom.m_stMutationCount > a_rSort.m_stMutationCount; }, l_dump_bests);
        emp::mat::swap(pFrom, pSort);

        if (pFrom != m_avBests.ptr(0))
        {
            emp::cnt::copy(*pFrom, m_avBests.ref(0));
            m_avBests.ref(1).resize(0);
        }

        //l_dump_bests(m_avBests.ptr(0));
    }

    EMP_LOGGER.m_eDisableLog = emp::log::log_e::Error;
    ASE_GAME.ose_renderer().reset();
    for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    {
        auto& rRenderlet = m_aSimulated.ref(stIndex);
        if (rRenderlet.m_pProgram != nullptr)
        {
            m_pRenderer->reset();
            m_pRenderer->run(rRenderlet,
                emp::ose::renderer_c::sc_ui32SimuInstructions,
                emp::ose::renderer_c::sc_fSimuTime,
                emp::ose::renderer_c::sc_ui32SimuBytes);
        }
    }
    EMP_LOGGER.m_eDisableLog = emp::log::log_e::none;

    if (m_bDummy)
    {
#if defined EMP_OSE_RENDERING
        emp::tt::silent(m_pRenderer->load_valid(m_Rendered, 1.0f/*EMP_GUI_SKIN.resolution.current()*/));
        m_bDummy = false;
        EMP_LOG_INFO("DUMMY-------> #0", m_Rendered.m_sMother);
#endif
    }
    //else
    {
        if (m_Rendered.m_pProgram != nullptr)
        {
            m_pRenderer->reset();
            m_pRenderer->run(m_Rendered,
                emp::ose::renderer_c::sc_ui32RenderInstructions,
                emp::ose::renderer_c::sc_fRenderTime,
                emp::ose::renderer_c::sc_ui32RenderBytes);

            m_fProcess = 100 * (m_Rendered.m_bCompleted == false ?
                (emp::tim::time_c::to_seconds(m_Rendered.m_Stamp) / emp::ose::renderer_c::sc_fRenderTime) : 1);
            m_Stamp = m_Rendered.m_Stamp;
        }
    }

    emp::gui::matrix_stacker_c oStacker(a_rStack, transformation(), state().enable);

    EMP_LOGGER.m_eDisableLog = emp::log::log_e::Error;
    c_v2f v2Size = transformation().size;
    for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    {
        if (m_aSimulated.ref(stIndex).m_pProgram != nullptr)
        { m_pRenderer->transform(m_aSimulated.ref(stIndex), v2Size); }
    }
    EMP_LOGGER.m_eDisableLog = emp::log::log_e::none;

    if (m_Rendered.m_pProgram != nullptr)
    { m_pRenderer->transform(m_Rendered, v2Size); }

    return true;
}

////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::render(void)
{
    if (state().visible == false)
    { return true; }

    m44f m44Identity;
    m44f::identity(m44Identity);

    auto& rDevice = EMP_GRAPHICS;
    gl_framebuffer_s oFramebuffer = rDevice.current_framebuffer();

    EMP_LOGGER.m_eDisableLog = emp::log::log_e::Error;
    //m44f const& rTransform = transformation().matrix;
    for (size_t stIndex = 0; stIndex < m_aSimulated.count(); ++stIndex)
    {
        if (m_aSimulated.ref(stIndex).m_pProgram != nullptr)
        {
            EMP_RET_FALSE_IF_NOT(m_pRenderer->renderU(m_aSimulated.ref(stIndex), m44Identity, emp::ose::renderer_c::sc_ui32SimuWidth, emp::ose::renderer_c::sc_ui32SimuHeight));
        }
    }
    EMP_LOGGER.m_eDisableLog = emp::log::log_e::none;


    if (m_Rendered.m_pProgram != nullptr)
    { EMP_RET_FALSE_IF_NOT(m_pRenderer->renderU(m_Rendered, m44Identity, m_pRenderer->m_ui32MaxWidth, m_pRenderer->m_ui32MaxHeight)); }

    EMP_RET_FALSE_IF_NOT(rDevice.bind_framebuffer(oFramebuffer.m_ui32Index, oFramebuffer.m_siW, oFramebuffer.m_siH));
//    EMP_RET_FALSE_IF_NOT(rDevice.clear({0, 0, 1, 1 }));

    if (emp::tt::is_not_max(m_uiTarget) &&
        emp::tt::is_not_max(m_Rendered.m_uiTarget1))
    {
        EMP_RET_FALSE_IF_NOT(rDevice.clear_target({0, 0, 0, 1 }));
        rDevice.set_blend(true);
        rDevice.set_depth_test(false);
        rDevice.set_depth_mask(false);
        rDevice.glBlendFunc_(GL_ONE, GL_ZERO);
        rDevice.set_vertex_buffer(rDevice.gl_quad_vertex_buffer());
        rDevice.set_index_buffer(rDevice.quad_index_buffer());
        rDevice.set_program(rDevice.transition_programs(m_stNextTransition));
        rDevice.set_uniform(uniform_e::m44Transform, m44Identity);

        {
            emp::gfx::target_s oTarget;
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(m_uiTarget, oTarget));
            rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTarget.m_ui32Texture);
        }


        {
            emp::gfx::target_s oRender;
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(m_Rendered.m_uiTarget1, oRender));
            rDevice.set_uniform(uniform_e::t2Texture_1, 1, oRender.m_ui32Texture);
        }

        rDevice.set_uniform(uniform_e::fProgress, (emp::mat::saturate(m_fTransitionTime)));
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    }
    else if (emp::tt::is_not_max(m_Rendered.m_uiTarget1))
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

        {
            emp::gfx::target_s oTarget;
            EMP_RET_FALSE_IF_NOT(rDevice.framebuffer(m_Rendered.m_uiTarget1, oTarget));
            rDevice.set_uniform(uniform_e::t2Texture_0, 0, oTarget.m_ui32Texture);
        }
        rDevice.draw(emp::gfx::primitive_e::Triangles);
    }

    if (m_bLaunched)
    {
        c_bool bEnded = m_fTransitionTime >= 1.0f;
        if (bEnded)
        {
            if (m_bSaved == false && ASE_GAME.user().options.evolution.left >= -1)
            {
                astring sPath;
                astring sFilename;
                EMP_RET_FALSE_IF_NOT(m_pRenderer->get_dnapath(sPath, sFilename));
                astring sBuffer;
                EMP_RET_FALSE_IF_NOT(m_pRenderer->save(m_ToSave, sPath, sBuffer, true, false, true));

                target_s oTarget;
                EMP_RET_FALSE_IF_NOT(EMP_GRAPHICS.framebuffer(m_uiTarget, oTarget)); // @@0 not the final compo...

                astring sThumbFile(sFilename);
                sThumbFile.trim_right(4);
                astring sThumbPath = STR_AFORMAT("#0png/#1_#2x#3.png", EMP_APPLICATION.resource_path(), sThumbFile,
                                                 oTarget.m_ui32Width, oTarget.m_ui32Height);

                EMP_RET_FALSE_IF_NOT(m_pRenderer->save(m_uiTarget, sThumbPath));

                if (&ASE_GAME.user().options.selection.pool->sources() != &m_vsTmpPool)
                {
                    ASE_GAME.user().options.selection.pool->add_source(sFilename);
                    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(ASE_GAME.user().options.selection.pool));
                }
                else
                { m_vsTmpPool.add(sFilename); }
                //EMP_RET_FALSE_IF_NOT(m_pRenderer->add_filename(sFilename));
                
                //if (m_OnFileAdded.is_valid())
                //{ m_OnFileAdded(sFilename); }

                /*if (ASE_GAME.user().options.evolution.left > 0)
                {
                    --ASE_GAME.user().options.evolution.left;
                }*/
                // @@0 remove EMP_LOG_INFO("SAVING #0", sFilename);
                m_bSaved = true;
            }

            // @@0 remove EMP_LOG_INFO("END CANCEL FALSE");
            m_bCancel = false;
        }


        m_fTransitionTime = emp::mat::saturate(m_fTransitionTime + m_fTransitionDelta * m_fTransitionDirection * v2f::length(m_v2fTransitionVelocity) * 100);
        // @@0 remove EMP_LOG_INFO("TRANSITION T #0, D #1, I #2, VX #3, VY #4, L #5", m_fTransitionTime, m_fTransitionDelta, m_fTransitionDirection,
           // m_v2fTransitionVelocity.x, m_v2fTransitionVelocity.y, v2f::length(m_v2fTransitionVelocity));
    }

    if (m_bDragged == false)
    {
        if (m_fTransitionTime <= 0.0f || m_fTransitionTime >= 1.0f)
        { m_bFirstSign = false; }
    }


    m_fSwipe = ((emp::tt::is_max(m_fTransitionTime) || (m_bDragged == false && m_fTransitionTime >= 1)) ||
        ASE_GAME.user().options.evolution.left < -1) ? 0 : (m_fTransitionTime * 100);

    //EMP_LOG_INFO("NEXT FALSE");
    //m_bNextFile = false;


    EMP_RET_FALSE_IF_NOT(tbase1::render());
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e evolution_c::process_mouse(emp::app::c_event_s& a_rEvent)
{
    emp::gui::process_mouse_on_control(*this, a_rEvent);

    if (state().visible == false)
    { return emp::gui::return_e::Unused; }

    auto const& rMouse = a_rEvent.mouse;
    auto& revolution = ASE_GAME.user().options.evolution;

    if (rMouse.m_eButton == emp::app::button_e::Left &&
        state().hover &&
        rMouse.m_stToucheCount == 1)
    {
        if (a_rEvent.mouse_pressed() && rMouse.m_pmTouches->count() == 1)
        {
            EMP_LOG_INFO("MOUSE_PRESSED");
            c_si64 siLeft = revolution.left;
            if (siLeft >= -1)
            {
                m_uiTouchId = rMouse.m_uiId;
                --revolution.left;
                if (m_bCancel == false && siLeft >= 0)
                {
                    ++revolution.consumed;

                    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(&ASE_GAME.user()));
                    auto& rDevice = EMP_GRAPHICS;

                    m_bMutate = true;

                    auto const l_dump_best = [this](emp::ose::best_s const& a_rBest EMP_UNUSED)
                    {
                        EMP_LOG_INFO("BESTDOWN #0, '#1' x '#2', TRI : #3, R : #4/#5, G : #6/#7, B : #8/#9, A #10/#11", m_avBests.ptr(0)->count(),
                                     a_rBest.m_sMother, a_rBest.m_sFather,
                                     a_rBest.m_stTriangleCount,
                                     a_rBest.m_fVarRed, a_rBest.m_fRed,
                                     a_rBest.m_fVarGreen, a_rBest.m_fGreen,
                                     a_rBest.m_fVarBlue, a_rBest.m_fBlue,
                                     a_rBest.m_fVarAlpha, a_rBest.m_fAlpha);
                    };

                    l_dump_best(m_Rendered.m_Best);

                    m_fTransitionTime = 0.0f;
                    m_v2fTransitionVelocity = v2f::zero();
                    m_stNextTransition = ASE_GAME.random().rand<size_t>(0, rDevice.transition_programs_count());

                    if (revolution.left >= 0)
                    {
                        if (emp::tt::is_not_max(m_uiTarget))
                        {
                            EMP_RET_X_IF_NOT(emp::gui::return_e::Error, EMP_GRAPHICS.unlock_framebuffer(m_uiTarget));
                            EMP_TT_MAX_VAR(m_uiTarget);
                        }
                    }
                    else
                    {
                        emp::gfx::target_s oRender;
                        EMP_ASSERT_BOOL_CALL(rDevice.framebuffer(m_uiTarget, oRender));
                        EMP_ASSERT_BOOL_CALL(rDevice.bind_framebuffer(oRender));
                        EMP_ASSERT_BOOL_CALL(rDevice.clear_target({0, 0, 0, 1 }));
                    }

                    emp::mat::swap(m_uiTarget, m_Rendered.m_uiTarget1);

                    //if (emp::tt::is_not_max(m_Rendered.m_uiTarget1))
                    //{ EMP_RET_FALSE_IF_NOT(is_beauty(m_Rendered, oBest, true)); }


                    if (m_ToSave.m_pProgram != nullptr)
                    {
                        EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(m_ToSave.m_stProgram));
                        m_ToSave.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(m_ToSave.m_stProgram);
                    }

                    if (m_Rendered.m_pProgram != nullptr)
                    {
                        m_ToSave.m_pProgram = m_Rendered.m_pProgram;
                        m_ToSave.m_stProgram = m_Rendered.m_stProgram;
                        m_ToSave.m_sMother = m_Rendered.m_sMother;
                        m_ToSave.m_sFather = m_Rendered.m_sFather;
                        m_Rendered.m_pProgram = nullptr;
                        EMP_TT_MAX_VAR(m_Rendered.m_stProgram);
                    }

                    auto* const pToAdd = m_avBests.ptr(0);

                    if (pToAdd->count() > 0)
                    {
                        emp::ose::best_s& rBest = pToAdd->first_ref();
                        EMP_RET_X_IF_NOT(emp::gui::return_e::Error, m_Rendered.reset(rBest.m_sMother, rBest.m_sFather, 1.0/*EMP_GUI_SKIN.resolution.current()*/));
                        m_Rendered.m_pProgram = rBest.m_pProgram;
                        m_Rendered.m_stProgram = rBest.m_stProgram;
                        m_Rendered.m_Best = rBest;
                        m_Rendered.m_pStack->copy(rBest.m_pProgram);
                        //m_pRenderer->print_ose(m_Rendered);
                        EMP_LOG_INFO("starting : '#0' x '#1'", m_Rendered.m_sMother, m_Rendered.m_sFather);
                        EMP_LOG_INFO("USE = M #0, C #1, X #2, S #3",
                                     rBest.m_stMutationCount, rBest.m_stCount,
                                     rBest.m_stCrossCount, emp::tim::time_c::to_seconds(rBest.m_Stamp));
                        rBest = emp::ose::best_s();
                        pToAdd->remove(0);
                    }
                    else
                    {
                        m_bDummy = revolution.left > -1;
                    }

                    size_t stIndex = 0;
                    while (stIndex < pToAdd->count())
                    {
                        emp::ose::best_s& rBest = pToAdd->ref(stIndex);
                        if (rBest.m_stCount <= 1)
                        {
                            if (rBest.m_pProgram != nullptr)
                            { EMP_ASSERT_BOOL_CALL(ASE_GAME.ose_renderer().m_pCompiler->dealloc_program(rBest.m_stProgram)); }
                            rBest = emp::ose::best_s();
                            pToAdd->remove(stIndex);
                        }
                        else
                        {
                            ++stIndex;
                        }
                    }
                    /*else
                     {
                     //m_Rendered.m_sMother = astring(ALITERAL("tgen_0000.txt"));
                     EMP_RET_FALSE_IF_NOT(m_pRenderer->load(m_Rendered, m_Rendered.m_sMother, m_Rendered.m_sFather, m_bMutate, EMP_GUI_SKIN.resolution().current_ratio()));
                     }*/
                    //m_Rendered.m_pStack->copy(m_Rendered.m_pProgram);
                    
                    if (revolution.left >= 0)
                    {
                        m_bFirstSign = false;
                        m_fTransitionTime = 0.0f;
                    }
                }
                m_bDragged = false;
                m_bLaunched = false;
                m_bSaved = false;
                m_bCancel = true;
                // @@0 remove EMP_LOG_INFO("TOUCH CANCEL TRUE");
                
                return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
            }
        }
        else if (a_rEvent.mouse_dragged() && v2f::length(rMouse.distance()) > 0.02f && rMouse.m_pmTouches->count() == 1)
        {
            if (emp::tt::is_not_max(m_uiTouchId))
            {
                auto const* const pDragged = rMouse.m_pmTouches->get(m_uiTouchId).value();
                if (pDragged != nullptr && v2f::length(pDragged->mouse.distance()) > 0.01f)
                {
                    //EMP_LOG_INFO("DELTA #0", v2f::length(pDragged->delta()));
                    m_v2fTransitionVelocity = pDragged->mouse.velocity();
                    c_float fVelocity = v2f::length(m_v2fTransitionVelocity);
                    if (m_bFirstSign == false && emp::mat::equal_(fVelocity, 0.0f) == false)
                    {
                        m_v2fTransitionDirection = m_v2fTransitionVelocity;
                        m_bFirstSign = true;
                    }

                    c_v2f v2fVelocity = fVelocity > 0 ? v2f::div(m_v2fTransitionVelocity, fVelocity) : v2f::zero();
                    m_fTransitionDirection = v2f::dot(v2f::normalize(m_v2fTransitionDirection), v2fVelocity);

                    m_fTransitionTime = emp::mat::saturate(m_fTransitionTime + fVelocity * m_fTransitionDirection);
                    m_bDragged = true;
                    m_bLaunched = false;

                    // @@0 remove EMP_LOG_INFO("LAUNCHED FALSE");
                    
                    return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                }
            }
        }
        else if (a_rEvent.mouse_released())
        {
            EMP_TT_MAX_VAR(m_uiTouchId);

            //if (emp::tt::is_not_max(m_uiTouchId))
            {
                if (rMouse.m_pmTouches->count() == 0)
                {
                    if (m_bDragged)
                    {
                        c_float fLength = v2f::length(m_v2fTransitionVelocity);

                        if (fLength <= 0.01f)
                        { m_v2fTransitionVelocity = v2f::zero(); }

                        if (fLength > 0.03f)
                        { m_v2fTransitionVelocity = v2f::mul(m_v2fTransitionVelocity, 0.03f / fLength); }

                        // @@0 remove EMP_LOG_INFO("LAUNCHED #0", v2f::length(m_v2fTransitionVelocity));

                        m_bCancel = true;
                        m_bLaunched = true;
                        m_bDragged = false;

                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                    else if (m_fTransitionTime <= 0)
                    {
                        EMP_LOG_INFO("TRANS CANCEL FALSE");
                        m_bCancel = false;
                        m_fTransitionTime = 1.0f;
                        return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
                    }
                    else // @@0 remove
                    {
                        int x = 0;
                        ++x;
                    }
                    
                    //m_fTransitionVelocity = m_fTransitionDuration;
                }
                else
                {
                    // @@0 remove EMP_LOG_INFO("TOUCH COUNT #0", rMouse.m_pmTouches->count());
                }
            }
        }
    }

    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN emp::gui::return_e evolution_c::process_keyboard(emp::app::c_event_s& a_rEvent)
{
    if (state().visible == false || state().enable == false)
    { return emp::gui::return_e::Unused; }

    auto const& rKeyboard = a_rEvent.keyboard;
    c_bool bPressed = rKeyboard.m_bPressed;
    emp::app::c_key_e eKey = rKeyboard.m_eKey;
    if (rKeyboard.modified() == false &&
        bPressed)
    {
        if (eKey == emp::app::key_e::Enter)
        {
            // @@0 remove astring sFilename;
            // @@0 remove EMP_RET_X_IF_NOT(return_e::Error, m_pRenderer->save(m_ToSave, sFilename, true, false));
            // @@0 remove return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
        else if (eKey == emp::app::key_e::Space)
        {
            //m_bMutate = false;
            //m_bSwitch = false;
            //m_bNextFile = true;
            return EMP_GUI_RETURN(a_rEvent, emp::gui::return_e::Consumed);
        }
        else if (eKey == emp::app::key_e::Alpha_S)
        {
            //m_bMutate = false;
            //m_bSwitch = true;
            //m_bNextFile = true;
        }
        else if (eKey == emp::app::key_e::Alpha_D)
        {
            //m_bMutate = true;
            //m_bSwitch = true;
            //m_bNextFile = true;
        }
        else if (eKey == emp::app::key_e::Alpha_F)
        {
            //m_bMutate = true;
            //m_bSwitch = false;
            //m_bNextFile = true;
        }
        else if (eKey == emp::app::key_e::Alpha_G)
        {
            m_bLookup = true;
        }
    }
    else if (rKeyboard.ctrl_only() && bPressed)
    {
        if (eKey == emp::app::key_e::Alpha_P)
        {
            // @@0 remove  m_bPng = true;
            // @@0 remove emp::tt::silent(m_pRenderer->save(m_uiTarget, m_Rendered.m_sMother));
            // @@0 remove return EMP_GUI_RETURN(a_rEvent, return_e::Consumed);
        }
    }
    return emp::gui::return_e::Unused;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void evolution_c::set_file(c_astring& a_sFile)
{
    m_vsTmpPool.resize(0);
    m_vsTmpPool.add(a_sFile);
    m_Rendered.m_sMother = a_sFile;
    m_Rendered.m_sFather = astring();
    ASE_GAME.ose_renderer().m_pvsOse = &m_vsTmpPool;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace mod {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
evolution_c::evolution_c(void):
evolution()
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
evolution_c::~evolution_c(void)
{ }

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::public_create(void)
{
    EMP_RET_FALSE_IF_NOT(evolution->create(&ASE_GAME.ose_renderer()));

    evolution->transformation().position_(0).size_(2.0f, 2.0f);
    evolution->state().autosize.xy(false, false);
    evolution->state().dock_(emp::gui::dock_e::Fill);
    //evolution->m_OnFileAdded = emp::fct::function(this, &evolution_c::on_file_added);

    window().add_control(evolution);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void evolution_c::public_destroy(void)
{
    // @@0 create m_pCtrlBank->destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::public_start(void)
{
    set_pause(false);
    evolution->reset(true);
    ASE_GAME.menu().docker.clear_controls();
    ASE_GAME.menu().docker.add_control(&evolution->docker);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void evolution_c::public_stop(void)
{
    ASE_GAME.menu().docker.clear_controls();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool evolution_c::public_update(void)
{ return true; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*void evolution_c::on_file_added(c_astring& a_sFilename)
{
    auto* const pPool = ASE_GAME.user().options.selection.pool;
    if (ASE_GAME.ose_renderer().pool() != &pPool->files())
    { pPool->add_file(a_sFilename); }
    EMP_ASSERT_BOOL_CALL(EMP_FACTORY.write_changes(ASE_GAME.user().options.selection.pool));
}*/

void evolution_c::set_file(c_astring& a_sFile)
{ evolution->set_file(a_sFile); }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

