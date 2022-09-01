//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_compiler.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_lexeme.h"
#include "emp_ose_memory.h"
#include "emp_ose_processor.h"
#include "emp_ose_program.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_literal(c_astring& a_sValue);
EMP_RETURN bool is_literal(c_astring& a_sValue)
{
    c_size stLen = a_sValue.len();
    if ((stLen > 0) == false)
    { return false; }

    for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
    {
        c_char cChar = a_sValue.get_char(stIndex);
        if ((
            (cChar >= 'a' && cChar <= 'z') ||
            (cChar >= 'A' && cChar <= 'Z') ||
            (cChar >= '0' && cChar <= '9') ||
            (cChar == '.')) == false)
        { return false; }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_letter(c_char a_cValue);
EMP_RETURN bool is_letter(c_char a_cValue)
{
    return (a_cValue >= 'a' && a_cValue <= 'z') ||
           (a_cValue >= 'A' && a_cValue <= 'Z') ||
           a_cValue == '_';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_digit(c_char a_cValue);
EMP_RETURN bool is_digit(c_char a_cValue)
{
    return a_cValue >= '0' && a_cValue <= '9';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_hexa(c_char a_cValue);
EMP_RETURN bool is_hexa(c_char a_cValue)
{
    return (a_cValue >= 'a' && a_cValue <= 'f') ||
           (a_cValue >= 'A' && a_cValue <= 'F');
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_not_hexa(c_char a_cValue);
EMP_RETURN bool is_not_hexa(c_char a_cValue)
{
    return (a_cValue >= 'g' && a_cValue <= 'z') ||
           (a_cValue >= 'G' && a_cValue <= 'Z');
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_white(c_char a_cValue);
EMP_RETURN bool is_white(c_char a_cValue)
{
    return
        a_cValue == ' ' ||
        a_cValue == '\t' ||
        a_cValue == '\r';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_dot(c_char a_cValue);
EMP_RETURN bool is_dot(c_char a_cValue)
{
    return a_cValue == '.';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_newline(c_char a_cValue);
EMP_RETURN bool is_newline(c_char a_cValue)
{
    return a_cValue == '\n';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void eat_line(astring& a_rsExpression);
void eat_line(astring& a_rsExpression)
{
    while (a_rsExpression.len() > 0 &&
           emp::str::is_newline(a_rsExpression.get_first_char()) == false)
    { a_rsExpression.trim_left(); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 EMP_XX_FUNCTION in constexpr
#define EMP_OSE_OUTPUT_SHELL(x_Shell) print_shell(astring(ALITERAL(EMP_XX_FUNCTION)), x_Shell)
#ifdef EMP_OSE_DEBUG
#define EMP_OSE_DEBUG_SHELL(x_Shell) EMP_OSE_OUTPUT_SHELL(x_Shell)
#else
#define EMP_OSE_DEBUG_SHELL(x_Shell)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void print_shell(c_astring& a_sName, token_chc& a_hTokens);
void print_shell(c_astring& a_sName, token_chc& a_hTokens)
{
    astring sTokens;
    sTokens.append_string(a_sName);
    sTokens.append_literal(ALITERAL(" : '"));

    c_size stCount = a_hTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        sTokens.append_string(a_hTokens.ref(stIndex).m_sName);
        sTokens.append_char(' ');
    }

    if (stCount > 0)
    { sTokens.trim_right(); }
    sTokens.append_literal(ALITERAL("'"));

    EMP_LOG_INFO("#0", sTokens);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
astring print_shell(token_chc& a_hTokens);
astring print_shell(token_chc& a_hTokens)
{
    astring sTokens;
    c_size stCount = a_hTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        sTokens.append_string(a_hTokens.ref(stIndex).m_sName);
        sTokens.append_char(' ');
    }

    if (stCount > 0)
    { sTokens.trim_right(); }
    return sTokens;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef punctuation_c const* const cpcpun;

EMP_RETURN bool eat_right_shell(c_si32 a_si32Stack, cpcpun a_pLeft, cpcpun a_pRight, token_hc& a_rhFrom, token_hc& a_rhTo);
EMP_RETURN bool eat_right_shell(c_si32 a_si32Stack, cpcpun a_pLeft, cpcpun a_pRight, token_hc& a_rhFrom, token_hc& a_rhTo)
{
    EMP_OSE_DEBUG_SHELL(a_rhFrom);

    size_t EMP_TT_MAX_VAR(stRight);

    c_size stCount = a_rhFrom.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stOffset = stCount - stIndex - 1;
        token_c const& rToken = a_rhFrom.ref(stOffset);
        if (rToken.m_siStack == a_si32Stack)
        {
            lexeme_c const* const pLexeme = rToken.m_pLexeme;
            if (emp::tt::is_max(stRight))
            {
                if (pLexeme == a_pRight)
                { stRight = stOffset; }
            }
            else if (pLexeme == a_pLeft)
            {
                a_rhTo = a_rhFrom;
                a_rhTo.trim_left(stOffset + 1);
                a_rhTo.trim_right(stCount - stRight);

                a_rhFrom.trim_right(stCount - stOffset);
                return true;
            }
        }
    }
    return emp::tt::is_max(stRight);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool parse_symbol(c_astring& a_sValue);
EMP_RETURN bool parse_symbol(c_astring& a_sValue)
{
    c_size stLen = a_sValue.len();
    if (stLen > 0)
    {
        {
            c_char cChar = a_sValue.get_char(0);
            if ((
                (cChar >= 'a' && cChar <= 'z') ||
                (cChar >= 'A' && cChar <= 'Z') ||
                 cChar == '_') == false)
            { return false; }
        }

        for (size_t stIndex = 1; stIndex < stLen; ++stIndex)
        {
            c_char cChar = a_sValue.get_char(stIndex);
            if ((
                (cChar >= 'a' && cChar <= 'z') ||
                (cChar >= 'A' && cChar <= 'Z') ||
                 cChar == '_' ||
                (cChar >= '0' && cChar <= '9')) == false)
            { return false; }
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void print_tokens(token_cv& a_vTokens, astring& a_rsBuffer);
void print_tokens(token_cv& a_vTokens, astring& a_rsBuffer)
{
    size_t stMaxLen = 0;
    c_size stCount = a_vTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { stMaxLen = emp::tt::maxof(stMaxLen, a_vTokens.ref(stIndex).m_sName.len()); }

    astring sSpace;

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        token_c const& rToken = a_vTokens.ref(stIndex);
        c_size stSpaceCount = stMaxLen - rToken.m_sName.len();

        sSpace.clear();
        for (size_t stSpace = 0; stSpace < stSpaceCount; ++stSpace)
        { sSpace.append_char(' '); }

        a_rsBuffer.append_string(STR_AFORMAT("line #0 : col #1 : stack #2 : '#3#4' : lexeme '#5'\n",
            INT_FMT_D3(rToken.m_Info.m_stLine),
            INT_FMT_D3(rToken.m_Info.m_stColumn),
            rToken.m_siStack, sSpace, rToken.m_sName,
            rToken.m_pLexeme != nullptr ? ALITERAL("true") : ALITERAL("false")));
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::print_ose(c_size a_stNode, astring& a_rsOutput)
{
    size_t stStack = 0;
    bool bEndScope = false;
    bool bEndDef = false;
    return print_ose(a_stNode, stStack, a_rsOutput, bEndScope, bEndDef, false);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::print_ose(c_size a_stNode, c_size a_stStack, astring& a_rsOutput, bool& a_rbEndScope, bool& a_rbEndDef, c_bool EMP_SILENT(a_bFunctionDef))
{
    node_c& rNode = m_vNodes.ref(a_stNode);

    //EMP_LOG_DEBUG("---------------------------------------");
    //EMP_LOG_DEBUG("#0", a_rsOutput);

    a_rbEndScope = false;
    a_rbEndDef = false;

    //EMP_RET_FALSE_IF(a_rmpNodes.get(a_pNode) != a_rmpNodes.end());

    //a_rmpNodes.set(a_pNode, true); // @@0 optim set return iterator

    //auto const oFinally = emp::tt::finally([&a_rmpNodes, a_pNode]{ a_rmpNodes.remove(a_rmpNodes.get(a_pNode)); });

    auto const& l_print_tab = [&a_rsOutput](c_size l_stStack)
    {
        for (size_t stIndex = 0; stIndex < ((0 + l_stStack) * 4); ++stIndex)
        { a_rsOutput.append_char(' '); }
    };

    //auto const& l_dump_signature = [](astring& a_rsOutput, rti::type_c const* const a_pType, c_astring a_sName, c_bool a_bDefinition, const auto& a_lDumpSignature) -> bool
    //{
    //    EMP_RET_FALSE_IF(a_pType == nullptr);

    //    if (a_sName.len() > 0)
    //    {
    //        a_rsOutput.append_char('@');
    //        a_rsOutput.append_string(a_sName);
    //    }

    //    auto const& l_dump_vector = [](astring& a_rsOutput, rti::const_type_v const& a_vElement, c_bool a_bDefinition, const auto& a_lDumpSignature) -> bool
    //    {
    //        c_size stCount = a_vElement.count();
    //        if (stCount > 0)
    //        {
    //            a_rsOutput.append_char('(');
    //            astring sParam;

    //            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    //            {
    //                sParam.clear();
    //                if (a_bDefinition)
    //                { STR_FORMATA(sParam, "p#0", stIndex); }
    //                EMP_RET_FALSE_IF_NOT(a_lDumpSignature(a_rsOutput, a_vElement.ref(stIndex), sParam, false, a_lDumpSignature));
    //                a_rsOutput.append_char(',');
    //                a_rsOutput.append_char(' ');
    //            }
    //            a_rsOutput.trim_right(2);
    //            a_rsOutput.append_char(')');
    //        }
    //        return true;
    //    };

    //    EMP_RET_FALSE_IF(a_pType == nullptr);
    //    rti::c_type_e eType = a_pType->type();
    //    switch (eType)
    //    {
    //        case rti::type_e::Data:
    //        {
    //            rti::data_c const* const pDataType = rti::to_type<rti::data_c>(a_pType);
    //            EMP_RET_FALSE_IF(pDataType == nullptr);
    //            a_rsOutput.append_char(' ');
    //            a_rsOutput.append_string(pDataType->m_sName);
    //            break;
    //        }
    //        case rti::type_e::Function:
    //        {
    //            rti::function_c const* const pFunctionType = rti::to_type<rti::function_c>(a_pType);
    //            EMP_RET_FALSE_IF(pFunctionType == nullptr);
    //            EMP_RET_FALSE_IF_NOT(l_dump_vector(a_rsOutput, pFunctionType->m_vpReturns, false, a_lDumpSignature));
    //            EMP_RET_FALSE_IF_NOT(l_dump_vector(a_rsOutput, pFunctionType->m_vpParameters, a_bDefinition, a_lDumpSignature));
    //            break;
    //        }
    //        case rti::type_e::Void:
    //        case rti::type_e::none:
    //        default:
    //        { return false; }
    //    };

    //    return true;
    //};

    c_node_e eNode = rNode.m_eType;
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves != nullptr ? pvstLeaves->count() : 0;
    lexeme_c const* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;

    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Intrinsic:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                {
                    if (pLexeme->m_bCold == false || emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("main"))))
                    {
                        c_bool bVariant = eLexeme == lexeme_e::Variant;

                        //EMP_RET_FALSE_IF_NOT(eLexeme == lexeme_e::Function || bVariant);
                        //function_c const* const pFunction = to_function<function_c>(pLexeme);
                        //EMP_RET_FALSE_IF(pFunction == nullptr);
                        a_rsOutput.append_char('@');
                        a_rsOutput.append_string(pLexeme->m_sName);

                        rti::function_c const* const pFunctionType = rti::to_type<rti::function_c>(pLexeme->m_pType);
                        EMP_RET_FALSE_IF(pFunctionType == nullptr);


                        c_size stVariant = bVariant ? 1u : 0u;

                        if (bVariant)
                        {
                            a_rsOutput.append_char(' ');
                            c_size stVariantWeight = pvstLeaves->val(0);
                            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stVariantWeight));
                            lexeme_c* pVariantWeight = m_vNodes.ref(stVariantWeight).m_pLexeme;
                            EMP_RET_FALSE_IF(pVariantWeight == nullptr);
                            EMP_RET_FALSE_IF(pVariantWeight->m_eType != lexeme_e::Literal);
                            a_rsOutput.append_string(pVariantWeight->m_sName);
                        }

                        if (stCount == (3 + stVariant))
                        {
                            a_rsOutput.append_char('(');
                            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(0 + stVariant), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, true));
                            a_rsOutput.trim_right(2);
                            a_rsOutput.append_char(')');

                            a_rsOutput.append_char('(');
                            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(1 + stVariant), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, true));
                            a_rsOutput.trim_right(2);
                            a_rsOutput.append_char(')');
                        }
                        else if (stCount == (2 + stVariant))
                        {
                            a_rsOutput.append_char('(');
                            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(0 + stVariant), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, true));
                            a_rsOutput.trim_right(2);
                            a_rsOutput.append_char(')');
                        }
                        else if (stCount == (1 + stVariant))
                        { }
                        else
                        { EMP_RET_FALSE; }

                        a_rsOutput.append_char('=');
                        a_rsOutput.append_char('\n');

                        EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->last_val(), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
					}
                    else
                    {
                        a_rbEndScope = true;
                    }
                    return true;
                }
                case node_e::Call:
                {
                    a_rsOutput.append_string(pLexeme->m_sName);

                    if (stCount > 0)
                    {
                        a_rsOutput.append_char('(');
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(stIndex), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
                            a_rsOutput.append_char(',');
                            a_rsOutput.append_char(' ');
                        }
                        a_rsOutput.trim_right(2);
                        a_rsOutput.append_char(')');
                    }
                    return true;
                }
                case node_e::Declaration:
                case node_e::RealLiteral:
                case node_e::IntegralLiteral:
                case node_e::none:
                default:
                { EMP_RET_FALSE; }
            }
        }
        case lexeme_e::Punctuation:
        {
            punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
            EMP_RET_FALSE_IF(pPunctuation == nullptr);
            c_punctuation_e ePunctuation = pPunctuation->m_ePunctuationType;
            switch (ePunctuation)
            {
                case punctuation_e::LeftBrace:
                {
                    //l_print_tab(a_stStack);
                    a_rsOutput.append_string(pLexeme->m_sName);
                    a_rsOutput.append_char('\n');
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        l_print_tab(a_stStack + 1);
                        EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(stIndex), /*a_rmpNodes, */a_stStack + 1, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
                        if (a_rbEndScope == false)
                        { a_rsOutput.append_char(';'); }
                        a_rsOutput.append_char('\n');
                    }
                    l_print_tab(a_stStack);
                    a_rsOutput.append_char('}');
                    a_rbEndScope = true;
                    return true;
                }
                case punctuation_e::RightBrace:
                {
                    a_rsOutput.append_string(pLexeme->m_sName);
                    a_rsOutput.append_char('\n');
                    return true;
                }
                case punctuation_e::SemiColon:
                {
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        c_size stLeaf = pvstLeaves->val(stIndex);
                        node_c const& rLeaf = m_vNodes.ref(stLeaf);
                        auto const* const pLeafLex = rLeaf.m_pLexeme;
                        if (pLeafLex != nullptr && pLeafLex->m_eType == lexeme_e::Literal)
                        {
							EMP_LOG_ERROR("Dropping ill formated node");
							return true;
						}
                        EMP_RET_FALSE_IF_NOT(print_ose(stLeaf, a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
                        if (a_rbEndScope == false && a_rbEndDef == false)
                        { a_rsOutput.append_char(';'); }
                        if (a_rbEndDef == false)
                        { a_rsOutput.append_char('\n'); }
                    }
                    return true;
                }
                case punctuation_e::RightPar:
                {
                    if (a_bFunctionDef)
                    {
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(stIndex), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
                            a_rsOutput.append_char(',');
                            a_rsOutput.append_char(' ');
                        }
                        return true;
                    }
                    return false;
                }
                case punctuation_e::Assign:
                case punctuation_e::Space:
                case punctuation_e::Comma:
                case punctuation_e::LeftPar:
                case punctuation_e::Declare:
                case punctuation_e::none:
                default:
                { EMP_RET_FALSE; }
            };
        }
        case lexeme_e::Literal:
        {
            a_rsOutput.append_string(pLexeme->m_sName);
            return true;
        }
        case lexeme_e::Operation:
        {
            if (stCount > 0)
            {
                if (stCount == 1)
                { a_rsOutput.append_string(pLexeme->m_sName); }
                /*else
                {
                    a_rsOutput.append_char('(');
                }*/

                //else
                {
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(stIndex), /*a_rmpNodes, */a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
                        a_rsOutput.append_char(' ');
                        a_rsOutput.append_string(pLexeme->m_sName);
                        a_rsOutput.append_char(' ');
                    }
                    a_rsOutput.trim_right(pLexeme->m_sName.len() + 2);
                }

                /*if (stCount > 1)
                {
                    a_rsOutput.append_char(')');
                }*/
                return true;
            }
            return false;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::Declaration:
                {
					if (pLexeme->m_bCold == false)
					{
	                    a_rsOutput.append_char('@');
    	                a_rsOutput.append_string(pLexeme->m_sQualified);
        	            EMP_RET_FALSE_IF(stCount > 0);
					}
                    else
                    {
                        a_rbEndDef = true;
                    }

        	        return true;
                }
                case node_e::DefinitionU:
                {
					if (pLexeme->m_bCold == false)
					{
	                    a_rsOutput.append_char('@');
	                    a_rsOutput.append_string(pLexeme->m_sQualified);
	                    if (stCount > 0)
	                    {
	                        auto const* const pvstChildLeaves = m_vNodes.ref(pvstLeaves->val(0)).m_pvstLeaves;
	                        EMP_RET_FALSE_IF(pvstChildLeaves == nullptr || pvstChildLeaves->count() != 1);
	                        a_rsOutput.append_char('=');
	                        EMP_RET_FALSE_IF_NOT(print_ose(pvstChildLeaves->val(0), a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
	                    }
					}
                    else
                    {
                        a_rbEndDef = true;
                    }

                    return true;
                }
                case node_e::Call:
                {
                    a_rsOutput.append_string(pLexeme->m_sName);
                    return true;
                }
                case node_e::RealLiteral:
                case node_e::IntegralLiteral:
                case node_e::none:
                default:
                { EMP_RET_FALSE; }
            };
        }
        case lexeme_e::Control:
        {
            EMP_RET_FALSE_IF_NOT((stCount > 0));
            a_rsOutput.append_string(pLexeme->m_sName);
            a_rsOutput.append_char(' ');
            a_rsOutput.append_char('(');
            EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(0), /*a_rmpNodes, */a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
            a_rsOutput.append_char(')');
            a_rsOutput.append_char('\n');
            if (stCount > 1)
            {
                l_print_tab(a_stStack);
                EMP_RET_FALSE_IF_NOT(print_ose(pvstLeaves->val(1), /*a_rmpNodes, */a_stStack, a_rsOutput, a_rbEndScope, a_rbEndDef, false));
            }
            return true;
        }
        case lexeme_e::Comment:
        case lexeme_e::none:
        default:
        { EMP_RET_FALSE; }
    };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::print_ast(c_size a_stNode, c_size a_stStack, astring& a_rsBuffer) // @@0 recursive
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));

    for (size_t stChar = 0; stChar < a_stStack; ++stChar)
    { a_rsBuffer.append_char('-'); }
    a_rsBuffer.append_literal(ALITERAL(" ")); // @@0 string why not char ?

    node_c& rNode = m_vNodes.ref(a_stNode);
    auto const eType = rNode.m_eType;
    if (eType == node_e::DefinitionU ||
        eType == node_e::Declaration)
    { a_rsBuffer.append_char('@'); }

    lexeme_c const* const pLexeme = rNode.m_pLexeme;
    if (pLexeme != nullptr)
    { a_rsBuffer.append_string(pLexeme->m_sQualified); }
    else
    {
        a_rsBuffer.append_string(rNode.m_sName);
        if (rNode.m_eType == node_e::RealLiteral)
        { a_rsBuffer.append_string(ALITERAL(" float")); }
        else if (rNode.m_eType == node_e::IntegralLiteral)
        { a_rsBuffer.append_string(ALITERAL(" int")); }
    }

    a_rsBuffer.append_char('\n');

    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    if (pvstLeaves != nullptr)
    {
        c_size stCount = pvstLeaves->count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        { EMP_RET_FALSE_IF_NOT(print_ast(pvstLeaves->val(stIndex), a_stStack + 1, a_rsBuffer)); }
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool print_asm(program_c const& a_rProgram, astring& a_rsBuffer, c_bool a_bFull, vector<astring>& a_rvsInstructions)
{
    auto const& l_add_instruction = [](vector<astring>& l_rvsInstructions, c_astring& l_rsInstruction)
    {
        bool bFound = false;
        for (size_t stIndex = 0; stIndex < l_rvsInstructions.count(); ++stIndex)
        {
            if (str::cmp(l_rvsInstructions.ref(stIndex), l_rsInstruction))
            {
                bFound = true;
                break;
            }
        }
        if (bFound == false)
        {
            l_rvsInstructions.add(l_rsInstruction);
        }
    };

    memory_walker_c oWalker(a_rProgram.m_Instructions);
    a_rsBuffer.append_char('\n');

    c_size stCount = a_rProgram.exit_point();

    while (oWalker.pointer() < stCount)
    {
        c_size stPointer = 0;//a_bFull ? oWalker.pointer() : 0;

        char cInstruction = 0;
        EMP_RET_FALSE_IF_NOT(oWalker.next(cInstruction));

        c_instruction_e eInstruction =
            static_cast<instruction_e>(cInstruction);

        switch (eInstruction)
        {
            case instruction_e::Function:
            {
                ui32 ui32Instruction = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32Instruction));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : Function : #1 : '#2' @#3\n",
                        INT_FMT_0H4(stPointer),
                        0,//a_rProgram.get_index_from_address(ui32Instruction),
                        a_rProgram.get_name_from_address(ui32Instruction),
                        INT_FMT_0H4(ui32Instruction)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("Function\n")));   
                }
                break;
            }
            case instruction_e::Variant:
            {
                ui32 ui32Instruction = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32Instruction));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : Variant : #1 : '#2'\n",
                        INT_FMT_0H4(stPointer),
                        0,//ui32Instruction,
                        a_rProgram.m_Table.get_name_from_index(ui32Instruction)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("Function\n")));
                }
                break;
            }
            case instruction_e::PushScope:
            {
                ui32 ui32PopAddress = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32PopAddress));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : PushScope : @#1\n", INT_FMT_0H4(stPointer), INT_FMT_0H4(ui32PopAddress)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("PushScope\n")));
                } 
                break;
            }
#define XTMP_PRINT_ASM(x_Instruction)\
            case instruction_e::x_Instruction:\
            {\
                if (a_bFull)\
                { a_rsBuffer.append_string(STR_AFORMAT("@#0 : #1\n", INT_FMT_0H4(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)))); }\
                else\
                { a_rsBuffer.append_string(STR_AFORMAT("#0\n", ALITERAL(EMP_PP_STRING(x_Instruction)))); }\
                break;\
            }
            XTMP_PRINT_ASM(PopScope)
            XTMP_PRINT_ASM(Return)
            case instruction_e::Enter:
            {
                ui32 ui32ParameterSize = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32ParameterSize));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : Enter : #1\n", INT_FMT_0H4(stPointer), ui32ParameterSize));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("Enter\n")));
                }
                break;
            }
            case instruction_e::Jump:
            {
                ui32 ui32Jump = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32Jump));

                if (a_bFull)
                {   
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : Jump : @#1\n", INT_FMT_0H4(stPointer), INT_FMT_0H4(ui32Jump)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("Jump\n")));
                }
                break;
            }
            case instruction_e::JumpCmp_SI32:
            {
                ui32 ui32End = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32End));
                l_add_instruction(a_rvsInstructions, astring(ALITERAL("loop")));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : JumpCmp_SI32 : @#1\n",INT_FMT_0H4(stPointer), INT_FMT_0H4(ui32End)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("JumpCmp\n")));
                    //a_rsBuffer.append_string(STR_AFORMAT("JumpCmp : @#0\n", INT_FMT_0H4(ui32End)));
                }
                break;
            }
            case instruction_e::JumpCmp_UI32:
            {
                ui32 ui32End = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32End));
                    l_add_instruction(a_rvsInstructions, astring(ALITERAL("loop")));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : JumpCmp_UI32 : @#1\n", INT_FMT_0H4(stPointer), INT_FMT_0H4(ui32End)));
                }
                else
                {
                    a_rsBuffer.append_string(astring(ALITERAL("JumpCmp\n")));
                    //a_rsBuffer.append_string(STR_AFORMAT("JumpCmp : @#0\n", INT_FMT_0H4(ui32End)));
                }
                break;
            }
            case instruction_e::JumpCmp_Bool:
            {
                ui32 ui32End = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32End));
                l_add_instruction(a_rvsInstructions, astring(ALITERAL("if")));
                if (a_bFull)
                {
                    a_rsBuffer.append_string(STR_AFORMAT("@#0 : JumpCmp_Bool : @#1\n", INT_FMT_0H4(stPointer), INT_FMT_0H4(ui32End)));
                }
                else
                {
                    //a_rsBuffer.append_string(STR_AFORMAT("JumpCmp : @#0\n", INT_FMT_0H4(ui32End)));
                    a_rsBuffer.append_string(astring(ALITERAL("JumpCmp\n")));
                }
                break;
            }
            #define XTMP_PRINT_ASM_2(x_Instruction, x_Name)\
            case instruction_e::x_Instruction:\
            {\
                l_add_instruction(a_rvsInstructions, astring(ALITERAL(x_Name)));\
                if (a_bFull)\
                { a_rsBuffer.append_string(STR_AFORMAT("@#0 : #1\n", INT_FMT_0H4(stPointer), astring(ALITERAL(x_Name)))); }\
                else\
                { a_rsBuffer.append_string(astring(ALITERAL("Intrinsic\n"))); }\
                break;\
            }

            XTMP_PRINT_ASM_2(Equal_I32, "equal")
            XTMP_PRINT_ASM_2(More_F32, "more")
            XTMP_PRINT_ASM_2(More_I32, "more")
            XTMP_PRINT_ASM_2(MoreEq_F32, "moreeq")
            XTMP_PRINT_ASM_2(MoreEq_I32, "moreeq")
            XTMP_PRINT_ASM_2(Less_F32, "less")
            XTMP_PRINT_ASM_2(Less_I32, "less")
            XTMP_PRINT_ASM_2(LessEq_F32, "lesseq")
            XTMP_PRINT_ASM_2(LessEq_I32, "lesseq")
            XTMP_PRINT_ASM_2(Add_F32, "add")
            XTMP_PRINT_ASM_2(Add_U32, "add")
            XTMP_PRINT_ASM_2(Add_I32, "add")
            XTMP_PRINT_ASM_2(Sub_F32, "sub")
            XTMP_PRINT_ASM_2(Sub_I32, "sub")
            XTMP_PRINT_ASM_2(Mul_F32, "mul")
            XTMP_PRINT_ASM_2(Mul_I32, "mul")
            XTMP_PRINT_ASM_2(Div_F32, "div")
            XTMP_PRINT_ASM_2(Div_I32, "div")
            XTMP_PRINT_ASM_2(Mod_I32, "mod")
            XTMP_PRINT_ASM_2(Neg_F32, "neg")
            XTMP_PRINT_ASM_2(Neg_I32, "neg")
            XTMP_PRINT_ASM_2(Pos_F32, "pos")
            XTMP_PRINT_ASM_2(Pos_I32, "pos")
            XTMP_PRINT_ASM_2(Abs_F32, "abs")
            XTMP_PRINT_ASM_2(Sqrt_F32, "sqrt")
            XTMP_PRINT_ASM_2(Abs_I32, "abs")
            XTMP_PRINT_ASM_2(CosD_F32, "cos")
            XTMP_PRINT_ASM_2(SinD_F32, "sin")
            XTMP_PRINT_ASM_2(Cos_F32, "cos")
            XTMP_PRINT_ASM_2(Sin_F32, "sin")
            XTMP_PRINT_ASM_2(Print_I32, "print")
            XTMP_PRINT_ASM_2(Randf_F32, "rand")
