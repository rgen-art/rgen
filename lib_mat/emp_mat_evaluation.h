//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_MAT_EVALUATION_H
#define EMP_MAT_EVALUATION_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_mat_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_map.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_pp_for.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// better asm
#define EMP_EVL_SOA

//#define EMP_EVL_AOS
#define EMP_EVL_DEBUG_NAN
#ifdef EMP_EVL_DEBUG_NAN
#define EMP_EVL_ASSERT(...) EMP_ASSERT(__VA_ARGS__)
#else
#define EMP_EVL_ASSERT(...)
#endif

#if defined EMP_EVL_SOA
#define EMP_EVL_STRIDE(x_Stride, x_Index, x_Size) ((x_Stride) + (x_Index) * emp::mat::precision_c::array_stride)
#elif defined EMP_EVL_AOS
#define EMP_EVL_STRIDE(x_Stride, x_Index, x_Size) ((x_Stride) * (x_Size) + (x_Index))
#else
#error
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_EVL_VECTOR_STRIDE 4
#define EMP_EVL_ARRAY_STRIDE 32
#define EMP_EVL_DATA_STRIDE 128

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class random_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOINSTANCE(class, precision_c)
public:
    typedef float type;
    static c_size vector_stride = EMP_EVL_VECTOR_STRIDE;
    static c_size array_stride = EMP_EVL_ARRAY_STRIDE;
    static c_size data_stride = EMP_EVL_DATA_STRIDE;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_EVL_OPERATOR_E_NAME operator_e
#define EMP_EVL_OPERATOR_E_VALUES\
    enum_value_ex( And ,(emp::str::and_ <char>::value, mat::and_impl))\
    enum_value_ex( RD  ,(emp::str::more <char>::value, mat::rd_impl))\
    enum_value_ex( XOr ,(emp::str::xor_ <char>::value, mat::xor_impl))\
    enum_value_ex( Or  ,(emp::str::or_  <char>::value, mat::or_impl))

#define enum_type
#define enum_name EMP_EVL_OPERATOR_E_NAME
#define enum_values EMP_EVL_OPERATOR_E_VALUES
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef bool (*PFnOperator)(c_bool, c_bool);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class expression;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, functor)
public:
    functor(void)
    { }

    virtual ~functor(void)
    { }

public:
    EMP_RETURN int operator()(expression const& a_rBinary, c_astring& a_sLeft, c_int a_siLeft,
        c_astring& a_sRight, c_int a_siRight, c_operator_e a_eOperator) const;

    EMP_RETURN int operator()(expression const& a_rBinary, c_astring& a_sPointer) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, expression)
public:
    expression(void);

public:
    EMP_RETURN int evaluate(c_operator_e a_eOperator, c_int a_siLeft, c_int a_siRight) const;

    template <typename t_Functor>
    EMP_NOINLINE EMP_RETURN int evaluate(c_astring& a_sExpression, t_Functor const& a_rFunctor) const
    {
        //EMP_LOG_DEBUG("Start : #0", a_sExpression);

        enum_array_of(size_t, operator_e) astOperations;
        for (size_t stIndex = 0; stIndex < astOperations.count(); ++stIndex)
        { astOperations.set(stIndex, size_t(emp::tt::max<size_t>::value)); } // @@0 set_max in emp::cnt::container_t

        int siParIndex = 0;
        uint uiParCount = 0;
        size_t stLen = 0;

        astring sCursor(a_sExpression);
        while (sCursor.len())
        {
            //EMP_LOG_DEBUG("CURSOR : #0", sCursor);

            c_char cCursor = sCursor.get_first_char();
            if (emp::str::is_null(cCursor))
            { break; }

            if (emp::str::is_not_alpha(cCursor) &&
                emp::str::is_not_numeric(cCursor) &&
                is_not_reserved(cCursor))
            {
                EMP_LOG_ERROR("invalid character while processing input '#0'", sCursor);
                return -1;
            }

            if (emp::str::is_left_parenthesis(cCursor))
            {
                if (emp::tt::is_max(siParIndex))
                {
                    EMP_LOG_ERROR("too much parenthesis");
                    return -1;
                }

                if (siParIndex == 0 && uiParCount == 0) {
                    uiParCount = 1;
                }
                ++siParIndex;
            }
            else if (emp::str::is_right_parenthesis(cCursor))
            {
                if (emp::tt::is_max(siParIndex))
                {
                    EMP_LOG_ERROR("too much parenthesis");
                    return -1;
                }

                if (siParIndex == 1 && uiParCount == 1) {
                    uiParCount = 2;
                }
                --siParIndex;
            }
            else if (siParIndex == 0)
            {
                c_size stCount = astOperations.count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    if (m_acOperators.val(stIndex) == cCursor &&
                        emp::tt::is_max(astOperations.val(stIndex)))
                    {
                        astOperations.set(stIndex, stLen);
                        EMP_LOG_DEBUG("operator found : #0", sCursor);
                    }
                }
            }

            sCursor.trim_left();
            ++stLen;

            if (emp::tt::is_not_max(astOperations.first_val()))
            {
                EMP_LOG_DEBUG("BREAK : #0", astOperations.first_val());
                break;
            }
        }

        if (siParIndex != 0 ||
            uiParCount % 2 != 0)
        {
            EMP_LOG_ERROR("invalid parenthesis count while processing input : '#0' : #1, #2",
                a_sExpression, siParIndex, uiParCount);
            return -1;
        }

        c_size stCount = astOperations.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            c_size stOperator = astOperations.val(stIndex);
            if (emp::tt::is_not_max(stOperator))
            {
                //EMP_LOG_DEBUG("EXPRESSION : #0", a_sExpression);
                astring sLeft(a_sExpression);
                //EMP_LOG_DEBUG("LL : #0, #1, #2, #3",
                //sLeft, sLeft.len(), sLeft.count(), stOperator);

                // @@0 remove count
                sLeft.trim_right(sLeft.len() - stOperator);

                astring sRight(a_sExpression);
                sRight.trim_left(stOperator + 1);

                //EMP_LOG_DEBUG("LEFT : #0", sLeft);
                c_int siLeft = evaluate(sLeft, a_rFunctor);
                if (siLeft != 0 && siLeft != 1)
                { return siLeft; }

                //EMP_LOG_DEBUG("RIGHT : #0", sRight);
                c_int siRight = evaluate(sRight, a_rFunctor);
                if (siRight != 0 && siRight != 1)
                { return siRight; }

                // @@0 enum_cast
                return a_rFunctor(*this, sLeft, siLeft, sRight, siRight, enum_from(operator_e, static_cast<ui32>(stIndex)));
            }
        }

        if (uiParCount == 2)
        {
            astring sLeft(a_sExpression);
            //EMP_LOG_DEBUG("Removing parenthesis from input : \"#0\"", sLeft);

            EMP_ASSERT(emp::str::is_left_parenthesis(sLeft.get_first_char()));
            EMP_ASSERT(emp::str::is_right_parenthesis(sLeft.get_last_char()));

            sLeft.trim_left();
            sLeft.trim_right();
            return evaluate(sLeft, a_rFunctor);
        }

        //EMP_LOG_DEBUG("Calling functor : \"#0\"", a_sExpression);
        return a_rFunctor(*this, a_sExpression);
    }

