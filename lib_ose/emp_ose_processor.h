//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_PROCESSOR_H
#define EMP_OSE_PROCESSOR_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"
#include "emp_ose_memory.h"
#include "emp_ose_type.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fct { template <typename> class function_t; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class program_c;
class stack_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef emp::fct::function_t<return_e(*)(program_c const&, stack_c&, c_ui32)> extension_d;
typedef emp::fct::function_t<return_e(*)(void)> extension_push_d;
typedef emp::fct::function_t<return_e(*)(void)> extension_pop_d;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*template <typename T>
EMP_NOCOPY(class, register_c)
private:
    emp::cnt::vector<T> stack;
    size_t pointer;

public:
    register_c(void):
        stack(),
        pointer(0)
    {}

    void clear(void)
    {
        stack.resize(0);
        pointer = 0;
    }

    void push(T const a_tValue)
    { stack.add(a_tValue); }

    EMP_RETURN bool pop(T& a_rtValue)
    {
        if (stack.count() > 1)
        {
            a_rtValue = stack.last_ref();
            stack.remove_last();
            return true;
        }
        return false;
    }

    void update_pointer(void)
    { pointer = stack.count(); }
};*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, stack_c)
private:
    size_t EMP_TT_MAX_VAR(m_stInstructionPointer);
    memory_c m_Data{};

    size_t m_stStackIndex = 0;
    size_t m_stStackAddress = 0;

    emp::cnt::vector<size_t> m_vstFrames{};
    emp::cnt::vector<size_t> m_vstStacks{};

public:
    stack_c(void) {}

public:
    EMP_RETURN size_t byte_count(void) const
    {
        size_t stSize = 0;
        stSize += m_Data.byte_count();
        stSize += m_vstFrames.size() * sizeof(size_t);
        stSize += m_vstStacks.size() * sizeof(stSize);
        return stSize;
    }
    
public:
    void copy(program_c const* const a_pProgram);

public:
    void halt(c_size a_stIndex);

public:
    EMP_RETURN size_t instruction_pointer(void) const
    { return m_stInstructionPointer; }

public:
    template <typename T>
    void push(T const& a_tValue)
    { m_Data.push<T>(a_tValue); }

    template <typename T>
    EMP_RETURN bool pop(T& a_rtValue)
    { return m_Data.pop(a_rtValue); }

    template <typename T>
    EMP_RETURN bool val(c_size a_stIndex, T& a_rtValue) const
    { return m_Data.val(a_stIndex, a_rtValue); }

public:
    void push_frame(c_size a_stValue);
    void push_stack(void);

    EMP_RETURN bool pop_stack(void);
    EMP_RETURN bool pop_frame(size_t& a_rstIndex, size_t& a_rstPop);


    EMP_RETURN size_t current_stack(void) { return m_stStackIndex; }

public:
    EMP_RETURN size_t base_address(void) const
    { return m_stStackAddress; }

    EMP_RETURN bool push_base_address(c_ui32 a_u32ParameterSize)
    {
        c_size stBase = m_Data.count();
        EMP_RET_FALSE_IF(stBase >= emp::tt::max<ui32>::value);
        c_ui32 u32Base = emp::ast::cast<ui32>(stBase);
        EMP_RET_FALSE_IF(u32Base < a_u32ParameterSize);

        c_ui32 u32StackAddress = u32Base - a_u32ParameterSize;
        push<ui32>(u32StackAddress);

        m_stStackAddress = u32StackAddress;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("push_base_address #0", m_stStackAddress);)
        return true;
    }

    EMP_RETURN bool pop_base_address(void)
    {
        ui32 u32Base = 0;
        EMP_RET_FALSE_IF_NOT(pop<ui32>(u32Base));

        m_stStackAddress = u32Base;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("pop_base_address #0", m_stStackAddress);)
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, processor_c)
private:
    EMP_POINTER(extension_d) m_pFctExtension;
    EMP_POINTER(extension_push_d) m_pFctPush;
    EMP_POINTER(extension_pop_d) m_pFctPop;

public:
    processor_c(void);

public:
    EMP_RETURN si32 run(program_c& a_rProgram, stack_c& a_rStack,
        c_ui32 a_uiInstructionsPerFrame, c_ui32 a_uiInstructionsPerImage, ui32& a_ruiInstructionsProcessed);

public:
    void set_extension(extension_d const& a_rValue);
    void set_push(extension_push_d const& a_rValue);
    void set_pop(extension_pop_d const& a_rValue);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