#undef XTMP_PRINT_ASM
#define XTMP_PRINT_PUSH(x_Instruction, x_Type)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tValue = 0;\
                EMP_RET_FALSE_IF_NOT(oWalker.next(tValue));\
                if (a_bFull)\
                { a_rsBuffer.append_string(STR_AFORMAT("@#0 : #1 : #2\n", INT_FMT_0H4(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)), tValue)); }\
                else\
                { a_rsBuffer.append_string(astring(ALITERAL("PushData\n"))); }\
                break;\
            }
            // @@0 literal
            XTMP_PRINT_PUSH(PushReal, real)
            XTMP_PRINT_PUSH(PushSInt, sint)
            XTMP_PRINT_PUSH(PushUInt, uint)
            XTMP_PRINT_PUSH(PushBool, bool)
#undef XTMP_PRINT_PUSH
#define XTMP_PRINT_LOAD(x_Instruction)\
            case instruction_e::x_Instruction:\
            {\
                ui32 u32Address = 0;\
                EMP_RET_FALSE_IF_NOT(oWalker.next(u32Address));\
                if (a_bFull)\
                { a_rsBuffer.append_string(STR_AFORMAT("@#0 : #1 : #2\n", INT_FMT_0H4(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)), u32Address)); }\
                else\
                { a_rsBuffer.append_string(astring(ALITERAL("LoadData\n"))); }\
                break;\
            }
            // @@0 literal
            XTMP_PRINT_LOAD(LoadAbs_F32)
            XTMP_PRINT_LOAD(LoadAbs_U32)
            XTMP_PRINT_LOAD(LoadAbs_S32)
            XTMP_PRINT_LOAD(LoadAbs_U8)
            XTMP_PRINT_LOAD(LoadRel_F32)
            XTMP_PRINT_LOAD(LoadRel_U32)
            XTMP_PRINT_LOAD(LoadRel_S32)
            XTMP_PRINT_LOAD(LoadRel_U8)