private:
    EMP_RETURN bool is_reserved(c_char a_cChar) const;
    EMP_RETURN bool is_not_reserved(c_char a_cChar) const;

private:
    enum_array_of(char, operator_e) m_acOperators;
    enum_array_of(PFnOperator, operator_e) m_apfnOperators;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name operation_e
#define enum_type
#define enum_values\
    enum_value( none          )\
    enum_value( Assign        )\
    enum_value( Comma         )\
    enum_value( Add           )\
    enum_value( Sub           )\
    enum_value( Mul           )\
    enum_value( Div           )\
    enum_value( Rand          )\
    enum_value( Sqrt          )\
    enum_value( Pow           )\
    enum_value( Sin           )\
    enum_value( Cos           )\
    enum_value( Abs           )\
    enum_value( Less          )\
    enum_value( More          )\
    enum_value( Lsq           )\
    enum_value( Mrq           )\
    enum_value( Sawtooth      )\
    enum_value( Triangle      )\
    enum_value( Square        )\
    enum_value( Scale         )\
    enum_value( Pi            )\
    enum_value( True          )\
    enum_value( False         )\
    enum_value( User          )\
    enum_value( UserMax, 1024 )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name instruction_e
#define enum_type
#define enum_values\
    enum_value( none      )\
    enum_value( Constant  )\
    enum_value( Input     )\
    enum_value( Link      )\
    enum_value( Operation )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <operation_e t_eOperation>
EMP_NOCOPY(class, operation_d) };

// @@0 static_cast overflow ?
#define EMP_EVL_OP(x_Operation, x_Result, x_Arity, x_Name, x_Precedence)\
template <>\
EMP_NOCOPY(class, operation_d<static_cast<operation_e>(x_Operation)>)\
public:\
    static const operation_e operation = static_cast<operation_e>(x_Operation);\
    static const ui32 precedence = x_Precedence;\
    static const ui32 result = x_Result;\
    static const ui32 arity = x_Arity;\
};\
typedef operation_d<static_cast<operation_e>(x_Operation)> x_Name;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@2 remove
typedef emp::tt::max<ui32> noprec_d;

#define EMP_EVL_COMMA_ARITY 2
#define EMP_EVL_ADD_ARITY 2
#define EMP_EVL_SUB_ARITY 2
#define EMP_EVL_MUL_ARITY 2
#define EMP_EVL_DIV_ARITY 2
#define EMP_EVL_RAND_ARITY 0
#define EMP_EVL_SQRT_ARITY 2
#define EMP_EVL_POW_ARITY 2
#define EMP_EVL_SIN_ARITY 1
#define EMP_EVL_COS_ARITY 1
#define EMP_EVL_ABS_ARITY 1
#define EMP_EVL_LESS_ARITY 2
#define EMP_EVL_MORE_ARITY 2
#define EMP_EVL_LSQ_ARITY 2
#define EMP_EVL_MRQ_ARITY 2
#define EMP_EVL_SAWTOOTH_ARITY 2
#define EMP_EVL_TRIANGLE_ARITY 2
#define EMP_EVL_SQUARE_ARITY 2
#define EMP_EVL_SCALE_ARITY 4
#define EMP_EVL_PI_ARITY 0
#define EMP_EVL_TRUE_ARITY 0
#define EMP_EVL_FALSE_ARITY 0


