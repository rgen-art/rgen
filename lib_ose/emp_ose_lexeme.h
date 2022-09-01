//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_LEXEME_H
#define EMP_OSE_LEXEME_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_type.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp::mat { EMP_FORWARD(struct, random_float_s) }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name lexeme_e
#define enum_type
#define enum_values\
    enum_value( none        )\
    enum_value( Comment     )\
    enum_value( Literal     )\
    enum_value( Punctuation )\
    enum_value( Operation   )\
    enum_value( Intrinsic   )\
    enum_value( Control     )\
    enum_value( Function    )\
    enum_value( Variant     )\
    enum_value( Data        )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name comment_e
#define enum_type
#define enum_values\
    enum_value( none          )\
    enum_value( SingleComment )\
    enum_value( BeginComment  )\
    enum_value( EndComment    )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name punctuation_e
#define enum_type
#define enum_values\
    enum_value( none          )\
    enum_value( Comma         )\
    enum_value( SemiColon     )\
    enum_value( RightBrace    )\
    enum_value( Declare       )\
    enum_value( LeftBrace     )\
    enum_value( Space         )\
    enum_value( Assign        )\
    enum_value( RightPar      )\
    enum_value( LeftPar       )
 #include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name instruction_e
#define enum_type
#define enum_values\
   enum_value( none         )\
   enum_value( PushScope    )\
   enum_value( PopScope     )\
   enum_value( PushReal     )\
   enum_value( PushUInt     )\
   enum_value( PushSInt     )\
   enum_value( PushBool     )\
   enum_value( LoadAbs_F32  )\
   enum_value( LoadAbs_U32  )\
   enum_value( LoadAbs_S32  )\
   enum_value( LoadAbs_U8   )\
   enum_value( LoadRel_F32  )\
   enum_value( LoadRel_U32  )\
   enum_value( LoadRel_S32  )\
   enum_value( LoadRel_U8   )\
   enum_value( Enter        )\
   enum_value( Return       )\
   enum_value( Jump         )\
   enum_value( JumpCmp_SI32 )\
   enum_value( JumpCmp_UI32 )\
   enum_value( JumpCmp_Bool )\
   enum_value( More_F32     )\
   enum_value( More_I32     )\
   enum_value( MoreEq_F32   )\
   enum_value( MoreEq_I32   )\
   enum_value( Less_F32     )\
   enum_value( Less_I32     )\
   enum_value( LessEq_F32   )\
   enum_value( LessEq_I32   )\
   enum_value( Equal_I32    )\
   enum_value( Add_F32      )\
   enum_value( Add_U32      )\
   enum_value( Add_I32      )\
   enum_value( Sub_F32      )\
   enum_value( Sub_I32      )\
   enum_value( Mul_F32      )\
   enum_value( Mul_I32      )\
   enum_value( Div_F32      )\
   enum_value( Div_I32      )\
   enum_value( Mod_I32      )\
   enum_value( Neg_F32      )\
   enum_value( Neg_I32      )\
   enum_value( Pos_F32      )\
   enum_value( Pos_I32      )\
   enum_value( Abs_F32      )\
   enum_value( Abs_I32      )\
   enum_value( Cos_F32      )\
   enum_value( Sin_F32      )\
   enum_value( CosD_F32     )\
   enum_value( SinD_F32     )\
   enum_value( Sqrt_F32     )\
   enum_value( Print_I32    )\
   enum_value( Randf_F32    )\
   enum_value( Function     )\
   enum_value( Variant      )\
   enum_value( Extension    )\
   enum_value( Reserved  , emp::tt::max<ui8>::value - 1)\
   enum_value( Invalid   , emp::tt::max<ui8>::value    )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name control_e
#define enum_type
#define enum_values\
    enum_value( none     )\
    enum_value( Loop_U32 )\
    enum_value( Loop_I32 )\
    enum_value( If   )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, lexeme_c)
