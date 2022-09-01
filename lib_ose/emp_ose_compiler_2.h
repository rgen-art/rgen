//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_COMPILER_2_H
#define EMP_OSE_COMPILER_2_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"
#include "emp_ose_lexeme.h"
#include "emp_ose_type.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_map.h"

#include "emp_mem_pointer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose2 {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class lexeme_c;
template <typename> class literal_t;
class node_c;
class punctuation_c;
class processor_c;
class program_c;
class stack_c;
class table_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef vector<program_c*> program_v;
typedef map_t<astring, program_c*> program_m;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef vector<node_c*> node_v;
typedef node_v const node_cv;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, token_c)
private:
    astring m_sName;
    lexeme_c* m_pLexeme;
    si32 m_siStack;
    size_t m_stStack;
    size_t m_stLine;
    size_t m_stColumn;

public:
    token_c(void):
        m_sName(),
        m_pLexeme(nullptr),
        EMP_TT_MAX_CTOR(m_siStack),
        EMP_TT_MAX_CTOR(m_stStack),
        EMP_TT_MAX_CTOR(m_stLine),
        EMP_TT_MAX_CTOR(m_stColumn)
        {}

public:
    token_c(token_c const& a_rFrom):
        m_sName(),
        m_pLexeme(nullptr),
        EMP_TT_MAX_CTOR(m_siStack),
        EMP_TT_MAX_CTOR(m_stStack),
        EMP_TT_MAX_CTOR(m_stLine),
        EMP_TT_MAX_CTOR(m_stColumn)
    { copy(a_rFrom); }

    token_c& operator=(token_c const& a_rFrom)
    {
        copy(a_rFrom);
        return *this;
    }

    void copy(token_c const& a_rFrom)
    {
        m_sName = a_rFrom.m_sName;
        m_pLexeme = a_rFrom.m_pLexeme;
        m_siStack = a_rFrom.m_siStack;
        m_stStack = a_rFrom.m_stStack;
        m_stLine = a_rFrom.m_stLine;
        m_stColumn = a_rFrom.m_stColumn;
    }

    EMP_RETURN c_astring& name(void) const { return m_pLexeme != nullptr ? m_pLexeme->name() : m_sName; }
    void set_name(c_astring& a_sValue) { m_sName = a_sValue; }

    EMP_RETURN lexeme_c* lexeme(void) const { return m_pLexeme; }
    void set_lexeme(lexeme_c* const a_pValue) { m_pLexeme = a_pValue; }

    EMP_RETURN size_t line_number(void) const { return m_stLine; }
    void set_line_number(c_size a_stValue) { m_stLine = a_stValue; }

    EMP_RETURN size_t column_number(void) const { return m_stColumn; }
    void set_column_number(c_size a_stValue) { m_stColumn = a_stValue; }

    EMP_RETURN si32 stack(void) const { return m_siStack; }
    void set_stack(c_si32 a_siValue) { m_siStack = a_siValue; }

    EMP_RETURN size_t st_stack(void) const { return m_stStack; }
    void set_st_stack(c_size a_stValue) { m_stStack = a_stValue; }
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
EMP_NOCOPY(class, context_c)
private:
    si32 m_siParenthesis;
    si32 m_siBrace;
    si32 m_siBracket;
    si32 m_siStack;

    bool m_bParenthesis;
    bool m_bBrace;
    bool m_bBracket;
    bool m_bStack;

    size_t m_stLine;
    size_t m_stColumn;

    astring m_sExpression;

public:
    context_c(void):
        m_siParenthesis(),
        m_siBrace(),
        m_siBracket(),
        m_siStack(),
        m_bParenthesis(),
        m_bBrace(),
        m_bBracket(),
        m_bStack(),
        m_stLine(),
        m_stColumn(),
        m_sExpression()
    { destroy(); }

public:
    void create(c_astring& a_sExpression)
    {
        destroy();
        m_sExpression = a_sExpression;
    }

public:
    EMP_RETURN c_astring& expression(void) const { return m_sExpression; }
    EMP_RETURN size_t expression_len(void) const { return m_sExpression.len(); }

public:
    EMP_RETURN size_t eat_char(void)
    {
        ++m_stColumn;
        m_sExpression.trim_left();
        return m_sExpression.len();
    }

    EMP_RETURN char get_char(void)
    { return m_sExpression.get_first_char(); }