EMP_EVL_OP(operation_e::Comma   ,2,EMP_EVL_COMMA_ARITY   ,comma_d   ,0)
EMP_EVL_OP(operation_e::Add     ,1,EMP_EVL_ADD_ARITY     ,add_d     ,1)
EMP_EVL_OP(operation_e::Sub     ,1,EMP_EVL_SUB_ARITY     ,sub_d     ,1)
EMP_EVL_OP(operation_e::Mul     ,1,EMP_EVL_MUL_ARITY     ,mul_d     ,2)
EMP_EVL_OP(operation_e::Div     ,1,EMP_EVL_DIV_ARITY     ,div_d     ,2)
EMP_EVL_OP(operation_e::Rand    ,1,EMP_EVL_RAND_ARITY    ,rand_d    ,noprec_d::value)
EMP_EVL_OP(operation_e::Sqrt    ,1,EMP_EVL_SQRT_ARITY    ,sqrt_d    ,noprec_d::value)
EMP_EVL_OP(operation_e::Pow     ,1,EMP_EVL_POW_ARITY     ,pow_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Sin     ,1,EMP_EVL_SIN_ARITY     ,sin_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Cos     ,1,EMP_EVL_COS_ARITY     ,cos_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Abs     ,1,EMP_EVL_ABS_ARITY     ,abs_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Less    ,1,EMP_EVL_LESS_ARITY    ,less_d    ,noprec_d::value)
EMP_EVL_OP(operation_e::More    ,1,EMP_EVL_MORE_ARITY    ,more_d    ,noprec_d::value)
EMP_EVL_OP(operation_e::Lsq     ,1,EMP_EVL_LSQ_ARITY     ,lsq_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Mrq     ,1,EMP_EVL_MRQ_ARITY     ,mrq_d     ,noprec_d::value)
EMP_EVL_OP(operation_e::Sawtooth,1,EMP_EVL_SAWTOOTH_ARITY,sawtooth_d,noprec_d::value)
EMP_EVL_OP(operation_e::Triangle,1,EMP_EVL_TRIANGLE_ARITY,triangle_d,noprec_d::value)
EMP_EVL_OP(operation_e::Square  ,1,EMP_EVL_SQUARE_ARITY  ,square_d  ,noprec_d::value)
EMP_EVL_OP(operation_e::Scale   ,1,EMP_EVL_SCALE_ARITY   ,scale_d   ,noprec_d::value)
EMP_EVL_OP(operation_e::Pi      ,1,EMP_EVL_PI_ARITY      ,pi_d      ,noprec_d::value)
EMP_EVL_OP(operation_e::True    ,1,EMP_EVL_TRUE_ARITY    ,true_d    ,noprec_d::value)
EMP_EVL_OP(operation_e::False   ,1,EMP_EVL_FALSE_ARITY   ,false_d   ,noprec_d::value)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class input_t;
template <typename> class eval_stack_t;
template <typename> class function_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, function_i)
private:
public:
    function_i(void) {}
    virtual ~function_i(void) = 0;

public:
    //>> function_i
    virtual void set_result(c_size a_stValue) = 0;
    virtual EMP_RETURN size_t result(void) const = 0;
    virtual EMP_RETURN size_t arity(void) const = 0;
    virtual EMP_RETURN size_t size(void) const = 0;
    virtual EMP_RETURN operation_e operation(void) const = 0;
    virtual EMP_RETURN function_t<T>* implementation(void) = 0;
    virtual EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack) = 0;
    virtual EMP_RETURN size_t input_count(void) const = 0;
    virtual EMP_RETURN size_t get_input(c_astring& a_sName) const = 0;
    virtual EMP_RETURN input_t<T>* get_input(c_size a_stIndex) = 0;
    virtual EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const = 0;
    virtual void link(void) = 0;
    virtual EMP_RETURN bool user_result(c_size a_stIndex) const = 0;
    //<< function_i
};
template <typename T>
inline function_i<T>::~function_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class cash_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, eval_stack_t)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

public:
    EMP_POINTER(random_c) m_pRandom{};

private:
    struct slice_c
    {
        vector_d* output;
        vector_d* input;
        size_t arity;
        size_t remaining;
        operation_e operation;
    };

private:
    vector_d* m_pv4Values = nullptr;
    vector_d* m_ptFramePointer = nullptr;

    slice_c* m_pStack = nullptr;
    slice_c* m_pStackPointer = nullptr;
    slice_c* m_pCurrentSlice = nullptr;
    slice_c* m_pPreviousSlice = nullptr;

    size_t m_stFrameCount = 0;
    size_t m_stFrameSize = 0;
    size_t m_stStackSize = 0;

    size_t m_stFrameIndex = 0;
    size_t m_stFrameOffset = 0;

    bool m_bFirstPassed = false;
    bool m_bSecondPassed = false;

    bool m_bCompute = false;

    emp::cnt::vector<cash_t<T>*> m_vpCash{};

public:
    eval_stack_t(void) = default;
    ~eval_stack_t(void)
    { destroy(); }

public:
    void create(c_size a_stFrameCount, c_size a_stFrameSize)
    {
        m_bFirstPassed = false;
        m_bSecondPassed = false;

        c_size stFrameCount = m_stFrameCount;
        c_size stFrameSize = m_stFrameSize;

        /*size_t stNewFrameSize = 512 * precision_c::array_stride;
        while (stNewFrameSize < a_stFrameSize)
        { stNewFrameSize += 512 * precision_c::array_stride; }*/
        c_size stNewFrameSize = a_stFrameSize;

        if (stFrameCount >= a_stFrameCount &&
            stFrameSize >= stNewFrameSize)
        {
            m_stFrameCount = a_stFrameCount;
            m_stFrameSize = stNewFrameSize;
            m_stStackSize = m_stFrameCount * m_stFrameSize;
            m_ptFramePointer = m_pv4Values;
            m_pStackPointer = m_pStack;
            return;
        }

        destroy();

        EMP_LOG_INFO("FRAME SIZE #0", stNewFrameSize);

        m_stFrameCount = a_stFrameCount;
        m_stFrameSize = stNewFrameSize;
        m_stStackSize = m_stFrameCount * m_stFrameSize;

        m_pv4Values = EMP_ALLOCATE_ALIGNED_POD_ARRAY(vector_d, m_stStackSize, 16);
//        emp::mem::set(m_pv4Values, m_stStackSize * sizeof(vector_d), 0); // @@0 bug some nan
        m_ptFramePointer = m_pv4Values;

        m_pStack = EMP_ALLOCATE_ARRAY(slice_c, m_stFrameSize);
        m_pStackPointer = m_pStack;
    }

    void destroy(void);