public:
    lexeme_e m_eType = lexeme_e::none;
    ui32 EMP_TT_MAX_VAR(m_ui32Index);
    astring m_sName{};
    astring m_sQualified{};
    rti::type_c const* m_pType = nullptr;
    bool m_bMother = false;
    bool m_bCold = false;
	bool m_bRenamed = false;
    bool m_bReserved = false;

    lexeme_c(void) {}
    virtual ~lexeme_c(void);

    EMP_RETURN astring compute_qualified_name(c_astring& a_sName) const;
    virtual void reassign(c_astring& a_sName, c_ui32 a_ui32Index);
    virtual void rename(c_astring& a_sName);

    template <typename T>
    static EMP_RETURN bool create(T* const a_pTo, c_ui32 a_ui32Index, c_astring& a_sName, rti::type_c const* const a_pType)
    {
        EMP_RET_FALSE_IF(a_pTo == nullptr);
        a_pTo->m_eType = T::lexeme_value;

        EMP_RET_FALSE_IF(a_pType == nullptr);
        a_pTo->m_pType = a_pType;

        a_pTo->reassign(a_sName, a_ui32Index);
        return true;
    }
};
inline lexeme_c::~lexeme_c(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, comment_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Comment;
    comment_e m_eCommentType = comment_e::none;

    EMP_RETURN bool create(c_comment_e a_eComment);

    comment_c(void) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOCOPY_BASE(class, literal_t, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Literal;
    static const rti::data_e data_value = rti::data_type_t<T>::value;

    rti::data_c const* m_pLiteralType = nullptr;
    T m_tLiteral = 0;

    literal_t(void) {}

   EMP_RETURN bool create(void)
   {
        if (emp::str::to(m_sName, m_tLiteral) == false) { return false; }
        return create(m_tLiteral);
   }

   EMP_RETURN bool create(T const& a_tLiteral)
   {
       rti::data_c const* const pType = rti::to_type<rti::data_c const>(m_pType);
       EMP_RET_FALSE_IF(pType == nullptr);
       EMP_RET_FALSE_IF(pType->m_eData != literal_t::data_value);
       m_pLiteralType = pType;
       m_tLiteral = a_tLiteral;
       return true;
   }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, punctuation_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Punctuation;

    punctuation_e m_ePunctuationType = punctuation_e::none;
    ui32 m_ui32Precedence = 0;

    punctuation_c(void) {}
    EMP_RETURN bool create(c_punctuation_e a_ePunctuation, c_ui32 a_ui32Precedence);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, operation_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Operation;

    rti::function_c const* m_pFunctionType = nullptr;
    instruction_e m_eInstruction = instruction_e::none;
    bool m_bLeftToRight = false;
    ui32 m_ui32Precedence = 0;

    operation_c(void) {}
    EMP_RETURN bool create(c_instruction_e a_eInstruction, c_ui32 a_ui32Precedence, c_bool a_bLeftToRight, c_bool a_bReserved);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, intrinsic_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Intrinsic;

    rti::function_c const* m_pFunctionType = nullptr;
    instruction_e m_eInstruction = instruction_e::none;

    intrinsic_c(void) {}
    EMP_RETURN bool create(c_instruction_e a_eInstruction, c_bool a_bReserved);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, control_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Control;

    rti::function_c const* m_pFunctionType = nullptr;
    control_e m_eControlType = control_e::none;

    control_c(void) {}
    EMP_RETURN bool create(c_control_e a_eControl, c_bool a_bReserved);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, function_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Function;

    rti::function_c const* m_pFunctionType = nullptr;
    ui32 EMP_TT_MAX_VAR(m_ui32Address);

    function_c(void) {}
    EMP_RETURN bool create(void);
    EMP_RETURN bool link(void) const { EMP_RET_FALSE_IF(emp::tt::is_max(m_ui32Address)); return true; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, variant_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Variant;

    struct slot_s
    {
        float weight;
        ui32 address;
    };

    rti::function_c const* m_pFunctionType = nullptr;
    size_t m_stLink = 0;
    float m_fSum = 0;
    emp::cnt::vector<slot_s> m_vSlots{};

    variant_c(void) {}
    EMP_RETURN bool create(void);

    EMP_RETURN size_t random(emp::mat::random_float_s& a_rRandom) const;

    void add(float const a_fWeight);

    EMP_RETURN bool add_address(c_ui32 a_ui32Address);

    void reassign(c_astring& a_sName, c_ui32 a_ui32Index);

    EMP_RETURN bool link(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, data_c, public, lexeme_c)
public:
    static const lexeme_e lexeme_value = lexeme_e::Data;

    rti::data_c const* m_pDataType = nullptr;
    bool m_bLocal = false;
    ui32 EMP_TT_MAX_VAR(m_ui32Address);

    data_c(void) {}
    EMP_RETURN bool create(void);
    EMP_RETURN bool link(void) const { return emp::tt::is_not_max(m_ui32Address); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename F>
inline EMP_RETURN T* to_lexeme_impl(F const a_pFrom)
{
    if (a_pFrom == nullptr)
    { return nullptr; }

    if (a_pFrom->m_eType != T::lexeme_value)
    { return nullptr; }

    return static_cast<T*>(a_pFrom);
}

template <typename T>
inline EMP_RETURN T const* to_lexeme(lexeme_c const* const a_pFrom) { return to_lexeme_impl<T const>(a_pFrom); }

template <typename T>
inline EMP_RETURN T* to_lexeme(lexeme_c* const a_pFrom) { return to_lexeme_impl<T>(a_pFrom); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename F>
EMP_RETURN T* to_literal_impl(F* const a_pFrom)
{
    if (a_pFrom == nullptr)
    { return nullptr; }

    if (a_pFrom->m_eType != T::lexeme_value)
    { return nullptr; }

    rti::data_c const* const pType = rti::to_type<rti::data_c const>(a_pFrom->m_pType);

    if (pType == nullptr)
    { return nullptr; }

    if (pType->m_eData != T::data_value)
    { return nullptr; }

    return static_cast<T*>(a_pFrom);
}

template <typename T>
EMP_RETURN literal_t<T> const* to_literal(lexeme_c const* const a_pFrom)
{ return to_literal_impl<literal_t<T> const>(a_pFrom); }

template <typename T>
EMP_RETURN literal_t<T>* to_literal(lexeme_c* const a_pFrom)
{ return to_literal_impl<literal_t<T>>(a_pFrom); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename F>
inline EMP_RETURN T* to_function_impl(F* const a_pFrom)
{
    if (a_pFrom == nullptr)
    { return nullptr; }

    if (a_pFrom->m_eType != T::lexeme_value)
    { return nullptr; }

    rti::function_c const* const pType = rti::to_type<rti::function_c const>(a_pFrom->m_pType);
    if (pType == nullptr)
    { return nullptr; }

    return static_cast<T*>(a_pFrom);
}

template <typename T>
inline EMP_RETURN T const* to_function(lexeme_c const* const a_pFrom)
{ return to_function_impl<T const>(a_pFrom); }

template <typename T>
inline EMP_RETURN T* to_function(lexeme_c* const a_pFrom)
{ return to_function_impl<T>(a_pFrom); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline EMP_RETURN lexeme_e get_lexeme(lexeme_c const* const a_pFrom);
inline EMP_RETURN lexeme_e get_lexeme(lexeme_c const* const a_pFrom)
{ return a_pFrom != nullptr ? a_pFrom->m_eType : lexeme_e::none; }

inline EMP_RETURN punctuation_e get_punctuation(lexeme_c const* const a_pFrom);
inline EMP_RETURN punctuation_e get_punctuation(lexeme_c const* const a_pFrom)
{
    punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(a_pFrom);
    return pPunctuation != nullptr ? pPunctuation->m_ePunctuationType : punctuation_e::none;
}

inline EMP_RETURN comment_e get_comment(lexeme_c const* const a_pFrom);
inline EMP_RETURN comment_e get_comment(lexeme_c const* const a_pFrom)
{
    comment_c const* const pComment = to_lexeme<comment_c const>(a_pFrom);
    return pComment != nullptr ? pComment->m_eCommentType: comment_e::none;
}

inline EMP_RETURN control_e get_control(lexeme_c const* const a_pFrom);
inline EMP_RETURN control_e get_control(lexeme_c const* const a_pFrom)
{
    control_c const* const pControl = to_function<control_c>(a_pFrom);
    return pControl != nullptr ? pControl->m_eControlType: control_e::none;
}

inline EMP_RETURN rti::type_c const* get_type(lexeme_c const* const a_pFrom);
inline EMP_RETURN rti::type_c const* get_type(lexeme_c const* const a_pFrom)
{ return a_pFrom != nullptr ? a_pFrom->m_pType : nullptr; }

inline EMP_RETURN instruction_e get_intrinsic(lexeme_c const* const a_pFrom);
inline EMP_RETURN instruction_e get_intrinsic(lexeme_c const* const a_pFrom)
{
    intrinsic_c const* const pIntrinsic = to_function<intrinsic_c>(a_pFrom);
    return pIntrinsic != nullptr ? pIntrinsic->m_eInstruction : instruction_e::none;
}

inline EMP_RETURN instruction_e get_operation(lexeme_c const* const a_pFrom);
inline EMP_RETURN instruction_e get_operation(lexeme_c const* const a_pFrom)
{
    operation_c const* const pOperation = to_function<operation_c>(a_pFrom);
    return pOperation != nullptr ? pOperation->m_eInstruction : instruction_e::none;
}

inline EMP_RETURN instruction_e get_instruction(lexeme_c const* const a_pFrom);
inline EMP_RETURN instruction_e get_instruction(lexeme_c const* const a_pFrom)
{
    if (a_pFrom != nullptr)
    {
        c_lexeme_e eLexeme = a_pFrom->m_eType;
        if (eLexeme == lexeme_e::Intrinsic)
        { return get_intrinsic(a_pFrom); }
        else if (eLexeme == lexeme_e::Operation)
        { return get_operation(a_pFrom); }
    }
    return instruction_e::none;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

