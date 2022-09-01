//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_COMPILER_H
#define EMP_OSE_COMPILER_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"
#include "emp_ose_lexeme.h"
#include "emp_ose_program.h"
#include "emp_ose_type.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_pool.h"
#include "emp_cnt_map.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace mat { class random_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

EMP_RETURN bool print_asm(program_c const& a_rProgram, astring& a_rsBuffer, c_bool a_bFull, vector<astring>& a_rvsInstructions);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class lexeme_c;
template <typename> class literal_t;
struct node_c;
class punctuation_c;
class processor_c;
class program_c;
class stack_c;
class table_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(lexeme_s)
    lexeme_c* lexeme;
    size_t index;
    size_t precedence;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name error_e
#define enum_values\
    enum_value(none)\
    enum_value(CompilerError)\
    enum_value(UserError)
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name token_e
#define enum_values\
    enum_value(none)\
    enum_value(Operator)\
    enum_value(IntegralLiteral)\
    enum_value(RealLiteral)
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(token_c)
public:
    astring m_sName{};
    lexeme_c* m_pLexeme = nullptr;
    token_e m_eType = token_e::none;
    si32 EMP_TT_MAX_VAR(m_siStack);
    size_t EMP_TT_MAX_VAR(m_stStack);
    info_s m_Info{};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef vector<token_c> token_v;
typedef token_v const token_cv;

typedef shell<token_c const> token_hc;
typedef token_hc const token_chc;
typedef vector<token_hc> token_vhc;
typedef token_vhc const token_cvhc;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(context_c)
public:
    si32 m_siParenthesis = 0;
    si32 m_siBrace = 0;
    si32 m_siBracket = 0;
    si32 m_siStack = 0;

    bool m_bParenthesis = false;
    bool m_bBrace = false;
    bool m_bBracket = false;
    bool m_bStack = false;

    size_t m_stLine = 1;
    size_t m_stColumn = 0;

    astring m_sExpression;

public:
    void create(c_astring& a_sExpression);
    void destroy(void);