public:
    void clean(void);

    void begin_frame(c_size a_stArity);

    void end_frame(void)
    {
        EMP_ASSERT(m_pStackPointer == (m_pStack + 1));
        m_stFrameIndex = (m_stFrameIndex + 1) % m_stFrameCount;
    }

public:
    EMP_RETURN cash_t<T>* get_function_cash(function_i<T>* const a_pValue);

    EMP_RETURN cash_t<T>* get_value_cash(input_t<T>* const a_pInput);

public:
    void push(c_operation_e a_eOperation, c_size a_stArity)
    {
        //EMP_LOG_DEBUG("STACK::PUSH OPERATION #0, ARITY #1, INDEX #2",
        //    static_cast<ui32>(a_eOperation), a_stArity,
        //        static_cast<ui32>(m_ptFramePointer - m_pv4Values));
        slice_c oSlice;
        oSlice.input = m_ptFramePointer;
        oSlice.output = oSlice.input;
        oSlice.arity = a_stArity;
        oSlice.remaining = a_stArity * precision_c::array_stride;
        oSlice.operation = a_eOperation;

        m_ptFramePointer = oSlice.input + oSlice.remaining;

        *m_pStackPointer = oSlice;
        m_pPreviousSlice = m_pCurrentSlice;
        m_pCurrentSlice = m_pStackPointer;
        ++m_pStackPointer;
    }

    void pop(void)
    {
        //EMP_LOG_DEBUG("STACK::POP");
        EMP_ASSERT(m_pStackPointer >= (m_pStack + 2));
        --m_pStackPointer;
        m_pCurrentSlice = m_pStackPointer - 1;
        m_pPreviousSlice = m_pCurrentSlice - 1;
    }

    EMP_RETURN bool valid_address(c_size a_stFrame, vector_d const* const a_pfAddress) const
    {
        vector_d* pFrameStart = m_pv4Values + a_stFrame * m_stFrameSize;
        c_size stFrameSize = static_cast<size_t>(a_pfAddress - pFrameStart);
        if (stFrameSize > m_stFrameSize)
        {
            EMP_LOG_ERROR("FRAME OVERFLOW #0, #1", stFrameSize, m_stFrameSize);
            return false;
        }

        c_size stStackSize = static_cast<size_t>(a_pfAddress - m_pv4Values);
        if (stStackSize > m_stStackSize)
        {
            EMP_LOG_ERROR("STACK OVERFLOW #0", stStackSize);
            return false;
        }

        return true;
    }

    EMP_RETURN size_t leaf_arity(void) const { return m_pCurrentSlice->arity; }
    EMP_RETURN size_t node_arity(void) const { return m_pPreviousSlice->arity; }
    EMP_RETURN size_t remaining(void) const { return m_pCurrentSlice->remaining; }
    EMP_RETURN size_t operation(void) const { return enum_to(m_pCurrentSlice->operation); }

    EMP_RETURN vector_d get_input(c_size a_stIndex) const
    {
        EMP_ASSERT(valid_address(m_stFrameIndex, m_pCurrentSlice->input + a_stIndex));
        return *(m_pCurrentSlice->input + a_stIndex);
    }

    EMP_RETURN vector_d get_last_input(c_size a_stIndex) const
    { return get_input(1, a_stIndex); }

    EMP_RETURN vector_d get_input(c_size a_stFrame, c_size a_stIndex) const
    {
        vector_d const* const pLastInput = m_pCurrentSlice->input - a_stFrame * m_stFrameSize +
            (m_stFrameIndex > 0 ? 0 : m_stStackSize);
        EMP_ASSERT(valid_address((m_stFrameIndex > 0 ? m_stFrameIndex : m_stFrameCount) - 1, pLastInput + a_stIndex));
        return *(pLastInput + a_stIndex);
    }

    EMP_RETURN vector_d get_last_output(c_size a_stIndex) const
    { return get_output(1, a_stIndex); }

    EMP_RETURN vector_d get_output(c_size a_stFrame, c_size a_stIndex) const
    {
        vector_d const* const pLastOutput = m_pPreviousSlice->output - a_stFrame * m_stFrameSize +
            (m_stFrameIndex > 0 ? 0 : m_stStackSize);
        EMP_ASSERT(valid_address((m_stFrameIndex > 0 ? m_stFrameIndex : m_stFrameCount) - 1, pLastOutput + a_stIndex));
        return *(pLastOutput + a_stIndex);
    }

    EMP_RETURN vector_d* input(void) const { return m_pCurrentSlice->input; }
    EMP_RETURN vector_d* node_output(void) const { return m_pPreviousSlice->output; }
    EMP_RETURN vector_d* leaf_output(void) const { return m_pCurrentSlice->output; }

    void push_node_output(c_size a_stStride, vector_d const a_fValue)
    {
        vector_d* const pOutput = m_pPreviousSlice->output + a_stStride;

        EMP_ASSERT(valid_address(m_stFrameIndex, pOutput));
        *pOutput = a_fValue;
        //++m_pPreviousSlice->output;

        EMP_ASSERT(m_pPreviousSlice->remaining > 0);
        //--m_pPreviousSlice->remaining;
        //EMP_LOG_DEBUG("NODE REMAINING #0/#1",m_pPreviousSlice->remaining, m_pPreviousSlice->arity*precision_c::array_stride);
    }

    void push_leaf_output(c_size a_stStride, vector_d const a_fValue)
    {
        vector_d* const pOutput = m_pCurrentSlice->output + a_stStride;

        EMP_ASSERT(valid_address(m_stFrameIndex, pOutput));
        *pOutput = a_fValue;
        //++m_pCurrentSlice->output;

        EMP_ASSERT(m_pCurrentSlice->remaining > 0);
        //--m_pCurrentSlice->remaining;
        //EMP_LOG_DEBUG("LEAF REMAINING #0/#1", m_pCurrentSlice->remaining, m_pCurrentSlice->arity * precision_c::array_stride);
    }

    void flush_node_output(c_size a_stCount)
    {
        EMP_ASSERT(m_pPreviousSlice->remaining >= (a_stCount * precision_c::array_stride));
        m_pPreviousSlice->remaining -= a_stCount * precision_c::array_stride;
#if defined EMP_EVL_SOA
        m_pPreviousSlice->output += a_stCount * precision_c::array_stride;
#elif defined EMP_EVL_AOS
        m_pPreviousSlice->output += a_stCount;
#else
#error
#endif
    }

    void flush_leaf_output(c_size a_stCount)
    {
        EMP_ASSERT(m_pCurrentSlice->remaining >= (a_stCount * precision_c::array_stride));
        m_pCurrentSlice->remaining -= a_stCount * precision_c::array_stride;
#if defined EMP_EVL_SOA
        m_pCurrentSlice->output += a_stCount * precision_c::array_stride;
#elif defined EMP_EVL_AOS
        m_pCurrentSlice->output += a_stCount;
#else
#error
#endif
    }