public:
    void destroy();

public:
    void add_line(void);

public:
    EMP_RETURN bool end_of_context(void);

public:
    EMP_RETURN bool resolve(token_c& a_rToken, bool const a_bStack);

private:
    void resolve(void);

public:
    EMP_RETURN bool not_underflow(void) const;
    EMP_RETURN bool not_overflow(void) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, compiler_c)
private:
    EMP_POINTER(stack_c) m_pStack;
    EMP_POINTER(processor_c) m_pProcessor;
    EMP_POINTER(program_c) m_pProgram;

    program_c* m_pCurrentProgram;

    token_v m_vTokens;

    node_c* m_pWeightNode;

    literal_t<real>* m_pWeight;
    literal_t<bool>* m_pTrue;
    literal_t<bool>* m_pFalse;

    punctuation_c* m_pComma;
    punctuation_c* m_pSemiColon;
    punctuation_c* m_pRightBrace;
    punctuation_c* m_pDeclare;
    punctuation_c* m_pLeftBrace;
    punctuation_c* m_pSpace;
    punctuation_c* m_pAssign;
    punctuation_c* m_pRightPar;
    punctuation_c* m_pLeftPar;

    EMP_POINTER(context_c) m_pContext;
    EMP_POINTER(table_c) m_pTable;

    node_v  m_vpDeleteNodes;
    program_v  m_vpPrograms;
    program_m m_mpPrograms;

    map_t<node_c const*, bool> m_mpNodes;

    vector<node_c*> m_vpAsmStack;

private:
    void eat_line(astring& a_rsExpression);

    EMP_RETURN bool eat_token(table_c& a_rTable, astring& a_rsToken, lexeme_c*& a_rpLexeme, bool& a_rbOperator);

public:
    compiler_c(void);
    ~compiler_c(void);

public:
    EMP_RETURN bool create(void);

private:
    void clear(void);

public:
    EMP_RETURN bool from_ast(node_c const& a_rNode, program_c& a_rProgram, bool& a_rbCache);
    EMP_RETURN program_c* from_file(c_astring& a_sFile, c_bool a_bMutate, bool& a_rbCache, program_c* const a_pFather);

private:
    EMP_RETURN program_c* from_cache(c_astring& a_sExpression);
    EMP_RETURN program_c* from_string(c_astring& a_sExpression, c_bool a_bMutate, program_c* const a_pFather);

private:
    EMP_RETURN node_c* fill_node(node_c* const a_pProgram, token_c const& a_rToken);
    EMP_RETURN node_c* fill_node(node_c* const a_pProgram, lexeme_c* const a_pLexeme);

public:
    EMP_RETURN node_c* build_ast(token_chc& a_rvTokens, c_si32 a_si32Stack, table_c& a_rTable);

    EMP_RETURN bool parse_parameters(token_chc& a_rvTokens, c_si32 a_si32Stack, table_c& a_rTable, node_c* a_pOut);

    //EMP_RETURN node_c* parse_declarations(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);
    EMP_RETURN node_c* parse_declaration(token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, bool const a_bLocal);

    EMP_RETURN node_c* parse_scope_or_control(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);
    EMP_RETURN node_c* parse_statements(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);
    EMP_RETURN node_c* parse_assignment(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);

    EMP_RETURN node_c* parse_calls(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable);
    EMP_RETURN node_c* parse_call(token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable);

public:
    EMP_RETURN bool build_tokens(program_c& a_rProgram);
    EMP_RETURN bool build_arguments(node_c& a_rNode, program_c& a_rProgram, table_c& a_rTable);

    EMP_RETURN bool resolve(node_c* const a_pNode, table_c& a_rTable);

    EMP_RETURN bool build_asm(node_cv& a_rvpNodes, program_c& a_rProgram, table_c& a_rTable);
    EMP_RETURN bool build_asm(node_c* const a_pRoot, program_c& a_rProgram, table_c& a_rTable);

public:
    EMP_RETURN bool print_ose(node_c const* const a_pNode, astring& a_rsOutput);
    EMP_RETURN bool mutate_ast_0(table_c& a_rTable, program_c* const a_pProgram);

public:
    EMP_RETURN table_c const& table(void) const { return *m_pTable; }
    EMP_RETURN table_c& table(void) { return *m_pTable; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