#undef XTMP_PRINT_LOAD
            case instruction_e::Reserved:
            { EMP_RET_FALSE_("@#0 : RESERVED INSTRUCTION #1", INT_FMT_0H4(stPointer), enum_to_astring(eInstruction)); }
            case instruction_e::none:
            case instruction_e::Invalid:
            { EMP_RET_FALSE_("@#0 : INVALID INSTRUCTION Invalid\n", INT_FMT_0H4(stPointer)); }
            case instruction_e::Extension:
            default:
            {
                if (eInstruction >= instruction_e::Extension)
                {
                    c_astring& sName = a_rProgram.m_Table.get_name_from_instruction(eInstruction);
                    
            // XTMP_GUI_REG(square       , "square"    , "()()")
            // XTMP_GUI_REG(triangle     , "triangle"  , "()()")
            // XTMP_GUI_REG(circle       , "circle"    , "()()")
            // XTMP_GUI_REG(hsba         , "hsba"      , "()(float,float,float,float,)")
            // XTMP_GUI_REG(translate    , "t"         , "()(float,)")
            // XTMP_GUI_REG(rotate       , "r"         , "()(float,)")
            // XTMP_GUI_REG(scale        , "s"         , "()(float,)")
            // XTMP_GUI_REG(flip         , "f"         , "()(float,)")

                    
                    if (str::cmp(astring(ALITERAL("hsba")), sName))
                    {
                        l_add_instruction(a_rvsInstructions, astring(ALITERAL("color")));
                    }
                    else if (str::cmp(astring(ALITERAL("t")), sName))
                    {
                        l_add_instruction(a_rvsInstructions, astring(ALITERAL("translate")));
                    }
                    else if (str::cmp(astring(ALITERAL("r")), sName))
                    {
                        l_add_instruction(a_rvsInstructions, astring(ALITERAL("rotate")));
                    }
                    else if (str::cmp(astring(ALITERAL("s")), sName))
                    {
                        l_add_instruction(a_rvsInstructions, astring(ALITERAL("scale")));
                    }
                    else if (str::cmp(astring(ALITERAL("f")), sName))
                    {
                        l_add_instruction(a_rvsInstructions, astring(ALITERAL("flip")));
                    }
                    else
                    {
                        l_add_instruction(a_rvsInstructions, sName);
                    }


                    EMP_RET_FALSE_IF_NOT((sName.len() > 0));

                    c_size stExtension = enum_to__(instruction_e::Extension);
                    c_size stInstruction = enum_to(eInstruction);
                    EMP_RET_FALSE_IF_NOT((stInstruction >= stExtension));

                    if (a_bFull)
                    {
                        a_rsBuffer.append_string(STR_AFORMAT("@#0 : Extension, '#1'\n", INT_FMT_0H4(stPointer), /*static_cast<ui32>(stInstruction - stExtension), */sName));
                    }
                    else
                    {
                        a_rsBuffer.append_string(astring(ALITERAL("Extension\n"))); //STR_AFORMAT("Extension : '#0'\n",  /*static_cast<ui32>(stInstruction - stExtension), */sName));
                    }
                    break;
                }

                EMP_RET_FALSE_("@#0 : UNHANDLED INSTRUCTION #1\n", INT_FMT_0H4(stPointer), static_cast<ui32>(eInstruction));
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_UNIT_TEST
EMP_RETURN bool compiler_c::unit_test_leak(void)
{
    EMP_RET_FALSE_IF(m_vpvLexemes.count() != m_vstFreeLexemes.count(),
                     "lexeme leak #0 / #1", m_vpvLexemes.count(), m_vstFreeLexemes.count());

    EMP_RET_FALSE_IF(m_vNodes.count() != (m_vstFreeNodes.count() + 1),
                     "node leak #0 / #1", m_vNodes.count(), (m_vstFreeNodes.count() + 1));

    EMP_RET_FALSE_IF(m_vpvstLeaves.count() != m_vstFreeLeaves.count(),
                     "leaves leak #0 / #1", m_vpvstLeaves.count(), m_vstFreeLeaves.count());

    EMP_RET_FALSE_IF(m_vpPrograms.count() != m_vstFreePrograms.count(),
                     "leaves leak #0 / #1", m_vpPrograms.count(), m_vstFreePrograms.count());
     return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::unit_test(c_astring& a_sResourcePath, c_bool a_bKill, c_bool a_bLog, c_astring& a_sUnitFile)
{
    {
        auto const l_unit_test_sources = [this, a_bKill, a_bLog](auto& a_rvTests, c_astring& a_sType) EMP_RETURN -> bool
        {
            size_t stFailed = 0;
            c_size stCount = a_rvTests.count();
            astring sName(ALITERAL("test"));
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                auto const l_continue = [this, a_bKill, a_bLog, stIndex, &a_rvTests, &a_sType]() EMP_RETURN -> bool
                {
                    auto& rTest = a_rvTests.ref(stIndex);
                    astring sTest = rTest.source;
                    astring sVar(STR_AFORMAT("test #0", a_sType));
                    if (a_bLog)
                    { EMP_LOG_INFO("source test #0 == #1", sTest, rTest.result); }
                    astring sSource = STR_AFORMAT("@#0 = #1;@main={}", sVar, sTest);
                    size_t stMutation = 0;
                    size_t stCrossover = 0;
                    c_size stProgram = from_string(sSource, emp::tt::max<size_t>::value, mutation_s(), stMutation, crossover_s(), stCrossover, false, emp::tt::max<size_t>::value);
                    EMP_RET_FALSE_IF(emp::tt::is_max(stProgram));

                    bool bDealloc = true;
                    auto const oFinally = emp::tt::finally([stProgram, &bDealloc, this]()
                    {
                        bDealloc = dealloc_program(stProgram);
                        if (bDealloc)
                        { bDealloc = unit_test_leak(); }
                    });
                    error_e const eResult = push_data(*m_vpPrograms.val(stProgram), sVar);
                    EMP_RET_FALSE_IF_NOT(eResult == error_e::none);
                    decltype(rTest.result) tResult;
                    EMP_RET_FALSE_IF_NOT(m_pStack->val(0, tResult));
                    if (rTest.success == false)
                    { return bDealloc; }
                    if (tResult != rTest.result)
                    {
                        EMP_LOG_ERROR("source test failed : '#0', expected == '#1' got  != '#2'", sTest, rTest.result, tResult);
                        if (a_bKill) { return false; }
                    }

                    return bDealloc;
                };

                if (l_continue() !=  a_rvTests.ref(stIndex).success)
                {
                    ++stFailed;
                    if (a_bKill) { break; }
                }
            }

            if (stFailed != 0)
            { EMP_LOG_ERROR("source test failed #0 / #1", stFailed, stCount); }
            else
            { EMP_LOG_INFO("source test success #0 / #1", stCount, stCount); }
            return stCount > 0 && stFailed == 0;
        };

        {
            struct test_sint_s
            {
                bool success = false;
                si32 result = 0;
                astring source{};
            };

#define XTMP_TEST(x_Test)\
vTests.add({true, x_Test, ALITERAL(EMP_PP_STRING(x_Test))});

            {
                auto const eDisableLog = EMP_LOGGER.m_eDisableLog;
                EMP_LOGGER.m_eDisableLog = log::log_e::Error;
                auto const oFinally = emp::tt::finally([eDisableLog](){ EMP_LOGGER.m_eDisableLog = eDisableLog; });

                vector<test_sint_s> vTests;
                vTests.add({ false,  0, ALITERAL("++1")});
                vTests.add({ false,  0, ALITERAL("1++")});
                vTests.add({ false,  0, ALITERAL("--1")});
                vTests.add({ false,  0, ALITERAL("1--")});
                vTests.add({ false,  0, ALITERAL("1++2")});
                vTests.add({ false,  0, ALITERAL("1--2")});
                if (l_unit_test_sources(vTests, ALITERAL("uint")) == false)
                {
                    EMP_LOG_ERROR("source test failed...");
                    if (a_bKill) { return false; }
                }
            }

            {
                vector<test_sint_s> vTests;
                XTMP_TEST(2+2)
                XTMP_TEST(+(2)+-(-2))
                if (l_unit_test_sources(vTests, ALITERAL("uint")) == false)
                {
                    EMP_LOG_ERROR("source test failed...");
                    if (a_bKill) { return false; }
                }
            }

            {
                vector<test_sint_s> vTests;
                XTMP_TEST(2-2)
                XTMP_TEST(-2-2)
                XTMP_TEST(-2+2)
                XTMP_TEST(-1+-2)
                if (l_unit_test_sources(vTests, ALITERAL("int")) == false)
                {
                    EMP_LOG_ERROR("source test failed...");
                    if (a_bKill) { return false; }
                }
            }

#undef XTMP_TEST

        }
    }

    {
        auto const l_unit_test_files = [a_bKill, a_bLog, &a_sUnitFile, &a_sResourcePath, this](c_astring& l_sExtension, c_astring& a_sError, auto const a_lUnitTest) EMP_RETURN -> bool
        {
            EMP_LOG_INFO("unit test : #0", a_sError);
            auto const l_create_program = [this](c_astring& a_rsFile) EMP_RETURN -> size_t
            {
                emp::fs::file_c oFile;
                EMP_RET_MAX_IF_NOT(size_t, oFile.create(a_rsFile, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open file '#0'", a_rsFile);

                astring sBuffer;
                EMP_RET_MAX_IF_NOT(size_t, oFile.read_all(sBuffer), "failed to read file '#0'", a_rsFile);
                EMP_RET_MAX_IF(size_t, sBuffer.is_empty(), "empty file '#0'", a_rsFile);
                clear();

                size_t stProgram = alloc_program();
                EMP_RET_MAX_IF_NOT(size_t, m_vpPrograms.is_valid_index(stProgram));

                auto* const pProgram = m_vpPrograms.val(stProgram);
                EMP_RET_MAX_IF_NOT(size_t, pProgram->m_Table.clone(m_Table));

                pProgram->m_sExpression = sBuffer;
                m_Context.create(sBuffer);

                return stProgram;
            };

            auto const l_test_file = [&a_sResourcePath](c_astring& a_rsTestFile, c_astring ll_sExtension, c_astring& a_sNewBuffer) EMP_RETURN -> bool
            {
                astring sDirectory(a_sResourcePath);
                EMP_RET_FALSE_IF_NOT(sDirectory.len() >= 6);
                sDirectory.trim_right(6);

                bool bExists = false;
                astring sOldDir(STR_AFORMAT("#0_obj/#1/", sDirectory, ll_sExtension));
                EMP_RET_FALSE_IF_NOT(emp::fs::create_directories(sOldDir));

                astring sOldPath(STR_AFORMAT("#0#1", sOldDir, a_rsTestFile));
                EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(sOldPath, bExists));
                if (bExists)
                {
                    astring sOldBuffer;

                    {
                        emp::fs::file_c oFile;
                        EMP_RET_FALSE_IF_NOT(oFile.create(sOldPath, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open file '#0'", sOldPath);
                        EMP_RET_FALSE_IF_NOT(oFile.read_all(sOldBuffer), "failed to read file '#0'", sOldPath);
                        EMP_RET_FALSE_IF(sOldBuffer.is_empty(), "empty file '#0'", sOldPath);
                    }

                    if (emp::str::cmp(a_sNewBuffer, sOldBuffer) == false)
                    {
                        emp::fs::file_c oFile;
                        EMP_RET_FALSE_IF_NOT(oFile.create(sOldPath, emp::fs::file_e::Binary, fs::create_e::Trunc));
                        EMP_RET_FALSE_IF_NOT(oFile.write(a_sNewBuffer));
                        EMP_RET_FALSE_("Buffer mismatch");
                    }
                    return true;
                }
                else
                {
                    emp::fs::file_c oFile;
                    EMP_RET_FALSE_IF_NOT(oFile.create(sOldPath, emp::fs::file_e::Binary, fs::create_e::New));
                    EMP_RET_FALSE_IF_NOT(oFile.write(a_sNewBuffer));
                    return true;
                    // return false; // @@0 uncomment
                }
            };

            vector<astring> vsFiles;
            astring sDirectory = STR_AFORMAT("#0ose/", a_sResourcePath);
            EMP_RET_FALSE_IF_NOT(emp::fs::list(sDirectory, ALITERAL("*.ose"), vsFiles, false));

            c_size stCount = vsFiles.count();

            size_t stFailed = 0;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                if (a_sUnitFile.len() > 0 && stIndex > 0)
                { break; }

                c_astring sFilename = a_sUnitFile.len() > 0 ? a_sUnitFile : vsFiles.ref(stIndex);
                astring sTestFile(sFilename);
                EMP_RET_FALSE_IF_NOT(sTestFile.len() >= 5);
                sTestFile.trim_right(3);
                sTestFile.append_string(l_sExtension);

                c_astring sFile(STR_AFORMAT("#0#1", sDirectory, sFilename));

                if (a_bLog)
                { EMP_LOG_INFO("#0 #1/#2 : #3", a_sError, stIndex + 1, stCount, sTestFile); }

                c_size stProgram = l_create_program(sFile);

                auto const l_continue = [&l_test_file, &a_lUnitTest, this, stProgram, &sFile, &a_sError, &sTestFile, &l_sExtension]() -> bool
                {
                    EMP_RET_FALSE_IF(m_vpPrograms.is_valid_index(stProgram) == false || m_vpPrograms.val(stProgram) == nullptr,
                                     "unit test failed #0 : program creation failed", sFile);

                    astring sNew;

                    bool bDealloc = true;
                    auto const oFinally = emp::tt::finally([stProgram, &bDealloc, this]()
                    {
                        bDealloc = dealloc_program(stProgram);
                        if (bDealloc)
                        { bDealloc = unit_test_leak(); }
                    });

                    EMP_RET_FALSE_IF_NOT(a_lUnitTest(*m_vpPrograms.val(stProgram), sNew),
                                         "unit test failed #0 : #1", sFile, a_sError);

                    EMP_RET_FALSE_IF_NOT(l_test_file(sTestFile, l_sExtension, sNew),
                                         "unit test failed #0 : #1", sFile, a_sError);
                    return bDealloc;
                };
                if (l_continue() == false)
                {
                    ++stFailed;
                    if (a_bKill) { break; }
                }
            }

            if (stFailed != 0)
            { EMP_LOG_ERROR("unit test failed #0 : #1 / #2", a_sError, stFailed, stCount); }
            else
            { EMP_LOG_INFO("unit test success #0 : #1 / #2", a_sError, stCount, stCount); }
            return stCount > 0 && stFailed == 0;
        };

        if (false == l_unit_test_files(ALITERAL("tok0"), ALITERAL("token"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                       {
                                           EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                           EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                           print_tokens(m_vTokens, a_rsNew);
                                           return true;
                                       }))
        {
            EMP_LOG_ERROR("tok0 token test failed...");
            if (a_bKill) { return false; }
        }

        if (false == l_unit_test_files(ALITERAL("tok1"), ALITERAL("arity"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                       {
                                           EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                           EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                           EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                           print_tokens(m_vTokens, a_rsNew);
                                           return true;
                                       }))
        {
            EMP_LOG_ERROR("tok1 arity test failed...");
            if (a_bKill) { return false; }
        }


        if (false == l_unit_test_files(ALITERAL("ast0"), ALITERAL("syntax"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                       {
                                           EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                           EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                           EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                           a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                           EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                           EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                           return true;
                                       }))
        {
            EMP_LOG_ERROR("ast0 syntax test failed...");
            if (a_bKill) { return false; }
        }


       if (false == l_unit_test_files(ALITERAL("ast1"), ALITERAL("resolve unaries"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                 {
                                     EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                     EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                     EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                     a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                     EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     EMP_RET_FALSE_IF_NOT(resolve_literals(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     return true;
                                 }))
        {
            EMP_LOG_ERROR("ast1 resolve unaries test failed...");
            if (a_bKill) { return false; }
        }

       if (false == l_unit_test_files(ALITERAL("ast2"), ALITERAL("resolve literals"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                 {
                                     EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                     EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                     EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                     a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                     EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     EMP_RET_FALSE_IF_NOT(resolve_literals(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_unaries(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     return true;
                                 }))
        {
            EMP_LOG_ERROR("ast2 resolve literals test failed...");
            if (a_bKill) { return false; }
        }



        if (false == l_unit_test_files(ALITERAL("ast3"), ALITERAL("resolve calls"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                 {
                                     EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                     EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                     EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                     a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                     EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     EMP_RET_FALSE_IF_NOT(resolve_literals(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_unaries(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_calls(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(print_ast(a_rProgram.m_stAst, 0, a_rsNew));
                                     return true;
                                 }))
        {
            EMP_LOG_ERROR("ast3 resolve calls test failed...");
            if (a_bKill) { return false; }
        }

        if (false == l_unit_test_files(ALITERAL("ose0"), ALITERAL("disast"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                 {
                                     EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                     EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                     EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                     a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                     EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                     EMP_RET_FALSE_IF_NOT(resolve_literals(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_unaries(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_calls(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(print_ose(a_rProgram.m_stAst, a_rsNew));
                                     return true;
                                 }))
        {
            EMP_LOG_ERROR("ose0 disast test failed...");
            if (a_bKill) { return false; }
        }

        if (false == l_unit_test_files(ALITERAL("asm0"), ALITERAL("link"), [this](program_c& a_rProgram, astring& a_rsNew) EMP_RETURN -> bool
                                 {
                                     EMP_RET_FALSE_IF_NOT(build_tokens(a_rProgram));
                                     EMP_RET_FALSE_IF_NOT(m_Context.end_of_context());
                                     EMP_RET_FALSE_IF_NOT(clean_tokens(a_rProgram));
                                     a_rProgram.m_stAst = build_ast(a_rProgram, token_chc(m_vTokens), 0, a_rProgram.m_Table, 0);
                                     EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_rProgram.m_stAst));
                                     EMP_RET_FALSE_IF_NOT(resolve_literals(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_unaries(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(resolve_calls(a_rProgram, a_rProgram.m_stAst, a_rProgram.m_Table));
                                     EMP_RET_FALSE_IF_NOT(build_asm(m_vNodes.ref(a_rProgram.m_stAst).m_pvstLeaves, a_rProgram, a_rProgram.m_Table, false));
                                     EMP_RET_FALSE_IF_NOT(a_rProgram.link(false));
                                     vector<astring> vsInstructions;
                                     EMP_RET_FALSE_IF_NOT(print_asm(a_rProgram, a_rsNew, true, vsInstructions));
                                     return true;
                                 }))
        {
            EMP_LOG_ERROR("asm0 link test failed...");
            if (a_bKill) { return false; }
        }
    }

    return true;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::merge_ast(program_c& a_rMother, program_c* const a_pFather)
{
    m_vstMergeNodes.resize(0);

    EMP_RET_FALSE_IF(&a_rMother == a_pFather);
    //a_rMother.m_Table.cold(false);
    if (a_pFather != nullptr)
    {
        a_rMother.m_Table.mother(true);
        a_pFather->m_Table.mother(false);

        //a_pFather->m_Table.cold(false);

        {
            auto* pvstFatherLeaves = m_vNodes.ref(a_pFather->m_stAst).m_pvstLeaves;
            if (pvstFatherLeaves != nullptr)
            {
                c_size stCount = pvstFatherLeaves->count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { EMP_RET_FALSE_IF_NOT(add_child_node(a_rMother.m_stAst, pvstFatherLeaves->val(stIndex))); }
                pvstFatherLeaves->resize(0);
            }
        }

        {
            auto const& rvstFatherNodes = *a_pFather->m_pvstNodes;
            c_size stCount = rvstFatherNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stNode = rvstFatherNodes.val(stIndex);
                if (emp::tt::is_not_max(stNode) && stNode != a_pFather->m_stAst)
                {
                    node_c& rNode = m_vNodes.ref(stNode);
                    rNode.m_bSource = 1;
                    add_program_node(a_rMother, stNode);
                }
            }
        }

        {
            auto& rNode = m_vNodes.ref(a_pFather->m_stAst);
            if (rNode.m_pvstLeaves != nullptr)
            { rNode.m_pvstLeaves->resize(0); }
            a_pFather->m_pvstNodes->resize(0);
            a_pFather->m_pvstNodes->add(a_pFather->m_stAst);
        }

        EMP_RET_FALSE_IF_NOT(a_rMother.m_Table.merge(a_pFather->m_Table));

        {
            auto const& rvstMotherNodes = *a_rMother.m_pvstNodes;
            c_size stCount = rvstMotherNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stNode = rvstMotherNodes.val(stIndex);
                if (emp::tt::is_not_max(stNode))
                {
                    node_c& rNode = m_vNodes.ref(stNode);
                    rNode.m_pLexeme = a_rMother.m_Table.get_lexeme(rNode.m_pLexeme->m_sQualified);
                }
            }
        }
    }

    return true;
}

EMP_RETURN bool compiler_c::mutate_ast(program_c& a_rProgram, table_c& a_rTable, mutation_s const& a_rMutation, crossover_s const& a_rCross EMP_UNUSED,
    size_t& a_rstCross, size_t& a_rstMutation)
{
    c_size stNodeCount = m_vstMergeNodes.count();

    a_rstCross = 0;
    a_rstMutation = 0;
    c_size stTrailingCount = m_pRandom->rand<ui32>(0, a_rMutation.trailing.m_ui32Count);

    auto const l_mutate_literal = [this, &a_rTable](c_size a_stNode, c_node_mutation_s& a_rNodeMutation) EMP_RETURN -> bool
    {
        {
            literal_t<float>* const pLiteral = to_literal<float>(m_vNodes.ref(a_stNode).m_pLexeme);
            if (pLiteral != nullptr)
            {
                c_float fOld = pLiteral->m_tLiteral;
                c_float fNew = fOld + m_pRandom->rand<float>(0, fOld) *
                    (m_pRandom->rand<float>(-a_rNodeMutation.m_fScale, a_rNodeMutation.m_fScale) +
                     m_pRandom->rand<float>(-a_rNodeMutation.m_fOffset, a_rNodeMutation.m_fOffset));
                m_vNodes.ref(a_stNode).m_pLexeme = a_rTable.reg_literal<float>(fNew);
                //EMP_LOG_INFO("mutating literal<float> '#0' -> '#1'", fOld, pLiteral->m_tLiteral);
                return true;
            }
        }

        {
            literal_t<int>* const pLiteral = to_literal<int>(m_vNodes.ref(a_stNode).m_pLexeme);
            if (pLiteral != nullptr)
            {
                float fValue = 0;
                EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(pLiteral->m_tLiteral, fValue));
                c_float fLiteral = m_pRandom->rand<float>(0, fValue) *
                (m_pRandom->rand<float>(-a_rNodeMutation.m_fScale, a_rNodeMutation.m_fScale) +
                 m_pRandom->rand<float>(-a_rNodeMutation.m_fOffset, a_rNodeMutation.m_fOffset));

                int siLiteral = 0;
                if (emp::ast::try_cast(fLiteral, siLiteral))
                {
                    int const siOld = pLiteral->m_tLiteral;
                    int const siNew = siOld + siLiteral;
                    m_vNodes.ref(a_stNode).m_pLexeme = a_rTable.reg_literal<int>(siNew);
                   // EMP_LOG_INFO("mutating literal<int> '#0' -> '#1'", siOld, pLiteral->m_tLiteral);
                }
                return true;
            }
        }

        return false;
    };

    auto const l_mutate_variant = [&a_rTable, this](c_size a_stNode, c_node_mutation_s& a_rNodeMutation) EMP_RETURN -> bool
    {
        variant_c* const pVariant = to_function<variant_c>(m_vNodes.ref(a_stNode).m_pLexeme);

        size_t EMP_TT_MAX_VAR(stVariant);
        {
            c_size stCount = m_vstMutateNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stNode = m_vstMutateNodes.val(stIndex);
                if (m_vNodes.ref(stNode).m_pLexeme == pVariant)
                { stVariant = emp::tt::is_max(stVariant) ? 0 : (stVariant + 1); }
                if (stNode == a_stNode)
                { break; }
            }
        }

        c_size stSlotCount = pVariant->m_vSlots.count();

        EMP_ASSERT(emp::tt::is_not_max(stVariant));
        EMP_ASSERT(stVariant < stSlotCount);
        if (emp::tt::is_not_max(stVariant) &&
            stVariant < stSlotCount)
        {
            c_float fOld = pVariant->m_vSlots.ref(stVariant).weight;
            c_float fNew = m_pRandom->rand<float>(0, fOld) *
                (m_pRandom->rand<float>(-a_rNodeMutation.m_fScale, a_rNodeMutation.m_fScale) +
                 m_pRandom->rand<float>(-a_rNodeMutation.m_fOffset, a_rNodeMutation.m_fOffset));
            pVariant->m_vSlots.ref(stVariant).weight = fNew;

            node_c& rNode = m_vNodes.ref(a_stNode);
            EMP_RET_FALSE_IF(rNode.m_pvstLeaves == nullptr);
            m_vNodes.ref(rNode.m_pvstLeaves->val(0)).m_pLexeme = a_rTable.reg_literal<float>(fNew);

            pVariant->m_fSum = 0;
            for (size_t stIndex = 0; stIndex < stSlotCount; ++stIndex)
            { pVariant->m_fSum += pVariant->m_vSlots.ref(stIndex).weight; }

            return true;
        }

        return false;
    };

    auto const l_mutate_call = [this, &a_rTable](c_size a_stNode, c_node_mutation_s& EMP_SILENT(a_rNodeMutation)) EMP_RETURN -> bool
    {
        node_c& rNode = m_vNodes.ref(a_stNode);
        m_vpTmpLexemes.resize(0);
        auto const& rvpLexemes = a_rTable.lexemes().vector();
        c_size stCount = rvpLexemes.count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            lexeme_c* const pLexeme = rvpLexemes.val(stIndex);
            if (pLexeme->m_eType == rNode.m_pLexeme->m_eType &&
                pLexeme != rNode.m_pLexeme &&
                pLexeme->m_bCold == false)
            {
                c_lexeme_e eLexeme = pLexeme->m_eType;
                if (eLexeme == lexeme_e::Function ||
                    eLexeme == lexeme_e::Variant ||
                    eLexeme == lexeme_e::Intrinsic ||
                    eLexeme == lexeme_e::Operation)
                {
                    if (emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("main"))) == false)
                    { m_vpTmpLexemes.add(pLexeme); }
                }
            }
        }
        if (m_vpTmpLexemes.count() > 0)
        {
            c_size stIndex = m_pRandom->rand<size_t>(0, m_vpTmpLexemes.count());
            rNode.m_pLexeme = m_vpTmpLexemes.val(stIndex);
            return true;
        }

        return false;
    };

    auto const l_mutate_data = [](c_size EMP_SILENT(a_stNode), c_node_mutation_s& EMP_SILENT(a_rNodeMutation)) EMP_RETURN -> bool
    {
        return false;
    };

    auto const l_mutate_nodes = [this](node_mutation_s const& a_rNodeMutation, auto const& a_rlMutate) EMP_RETURN -> size_t
    {
        ui32 ui32Count = a_rNodeMutation.m_ui32Base + m_pRandom->rand<ui32>(0, a_rNodeMutation.m_ui32Count + 1);
        ui32 ui32Left = ui32Count;
        c_size stCount = m_vstMutateNodes.count();
        size_t stMutation = 0;
        if (stCount > 0)
        {
            while (ui32Left > 0)
            {
                c_size stIndex = m_pRandom->rand<size_t>(0, stCount);
                if (a_rlMutate(m_vstMutateNodes.val(stIndex), a_rNodeMutation))
                { ++stMutation; }
                --ui32Left;
            }
        }

        return stMutation;
    };

    //EMP_LOG_INFO("starting mutation");

    if (stNodeCount > 0)
    {
        if (a_rMutation.literal.m_bActive)
        {
            m_vstMutateNodes.resize(0);
            for (size_t stIndex = 0; stIndex < stNodeCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                if (m_vNodes.ref(stNode).m_pLexeme->m_eType == lexeme_e::Literal)
                { m_vstMutateNodes.add(stNode); }
            }
            c_size stMutation = l_mutate_nodes(a_rMutation.literal, l_mutate_literal);
            a_rstMutation += stMutation;
            if (stMutation > 0)
            {
                //EMP_LOG_INFO("mutated #0 literals", stMutation);
            }
        }

        if (a_rMutation.variant.m_bActive)
        {
            m_vstMutateNodes.resize(0);
            for (size_t stIndex = 0; stIndex < stNodeCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                node_c& rNode = m_vNodes.ref(stNode);
                if (rNode.m_eType == node_e::DefinitionU)
                {
                    if (rNode.m_pLexeme->m_eType == lexeme_e::Variant)
                    { m_vstMutateNodes.add(stNode); }
                }
            }
            c_size stMutation = l_mutate_nodes(a_rMutation.variant, l_mutate_variant);
            a_rstMutation += stMutation;
            if (stMutation > 0)
            {
                //EMP_LOG_INFO("mutated #0 variants", stMutation);
            }
        }

        if (a_rMutation.call.m_bActive)
        {
            m_vstMutateNodes.resize(0);
            for (size_t stIndex = 0; stIndex < stNodeCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                node_c& rNode = m_vNodes.ref(stNode);
                if (rNode.m_eType == node_e::Call)
                {
                    auto const* pLexeme = rNode.m_pLexeme;
                    c_lexeme_e eLexeme = pLexeme->m_eType;
                    if (eLexeme == lexeme_e::Function ||
                        eLexeme == lexeme_e::Variant ||
                        eLexeme == lexeme_e::Intrinsic ||
                        eLexeme == lexeme_e::Operation)
                    {
                        if (emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("main"))) == false) // @@0 store info when parsing
                        { m_vstMutateNodes.add(stNode); }
                    }
                }
            }
            c_size stMutation = l_mutate_nodes(a_rMutation.call, l_mutate_call);
            a_rstMutation += stMutation;
            if (stMutation > 0)
            {
                //EMP_LOG_INFO("mutated #0 calls", stMutation);
            }
        }

        if (a_rMutation.data.m_bActive)
        {
            m_vstMutateNodes.resize(0);
            for (size_t stIndex = 0; stIndex < stNodeCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                node_c& rNode = m_vNodes.ref(stNode);
                if (rNode.m_eType == node_e::Call)
                {
                    if (rNode.m_pLexeme->m_eType == lexeme_e::Data)
                    { m_vstMutateNodes.add(stNode); }
                }
            }
            c_size stMutation = l_mutate_nodes(a_rMutation.data, l_mutate_data);
            a_rstMutation += stMutation;
            if (stMutation > 0)
            {
                //EMP_LOG_INFO("mutated #0 data", stMutation);
            }
        }

        {

/*            EMP_OSE_IF_DEBUG(
                EMP_LOG_INFO("//----------------------------------------");
                EMP_LOG_INFO("// >> CHILD");
                EMP_LOG_INFO("//----------------------------------------");
                astring sSource;
                EMP_RET_FALSE_IF_NOT(print_ose(a_rProgram->m_stAst, sSource));
                EMP_LOG_INFO("#0", sSource);
            )*/
        }

        {
            c_size stCount = m_vstMergeNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                node_c& rNode = m_vNodes.ref(stNode);
                if (rNode.m_eType == node_e::DefinitionU)
                {
                    lexeme_c* const pLexeme = rNode.m_pLexeme;
                    EMP_RET_FALSE_IF(pLexeme == nullptr);
                    c_lexeme_e eLexeme = pLexeme->m_eType;
                    if (eLexeme == lexeme_e::Variant ||
                        eLexeme == lexeme_e::Function) // @@0 dub with data
                    { m_mstTmpDefinitions.set(pLexeme, stNode); }
                }
            }
        }

        auto const& l_find_calls = [this](c_size a_stParent, auto& a_rmpDefinitions, auto const& a_lFindCalls) EMP_RETURN -> bool
        {
            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stParent));
            node_c& rParent = m_vNodes.ref(a_stParent);

            if (rParent.m_eType == node_e::Call) // @@0 used to be != node_e::Definition
            {
                // @@0 variables / data ??
                lexeme_c* const pLexeme = rParent.m_pLexeme;
                EMP_RET_FALSE_IF(pLexeme == nullptr);
                c_lexeme_e eLexeme = pLexeme->m_eType;
                if (eLexeme == lexeme_e::Variant ||
                    eLexeme == lexeme_e::Function //|| @@0 bug when removing data definitions
                    //eLexeme == lexeme_e::Data
                    )
                {
                    auto const it = a_rmpDefinitions.get(pLexeme);
                    if (it != a_rmpDefinitions.end())
                    {
                        a_rmpDefinitions.remove(it);
                        EMP_RET_FALSE_IF_NOT(a_lFindCalls(*it.value(), a_rmpDefinitions, a_lFindCalls));


                        /*for (size_t stIndex = 0; stIndex < a_rvpNodes.count(); ++stIndex)
                        {
                            node_c* const pNode = a_rvpNodes.val(stIndex);
                            if (pNode->m_eType == node_e::Definition)
                            {
                                c_lexeme_e eLexemeDef = pNode->m_pLexeme() != nullptr ? pNode->m_pLexeme->m_eType : lexeme_e::none;
                                if (eLexemeDef == lexeme_e::Function ||
                                    eLexemeDef == lexeme_e::Variant)
                                {
                                    EMP_RET_FALSE_IF_NOT(a_lFindCalls(a_rvpNodes, pNode, a_rmpDefinitions, a_lFindCalls));
                                }
                            }
                        }*/
                    }
                }
            }

            auto const* const pvstLeaves = rParent.m_pvstLeaves;
            if (pvstLeaves != nullptr)
            {
                c_size stCount = pvstLeaves->count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { EMP_RET_FALSE_IF_NOT(a_lFindCalls(pvstLeaves->val(stIndex), a_rmpDefinitions, a_lFindCalls)); }
            }

            return true;
        };


        lexeme_c* const pMainLexeme = a_rTable.get_function(ALITERAL("main()()")); // @@0 todo main could return and arg
        EMP_RET_FALSE_IF(pMainLexeme == nullptr);

        {
            size_t EMP_TT_MAX_VAR(stMain);
            c_size stCount = m_vstMergeNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                node_c* pNode = m_vNodes.ptr(m_vstMergeNodes.val(stIndex));
                if (pNode->m_eType != node_e::DefinitionU)
                { continue; }

                if (pMainLexeme == pNode->m_pLexeme)
                {
                    stMain = stNode;
                    break;
                }
            }
            EMP_RET_FALSE_IF_NOT(l_find_calls(stMain, m_mstTmpDefinitions, l_find_calls));
        }

        auto const& l_remove_node = [this, &a_rProgram](auto& a_rmpDefinitions, auto& a_rvpDelete, auto& a_rvstNodes, c_size a_stParent) EMP_RETURN -> bool
        {
            {
                c_size stCount = a_rvstNodes.count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    c_size stNode = a_rvstNodes.val(stIndex);
                    node_c& rNode = m_vNodes.ref(stNode);
                    size_t stChild = 0;
                    auto* const pvstLeaves = rNode.m_pvstLeaves;
                    if (pvstLeaves != nullptr)
                    {
                        while (stChild < pvstLeaves->count())
                        {
                            if (pvstLeaves->val(stChild) == a_stParent)
                            { pvstLeaves->remove(stChild); }
                            else
                            { ++stChild; }
                        }
                    }
                }

                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    if (a_rvstNodes.val(stIndex) == a_stParent)
                    {
                        a_rvstNodes.remove(stIndex);
                        break;
                    }
                }
            }

            for (auto itChild = a_rmpDefinitions.begin(); itChild != a_rmpDefinitions.end(); ++itChild)
            {
                if (*itChild.value() == a_stParent)
                { a_rvpDelete.add(*itChild.key()); }
            }

            EMP_RET_FALSE_IF_NOT(dealloc_node(a_rProgram, a_stParent));
            return true;
        };

        {
            auto const oItMain = m_mstTmpDefinitions.get(pMainLexeme);
            if (oItMain != m_mstTmpDefinitions.end())
            { m_mstTmpDefinitions.remove(oItMain); }
        }

        {
            EMP_OSE_IF_DEBUG(EMP_LOG_INFO("removing #0 definitions", m_mstTmpDefinitions.count());)
            EMP_OSE_IF_DEBUG(
            for (auto it = m_mstTmpDefinitions.begin(); it != m_mstTmpDefinitions.end(); ++it)
            {
                astring sSource;
                EMP_RET_FALSE_IF_NOT(print_ose(*it.value(), sSource));
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("=====>>>> removing definition\n #0", sSource);)
            }
            )
        }

        if (m_mstTmpDefinitions.count() > 0)
        {
            for (size_t stIndex = 0; stIndex < stTrailingCount; ++stIndex)
            {
                if (m_mstTmpDefinitions.count() > 0)
                {
                    size_t stDefinition = 0;
                    c_size stTrailing = m_pRandom->rand<size_t>(0, m_mstTmpDefinitions.count());
                    for (auto it = m_mstTmpDefinitions.begin(); it != m_mstTmpDefinitions.end(); ++it)
                    {
                        if (stDefinition == stTrailing)
                        {
                            m_mstTmpDefinitions.remove(it);
                            break;
                        }
                        ++stDefinition;
                    }
                }
            }
        }

        m_vpTmpLexemes.resize(0);
        for (auto it = m_mstTmpDefinitions.begin(); it != m_mstTmpDefinitions.end(); ++it)
        {
            (*it.key())->m_bCold = true;
            EMP_RET_FALSE_IF_NOT(l_remove_node(m_mstTmpDefinitions, m_vpTmpLexemes, m_vstMergeNodes, *it.value()));
            size_t stIndex = 0;
            while (stIndex < m_vstMergeNodes.count())
            {
                c_size stNode = m_vstMergeNodes.val(stIndex);
                if (*it.key() == m_vNodes.ref(stIndex).m_pLexeme)
                { EMP_RET_FALSE_IF_NOT(l_remove_node(m_mstTmpDefinitions, m_vpTmpLexemes, m_vstMergeNodes, stNode)); }
                else
                { ++stIndex; }
            }
        }

        for (size_t stIndex = 0; stIndex < m_vpTmpLexemes.count(); ++stIndex)
        { m_mstTmpDefinitions.remove(m_mstTmpDefinitions.get(m_vpTmpLexemes.val(stIndex))); }

        c_size stCrossOverCount = a_rCross.m_ui32Base + m_pRandom->rand<ui32>(0, a_rCross.m_ui32Count);

        if (stNodeCount > 1 && a_rCross.m_bActive)
        {
            for (size_t stIndex = 0; stIndex < stCrossOverCount * 2; ++stIndex)
            {
                c_size stRandomIndex0 = m_pRandom->rand<size_t>(0, stNodeCount);
                c_size stRandomIndex1 = m_pRandom->rand<size_t>(0, stNodeCount);
                if (stRandomIndex0 != stRandomIndex1)
                {
                    c_size stNode = m_vstMergeNodes.val(stRandomIndex0);
                    c_size stNext = m_vstMergeNodes.val(stRandomIndex1);


                    if (m_vNodes.ref(stNode).m_bSource != m_vNodes.ref(stNext).m_bSource)
                    {
                        bool bCross = false;
                        for (size_t stIndex0 = 0; stIndex0 < stNodeCount; ++stIndex0)
                        {
                            c_size stTest = m_vstMergeNodes.val(stIndex);
                            node_c& rTestNode = m_vNodes.ref(stTest);
                            auto* const pvstTestLeaves = rTestNode.m_pvstLeaves;
                            if (pvstTestLeaves != nullptr)
                            {
                                c_size stTestCount = pvstTestLeaves->count();
                                for (size_t stChildIndex = 0; stChildIndex < stTestCount; ++stChildIndex)
                                {
                                    c_size stChild = pvstTestLeaves->val(stChildIndex);
                                    if (stChild == stNode)
                                    {
                                        pvstTestLeaves->set(stChildIndex, stNext);
                                        bCross = true;
                                    }
                                    else if (stChild == stNext)
                                    {
                                        pvstTestLeaves->set(stChildIndex, stNode);
                                        bCross = true;
                                    }
                                }
                            }
                        }

                        if (bCross)
                        {
                            ++a_rstCross;
                        }
                    }
                }

                if (a_rstCross >= stCrossOverCount)
                {
                    break;
                }
            }
        }

        if (a_rstMutation > 0)
        {
            //EMP_LOG_INFO("mutated #0 nodes", a_rstMutation);
        }
        if (a_rstCross > 0)
        {
            //EMP_LOG_INFO("crossed #0 nodes", a_rstCross);
        }
    }

    if (a_rMutation.active() == true && ((a_rstMutation > 0) == false))
    { return false; }
    if (a_rCross.m_bActive == true && ((a_rstCross > 0) == false))
    { return false; }
    //EMP_LOG_INFO("ending mutation");
    return true;
}

/*EMP_RETURN bool mutate_ast(table_c const& a_rTable, node_c* a_pNode, size_t& a_rstIndex);
EMP_RETURN bool mutate_ast(table_c const& a_rTable, node_c* a_pNode, size_t& a_rstIndex)
{
    ++a_rstIndex;
    EMP_RET_FALSE_IF(a_pNode == nullptr);

    switch (a_pNode->m_pLexeme->m_eType)
    {
        case lexeme_e::Literal:
        {
            literal_t<float>* const pLiteral = to_literal<float>(a_pNode->m_pLexeme);
            if (pLiteral != nullptr)
            {
                pLiteral->m_tLiteral += emp::mat::random_float(pLiteral->m_tLiteral) * (emp::mat::random_float(1.0f) - 0.5f);
            }
            break;
        }
        case lexeme_e::Variant:
        {
            if ((a_rstIndex % (2 + emp::mat::urandom() % 2)) == 0)
            {
                variant_c* const pVariant = to_function<variant_c>(a_pNode->m_pLexeme);
                EMP_RET_FALSE_IF(pVariant == nullptr);
                size_t stCount = pVariant->m_vSlots.count();
                pVariant->m_fSum = 0;
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                {
                    c_float fWeight = emp::mat::random_float(pVariant->m_vSlots.ref(stIndex).weight);
                    pVariant->m_vSlots.ref(stIndex).weight = fWeight;
                    pVariant->m_fSum += fWeight;
                }
                break;
            }
            // @@comment fall through
        }
        case lexeme_e::Function:
        {
            if (emp::str::cmp(a_pNode->m_pLexeme->m_sName, astring(ALITERAL("main"))))
            {
                break;
            }
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Operation:
        {
            if ((a_pNode->m_eType == node_e::Call || a_pNode->m_pLexeme->m_eLexeme == lexeme_e::Control)// &&
                //(a_rstIndex % (2 + emp::mat::urandom() % 2)) == 0
                )
            {
                auto const& vLexemes = a_rTable.lexemes().vector();
                size_t stCount = vLexemes.count();
                size_t stIndex = 0;
                size_t stRand = emp::mat::urandom() % stCount;
                size_t stMutation = 0;
                while (true)
                {
                    lexeme_c* const pLexeme = vLexemes.val(stIndex % stCount);
                    if (pLexeme->m_pType == a_pNode->m_pLexeme->m_pType &&
                        pLexeme != a_pNode->m_pLexeme)
                    {
                        if (pLexeme->m_eType != lexeme_e::Function ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("main"))) == false)
                        {
                            if (stMutation == stRand)
                            {
                                a_pNode->m_pLexeme = pLexeme;
                                break;
                            }
                            else
                            { ++stMutation; }
                        }
                    }
                    ++stIndex;
                    if (stIndex >= stCount && stMutation == 0)
                    {
                        break;
                    }
                }
            }
            break;
        }
        case lexeme_e::Data:
        case lexeme_e::Punctuation:
        case lexeme_e::none:
        case lexeme_e::Comment:
        default:
        { break; }
    };

    c_size stCount = a_pNode->m_pvpLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        node_c* pChild = a_pNode->m_pvpLeaves->val(stIndex);
        EMP_RET_FALSE_IF(mutate_ast(a_rTable, pChild, a_rstIndex));
    }

    return true;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void context_c::create(c_astring& a_rsExpression)
{
    destroy();
    m_sExpression = a_rsExpression;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void context_c::destroy(void)
{
    m_siParenthesis = 0;
    m_siBrace = 0;
    m_siBracket = 0;
    m_siStack = 0;

    m_bParenthesis = false;
    m_bBrace = false;
    m_bBracket = false;
    m_bStack = false;

    m_stLine = 1;
    m_stColumn = 0;

    m_sExpression = astring();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool context_c::end_of_context(void)
{
    resolve();
    return not_underflow() && not_overflow();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void context_c::resolve(void)
{
    m_siParenthesis += m_bParenthesis ? 1 : 0;
    m_siBrace += m_bBrace ? 1 : 0;
    m_siBracket += m_bBracket ? 1 : 0;
    m_siStack += m_bStack ? 1 : 0;

    m_bParenthesis = false;
    m_bBrace = false;
    m_bBracket = false;
    m_bStack = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool context_c::resolve(token_c& a_rToken, c_bool a_bStack)
{
    resolve();

    if (a_bStack)
    {
        c_astring& sToken = a_rToken.m_sName;

        c_size stLen = sToken.len();
        for (size_t stIndex = 0; stIndex < stLen; ++stIndex)
        {
            c_char cChar = sToken.get_char(stIndex);
            switch (cChar)
            {
                case '(':
                {
                    m_bParenthesis = true;
                    m_bStack = true;
                    break;
                }
                case ')':
                {
                    --m_siParenthesis;
                    --m_siStack;
                    break;
                }
                case '{':
                {
                    m_bBrace = true;
                    m_bStack = true;
                    break;
                }
                case '}':
                {
                    --m_siBrace;
                    --m_siStack;
                    break;
                }
                case '[':
                {
                    m_bBracket = true;
                    m_bStack = true;
                    break;
                }
                case ']':
                {
                    --m_siBracket;
                    --m_siStack;
                    break;
                }
                default:
                { break; }
            };
        }
    }

    a_rToken.m_Info.m_stLine = m_stLine;
    a_rToken.m_Info.m_stColumn = m_stColumn - a_rToken.m_sName.len();
    EMP_RET_FALSE_IF(m_siStack < 0, "failed to resolve token, invalid stack");

    a_rToken.m_siStack = m_siStack;
    a_rToken.m_stStack = static_cast<size_t>(m_siStack);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool context_c::not_underflow(void) const
{
    EMP_RET_FALSE_IF(m_siParenthesis > 0, "parenthesis mismatch line #0, column #1", m_stLine, m_stColumn);
    EMP_RET_FALSE_IF(m_siBrace > 0, "brace mismatch line #0, column #1", m_stLine, m_stColumn);
    EMP_RET_FALSE_IF(m_siBracket > 0, "bracket mismatch line #0, column #1", m_stLine, m_stColumn);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool context_c::not_overflow(void) const
{
    EMP_RET_FALSE_IF(m_siParenthesis < 0, "parenthesis mismatch line #0, column #1", m_stLine, m_stColumn);
    EMP_RET_FALSE_IF(m_siBrace < 0, "brace mismatch line #0, column #1", m_stLine, m_stColumn);
    EMP_RET_FALSE_IF(m_siBracket < 0, "bracket mismatch line #0, column #1", m_stLine, m_stColumn);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compiler_c::compiler_c(void):
    m_pStack(),
    m_pProcessor(),
    m_mstTmpDefinitions(8)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compiler_c::~compiler_c(void)
{
    emp::cnt::foreach(m_vpPrograms, emp::mem::safe_delete_reset_t());
    emp::cnt::foreach(m_vpvstLeaves, emp::mem::safe_delete_reset_t());
    emp::cnt::foreach(m_vpvLexemes, emp::mem::safe_delete_reset_t());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::create(void)
{
    EMP_RET_FALSE_IF_NOT(m_Table.create());

    rti::type_c const* const pSIntType = m_Table.sint_type();
    rti::type_c const* const pUIntType = m_Table.uint_type();
    rti::type_c const* const pRealType = m_Table.real_type();
    rti::type_c const* const pBoolType = m_Table.bool_type();

    { // ()()
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // ()(int,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // ()(uint,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pUIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // ()(bool,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pBoolType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (float,)()
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (float,)(float,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (bool,)(float,float,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pBoolType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (float,)(float,float,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (int,)(int,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (bool,)(int,int,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pBoolType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (int,)(int,int,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

    { // (uint,)(uint,uint,)
        rti::function_c* const pType = m_Table.new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pUIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pUIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pUIntType));
        EMP_RET_FALSE_IF_NOT(m_Table.qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_Table.reg_type(pType));
    }

#define XTMP_com(x_Val, ...) m_Table.reg_comment    (comment_e    ::x_Val,__VA_ARGS__)
#define XTMP_pun(x_Val, ...) m_Table.reg_punctuation(punctuation_e::x_Val,__VA_ARGS__)
#define XTMP_ope(x_Val, ...) m_Table.reg_operation  (instruction_e::x_Val,__VA_ARGS__)
#define XTMP_ctr(x_Val, ...) m_Table.reg_control    (control_e    ::x_Val,__VA_ARGS__)
#define XTMP_ist(x_Val, ...) m_Table.reg_intrinsic  (instruction_e::x_Val,__VA_ARGS__)
#define XTMP_lit(x_Val, ...) m_Table.reg_literal<x_Val>(__VA_ARGS__)

#define XTMP_COM(...) EMP_RET_FALSE_IF(XTMP_com(__VA_ARGS__) == nullptr)
#define XTMP_PUN(...) EMP_RET_FALSE_IF(XTMP_pun(__VA_ARGS__) == nullptr)
#define XTMP_OPE(...) EMP_RET_FALSE_IF(XTMP_ope(__VA_ARGS__) == nullptr)
#define XTMP_CTR(...) EMP_RET_FALSE_IF(XTMP_ctr(__VA_ARGS__) == nullptr)
#define XTMP_IST(...) EMP_RET_FALSE_IF(XTMP_ist(__VA_ARGS__) == nullptr)
#define XTMP_LIT(...) EMP_RET_FALSE_IF(XTMP_lit(__VA_ARGS__) == nullptr)
#define XTMP_RES(...) XTMP_IST(Reserved, ALITERAL(__VA_ARGS__), ALITERAL("()()"), true)

XTMP_COM( SingleComment , ALITERAL("//") );
XTMP_COM( BeginComment  , ALITERAL("/*") );
XTMP_COM( EndComment    , ALITERAL("*/") );

m_pComma      = XTMP_pun( Comma         , ALITERAL(",") , 0 );
m_pSemiColon  = XTMP_pun( SemiColon     , ALITERAL(";") , 1 );
m_pRightBrace = XTMP_pun( RightBrace    , ALITERAL("}") , 1 );
m_pDeclare    = XTMP_pun( Declare       , ALITERAL("@") , 2 );
m_pAssign     = XTMP_pun( Assign        , ALITERAL("=") , 3 );
m_pLeftBrace  = XTMP_pun( LeftBrace     , ALITERAL("{") , 4 );
m_pSpace      = XTMP_pun( Space         , ALITERAL(" ") , 5 );

                  XTMP_OPE( Equal_I32     , ALITERAL("==") ,  7 , ALITERAL("(bool,)(int,int,)")     , true , false );
                  XTMP_OPE( More_I32      , ALITERAL(">")  ,  8 , ALITERAL("(bool,)(int,int,)")     , true , false );
                  XTMP_OPE( More_F32      , ALITERAL(">")  ,  8 , ALITERAL("(bool,)(float,float,)") , true , false );
                  XTMP_OPE( MoreEq_I32    , ALITERAL(">=") ,  8 , ALITERAL("(bool,)(int,int,)")     , true , false );
                  XTMP_OPE( MoreEq_F32    , ALITERAL(">=") ,  8 , ALITERAL("(bool,)(float,float,)") , true , false );
                  XTMP_OPE( Less_I32      , ALITERAL("<")  ,  9 , ALITERAL("(bool,)(int,int,)")     , true , false );
                  XTMP_OPE( Less_F32      , ALITERAL("<")  ,  9 , ALITERAL("(bool,)(float,float,)") , true , false );
                  XTMP_OPE( LessEq_F32    , ALITERAL("<=") ,  9 , ALITERAL("(bool,)(float,float,)") , true , false );
                  XTMP_OPE( Add_U32       , ALITERAL("+")  , 10 , ALITERAL("(uint,)(uint,uint,)")   , true , false );
                  XTMP_OPE( Add_I32       , ALITERAL("+")  , 10 , ALITERAL("(int,)(int,int,)")      , true , false );
                  XTMP_OPE( Add_F32       , ALITERAL("+")  , 10 , ALITERAL("(float,)(float,float,)"), true , false );
                  XTMP_OPE( Sub_I32       , ALITERAL("-")  , 10 , ALITERAL("(int,)(int,int,)")      , true , false );
                  XTMP_OPE( Sub_F32       , ALITERAL("-")  , 10 , ALITERAL("(float,)(float,float,)"), true , false );
                  XTMP_OPE( Mul_I32       , ALITERAL("*")  , 11 , ALITERAL("(int,)(int,int,)")      , true , false );
                  XTMP_OPE( Mul_F32       , ALITERAL("*")  , 11 , ALITERAL("(float,)(float,float,)"), true , false );
                  XTMP_OPE( Div_I32       , ALITERAL("/")  , 11 , ALITERAL("(int,)(int,int,)")      , true , false );
                  XTMP_OPE( Div_F32       , ALITERAL("/")  , 11 , ALITERAL("(float,)(float,float,)"), true , false );
                  XTMP_OPE( Mod_I32       , ALITERAL("%")  , 11 , ALITERAL("(int,)(int,int,)")      , true , false );
m_pUnaryPos_I32 = XTMP_ope( Pos_I32       , ALITERAL("+")  , 12 , ALITERAL("(int,)(int,)")          , false, false );
m_pUnaryPos_F32 = XTMP_ope( Pos_F32       , ALITERAL("+")  , 12 , ALITERAL("(float,)(float,)")      , false, false );
m_pUnaryNeg_I32 = XTMP_ope( Neg_I32       , ALITERAL("-")  , 12 , ALITERAL("(int,)(int,)")          , false, false );
m_pUnaryNeg_F32 = XTMP_ope( Neg_F32       , ALITERAL("-")  , 12 , ALITERAL("(float,)(float,)")      , false, false );
                //   XTMP_OPE( Reserved      , ALITERAL("++") , 12 , ALITERAL("(int,)(int,)")          , false, true  );
                //   XTMP_OPE( Reserved      , ALITERAL("++") , 12 , ALITERAL("(float,)(float,)")      , false, true  );
                //   XTMP_OPE( Reserved      , ALITERAL("--") , 12 , ALITERAL("(int,)(int,)")          , false, true  );
                //   XTMP_OPE( Reserved      , ALITERAL("--") , 12 , ALITERAL("(float,)(float,)")      , false, true  );

m_pRightPar   = XTMP_pun( RightPar      , ALITERAL(")")  , 13 );
m_pLeftPar    = XTMP_pun( LeftPar       , ALITERAL("(")  , 14 );

m_pWeight     = XTMP_lit( real, ALITERAL("1.0")  , 1.0f  );
m_pTrue       = XTMP_lit( bool, ALITERAL("true") , true  );
m_pFalse      = XTMP_lit( bool, ALITERAL("false"), false );

XTMP_IST( Sqrt_F32      , ALITERAL("sqrt")  , ALITERAL("(float,)(float,)"), false);
XTMP_IST( Abs_F32       , ALITERAL("abs")   , ALITERAL("(float,)(float,)"), false);
XTMP_IST( Abs_I32       , ALITERAL("abs")   , ALITERAL("(int,)(int,)"    ), false);
XTMP_IST( CosD_F32      , ALITERAL("cosd")  , ALITERAL("(float,)(float,)"), false);
XTMP_IST( SinD_F32      , ALITERAL("sind")  , ALITERAL("(float,)(float,)"), false);
XTMP_IST( Cos_F32       , ALITERAL("cos")   , ALITERAL("(float,)(float,)"), false);
XTMP_IST( Sin_F32       , ALITERAL("sin")   , ALITERAL("(float,)(float,)"), false);
XTMP_IST( Print_I32     , ALITERAL("print") , ALITERAL("()(int,)"        ), false);
XTMP_IST( Randf_F32     , ALITERAL("randf") , ALITERAL("(float,)()"      ), false);
XTMP_CTR( Loop_U32      , ALITERAL("loop")  , ALITERAL("()(uint,)"       ), false);
XTMP_CTR( If            , ALITERAL("if")    , ALITERAL("()(bool,)"       ), false);

XTMP_RES("alignas"          );
XTMP_RES("alignof"          );
XTMP_RES("and"              );
XTMP_RES("and_eq"           );
XTMP_RES("asm"              );
XTMP_RES("auto"             );
XTMP_RES("bitand"           );
XTMP_RES("bitor"            );
XTMP_RES("bool"             );
XTMP_RES("break"            );
XTMP_RES("case"             );
XTMP_RES("catch"            );
XTMP_RES("char"             );
XTMP_RES("char16_t"         );
XTMP_RES("char32_t"         );
XTMP_RES("class"            );
XTMP_RES("compl"            );
XTMP_RES("const"            );
XTMP_RES("constexpr"        );
XTMP_RES("const_cast"       );
XTMP_RES("continue"         );
XTMP_RES("decltype"         );
XTMP_RES("default"          );
XTMP_RES("delete"           );
XTMP_RES("do"               );
XTMP_RES("double"           );
XTMP_RES("dynamic_cast"     );
XTMP_RES("else"             );
XTMP_RES("enum"             );
XTMP_RES("explicit"         );
XTMP_RES("export"           );
XTMP_RES("extern"           );
//XTMP_RES("false"            );
//XTMP_RES("float"            );
XTMP_RES("for"              );
XTMP_RES("friend"           );
XTMP_RES("goto"             );
//XTMP_RES("if"               );
XTMP_RES("inline"           );
//XTMP_RES("int"              );
XTMP_RES("long"             );
XTMP_RES("mutable"          );
XTMP_RES("namespace"        );
XTMP_RES("new"              );
XTMP_RES("noexcept"         );
XTMP_RES("not"              );
XTMP_RES("not_eq"           );
XTMP_RES("nullptr"          );
XTMP_RES("operator"         );
XTMP_RES("or"               );
XTMP_RES("or_eq"            );
XTMP_RES("private"          );
XTMP_RES("protected"        );
XTMP_RES("public"           );
XTMP_RES("register"         );
XTMP_RES("reinterpret_cast" );
XTMP_RES("return"           );
XTMP_RES("short"            );
XTMP_RES("signed"           );
XTMP_RES("sizeof"           );
XTMP_RES("static"           );
XTMP_RES("static_assert"    );
XTMP_RES("static_cast"      );
XTMP_RES("struct"           );
XTMP_RES("switch"           );
XTMP_RES("template"         );
XTMP_RES("this"             );
XTMP_RES("thread_local"     );
XTMP_RES("throw"            );
//XTMP_RES("true"             );
XTMP_RES("try"              );
XTMP_RES("typedef"          );
XTMP_RES("typeid"           );
XTMP_RES("typename"         );
XTMP_RES("union"            );
XTMP_RES("unsigned"         );
XTMP_RES("using"            );
XTMP_RES("virtual"          );
XTMP_RES("void"             );
XTMP_RES("volatile"         );
XTMP_RES("wchar_t"          );
XTMP_RES("while"            );
XTMP_RES("xor"              );
XTMP_RES("xor_eq"           );
XTMP_RES("override"         );
XTMP_RES("final"            );
XTMP_RES("elif"             );
XTMP_RES("endif"            );
XTMP_RES("defined"          );
XTMP_RES("ifdef"            );
XTMP_RES("ifndef"           );
XTMP_RES("define"           );
XTMP_RES("undef"            );
XTMP_RES("include"          );
XTMP_RES("line"             );
XTMP_RES("error"            );
XTMP_RES("pragma"           );

#undef XTMP_CTR
#undef XTMP_OPE
#undef XTMP_PUN
#undef XTMP_COM
#undef XTMP

    m_stWeightNode = alloc_node(nullptr);
    EMP_RET_FALSE_IF(emp::tt::is_max(m_stWeightNode));

    m_vNodes.ref(m_stWeightNode).m_pLexeme = m_pWeight;

    EMP_RET_FALSE_IF(m_pComma      == nullptr);
    EMP_RET_FALSE_IF(m_pSemiColon  == nullptr);
    EMP_RET_FALSE_IF(m_pRightBrace == nullptr);
    EMP_RET_FALSE_IF(m_pDeclare    == nullptr);
    EMP_RET_FALSE_IF(m_pLeftBrace  == nullptr);
    EMP_RET_FALSE_IF(m_pAssign     == nullptr);
    EMP_RET_FALSE_IF(m_pRightPar   == nullptr);
    EMP_RET_FALSE_IF(m_pLeftPar    == nullptr);

    EMP_RET_FALSE_IF(m_pWeight    == nullptr);
    EMP_RET_FALSE_IF(m_pTrue      == nullptr);
    EMP_RET_FALSE_IF(m_pFalse     == nullptr);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_OSE_LC(x_Line, x_Column, x_Format, ...)\
    "EE : l-#0 : c-#1 : #2", x_Line, x_Column, STR_AFORMAT(x_Format, __VA_ARGS__) // @@0 alloc

#define EMP_OSE_INFO(x_Info, x_Format, ...) EMP_OSE_LC((x_Info).m_stLine, (x_Info).m_stColumn, x_Format, __VA_ARGS__)
#define EMP_OSE_NODE(x_Node, x_Format, ...) EMP_OSE_INFO((x_Node).m_Info, x_Format, __VA_ARGS__)
#define EMP_OSE_TOKEN(x_Token, x_Format, ...) EMP_OSE_INFO((x_Token).m_Info, x_Format, __VA_ARGS__)

//#define EMP_OSE_ERROR(x_Info, x_Format, ...) EMP_LOG_ERROR(EMP_OSE_INFO(x_Info, x_Format, __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compiler_c::clear(void)
{
    m_Context.destroy();
    m_vTokens.resize(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN error_e compiler_c::push_data(program_c& a_rProgram, c_astring& a_rsName)
{
    auto* const pLexeme = a_rProgram.m_Table.get_lexeme(a_rsName);
    if (pLexeme == nullptr)
    { return error_e::UserError; }

    auto* const pData = to_lexeme<data_c>(pLexeme);
    if (pData == nullptr)
    { return error_e::UserError; }

    m_pStack->copy(&a_rProgram);

    c_size stNode = a_rProgram.find_node(this, pLexeme);
    EMP_RET_X_IF(error_e::CompilerError, emp::tt::is_max(stNode));

    auto const* const pvstLeaves = m_vNodes.ref(stNode).m_pvstLeaves;
    size_t stConstProgram = alloc_program();
    EMP_RET_X_IF_NOT(error_e::CompilerError, m_vpPrograms.is_valid_index(stConstProgram));

    auto* const pConstProgram = m_vpPrograms.val(stConstProgram);
    EMP_RET_X_IF(error_e::CompilerError, pConstProgram == nullptr);

    auto oFinally = emp::tt::finally([this, &stConstProgram](){
        if (dealloc_program(stConstProgram) == false)
        {
            EMP_LOG_ERROR("Failed to deallocate program");
            EMP_TT_MAX_VAR(stConstProgram);
        }
    });

    EMP_RET_X_IF_NOT(error_e::CompilerError, build_asm(pvstLeaves, *pConstProgram, pConstProgram->m_Table, false));
    EMP_RET_X_IF_NOT(error_e::CompilerError, pConstProgram->link(true));

    c_size stMaxCount = 1024;
    ui32 ui32Processed = 0;
    int siProcessed = m_pProcessor->run(*pConstProgram, *m_pStack, stMaxCount, stMaxCount, ui32Processed);
    EMP_RET_X_IF(error_e::CompilerError, siProcessed < 0);

    c_size stProcessed = static_cast<size_t>(siProcessed);
    EMP_RET_X_IF(error_e::CompilerError, stProcessed >= stMaxCount);

    return emp::tt::is_not_max(stConstProgram) ? error_e::none : error_e::CompilerError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::from_file(c_astring& a_sFile, c_size a_stFather,
    c_mutation_s& a_rMutation, size_t& a_rstMutation, c_crossover_s& a_rCrossover, size_t& a_rstCross, c_bool a_bSkipGlobals, c_size a_stGen)
{
    emp::fs::file_c oFile;

    EMP_RET_MAX_IF_NOT(size_t, oFile.create(a_sFile, emp::fs::file_e::Binary, fs::create_e::Read), "failed to open file '#0'", a_sFile);

    astring sBuffer;
    EMP_RET_MAX_IF_NOT(size_t, oFile.read_all(sBuffer), "failed to read file '#0'", a_sFile);
    EMP_RET_MAX_IF(size_t, sBuffer.is_empty(), "empty file '#0'", a_sFile);

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("//----------------------------------------");)
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("// building '#0'", a_sFile);)
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("//----------------------------------------");)
    c_size stProgram = from_string(sBuffer, a_stFather, a_rMutation, a_rstMutation, a_rCrossover, a_rstCross, a_bSkipGlobals, a_stGen);
    EMP_RET_MAX_IF_NOT(size_t, m_vpPrograms.is_valid_index(stProgram), "Failed to build file '#0'", a_sFile);

    m_vpPrograms.val(stProgram)->m_sFilename = a_sFile;
    m_vpPrograms.val(stProgram)->m_sBuffer = sBuffer;

    EMP_OSE_IF_DEBUG(
                     EMP_LOG_INFO("//----------------------------------------");
                     EMP_LOG_INFO("// builded '#0'", a_sFile);
                     EMP_LOG_INFO("//----------------------------------------");)

    return stProgram;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::from_string(c_astring& a_sExpression, c_size a_stFather,
    c_mutation_s& a_rMutation, size_t& a_rstMutation, c_crossover_s& a_rCrossover, size_t& a_rstCross, c_bool a_bSkipGlobals, c_size a_stGen)
{
    clear();

    size_t stProgram = alloc_program();
    EMP_RET_MAX_IF_NOT(size_t, m_vpPrograms.is_valid_index(stProgram));

    auto oFinally = emp::tt::finally([this, &stProgram]()
    {
        if (m_vpPrograms.is_valid_index(stProgram))
        {
            if (dealloc_program(stProgram) == false)
            {
                EMP_TT_MAX_VAR(stProgram);
            }
        }
    });

    auto* const pProgram = m_vpPrograms.val(stProgram);
    EMP_RET_MAX_IF_NOT(size_t, pProgram->m_Table.clone(m_Table));

    pProgram->m_sExpression = a_sExpression;
    pProgram->m_stGen = a_stGen;

    m_Context.create(a_sExpression);

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// starting to build #0", a_sExpression);
        EMP_LOG_INFO("//----------------------------------------");)
    EMP_RET_MAX_IF_NOT(size_t, build_tokens(*pProgram));
    EMP_RET_MAX_IF_NOT(size_t, m_Context.end_of_context());

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> TOKENS");
        EMP_LOG_INFO("//----------------------------------------");
    {
        astring sBuffer;
        print_tokens(m_vTokens, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
    })

    EMP_RET_MAX_IF_NOT(size_t, clean_tokens(*pProgram));

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> CLEAN TOKENS");
        EMP_LOG_INFO("//----------------------------------------");
    {
        astring sBuffer;
        print_tokens(m_vTokens, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
    })


    pProgram->m_stAst = build_ast(*pProgram, token_chc(m_vTokens), 0, pProgram->m_Table, 0);
    EMP_RET_MAX_IF(size_t, emp::tt::is_max(pProgram->m_stAst));
    EMP_OSE_IF_DEBUG({
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST0");
        EMP_LOG_INFO("//----------------------------------------");
        astring sBuffer;
        c_bool bResult = print_ast(pProgram->m_stAst, 0, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
        EMP_RET_MAX_IF_NOT(size_t, bResult);
    })

    EMP_RET_MAX_IF_NOT(size_t, resolve_unaries(*pProgram, pProgram->m_stAst, pProgram->m_Table));
    EMP_OSE_IF_DEBUG({
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST1");
        EMP_LOG_INFO("//----------------------------------------");
        astring sBuffer;
        c_bool bResult = print_ast(pProgram->m_stAst, 0, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
        EMP_RET_MAX_IF_NOT(size_t, bResult);
    })


    EMP_RET_MAX_IF_NOT(size_t, resolve_literals(*pProgram, pProgram->m_stAst, pProgram->m_Table));
    EMP_OSE_IF_DEBUG({
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST2");
        EMP_LOG_INFO("//----------------------------------------");
        astring sBuffer;
        c_bool bResult = print_ast(pProgram->m_stAst, 0, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
        EMP_RET_MAX_IF_NOT(size_t, bResult);
    })

    EMP_RET_MAX_IF_NOT(size_t, resolve_calls(*pProgram, pProgram->m_stAst, pProgram->m_Table));
    EMP_OSE_IF_DEBUG({
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST3");
        EMP_LOG_INFO("//----------------------------------------");
        astring sBuffer;
        c_bool bResult = print_ast(pProgram->m_stAst, 0, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
        EMP_RET_MAX_IF_NOT(size_t, bResult);
    })



    EMP_OSE_IF_DEBUG({
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> OSE");
        EMP_LOG_INFO("//----------------------------------------");
        astring sBuffer;
        c_bool bResult = print_ose(pProgram->m_stAst, sBuffer);
        EMP_LOG_INFO("#0", sBuffer);
        EMP_RET_MAX_IF_NOT(size_t, bResult);
    })

    if (a_rMutation.active() || a_rCrossover.m_bActive)
    {
        if (emp::tt::is_not_max(a_stFather))
        {
            EMP_RET_FALSE_IF_NOT(m_vpPrograms.is_valid_index(a_stFather));
            EMP_RET_MAX_IF_NOT(size_t, merge_ast(*pProgram, m_vpPrograms.val(a_stFather)));
        }

        EMP_OSE_IF_DEBUG({
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// >> SEX");
            EMP_LOG_INFO("//----------------------------------------");
            astring sBuffer;
            c_bool bResult = print_ast(pProgram->m_stAst, 0, sBuffer);
            EMP_LOG_INFO("#0", sBuffer);
            EMP_RET_MAX_IF_NOT(size_t, bResult);
        })

        EMP_RET_MAX_IF_NOT(size_t, mutate_ast(*pProgram, pProgram->m_Table, a_rMutation, a_rCrossover, a_rstCross, a_rstMutation));

 /*       EMP_OSE_IF_DEBUG(
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// >> MUT");
            EMP_LOG_INFO("//----------------------------------------");
            print_ast(pProgram->m_pAst, 0);)*/

        EMP_OSE_IF_DEBUG({
                EMP_LOG_INFO("//----------------------------------------");
                EMP_LOG_INFO("// >> OSE");
                EMP_LOG_INFO("//----------------------------------------");
                astring sBuffer;
                c_bool bResult = print_ose(pProgram->m_stAst, sBuffer);
                EMP_LOG_INFO("#0", sBuffer);
                EMP_RET_MAX_IF_NOT(size_t, bResult);
        })
    }

    EMP_RET_MAX_IF_NOT(size_t, build_asm(m_vNodes.ref(pProgram->m_stAst).m_pvstLeaves, *pProgram, pProgram->m_Table, a_bSkipGlobals));
    if (emp::tt::is_max(a_stGen))
    {
        EMP_RET_MAX_IF_NOT(size_t, pProgram->link(emp::tt::is_not_max(a_stGen)));
        
    }
    else
    {
        EMP_RET_MAX_IF(size_t, pProgram->m_stGen >= pProgram->m_stCurrentGen);
    }

    // TODO_RGEN: readd issue with print_asm()
    // EMP_OSE_IF_DEBUG({
    //     EMP_LOG_INFO("//----------------------------------------");
    //     EMP_LOG_INFO("// >> ASM");
    //     EMP_LOG_INFO("//----------------------------------------");
    //     astring sBuffer;
    //     c_bool bResult = print_asm(*pProgram, sBuffer);
    //     EMP_LOG_INFO("#0", sBuffer);
    //     EMP_RET_MAX_IF_NOT(size_t, bResult);
    //  })

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// compilation ended sucessfully");
        EMP_LOG_INFO("//----------------------------------------");)

    oFinally.release();
    return stProgram;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::alloc_lexemes(void)
{
    if (m_vstFreeLexemes.count() > 0)
    {
        c_size stLexemes = m_vstFreeLexemes.last_val();
        m_vstFreeLexemes.remove_last();
        EMP_RET_MAX_IF_NOT(size_t, m_vpvLexemes.is_valid_index(stLexemes));
        auto* const pvLexemes = m_vpvLexemes.val(stLexemes);
        EMP_RET_MAX_IF(size_t, pvLexemes == nullptr);
        pvLexemes->resize(0);
        return stLexemes;
    }
    else
    {
        auto* const pvstLexemes = EMP_ALLOCATE(vector<lexeme_s>);
        c_size stLexemes = m_vpvLexemes.count();
        m_vpvLexemes.add(pvstLexemes);
        return stLexemes;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::dealloc_lexemes(c_size a_stLexemes)
{
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(a_stLexemes));
    m_vstFreeLexemes.add(a_stLexemes);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::alloc_program(void)
{
    size_t EMP_TT_MAX_VAR(stProgram);
    program_c* pProgram = nullptr;
    if (m_vstFreePrograms.count() > 0)
    {
        stProgram = m_vstFreePrograms.last_val();
        m_vstFreePrograms.remove_last();
        EMP_RET_MAX_IF_NOT(size_t, m_vpPrograms.is_valid_index(stProgram));
        pProgram = m_vpPrograms.val(stProgram);
        EMP_RET_MAX_IF(size_t, pProgram == nullptr);
        pProgram->clear();
    }
    else
    {
        stProgram = m_vpPrograms.count();
        pProgram = EMP_ALLOCATE(program_c);
        m_vpPrograms.add(pProgram);
    }

    c_size stLeaves = alloc_leaves();
    pProgram->m_stNodes = stLeaves;
    EMP_RET_FALSE_IF_NOT(m_vpvstLeaves.is_valid_index(stLeaves));
    pProgram->m_pvstNodes = m_vpvstLeaves.val(stLeaves);

    return stProgram;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::dealloc_program(c_size a_stProgram)
{
    EMP_RET_FALSE_IF_NOT(m_vpPrograms.is_valid_index(a_stProgram));
    auto* const pProgram = m_vpPrograms.val(a_stProgram);
    EMP_RET_FALSE_IF(pProgram == nullptr);

    m_vstFreePrograms.add(a_stProgram);

    c_size stNodes = pProgram->m_stNodes;
    auto* const pvstNodes = pProgram->m_pvstNodes;
    EMP_RET_FALSE_IF(pvstNodes == nullptr || emp::tt::is_max(stNodes));

    c_size stCount = pvstNodes->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stNode = pvstNodes->val(stIndex);
        if (emp::tt::is_not_max(stNode))
        { EMP_RET_FALSE_IF_NOT(dealloc_node(*pProgram, pvstNodes->val(stIndex))); }
    }

    return dealloc_leaves(stNodes);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::alloc_leaves(void)
{
    if (m_vstFreeLeaves.count() > 0)
    {
        c_size stLeaves = m_vstFreeLeaves.last_val();
        m_vstFreeLeaves.remove_last();
        EMP_RET_MAX_IF_NOT(size_t, m_vpvstLeaves.is_valid_index(stLeaves));
        auto* const pvstLeaves = m_vpvstLeaves.val(stLeaves);
        EMP_RET_MAX_IF(size_t, pvstLeaves == nullptr);
        pvstLeaves->resize(0);
        return stLeaves;
    }
    else
    {
        auto* const pvstLeaves = EMP_ALLOCATE(vector<size_t>);
        c_size stLeaves = m_vpvstLeaves.count();
        m_vpvstLeaves.add(pvstLeaves);
        return stLeaves;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::dealloc_leaves(c_size a_stLeaves)
{
    EMP_RET_FALSE_IF_NOT(m_vpvstLeaves.is_valid_index(a_stLeaves));
    m_vpvstLeaves.ref(a_stLeaves)->resize(0);
    m_vstFreeLeaves.add(a_stLeaves);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::alloc_node(program_c* const a_pProgram)
{
    size_t EMP_TT_MAX_VAR(stNode);

    if (m_vstFreeNodes.count() > 0)
    {
        stNode = m_vstFreeNodes.last_val();
        m_vstFreeNodes.remove_last();
    }
    else
    {
        stNode = m_vNodes.count();
        emp::tt::silent(m_vNodes.push());
    }
    EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stNode));

    auto& rNode = m_vNodes.ref(stNode);
    rNode = node_c();

    if (a_pProgram != nullptr)
    { add_program_node(*a_pProgram, stNode); }


    /*c_size stLeaves = alloc_leaves();
    EMP_RET_FALSE_IF_NOT(m_vpvstLeaves.is_valid_index(stLeaves));

    EMP_RET_FALSE_IF(emp::tt::is_not_max(rNode.m_stLeaves) || rNode.m_pvstLeaves != nullptr);
    rNode.m_stLeaves = stLeaves;

    auto* const pLeaves = m_vpvstLeaves.val(stLeaves);
    EMP_RET_FALSE_IF(pLeaves == nullptr);

    pLeaves->resize(0);
    rNode.m_pvstLeaves = pLeaves;*/

    return stNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::dealloc_node(program_c& a_rProgram, c_size a_stNode)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));

    auto& rNode = m_vNodes.ref(a_stNode);
    EMP_RET_FALSE_IF(a_rProgram.m_pvstNodes == nullptr || a_rProgram.m_pvstNodes->is_valid_index(rNode.m_stProgramIndex) == false);
    a_rProgram.m_pvstNodes->set(rNode.m_stProgramIndex, emp::tt::max<size_t>::value);
    m_vstFreeNodes.add(a_stNode);

    c_size stLeaves = rNode.m_stLeaves;
    if (emp::tt::is_max(stLeaves))
    {
        EMP_RET_FALSE_IF(rNode.m_pvstLeaves != nullptr);
    }
    else
    {
        EMP_RET_FALSE_IF(rNode.m_pvstLeaves == nullptr);
        EMP_RET_FALSE_IF_NOT(m_vpvstLeaves.is_valid_index(stLeaves));

        auto* const pLeaves = m_vpvstLeaves.val(stLeaves);
        EMP_RET_FALSE_IF(rNode.m_pvstLeaves != pLeaves || pLeaves == nullptr);

        EMP_RET_FALSE_IF_NOT(dealloc_leaves(stLeaves));

        if (pLeaves->count() > 0)
        { // @@0 remove
            int kk = 0;
            ++kk;
        }
    }

    EMP_TT_MAX_VAR(rNode.m_stLeaves);
    rNode.m_pvstLeaves = nullptr;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::fill_node(c_size a_stNode, token_c const& a_rToken)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);

    rNode.m_pLexeme = a_rToken.m_pLexeme;
    rNode.m_sName = a_rToken.m_sName;
    rNode.m_Info = a_rToken.m_Info;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::fill_node(c_size a_stNode, lexeme_c* const a_pLexeme, info_s const& a_rInfo)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);

    rNode.m_pLexeme = a_pLexeme;
    rNode.m_Info = a_rInfo;

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN node_c* compiler_c::parse_declarations(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    c_size stDeclarationCount = a_vhTokens.count();
    EMP_RET_NULL_IF_NOT((stDeclarationCount > 0))

    node_c* pRoot = add_node(m_pSemiColon);

    {
        token_chc& hTokens = a_vhTokens.ref(0);
        c_size stTokenCount = hTokens.count();
        if (stTokenCount != 0)
        {
            node_c* const pStatement = build_ast(hTokens, a_si32Stack, a_rTable);
            EMP_RET_NULL_IF(pStatement == nullptr)

            if (pStatement->m_pLexeme == m_pSemiColon)
            { emp::cnt::copy(*pStatement->m_pvpLeaves, *pRoot->m_pvpLeaves); }
            else
            { pRoot->m_pvpLeaves->add(pStatement); }
        }
    }

    for (size_t stIndex = 1; stIndex < stDeclarationCount; ++stIndex)
    {
        node_c* const pDeclaration = parse_declaration(a_vhTokens.ref(stIndex), a_si32Stack, a_rTable);
        EMP_RET_NULL_IF(pDeclaration == nullptr)
        pRoot->m_pvpLeaves->add(pDeclaration);
    }

    EMP_RET_NULL_IF_NOT((pRoot->m_pvpLeaves->count() > 0))
    return pRoot;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::get_lexemes(table_c const& a_rTable EMP_UNUSED, vector<lexeme_s>& a_rvLexemes, token_chc& a_hTokens, c_si32 a_siStack)
{
    c_size stCount = a_hTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        token_c const& rToken = a_hTokens.ref(stIndex);
        if (rToken.m_siStack == a_siStack)
        {
            bool bPunctuation = false;
            lexeme_c* pLexeme = rToken.m_pLexeme;
            if (pLexeme == nullptr)
            { continue; }

            c_lexeme_e eLexeme = pLexeme->m_eType;
            size_t EMP_TT_MAX_VAR(stNewPrecedence);

            if (eLexeme == lexeme_e::Punctuation)
            {
                punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
                EMP_RET_FALSE_IF(pPunctuation == nullptr);
                stNewPrecedence = pPunctuation->m_ui32Precedence;
                bPunctuation = true;
            }
            else if (eLexeme == lexeme_e::Operation)
            {
                operation_c* const pOperation = to_lexeme<operation_c>(pLexeme);
                EMP_RET_FALSE_IF(pOperation == nullptr);
                stNewPrecedence = emp::tt::minof<size_t>(stNewPrecedence, pOperation->m_ui32Precedence);
            }
            else
            { continue; }

            if (emp::tt::is_not_max(stNewPrecedence))
            {
                if (a_rvLexemes.count() > 0)
                {
                    c_size stPrecedence = a_rvLexemes.ref(0).precedence;
                    if (stNewPrecedence > stPrecedence)
                    { continue; }
                    else if (stNewPrecedence < stPrecedence)
                    { a_rvLexemes.resize(0); }
                }

                if (a_rvLexemes.count() <= 0 || bPunctuation)
                {
                    lexeme_s& rLexeme = a_rvLexemes.push();
                    rLexeme.lexeme = pLexeme;
                    rLexeme.index = stIndex;
                    rLexeme.precedence = stNewPrecedence;
                }
            }
        }
    }
    return true;
}

// EMP_RETURN bool compiler_c::get_lexemes(table_c const& a_rTable, vector<lexeme_s>& a_rvLexemes, token_chc& a_hTokens, c_si32 a_siStack)
// {
//     bool bOperation = false;

//     c_size stCount = a_hTokens.count();
//     for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
//     {
//         token_c const& rToken = a_hTokens.ref(stIndex);
//         if (rToken.m_siStack == a_siStack)
//         {
//             bool bPunctuation = false;
//             c_bool bPrevious = bOperation;
//             bOperation = false;
//             lexeme_c* pLexeme = rToken.m_pLexeme;
//             if (pLexeme == nullptr)
//             { continue; }

//             c_lexeme_e eLexeme = pLexeme->m_eType;
//             size_t EMP_TT_MAX_VAR(stNewPrecedence);

//             if (eLexeme == lexeme_e::Punctuation)
//             {
//                 punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
//                 EMP_RET_FALSE_IF(pPunctuation == nullptr);
//                 stNewPrecedence = pPunctuation->m_ui32Precedence;
//                 bPunctuation = true;
//             }
//             else if (eLexeme == lexeme_e::Operation)
//             {
//                 bOperation = true;
//                 auto const pOverloads = a_rTable.get_overload(pLexeme->m_sName);
//                 auto const& vpLexemes = pOverloads->vector();

//                 c_size stOverloadCount = vpLexemes.count();
//                 for (size_t stOverload = 0; stOverload < stOverloadCount; ++stOverload)
//                 {
//                     lexeme_c* const pOverload = vpLexemes.val(stOverload);
//                     EMP_RET_FALSE_IF(pOverload == nullptr);

//                     c_lexeme_e eOverload = pOverload->m_eType;
//                     EMP_RET_FALSE_IF(eOverload != lexeme_e::Operation);

//                     operation_c* const pOperation = to_lexeme<operation_c>(pOverload);
//                     EMP_RET_FALSE_IF(pOperation == nullptr);

//                     c_size stOverloadPrecedence = pOperation->m_ui32Precedence;
//                     if (bPrevious)
//                     {
//                         rti::function_c const* const pFunction = pOperation->m_pFunctionType;
//                         EMP_RET_FALSE_IF(pFunction == nullptr);
//                         if (pFunction->m_vpParameters.count() == 1)
//                         {
//                             if (pOperation->m_ui32Precedence < stNewPrecedence)
//                             {
//                             goto GOTO_LABEL_ADD_LEXEME;
//                             // @@0 wtf
//                                 //EMP_XX_IF_NOT_IOS(stNewPrecedence = emp::tt::minof(stNewPrecedence, stOverloadPrecedence);
//                                 //    pLexeme = pOperation;)
//                             }
//                         }
//                     }
//                     else
//                     {
//                     GOTO_LABEL_ADD_LEXEME:
//                         stNewPrecedence = emp::tt::minof(stNewPrecedence, stOverloadPrecedence);
//                         pLexeme = pOperation;
//                     }
//                 }
//             }
//             else
//             { continue; }

//             if (emp::tt::is_not_max(stNewPrecedence))
//             {
//                 if (a_rvLexemes.count() > 0)
//                 {
//                     c_size stPrecedence = a_rvLexemes.ref(0).precedence;
//                     if (stNewPrecedence > stPrecedence)
//                     { continue; }
//                     else if (stNewPrecedence < stPrecedence)
//                     { a_rvLexemes.resize(0); }
//                 }

//                 if (a_rvLexemes.count() <= 0 || bPunctuation)
//                 {
//                     lexeme_s& rLexeme = a_rvLexemes.push();
//                     rLexeme.lexeme = pLexeme;
//                     rLexeme.index = stIndex;
//                     rLexeme.precedence = stNewPrecedence;
//                 }
//             }
//         }
//     }
//     return true;
// }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void get_shells(vector<lexeme_s> const& a_rvLexemes, token_chc& a_hFrom, token_vhc& a_rvTo);
void get_shells(vector<lexeme_s> const& a_rvLexemes, token_chc& a_hFrom, token_vhc& a_rvTo)
{
    c_size stLexemeCount = a_rvLexemes.count();

    size_t stPrevious = 0;
    for (size_t stIndex = 0; stIndex < stLexemeCount; ++stIndex)
    {
        lexeme_s const& rCurrent = a_rvLexemes.ref(stIndex);
        c_size stCurrent = rCurrent.index;

        token_hc hTokens;
        if (stCurrent >= stPrevious)
        {
            c_size stCount = stCurrent - stPrevious;
            if (stCount > 0)
            { hTokens = token_hc(a_hFrom.bulk<token_c const>(), stPrevious, stCount, true); }
        }

        a_rvTo.add(hTokens);
        EMP_OSE_DEBUG_SHELL(hTokens);

        stPrevious = stCurrent + 1;
        if (rCurrent.lexeme->m_eType == lexeme_e::Operation)
        { break; }
    }

    c_size stTokenCount = a_hFrom.count();
    if (stPrevious <= stTokenCount)
    {
        token_hc hTokens;
        c_size stCount = stTokenCount - stPrevious;
        if (stCount > 0)
        { hTokens = token_hc(a_hFrom.bulk<token_c const>(), stPrevious, stCount, true); }
        a_rvTo.add(hTokens);
        EMP_OSE_DEBUG_SHELL(hTokens);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::alloc_leaves(node_c& a_rNode)
{
    if (a_rNode.m_pvstLeaves == nullptr)
    {
        EMP_RET_FALSE_IF(emp::tt::is_not_max(a_rNode.m_stLeaves));
        c_size stLeaves = alloc_leaves();
        EMP_RET_FALSE_IF_NOT(m_vpvstLeaves.is_valid_index(stLeaves));
        a_rNode.m_stLeaves = stLeaves;
        a_rNode.m_pvstLeaves = m_vpvstLeaves.val(stLeaves);
        EMP_RET_FALSE_IF(a_rNode.m_pvstLeaves == nullptr);
    }
    else
    { EMP_RET_FALSE_IF(emp::tt::is_max(a_rNode.m_stLeaves)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::set_child_node(c_size a_stParent, c_size a_stChild, c_size a_stIndex)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stParent));
    node_c& rParent = m_vNodes.ref(a_stParent);
    EMP_RET_FALSE_IF(rParent.m_pvstLeaves == nullptr || emp::tt::is_max(rParent.m_stLeaves));
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stChild));
    rParent.m_pvstLeaves->set(a_stIndex, a_stChild);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::insert_child_node(c_size a_stParent, c_size a_stChild, c_size a_stIndex)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stParent));
    node_c& rParent = m_vNodes.ref(a_stParent);
    EMP_RET_FALSE_IF_NOT(alloc_leaves(rParent));
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stChild));
    rParent.m_pvstLeaves->insert(a_stIndex,a_stChild);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::add_child_node(c_size a_stParent, c_size a_stChild)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stParent));
    node_c& rParent = m_vNodes.ref(a_stParent);
    EMP_RET_FALSE_IF_NOT(alloc_leaves(rParent));
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stChild));
    rParent.m_pvstLeaves->add(a_stChild);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compiler_c::add_program_node(program_c& a_rProgram, c_size a_stNode)
{
    m_vNodes.ref(a_stNode).m_stProgramIndex = a_rProgram.m_pvstNodes->count();
    a_rProgram.m_pvstNodes->add(a_stNode);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::parse_parameters(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth, c_size a_stOut)
{
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stOut));

    c_size stLex = alloc_lexemes();
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    bool bDealloc = true;
    auto oFinally = emp::tt::finally([this, &bDealloc, stLex]()
    { bDealloc = dealloc_lexemes(stLex); });
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    auto* const pvLexemes = m_vpvLexemes.val(stLex);
    EMP_RET_FALSE_IF(pvLexemes == nullptr);

    EMP_RET_FALSE_IF_NOT(get_lexemes(a_rTable, *pvLexemes, a_hTokens, a_si32Stack));

    c_size stCount = pvLexemes->count();
    if (stCount > 0)
    {
        lexeme_s const& rLexeme = pvLexemes->ref(0);
        lexeme_c const* const pLexeme = rLexeme.lexeme;
        EMP_RET_FALSE_IF(pLexeme == nullptr);

        if (pLexeme == m_pComma)
        {
            token_vhc vhTokens;
            get_shells(*pvLexemes, a_hTokens, vhTokens);
            c_size stParameterCount = vhTokens.count();
            EMP_RET_FALSE_IF(stParameterCount < 2);
            for (size_t stIndex = 0; stIndex < stParameterCount; ++stIndex)
            {
                token_chc& hTokens = vhTokens.ref(stIndex);
                c_size stTokenCount = hTokens.count();
                EMP_RET_FALSE_IF_NOT((stTokenCount > 0) && (stIndex) < stParameterCount);

                if (stTokenCount > 0)
                {
                    c_size stParameter = parse_declaration(a_rProgram, hTokens, a_si32Stack, a_rTable, true, a_si32Depth);
                    EMP_RET_FALSE_IF(emp::tt::is_max(stParameter));
                    m_vNodes.ref(stParameter).m_eType = node_e::Declaration;
                    EMP_RET_FALSE_IF_NOT(add_child_node(a_stOut, stParameter));
                }
            }
            return bDealloc;
        }
    }
    else
    {
        c_size stParameter = parse_declaration(a_rProgram, a_hTokens, a_si32Stack, a_rTable, true, a_si32Depth);
        EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stParameter));
        EMP_RET_FALSE_IF_NOT(add_child_node(a_stOut, stParameter));
        return bDealloc;
    }

    info_s oInfo;
    if (a_hTokens.count() > 0)
    { oInfo = a_hTokens.ref(0).m_Info; }

    EMP_RET_FALSE_(EMP_OSE_INFO(oInfo, "Failed to parse paremeter list #0", print_shell(a_hTokens)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::copy_leaves(program_c& a_rProgram, c_size a_stFrom, c_size a_stTo)
{
    auto& rFrom = m_vNodes.ref(a_stFrom);
    auto& rTo = m_vNodes.ref(a_stTo);

    auto* const pTo = rTo.m_pvstLeaves;
    c_size stTo = rTo.m_stLeaves;

    rTo.m_pvstLeaves = rFrom.m_pvstLeaves;
    rTo.m_stLeaves = rFrom.m_stLeaves;

    rFrom.m_pvstLeaves = pTo;
    rFrom.m_stLeaves = stTo;

    return dealloc_node(a_rProgram, a_stFrom);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::parse_declaration(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, bool a_bLocal, c_si32 a_si32Depth)
{
    //EMP_LOG_INFO("#0", print_shell(a_hTokens));
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_MAX_IF_NOT(size_t, (a_hTokens.count() > 0));
    token_hc hTokens(a_hTokens);

    token_c const& rDeclaration = hTokens.ref(0);
    EMP_RET_MAX_IF(size_t, rDeclaration.m_pLexeme != m_pDeclare);
    hTokens.trim_left();
    EMP_RET_MAX_IF_NOT(size_t, (hTokens.count() > 0));

    token_c const& rName = hTokens.ref(0);
    EMP_RET_MAX_IF(size_t, rName.m_pLexeme != nullptr);
    hTokens.trim_left();
    EMP_RET_MAX_IF_NOT(size_t, (hTokens.count() > 0));

    c_size stName = alloc_node(&a_rProgram);
    EMP_RET_MAX_IF(size_t, emp::tt::is_max(stName));
    EMP_RET_MAX_IF_NOT(size_t, fill_node(stName, rName));

    size_t EMP_TT_MAX_VAR(stWeight);
    float fVariant = 0.0f;

    c_size stLex = alloc_lexemes();
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    bool bDealloc = true;
    auto oFinally = emp::tt::finally([this, &bDealloc, stLex]()
    { bDealloc = dealloc_lexemes(stLex); });
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    auto* const pvLexemes = m_vpvLexemes.val(stLex);
    EMP_RET_FALSE_IF(pvLexemes == nullptr);

    EMP_RET_MAX_IF_NOT(size_t, get_lexemes(a_rTable, *pvLexemes, hTokens, a_si32Stack));

    token_hc hBody;

    if (pvLexemes->count() > 0)
    {
        lexeme_s const& rLexeme = pvLexemes->ref(0);
        lexeme_c const* const pLexeme = rLexeme.lexeme;
        if (pLexeme == m_pAssign)
        {
            token_vhc vhTokens;
            get_shells(*pvLexemes, hTokens, vhTokens);
            c_size stShellCount = vhTokens.count();
            EMP_RET_X_IF(emp::tt::max<size_t>::value, stShellCount != 2);

            hTokens = vhTokens.ref(0);
            hBody = vhTokens.ref(1);
        }
        else
        {
            EMP_RET_MAX_(size_t, EMP_OSE_TOKEN(rDeclaration, "Invalid declaration missing assignment #0", m_vNodes.ref(stName).m_sName));
        }
    }

    c_bool bHasBody = hBody.count() > 0;

    // @@0 BUG using the body to detect if its a function or value...
    // @@0 BUG not working without body for function parameters
    // @@0 BUG initalizer list.
    c_bool bFunction = bHasBody && hBody.ref(0).m_pLexeme == m_pLeftBrace;

    size_t EMP_TT_MAX_VAR(stBody);
    if (bHasBody)
    {
        auto const& rBodyToken = hBody.ref(0);

        stBody = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stBody));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stBody, m_pLeftBrace, rBodyToken.m_Info));

        if (bFunction)
        { hBody.trim_left(); }
        if (hBody.count() > 0)
        {
            c_size stChild = build_ast(a_rProgram, hBody, a_si32Stack + (bFunction ? 1 : 0), a_rTable, a_si32Depth + 1);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stChild), EMP_OSE_NODE(m_vNodes.ref(stBody), "Empty body : #0", m_vNodes.ref(stBody).m_sName));

            if (m_vNodes.ref(stChild).m_pLexeme == m_pSemiColon)
            { EMP_RET_MAX_IF_NOT(size_t, copy_leaves(a_rProgram, stChild, stBody)); }
            else
            { EMP_RET_MAX_IF_NOT(size_t, add_child_node(stBody, stChild)); }
        }
    }

    lexeme_c* pDeclaredLexeme = nullptr;
    if (bFunction)
    {
        if (hTokens.count() > 0)
        {
            token_c const& rWeight = hTokens.ref(0);
            if (rWeight.m_eType == token_e::IntegralLiteral ||
                rWeight.m_eType == token_e::RealLiteral)
            {
                literal_t<float>* const pWeight = a_rProgram.m_Table.reg_literal<float>(rWeight.m_sName);
                EMP_RET_MAX_IF_NOT(size_t, pWeight != nullptr, EMP_OSE_TOKEN(rWeight, "invalid weight type #0", rWeight.m_sName));

                hTokens.trim_left();
                fVariant = pWeight->m_tLiteral;

                stWeight = alloc_node(&a_rProgram);
                EMP_RET_MAX_IF(size_t, emp::tt::is_max(stWeight));
                EMP_RET_MAX_IF_NOT(size_t, fill_node(stWeight, pWeight, rWeight.m_Info));
                EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stWeight));
            }
        }

        token_hc hParameters;
        EMP_RET_MAX_IF_NOT(size_t, eat_right_shell(0, m_pLeftPar, m_pRightPar, hTokens, hParameters));

        size_t EMP_TT_MAX_VAR(stParameters);
        if (hParameters.count() > 0)
        {
            stParameters = alloc_node(&a_rProgram);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stParameters));
            EMP_RET_MAX_IF_NOT(size_t, fill_node(stParameters, m_pRightPar, hParameters.ref(0).m_Info));

            EMP_RET_MAX_IF_NOT(size_t, parse_parameters(a_rProgram, hParameters, a_si32Stack + 1, a_rTable, a_si32Depth + 1, stParameters));

            EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stParameters));
        }

        size_t EMP_TT_MAX_VAR(stReturns);

        token_hc hReturns;
        EMP_RET_MAX_IF_NOT(size_t, eat_right_shell(0, m_pLeftPar, m_pRightPar, hTokens, hReturns));
        if (hReturns.count() > 0)
        {
            stReturns = alloc_node(&a_rProgram);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stReturns));
            EMP_RET_MAX_IF_NOT(size_t, fill_node(stReturns, m_pLeftPar, hTokens.ref(0).m_Info));

            EMP_RET_MAX_IF_NOT(size_t, parse_parameters(a_rProgram, hReturns, a_si32Stack + 1, a_rTable, a_si32Depth, stReturns),
                EMP_OSE_TOKEN(rDeclaration, "Invalid return list '#0'", print_shell(hTokens)));

            EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stReturns));
        }

        rti::function_c const* pDefinedFunction = nullptr;
        rti::function_c* const pFunction = a_rTable.new_type<rti::function_c>();
        EMP_RET_MAX_IF(size_t, pFunction == nullptr);

        if (emp::tt::is_not_max(stReturns))
        {
            auto const* const pvstLeaves = m_vNodes.ref(stReturns).m_pvstLeaves;
            c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                EMP_RET_MAX_IF_NOT(size_t, pFunction->add_return(
                    get_type(m_vNodes.ref(pvstLeaves->val(stIndex)).m_pLexeme)));
            }
        }

        if (emp::tt::is_not_max(stParameters))
        {
            auto const& pvstLeaves = m_vNodes.ref(stParameters).m_pvstLeaves;
            c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                lexeme_c const* const pParameterLexeme = m_vNodes.ref(pvstLeaves->val(stIndex)).m_pLexeme;
                EMP_RET_MAX_IF(size_t, pParameterLexeme == nullptr);

                rti::type_c const* const pParameterType = get_type(pParameterLexeme);
                EMP_RET_MAX_IF(size_t, pParameterType == nullptr);

                EMP_RET_MAX_IF_NOT(size_t, pFunction->add_parameter(pParameterType));
            }
        }

        EMP_RET_MAX_IF(size_t, pFunction->m_sName.len() > 0);
        EMP_RET_MAX_IF_NOT(size_t, a_rTable.qualify_type(pFunction));
        EMP_RET_MAX_IF_NOT(size_t, (pFunction->m_sName.len() > 0));
        pDefinedFunction = a_rTable.get_function_type(pFunction->m_sName);
        if (pDefinedFunction == nullptr)
        {
            EMP_RET_MAX_IF_NOT(size_t, a_rTable.reg_type(pFunction));
            pDefinedFunction = pFunction;
        }

        EMP_RET_MAX_IF(size_t, pDefinedFunction == nullptr);

        lexeme_c* const pLexeme = a_rTable.get_lexeme(m_vNodes.ref(stName).m_sName, pDefinedFunction);

        variant_c* pVariant = nullptr;
        if (pLexeme != nullptr)
        {
            pVariant = to_function<variant_c>(pLexeme);
            if (emp::tt::is_not_max(stWeight))
            {
                if (pVariant == nullptr)
                { EMP_RET_MAX_(size_t, EMP_OSE_NODE(rName, "'@#0' is already defined as NON variant", m_vNodes.ref(stName).m_sName)); }
                pDeclaredLexeme = pVariant;
            }
            else if (pVariant == nullptr)
            { EMP_RET_MAX_(size_t, EMP_OSE_NODE(rName, "duplicate symbol '@#0'", m_vNodes.ref(stName).m_sName)); }
            else
            { EMP_RET_MAX_(size_t, EMP_OSE_NODE(rName, "'@#0' is already defined AS variant", m_vNodes.ref(stName).m_sName)); }
        }
        else if (emp::tt::is_not_max(stWeight))
        {
            pVariant = a_rTable.reg_variant(m_vNodes.ref(stName).m_sName, pDefinedFunction);
            pDeclaredLexeme = pVariant;
        }
        else
        { pDeclaredLexeme = a_rTable.reg_function(m_vNodes.ref(stName).m_sName, pDefinedFunction); }

        if (pVariant != nullptr)
        {
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stWeight));
            pVariant->add(fVariant);
        }
        else if (pDeclaredLexeme != nullptr)
        { EMP_RET_MAX_IF(size_t, emp::tt::is_not_max(stWeight)); }
        else
        { EMP_RET_MAX_(size_t, EMP_OSE_NODE(rName, "failed to define symbol '@#0'", m_vNodes.ref(stName).m_sName)); }
    }
    else
    {
        EMP_RET_MAX_IF(size_t, hTokens.count() != 1, EMP_OSE_TOKEN(rDeclaration,
         "Invalid declaration #0", m_vNodes.ref(stName).m_sName));

        token_c const& rType = hTokens.ref(0);
        hTokens.trim_left();

        rti::type_c const* const pType = a_rTable.get_type(rType.m_sName);
        EMP_RET_MAX_IF(size_t, pType == nullptr);

        rti::data_c const* const pDataType = rti::to_type<rti::data_c>(pType);
        EMP_RET_MAX_IF(size_t, pDataType == nullptr);

        /*EMP_XX_IF_DEBUG(*/EMP_XX_IF_LOG(c_astring& sType = pDataType->m_sName;)/*)*/

        lexeme_c const* const pLexeme = a_rTable.get_lexeme(rName.m_sName, pDataType);
        if (pLexeme != nullptr)
        { EMP_RET_MAX_(size_t, EMP_OSE_NODE(rName, "duplicate symbol '@#0 #1'", m_vNodes.ref(stName).m_sName, sType)); }

        data_c* const pData = a_rTable.reg_data(m_vNodes.ref(stName).m_sName, pDataType);
        EMP_RET_MAX_IF(size_t, pData == nullptr);
        pData->m_bLocal = a_bLocal;
        pDeclaredLexeme = pData;
    }

    EMP_RET_MAX_IF(size_t, pDeclaredLexeme == nullptr);
    m_vNodes.ref(stName).m_pLexeme = pDeclaredLexeme;
    m_vNodes.ref(stName).m_eType = node_e::DefinitionU;
    if (bHasBody)
    {
        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stBody));
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stBody));
    }

    return stName;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::parse_scope_or_control(program_c& a_rProgram, token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth)
{
    c_size stShellCount = a_vhTokens.count();
    EMP_RET_MAX_IF_NOT(size_t, (stShellCount > 0));

    if (stShellCount == 1)
    {
        token_chc hTokens = a_vhTokens.ref(0);
        if (hTokens.count() == 0)
        {
            c_size stNode = alloc_node(&a_rProgram);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stNode));
            EMP_RET_MAX_IF_NOT(size_t, fill_node(stNode, nullptr, info_s()));
            return stNode;
        }
        else
        { return build_ast(a_rProgram, hTokens, a_si32Stack, a_rTable, a_si32Depth); }
    }
    else
    {
        size_t EMP_TT_MAX_VAR(stControl);

        token_chc& hControl = a_vhTokens.ref(0);
        info_s oInfo;
        if (hControl.count() > 0)
        {
            oInfo = hControl.ref(0).m_Info;
            stControl = parse_call(a_rProgram, hControl, a_si32Stack, a_rTable, a_si32Depth);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stControl));
        }

        c_size stBody = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stBody));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stBody, m_pLeftBrace, oInfo));

        token_chc& hBody = a_vhTokens.ref(1);
        if (hBody.count() > 0)
        {
            c_size stChild = build_ast(a_rProgram, hBody, a_si32Stack + 1, a_rTable, a_si32Depth + 1);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stChild));

            if (m_vNodes.ref(stChild).m_pLexeme == m_pSemiColon)
            { EMP_RET_MAX_IF_NOT(size_t, copy_leaves(a_rProgram, stChild, stBody)); }
        }

        if (emp::tt::is_not_max(stControl))
        {
            lexeme_c const* const pLexeme = m_vNodes.ref(stControl).m_pLexeme;
            if (pLexeme != nullptr &&
                pLexeme->m_eType != lexeme_e::Control)
            {
                c_size stRoot = alloc_node(&a_rProgram);
                EMP_RET_MAX_IF(size_t, emp::tt::is_max(stRoot));
                EMP_RET_MAX_IF_NOT(size_t, fill_node(stRoot, m_pSemiColon, oInfo));

                EMP_RET_MAX_IF_NOT(size_t, add_child_node(stRoot, stControl));
                EMP_RET_MAX_IF_NOT(size_t, add_child_node(stRoot, stBody));

                return stRoot;
            }

            EMP_RET_MAX_IF_NOT(size_t, add_child_node(stControl, stBody));
            return stControl;
        }

        return stBody;
    }

    EMP_LOG_ERROR("failed to parse scope or control");
    EMP_RET_MAX(size_t);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::parse_statements(token_cvhc& EMP_SILENT(a_vhTokens), c_si32 EMP_SILENT(a_si32Stack),
    table_c& EMP_SILENT(a_rTable))
{
    //EMP_LOG_ERROR(EMP_XX_FUNCTION);
    //EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_MAX(size_t);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::parse_assignment(token_cvhc& EMP_SILENT(a_vhTokens), c_si32 EMP_SILENT(a_si32Stack),
    table_c& EMP_SILENT(a_rTable))
{
    //EMP_LOG_ERROR(EMP_XX_FUNCTION);
    //EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_MAX(size_t);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN node_c* compiler_c::parse_calls(program_c& a_rProgram, token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    c_size stStatementCount = a_vhTokens.count();
    EMP_RET_NULL_IF_NOT((stStatementCount > 0));

    node_c* pRoot = fill_node(a_rProgram.add_node(), m_pSemiColon);

    for (size_t stIndex = 0; stIndex + 1 < stStatementCount; ++stIndex)
    {
        node_c* const pCall = parse_call(a_vhTokens.ref(stIndex), a_si32Stack, a_rTable);
        EMP_RET_NULL_IF(pCall == nullptr);
        pRoot->m_pvpLeaves->add(pCall);
    }

    {
        token_chc& hTokens = a_vhTokens.last_ref();
        size_t stTokenCount = hTokens.count();
        if (stTokenCount != 0)
        {
            node_c* const pStatement = build_ast(hTokens, a_si32Stack, a_rTable);
            EMP_RET_NULL_IF(pStatement == nullptr);

            if (pStatement->m_pLexeme == m_pSemiColon)
            { emp::cnt::copy(*pStatement->m_pvpLeaves, *pRoot->m_pvpLeaves); }
            else
            { pRoot->m_pvpLeaves->add(pStatement); }
        }
    }

    EMP_RET_NULL_IF_NOT((pRoot->m_pvpLeaves->count() > 0));
    return pRoot;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::parse_call(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, c_si32 a_si32Depth)
{
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_MAX_IF_NOT(size_t, (a_hTokens.count() > 0));

    token_hc hTokens(a_hTokens);
    token_c const& rName = hTokens.ref(0);
    c_bool bRealCall = rName.m_pLexeme != m_pLeftPar;

    if (bRealCall)
    { hTokens.trim_left(); }

    token_hc hArguments;
    EMP_RET_MAX_IF_NOT(size_t, eat_right_shell(a_si32Stack, m_pLeftPar, m_pRightPar, hTokens, hArguments));

    if (bRealCall == false)
    { return build_ast(a_rProgram, hArguments, a_si32Stack + 1, a_rTable, a_si32Depth + 1); }

    c_size stName = alloc_node(&a_rProgram);
    EMP_RET_MAX_IF(size_t, emp::tt::is_max(stName));
    EMP_RET_MAX_IF_NOT(size_t, fill_node(stName, rName));

    {
        c_size stArgumentCount = hArguments.count();
        if (stArgumentCount > 0)
        {
            c_size stArgument = build_ast(a_rProgram, hArguments, a_si32Stack + 1, a_rTable, a_si32Depth + 1);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stArgument));

            if (m_vNodes.ref(stArgument).m_pLexeme == m_pComma)
            {
                c_size stCommaCount = [this, hArguments, stArgumentCount]() EMP_RETURN -> size_t
                {
                    size_t stCount = 0;
                    for (size_t stIndex = 0; stIndex < stArgumentCount; ++stIndex)
                    {
                        if (hArguments.ref(stIndex).m_pLexeme == m_pComma)
                        { ++stCount; }
                    }
                    return stCount;
                }();
                EMP_RET_MAX_IF(size_t, m_vNodes.ref(stArgument).m_pvstLeaves == nullptr, "invalid argument list : '#0'", rName.m_sName);
                EMP_RET_MAX_IF(size_t, (stCommaCount + 1) != m_vNodes.ref(stArgument).m_pvstLeaves->count(),
                "invalid argument list : '#0'", rName.m_sName);
                EMP_RET_MAX_IF_NOT(size_t, copy_leaves(a_rProgram, stArgument, stName));
            }
            else
            { EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stArgument)); }
        }
    }

    if (str::cmp(rName.m_sName, astring(ALITERAL("hsb"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("sba"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("hs"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Bright = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Bright = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Bright));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Bright, pLiteral_Bright, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Bright));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("hb"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Sat = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Sat = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Sat));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Sat, pLiteral_Sat, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Sat, 1));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("sb"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("sa"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Bright = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Bright = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Bright));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Bright, pLiteral_Bright, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Bright, 2));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("ba"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Sat = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Sat = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Sat));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Sat, pLiteral_Sat, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Sat, 1));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("h"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Sat = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Sat = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Sat));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Sat, pLiteral_Sat, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Sat));

        lexeme_c* const pLiteral_Bright = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Bright = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Bright));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Bright, pLiteral_Bright, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Bright));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("u"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Bright = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Bright = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Bright));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Bright, pLiteral_Bright, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Bright));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("b"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Sat = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Sat = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Sat));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Sat, pLiteral_Sat, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Sat, 1));

        lexeme_c* const pLiteral_Alpha = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Alpha = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Alpha));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Alpha, pLiteral_Alpha, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Alpha));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("a"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("hsba()(float,float,float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Hue = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Hue = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Hue));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Hue, pLiteral_Hue, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Hue, 0));

        lexeme_c* const pLiteral_Sat = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Sat = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Sat));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Sat, pLiteral_Sat, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Sat, 1));

        lexeme_c* const pLiteral_Bright = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Bright = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Bright));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Bright, pLiteral_Bright, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_Bright, 2));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("x"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("t()(float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Y = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_Y = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Y));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Y, pLiteral_Y, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Y));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("y"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("t()(float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_X = a_rTable.reg_literal<float>(0.0);
        c_size stLiteral_X = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_X));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_X, pLiteral_X, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_X, 0));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("sx"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("s()(float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_Y = a_rTable.reg_literal<float>(1.0);
        c_size stLiteral_Y = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_Y));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_Y, pLiteral_Y, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, add_child_node(stName, stLiteral_Y));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("sy"))))
    {
        lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("s()(float,float,)")));
        node_c& rNewNode = m_vNodes.ref(stName);
        rNewNode.m_pLexeme = pNewLexeme;
        rNewNode.m_sName = pNewLexeme->m_sName;

        lexeme_c* const pLiteral_X = a_rTable.reg_literal<float>(1.0);
        c_size stLiteral_X = alloc_node(&a_rProgram);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_X));
        EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_X, pLiteral_X, {0, 0})); //@@1 invalid line/column number
        EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_X, 0));
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("t"))))
    {
        // node_c& rNewNode = m_vNodes.ref(stName);
        // if (rNewNode.m_pvstLeaves != nullptr &&
        //     rNewNode.m_pvstLeaves->count() == 1)
        //     {
        //         lexeme_c* const pNewLexeme = a_rTable.get_lexeme(astring(ALITERAL("t()(float,float,)")));
        //         rNewNode.m_pLexeme = pNewLexeme;
        //         rNewNode.m_sName = pNewLexeme->m_sName;

        //         node_c& rLeafNode = m_vNodes.ref(rNewNode.m_pvstLeaves->val(0));
        //         lexeme_c* const pLiteral_X = a_rTable.reg_literal<float>(0.0);
        //         c_size stLiteral_X = alloc_node(&a_rProgram);
        //         EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLiteral_X));
        //         EMP_RET_MAX_IF_NOT(size_t, fill_node(stLiteral_X, pLiteral_X, {0, 0})); //@@1 invalid line/column number
        //         EMP_RET_MAX_IF_NOT(size_t, insert_child_node(stName, stLiteral_X, 0));
        //     }
    }
    else if (str::cmp(rName.m_sName, astring(ALITERAL("t"))))
    {
    }




    if (hTokens.count() == 0)
    {
        m_vNodes.ref(stName).m_eType = node_e::Call;
        return stName;
    }

    EMP_RET_MAX_(size_t, EMP_OSE_TOKEN(rName, "invalid function call : '#0'", rName.m_sName));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::eat_token(table_c& a_rTable, astring& a_rsToken, lexeme_c*& a_rpLexeme, token_e& a_reToken)
{
    a_rpLexeme = nullptr;
    a_reToken = token_e::none;

    char cChar = 0;

    c_astring& sExpression(m_Context.m_sExpression);
    a_rsToken = sExpression;

    size_t stLen = sExpression.len();
    if ((stLen > 0) == false)
    { return true; }

    while (true)
    {
        cChar = m_Context.get_char();
        if (is_newline(cChar))
        { m_Context.add_line(); }
        else if (is_white(cChar) == false)
        { break; }

        stLen = m_Context.eat_char();
        if ((stLen > 0) == false)
        {
            a_rsToken = astring();
            return true;
        }

        a_rsToken.trim_left();
    }

#define XTMP_OSE_PARSE(x_Token)\
if (stLen > 0) { a_rsToken.trim_right(stLen); }\
a_reToken = x_Token;

    if (is_digit(cChar))
    {
        stLen = m_Context.eat_char();
        if (stLen > 0)
        {
            c_char cLast = cChar;
            cChar = m_Context.get_char();
            if (cLast == '0' && cChar == 'x')
            {
                while (true)
                {
                    stLen = m_Context.eat_char();
                    if ((stLen > 0) == false)
                    { break; }
                    cChar = m_Context.get_char();

                    if (is_digit(cChar) ||
                        is_hexa(cChar))
                    { }
                    else if (is_not_hexa(cChar))
                    {
                        EMP_LOG_ERROR("invalid token");
                        return false;
                    }
                    else
                    { break; }
                }

                XTMP_OSE_PARSE(token_e::IntegralLiteral)
                return true;

            }
            else
            {
                bool bDot = false;
                while (true)
                {
                    if (is_digit(cChar))
                    { }
                    else if (is_dot(cChar))
                    {
                        if (bDot)
                        { break; }
                        else
                        { bDot = true; }
                    }
                    else if (is_newline(cChar))
                    { break; }
                    else if (is_letter(cChar))
                    {
                        EMP_LOG_ERROR("invalid token");
                        return false;
                    }
                    else
                    { break; }

                    stLen = m_Context.eat_char();
                    if ((stLen > 0) == false)
                    { break; }
                    cChar = m_Context.get_char();
                }

                if (bDot)
                {
                    XTMP_OSE_PARSE(token_e::RealLiteral)
                    return true;
                }

                XTMP_OSE_PARSE(token_e::IntegralLiteral)
                return true;
            }
        }
        else
        {
            XTMP_OSE_PARSE(token_e::IntegralLiteral)
            return true;
        }
    }
    else if (is_dot(cChar))
    {
        while (true)
        {
            stLen = m_Context.eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_Context.get_char();

            if (is_digit(cChar))
            { }
            else if (is_newline(cChar))
            { break; }
            else if (is_letter(cChar))
            {
                EMP_LOG_ERROR("invalid token");
                return false;
            }
            else
            { break; }
        }

        XTMP_OSE_PARSE(token_e::RealLiteral)
        return true;
    }
    else if (is_letter(cChar))
    {
        while (true)
        {
            stLen = m_Context.eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_Context.get_char();

            if (is_letter(cChar) ||
                is_digit(cChar))
            { }
            else
            { break; }
        }

        XTMP_OSE_PARSE(token_e::none);
        a_rpLexeme = a_rTable.get_match(a_rsToken);
        return true;
    }
    else
    {
        bool bOperator = false;
        while (true)
        {
            astring sComment(sExpression);
            c_size stTrim = emp::tt::maxof<size_t>(sComment.len(), 2) - 2;
            if (stTrim > 0)
            { sComment.trim_right(stTrim); }
            comment_c* const pComment = a_rTable.get_comment(sComment);
            if (pComment != nullptr)
            {
                if (bOperator)
                { break; }

                a_rsToken = sComment;
                a_rpLexeme = pComment;
                if ((m_Context.eat_char() > 1) == false)
                { return false; }
                emp::tt::silent(m_Context.eat_char());
                return true;
            }

            stLen = m_Context.eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_Context.get_char();

            if (is_digit(cChar) ||
                is_letter(cChar) ||
                is_dot(cChar) ||
                is_white(cChar))
            { break; }
            else if (is_newline(cChar))
            { break; }

            bOperator = true;
        }

        XTMP_OSE_PARSE(token_e::Operator);
        return true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_tokens(program_c& a_rProgram)
{
    while (true)
    {
        c_size stStartLen = m_Context.m_sExpression.len();
        if ((stStartLen > 0) == false)
        { break; }

        token_c oToken;

        astring sToken;
        EMP_RET_FALSE_IF_NOT(eat_token(a_rProgram.m_Table, sToken, oToken.m_pLexeme, oToken.m_eType));
        if ((sToken.len() > 0) == false)
        { break; }

        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("eat_token '#0'", sToken);)

        c_astring sLexeme(sToken);
        if (oToken.m_pLexeme != nullptr)
        {
            if (oToken.m_pLexeme->m_bReserved)
            {
                emp::tt::silent(m_Context.resolve(oToken, false));
                EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "reserved keyword '#0'", sToken));
            }

            c_lexeme_e eLexeme = oToken.m_pLexeme->m_eType;
            switch (eLexeme)
            {
                case lexeme_e::Comment:
                {
                    switch (get_comment(oToken.m_pLexeme))
                    {
                        case comment_e::SingleComment:
                        {
                            size_t stLen = m_Context.m_sExpression.len();
                            while (stLen > 0)
                            {
                                c_char cChar = m_Context.get_char();
                                stLen = m_Context.eat_char();
                                if (is_newline(cChar))
                                {
                                    m_Context.add_line();
                                    break;
                                }
                            }
                            break;
                        }
                        case comment_e::BeginComment:
                        {
                            int siCommentStack = 1;

                            size_t stLen = m_Context.m_sExpression.len();
                            while (stLen > 0)
                            {
                                c_char cChar = m_Context.get_char();

                                if (is_newline(cChar))
                                { m_Context.add_line(); }
                                else
                                {
                                    astring sComment(m_Context.m_sExpression);
                                    c_size stCommentLen = emp::tt::maxof<size_t>(sComment.len(), 2) - 2;
                                    if (stCommentLen > 0)
                                    { sComment.trim_right(stCommentLen); }
                                    comment_c const* const pComment = a_rProgram.m_Table.get_comment(sComment);
                                    if (pComment != nullptr)
                                    {
                                        switch (pComment->m_eCommentType)
                                        {
                                            case comment_e::BeginComment:
                                            {
                                                ++siCommentStack;
                                                break;
                                            }
                                            case comment_e::EndComment:
                                            {
                                                --siCommentStack;
                                                break;
                                            }
                                            case comment_e::SingleComment:
                                            { break; }
                                            case comment_e::none:
                                            default:
                                            { return false; }
                                        };
                                        stLen = m_Context.eat_char(); // @@note not using stLen but eating char
                                        if ((siCommentStack > 0) == false)
                                        {
                                            stLen = m_Context.eat_char(); // @@note not using stLen but eating char
                                            break;
                                        }
                                    }
                                }
                                stLen = m_Context.eat_char();
                            }

                            if (siCommentStack != 0)
                            {
                                emp::tt::silent(m_Context.resolve(oToken, false));
                                EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "Invalid comment '#0'", sToken));
                            }
                            break;
                        }
                        case comment_e::EndComment:
                        case comment_e::none:
                        default:
                        {
                            emp::tt::silent(m_Context.resolve(oToken, false));
                            EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "invalid comment '#0'", sToken));
                        }
                    }
                    break;
                }
                case lexeme_e::Literal:
                case lexeme_e::Control:
                case lexeme_e::Intrinsic:
                {
                    oToken.m_sName = sToken;
                    EMP_RET_FALSE_IF_NOT(m_Context.resolve(oToken, false));
                    m_vTokens.add(oToken);
                    break;
                }
                case lexeme_e::Punctuation:
                case lexeme_e::Operation:
                case lexeme_e::Function:
                case lexeme_e::Variant:
                case lexeme_e::Data:
                case lexeme_e::none:
                default:
                { EMP_RET_FALSE_("Invalid lexeme_e::'#0 : #1'", enum_to_astring(eLexeme), enum_to(eLexeme)); }
            }
        }
        else if (oToken.m_eType == token_e::Operator)
        {
            size_t stParsedCount = 0;
            while (sToken.len())
            {
                oToken.m_pLexeme = a_rProgram.m_Table.get_match(sToken);
                if (oToken.m_pLexeme != nullptr)
                {
                    oToken.m_sName = sToken;
                    if (m_Context.resolve(oToken, true) == false)
                    { EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "failed to resolve stack '#0'", oToken.m_sName)); }

                    stParsedCount += sToken.len();
                    oToken.m_Info.m_stColumn = oToken.m_Info.m_stColumn - sLexeme.len() + stParsedCount;

                    m_vTokens.add(oToken);

                    sToken = sLexeme;
                    sToken.trim_left(stParsedCount);
                }
                else
                { sToken.trim_right(); }
            }

            if (stParsedCount != sLexeme.len())
            {
                emp::tt::silent(m_Context.resolve(oToken, false));
                EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "invalid operator '#0'", sLexeme));
            }

            if (oToken.m_pLexeme != nullptr &&
                oToken.m_pLexeme->m_bReserved)
            {
                emp::tt::silent(m_Context.resolve(oToken, false));
                EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "reserved operator '#0'", oToken.m_sName));
            }
        }
        else
        {
            oToken.m_sName = sToken;
            EMP_RET_FALSE_IF_NOT(m_Context.resolve(oToken, false));
            m_vTokens.add(oToken);
        }

        if (stStartLen == m_Context.m_sExpression.len())
        {
            emp::tt::silent(m_Context.resolve(oToken, false));
            EMP_RET_FALSE_(EMP_OSE_TOKEN(oToken, "unhandled token '#0'", sToken));
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::clean_tokens(program_c& a_rProgram EMP_UNUSED)
{
    c_size stCount = m_vTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        token_c& rToken = m_vTokens.ref(stIndex);
        auto* const pLexeme = rToken.m_pLexeme;
        if (pLexeme == nullptr ||
            pLexeme->m_eType != lexeme_e::Operation)
        { continue; }

        c_bool bUnary = [stIndex, this]()
        {
            if (stIndex == 0)
            { return true; }

            auto const& rPrevious = m_vTokens.ref(stIndex - 1);
            auto const* const pPrevious = rPrevious.m_pLexeme != nullptr ? rPrevious.m_pLexeme : nullptr;
            if (pPrevious != nullptr)
            {
                auto const ePrevious = pPrevious->m_eType;
                if ((ePrevious == lexeme_e::Punctuation &&
                     pPrevious != m_pRightPar) ||
                    ePrevious == lexeme_e::Operation)
                { return true; }
            }

            return false;
        }();

        auto const pOverloads = a_rProgram.m_Table.get_overload(pLexeme->m_sName);
        auto const& vpLexemes = pOverloads->vector();

        c_size stOverloadCount = vpLexemes.count();
        for (size_t stOverload = 0; stOverload < stOverloadCount; ++stOverload)
        {
            lexeme_c* const pOverload = vpLexemes.val(stOverload);
            EMP_RET_FALSE_IF(pOverload == nullptr);

            c_lexeme_e eOverload = pOverload->m_eType;
            EMP_RET_FALSE_IF(eOverload != lexeme_e::Operation);

            operation_c* const pOperation = to_lexeme<operation_c>(pOverload);
            EMP_RET_FALSE_IF(pOperation == nullptr);

            rti::function_c const* const pFunction = pOperation->m_pFunctionType;
            EMP_RET_FALSE_IF(pFunction == nullptr);
            c_size stParameterCount = pFunction->m_vpParameters.count();
            EMP_RET_FALSE_IF_NOT(stParameterCount > 0);

            c_bool bOverloadUnary = pFunction->m_vpParameters.count() == 1;
            if (bUnary == bOverloadUnary)
            {
                m_vTokens.ref(stIndex).m_pLexeme = pOperation;
                break;
            }
        }
    }
    /*m_vpResolveStack.clear();
    m_vpResolveStack.add({a_stNode, 0});

    auto const l_clean = [](c_size a_stNode) EMP_RETURN -> bool
    {
        if (m_vNodes.ref(a_stNode).m_pLexeme == )
    };
    while (true)
    {
        if (m_vpResolveStack.count() == 0)
        { break; }

        c_size stCount = m_vpResolveStack.count();
        slice_s* pSlice = m_vpResolveStack.last_ptr();
        EMP_RET_FALSE_IF(emp::tt::is_max(pSlice->node));

        node_c& rNode = m_vNodes.ref(pSlice->node);
        if (rNode.m_eType == node_e::Call)
        auto const* const pvstLeaves = m_vNodes.ref(pSlice->node).m_pvstLeaves;
        if (pvstLeaves != nullptr && pSlice->child < pvstLeaves->count())
        {
            m_vpResolveStack.add({pvstLeaves->val(pSlice->child), 0});
            m_vpResolveStack.ref(stCount - 1).child++;
            continue;
        }
        else
        {
            EMP_RET_FALSE_IF_NOT(l_clean(pSlice->node));
            m_vpResolveStack.remove_last();
        }
    }*/

    return true;
}