public:
    EMP_RETURN size_t frame_index(void) const { return m_stFrameIndex; }
    EMP_RETURN size_t frame_count(void) const { return m_stFrameCount; }
    EMP_RETURN bool first_passed(void) const { return m_bFirstPassed; }
    EMP_RETURN bool second_passed(void) const { return m_bSecondPassed; }

    EMP_RETURN bool compute(void) const { return m_bCompute; }
    void set_compute(c_bool a_bValue) { m_bCompute = a_bValue; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, cash_t)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    function_i<T>* m_pFunction;
    vector_d* m_pv4Results;
    bool m_bComputed;
    bool m_bResult;
    size_t m_stSize;
    size_t m_stCount;

public:
    cash_t(void):
        m_pFunction(),
        m_pv4Results(),
        m_bComputed(),
        m_bResult(),
        m_stSize(),
        m_stCount()
    { destroy(); }

    ~cash_t(void)
    { destroy(); }

public:
    void destroy(void)
    {
        m_pFunction = nullptr;
        if (m_pv4Results)
        { EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Results); }
        m_pv4Results = nullptr;
        m_stSize = 0;
        m_stCount = 0;
        clean();
    }

public:
    void clean(void)
    {
        m_bComputed = false;
        m_bResult = false;
    }

public:
    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack)
    {
        if (m_pFunction)
        {
            if (m_bComputed == false)
            {
                a_rStack.push(operation_e::Assign, m_pFunction->result());
                m_bResult = m_pFunction->evaluate(a_rStack);
                c_size stCount = m_pFunction->result();
                // @@0  simd aos soa memcpy
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    for (size_t stStride = 0; stStride < precision_c::array_stride; ++stStride)
                    {
                        c_size stOffset = EMP_EVL_STRIDE(stStride, stIndex, stCount);
                        set_result(stOffset, a_rStack.get_input(stOffset));
                    }
                }
                m_bComputed = true;
                a_rStack.pop();
                return m_bResult;
            }
            else
            { /*EMP_LOG_INFO("CASH");*/ }
        }
        else
        { m_bResult = true; }
        return m_bResult;
    }

    void realloc_results(c_size a_stNewCount)
    {
        if (a_stNewCount > m_stSize)
        {
            if (m_pv4Results)
            { EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Results); }
            m_pv4Results = EMP_ALLOCATE_ALIGNED_POD_ARRAY(vector_d, a_stNewCount, 16);
            m_stSize = a_stNewCount;
        }
        m_stCount = a_stNewCount;
    }

    void set_function(function_i<T>* const a_pValue)
    {
        m_pFunction = a_pValue;
        if (m_pFunction)
        { realloc_results(m_pFunction->result() * precision_c::array_stride); }
    }

    void set_value(c_size a_stStride, vector_d const a_v4Value)
    {
        m_pFunction = nullptr;
        realloc_results(precision_c::array_stride);
        set_result(a_stStride, a_v4Value);
    }

    EMP_RETURN size_t result_count(void) const { return m_stCount; }

    EMP_RETURN vector_d result(c_size a_stIndex)
    {
        EMP_ASSERT(a_stIndex < m_stCount);
        EMP_EVL_ASSERT(is_valid(m_pv4Results[a_stIndex]));
        return m_pv4Results[a_stIndex];
    }

    EMP_RETURN bool is_valid(vector_d const EMP_SILENT(a_v4Value EMP_UNUSED))
    {
        return false;
        /*return
            EMP_ASSERT_NUMBER(tool_d::get_x(a_v4Value)) &&
            EMP_ASSERT_NUMBER(tool_d::get_y(a_v4Value)) &&
            EMP_ASSERT_NUMBER(tool_d::get_z(a_v4Value)) &&
            EMP_ASSERT_NUMBER(tool_d::get_w(a_v4Value));*/
    }

    void set_result(c_size a_stIndex, vector_d const a_v4Value)
    {
        EMP_ASSERT(a_stIndex < m_stCount);
        EMP_EVL_ASSERT(is_valid(a_v4Value));
        m_pv4Results[a_stIndex] = a_v4Value;
    }

    EMP_RETURN vector_d const* results(void) const { return m_pv4Results; }

