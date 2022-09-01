//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_PROGRAM_H
#define EMP_OSE_PROGRAM_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"
#include "emp_ose_lexeme.h"
#include "emp_ose_memory.h"
#include "emp_ose_type.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { struct random_float_s; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class lexeme_c;
struct node_c;
class compiler_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name node_e
#define enum_values\
    enum_value( none            )\
    enum_value( Declaration     )\
    enum_value( DefinitionU     )\
    enum_value( Call            )\
    enum_value( RealLiteral     )\
    enum_value( IntegralLiteral )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(node_c) // @@0 rename node_s
public:
    node_e m_eType = node_e::none;
    astring m_sName{};
    lexeme_c* m_pLexeme = nullptr;
    bool m_bConstant = false;
    bool m_bSource = 0;
    size_t EMP_TT_MAX_VAR(m_stProgramIndex);
    info_s m_Info{};
    vector<size_t>* m_pvstLeaves = nullptr;
    size_t EMP_TT_MAX_VAR(m_stLeaves);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, table_c)
public:
    friend class compiler_c;

private:
    vector_map_t<astring, vector_map_t<astring, lexeme_c*>*> m_vmOverloads;

public: // @@0 private
    vector_map_t<astring, lexeme_c*> m_vmLexemes;

    rti::type_vm m_vmTypes;

    rti::void_c const* m_pVoidType = nullptr;
    rti::data_c const* m_pRealType = nullptr;
    rti::data_c const* m_pSIntType = nullptr;
    rti::data_c const* m_pUIntType = nullptr;
    rti::data_c const* m_pBoolType = nullptr;

    vector<lexeme_c*> m_vpDeleteLexemes;
    vector<vector_map_t<astring, lexeme_c*>*> m_vpDeleteOverloads;
    vector<rti::type_c*> m_vpDeleteTypes;

    astring m_sEmpty;

    size_t m_stDataAddress = 0;

public:
    table_c(void);
    ~table_c(void);

public:
    EMP_RETURN bool create(void);
    EMP_RETURN bool clone(table_c const& a_rFrom);
    EMP_RETURN bool merge(table_c& a_rFrom);
    void destroy(void);

    void mother(c_bool a_bValue);

public:
    EMP_RETURN size_t data_address(void) const { return m_stDataAddress; }
    EMP_RETURN bool move_data_address(c_size a_stSize)
    {
        EMP_RET_FALSE_IF_NOT(a_stSize > 0);
        m_stDataAddress += a_stSize;
        return true;
    }

    EMP_RETURN vector_map_t<astring, lexeme_c*> const& lexemes(void) const
    { return m_vmLexemes; }

    template <typename t>
    EMP_INLINE EMP_RETURN rti::data_c const* get_data_type(void) const;

//-----------------------------------------------------------------------------
// lexeme
//-----------------------------------------------------------------------------
public:
    EMP_RETURN intrinsic_c const* reg_extension(c_ui32 a_ui32Extension, c_astring& a_sName, c_astring& a_sType, c_bool a_bReserved)
    {
        c_size stExtension = enum_to(instruction_e::Extension);
        c_instruction_e eInstruction = enum_from(instruction_e, stExtension + a_ui32Extension);
        return reg_intrinsic(eInstruction, a_sName, a_sType, a_bReserved);
    }

    EMP_RETURN c_astring& get_name_from_index(c_ui32 a_ui32Call) const;
    EMP_RETURN c_astring& get_name_from_instruction(c_instruction_e a_eInstruction) const;

    EMP_RETURN lexeme_c* get_lexeme(c_ui32 a_ui32Name) const;

    EMP_RETURN size_t get_variant(emp::mat::random_float_s& a_rRandom, c_ui32 a_ui32Call) const;

    EMP_RETURN function_c const* get_function(c_astring& a_sName) const
    { return to_function<function_c>(get_lexeme(a_sName)); }

    EMP_RETURN function_c* get_function(c_astring& a_sName)
    { return to_function<function_c>(get_lexeme(a_sName)); }

    EMP_RETURN lexeme_c* get_lexeme(c_astring& a_szName) const;
    //EMP_RETURN lexeme_c* get_lexeme(cpcchar a_szName) const;
    EMP_RETURN lexeme_c* get_lexeme(c_astring& a_sCall, rti::type_c const* const a_pType) const;