public:
    EMP_RETURN size_t eat_char(void)
    {
        ++m_stColumn;
        m_sExpression.trim_left();
        return m_sExpression.len();
    }

    EMP_RETURN char get_char(void)
    { return m_sExpression.get_first_char(); }


    void add_line(void)
    {
        ++m_stLine;
        m_stColumn = 0;
    }

    EMP_RETURN bool end_of_context(void);

    EMP_RETURN bool resolve(token_c& a_rToken, c_bool a_bStack);
    void resolve(void);

    EMP_RETURN bool not_underflow(void) const;
    EMP_RETURN bool not_overflow(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(node_mutation_s)
    bool m_bActive = false;
    ui32 m_ui32Base = 0;
    ui32 m_ui32Count = 0;
    float m_fScale = 1.0f;
    float m_fOffset = 0.0f;

    EMP_RETURN bool operator==(node_mutation_s const& a_rFrom) const
    {
        return
            m_bActive == a_rFrom.m_bActive &&
            m_ui32Base  == a_rFrom.m_ui32Base &&
            m_ui32Count == a_rFrom.m_ui32Count &&
            emp::mat::equal_(m_fScale    , a_rFrom.m_fScale) &&
            emp::mat::equal_(m_fOffset   , a_rFrom.m_fOffset);
    }


    EMP_RETURN bool operator!=(node_mutation_s const& a_rFrom) const
    { return operator==(a_rFrom) == false; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(mutation_s)
    node_mutation_s literal{};
    node_mutation_s variant{};
    node_mutation_s call{};
    node_mutation_s data{};
    node_mutation_s trailing{};

    EMP_RETURN bool operator==(mutation_s const& a_rFrom) const
    {
        return
            literal == a_rFrom.literal &&
            variant == a_rFrom.variant &&
            call == a_rFrom.call &&
            data == a_rFrom.data &&
            trailing == a_rFrom.trailing;
    }

    EMP_RETURN bool operator!=(mutation_s const& a_rFrom) const
    { return operator==(a_rFrom) == false; }

    EMP_RETURN bool active(void) const
    {
        return
            literal.m_bActive ||
            variant.m_bActive ||
            call.m_bActive ||
            data.m_bActive ||
            trailing.m_bActive;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(crossover_s)
    bool m_bActive = false;
    ui32 m_ui32Base = 0;
    ui32 m_ui32Count = 0;
    ui32 m_ui32Offset = 0;
    bool m_bXMother = false;
    bool m_bXFather = false;

    EMP_RETURN bool operator==(crossover_s const& a_rFrom) const
    {
        return
            m_bActive  == a_rFrom.m_bActive &&
            m_ui32Base   == a_rFrom.m_ui32Base &&
            m_ui32Count  == a_rFrom.m_ui32Count &&
            m_ui32Offset == a_rFrom.m_ui32Offset &&
            m_bXMother == a_rFrom.m_bXMother &&
            m_bXFather == a_rFrom.m_bXFather;
    }

    EMP_RETURN bool operator!=(crossover_s const& a_rFrom) const
    { return operator==(a_rFrom); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, compiler_c)
private:
    EMP_POINTER(emp::mat::random_c) m_pRandom{};

public:
    EMP_POINTER(stack_c) m_pStack; // @@0 remove pointer

private:
    EMP_POINTER(processor_c) m_pProcessor; // @@0 remove pointer

    token_v m_vTokens{};

    size_t EMP_TT_MAX_VAR(m_stWeightNode);

    literal_t<real>* m_pWeight = nullptr;
    literal_t<bool>* m_pTrue = nullptr;
    literal_t<bool>* m_pFalse = nullptr;

    punctuation_c* m_pComma = nullptr;
    punctuation_c* m_pSemiColon = nullptr;
    punctuation_c* m_pRightBrace = nullptr;
    punctuation_c* m_pDeclare = nullptr;

    operation_c* m_pUnaryPos_I32 = nullptr;
    operation_c* m_pUnaryPos_F32 = nullptr;
    operation_c* m_pUnaryNeg_I32 = nullptr;
    operation_c* m_pUnaryNeg_F32 = nullptr;

public:
    punctuation_c* m_pLeftBrace = nullptr;

private:
    punctuation_c* m_pSpace = nullptr;

public:
    punctuation_c* m_pAssign = nullptr;
private:
    punctuation_c* m_pRightPar = nullptr;
    punctuation_c* m_pLeftPar = nullptr;

    context_c m_Context{};
public:
    table_c m_Table{};

public:
    vector<vector<lexeme_s>*> m_vpvLexemes{};
    vector<size_t> m_vstFreeLexemes{};

    vector<node_c> m_vNodes{};
    vector<size_t> m_vstFreeNodes{};

    vector<vector<size_t>*> m_vpvstLeaves{};
    vector<size_t> m_vstFreeLeaves{};

    vector<program_c*> m_vpPrograms{};
    vector<size_t> m_vstFreePrograms{};

    //emp::cnt::pool_t<variant_c, 4096> m_lVariants;

    vector<size_t> m_vstMergeNodes{};
    vector<size_t> m_vstMutateNodes{};

    vector<lexeme_c*> m_vpTmpLexemes{};
    map_t<lexeme_c*, size_t> m_mstTmpDefinitions;

    struct slice_s
    {
        size_t EMP_TT_MAX_VAR(node);
        size_t EMP_TT_MAX_VAR(child);
    };

    vector<slice_s> m_vpResolveStack{};

private:
    void eat_line(astring& a_rsExpression);

    EMP_RETURN bool eat_token(table_c& a_rTable, astring& a_rsToken, lexeme_c*& a_rpLexeme, token_e& a_rToken);

public:
    compiler_c(void);
    ~compiler_c(void);

public:
    EMP_RETURN bool create(void);

private:
    void clear(void);

public:
    template <typename T>
    EMP_RETURN bool foreach_nodes(c_size a_stNode, T const& a_rlApply)
    {
        m_vpResolveStack.clear();
        m_vpResolveStack.add({a_stNode, 0});

        while (true)
        {
            if (m_vpResolveStack.count() == 0)
            { break; }

            c_size stCount = m_vpResolveStack.count();
            slice_s* pSlice = m_vpResolveStack.last_ptr();
            EMP_RET_FALSE_IF(emp::tt::is_max(pSlice->node));

            auto const* const pvstLeaves = m_vNodes.ref(pSlice->node).m_pvstLeaves;
            if (pvstLeaves != nullptr && pSlice->child < pvstLeaves->count())
            {
                m_vpResolveStack.add({pvstLeaves->val(pSlice->child), 0});
                m_vpResolveStack.ref(stCount - 1).child++;
                continue;
            }
            else
            {
                EMP_RET_FALSE_IF_NOT(a_rlApply(pSlice->node));
                m_vpResolveStack.remove_last();
            }
        }
        return true;
    }

public:
#if defined EMP_UNIT_TEST
    EMP_RETURN bool unit_test_leak(void);
    EMP_RETURN bool unit_test(c_astring& a_sResourcePath, c_bool a_bKill, c_bool a_bLog, c_astring& a_sFile);
#endif

public:
    EMP_RETURN error_e push_data(program_c& a_rProgram, c_astring& a_rsName);

public:
    EMP_RETURN size_t from_file(c_astring& a_sFile, c_size a_stFather, c_mutation_s& a_rMutation, size_t& a_rstMutation, c_crossover_s& a_rCrossover, size_t& a_rstCross, c_bool a_bSkipGlobals, c_size a_stGen);

private:
    EMP_RETURN size_t from_string(c_astring& a_sExpression, c_size a_stFather, c_mutation_s& a_rMutation, size_t& a_rstMutation, c_crossover_s& a_rCrossover, size_t& a_rstCross, c_bool a_bSkipGlobals, c_size a_stGen);

public:
    EMP_RETURN size_t alloc_lexemes(void);
    EMP_RETURN bool dealloc_lexemes(c_size a_stLexeme);
    
    EMP_RETURN size_t alloc_program(void);
    EMP_RETURN bool dealloc_program(c_size a_stProgram);

    EMP_RETURN bool copy_leaves(program_c& a_rProgram, c_size a_stFrom, c_size a_stTo);
    EMP_RETURN size_t alloc_leaves(void);
    EMP_RETURN bool alloc_leaves(node_c& a_rNode);
    EMP_RETURN bool dealloc_leaves(c_size a_stLeaves);

EMP_RETURN bool set_child_node(c_size a_stParent, c_size a_stChild, c_size a_stIndex);
    EMP_RETURN bool insert_child_node(c_size a_stParent, c_size a_stChild, c_size a_stIndex);
    EMP_RETURN bool add_child_node(c_size a_stParent, c_size a_stChild);
    void add_program_node(program_c& a_rProgram, c_size a_stNode);

    EMP_RETURN size_t alloc_node(program_c* a_pProgram);
    EMP_RETURN bool dealloc_node(program_c& a_rProgram, c_size a_stNode);
    EMP_RETURN bool fill_node(c_size a_stNode, token_c const& a_rToken);
    EMP_RETURN bool fill_node(c_size a_stNode, lexeme_c* const a_pLexeme, info_s const& a_rInfo);

public:
    EMP_RETURN size_t build_ast(program_c& a_rProgram, token_chc& a_rvTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth);

    EMP_RETURN bool parse_parameters(program_c& a_rProgram, token_chc& a_rvTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth, c_size a_stOut);

    EMP_RETURN size_t parse_declaration(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, c_bool a_bLocal, c_si32 a_si32Depth);

    EMP_RETURN size_t parse_scope_or_control(program_c& a_rProgram, token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth);
    EMP_RETURN size_t parse_statements(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);
    EMP_RETURN size_t parse_assignment(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);

    EMP_RETURN size_t parse_call(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth);

private:
    EMP_RETURN bool get_lexemes(table_c const& a_rTable, vector<lexeme_s>& a_rvLexemes, token_chc& a_hTokens, c_si32 a_siStack);
public:
    EMP_RETURN bool build_tokens(program_c& a_rProgram);
    EMP_RETURN bool clean_tokens(program_c& a_rProgram);

    EMP_RETURN bool build_arguments(c_size a_stNode, program_c& a_rProgram, table_c& a_rTable);

    EMP_RETURN bool resolve_unaries(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable);
    EMP_RETURN bool resolve_literals(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable);
    EMP_RETURN bool resolve_calls(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable);

    EMP_RETURN bool resolve2(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable);

    EMP_RETURN bool build_asm(vector<size_t> const* const a_pvstNodes, program_c& a_rProgram, table_c& a_rTable, c_bool a_bSkipGlobals);
    EMP_RETURN bool build_asm(c_size a_stRoot, program_c& a_rProgram, table_c& a_rTable, c_bool a_bSkipGlobals);

public:
    EMP_RETURN bool print_ose(c_size a_stNode, astring& a_rsOutput);
    EMP_RETURN bool print_ose(c_size a_stNode, c_size a_stStack, astring& a_rsOutput, bool& a_rbEndScope, bool& a_rbEndDef, c_bool a_bFunctionDef);

public:
    EMP_RETURN bool mutate_ast(program_c& a_rProgram, table_c& a_rTable, c_mutation_s& a_rMutation, c_crossover_s& a_rCrossover, size_t& a_rstCross, size_t& a_rstMutation);
    EMP_RETURN bool merge_ast(program_c& a_rMother, program_c* const a_pFather);
    EMP_RETURN bool print_ast(c_size a_stNode, c_size a_stStack, astring& a_rsBuffer);


private:
    EMP_RETURN size_t insert_float(program_c& a_rProgram, astring const& a_sName, float const a_Value);
    EMP_RETURN size_t insert_uint(program_c& a_rProgram, astring const& a_sName, uint32_t const a_Value);
    EMP_RETURN size_t get_global_var(program_c& a_rProgram, astring const& a_sName);

    template <typename T>
    EMP_RETURN bool set_global_data(program_c& a_rProgram, rti::data_c const* const a_pType, astring const& a_sName, T const a_Value, T const a_Default);

    template <typename T>
    EMP_RETURN bool get_global_data(program_c& a_rProgram, astring const& a_sName, T& a_Value, T const a_Default);

    EMP_RETURN size_t get_global(program_c& a_rProgram, astring const& a_sName);

public:
    EMP_RETURN bool set_float(program_c& a_rProgram, astring const& a_sName, float const a_Value, float const a_Default);
    EMP_RETURN bool set_uint(program_c& a_rProgram, astring const& a_sName, uint32_t const a_Value, uint32_t const a_Default);

    EMP_RETURN bool rename(c_size a_stProgram, c_bool a_bPrune);
    EMP_RETURN bool prune(size_t a_stProgram);

    EMP_RETURN bool reprune(c_size a_stPRogram);

private:
    EMP_RETURN bool rename(c_size a_stAst, table_c& a_rTable, c_size a_stNode, size_t& a_rstGenes);
    EMP_RETURN bool rename_calls(c_size a_stNode, c_astring& a_sOldName, c_astring& a_sNewName);
    EMP_RETURN bool rename_def(c_size a_stAst, table_c& a_rTable, lexeme_c* const a_pLexeme, size_t& a_rstGenes);
    EMP_RETURN bool prune_calls(table_c& a_rTable, c_size a_stNode);
    EMP_RETURN bool prune_defs(c_size a_stNode);
    EMP_RETURN bool prune_comments(c_size a_stNode);

    EMP_RETURN size_t reprune_calls(table_c& a_rTable, c_size a_stNode);
    EMP_RETURN size_t reprune_defs(table_c& a_rTable, c_size a_stNode);


};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