public:
    EMP_RETURN function_i<T>* function(void) const { return m_pFunction; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline void eval_stack_t<T>::clean(void)
{
    c_size stCount = m_vpCash.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cash_t<T>* const pCash = m_vpCash.val(stIndex);
        pCash->clean();
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline void eval_stack_t<T>::begin_frame(c_size a_stArity) //, c_size a_stResult)
{
    clean();

    if (m_bFirstPassed == false && m_stFrameIndex > 0)
    { m_bFirstPassed = true; }

    if (m_bSecondPassed == false && m_stFrameIndex > 1)
    { m_bSecondPassed = true; }

    m_stFrameOffset = m_stFrameIndex * m_stFrameSize;
    m_ptFramePointer = m_pv4Values + m_stFrameOffset;
    m_pStackPointer = m_pStack;

    //EMP_LOG_DEBUG(",__________________________________________________");
    //EMP_LOG_DEBUG("FRAME #0, INDEX #1", m_stFrameIndex, static_cast<ui32>(m_ptFramePointer - m_pv4Values));
    push(operation_e::Assign, a_stArity);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline EMP_RETURN cash_t<T>* eval_stack_t<T>::get_function_cash(
    function_i<T>* const a_pValue)
{
    cash_t<T>* pCash = nullptr;

    c_size stCount = m_vpCash.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        cash_t<T>* const pCurrent = m_vpCash.val(stIndex);
        function_i<T>* const pFunction = pCurrent->function();
        if (pFunction != nullptr &&
            pFunction == a_pValue)
        {
            pCash = pCurrent;
            break;
        }
    }

    if (pCash == nullptr)
    {
        pCash = EMP_ALLOCATE(cash_t<T>);
        pCash->set_function(a_pValue);
        //EMP_LOG_INFO("ALLOCATE_CASH");
        m_vpCash.add(pCash);
    }
    return pCash;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename> class function_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(class, operation_t)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

public:
    typedef function_i<T>* (*pfn_allocate_d)(void);

private:
    EMP_PRAGMA_PUSH_IGNORE(EMP_W_4324)
    vector_d m_v4Value;
    astring m_sName;
    operation_e m_eOperation;
    ui32 m_ui32Precedence;
    ui32 m_ui32Result;
    ui32 m_ui32Arity;
    pfn_allocate_d m_pFunction;
    EMP_PRAGMA_POP_IGNORE(EMP_W_4324)

public:
    operation_t(void):
        m_v4Value(),
        m_sName(),
        m_eOperation(),
        m_ui32Precedence(),
        m_ui32Result(),
        m_ui32Arity(),
        m_pFunction()
    { destroy(); }

    operation_t(operation_t const& a_rFrom):
        m_v4Value(),
        m_sName(),
        m_eOperation(),
        m_ui32Precedence(),
        m_ui32Result(),
        m_ui32Arity(),
        m_pFunction()
    { copy(a_rFrom); }

    EMP_INLINE operation_t(
        c_astring& a_sName,
        c_operation_e a_eOperation,
        c_ui32 a_ui32Precedence,
        c_ui32 a_ui32Result,
        c_ui32 a_ui32Arity,
        vector_d const a_fValue,
        pfn_allocate_d const a_pFunction):
        m_v4Value(a_fValue),
        m_sName(a_sName),
        m_eOperation(a_eOperation),
        m_ui32Precedence(a_ui32Precedence),
        m_ui32Result(a_ui32Result),
        m_ui32Arity(a_ui32Arity),
        m_pFunction(a_pFunction)
        {}


    ~operation_t(void)
    {}

public:
    operation_t& operator=(operation_t const& a_rFrom)
    {
        copy(a_rFrom);
        return *this;
    }

public:
    void destroy(void)
    {
        m_sName = astring();
        m_eOperation = operation_e::none;
        EMP_TT_MAX_VAR(m_ui32Precedence);
        m_ui32Result = 0;
        m_ui32Arity = 0;
        m_v4Value = tool_d::zero();
        m_pFunction = nullptr;
    }

    void copy(operation_t const& a_rFrom)
    {
        m_sName = a_rFrom.m_sName;
        m_eOperation = a_rFrom.m_eOperation;
        m_ui32Precedence = a_rFrom.m_ui32Precedence;
        m_ui32Result = a_rFrom.m_ui32Result;
        m_ui32Arity = a_rFrom.m_ui32Arity;
        m_v4Value = a_rFrom.m_v4Value;
        m_pFunction = a_rFrom.m_pFunction;
    }

public:
    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }
    EMP_RETURN c_astring& name(void) const { return m_sName; }

    void set_operation(c_operation_e a_eValue) { m_eOperation = a_eValue; }
    EMP_RETURN operation_e operation(void) const { return m_eOperation; }

    void set_precedence(c_ui32 a_ui32Value) { m_ui32Precedence = a_ui32Value; }
    EMP_RETURN ui32 precedence(void) const { return m_ui32Precedence; }

    void set_result(c_ui32 a_ui32Value) { m_ui32Result = a_ui32Value; }
    EMP_RETURN ui32 result(void) const { return m_ui32Result; }

    void set_arity(c_ui32 a_ui32Value) { m_ui32Arity = a_ui32Value; }
    EMP_RETURN ui32 arity(void) const { return m_ui32Arity; }

    EMP_RETURN bool infix(void) const { return emp::tt::is_not_max(m_ui32Precedence); }
    EMP_RETURN bool variable(void) const { return emp::tt::is_max(m_ui32Arity); }

    void set_function(pfn_allocate_d const a_pValue) { m_pFunction = a_pValue; }
    EMP_RETURN pfn_allocate_d function(void) const { return m_pFunction; }

    void set_value(vector_d const a_fValue) { m_v4Value = a_fValue; }
    EMP_RETURN vector_d value(void) const { return m_v4Value; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_(class, input_t)
public:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

public:
    friend class function_t<T>;

private:
    astring m_sName;
    size_t m_stBind;
    cash_t<T>* m_pCash;

public:
    input_t(void):
        m_sName(),
        m_stBind(),
        m_pCash()
    { destroy(); }

    input_t(input_t const& a_rFrom):
        m_sName(),
        m_stBind(),
        m_pCash()
    { copy(a_rFrom); }

    ~input_t(void)
    {}

public:
    input_t& operator=(input_t const& a_rFrom)
    {
        copy(a_rFrom);
        return *this;
    }

public:
    void destroy(void)
    {
        m_sName = astring();
        if (m_pCash != nullptr)
        { m_pCash->destroy(); }
        m_pCash = nullptr;
        reset();
    }

    void reset(void)
    {
        EMP_TT_MAX_VAR(m_stBind);
        if (m_pCash)
        {
            if (m_pCash->function())
            { m_pCash->destroy(); }
            else
            { m_pCash->clean(); }
        }
    }

    void clean(void)
    {
        if (m_pCash)
        { m_pCash->clean(); }
    }

    void copy(input_t const& a_rFrom)
    {
        m_sName = a_rFrom.m_sName;
        m_stBind = a_rFrom.m_stBind;
        m_pCash = a_rFrom.m_pCash;
    }

public:
    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack)
    {
#ifdef EMP_EVL_AOS
        c_ui32 ui32Arity = a_rStack.leaf_arity();
#endif
        c_size stBind = m_stBind;

        c_bool bReturn = m_pCash->evaluate(a_rStack);
        c_size stResult = m_pCash->result_count() / precision_c::array_stride;
        if (stBind < stResult)
        {
            vector_d* const pOutput = a_rStack.leaf_output();
            vector_d const* const pInput = m_pCash->results();

#define XTMP_STRIDE(x_Depth, x_Index, x_Count)\
            EMP_EVL_ASSERT(is_valid(pInput[EMP_EVL_STRIDE(x_Index, stBind, stResult)]));\
            pOutput[EMP_EVL_STRIDE(x_Index, 0, ui32Arity)] = pInput[EMP_EVL_STRIDE(x_Index, stBind, stResult)];
            EMP_PP_FOR(0, EMP_EVL_ARRAY_STRIDE, XTMP_STRIDE)
#undef XTMP_STRIDE
            a_rStack.flush_leaf_output(1);
        }
        return bReturn;
    }

public:
    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }
    void set_function(eval_stack_t<T>& a_rStack, c_size a_stBind, function_i<T>* const a_pValue);
    void set_value(eval_stack_t<T>& a_rStack, c_size a_stStride, vector_d const a_vValue);

public:
    EMP_RETURN c_astring& name(void) const { return m_sName; }
    EMP_RETURN cash_t<T>* cash(void) const { return m_pCash; }
    EMP_RETURN size_t bind(void) const { return m_stBind; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline EMP_RETURN cash_t<T>* eval_stack_t<T>::get_value_cash(input_t<T>* const a_pInput)
{
    cash_t<T>* pCash = a_pInput->cash();
    if (pCash == nullptr || pCash->function() != nullptr)
    {
        pCash = EMP_ALLOCATE(cash_t<T>);
        m_vpCash.add(pCash);
    }
    return pCash;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class,function_t, public, function_i<T>)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    emp::cnt::pod_vector<ui32> m_vui32Program;

    emp::cnt::map_t<astring, size_t> m_mstInputs;
    emp::cnt::vector<input_t<T> > m_vInputs;
    vector_d* m_pv4Constants;
    size_t m_stConstants;

    size_t m_stResult;
    size_t m_stArity;
    size_t m_stSize;
    size_t m_stTmpSize;


public:
    function_t(void):
        m_vui32Program(),
        m_mstInputs(4),
        m_vInputs(),
        m_pv4Constants(),
        m_stConstants(),
        m_stResult(),
        m_stArity(),
        m_stSize(),
        m_stTmpSize()
    {}

    ~function_t(void)
    { destroy(); }

public:
    void destroy(void);

public:
    //>> function_i
    void set_result(c_size a_stValue) { m_stResult = a_stValue; }
    EMP_RETURN size_t result(void) const { return m_stResult; }

    EMP_RETURN size_t arity(void) const { return m_stArity; }
    EMP_RETURN size_t size(void) const { return m_stSize; }

    EMP_RETURN operation_e operation(void) const { return operation_e::none; }
    EMP_RETURN function_t<T>* implementation(void) { return this; }

    EMP_RETURN bool evaluate(eval_stack_t<T>& a_rStack);

    EMP_RETURN size_t input_count(void) const
    { return m_vInputs.count(); }

    EMP_RETURN size_t get_input(c_astring& a_sName) const
    {
        emp::cnt::map_t<astring, size_t>::iterator_c const oIt = m_mstInputs.get(a_sName);
        if (oIt != m_mstInputs.end())
        { return *oIt.value(); }
        return emp::tt::max<size_t>::value;
    }

    EMP_RETURN input_t<T>* get_input(c_size a_stIndex) { return m_vInputs.ptr(a_stIndex); }
    EMP_RETURN input_t<T> const* get_input(c_size a_stIndex) const { return m_vInputs.ptr(a_stIndex); }

    // @@0  evl compute size
    void link(void)
    {
        m_stSize = m_stTmpSize + m_stArity * precision_c::array_stride;
        c_size stCount = m_vInputs.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            input_t<T>* const pInput = m_vInputs.ptr(stIndex);
            if (pInput && pInput->cash() && pInput->cash()->function())
            {
                function_i<T>* const pFunction = pInput->cash()->function();
                pFunction->link();
                m_stSize += pFunction->size();
            }
        }
    }
    EMP_RETURN bool user_result(c_size a_stIndex) const
    { return a_stIndex < result(); }
    //<< function_i

public:
    void set_arity(c_size a_stValue)
    { m_stArity = a_stValue; }

public:
    void add_constant(vector_d const a_v4Value, size_t& a_rstArity, size_t& a_rstResult)
    {
        size_t EMP_TT_MAX_VAR(stCurrent);

        c_size stCount = m_stConstants;
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                if (emp::mat::bool_equal(a_v4Value, m_pv4Constants[stIndex]))
                {
                    stCurrent = stIndex;
                    break;
                }
            }
        }

        if (emp::tt::is_max(stCurrent))
        {
            stCurrent = stCount;
            ++m_stConstants;
            vector_d* const pv4Constants = EMP_ALLOCATE_ALIGNED_POD_ARRAY(vector_d, m_stConstants, 16);
            pv4Constants[stCurrent] = a_v4Value;

            EMP_LOG_INFO("RESIZE #0", m_stConstants);
            if (stCount > 0)
            {
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { pv4Constants[stIndex] = m_pv4Constants[stIndex]; }
                EMP_DEALLOCATE_ALIGNED_POD_ARRAY(m_pv4Constants);
            }

            m_pv4Constants = pv4Constants;
        }

        add_instruction(enum_to_type_(instruction_e::Constant, ui32));
        add_instruction(static_cast<ui32>(stCurrent));
        a_rstArity = 0;
        a_rstResult = 1;
    }

    EMP_INLINE void add_input(c_astring& a_rsValue, size_t& a_rstArity, size_t& a_rstResult)
    {
        add_instruction(enum_to__(instruction_e::Input));
        add_instruction(static_cast<ui32>(register_input(a_rsValue)));
        a_rstArity = 1;
        a_rstResult = 1;
    }

    void add_link(function_t<T>* const EMP_SILENT(a_pValue), size_t& a_rstArity, size_t& a_rstResult)
    {
        add_instruction(enum_to__(instruction_e::Link));
        // @@0  lib_mat register link
        //add_instruction(register_link(a_pValue));

        a_rstArity = m_vui32Program.count();
        add_instruction(0);

        a_rstResult = m_vui32Program.count();
        add_instruction(0);
    }

    void add_operation(c_operation_e a_eValue, size_t& a_rstArity, size_t& a_rstResult)
    {
        add_instruction(enum_to__(instruction_e::Operation));
        add_instruction(enum_to_type(a_eValue, ui32));

        a_rstArity = m_vui32Program.count();
        add_instruction(0);

        a_rstResult = m_vui32Program.count();
        add_instruction(0);
    }

    void add_instruction(c_ui32 a_ui32Value)
    { m_vui32Program.add(a_ui32Value); }

    void set_instruction_arity(c_size a_stIndex, c_ui32 a_ui32Value)
    {
        m_stTmpSize += a_ui32Value * precision_c::array_stride;
        m_vui32Program.set(a_stIndex, a_ui32Value);
    }

    void set_instruction_result(c_size a_stIndex, c_ui32 a_ui32Value)
    { m_vui32Program.set(a_stIndex, a_ui32Value); }

    EMP_RETURN size_t get_instruction_index(void) const
    { return m_vui32Program.count(); }