private:
    EMP_RETURN lexeme_c* get_match(c_astring& a_sName) const;

    EMP_RETURN comment_c* get_comment(c_astring& a_sName) const
    {
        auto const pLexeme = get_lexeme(a_sName);
        return pLexeme != nullptr ? to_lexeme<comment_c>(pLexeme) : nullptr;
    }

public:
    EMP_RETURN vector_map_t<astring, lexeme_c*>* get_overload(c_astring& a_sCall) const;

private:
    template <typename L>
    EMP_RETURN L* reg_lexeme(L* const a_pLexeme)
    {
        EMP_RET_NULL_IF(a_pLexeme == nullptr);

        rti::type_c const* const pType = a_pLexeme->m_pType;
        EMP_RET_NULL_IF(pType == nullptr);

        c_astring& sName = a_pLexeme->m_sName;
        auto* pOverload = get_overload(sName);
        if (pOverload != nullptr)
        {
            auto const& rVector = pOverload->vector();
            c_size stCount = rVector.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                lexeme_c const* const pLexeme = rVector.val(stIndex);
                EMP_RET_NULL_IF(pLexeme == nullptr);

                rti::type_c const* const pLexemeType = pLexeme->m_pType;
                EMP_RET_NULL_IF(pLexemeType == nullptr);
                if (pLexemeType->m_eType != pType->m_eType)
                {
                    EMP_LOG_ERROR("type mismatch between '#0' overloads, previous : '#1', current : '#2'",
                        pLexemeType->m_sName, enum_to_astring(pType->m_eType), enum_to_astring(pLexemeType->m_eType));
                    return nullptr;
                }
            }
        }
        else
        {
            pOverload = EMP_ALLOCATE((vector_map_t<astring, lexeme_c*>), 64u);
            EMP_RET_NULL_IF_NOT(m_vmOverloads.add(sName, pOverload));
            m_vpDeleteOverloads.add(pOverload);
        }

        EMP_RET_NULL_IF(get_lexeme(a_pLexeme->m_sName, pType) != nullptr, "duplicate symbol '#0'", sName);

        c_astring& sQualifiedName = a_pLexeme->m_sQualified;
        EMP_RET_NULL_IF_NOT((sQualifiedName.len() > 0));

        EMP_RET_NULL_IF_NOT(m_vmLexemes.add(sQualifiedName, a_pLexeme));
        EMP_RET_NULL_IF_NOT(pOverload->add(sQualifiedName, a_pLexeme));

        return a_pLexeme;
    }

    template <typename L, typename T>
    EMP_RETURN L* new_lexeme_sp(c_astring& a_sName, T const* const a_pType)
    {
        L* const pLexeme = EMP_ALLOCATE(L);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        m_vpDeleteLexemes.add(pLexeme);
        EMP_RET_NULL_IF_NOT(lexeme_c::create(pLexeme, index(), a_sName, a_pType));
        return pLexeme;
    }

    template <typename L>
    EMP_RETURN L* new_lexeme_cc(c_astring& a_sName, c_astring& a_sType)
    { return new_lexeme_sp<L>(a_sName, get_function_type(a_sType)); }

    template <typename L, typename T>
    EMP_RETURN L* new_lexeme_cp(c_astring& a_sName, T const* const a_pType)
    { return new_lexeme_sp<L>(a_sName, a_pType); }

    EMP_RETURN ui32 index(void) const
    { return emp::ast::cast<ui32>(m_vmLexemes.count()); }

    EMP_RETURN comment_c const* reg_comment(c_comment_e a_eComment, c_astring& a_sName)
    {
        auto pLexeme = new_lexeme_cp<comment_c>(a_sName, m_pVoidType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create(a_eComment) == false);
        return reg_lexeme(pLexeme);
    }

public:
    template <typename T>
    EMP_RETURN literal_t<T>* reg_literal(c_astring& a_sValue);

    template <typename T>
    EMP_RETURN literal_t<T>* reg_literal(T const& a_tValue);

    template <typename T>
    EMP_RETURN literal_t<T>* reg_literal(c_astring& a_sValue, T const& a_tValue);

private:
    EMP_RETURN punctuation_c* reg_punctuation(c_punctuation_e a_eValue, c_astring& a_sName, c_ui32 a_ui32Precedence)
    {
        auto pLexeme = new_lexeme_cp<punctuation_c>(a_sName, m_pVoidType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create(a_eValue, a_ui32Precedence) == false);
        return reg_lexeme(pLexeme);
    }

    EMP_RETURN operation_c* reg_operation(c_instruction_e a_eValue, c_astring& a_sName, c_ui32 a_ui32Precedence,
        c_astring& a_sType, c_bool a_bLeftToRight, c_bool a_bReserved)
    {
        auto pLexeme = new_lexeme_cc<operation_c>(a_sName, a_sType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create(a_eValue, a_ui32Precedence, a_bLeftToRight, a_bReserved) == false);
        return reg_lexeme(pLexeme);
    }

    EMP_RETURN intrinsic_c const* reg_intrinsic(c_instruction_e a_eValue, c_astring& a_sName, c_astring& a_sType, c_bool a_bReserved)
    {
        auto pLexeme = new_lexeme_cc<intrinsic_c>(a_sName, a_sType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create(a_eValue, a_bReserved) == false);
        return reg_lexeme(pLexeme);
    }

    EMP_RETURN control_c const* reg_control(c_control_e a_eValue, c_astring a_sName, c_astring a_sType, c_bool a_bReserved)
    {
        auto pLexeme = new_lexeme_cc<control_c>(a_sName, a_sType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create(a_eValue, a_bReserved) == false);
        return reg_lexeme(pLexeme);
    }

    EMP_RETURN function_c* reg_function(c_astring& a_sName, rti::function_c const* const a_pType)
    {
        auto pLexeme = new_lexeme_sp<function_c>(a_sName, a_pType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create() == false);
        return reg_lexeme(pLexeme);
    }

    EMP_RETURN variant_c* reg_variant(c_astring& a_sName, rti::function_c const* const a_pType)
    {
        auto pLexeme = new_lexeme_sp<variant_c>(a_sName, a_pType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create() == false);
        return reg_lexeme(pLexeme);
    }

public:
    EMP_RETURN data_c* reg_data(c_astring& a_sName, rti::data_c const* const a_pType)
    {
        auto pLexeme = new_lexeme_sp<data_c>(a_sName, a_pType);
        EMP_RET_NULL_IF(pLexeme == nullptr);
        EMP_RET_NULL_IF(pLexeme->create() == false);
        return reg_lexeme(pLexeme);
    }

//-----------------------------------------------------------------------------
// type
//-----------------------------------------------------------------------------
public:
    template <typename T>
    EMP_RETURN T* new_type(c_astring& a_sName)
    {
        T* const pType = EMP_ALLOCATE(T);
        EMP_RET_NULL_IF(pType == nullptr);
        m_vpDeleteTypes.add(pType);
        EMP_RET_NULL_IF_NOT(rti::type_c::create(pType, a_sName));
        return pType;
    }

    template <typename T>
    EMP_RETURN T* new_type(void) { return new_type<T>(astring()); }

    EMP_RETURN bool qualify_type(rti::function_c* const a_pType) const;

    template <typename T>
    EMP_RETURN bool reg_type(T* const a_pType)
    {
        EMP_RET_FALSE_IF(a_pType == nullptr);
        c_astring& sName = a_pType->m_sName;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("registering type '#0'", a_pType->m_sName);)
            EMP_RET_FALSE_IF(get_type(sName) != nullptr);
        EMP_RET_FALSE_IF_NOT(m_vmTypes.add(sName, a_pType));
        return true;
    }


    EMP_RETURN rti::data_c const* real_type(void) const { return m_pRealType; }
    EMP_RETURN rti::data_c const* uint_type(void) const { return m_pUIntType; }
    EMP_RETURN rti::data_c const* sint_type(void) const { return m_pSIntType; }
    EMP_RETURN rti::data_c const* bool_type(void) const { return m_pBoolType; }

    EMP_RETURN bool link(void) const;

private:
    EMP_RETURN bool qualify_type(astring& a_rsName, rti::const_type_v const& a_rvTypes) const;

    EMP_RETURN rti::function_c const* get_function_type(c_astring& a_sName) const
    { return rti::to_type<rti::function_c const>(get_type(a_sName)); }

public:
    EMP_RETURN rti::type_c const* get_type(c_astring& a_sName) const;

private:
    EMP_RETURN rti::void_c const* void_type(void) const { return m_pVoidType; }
};

template <>
EMP_INLINE EMP_RETURN rti::data_c const* table_c::get_data_type<float>(void) const { return m_pRealType; }

template <>
EMP_INLINE EMP_RETURN rti::data_c const* table_c::get_data_type<ui32>(void) const { return m_pUIntType; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, program_c)
public:
    friend class compiler_c;
    friend class processor_c;

    typedef emp::cnt::map_t<ui32, ui32> address_m;

    struct slot_s
    {
        bool variant;
        ui32 address;
    };
    typedef emp::cnt::map_t<ui32, slot_s> slot_m;

    size_t EMP_TT_MAX_VAR(m_stGen);
    size_t m_stCurrentGen = 0;

    astring m_sEmpty{};
    astring m_sFilename{};
    astring m_sExpression{};

    size_t EMP_TT_MAX_VAR(m_stEntryPoint);
    size_t EMP_TT_MAX_VAR(m_stEndOfBoot);
    size_t EMP_TT_MAX_VAR(m_stAst);

    table_c m_Table;

    memory_c m_Instructions{};
    memory_c m_Data{};

    slot_m m_mSlotFromIndex;
    address_m m_mui32IndexFromAddress;
    address_m m_mui32Calls;

    vector<size_t>* m_pvstNodes = nullptr;
    size_t EMP_TT_MAX_VAR(m_stNodes);

    EMP_POINTER(emp::mat::random_float_s) m_pRandom;

    astring m_sBuffer{};

public:
    program_c(void);
    ~program_c(void);

    void clear(void);

public:
    EMP_RETURN size_t find_node(compiler_c const* const a_pCompiler, lexeme_c const* const a_pLexeme);

public:
    EMP_RETURN bool instruction(c_size a_stIndex, char& a_rcInstruction) const;
    EMP_RETURN bool add_instruction(c_instruction_e a_eValue);

public:
    EMP_RETURN size_t entry_point(void) const { return m_stEntryPoint; }
    EMP_RETURN bool set_entry_point(c_size a_stEntry);
    EMP_RETURN size_t exit_point(void) const { return m_Instructions.count(); }

    template <typename T>
    void add_data(T const a_tValue)
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("ADD_DATA,  size #0, value '#1'", sizeof(T), a_tValue);)
        m_Instructions.push(a_tValue);
    }

    template <typename V, typename T>
    EMP_RETURN bool add_data_ex(T const a_tValue)
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("ADD_DATA, dst_size #0, src_size #1, value '#2'", sizeof(V), sizeof(T), a_tValue);)
        return m_Instructions.push<V, T>(a_tValue);
    }

    template <typename T>
    EMP_RETURN bool set_data(c_ui32 a_ui32Address, T const a_tValue)
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("SET_DATA, size #0, address #1, value '#2'", sizeof(T), a_ui32Address, a_tValue);)
        return m_Instructions.set<T>(a_ui32Address, a_tValue);
    }

    template <typename V, typename T>
    EMP_RETURN bool set_data(c_ui32 a_ui32Address, T const a_tValue)
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("SET_DATA, dst_size #0, src_size #1, address #2, value '#3'",
            sizeof(V), sizeof(T), a_ui32Address, a_tValue);)
        return m_Instructions.set<V, T>(a_ui32Address, a_tValue);
    }

public:

    EMP_RETURN bool add_data(lexeme_c const* const a_pLexeme);
    EMP_RETURN bool add_definition(c_ui32 a_ui32Index);

    EMP_RETURN bool add_function_call(c_ui32 a_ui32Index) { return add_call_impl<instruction_e::Function>(a_ui32Index); }

    EMP_RETURN bool add_variant_call(c_ui32 a_ui32Index) { return add_call_impl<instruction_e::Variant>(a_ui32Index); }

private:
    template <instruction_e t_eCall>
    EMP_RETURN bool add_call_impl(c_ui32 a_ui32Call);

public:
    EMP_RETURN bool link(c_bool a_bBlob);

public:
    EMP_RETURN c_astring& get_name_from_address(c_ui32 a_ui32Address) const;
    EMP_RETURN ui32 get_index_from_address(c_ui32 a_ui32Address) const;
    EMP_RETURN astring buffer(void) {return m_sBuffer; }
    void set_buffer(c_astring& a_rsValue) { m_sBuffer = a_rsValue; }

    void touch_variant_from_index(c_ui32 a_ui32Call);
    void touch_function_from_address(c_ui32 a_ui32Address);

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