#undef XTMP_OSE_CAST
#undef XTMP_OSE_ARG
#undef XTMP_OSE_OVERLOAD


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_call(lexeme_c const* const a_pLexeme);
EMP_RETURN bool is_call(lexeme_c const* const a_pLexeme)
{
    EMP_RET_FALSE_IF(a_pLexeme == nullptr);
    c_lexeme_e eLexeme = a_pLexeme->m_eType;
    switch (eLexeme)
    {
        case lexeme_e::Operation:
        case lexeme_e::Intrinsic:
        case lexeme_e::Control:
        case lexeme_e::Function:
        case lexeme_e::Variant:
        { return true; }
        case lexeme_e::Comment:
        case lexeme_e::Literal:
        case lexeme_e::Punctuation:
        case lexeme_e::Data:
        { return false; }
        case lexeme_e::none:
        default:
        {
            EMP_LOG_ERROR("unhandled lexeme '#0' : #1", enum_to_astring(eLexeme), enum_to(eLexeme));
            return false;
        }
    };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::build_ast(program_c& a_rProgram, token_chc& a_hTokens, c_si32 a_siStack, table_c& a_rTable, c_si32 a_si32Depth) // @@0 iterative
{
    EMP_RET_MAX_IF(size_t, a_hTokens.count() <= 0);
    EMP_OSE_DEBUG_SHELL(a_hTokens);

    info_s oInfo;
    for (size_t stIndex = 0; stIndex < a_hTokens.count(); ++stIndex)
    {
        oInfo = a_hTokens.ref(0).m_Info;
        if (emp::tt::is_not_max(oInfo.m_stLine) && emp::tt::is_not_max(oInfo.m_stColumn))
        { break; }
    }

    c_size stLex = alloc_lexemes();
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    bool bDealloc = true;
    auto oFinally = emp::tt::finally([this, &bDealloc, stLex]()
    { bDealloc = dealloc_lexemes(stLex); });
    EMP_RET_FALSE_IF_NOT(m_vpvLexemes.is_valid_index(stLex));
    auto* const pvLexemes = m_vpvLexemes.val(stLex);
    EMP_RET_FALSE_IF(pvLexemes == nullptr);

    EMP_RET_MAX_IF_NOT(size_t, get_lexemes(a_rTable, *pvLexemes, a_hTokens, a_siStack));

    {
        c_size stCount = pvLexemes->count();
        if ((stCount > 0) == false)
        {
            if (a_hTokens.count() == 1)
            {
                token_c const& rToken = a_hTokens.ref(0);

                c_size stNode = alloc_node(&a_rProgram);
                EMP_RET_MAX_IF(size_t, emp::tt::is_max(stNode));
                EMP_RET_MAX_IF_NOT(size_t, fill_node(stNode, rToken));
                lexeme_c const* const pLexeme = rToken.m_pLexeme;

                EMP_RET_MAX_IF(size_t, a_si32Depth <= 0,
                 EMP_OSE_NODE(m_vNodes.ref(stNode), "only declaration allowed at global scope #0", m_vNodes.ref(stNode).m_sName));

                m_vNodes.ref(stNode).m_eType = [eToken=rToken.m_eType, pLexeme]()
                {
                    if (eToken == token_e::RealLiteral) { return node_e::RealLiteral; }
                    else if (eToken == token_e::IntegralLiteral) { return node_e::IntegralLiteral; }
                    else if (pLexeme == nullptr || is_call(pLexeme)) { return node_e::Call; }
                    return node_e::none;
                }();

                return stNode;
            }

            EMP_RET_MAX_(size_t, EMP_OSE_TOKEN(a_hTokens.ref(0), "invalid leaf '#0'", a_hTokens.ref(0).m_sName));
        }
    }

    lexeme_s const& rLexeme = pvLexemes->ref(0);
    lexeme_c* const pLexeme = rLexeme.lexeme;

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("lexeme found #0", pLexeme->m_sName);)

    token_vhc vhTokens;
    get_shells(*pvLexemes, a_hTokens, vhTokens);
    c_size stShellCount = vhTokens.count();

    if (pLexeme == m_pDeclare)
    {
        EMP_RET_MAX_IF(size_t, a_si32Depth > 0,
         EMP_OSE_INFO(oInfo, "declaration is only allowed at global scoce #0", pLexeme->m_sName));
        return parse_declaration(a_rProgram, a_hTokens, a_siStack, a_rTable, false, a_si32Depth);
    }

    EMP_RET_MAX_IF(size_t, a_si32Depth <= 0 && (pLexeme != m_pSemiColon && pLexeme != m_pRightBrace),
     EMP_OSE_INFO(oInfo, "invalid statement at global scope #0", pLexeme->m_sName));

    if (pLexeme == m_pLeftBrace)
    { return parse_scope_or_control(a_rProgram, vhTokens, a_siStack, a_rTable, a_si32Depth); }
    /*else if (pLexeme == m_pSpace)
    { return parse_statements(vhTokens, a_siStack, a_rTable); }
    else if (pLexeme == m_pAssign)
    { return parse_assignment(vhTokens, a_siStack, a_rTable); }*/
    else if (pLexeme == m_pRightPar)
    { return parse_call(a_rProgram, a_hTokens, a_siStack, a_rTable, a_si32Depth); }

    /*for (size_t stIndex = 0; stIndex < stShellCount; ++stIndex)
    {
        auto const& hTokens = vhTokens.ref(stIndex);
        if (hTokens.count() > 0)
        {
            stLine = hTokens.ref(0).m_stLine;
            stColumn = hTokens.ref(0).m_stColumn;
            break;
        }
    }*/

    c_size stNode = alloc_node(&a_rProgram);
    EMP_RET_MAX_IF(size_t, emp::tt::is_max(stNode));
    EMP_RET_MAX_IF_NOT(size_t, fill_node(stNode, pLexeme == m_pRightBrace ? m_pSemiColon : pLexeme, oInfo));

    if (is_call(pLexeme))
    { m_vNodes.ref(stNode).m_eType = node_e::Call; }

    for (size_t stIndex = 0; stIndex < stShellCount; ++stIndex)
    {
        token_chc& hTokens = vhTokens.ref(stIndex);
        if (hTokens.count() > 0)
        {
            c_size stChild = build_ast(a_rProgram, hTokens, a_siStack, a_rTable, a_si32Depth);
            EMP_RET_MAX_IF(size_t, emp::tt::is_max(stChild),
             EMP_OSE_NODE(m_vNodes.ref(stNode), "invalid child element #0", m_vNodes.ref(stNode).m_sName));

            if (m_vNodes.ref(stChild).m_pLexeme == m_pSemiColon)
            { EMP_RET_MAX_IF_NOT(size_t, copy_leaves(a_rProgram, stChild, stNode)); }
            else
            { EMP_RET_MAX_IF_NOT(size_t, add_child_node(stNode, stChild)); }
        }
    }

    if (m_vNodes.ref(stNode).m_pLexeme == m_pSemiColon)
    {
        auto const* const pvstLeaves = m_vNodes.ref(stNode).m_pvstLeaves;
        c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            node_c* const pChild = m_vNodes.ptr(pvstLeaves->val(stIndex));
            lexeme_c* const pChildLexeme = pChild->m_pLexeme;
            if (pChildLexeme != nullptr)
            {
                c_lexeme_e eChildLex = pChildLexeme->m_eType;
                if (eChildLex == lexeme_e::Literal)
                { EMP_RET_MAX_(size_t, EMP_OSE_NODE(*pChild, "unused literal #0", pChildLexeme->m_sName)); }
                else if (eChildLex == lexeme_e::Operation)
                { EMP_RET_MAX_(size_t, EMP_OSE_NODE(*pChild, "unused operation #0", pChildLexeme->m_sName)); }
            }
            switch (pChild->m_eType)
            {
                case node_e::IntegralLiteral:
                case node_e::RealLiteral:
                {
                    EMP_RET_MAX_(size_t, EMP_OSE_NODE(*pChild, "unused literal #0", pChild->m_sName));
                }
                case node_e::Call:
                case node_e::Declaration:
                case node_e::DefinitionU:
                case node_e::none:
                default:
                { break;}
            };
        }
    }

    return stNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_asm(emp::cnt::vector<size_t> const* const a_pvstNodes, program_c& a_rProgram, table_c& a_rTable, c_bool a_bSkipGlobals) // @@0 iterative
{
    EMP_RET_FALSE_IF(a_pvstNodes == nullptr || (a_pvstNodes->count() > 0) == false);

    c_size stCount = a_pvstNodes->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(build_asm(a_pvstNodes->val(stIndex), a_rProgram, a_rTable, a_bSkipGlobals)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_OSE_UNHANDLED(x_Case)\
    case lexeme_e::x_Case:\
    {\
        EMP_LOG_ERROR(ALITERAL("unhandled lexeme #0, #1, '#2'"), static_cast<ui32>(lexeme_e::x_Case), EMP_PP_STRING(x_Case), a_pNode->m_sName);\
        return false;\
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_arguments(c_size a_stNode, program_c& a_rProgram, table_c& a_rTable) // @@0 iterative
{
    auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    if (stCount > 0)
    { EMP_RET_FALSE_IF_NOT(build_asm(pvstLeaves, a_rProgram, a_rTable, false), "failed to build argument list for call '#0'", m_vNodes.ref(a_stNode).m_sName); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::resolve_unaries(program_c& a_rProgram, c_size a_stNode, table_c& EMP_SILENT(a_rTable))
{
    auto const l_resolve_unaries = [&a_rProgram, this](c_size a_stLeaf) EMP_RETURN -> bool
    {
        node_c& rNode = m_vNodes.ref(a_stLeaf);
        lexeme_c* const pLexeme = rNode.m_pLexeme;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("NODE #0 | #1", rNode.m_sName, pLexeme != nullptr ? pLexeme->m_sName : astring()););
        if (pLexeme == m_pUnaryPos_I32||
            pLexeme == m_pUnaryNeg_I32)
        {
            EMP_RET_FALSE_IF(rNode.m_pvstLeaves == nullptr || rNode.m_pvstLeaves->count() <= 0, EMP_OSE_NODE(rNode, "invalid unary expression #0", rNode.m_sName));
            c_size stChild = rNode.m_pvstLeaves->val(0);
            node_c& rChild = m_vNodes.ref(rNode.m_pvstLeaves->val(0));
            lexeme_c* const pChild = rChild.m_pLexeme;
            if (pLexeme == m_pUnaryPos_I32)
            {
                if (pChild == m_pUnaryPos_I32 ||
                    pChild == m_pUnaryNeg_I32)
                { EMP_RET_FALSE_IF_NOT(copy_leaves(a_rProgram, stChild, a_stLeaf)); }
            }
            else if (pLexeme == m_pUnaryNeg_I32)
            {
                if (pChild == m_pUnaryPos_I32)
                {
                    EMP_RET_FALSE_IF_NOT(copy_leaves(a_rProgram, stChild, a_stLeaf));
                    rNode.m_pLexeme = m_pUnaryNeg_I32;
                }
                else if (pChild == m_pUnaryNeg_I32)
                {
                    EMP_RET_FALSE_IF_NOT(copy_leaves(a_rProgram, stChild, a_stLeaf));
                    rNode.m_pLexeme = m_pUnaryPos_I32;
                }
            }
        }
        return true;
    };

    EMP_RET_FALSE_IF_NOT(foreach_nodes(a_stNode, l_resolve_unaries));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::resolve_literals(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable)
{
    auto const l_resolve_literals = [&a_rProgram, this, &a_rTable](c_size a_stLeaf EMP_UNUSED) EMP_RETURN -> bool
    {
        node_c& rNode = m_vNodes.ref(a_stLeaf);
        lexeme_c* const pLexeme = rNode.m_pLexeme;
        //EMP_OSE_IF_DEBUG(EMP_LOG_INFO("NODE #0 | #1", rNode.m_sName, pLexeme != nullptr ? pLexeme->m_sName : astring()););

        c_bool bUnaryPos = pLexeme == m_pUnaryPos_I32;
        c_bool bUnaryNeg = pLexeme == m_pUnaryNeg_I32;
        if (bUnaryPos || bUnaryNeg)
        {
            c_size stChild = rNode.m_pvstLeaves->val(0);
            node_c& rChild = m_vNodes.ref(stChild);
            node_e const eChild = rChild.m_eType;

            c_bool bRealLiteral = eChild == node_e::RealLiteral;
            c_bool bIntegralLiteral = eChild == node_e::IntegralLiteral;

            if (bRealLiteral || bIntegralLiteral)
            {
                astring sLiteral = bUnaryNeg ? STR_AFORMAT("-#0", rChild.m_sName) : rChild.m_sName;

                auto* const pLiteral = [bRealLiteral, bUnaryNeg, &a_rTable, &sLiteral]() EMP_RETURN -> lexeme_c*
                {
                    if (bRealLiteral)
                    { return a_rTable.reg_literal<float>(sLiteral); }
                    else
                    {
                        if (bUnaryNeg)
                        { return a_rTable.reg_literal<si32>(sLiteral); }
                        else
                        { return a_rTable.reg_literal<ui32>(sLiteral); }
                    }
                }();
                EMP_RET_FALSE_IF(pLiteral == nullptr);
                rNode.m_pLexeme = pLiteral;
                rNode.m_sName = sLiteral;
                rNode.m_eType = eChild;
                EMP_RET_FALSE_IF_NOT(copy_leaves(a_rProgram, stChild, a_stLeaf));
                EMP_RET_FALSE_IF_NOT(emp::tt::is_max(rNode.m_stLeaves));
                EMP_RET_FALSE_IF_NOT(rNode.m_pvstLeaves == nullptr);
            }
        }
        return true;
    };

    EMP_RET_FALSE_IF_NOT(foreach_nodes(a_stNode, l_resolve_literals));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::resolve_calls(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable)
{
    auto const l_resolve_calls = [&a_rProgram, this, &a_rTable](c_size a_stLeaf) EMP_RETURN -> bool
    {
        node_c& rNode = m_vNodes.ref(a_stLeaf);
        lexeme_c* const pLexeme = rNode.m_pLexeme;
        c_astring& sName = (pLexeme != nullptr && pLexeme->m_sName.len() > 0) ? pLexeme->m_sName : m_vNodes.ref(a_stLeaf).m_sName;
        EMP_OSE_IF_DEBUG(c_astring& sQualified = (pLexeme != nullptr && pLexeme->m_sName.len() > 0) ? pLexeme->m_sQualified : m_vNodes.ref(a_stLeaf).m_sName);
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("trying to resolve : '#0'", sQualified));

        if (rNode.m_bConstant)
        { EMP_RET_FALSE_("unhandled constant lexeme"); }

        emp::cnt::vector<size_t> const* const pvstLeaves = rNode.m_pvstLeaves;
        c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();

        auto const l_resolve_literal = [this, &rTable = a_rProgram.m_Table](rti::type_c const* const a_pParam, c_astring& a_rsName) EMP_RETURN -> lexeme_c*
        {
            if (a_pParam == m_Table.m_pRealType)
            { return rTable.reg_literal<float>(a_rsName); }
            else if (a_pParam == m_Table.m_pUIntType)
            { return rTable.reg_literal<ui32>(a_rsName); }
            else if (a_pParam == m_Table.m_pSIntType)
            { return rTable.reg_literal<si32>(a_rsName); }
            return nullptr;
        };

        if (rNode.m_eType == node_e::DefinitionU)
        {
            if (pLexeme != nullptr &&
                pLexeme->m_eType == lexeme_e::Data)
            {
                auto* const pParamType = pLexeme->m_pType;

                EMP_RET_FALSE_IF(stCount != 1);
                c_size stChild = pvstLeaves->val(0);
                EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stChild));

                auto* const pChild = m_vNodes.ptr(stChild);
                EMP_RET_FALSE_IF(pChild == nullptr);
                EMP_RET_FALSE_IF(pChild->m_pLexeme != m_pLeftBrace);

                auto const* const pvstChildLeaves = pChild->m_pvstLeaves;
                EMP_RET_FALSE_IF(pvstChildLeaves == nullptr);
                EMP_RET_FALSE_IF(pvstChildLeaves->count() != 1);

                auto* const pDataNode = m_vNodes.ptr(pvstChildLeaves->val(0));
                auto* const pDataLexeme = pDataNode->m_pLexeme;
                if (pDataLexeme != nullptr)
                {
                    rti::type_c const* pLeafType = pDataLexeme->m_pType;
                    EMP_RET_FALSE_IF(pLeafType == nullptr);

                    if (pLeafType->m_eType == rti::type_e::Function)
                    {
                        EMP_RET_FALSE_IF(pDataNode->m_eType == node_e::DefinitionU,
                                         EMP_OSE_NODE(*pDataNode, "unhandled function pointer : #0", pDataNode->m_sName));

                        rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                        EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                        auto const& rvReturns = pLeafFunction->m_vpReturns;
                        c_size stReturnCount = rvReturns.count();
                        EMP_RET_FALSE_IF(stReturnCount != 1,
                                         EMP_OSE_NODE(*pDataNode, "invalid return count #0 : #1", stReturnCount, pDataNode->m_sName));
                        rti::type_c const* const pReturnType = rvReturns.val(0);
                        EMP_RET_FALSE_IF(pReturnType == nullptr);

                        EMP_RET_FALSE_IF(pParamType != pReturnType,
                                         EMP_OSE_NODE(*pDataNode, "invalid cast #0 -> #1", pReturnType->m_sName, pParamType->m_sName));
                        return true;
                    }
                    else if (pParamType == pLeafType)
                    { return true; }
                    else
                    {
                        goto LABEL_EMP_RESOLVE_LITERAL;
                    }
                }
                else if (pDataNode->m_eType == node_e::RealLiteral ||
                         pDataNode->m_eType == node_e::IntegralLiteral)
                {
LABEL_EMP_RESOLVE_LITERAL:
                    lexeme_c* const pLeafLiteral = l_resolve_literal(pParamType, pDataNode->m_sName);
                    EMP_RET_FALSE_IF(pLeafLiteral == nullptr || pLeafLiteral->m_pType != pParamType);
                    pDataNode->m_pLexeme = pLeafLiteral;
                    return true;
                }

                EMP_RET_FALSE_(EMP_OSE_NODE(rNode, "failed to resolve call '#0'", rNode.m_sName));
            }
        }

        if (rNode.m_eType != node_e::Call)
        { return true; }

        auto* const pOverloads = a_rTable.get_overload(sName);
        EMP_RET_FALSE_IF(pOverloads == nullptr, EMP_OSE_NODE(rNode, "unresolved symbol '#0'", sName));

        emp::cnt::vector<lexeme_c*> const& rvpLexemes = pOverloads->vector();
        lexeme_c* pSymbol = nullptr;

        auto const l_release_literals = [this, pvstLeaves, stCount]()
        {
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                auto const eType = m_vNodes.ptr(pvstLeaves->val(stIndex))->m_eType;
                if (eType == node_e::RealLiteral ||
                    eType == node_e::IntegralLiteral)
                { m_vNodes.ptr(pvstLeaves->val(stIndex))->m_pLexeme = nullptr; }
            }
        };

        auto l_finally_literals = emp::tt::finally([l_release_literals]() { l_release_literals(); });

        c_size stLexemeCount = rvpLexemes.count();
        for (size_t stLexeme = 0; stLexeme < stLexemeCount; ++stLexeme)
        {
            lexeme_c* const pOverload = rvpLexemes.val(stLexeme);
            c_lexeme_e eOverload = pOverload->m_eType;

            if (eOverload == lexeme_e::Data)
            {
                EMP_RET_FALSE_IF(stLexemeCount != 1);
                rNode.m_pLexeme = pOverload;
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pOverload->m_sQualified);)
                l_finally_literals.release();
                return true;
            }

            rti::function_c const* const pOverloadType = rti::to_type<rti::function_c const>(pOverload->m_pType);
            EMP_RET_FALSE_IF(pOverloadType == nullptr);

            emp::cnt::vector<rti::type_c const*> const& rvpParameters = pOverloadType->m_vpParameters;
            c_size stParameterCount = rvpParameters.count();

            size_t stArgument = 0;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                node_c* const pLeaf = m_vNodes.ptr(pvstLeaves->val(stIndex));
                EMP_RET_FALSE_IF(pLeaf == nullptr);

                lexeme_c const* pLeafOp = pLeaf->m_pLexeme;
                if (pLeafOp != nullptr)
                {
                    rti::type_c const* const pLeafType = pLeafOp->m_pType;
                    EMP_RET_FALSE_IF(pLeafType == nullptr);

                    if (pLeafType->m_eType == rti::type_e::Function)
                    {
                        EMP_RET_FALSE_IF(pLeaf->m_eType == node_e::DefinitionU,
                            EMP_OSE_NODE(*pLeaf, "unhandled function pointer : #0", pLeaf->m_sName));

                        rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                        EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                        auto const& rvReturns = pLeafFunction->m_vpReturns;
                        c_size stReturnCount = rvReturns.count();
                        if (stReturnCount > 0)
                        {
                            for (size_t stReturn = 0; stReturn < stReturnCount; ++stReturn)
                            {
                                rti::type_c const* const pReturnType = rvReturns.val(stReturn);
                                EMP_RET_FALSE_IF(pReturnType == nullptr);

                                if (stArgument >= stParameterCount)
                                { goto NEXT_STRICT_OVERLOAD; }
                                rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                                ++stArgument;

                                EMP_RET_FALSE_IF(pParamType == nullptr);
                                if (pParamType != pReturnType)
                                { goto NEXT_STRICT_OVERLOAD; }
                            }
                        }
                        else
                        { goto NEXT_STRICT_OVERLOAD; }
                    }
                    else
                    {
                        if (stArgument >= stParameterCount)
                        { goto NEXT_STRICT_OVERLOAD; }
                        rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                        EMP_RET_FALSE_IF(pParamType == nullptr);
                        ++stArgument;

                        if (pParamType != pLeafType)
                        { goto NEXT_STRICT_OVERLOAD; }
                    }
                }
                else if (pLeaf->m_eType == node_e::RealLiteral ||
                         pLeaf->m_eType == node_e::IntegralLiteral)
                {
                    if (stArgument >= stParameterCount)
                    { goto NEXT_STRICT_OVERLOAD; }

                    rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                    EMP_RET_FALSE_IF(pParamType == nullptr);
                    ++stArgument;

                    lexeme_c* const pLeafLiteral = l_resolve_literal(pParamType, pLeaf->m_sName);
                    if (pLeafLiteral == nullptr)
                    { goto NEXT_STRICT_OVERLOAD; }
                    EMP_RET_FALSE_IF(pLeafLiteral->m_pType != pParamType);
                    pLeaf->m_pLexeme = pLeafLiteral;
                }

                if (pOverload->m_eType == lexeme_e::Control)
                { break; }
            }

            if (stArgument == stParameterCount)
            {
                EMP_RET_FALSE_IF(pSymbol != nullptr, "ambiguous symbol '#0'", sName);
                pSymbol = pOverload;
                break;
                //goto NEXT_STRICT_OVERLOAD;
            }

            NEXT_STRICT_OVERLOAD:
                l_release_literals();
        }

        EMP_RET_FALSE_IF(pSymbol == nullptr, EMP_OSE_NODE(rNode, "unresolved symbol '#0'", sName));

        rNode.m_pLexeme = pSymbol;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pSymbol->m_sQualified);)
        l_finally_literals.release();
        return true;
    };

    EMP_RET_FALSE_IF_NOT(foreach_nodes(a_stNode, l_resolve_calls));
    return true;
}

#undef XTMP_OSE_CAST
#undef XTMP_OSE_ARG
#undef XTMP_OSE_OVERLOAD

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::resolve2(program_c& a_rProgram, c_size a_stNode, table_c& a_rTable)
{
    auto const l_resolve_leaf = [&a_rProgram, this, &a_rTable](c_size a_stLeaf)
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("trying to resolve : '#0'", m_vNodes.ref(a_stLeaf).m_sName));

        if (m_vNodes.ref(a_stLeaf).m_bConstant)
        {
/*            bool bCache = false;
            m_pStack->copy(m_pProgram);
            EMP_RET_FALSE_IF_NOT(from_ast(a_stLeaf, *m_pProgram, bCache));
            c_size stMaxCount = 1024;
            int siProcessed = m_pProcessor->run(*m_pProgram, *m_pStack, stMaxCount);
            EMP_RET_FALSE_IF(siProcessed < 0);

            c_size stProcessed = static_cast<size_t>(siProcessed);
            if (stProcessed < stMaxCount) // @@0 why testing return false ?
            { // @@0 stProcessed is not the number of processed instructions
                //a_pNode->set_lexeme() new lexeme base on result
                EMP_LOG_ERROR("unhandled constant lexeme");
                return false;
            }*/

            EMP_RET_FALSE_("unhandled constant lexeme");
        }

        emp::cnt::vector<size_t> const* const pvstLeaves = m_vNodes.ref(a_stLeaf).m_pvstLeaves;
        c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();

        rti::type_c const* const pRealType = a_rTable.real_type();
        rti::type_c const* const pUIntType = a_rTable.uint_type();
        rti::type_c const* const pSIntType = a_rTable.sint_type();

        if (m_vNodes.ref(a_stLeaf).m_eType == node_e::DefinitionU)
        {
            auto* const pLexeme = m_vNodes.ref(a_stLeaf).m_pLexeme;
            if (pLexeme != nullptr &&
                pLexeme->m_eType == lexeme_e::Data)
            {
                EMP_RET_FALSE_IF(stCount != 1);
                c_size stChild = pvstLeaves->val(0);
                EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stChild));

                auto* const pChild = m_vNodes.ptr(stChild);
                EMP_RET_FALSE_IF(pChild == nullptr);
                EMP_RET_FALSE_IF(pChild->m_pLexeme != m_pLeftBrace);

                auto const* const pvstChildLeaves = pChild->m_pvstLeaves;
                EMP_RET_FALSE_IF(pvstChildLeaves == nullptr);
                EMP_RET_FALSE_IF(pvstChildLeaves->count() != 1);

                auto* const pDataNode = m_vNodes.ptr(pvstChildLeaves->val(0));
                auto* const pDataLexeme = pDataNode->m_pLexeme;
                EMP_RET_FALSE_IF(pDataLexeme == nullptr);

                auto* const pParamType = pLexeme->m_pType;

                rti::type_c const* pLeafType = pDataLexeme->m_pType;
                EMP_RET_FALSE_IF(pLeafType == nullptr);

                if (pLeafType->m_eType == rti::type_e::Function)
                {
                    EMP_RET_FALSE_IF(pDataNode->m_eType == node_e::DefinitionU,
                     EMP_OSE_NODE(*pDataNode, "unhandled function pointer : #0", pDataNode->m_sName));

                    rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                    EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                    auto const& rvReturns = pLeafFunction->m_vpReturns;
                    c_size stReturnCount = rvReturns.count();
                    EMP_RET_FALSE_IF(stReturnCount != 1,
                     EMP_OSE_NODE(*pDataNode, "invalid return count #0 : #1", stReturnCount, pDataNode->m_sName));
                    rti::type_c const* const pReturnType = rvReturns.val(0);
                    EMP_RET_FALSE_IF(pReturnType == nullptr);

                    EMP_RET_FALSE_IF(pParamType != pReturnType,
                     EMP_OSE_NODE(*pDataNode, "invalid cast #0 -> #1", pReturnType->m_sName, pParamType->m_sName));
                    return true;
                }
                else
                {
                    if (pParamType == pLeafType)
                    { return true; }
                    else if (pParamType == pRealType)
                    {
                        auto const l_cast = [this, pDataLexeme, pDataNode, stChild, &a_rTable, &a_rProgram]
                        (auto a_tType) EMP_RETURN -> bool
                        {
                            typedef decltype(a_tType) type;
                            literal_t<type>* const pLiteral = to_literal<type>(pDataLexeme);
                            EMP_RET_FALSE_IF(pLiteral == nullptr);
                            literal_t<real>* const pCastedLiteral = a_rTable.reg_literal(static_cast<real>(pLiteral->m_tLiteral));
                            EMP_RET_FALSE_IF(pCastedLiteral == nullptr);
                            c_size stCastedNode = alloc_node(&a_rProgram);
                            EMP_RET_FALSE_IF(emp::tt::is_max(stCastedNode));
                            EMP_RET_FALSE_IF_NOT(fill_node(stCastedNode, pCastedLiteral, pDataNode->m_Info));
                            EMP_RET_FALSE_IF_NOT(set_child_node(stChild, stCastedNode, 0));
                            return true;
                        };

                        if (pLeafType == pUIntType) { EMP_RET_FALSE_IF_NOT(l_cast((uint)0)); }
                        else if (pLeafType == pSIntType) { EMP_RET_FALSE_IF_NOT(l_cast((sint)0)); }
                    }
                }
            }
        }

        if (m_vNodes.ref(a_stLeaf).m_eType != node_e::Call)
        { return true; }

        lexeme_c const* const pLexeme = m_vNodes.ref(a_stLeaf).m_pLexeme;
        c_astring& sName = (pLexeme != nullptr && pLexeme->m_sName.len() > 0) ? pLexeme->m_sName : m_vNodes.ref(a_stLeaf).m_sName;

        auto* const pOverloads = a_rTable.get_overload(sName);
        EMP_RET_FALSE_IF(pOverloads == nullptr, EMP_OSE_NODE(m_vNodes.ref(a_stLeaf), "unresolved symbol '#0'", sName));

        emp::cnt::vector<lexeme_c*> const& rvpLexemes = pOverloads->vector();
        lexeme_c* pSymbol = nullptr;

        c_size stLexemeCount = rvpLexemes.count();
        for (size_t stLexeme = 0; stLexeme < stLexemeCount; ++stLexeme)
        {
            lexeme_c* const pOverload = rvpLexemes.val(stLexeme);
            c_lexeme_e eOverload = pOverload->m_eType;

            if (eOverload == lexeme_e::Data)
            {
                EMP_RET_FALSE_IF(stLexemeCount != 1);
                m_vNodes.ref(a_stLeaf).m_pLexeme = pOverload;
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pOverload->m_sQualified);)
                return true;
            }

            rti::function_c const* const pOverloadType = rti::to_type<rti::function_c const>(pOverload->m_pType);
            EMP_RET_FALSE_IF(pOverloadType == nullptr);

            emp::cnt::vector<rti::type_c const*> const& rvpParameters = pOverloadType->m_vpParameters;
            c_size stParameterCount = rvpParameters.count();

            size_t stArgument = 0;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                node_c const* const pLeaf = m_vNodes.ptr(pvstLeaves->val(stIndex));
                EMP_RET_FALSE_IF(pLeaf == nullptr);

                lexeme_c const* const pLeafOp = pLeaf->m_pLexeme;
                EMP_RET_FALSE_IF(pLeafOp == nullptr);

                rti::type_c const* const pLeafType = pLeafOp->m_pType;
                EMP_RET_FALSE_IF(pLeafType == nullptr);

                if (pLeafType->m_eType == rti::type_e::Function)
                {
                    EMP_RET_FALSE_IF(pLeaf->m_eType == node_e::DefinitionU,
                     EMP_OSE_NODE(*pLeaf, "unhandled function pointer : #0", pLeaf->m_sName));

                    rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                    EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                    auto const& rvReturns = pLeafFunction->m_vpReturns;
                    c_size stReturnCount = rvReturns.count();
                    if (stReturnCount > 0)
                    {
                        for (size_t stReturn = 0; stReturn < stReturnCount; ++stReturn)
                        {
                            rti::type_c const* const pReturnType = rvReturns.val(stReturn);
                            EMP_RET_FALSE_IF(pReturnType == nullptr);

                            if (stArgument >= stParameterCount)
                            { goto NEXT_STRICT_OVERLOAD; }
                            rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                            ++stArgument;

                            EMP_RET_FALSE_IF(pParamType == nullptr);
                            if (pParamType != pReturnType)
                            { goto NEXT_STRICT_OVERLOAD; }
                        }
                    }
                    else
                    { goto NEXT_STRICT_OVERLOAD; }
                }
                else
                {
                    if (stArgument >= stParameterCount)
                    { goto NEXT_STRICT_OVERLOAD; }
                    rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                    ++stArgument;

                    EMP_RET_FALSE_IF(pParamType == nullptr);
                    if (pParamType != pLeafType)
                    { goto NEXT_STRICT_OVERLOAD; }
                }

                if (pOverload->m_eType == lexeme_e::Control)
                { break; }
            }

            if (stArgument == stParameterCount)
            {
                EMP_RET_FALSE_IF(pSymbol != nullptr);
                pSymbol = pOverload;
                break;
            }
        NEXT_STRICT_OVERLOAD:{}
        }

        if (pSymbol != nullptr)
        {
            m_vNodes.ref(a_stLeaf).m_pLexeme = pSymbol;
            EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pSymbol->m_sQualified);)
            return true;
        }

        for (size_t stLexeme = 0; stLexeme < stLexemeCount; ++stLexeme)
        {
            lexeme_c* const pOverload = rvpLexemes.val(stLexeme);
            rti::function_c const* const pOverloadType = rti::to_type<rti::function_c const>(pOverload->m_pType);
            EMP_RET_FALSE_IF(pOverload == nullptr);
            emp::cnt::vector<rti::type_c const*> const& rvpParameters = pOverloadType->m_vpParameters;
            c_size stParameterCount = rvpParameters.count();
            size_t stArgument = 0;
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                node_c const* const pLeaf = m_vNodes.ptr(pvstLeaves->val(stIndex));
                EMP_RET_FALSE_IF(pLeaf == nullptr);

                rti::type_c const* const pLeafType = get_type(pLeaf->m_pLexeme);
                EMP_RET_FALSE_IF(pLeafType == nullptr);

                if (pLeafType->m_eType == rti::type_e::Function)
                {
                    EMP_RET_FALSE_IF(pLeaf->m_eType == node_e::DefinitionU,
                     EMP_OSE_NODE(*pLeaf, "unhandled function pointer : #0", pLeaf->m_sName));

                    rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                    EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                    auto const& rvReturns = pLeafFunction->m_vpReturns;
                    c_size stReturnCount = rvReturns.count();
                    if (stReturnCount > 0)
                    {
                        for (size_t stReturn = 0; stReturn < stReturnCount; ++stReturn)
                        {
                            rti::type_c const* const pReturnType = rvReturns.val(stReturn);
                            EMP_RET_FALSE_IF(pReturnType == nullptr);

                            if (stArgument >= stParameterCount)
                            { goto NEXT_LOOSE_OVERLOAD; }
                            rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                            ++stArgument;

                            EMP_RET_FALSE_IF(pParamType == nullptr);
                            if (pParamType != pReturnType)
                            { goto NEXT_LOOSE_OVERLOAD; }
                        }
                    }
                    else
                    { goto NEXT_LOOSE_OVERLOAD; }
                }
                else
                {
                    if (stArgument >= stParameterCount)
                    { goto NEXT_LOOSE_OVERLOAD; }
                    rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                    ++stArgument;

                    EMP_RET_FALSE_IF(pParamType == nullptr);
                    if (pParamType == pLeafType)
                    { continue; }
                    else if (pParamType == pRealType)
                    {
                        lexeme_c const* const pLeafLexeme = pLeaf->m_pLexeme;
                        EMP_RET_FALSE_IF(pLeafLexeme == nullptr);
                        if (pLeafType == pUIntType)
                        { continue; }
                        else if (pLeafType == pSIntType)
                        { continue; }
                    }
                    goto NEXT_LOOSE_OVERLOAD;
                }
            }

            if (stArgument == stParameterCount)
            {
                EMP_RET_FALSE_IF(pSymbol != nullptr, "ambiguous symbol '#0'", sName);
                pSymbol = pOverload;
            }
        NEXT_LOOSE_OVERLOAD:{}
        }

        EMP_RET_FALSE_IF(pSymbol == nullptr, EMP_OSE_NODE(m_vNodes.ref(a_stLeaf), "unresolved symbol '#0'", sName));

        rti::function_c const* const pSymbolType = rti::to_type<rti::function_c const>(pSymbol->m_pType);
        EMP_RET_FALSE_IF(pSymbol == nullptr);
        emp::cnt::vector<rti::type_c const*> const& rvpParameters = pSymbolType->m_vpParameters;
        c_size stParameterCount = rvpParameters.count();
        size_t stArgument = 0;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            node_c const* const pLeaf = m_vNodes.ptr(pvstLeaves->val(stIndex));
            EMP_RET_FALSE_IF(pLeaf == nullptr);

            rti::type_c const* const pLeafType = get_type(pLeaf->m_pLexeme);
            EMP_RET_FALSE_IF(pLeafType == nullptr);

            if (pLeafType->m_eType == rti::type_e::Function)
            {
                EMP_RET_FALSE_IF(pLeaf->m_eType == node_e::DefinitionU, EMP_OSE_NODE(*pLeaf, "unhandled function pointer : #0", pLeaf->m_sName));

                rti::function_c const* const pLeafFunction = rti::to_type<rti::function_c const>(pLeafType);
                EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                auto const& rvReturns = pLeafFunction->m_vpReturns;
                c_size stReturnCount = rvReturns.count();
                if (stReturnCount > 0)
                {
                    for (size_t stReturn = 0; stReturn < stReturnCount; ++stReturn)
                    {
                        rti::type_c const* const pReturnType = rvReturns.val(stReturn);
                        EMP_RET_FALSE_IF(pReturnType == nullptr);

                        EMP_RET_FALSE_IF(stArgument >= stParameterCount);
                        rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                        ++stArgument;

                        EMP_RET_FALSE_IF(pParamType == nullptr);
                        EMP_RET_FALSE_IF (pReturnType != pParamType);
                    }
                }
                else
                {
                }
            }
            else
            {
                EMP_RET_FALSE_IF(stArgument >= stParameterCount);
                rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                ++stArgument;

                EMP_RET_FALSE_IF(pParamType == nullptr);
                if (pParamType == pLeafType)
                { continue; }
                else if (pParamType == pRealType)
                {
                    lexeme_c* const pLeafLexeme = pLeaf->m_pLexeme;
                    EMP_RET_FALSE_IF(pLeafLexeme == nullptr);

#define XTMP_OSE_CAST(x_Type, x_TableType)\
if (pLeafType == x_TableType)\
{\
literal_t<x_Type>* const pLiteral = to_literal<x_Type>(pLeafLexeme);\
EMP_RET_FALSE_IF(pLiteral == nullptr);\
literal_t<real>* const pCastedLiteral = a_rTable.reg_literal(static_cast<real>(pLiteral->m_tLiteral));\
EMP_RET_FALSE_IF(pCastedLiteral == nullptr);\
c_size stCastedNode = alloc_node(&a_rProgram);\
EMP_RET_FALSE_IF(emp::tt::is_max(stCastedNode));\
EMP_RET_FALSE_IF_NOT(fill_node(stCastedNode, pCastedLiteral, m_vNodes.ref(a_stLeaf).m_Info));\
EMP_RET_FALSE_IF_NOT(set_child_node(a_stLeaf, stCastedNode, stIndex));\
continue;\
} else
                    XTMP_OSE_CAST(uint, pUIntType)
                    XTMP_OSE_CAST(sint, pSIntType)
                    { return false; } // unhandled cast, must not happend.
#undef XTMP_OSE_CAST
                }
            }
        }

        EMP_RET_FALSE_IF(stArgument != stParameterCount);
        m_vNodes.ref(a_stLeaf).m_pLexeme = pSymbol;
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pSymbol->m_sQualified);)
        return true;
    };

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
            EMP_RET_FALSE_IF_NOT(l_resolve_leaf(pSlice->node));
            m_vpResolveStack.remove_last();
        }
    }

    return true;
}

#undef XTMP_OSE_CAST
#undef XTMP_OSE_ARG
#undef XTMP_OSE_OVERLOAD


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_asm(c_size a_stNode, program_c& a_rProgram, table_c& a_rTable, c_bool a_bSkipGlobals) // @@0 iterative
{
    c_node_e eNode = m_vNodes.ref(a_stNode).m_eType;
    if (eNode == node_e::Call) {}
    lexeme_c const* const pLexeme = m_vNodes.ref(a_stNode).m_pLexeme;
    EMP_RET_FALSE_IF(pLexeme == nullptr);

    c_lexeme_e eLexeme = pLexeme->m_eType;
    switch (eLexeme)
    {
        case lexeme_e::Punctuation:
        {
            punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
            EMP_RET_FALSE_IF(pPunctuation == nullptr);
            c_punctuation_e ePunctuation = pPunctuation->m_ePunctuationType;
            switch (ePunctuation)
            {
                case punctuation_e::LeftBrace:
                {
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));
                    c_size stPushData = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stPushData >= emp::tt::max<ui32>::value);
                    c_ui32 ui32PushData = static_cast<ui32>(stPushData);
                    a_rProgram.add_data(emp::tt::max<ui32>::value);

                    auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                    if (pvstLeaves != nullptr && pvstLeaves->count() > 0)
                    { EMP_RET_FALSE_IF_NOT(build_asm(pvstLeaves, a_rProgram, a_rTable, a_bSkipGlobals)); }

                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                    c_size stPopAddress = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stPopAddress >= emp::tt::max<ui32>::value);
                    c_ui32 ui32PopAddress = static_cast<ui32>(stPopAddress);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32PushData, ui32PopAddress));

                    return true;
                }
                case punctuation_e::RightPar:
                {
                    auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                    c_size stLeafCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
                    EMP_RET_FALSE_IF_NOT((stLeafCount > 0));
                    size_t stAddress = 0;
                    for (size_t stLeaf = 0; stLeaf < stLeafCount; ++stLeaf)
                    {
                        node_c const* const pLeaf = m_vNodes.ptr(pvstLeaves->val(stLeaf));
                        EMP_RET_FALSE_IF(pLeaf == nullptr);

                        lexeme_c const* const pLeafLex = pLeaf->m_pLexeme;
                        EMP_RET_FALSE_IF(pLeafLex == nullptr);

                        lexeme_c* const pLeafLexeme = a_rTable.get_lexeme(pLeafLex->m_sName, pLeafLex->m_pType);
                        data_c* const pData = to_lexeme<data_c>(pLeafLexeme);
                        if (pData != nullptr)
                        {
                            rti::data_c const* const pDataType = pData->m_pDataType;
                            EMP_RET_FALSE_IF(pDataType == nullptr);

                            EMP_RET_FALSE_IF(stAddress >= emp::tt::max<ui32>::value);
                            pData->m_ui32Address = static_cast<ui32>(stAddress);
                            stAddress += pDataType->m_stSize;
                        }
                    }
                    return stAddress > 0;
                }
                case punctuation_e::none:
                case punctuation_e::Comma:
                case punctuation_e::SemiColon:
                case punctuation_e::RightBrace:
                case punctuation_e::Declare:
                case punctuation_e::Space:
                case punctuation_e::Assign:
                case punctuation_e::LeftPar:
                default:
                {
                    EMP_LOG_ERROR("unhandled operation '#0' : #1", enum_to_astring(ePunctuation), enum_to(ePunctuation));
                    return false;
                }
            }
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                {
                    data_c* const pData = to_lexeme<data_c>(a_rTable.get_lexeme(pLexeme->m_sName, pLexeme->m_pType));
                    if (a_bSkipGlobals)
                    {
                        if (emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__clear_hue"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__clear_saturation"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__clear_brightness"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__clear_alpha"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__min_size"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__min_alpha"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__origin_x"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__origin_y"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__scale_x"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__scale_y"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__seed"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__instructions_per_frame"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__instructions_per_image"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__primitives_per_image"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__triangles_per_frame"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__triangles_per_image"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__bytes_per_frame"))) ||
                            emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("__bytes_per_image"))))
                        {
//                            EMP_LOG_INFO("SKIPPING GLOBAL #0", pLexeme->m_sName);
                            return true;
                        }
                    }

                    EMP_RET_FALSE_IF(pData == nullptr);

                    c_size stAddress = a_rTable.data_address();
                    EMP_RET_FALSE_IF(stAddress >= emp::tt::max<ui32>::value);
                    pData->m_ui32Address = static_cast<ui32>(stAddress);

                    rti::data_c const* const pDataType = pData->m_pDataType;
                    EMP_RET_FALSE_IF(pDataType == nullptr);
                    EMP_RET_FALSE_IF_NOT(a_rTable.move_data_address(pDataType->m_stSize));

                    auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                    EMP_RET_FALSE_IF(pvstLeaves == nullptr || pvstLeaves->count() != 1);

                    c_size stEntryPoint = a_rProgram.exit_point();

                    EMP_RET_FALSE_IF_NOT(build_asm(pvstLeaves->val(0), a_rProgram, a_rTable, false));
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_entry_point(stEntryPoint));

                    return true;
                }
                case node_e::Call:
                {
                    data_c const* const pData = to_lexeme<data_c const>(pLexeme);
                    EMP_RET_FALSE_IF(pData == nullptr);

                    rti::data_c const* const pDataType = pData->m_pDataType;
                    EMP_RET_FALSE_IF(pDataType == nullptr);

                    rti::c_data_e eData = pDataType->m_eData;

                    switch (eData)
                    {
#define XTMP_CASE(x_Data, x_Rel, x_Abs)\
                        case rti::data_e::x_Data:\
                        {\
                            EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(pData->m_bLocal ? instruction_e::x_Rel : instruction_e::x_Abs));\
                            a_rProgram.add_data<ui32>(pData->m_ui32Address);\
                            return true;\
                        }
                        // @@0 literal
                        XTMP_CASE(Real  , LoadRel_F32 , LoadAbs_F32)
                        XTMP_CASE(UInt  , LoadRel_U32 , LoadAbs_U32)
                        XTMP_CASE(SInt  , LoadRel_S32 , LoadAbs_S32)
                        XTMP_CASE(eBool , LoadRel_U8  , LoadAbs_U8)
#undef XTMP_CASE
                        case rti::data_e::none:
                        default:
                        {
                            EMP_LOG_ERROR("unhandled data '#0' : #1", enum_to_astring(eData), enum_to(eData));
                            return false;
                        }
                    }
                    return false;
                }
                case node_e::Declaration:
                case node_e::RealLiteral:
                case node_e::IntegralLiteral:
                case node_e::none:
                default:
                {
                    EMP_LOG_ERROR("unhandled node type #0", static_cast<ui32>(eNode));
                    return false;
                }
            }
        }
        case lexeme_e::Control:
        {
            c_control_e eControl = get_control(pLexeme);

            auto const l_loop = [this, a_stNode, &a_rProgram, &a_rTable, pLexeme](instruction_e const a_eJump, instruction_e const a_ePush, auto a_tType) EMP_RETURN -> bool
            {
                auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
                EMP_RET_FALSE_IF(stCount < 2);

                // push
                EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));
                c_size stPushData = a_rProgram.exit_point();
                EMP_RET_FALSE_IF(stPushData >= emp::tt::max<ui32>::value);
                c_ui32 ui32PushData = static_cast<ui32>(stPushData);
                a_rProgram.add_data(emp::tt::max<ui32>::value);

                // index
                EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(a_ePush));
                a_rProgram.add_data<decltype(a_tType)>(0);

                // jump
                c_size stJump = a_rProgram.exit_point();
                EMP_RET_FALSE_IF (stJump >= emp::tt::max<ui32>::value);
                c_ui32 ui32Jump = static_cast<ui32>(stJump);

                // test
                EMP_RET_FALSE_IF_NOT(build_asm(pvstLeaves->val(0), a_rProgram, a_rTable, false));
                rti::type_c const* const pType = pLexeme->m_pType;
                EMP_RET_FALSE_IF(pType == nullptr);
                EMP_RET_FALSE_IF(a_rTable.get_lexeme(m_vNodes.ref(a_stNode).m_sName, pType) == nullptr);
                EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(a_eJump));

                // end
                c_size stEnd = a_rProgram.exit_point();
                EMP_RET_FALSE_IF(stEnd >= emp::tt::max<ui32>::value);
                c_ui32 ui32End = static_cast<ui32>(stEnd);
                a_rProgram.add_data(emp::tt::max<ui32>::value);

                // body
                node_c* const pBody = m_vNodes.ptr(pvstLeaves->val(1));
                EMP_RET_FALSE_IF(
                                 pBody == nullptr ||
                                 pBody->m_pLexeme
                                 != m_pLeftBrace ||
                                 (pBody->m_pvstLeaves != nullptr && pBody->m_pvstLeaves->count() > 0 &&
                                  build_asm(pBody->m_pvstLeaves, a_rProgram, a_rTable, false) == false));

                // jump
                EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Jump));
                a_rProgram.add_data(ui32Jump);

                // break
                c_size stBreak = a_rProgram.exit_point();
                EMP_RET_FALSE_IF(stBreak >= emp::tt::max<ui32>::value);
                c_ui32 ui32Break = static_cast<ui32>(stBreak);
                EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32End, ui32Break));

                // pop
                EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                c_size stPopAddress = a_rProgram.exit_point();
                EMP_RET_FALSE_IF(stPopAddress >= emp::tt::max<ui32>::value);
                c_ui32 ui32PopAddress = static_cast<ui32>(stPopAddress);
                EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32PushData, ui32PopAddress));

                return true;
            };
            switch (eControl)
            {
                case control_e::Loop_I32:
                { return l_loop(instruction_e::JumpCmp_SI32, instruction_e::PushSInt, (si32)0); }
                case control_e::Loop_U32:
                { return l_loop(instruction_e::JumpCmp_UI32, instruction_e::PushUInt, (ui32)0); }
                case control_e::If:
                {
                    auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
                    EMP_RET_FALSE_IF(stCount < 2);

                    // push
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));
                    c_size stPushData = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stPushData >= emp::tt::max<ui32>::value);
                    c_ui32 ui32PushData = static_cast<ui32>(stPushData);
                    a_rProgram.add_data(emp::tt::max<ui32>::value);



                    // test
                    EMP_RET_FALSE_IF_NOT(build_asm(pvstLeaves->val(0), a_rProgram, a_rTable, false));
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::JumpCmp_Bool));

                    // end
                    c_size stEnd = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stEnd >= emp::tt::max<ui32>::value);
                    c_ui32 ui32End = static_cast<ui32>(stEnd);
                    a_rProgram.add_data(emp::tt::max<ui32>::value);

                    // body
                    node_c* const pBody = m_vNodes.ptr(pvstLeaves->val(1));
                    EMP_RET_FALSE_IF(
                        pBody == nullptr ||
                        pBody->m_pLexeme != m_pLeftBrace ||
                        (pBody->m_pvstLeaves != nullptr && pBody->m_pvstLeaves->count() > 0 &&
                         build_asm(pBody->m_pvstLeaves, a_rProgram, a_rTable, false) == false));

                    // break
                    c_size stBreak = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stBreak >= emp::tt::max<ui32>::value);
                    c_ui32 ui32Break = static_cast<ui32>(stBreak);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32End, ui32Break));

                    // pop
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                    c_size stPopAddress = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stPopAddress >= emp::tt::max<ui32>::value);
                    c_ui32 ui32PopAddress = static_cast<ui32>(stPopAddress);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32PushData, ui32PopAddress));

                    return true;
                }
                case control_e::none:
                default:
                {
                    EMP_LOG_ERROR("unhandled control '#0' : #1", enum_to_astring(eControl), enum_to(eControl));
                    return false;
                }
            };
            return false;
        }
        case lexeme_e::Literal:
        {
            EMP_RET_FALSE_IF_NOT(a_rProgram.add_data(pLexeme));
            return true;
        }
        case lexeme_e::Operation:
        case lexeme_e::Intrinsic:
        {
            c_instruction_e eInstruction = get_instruction(pLexeme);
            EMP_RET_FALSE_IF(eInstruction == instruction_e::none);
            EMP_RET_FALSE_IF(eInstruction >= instruction_e::Invalid);
            EMP_RET_FALSE_IF_NOT(build_arguments(a_stNode, a_rProgram, a_rTable));
            EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(eInstruction));
            return true;
        }
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                {
                    if (emp::tt::is_max(a_rProgram.m_stGen) || (a_rProgram.m_stGen == a_rProgram.m_stCurrentGen))
                    {
                        EMP_RET_FALSE_IF_NOT(a_rProgram.add_definition(pLexeme->m_ui32Index));

                        auto const* const pvstLeaves = m_vNodes.ref(a_stNode).m_pvstLeaves;
                        c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
                        EMP_RET_FALSE_IF(stCount < 1);

                        size_t EMP_TT_MAX_VAR(stReturns);
                        size_t EMP_TT_MAX_VAR(stParameters);
                        size_t EMP_TT_MAX_VAR(stBody);

                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            c_size stLeaf = pvstLeaves->val(stIndex);
                            node_c* const pLeaf = m_vNodes.ptr(stLeaf);
                            lexeme_c const * const pLeafLexeme = pLeaf->m_pLexeme;
                            if (pLeafLexeme == m_pLeftPar)
                            { stReturns = stLeaf; }
                            else if (pLeafLexeme == m_pRightPar)
                            { stParameters = stLeaf; }
                            else if (pLeafLexeme == m_pLeftBrace)
                            { stBody = stLeaf; }
                        }

                        if (emp::tt::is_not_max(stReturns))
                        { EMP_RET_FALSE_IF_NOT(build_asm(stReturns, a_rProgram, a_rTable, false)); }

                        rti::type_c const* const pType = pLexeme->m_pType;
                        EMP_RET_FALSE_IF(pType == nullptr);

                        rti::function_c const* const pFunctionType =rti::to_type<rti::function_c const>(pType);
                        EMP_RET_FALSE_IF(pFunctionType == nullptr);

                        c_size stParameterSize = pFunctionType->m_stParameterSize;
                        EMP_RET_FALSE_IF(stParameterSize >= emp::tt::max<ui32>::value);

                        if (emp::tt::is_not_max(stParameters))
                        {
                            EMP_RET_FALSE_IF_NOT((stParameterSize > 0));
                            EMP_RET_FALSE_IF_NOT(build_asm(stParameters, a_rProgram, a_rTable, false));
                        }

                        EMP_RET_FALSE_IF(emp::tt::is_max(stBody));
                        EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Enter));
                        a_rProgram.add_data<ui32>(static_cast<ui32>(stParameterSize));
                        EMP_RET_FALSE_IF_NOT(build_asm(stBody, a_rProgram, a_rTable, false));
                        EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Return));
                    }
                    ++a_rProgram.m_stCurrentGen;
                    return true;
                }
                case node_e::Call:
                {
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("#0 : '#1'",
                        eLexeme == lexeme_e::Function ? ALITERAL("Function") : ALITERAL("Variant"), m_vNodes.ref(a_stNode).m_sName);)
                    EMP_RET_FALSE_IF_NOT(build_arguments(a_stNode, a_rProgram, a_rTable));
                    if (eLexeme == lexeme_e::Function)
                    { EMP_RET_FALSE_IF_NOT(a_rProgram.add_function_call(pLexeme->m_ui32Index)); }
                    else if (eLexeme == lexeme_e::Variant)
                    { EMP_RET_FALSE_IF_NOT(a_rProgram.add_variant_call(pLexeme->m_ui32Index)); }
                    else
                    {
                        EMP_LOG_ERROR("invalid function/variant call");
                        return false;
                    }
                    return true;
                }
                case node_e::Declaration:
                case node_e::RealLiteral:
                case node_e::IntegralLiteral:
                case node_e::none:
                default:
                {
                    EMP_LOG_ERROR("unhandled node type #0", static_cast<ui32>(eNode));
                }
            };
            return false;
        }
        case lexeme_e::Comment:
        case lexeme_e::none:
        default:
        {
            EMP_LOG_ERROR("unhandled lexeme '#0' : #1", enum_to_astring(eLexeme), enum_to(eLexeme));
            return false;
        }
    };
// @@0 unreachable    return false;
}

#undef XTMP_OSE_UNHANDLED


template <typename T>
EMP_RETURN bool compiler_c::set_global_data(program_c& a_rProgram, rti::data_c const* const a_pType, astring const& a_sName, T const a_Value, T const a_Default)
{
    auto& rTable = a_rProgram.m_Table;
    c_size stNode = get_global(a_rProgram, a_sName);
    if (m_vNodes.is_valid_index(stNode))
    {
        if (!emp::tt::equal_(a_Value, a_Default))
        {
            // literal
            lexeme_c* const pLiteralLexeme = rTable.reg_literal<T>(a_Value);
            c_size stLiteral = alloc_node(&a_rProgram);
            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stLiteral));
            EMP_RET_FALSE_IF_NOT(fill_node(stLiteral, pLiteralLexeme, {0, 0})); //@@1 invalid line/column number

            // if (pLiteral->lexeme() == nullptr || is_call(pLiteral->lexeme())) @@0 needed ?
            // { pLiteral->set_value(node_e::Call); }

            c_size stChild0 = m_vNodes.ref(stNode).m_pvstLeaves->val(0);
            c_size stChild1 = m_vNodes.ref(stChild0).m_pvstLeaves->val(0);
            m_vNodes.ref(stChild1).m_pLexeme = pLiteralLexeme;
            m_vNodes.ref(stNode).m_pLexeme->m_bCold = false;
            return true;
        }
        else
        {
            m_vNodes.ref(stNode).m_pLexeme->m_bCold = true;
            return true;
        }
    }
    else
    {
        if(!emp::tt::equal_(a_Value, a_Default))
        {
            c_size stDeclaration = alloc_node(&a_rProgram);
            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stDeclaration));
            EMP_RET_FALSE_IF_NOT(fill_node(stDeclaration, m_pDeclare, {0, 0})); // @@0 invalid line/column number

            lexeme_c const* const pDeclarationLexeme = rTable.get_lexeme(a_sName, a_pType);
            if (pDeclarationLexeme != nullptr)
            {
                EMP_LOG_ERROR("duplicate symbol '@#0 #1'", a_sName, a_pType->m_sName);
                return false;
            }

            // data
            data_c* const pData = rTable.reg_data(a_sName, a_pType);
            EMP_RET_FALSE_IF(pData == nullptr);
            pData->m_bLocal = false;
            pData->m_bCold = false;

            auto* pDeclaration = m_vNodes.ptr(stDeclaration);
            pDeclaration->m_pLexeme = pData;
            pDeclaration->m_eType = node_e::DefinitionU;

            // body
            c_size stBody =  alloc_node(&a_rProgram);
            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stBody));
            EMP_RET_FALSE_IF_NOT(fill_node(stBody, m_pLeftBrace, {0,0})); // @@0 invalid line/column number.

            // literal
            lexeme_c* const pLiteralLexeme = rTable.reg_literal<T>(a_Value);
            c_size stLiteral = alloc_node(&a_rProgram);
            EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stLiteral));
            EMP_RET_FALSE_IF_NOT(fill_node(stLiteral, pLiteralLexeme, {0, 0})); // @@0 invalid line/column number.

            // if (pLiteral->lexeme() == nullptr || is_call(pLiteral->lexeme()))
            // { pLiteral->set_value(node_e::Call); }

            EMP_RET_FALSE_IF_NOT(add_child_node(stBody, stLiteral));
            EMP_RET_FALSE_IF_NOT(add_child_node(stDeclaration, stBody));
            EMP_RET_FALSE_IF_NOT(insert_child_node(a_rProgram.m_stAst, stDeclaration, 0));
            return true;
        }
        else
        {
            return true;
        }
    }

    return false;
}

template <typename T>
EMP_RETURN bool compiler_c::get_global_data(program_c& a_rProgram, astring const& a_sName, T& a_Value, T const a_Default)
{
   a_Value = a_Default;

    c_size stNode = get_global(a_rProgram, a_sName);
    if (m_vNodes.is_valid_index(stNode))
    {
        c_size stChild0 = m_vNodes.ref(stNode).m_pvstLeaves->val(0);
        EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stChild0));

        c_size stChild1 = m_vNodes.ref(stChild0).m_pvstLeaves->val(0);
        EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(stChild1));

        lexeme_c const* const pLexeme = m_vNodes.ref(stChild1).m_pLexeme;
        EMP_RET_FALSE_IF(pLexeme == nullptr);

        literal_t<T> const* const pLiteral = to_literal<T>(pLexeme);
        if (pLiteral == nullptr)
        {
            literal_t<int> const* const pCastedLiteral = to_literal<int>(pLexeme);
            EMP_RET_FALSE_IF(pCastedLiteral == nullptr);
            a_Value = static_cast<T>(pCastedLiteral->m_tLiteral);
            return true;
        }

        a_Value = pLiteral->literal();
        return true;
    }
    return false;
}

EMP_RETURN size_t compiler_c::get_global(program_c& a_rProgram, astring const& a_sName)
{
    c_size stAst = a_rProgram.m_stAst;
    EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stAst));
    auto* pvstLeaves = m_vNodes.ref(stAst).m_pvstLeaves;
    EMP_RET_MAX_IF(size_t, pvstLeaves == nullptr);

    c_size stCount = pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stLeaf = pvstLeaves->val(stIndex);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stLeaf));

        auto& rLeaf = m_vNodes.ref(stLeaf);
        EMP_RET_MAX_IF(size_t, rLeaf.m_eType != node_e::DefinitionU);

        lexeme_c* const pLexeme = rLeaf.m_pLexeme;
        EMP_RET_MAX_IF(size_t, pLexeme == nullptr);
        EMP_RET_MAX_IF(size_t, rLeaf.m_pvstLeaves == nullptr);
        EMP_RET_MAX_IF(size_t, rLeaf.m_pvstLeaves->count() <= 0);
        if (!emp::str::cmp(pLexeme->m_sName, a_sName))
        { continue; }

        c_size stBody = rLeaf.m_pvstLeaves->val(0);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stBody));

        auto& rBody = m_vNodes.ref(stBody);
        EMP_RET_MAX_IF(size_t, rBody.m_pLexeme != m_pLeftBrace);
        EMP_RET_MAX_IF(size_t, rBody.m_pvstLeaves == nullptr);
        EMP_RET_MAX_IF(size_t, rBody.m_pvstLeaves->count() <= 0);

        c_size stValue = rBody.m_pvstLeaves->val(0);
        EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(stValue));

        return stLeaf;
    }
    return emp::tt::max<size_t>::value;
}

EMP_RETURN bool compiler_c::set_float(program_c& a_rProgram, astring const& a_sName, float const a_Value, float const a_Default)
{
    float fValue = a_Value;
    if (emp::tt::is_inf(a_Value) || emp::tt::is_nan(a_Value))
    { fValue = a_Default; }
    return set_global_data<float>(a_rProgram, a_rProgram.m_Table.real_type(), a_sName, fValue, a_Default);
}

EMP_RETURN bool compiler_c::set_uint(program_c& a_rProgram, astring const& a_sName, uint32_t const a_Value, uint32_t const a_Default)
{
    return set_global_data<uint32_t>(a_rProgram, a_rProgram.m_Table.uint_type(), a_sName, a_Value, a_Default);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::rename(c_size a_stProgram, c_bool a_bPrune)
{
    EMP_RET_FALSE_IF_NOT(m_vpPrograms.is_valid_index(a_stProgram));

    auto* const pProgram = m_vpPrograms.val(a_stProgram);
    EMP_RET_FALSE_IF(pProgram == nullptr);

    c_size stAst = pProgram->m_stAst;
    table_c& rTable = pProgram->m_Table;

    if (a_bPrune)
    {
        EMP_RET_FALSE_IF_NOT(prune(a_stProgram));
        EMP_RET_FALSE_IF_NOT(prune_calls(rTable, stAst)); //@@RG only below main function
    }
    
    size_t stGenes = 0;
    EMP_RET_FALSE_IF_NOT(rename(stAst, rTable, stAst, stGenes));
    return true;
}

EMP_RETURN bool compiler_c::prune(size_t a_stProgram)
{
    EMP_RET_FALSE_IF_NOT(m_vpPrograms.is_valid_index(a_stProgram));

    auto* const pProgram = m_vpPrograms.val(a_stProgram);
    EMP_RET_FALSE_IF(pProgram == nullptr);

    c_size stAst = pProgram->m_stAst;
//    table_c& rTable = pProgram->m_Table;
    EMP_RET_FALSE_IF_NOT(prune_comments(stAst));
    EMP_RET_FALSE_IF_NOT(prune_defs(stAst));
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::prune_comments(c_size a_stNode)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    switch (eLexeme)
    {
        case lexeme_e::Comment:
        {
            pLexeme->m_bCold = true;
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Data:
        case lexeme_e::Function:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Variant:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(prune_comments(pvstLeaves->val(stIndex))); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::reprune(c_size a_stProgram)
{
     EMP_RET_FALSE_IF_NOT(m_vpPrograms.is_valid_index(a_stProgram));

    auto* const pProgram = m_vpPrograms.val(a_stProgram);
    EMP_RET_FALSE_IF(pProgram == nullptr);

    c_size stAst = pProgram->m_stAst;
   
   while (true)
   {
       c_size stReprune = reprune_defs(pProgram->m_Table, stAst);
       if (emp::tt::is_max(stReprune))
       { return false; }
       if (stReprune == 0)
        { break;}
   }

   return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN size_t compiler_c::reprune_calls(table_c& a_rTable, c_size a_stNode)
{
    EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    size_t stSum = 0;
    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    auto* const pDef = a_rTable.get_lexeme(pLexeme->m_sQualified);
                    if (pDef != nullptr && pDef->m_bCold)
                    {  
                        pDef->m_bCold = false;
                        ++stSum;
                    }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    auto* const pDef = a_rTable.get_lexeme(pLexeme->m_sQualified);
                    if (pDef != nullptr)
                    { pDef->m_bCold = false; }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            };
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stPrune = reprune_calls(a_rTable, pvstLeaves->val(stIndex));
        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stPrune));
        stSum += stPrune;
    }
    return stSum;
}
EMP_RETURN size_t compiler_c::reprune_defs(table_c& a_rTable, c_size a_stNode)
{
    EMP_RET_MAX_IF_NOT(size_t, m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    size_t stSum = 0;

    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                case node_e::Declaration:
                {
                    if (pLexeme->m_bCold == false || emp::str::cmp(pLexeme->m_sName, astring(ALITERAL("main"))))
                    {
                        c_size stPrune = reprune_calls(a_rTable, a_stNode);
                        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stPrune));
                        stSum += stPrune;
                    }
                    break;
                }
                case node_e::Call:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        c_size stPrune = reprune_defs(a_rTable, pvstLeaves->val(stIndex));
        EMP_RET_MAX_IF(size_t, emp::tt::is_max(stPrune));
        stSum += stPrune;
    }
    return stSum;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::prune_defs(c_size a_stNode)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                case node_e::Declaration:
                {
                    pLexeme->m_bCold = true;
                    pLexeme->m_bRenamed = false;
                    break;
                }
                case node_e::Call:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                case node_e::Declaration:
                {
                    pLexeme->m_bCold = true;
                    pLexeme->m_bRenamed = false;
                    break;
                }
                case node_e::Call:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            };
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(prune_defs(pvstLeaves->val(stIndex))); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::prune_calls(table_c& a_rTable, c_size a_stNode)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    auto* const pDef = a_rTable.get_lexeme(pLexeme->m_sQualified);
                    if (pDef != nullptr)
                    { pDef->m_bCold = false; }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    auto* const pDef = a_rTable.get_lexeme(pLexeme->m_sQualified);
                    if (pDef != nullptr)
                    { pDef->m_bCold = false; }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            };
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(prune_calls(a_rTable, pvstLeaves->val(stIndex))); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::rename_calls(c_size a_stNode, c_astring& a_sOldQualifiedName, c_astring& a_sNewName)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    if (emp::str::cmp(pLexeme->m_sQualified, a_sOldQualifiedName))
                    { pLexeme->rename(a_sNewName); }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::Call:
                {
                    if (emp::str::cmp(pLexeme->m_sQualified, a_sOldQualifiedName))
                    { pLexeme->rename(a_sNewName); }
                    break;
                }
                case node_e::DefinitionU:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            };
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(rename_calls(pvstLeaves->val(stIndex), a_sOldQualifiedName, a_sNewName)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::rename_def(c_size a_stAst, table_c& a_rTable, lexeme_c* const a_pLexeme, size_t& a_rstGenes)
{
    astring sOldName = a_pLexeme->m_sName;
    if ((false == a_pLexeme->m_bRenamed) && (false == a_pLexeme->m_bCold) && (false == emp::str::cmp(sOldName, astring(ALITERAL("main")))))
    {
        while (true)
        {
            astring sNewName = STR_AFORMAT("gen#0", a_rstGenes);
            astring sNewQualifiedName = a_pLexeme->compute_qualified_name(sNewName);
            ++a_rstGenes;
            if (emp::str::cmp(sNewQualifiedName, a_pLexeme->m_sQualified))
            {
                a_pLexeme->m_bRenamed = true;
                return true;
            }
            if (a_rTable.get_lexeme(sNewQualifiedName) == nullptr)
            {
                a_pLexeme->rename(sNewName);
                return rename_calls(a_stAst, a_pLexeme->m_sQualified, sNewName);
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::rename(c_size a_stAst, table_c& a_rTable, c_size a_stNode, size_t& a_rstGenes)
{
    EMP_RET_FALSE_IF_NOT(m_vNodes.is_valid_index(a_stNode));
    node_c& rNode = m_vNodes.ref(a_stNode);
    c_node_e eNode = rNode.m_eType;
    lexeme_c* const pLexeme = rNode.m_pLexeme;
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->m_eType : lexeme_e::none;
    switch (eLexeme)
    {
        case lexeme_e::Function:
        //case lexeme_e::Intrinsic: //@@RG: don't rename intrinsics
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                {
                    EMP_RET_FALSE_IF_NOT(rename_def(a_stAst, a_rTable, pLexeme, a_rstGenes));
                    break;
                }
                case node_e::Call:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            }
            break;
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::DefinitionU:
                {
                    EMP_RET_FALSE_IF_NOT(rename_def(a_stAst, a_rTable, pLexeme, a_rstGenes));
                    break;
                }
                case node_e::Call:
                case node_e::Declaration:
                case node_e::IntegralLiteral:
                case node_e::none:
                case node_e::RealLiteral:
                default:
                { break; }
            };
            break;
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Literal:
        case lexeme_e::none:
        case lexeme_e::Operation:
        case lexeme_e::Punctuation:
        case lexeme_e::Comment:
        default:
        { break; }
    };
    auto const* const pvstLeaves = rNode.m_pvstLeaves;
    c_size stCount = pvstLeaves == nullptr ? 0 : pvstLeaves->count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(rename(a_stAst, a_rTable, pvstLeaves->val(stIndex), a_rstGenes)); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