public:
    EMP_INLINE EMP_RETURN size_t register_input(c_astring& a_sName)
    {
        c_size stIndex = get_input(a_sName);
        if (emp::tt::is_max(stIndex))
        {
            c_size stCount = m_vInputs.count();
            m_mstInputs.set(a_sName, stCount);
            input_t<T> oInput;
            oInput.set_name(a_sName);
            m_vInputs.add(oInput);
            return stCount;
        }
        return stIndex;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline void input_t<T>::set_function(eval_stack_t<T>& a_rStack, c_size a_stBind, function_i<T>* const a_pValue)
{
    m_pCash = a_rStack.get_function_cash(a_pValue);
    m_stBind = a_stBind;
    m_pCash->set_function(a_pValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
inline void input_t<T>::set_value(eval_stack_t<T>& a_rStack, c_size a_stStride, typename input_t::vector_d const a_vValue)
{
    m_pCash = a_rStack.get_value_cash(this);
    m_stBind = 0;
    m_pCash->set_value(a_stStride, a_vValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY(class, compiler_t)
private:
    using tool_d = emp::mat::simd_v4<T>;
    using vector_d = typename tool_d::type;

private:
    operation_t<T>* m_pOperations;
    size_t m_stCount;

public:
    compiler_t(void):
        m_pOperations(),
        m_stCount()
    {}

    ~compiler_t(void)
    { destroy(); }

public:
    EMP_RETURN bool create(c_size a_stCount);

    void destroy(void)
    {
        if (m_pOperations)
        {
            EMP_DEALLOCATE_ALIGNED_ARRAY(m_pOperations);
            m_pOperations = nullptr;
        }
        m_stCount = 0;
    }

public:
    EMP_RETURN bool regop(c_operation_e a_eOperation,
        operation_t<T> const& a_rOperation);
public:
    EMP_RETURN bool build(vector<function_i<T>*>& a_rvpFunctions, c_astring& a_sValue) const;

private:
    EMP_RETURN bool validate(c_astring& a_sName) const;

private:
    EMP_RETURN bool build__(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot, c_astring& a_sValue,
        size_t& a_rstArity, size_t& a_rstResult) const;

    EMP_RETURN operation_t<T> const* getop(c_astring& a_rsName) const;

    EMP_RETURN bool build_call(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot, c_astring& a_sExpression,
        size_t& a_rstArity, size_t& a_rstResult) const;

    EMP_RETURN bool build_leaf(vector<function_i<T>*>& a_rvpFunctions, function_t<T>* const a_pRoot, c_astring& a_sExpression,
        size_t& a_rstArity, size_t& a_rstResult) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif
