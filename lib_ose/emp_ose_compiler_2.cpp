//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_ose_compiler_2.h"

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
namespace emp { namespace ose2 {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name node_e
#define enum_values\
    enum_value( none       )\
    enum_value( Definition )\
    enum_value( Call       )
#include "emp_tt_enum.def.h"

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
           (a_cValue >= 'G' && a_cValue <= 'G');
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
#define EMP_OSE_OUTPUT_SHELL(x_Shell) print_shell(astring(ALITERAL("EMP_XX_FUNCTION")), x_Shell)
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
        sTokens.append_string(a_hTokens.ref(stIndex).name());
        sTokens.append_char(' ');
    }

    if (stCount > 0)
    { sTokens.trim_right(); }
    sTokens.append_literal(ALITERAL("'"));

    EMP_LOG_INFO("#0", sTokens);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef punctuation_c const* const cpcpun;

EMP_RETURN bool eat_right_shell(c_si32 a_si32Stack, cpcpun a_pLeft, cpcpun a_pRight, token_hc& a_rhFrom, token_hc& a_rhTo);
EMP_RETURN bool eat_right_shell(c_si32 a_si32Stack, cpcpun a_pLeft, cpcpun a_pRight, token_hc& a_rhFrom, token_hc& a_rhTo)
{
    EMP_OSE_DEBUG_SHELL(a_rhFrom);

    size_t stRight = emp::tt::max<size_t>::value;

    size_t const stCount = a_rhFrom.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        size_t const stOffset = stCount - stIndex - 1;
        token_c const& rToken = a_rhFrom.ref(stOffset);
        if (rToken.stack() == a_si32Stack)
        {
            lexeme_c const* const pLexeme = rToken.lexeme();
            if (stRight == emp::tt::max<size_t>::value)
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
    return stRight == emp::tt::max<size_t>::value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool parse_symbol(c_astring& a_sValue);
EMP_RETURN bool parse_symbol(c_astring& a_sValue)
{
    size_t const stLen = a_sValue.len();
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
void print_tokens(token_cv& a_vTokens);
void print_tokens(token_cv& a_vTokens)
{
    size_t stMaxLen = 0;
    c_size stCount = a_vTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { stMaxLen = emp::tt::maxof(stMaxLen, a_vTokens.ref(stIndex).name().len()); }

    astring sSpace;

    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        token_c const& rToken = a_vTokens.ref(stIndex);
        c_size stSpaceCount = stMaxLen - rToken.name().len();

        sSpace.clear();
        for (size_t stSpace = 0; stSpace < stSpaceCount; ++stSpace)
        { sSpace.append_char(' '); }

        EMP_LOG_INFO("line #0 : col #1 : stack #2 : '#3#4' : lexeme '#5'",
            emp::str::int_fmt_3U(rToken.line_number()),
            emp::str::int_fmt_2U(rToken.column_number()),
            rToken.stack(), sSpace, rToken.name(),
            rToken.lexeme() != nullptr ? ALITERAL("true") : ALITERAL("false"));
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool mutate_node(table_c const& a_rTable, node_c* const a_pNode, c_size a_stIndex);
EMP_RETURN bool mutate_node(table_c const& a_rTable, node_c* const a_pNode, c_size a_stIndex)
{
    EMP_RET_FALSE_IF(a_pNode == nullptr);

    switch (a_pNode->lexeme()->lexeme())
    {
        case lexeme_e::Literal:
        {
            literal_t<float>* const pLiteral = to_literal<float>(a_pNode->m_pLexeme);
            if (pLiteral != nullptr)
            { pLiteral->m_tLiteral += emp::mat::random_float(pLiteral->m_tLiteral) * (emp::mat::random_float(1.0f) - 0.5f); }
            break;
        }
        case lexeme_e::Variant:
        {
            if ((a_stIndex % (2 + emp::mat::urandom() % 2)) == 0)
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
                //break;
            }
            // @@comment fall through
        }
        case lexeme_e::Function:
        {
            if (emp::str::cmp(a_pNode->m_pLexeme->name(), astring(ALITERAL("main"))))
            {
                break;
            }
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Operation:
        {
            if ((a_pNode->value() == node_e::Call || a_pNode->m_pLexeme->lexeme() == lexeme_e::Control))
            {
                auto const& vLexemes = a_rTable.lexemes().vector();
                size_t stCount = vLexemes.count();
                size_t stIndex = 0;
                size_t stRand = emp::mat::urandom() % stCount;
                size_t stMutation = 0;
                while (true)
                {
                    lexeme_c* const pLexeme = vLexemes.val(stIndex % stCount);
                    if (pLexeme->type() == a_pNode->m_pLexeme->type() &&
                        pLexeme != a_pNode->m_pLexeme &&
                        pLexeme->cold() == false)
                    {
                        //if (a_bFunction)
                        {
                            if ((pLexeme->lexeme() == lexeme_e::Function ||
                                pLexeme->lexeme() == lexeme_e::Variant) &&
                                emp::str::cmp(pLexeme->name(), astring(ALITERAL("main"))) == false// &&
                                //a_pNode->m_pLexeme->m_bMother && pLexeme->m_bMother == false
                                )
                            {
                                if (stMutation == stRand)
                                {
                                    a_pNode->m_pLexeme = pLexeme;
                                    break;
                                }
                                else
                                {
                                    ++stMutation;
                                }
                            }
                        }
                        //else
                        //{
                        //    if (pLexeme->lexeme() != lexeme_e::Function ||
                        //        emp::str::cmp(pLexeme->name(), astring(ALITERAL("main"))) == false)
                        //    {
                        //        if (stMutation == stRand)
                        //        {
                        //            a_pNode->m_pLexeme = pLexeme;
                        //            break;
                        //        }
                        //        else
                        //        {
                        //            ++stMutation;
                        //        }
                        //    }
                        //}
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

    return true;
}

EMP_RETURN bool merge_ast(program_c* const a_pMother, program_c* const a_pFather);
EMP_RETURN bool merge_ast(program_c* const a_pMother, program_c* const a_pFather)
{
    EMP_RET_FALSE_IF((a_pMother == nullptr) || (a_pMother == a_pFather));
    //a_pMother->table().cold(false);
    if (a_pFather != nullptr)
    {
        a_pMother->table().mother(true);
        a_pFather->table().mother(false);

        //a_pFather->table().cold(false);
        for (size_t stIndex = 0; stIndex < a_pFather->m_pAst->leaves().count(); ++stIndex)
        { a_pMother->m_pAst->leaves().add(a_pFather->m_pAst->leaves().val(stIndex)); }

        for (size_t stIndex = 0; stIndex < a_pFather->m_vpNodes.count(); ++stIndex)
        {
            node_c* const pNode = a_pFather->m_vpNodes.val(stIndex);
            if (pNode != a_pFather->m_pAst)
            { a_pMother->m_vpNodes.add(a_pFather->m_vpNodes.val(stIndex)); }
        }

        a_pFather->m_vpNodes.resize(0);
        a_pFather->m_vpNodes.add(a_pFather->m_pAst);
        a_pFather->m_pAst->leaves().resize(0);


        EMP_RET_FALSE_IF_NOT(a_pMother->table().merge(a_pFather->table()));

        for (size_t stIndex = 0; stIndex < a_pMother->m_vpNodes.count(); ++stIndex)
        {
            node_c* const pNode = a_pMother->m_vpNodes.val(stIndex);
            pNode->m_pLexeme = a_pMother->table().get_lexeme(pNode->m_pLexeme->qualified());
        }
    }

    return true;
}

EMP_RETURN bool compiler_c::mutate_ast_0(table_c& a_rTable, program_c* const a_pProgram)
{
    EMP_RET_FALSE_IF(a_pProgram == nullptr);

    auto& vpNodes = a_pProgram->m_vpNodes;
    c_size stNodeCount = vpNodes.count();

    c_size stMutationCount = 1 + (emp::mat::urandom() % 2) * (emp::mat::urandom() % 38) + (emp::mat::urandom() % 4);
    c_size stCrossOverCount = 1 + (emp::mat::urandom() % 2) * (emp::mat::urandom() % 38) + (emp::mat::urandom() % 4);
    //c_size stDeletionCount = (emp::mat::urandom() % 4);

    if (stNodeCount > 0)
    {
        for (size_t stMutation = 0; stMutation < stMutationCount; ++stMutation)
        {
            c_size stNodeIndex = emp::mat::urandom() % stNodeCount;
            node_c* const pNode = vpNodes.val(stNodeIndex);
            EMP_RET_FALSE_IF_NOT(mutate_node(a_rTable, pNode, stMutation));
        }

        for (size_t stMutation = 0; stMutation < stMutationCount; ++stMutation)
        {
            for (size_t stNode = 0; stNode < stNodeCount; ++stNode)
            {
                node_c* const pNode = vpNodes.val(stNode);
                c_node_e eNode = pNode->value();
                if (eNode == node_e::Call)
                {
                    c_lexeme_e eLexeme = pNode->lexeme()->lexeme();
                    if (eLexeme == lexeme_e::Function ||
                        eLexeme == lexeme_e::Variant)
                    {
                        EMP_RET_FALSE_IF_NOT(mutate_node(a_rTable, pNode, stMutation));
                        break;
                    }
                }
            }
        }

        {

            EMP_OSE_IF_DEBUG(
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// >> CHILD");
            EMP_LOG_INFO("//----------------------------------------");
            astring sSource;
            EMP_RET_FALSE_IF_NOT(print_ose(a_pProgram->m_pAst, sSource));
            EMP_LOG_INFO("#0", sSource);
            )
        }



        map_t<lexeme_c*, node_c*> mpDefinitions(8);
        for (size_t stNode = 0; stNode < vpNodes.count(); ++stNode)
        {
            node_c* const pNode = vpNodes.val(stNode);
            if (pNode->value() == node_e::Definition)
            {
                lexeme_c* const pLexeme = pNode->lexeme();
                EMP_RET_FALSE_IF(pLexeme == nullptr);
                c_lexeme_e eLexeme = pLexeme->lexeme();
                if (eLexeme == lexeme_e::Variant ||
                    eLexeme == lexeme_e::Function) // @@0 dub with data
                {
                    mpDefinitions.set(pLexeme, pNode);
                }
            }
        }

        auto const& l_find_calls = [](vector<node_c*>& a_rvpNodes, node_c* const a_pParent, auto& a_rmpDefinitions, auto const& a_lFindCalls) EMP_RETURN -> bool
        {
            EMP_RET_FALSE_IF(a_pParent == nullptr);
            if (a_pParent->value() != node_e::Definition)
            {
                // @@0 variables / data ??
                lexeme_c* const pLexeme = a_pParent->lexeme();
                EMP_RET_FALSE_IF(pLexeme == nullptr);
                c_lexeme_e eLexeme = pLexeme->lexeme();
                if (eLexeme == lexeme_e::Variant ||
                    eLexeme == lexeme_e::Function //|| @@0 bug when removing data definitions
                    //eLexeme == lexeme_e::Data
                    )
                {
                    auto const it = a_rmpDefinitions.get(pLexeme);
                    if (it != a_rmpDefinitions.end())
                    {
                        a_rmpDefinitions.remove(it);

                        for (size_t stIndex = 0; stIndex < a_rvpNodes.count(); ++stIndex)
                        {
                            node_c* const pNode = a_rvpNodes.val(stIndex);
                            if (pNode->value() == node_e::Definition)
                            {
                                c_lexeme_e eLexemeDef = pNode->lexeme() != nullptr ? pNode->lexeme()->lexeme() : lexeme_e::none;
                                if (eLexemeDef == lexeme_e::Function ||
                                    eLexemeDef == lexeme_e::Variant)
                                {
                                    EMP_RET_FALSE_IF_NOT(a_lFindCalls(a_rvpNodes, pNode, a_rmpDefinitions, a_lFindCalls));
                                }
                            }
                        }
                    }
                }
            }

            auto const& rvpLeaves = a_pParent->leaves();
            c_size stCount = rvpLeaves.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { EMP_RET_FALSE_IF_NOT(a_lFindCalls(a_rvpNodes, rvpLeaves.val(stIndex), a_rmpDefinitions, a_lFindCalls)); }

            return true;
        };


        lexeme_c* const pMainLexeme = a_rTable.get_function(ALITERAL("main()()")); // @@0 todo main could return and arg
        EMP_RET_FALSE_IF(pMainLexeme == nullptr);

        {
            node_c* pMain = nullptr;
            c_size stCount = vpNodes.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                node_c* const pNode = vpNodes.val(stIndex);
                if (pNode->value() != node_e::Definition)
                { continue; }

                if (pMainLexeme == pNode->lexeme())
                {
                    pMain = pNode;
                    break;
                }
            }
            EMP_RET_FALSE_IF_NOT(l_find_calls(vpNodes, pMain, mpDefinitions, l_find_calls));
        }

        auto const& l_delete_node = [](auto& a_rmpDefinitions, auto& a_rvpNodes, node_c* const a_pParent, auto const& a_lDeleteNode) EMP_RETURN -> bool
        {
            {
                c_size stCount = a_rvpNodes.count();
                for (size_t stNode = 0; stNode < stCount; ++stNode)
                {
                    node_c* const pNode = a_rvpNodes.val(stNode);
                    size_t stChild = 0;
                    while (stChild < pNode->leaves().count())
                    {
                        if (pNode->leaves().val(stChild) == a_pParent)
                        { pNode->leaves().remove(stChild); }
                        else
                        { ++stChild; }
                    }
                }

                for (size_t stNode = 0; stNode < stCount; ++stNode)
                {
                    if (a_rvpNodes.val(stNode) == a_pParent)
                    {
                        a_rvpNodes.remove(stNode);
                        break;
                    }
                }
            }

            {
                auto const& rvpLeaves = a_pParent->leaves();
                c_size stCount = rvpLeaves.count();
                for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                { EMP_RET_FALSE_IF_NOT(a_lDeleteNode(a_rmpDefinitions, a_rvpNodes, rvpLeaves.val(stIndex), a_lDeleteNode)); }
            }

            for (auto itChild = a_rmpDefinitions.begin(); itChild != a_rmpDefinitions.end(); ++itChild)
            {
                if (*itChild.value() == a_pParent)
                {
                    a_rmpDefinitions.remove(itChild);
                }
            }
            EMP_DEALLOCATE(a_pParent);
            return true;
        };

        {
            auto const oItMain = mpDefinitions.get(pMainLexeme);
            if (oItMain != mpDefinitions.end())
            { mpDefinitions.remove(oItMain); }
        }

        {
            EMP_OSE_IF_DEBUG(EMP_LOG_INFO("removing #0 definitions", mpDefinitions.count());)
            EMP_OSE_IF_DEBUG(
            for (auto it = mpDefinitions.begin(); it != mpDefinitions.end(); ++it)
            {
                astring sSource;
                EMP_RET_FALSE_IF_NOT(print_ose(*it.value(), sSource));
                EMP_OSE_IF_DEBUG(EMP_LOG_INFO("=====>>>> removing definition\n #0", sSource);)
            }
            )
        }


        //size_t stDeleted = 0;
        for (auto it = mpDefinitions.begin(); it != mpDefinitions.end(); ++it)
        {
            /*if (mpDefinitions.count() - stDeleted <= stDeletionCount)
            { break; }
            ++stDeleted;*/

            (*it.key())->m_bCold = true;
            EMP_RET_FALSE_IF_NOT(l_delete_node(mpDefinitions, vpNodes, *it.value(), l_delete_node));
            size_t stIndex = 0;
            while (stIndex < vpNodes.count())
            {
                node_c* const pChild = vpNodes.val(stIndex);
                if (*it.key() == pChild->lexeme())
                { EMP_RET_FALSE_IF_NOT(l_delete_node(mpDefinitions, vpNodes, pChild, l_delete_node)); }
                else
                { ++stIndex; }
            }
        }

        for (size_t stIndex = 0; stIndex < stCrossOverCount; ++stIndex)
        {
            c_size stRandomIndex = emp::mat::urandom();
            node_c* const pNode = a_pProgram->m_vpNodes.val(stRandomIndex % a_pProgram->m_vpNodes.count());
            node_c* const pNext = a_pProgram->m_vpNodes.val((stRandomIndex + 38) % a_pProgram->m_vpNodes.count());


            for (size_t stNode = 0; stNode < a_pProgram->m_vpNodes.count(); ++stNode)
            {
                node_c* const pTest = a_pProgram->m_vpNodes.val(stNode);
                for (size_t stChild = 0; stChild < pTest->leaves().count(); ++stChild)
                {
                    node_c* const pChild = pTest->leaves().val(stChild);
                    if (pChild == pNode)
                    {
                        pTest->leaves().set(stChild, pNext);
                    }
                    else if (pChild == pNext)
                    {
                        pTest->leaves().set(stChild, pNode);
                    }
                }
            }
        }
    }

    return true;
}

EMP_RETURN bool mutate_ast(table_c const& a_rTable, node_c* a_pNode, size_t& a_rstIndex);
EMP_RETURN bool mutate_ast(table_c const& a_rTable, node_c* a_pNode, size_t&a_rstIndex)
{
    ++a_rstIndex;
    EMP_RET_FALSE_IF(a_pNode == nullptr);

    switch (a_pNode->lexeme()->lexeme())
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
            if (emp::str::cmp(a_pNode->m_pLexeme->name(), astring(ALITERAL("main"))))
            {
                break;
            }
        }
        case lexeme_e::Control:
        case lexeme_e::Intrinsic:
        case lexeme_e::Operation:
        {
            if ((a_pNode->value() == node_e::Call || a_pNode->m_pLexeme->lexeme() == lexeme_e::Control)// &&
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
                    if (pLexeme->type() == a_pNode->m_pLexeme->type() &&
                        pLexeme != a_pNode->m_pLexeme)
                    {
                        if (pLexeme->lexeme() != lexeme_e::Function ||
                            emp::str::cmp(pLexeme->name(), astring(ALITERAL("main"))) == false)
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

    c_size stCount = a_pNode->leaves().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        node_c* pChild = a_pNode->leaves().val(stIndex);
        EMP_RET_FALSE_IF(mutate_ast(a_rTable, pChild, a_rstIndex));
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void print_ast(node_c const* const a_pNode, c_size a_stStack);
void print_ast(node_c const* const a_pNode, c_size a_stStack)
{
    astring sFormat;
    for (size_t stChar = 0; stChar < a_stStack; ++stChar)
    { sFormat.append_char('-'); }
    sFormat.append_literal(ALITERAL(" ")); // @@0 string why not char ?

    if (a_pNode->value() == node_e::Definition)
    { sFormat.append_char('@'); }

    lexeme_c const* const pLexeme = a_pNode->lexeme();
    if (pLexeme != nullptr)
    { sFormat.append_string(pLexeme->qualified()); }
    else
    { sFormat.append_string(a_pNode->name()); }

    EMP_LOG_INFO("#0", sFormat); // @@0 single argument format

    c_size stCount = a_pNode->leaves().count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { print_ast(a_pNode->leaves().val(stIndex), a_stStack + 1); }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool print_ose(node_c const* const a_pNode, map_t<node_c const*, bool>& a_rmpNodes, c_size a_stStack, astring& a_rsOutput, bool& a_rbEndScope);
EMP_RETURN bool print_ose(node_c const* const a_pNode, map_t<node_c const*, bool>& a_rmpNodes, c_size a_stStack, astring& a_rsOutput, bool& a_rbEndScope)
{
    EMP_LOG_INFO("---------------------------------------");
    EMP_LOG_INFO("#0", a_rsOutput);

    a_rbEndScope = false;
    EMP_RET_FALSE_IF(a_rmpNodes.get(a_pNode) != a_rmpNodes.end());

    a_rmpNodes.set(a_pNode, true); // @@0 optim set return iterator

    auto const oFinally = emp::tt::finally([&a_rmpNodes, a_pNode]{ a_rmpNodes.remove(a_rmpNodes.get(a_pNode)); });

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
    //            a_rsOutput.append_string(pDataType->name());
    //            break;
    //        }
    //        case rti::type_e::Function:
    //        {
    //            rti::function_c const* const pFunctionType = rti::to_type<rti::function_c>(a_pType);
    //            EMP_RET_FALSE_IF(pFunctionType == nullptr);
    //            EMP_RET_FALSE_IF_NOT(l_dump_vector(a_rsOutput, pFunctionType->returns(), false, a_lDumpSignature));
    //            EMP_RET_FALSE_IF_NOT(l_dump_vector(a_rsOutput, pFunctionType->parameters(), a_bDefinition, a_lDumpSignature));
    //            break;
    //        }
    //        case rti::type_e::Void:
    //        case rti::type_e::none:
    //        default:
    //        { return false; }
    //    };

    //    return true;
    //};

    c_node_e eNode = a_pNode->value();
    auto const& vpLeaves = a_pNode->leaves();
    c_size stCount = vpLeaves.count();
    lexeme_c const* const pLexeme = a_pNode->lexeme();
    c_lexeme_e eLexeme = pLexeme != nullptr ? pLexeme->lexeme() : lexeme_e::none;

    switch (eLexeme)
    {
        case lexeme_e::Function:
        case lexeme_e::Intrinsic:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::Definition:
                {
                    c_bool bVariant = eLexeme == lexeme_e::Variant;

                    EMP_RET_FALSE_IF_NOT(eLexeme == lexeme_e::Function || bVariant);
                    //function_c const* const pFunction = to_function<function_c>(pLexeme);
                    //EMP_RET_FALSE_IF(pFunction == nullptr);
                    a_rsOutput.append_char('@');
                    a_rsOutput.append_string(pLexeme->name());

                    rti::function_c const* const pFunctionType = rti::to_type<rti::function_c>(pLexeme->type());
                    EMP_RET_FALSE_IF(pFunctionType == nullptr);

                    auto const& l_dump_list = [&a_rsOutput, &vpLeaves, stCount](node_c const* const l_pNode, c_bool a_bParameters) EMP_RETURN -> bool
                    {
                        EMP_RET_FALSE_IF(l_pNode == nullptr);

                        a_rsOutput.append_char('(');
                        //@@0 remove auto const& vpLeaves = a_pNode->leaves();
                        // @@0 remove c_size lstCount = vpLeaves.count();
                        if (stCount > 0)
                        {
                            auto const& rvpParameters = vpLeaves.val(0)->leaves();
                            c_size stParameterCount = rvpParameters.count();
                            for (size_t stIndex = 0; stIndex < stParameterCount; ++stIndex)
                            {
                                node_c const* const pChild = rvpParameters.val(stIndex);
                                EMP_RET_FALSE_IF(pChild == nullptr);
                                if (a_bParameters)
                                { a_rsOutput.append_char('@'); }
                                lexeme_c const* const pChildLex = pChild->lexeme();
                                EMP_RET_FALSE_IF(pChildLex == nullptr);
                                a_rsOutput.append_string(pChildLex->qualified());
                                a_rsOutput.append_char(',');
                                a_rsOutput.append_char(' ');

                                // @@0 recurse to support function as parameter

                            }
                            a_rsOutput.trim_right(2);
                        }
                        a_rsOutput.append_char(')');
                        return true;
                    };

                    c_size stVariant = bVariant ? 1u : 0u;

                    if (bVariant)
                    {
                        a_rsOutput.append_char(' ');
                        a_rsOutput.append_string(vpLeaves.val(0)->name());
                    }

                    if (stCount == (3 + stVariant))
                    {
                        EMP_RET_FALSE_IF_NOT(l_dump_list(vpLeaves.val(0 + stVariant), false));
                        EMP_RET_FALSE_IF_NOT(l_dump_list(vpLeaves.val(1 + stVariant), true));
                    }
                    else if (stCount == (2 + stVariant))
                    {
                        EMP_RET_FALSE_IF_NOT(l_dump_list(vpLeaves.val(0 + stVariant), true));
                    }
                    else if (stCount == (1 + stVariant))
                    { }
                    else
                    { EMP_RET_FALSE_IF(true); }

                    a_rsOutput.append_char('=');
                    a_rsOutput.append_char('\n');

                    EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.last_val(), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
                    return true;
                }
                case node_e::Call:
                {
                    a_rsOutput.append_string(pLexeme->name());

                    if (stCount > 0)
                    {
                        a_rsOutput.append_char('(');
                        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                        {
                            EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(stIndex), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
                            a_rsOutput.append_char(',');
                            a_rsOutput.append_char(' ');
                        }
                        a_rsOutput.trim_right(2);
                        a_rsOutput.append_char(')');
                    }
                    return true;
                }
                case node_e::none:
                default:
                { EMP_RET_FALSE_IF(true); }
            }
        }
        case lexeme_e::Punctuation:
        {
            punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
            EMP_RET_FALSE_IF(pPunctuation == nullptr);
            c_punctuation_e ePunctuation = pPunctuation->punctuation();
            switch (ePunctuation)
            {
                case punctuation_e::LeftBrace:
                {
                    //l_print_tab(a_stStack);
                    a_rsOutput.append_string(pLexeme->name());
                    a_rsOutput.append_char('\n');
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        l_print_tab(a_stStack + 1);
                        EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(stIndex), a_rmpNodes, a_stStack + 1, a_rsOutput, a_rbEndScope));
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
                    a_rsOutput.append_string(pLexeme->name());
                    a_rsOutput.append_char('\n');
                    return true;
                }
                case punctuation_e::SemiColon:
                {
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(stIndex), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
                        if (a_rbEndScope == false)
                        { a_rsOutput.append_char(';'); }
                        a_rsOutput.append_char('\n');
                    }
                    return true;
                }
                case punctuation_e::RightPar:
                case punctuation_e::Assign:
                case punctuation_e::Space:
                case punctuation_e::Comma:
                case punctuation_e::LeftPar:
                case punctuation_e::Declare:
                case punctuation_e::none:
                default:
                { EMP_RET_FALSE_IF(true); }
            };
        }
        case lexeme_e::Literal:
        {
            a_rsOutput.append_string(pLexeme->name());
            return true;
        }
        case lexeme_e::Operation:
        {
            if (stCount > 0)
            {
                if (stCount == 1)
                { a_rsOutput.append_string(pLexeme->name()); }
                else
                {
                    a_rsOutput.append_char('(');
                }

                //else
                {
                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(stIndex), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
                        a_rsOutput.append_char(' ');
                        a_rsOutput.append_string(pLexeme->name());
                        a_rsOutput.append_char(' ');
                    }
                    a_rsOutput.trim_right(pLexeme->name().len() + 2);
                }

                if (stCount > 1)
                {
                    a_rsOutput.append_char(')');
                }
                return true;
            }
        }
        case lexeme_e::Data:
        {
            switch (eNode)
            {
                case node_e::Definition:
                {
                    a_rsOutput.append_char('@');
                    a_rsOutput.append_string(pLexeme->qualified());
                    if (stCount > 0)
                    {
                        EMP_RET_FALSE_IF(vpLeaves.val(0)->leaves().count() != 1);
                        a_rsOutput.append_char('=');
                        EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(0)->leaves().val(0), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
                    }
                    return true;
                }
                case node_e::Call:
                {
                    a_rsOutput.append_string(pLexeme->name());
                    return true;
                }
                case node_e::none:
                default:
                { EMP_RET_FALSE_IF(true); }
            };
        }
        case lexeme_e::Control:
        {
            EMP_RET_FALSE_IF_NOT((stCount > 0));
            a_rsOutput.append_string(pLexeme->name());
            a_rsOutput.append_char(' ');
            a_rsOutput.append_char('(');
            EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(0), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
            a_rsOutput.append_char(')');
            a_rsOutput.append_char('\n');
            if (stCount > 1)
            {
                l_print_tab(a_stStack);
                EMP_RET_FALSE_IF_NOT(print_ose(vpLeaves.val(1), a_rmpNodes, a_stStack, a_rsOutput, a_rbEndScope));
            }
            return true;
        }
        case lexeme_e::Comment:
        case lexeme_e::none:
        default:
        { EMP_RET_FALSE_IF(true); }
    };

    EMP_RET_FALSE_IF(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool print_asm(program_c const& a_rProgram);
EMP_RETURN bool print_asm(program_c const& a_rProgram)
{
    memory_walker_c oWalker(a_rProgram.instructions());

    size_t const stCount = a_rProgram.exit_point();
    while (oWalker.pointer() < stCount)
    {
        EMP_XX_IF_LOG(c_size stPointer = oWalker.pointer();)

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
                EMP_LOG_INFO("@#0 : Function : #1 : '#2' @#3",
                    emp::str::int_fmt_S04H(stPointer),
                    a_rProgram.get_index_from_address(ui32Instruction),
                    a_rProgram.get_name_from_address(ui32Instruction),
                    emp::str::int_fmt_S04H(ui32Instruction));
                break;
            }
            case instruction_e::Variant:
            {
                ui32 ui32Instruction = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32Instruction));
                EMP_LOG_INFO("@#0 : Variant : #1 : '#2'",
                    emp::str::int_fmt_S04H(stPointer),
                    ui32Instruction,
                    a_rProgram.table().get_name_from_index(ui32Instruction));
                break;
            }
#define XTMP_PRINT_ASM(x_Instruction)\
            case instruction_e::x_Instruction:\
            {\
                EMP_LOG_INFO("@#0 : #1", emp::str::int_fmt_S04H(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)));\
                break;\
            }
            XTMP_PRINT_ASM(PushScope)
            XTMP_PRINT_ASM(PopScope)
            XTMP_PRINT_ASM(Return)
            case instruction_e::Enter:
            {
                ui32 ui32ParameterSize = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32ParameterSize));
                EMP_LOG_INFO("@#0 : Enter : #1", emp::str::int_fmt_S04H(stPointer), ui32ParameterSize);
                break;
            }
            case instruction_e::Jump:
            {
                ui32 ui32Jump = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32Jump));
                EMP_LOG_INFO("@#0 : Jump : @#1", emp::str::int_fmt_S04H(stPointer), emp::str::int_fmt_S04H(ui32Jump));
                break;
            }
            case instruction_e::JumpCmp_S32:
            {
                ui32 ui32End = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32End));
                EMP_LOG_INFO("@#0 : JumpCmp_S32 : @#1",emp::str::int_fmt_S04H(stPointer),
                    emp::str::int_fmt_S04H(ui32End));
                break;
            }
            case instruction_e::JumpCmp_Bool:
            {
                ui32 ui32End = 0;
                EMP_RET_FALSE_IF_NOT(oWalker.next(ui32End));
                EMP_LOG_INFO("@#0 : JumpCmp_Bool : @#1", emp::str::int_fmt_S04H(stPointer),
                    emp::str::int_fmt_S04H(ui32End));
                break;
            }
            XTMP_PRINT_ASM(Equal_I32)
            XTMP_PRINT_ASM(More_F32)
            XTMP_PRINT_ASM(More_I32)
            XTMP_PRINT_ASM(MoreEq_F32)
            XTMP_PRINT_ASM(MoreEq_I32)
            XTMP_PRINT_ASM(Less_F32)
            XTMP_PRINT_ASM(Less_I32)
            XTMP_PRINT_ASM(LessEq_F32)
            XTMP_PRINT_ASM(LessEq_I32)
            XTMP_PRINT_ASM(Add_F32)
            XTMP_PRINT_ASM(Add_I32)
            XTMP_PRINT_ASM(Sub_F32)
            XTMP_PRINT_ASM(Sub_I32)
            XTMP_PRINT_ASM(Mul_F32)
            XTMP_PRINT_ASM(Mul_I32)
            XTMP_PRINT_ASM(Div_F32)
            XTMP_PRINT_ASM(Div_I32)
            XTMP_PRINT_ASM(Mod_I32)
            XTMP_PRINT_ASM(Neg_F32)
            XTMP_PRINT_ASM(Neg_I32)
            XTMP_PRINT_ASM(Abs_F32)
            XTMP_PRINT_ASM(Abs_I32)
            XTMP_PRINT_ASM(CosD_F32)
            XTMP_PRINT_ASM(SinD_F32)
            XTMP_PRINT_ASM(Cos_F32)
            XTMP_PRINT_ASM(Sin_F32)
            XTMP_PRINT_ASM(Print_I32)
            XTMP_PRINT_ASM(Randf_F32)
#undef XTMP_PRINT_ASM
#define XTMP_PRINT_PUSH(x_Instruction, x_Type)\
            case instruction_e::x_Instruction:\
            {\
                x_Type tValue = 0;\
                EMP_RET_FALSE_IF_NOT(oWalker.next(tValue));\
                EMP_LOG_INFO("@#0 : #1 : #2", emp::str::int_fmt_S04H(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)), tValue);\
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
                EMP_LOG_INFO("@#0 : #1 : #2", emp::str::int_fmt_S04H(stPointer), ALITERAL(EMP_PP_STRING(x_Instruction)), u32Address);\
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
            case instruction_e::none:
            {
                EMP_LOG_INFO("@#0 : UNHANDLED INSTRUCTION none", emp::str::int_fmt_S04H(stPointer));
                return false;
            }
            case instruction_e::Invalid:
            {
                EMP_LOG_INFO("@#0 : UNHANDLED INSTRUCTION Invalid", emp::str::int_fmt_S04H(stPointer));
                return false;
            }
            case instruction_e::Extension:
            default:
            {
                if (eInstruction >= instruction_e::Extension)
                {
                    c_astring& sName = a_rProgram.table().get_name_from_instruction(eInstruction);
                    EMP_RET_FALSE_IF_NOT((sName.len() > 0));

                    c_size stExtension = enum_to__(instruction_e::Extension);
                    c_size stInstruction = enum_to(eInstruction);
                    EMP_RET_FALSE_IF_NOT((stInstruction >= stExtension));

                    EMP_LOG_INFO("@#0 : Extension : #1, '#2'", emp::str::int_fmt_S04H(stPointer),
                        static_cast<ui32>(stInstruction - stExtension), sName);
                    break;
                }

                EMP_LOG_INFO("@#0 : UNHANDLED INSTRUCTION #1", emp::str::int_fmt_S04H(stPointer),
                    static_cast<ui32>(eInstruction));
                return false;
            }
        }
    }
    return true;
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
void context_c::add_line(void)
{
    ++m_stLine;
    m_stColumn = 0;
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
        c_astring& sToken = a_rToken.name();

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

    a_rToken.set_line_number(m_stLine);
    a_rToken.set_column_number(m_stColumn - a_rToken.name().len());
    EMP_RET_FALSE_IF(m_siStack < 0, "failed to resolve token, invalid stack");

    a_rToken.set_stack(m_siStack);
    a_rToken.set_st_stack(static_cast<size_t>(m_siStack));
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
    m_pProgram(),
    m_pCurrentProgram(nullptr),
    m_vTokens(),
    m_pWeightNode(nullptr),
    m_pWeight(nullptr),
    m_pTrue(nullptr),
    m_pFalse(nullptr),
    m_pComma(nullptr),
    m_pSemiColon(nullptr),
    m_pRightBrace(nullptr),
    m_pDeclare(nullptr),
    m_pLeftBrace(nullptr),
    m_pSpace(nullptr),
    m_pAssign(nullptr),
    m_pRightPar(nullptr),
    m_pLeftPar(nullptr),
    m_pContext(),
    m_pTable(),
    m_vpDeleteNodes(),
    m_vpPrograms(),
    m_mpPrograms(64),
    m_mpNodes(256)
{}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
compiler_c::~compiler_c(void)
{
    emp::cnt::delete_null_resize(m_vpDeleteNodes);
    emp::cnt::delete_null_resize(m_vpPrograms);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::create(void)
{
    EMP_RET_FALSE_IF_NOT(m_pTable->create());

    rti::type_c const* const pSIntType = m_pTable->sint_type();
    rti::type_c const* const pRealType = m_pTable->real_type();
    rti::type_c const* const pBoolType = m_pTable->bool_type();

    { // ()()
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // ()(int,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // ()(bool,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pBoolType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (float,)()
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (float,)(float,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (bool,)(float,float,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pBoolType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (float,)(float,float,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pRealType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (int,)(int,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (bool,)(int,int,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pBoolType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

    { // (int,)(int,int,)
        rti::function_c* const pType = m_pTable->new_type<rti::function_c>();
        EMP_RET_FALSE_IF(pType == nullptr);
        EMP_RET_FALSE_IF_NOT(pType->add_return(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(pType->add_parameter(pSIntType));
        EMP_RET_FALSE_IF_NOT(m_pTable->qualify_type(pType));
        EMP_RET_FALSE_IF_NOT(m_pTable->reg_type(pType));
    }

#define XTMP_com(x_Val, ...) m_pTable->reg_comment    (comment_e    ::x_Val,__VA_ARGS__)
#define XTMP_pun(x_Val, ...) m_pTable->reg_punctuation(punctuation_e::x_Val,__VA_ARGS__)
#define XTMP_ope(x_Val, ...) m_pTable->reg_operation  (instruction_e::x_Val,__VA_ARGS__)
#define XTMP_ctr(x_Val, ...) m_pTable->reg_control    (control_e    ::x_Val,__VA_ARGS__)
#define XTMP_ist(x_Val, ...) m_pTable->reg_intrinsic  (instruction_e::x_Val,__VA_ARGS__)
#define XTMP_lit(x_Val, ...) m_pTable->reg_literal<x_Val>(__VA_ARGS__)

#define XTMP_COM(...) EMP_RET_FALSE_IF(XTMP_com(__VA_ARGS__) == nullptr)
#define XTMP_PUN(...) EMP_RET_FALSE_IF(XTMP_pun(__VA_ARGS__) == nullptr)
#define XTMP_OPE(...) EMP_RET_FALSE_IF(XTMP_ope(__VA_ARGS__) == nullptr)
#define XTMP_CTR(...) EMP_RET_FALSE_IF(XTMP_ctr(__VA_ARGS__) == nullptr)
#define XTMP_IST(...) EMP_RET_FALSE_IF(XTMP_ist(__VA_ARGS__) == nullptr)
#define XTMP_LIT(...) EMP_RET_FALSE_IF(XTMP_lit(__VA_ARGS__) == nullptr)

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

XTMP_OPE( Equal_I32     , ALITERAL("==") ,  7 , ALITERAL("(bool,)(int,int,)")     , true  );
XTMP_OPE( More_F32      , ALITERAL(">")  ,  8 , ALITERAL("(bool,)(float,float,)") , true  );
XTMP_OPE( More_I32      , ALITERAL(">")  ,  8 , ALITERAL("(bool,)(int,int,)")     , true  );
XTMP_OPE( MoreEq_F32    , ALITERAL(">=") ,  8 , ALITERAL("(bool,)(float,float,)") , true  );
XTMP_OPE( MoreEq_I32    , ALITERAL(">=") ,  8 , ALITERAL("(bool,)(int,int,)")     , true  );
XTMP_OPE( Less_F32      , ALITERAL("<")  ,  9 , ALITERAL("(bool,)(float,float,)") , true  );
XTMP_OPE( Less_I32      , ALITERAL("<")  ,  9 , ALITERAL("(bool,)(int,int,)")     , true  );
XTMP_OPE( LessEq_F32    , ALITERAL("<=") ,  9 , ALITERAL("(bool,)(float,float,)") , true  );
XTMP_OPE( Add_F32       , ALITERAL("+")  , 10 , ALITERAL("(float,)(float,float,)"), true  );
XTMP_OPE( Add_I32       , ALITERAL("+")  , 10 , ALITERAL("(int,)(int,int,)")      , true  );
XTMP_OPE( Sub_F32       , ALITERAL("-")  , 10 , ALITERAL("(float,)(float,float,)"), true  );
XTMP_OPE( Sub_I32       , ALITERAL("-")  , 10 , ALITERAL("(int,)(int,int,)")      , true  );
XTMP_OPE( Mul_F32       , ALITERAL("*")  , 11 , ALITERAL("(float,)(float,float,)"), true  );
XTMP_OPE( Mul_I32       , ALITERAL("*")  , 11 , ALITERAL("(int,)(int,int,)")      , true  );
XTMP_OPE( Div_F32       , ALITERAL("/")  , 11 , ALITERAL("(float,)(float,float,)"), true  );
XTMP_OPE( Div_I32       , ALITERAL("/")  , 11 , ALITERAL("(int,)(int,int,)")      , true  );
XTMP_OPE( Mod_I32       , ALITERAL("%")  , 11 , ALITERAL("(int,)(int,int,)")      , true  );
XTMP_OPE( Neg_F32       , ALITERAL("-")  , 12 , ALITERAL("(float,)(float,)")      , false );
XTMP_OPE( Neg_I32       , ALITERAL("-")  , 12 , ALITERAL("(int,)(int,)")          , false );

m_pRightPar   = XTMP_pun( RightPar      , ALITERAL(")")  , 13 );
m_pLeftPar    = XTMP_pun( LeftPar       , ALITERAL("(")  , 14 );


m_pWeight     = XTMP_lit( real, ALITERAL("1.0")  , 1.0f  );
m_pTrue       = XTMP_lit( bool, ALITERAL("true") , true  );
m_pFalse      = XTMP_lit( bool, ALITERAL("false"), false );

XTMP_IST( Abs_F32       , ALITERAL("abs")  , ALITERAL("(float,)(float,)"));
XTMP_IST( Abs_I32       , ALITERAL("abs")  , ALITERAL("(int,)(int,)"    ));
XTMP_IST( CosD_F32      , ALITERAL("cosd") , ALITERAL("(float,)(float,)"));
XTMP_IST( SinD_F32      , ALITERAL("sind") , ALITERAL("(float,)(float,)"));
XTMP_IST( Cos_F32       , ALITERAL("cos")  , ALITERAL("(float,)(float,)"));
XTMP_IST( Sin_F32       , ALITERAL("sin")  , ALITERAL("(float,)(float,)"));
XTMP_IST( Print_I32     , ALITERAL("print"), ALITERAL("()(int,)"        ));
XTMP_IST( Randf_F32     , ALITERAL("randf"), ALITERAL("(float,)()"      ));
XTMP_CTR( Loop          , ALITERAL("loop") , ALITERAL("()(int,)"        ));
XTMP_CTR( If            , ALITERAL("if")   , ALITERAL("()(bool,)"       ));
#undef XTMP_CTR
#undef XTMP_OPE
#undef XTMP_PUN
#undef XTMP_COM
#undef XTMP

    m_pWeightNode = fill_node(EMP_ALLOCATE(node_c), m_pWeight);

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
#define PRINT_IMPL(x_Type, x_Token, ...)\
    x_Type("EE : l-#0 : c-#1 : #2", (x_Token).line_number(), (x_Token).column_number(), __VA_ARGS__);

#define PRINT_ERROR(x_Token, ...) PRINT_IMPL(EMP_LOG_ERROR, x_Token, __VA_ARGS__)
#define PRINT_WARNING(x_Token, ...) PRINT_IMPL(EMP_LOG_ERROR, x_Token, __VA_ARGS__)
#define PRINT_INFO(x_Token, ...) PRINT_IMPL(EMP_LOG_INFO, x_Token, __VA_ARGS__)
#define EMP_OSE_ERROR(x_Token, x_Format, ...)\
    PRINT_IMPL(EMP_LOG_ERROR, x_Token, STR_AFORMAT(x_Format, __VA_ARGS__)) //@@0 alloc,

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compiler_c::clear(void)
{
    m_pContext->destroy();
    m_vTokens.resize(0);

    //emp::cnt::delete_null_resize(m_vpDeleteNodes);

    //m_pWeightNode = add_node(m_pWeight);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::from_ast(node_c const& EMP_SILENT(a_rNode), program_c& EMP_SILENT(a_rProgram), bool& a_rbCache)
{
    a_rbCache = false;
    return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN program_c* compiler_c::from_file(c_astring& a_sFile, c_bool a_bMutate, bool& a_rbCache, program_c* const a_pFather)
{
    m_pCurrentProgram = nullptr;
    a_rbCache = false;

    emp::fs::file_c oFile;
    EMP_RET_NULL_IF_NOT(oFile.create(a_sFile, emp::fs::file_e::Text, false), "failed to open file '#0'", a_sFile);

    astring sBuffer;
    EMP_RET_NULL_IF_NOT(oFile.read_all(sBuffer), "failed to read file '#0'", a_sFile);

    EMP_RET_NULL_IF(sBuffer.is_empty(), "empty file '#0'", a_sFile);

    program_c* pProgram = nullptr;// from_cache(sBuffer);
    //if (pProgram != nullptr)
    //{ a_rbCache = true; }
    //else
    {
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("//----------------------------------------");)
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("// building '#0'", a_sFile);)
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("//----------------------------------------");)
        pProgram = from_string(sBuffer, a_bMutate, a_pFather);
        if (pProgram == nullptr)
        {
            EMP_OSE_IF_DEBUG(EMP_LOG_ERROR("Failed to build file '#0'", a_sFile));
            return nullptr;
        }
        pProgram->m_sFilename = a_sFile;
        EMP_OSE_IF_DEBUG(
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// builded '#0'", a_sFile);
            EMP_LOG_INFO("//----------------------------------------");)
    }

    return pProgram;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN program_c* compiler_c::from_cache(c_astring& a_sExpression)
{
    program_m::iterator_c const oIt = m_mpPrograms.get(a_sExpression);
    if (oIt != m_mpPrograms.end())
    { return *oIt.value(); }
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN program_c* compiler_c::from_string(c_astring& a_sExpression, c_bool a_bMutate, program_c* const a_pFather)
{
    clear();

    m_pCurrentProgram = EMP_ALLOCATE(program_c);

    auto oFinally = emp::tt::finally([this]()
    { EMP_DEALLOCATE(m_pCurrentProgram); m_pCurrentProgram = nullptr; });

    EMP_RET_NULL_IF_NOT(m_pCurrentProgram->table().clone(*m_pTable));

    //m_vpPrograms.add(m_pCurrentProgram);
    m_pCurrentProgram->m_sExpression = a_sExpression;

    m_pContext->create(a_sExpression);

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// starting to build #0", a_sExpression);
        EMP_LOG_INFO("//----------------------------------------");)
    EMP_RET_NULL_IF_NOT(build_tokens(*m_pCurrentProgram));
    EMP_RET_NULL_IF_NOT(m_pContext->end_of_context());

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> TOKENS");
        EMP_LOG_INFO("//----------------------------------------");
    print_tokens(m_vTokens);)

    m_pCurrentProgram->m_pAst = build_ast(token_chc(m_vTokens), 0, m_pCurrentProgram->table());
    EMP_RET_NULL_IF(m_pCurrentProgram->m_pAst == nullptr);
    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST");
        EMP_LOG_INFO("//----------------------------------------");
        print_ast(m_pCurrentProgram->m_pAst, 0);
    )

    EMP_RET_NULL_IF_NOT(resolve(m_pCurrentProgram->m_pAst, m_pCurrentProgram->table()));
    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> AST");
        EMP_LOG_INFO("//----------------------------------------");
        print_ast(m_pCurrentProgram->m_pAst, 0);
    )

        EMP_OSE_IF_DEBUG(
            {
                EMP_LOG_INFO("//----------------------------------------");
                EMP_LOG_INFO("// >> OSE");
                EMP_LOG_INFO("//----------------------------------------");
                astring sSource;
                EMP_RET_NULL_IF_NOT(print_ose(m_pCurrentProgram->m_pAst, sSource));
                EMP_LOG_INFO("#0", sSource);
            }
            )

    if (a_bMutate)
    {
        EMP_RET_NULL_IF_NOT(merge_ast(m_pCurrentProgram, a_pFather));

        EMP_OSE_IF_DEBUG(
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// >> SEX");
            EMP_LOG_INFO("//----------------------------------------");
            print_ast(m_pCurrentProgram->m_pAst, 0);)

        EMP_RET_NULL_IF_NOT(mutate_ast_0(m_pCurrentProgram->table(), m_pCurrentProgram));

 /*       EMP_OSE_IF_DEBUG(
            EMP_LOG_INFO("//----------------------------------------");
            EMP_LOG_INFO("// >> MUT");
            EMP_LOG_INFO("//----------------------------------------");
            print_ast(m_pCurrentProgram->m_pAst, 0);)*/

        EMP_OSE_IF_DEBUG(
            {
                EMP_LOG_INFO("//----------------------------------------");
                EMP_LOG_INFO("// >> OSE");
                EMP_LOG_INFO("//----------------------------------------");
                astring sSource;
                EMP_RET_NULL_IF_NOT(print_ose(m_pCurrentProgram->m_pAst, sSource));
                EMP_LOG_INFO("#0", sSource);
            }
        )
    }

    EMP_RET_NULL_IF_NOT(build_asm(m_pCurrentProgram->m_pAst->leaves(), *m_pCurrentProgram, m_pCurrentProgram->table()));
    EMP_RET_NULL_IF_NOT(m_pCurrentProgram->link());
    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// >> ASM");
        EMP_LOG_INFO("//----------------------------------------");
        EMP_RET_NULL_IF_NOT(print_asm(*m_pCurrentProgram));
        
    )

    EMP_OSE_IF_DEBUG(
        EMP_LOG_INFO("//----------------------------------------");
        EMP_LOG_INFO("// compilation ended sucessfully");
        EMP_LOG_INFO("//----------------------------------------");)

    //m_mpPrograms.set(a_sExpression, m_pCurrentProgram);
    oFinally.release();
    return m_pCurrentProgram;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::fill_node(node_c* const a_pNode, lexeme_c* const a_pLexeme)
{
    EMP_RET_NULL_IF(a_pNode == nullptr);
    a_pNode->set_lexeme(a_pLexeme);
    return a_pNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::fill_node(node_c* const a_pNode, token_c const& a_rToken)
{
    EMP_RET_NULL_IF(a_pNode == nullptr);
    a_pNode->set_lexeme(a_rToken.lexeme());
    a_pNode->set_name(a_rToken.name());
    return a_pNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::print_ose(node_c const* const a_pNode, astring& a_rsOutput)
{
    m_mpNodes.clear();
    size_t stStack = 0;
    bool bEndScope = false;
    return emp::ose::print_ose(a_pNode, m_mpNodes, stStack, a_rsOutput, bEndScope);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_RETURN node_c* compiler_c::parse_declarations(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    size_t const stDeclarationCount = a_vhTokens.count();
    EMP_RET_NULL_IF_NOT((stDeclarationCount > 0))

    node_c* pRoot = add_node(m_pSemiColon);

    {
        token_chc& hTokens = a_vhTokens.ref(0);
        size_t const stTokenCount = hTokens.count();
        if (stTokenCount != 0)
        {
            node_c* const pStatement = build_ast(hTokens, a_si32Stack, a_rTable);
            EMP_RET_NULL_IF(pStatement == nullptr)

            if (pStatement->lexeme() == m_pSemiColon)
            { emp::cnt::copy(pStatement->leaves(), pRoot->leaves()); }
            else
            { pRoot->leaves().add(pStatement); }
        }
    }

    for (size_t stIndex = 1; stIndex < stDeclarationCount; ++stIndex)
    {
        node_c* const pDeclaration = parse_declaration(a_vhTokens.ref(stIndex), a_si32Stack, a_rTable);
        EMP_RET_NULL_IF(pDeclaration == nullptr)
        pRoot->leaves().add(pDeclaration);
    }

    EMP_RET_NULL_IF_NOT((pRoot->leaves().count() > 0))
    return pRoot;
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct lexeme_s
{
    lexeme_c* lexeme;
    size_t index;
    size_t precedence;
};

typedef emp::cnt::vector<lexeme_s> lexeme_v;
typedef lexeme_v const lexeme_cv;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool get_lexemes(table_c const& a_rTable, lexeme_v& a_rvLexemes, token_chc& a_hTokens, c_si32 a_siStack);
EMP_RETURN bool get_lexemes(table_c const& a_rTable, lexeme_v& a_rvLexemes, token_chc& a_hTokens, c_si32 a_siStack)
{
    bool bOperation = false;

    c_size stCount = a_hTokens.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        token_c const& rToken = a_hTokens.ref(stIndex);
        if (rToken.stack() == a_siStack)
        {
            bool const bPrevious = bOperation;
            bOperation = false;
            lexeme_c* pLexeme = rToken.lexeme();
            if (pLexeme == nullptr)
            { continue; }

            c_lexeme_e eLexeme = pLexeme->lexeme();
            size_t EMP_TT_MAX_VAR(stNewPrecedence);

            if (eLexeme == lexeme_e::Punctuation)
            {
                punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
                EMP_RET_FALSE_IF(pPunctuation == nullptr);
                stNewPrecedence = pPunctuation->precedence();
            }
            else if (eLexeme == lexeme_e::Operation)
            {
                bOperation = true;
                auto const pOverloads = a_rTable.get_overload(pLexeme->name());
                auto const& vpLexemes = pOverloads->vector();

                c_size stOverloadCount = vpLexemes.count();
                for (size_t stOverload = 0; stOverload < stOverloadCount; ++stOverload)
                {
                    lexeme_c* const pOverload = vpLexemes.val(stOverload);
                    EMP_RET_FALSE_IF(pOverload == nullptr);

                    c_lexeme_e eOverload = pOverload->lexeme();
                    EMP_RET_FALSE_IF(eOverload != lexeme_e::Operation);

                    operation_c* const pOperation = to_lexeme<operation_c>(pOverload);
                    EMP_RET_FALSE_IF(pOperation == nullptr);

                    c_size stOverloadPrecedence = pOperation->precedence();
                    if (bPrevious)
                    {
                        rti::function_c const* const pFunction = pOperation->function();
                        EMP_RET_FALSE_IF(pFunction == nullptr);
                        if (pFunction->parameters().count() == 1)
                        {
                            if (pOperation->precedence() < stNewPrecedence)
                            {
                            goto GOTO_LABEL_ADD_LEXEME;
                            // @@0 wtf
                                //EMP_XX_IF_NOT_IOS(stNewPrecedence = emp::tt::minof(stNewPrecedence, stOverloadPrecedence);
                                //    pLexeme = pOperation;)
                            }
                        }
                    }
                    else
                    {
                    GOTO_LABEL_ADD_LEXEME:
                        stNewPrecedence = emp::tt::minof(stNewPrecedence, stOverloadPrecedence);
                        pLexeme = pOperation;
                    }
                }
            }
            else
            { continue; }

            if (stNewPrecedence != emp::tt::max<size_t>::value)
            {
                if (a_rvLexemes.count() > 0)
                {
                    size_t const stPrecedence = a_rvLexemes.ref(0).precedence;
                    if (stNewPrecedence > stPrecedence)
                    { continue; }
                    else if (stNewPrecedence < stPrecedence)
                    { a_rvLexemes.resize(0); }
                }

                lexeme_s& rLexeme = a_rvLexemes.push();
                rLexeme.lexeme = pLexeme;
                rLexeme.index = stIndex;
                rLexeme.precedence = stNewPrecedence;
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void get_shells(lexeme_cv& a_rvLexemes, token_chc& a_hFrom, token_vhc& a_rvTo);
void get_shells(lexeme_cv& a_rvLexemes, token_chc& a_hFrom, token_vhc& a_rvTo)
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
EMP_RETURN bool compiler_c::parse_parameters(token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, node_c* a_pOut)
{
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_FALSE_IF(a_pOut == nullptr);

    lexeme_v vLexemes;
    EMP_RET_FALSE_IF_NOT(get_lexemes(a_rTable, vLexemes, a_hTokens, a_si32Stack));

    size_t const stCount = vLexemes.count();
    if (stCount > 0)
    {
        lexeme_s const& rLexeme = vLexemes.ref(0);
        lexeme_c const* const pLexeme = rLexeme.lexeme;
        EMP_RET_FALSE_IF(pLexeme == nullptr);

        if (pLexeme == m_pComma)
        {
            token_vhc vhTokens;
            get_shells(vLexemes, a_hTokens, vhTokens);
            size_t const stParameterCount = vhTokens.count();
            EMP_RET_FALSE_IF(stParameterCount < 2);
            for (size_t stIndex = 0; stIndex < stParameterCount; ++stIndex)
            {
                token_chc& hTokens = vhTokens.ref(stIndex);
                size_t const stTokenCount = hTokens.count();
                //EMP_RET_FALSE_IF_NOT((stTokenCount > 0) && (stIndex + 1) < stParameterCount);

                if (stTokenCount > 0)
                {
                    node_c* const pParameter = parse_declaration(hTokens, a_si32Stack, a_rTable, true);
                    EMP_RET_FALSE_IF(pParameter == nullptr);
                    a_pOut->leaves().add(pParameter);
                }
            }
            return true;
        }
    }
    else
    {
        node_c* const pParameter = parse_declaration(a_hTokens, a_si32Stack, a_rTable, true);
        EMP_RET_FALSE_IF(pParameter == nullptr);
        a_pOut->leaves().add(pParameter);
        return true;
    }

    EMP_LOG_ERROR("failed to parse paremeter list");
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_declaration(token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable, bool a_bLocal)
{
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_NULL_IF_NOT((a_hTokens.count() > 0));
    token_hc hTokens(a_hTokens);

    token_c const& rDeclaration = hTokens.ref(0);
    EMP_RET_NULL_IF(rDeclaration.lexeme() != m_pDeclare);
    hTokens.trim_left();
    EMP_RET_NULL_IF_NOT((hTokens.count() > 0));

    token_c const& rName = hTokens.ref(0);
    EMP_RET_NULL_IF(rName.lexeme() != nullptr);
    hTokens.trim_left();
    EMP_RET_NULL_IF_NOT((hTokens.count() > 0));

    node_c* const pName = fill_node(m_pCurrentProgram->add_node(), rName);
    c_astring& sName = pName->name();

    node_c* pWeight = nullptr;
    float fVariant = 0.0f;


    lexeme_v vLexemes;
    EMP_RET_NULL_IF_NOT(get_lexemes(a_rTable, vLexemes, hTokens, a_si32Stack));

    token_hc hBody;

    if (vLexemes.count() > 0)
    {
        lexeme_s const& rLexeme = vLexemes.ref(0);
        lexeme_c const* const pLexeme = rLexeme.lexeme;
        if (pLexeme == m_pAssign)
        {
            token_vhc vhTokens;
            get_shells(vLexemes, hTokens, vhTokens);
            size_t const stShellCount = vhTokens.count();
            EMP_RET_NULL_IF(stShellCount != 2);

            hTokens = vhTokens.ref(0);
            hBody = vhTokens.ref(1);
        }
    }

    bool const bHasBody = hBody.count() > 0;

    // @@0 BUG using the body to detect if its a function or value...
    // @@0 BUG not working without body for function parameters
    // @@0 BUG initalizer list.
    bool const bFunction = bHasBody && hBody.ref(0).lexeme() == m_pLeftBrace;

    node_c* pBody = nullptr;
    if (bHasBody)
    {
        pBody = fill_node(m_pCurrentProgram->add_node(), m_pLeftBrace);
        if (bFunction)
        { hBody.trim_left(); }
        node_c* const pChild = build_ast(hBody, a_si32Stack + (bFunction ? 1 : 0), a_rTable);
        EMP_RET_NULL_IF(pChild == nullptr);

        if (pChild->lexeme() == m_pSemiColon)
        { emp::cnt::copy(pChild->leaves(), pBody->leaves()); }
        else
        { pBody->leaves().add(pChild); }
    }

    lexeme_c* pDeclaredLexeme = nullptr;
    if (bFunction)
    {
        if (hTokens.count() > 0)
        {
            token_c const& rWeight = hTokens.ref(0);
            lexeme_c* const pLexeme = rWeight.lexeme();
            if (pLexeme != nullptr &&
                pLexeme->lexeme() == lexeme_e::Literal)
            {
                hTokens.trim_left();
                rti::type_c const* const pType = pLexeme->type();
                // @@0 literal
                if (pType == a_rTable.uint_type())
                {
                    literal_t<uint> const* const pLiteral = to_literal<uint>(pLexeme);
                    EMP_RET_NULL_IF(pLiteral == nullptr);
                    fVariant = static_cast<float>(pLiteral->literal());
                }
                else if (pType == a_rTable.sint_type())
                {
                    literal_t<sint> const* const pLiteral = to_literal<sint>(pLexeme);
                    EMP_RET_NULL_IF(pLiteral == nullptr);
                    fVariant = static_cast<float>(pLiteral->literal());
                }
                else if (pType == a_rTable.real_type())
                {
                    literal_t<real> const* const pLiteral = to_literal<real>(pLexeme);
                    EMP_RET_NULL_IF(pLiteral == nullptr);
                    fVariant = pLiteral->literal();
                }
                else
                {
                    EMP_OSE_ERROR(rWeight, "invalid weight type '#0'", (pType != nullptr ? pType->name() : astring()));
                    return nullptr;
                }
                pWeight = fill_node(m_pCurrentProgram->add_node(), pLexeme);
                pName->leaves().add(pWeight);
            }
        }

        token_hc hParameters;
        EMP_RET_NULL_IF_NOT(eat_right_shell(0, m_pLeftPar, m_pRightPar, hTokens, hParameters));

        node_c* pParameters = nullptr;
        if (hParameters.count() > 0)
        {
            pParameters = fill_node(m_pCurrentProgram->add_node(), m_pRightPar);
            EMP_RET_NULL_IF_NOT(parse_parameters(hParameters, a_si32Stack + 1, a_rTable, pParameters));
            if (pParameters != nullptr)
            { pName->leaves().add(pParameters); }
        }

        node_c* pReturns = nullptr;
        if (hTokens.count() > 0)
        {
            pReturns = fill_node(m_pCurrentProgram->add_node(), m_pLeftPar);
            EMP_RET_NULL_IF_NOT(parse_parameters(hTokens, a_si32Stack, a_rTable, pReturns));
            if (pReturns != nullptr)
            { pName->leaves().add(pReturns); }
        }

        rti::function_c const* pDefinedFunction = nullptr;
        rti::function_c* const pFunction = a_rTable.new_type<rti::function_c>();
        EMP_RET_NULL_IF(pFunction == nullptr);

        if (pReturns != nullptr)
        {
            auto const& rvpLeaves = pReturns->leaves();
            size_t stCount = rvpLeaves.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            { EMP_RET_NULL_IF_NOT(pFunction->add_return(get_type(rvpLeaves.val(stIndex)->lexeme()))); }
        }

        if (pParameters != nullptr)
        {
            auto const& rvpLeaves = pParameters->leaves();
            size_t const stCount = rvpLeaves.count();
            for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
            {
                lexeme_c const* const pParameterLexeme = rvpLeaves.val(stIndex)->lexeme();
                EMP_RET_NULL_IF(pParameterLexeme == nullptr);

                rti::type_c const* const pParameterType = get_type(pParameterLexeme);
                EMP_RET_NULL_IF(pParameterType == nullptr);

                EMP_RET_NULL_IF_NOT(pFunction->add_parameter(pParameterType));
            }
        }

        EMP_RET_NULL_IF((pFunction->name().len() > 0) != false);
        EMP_RET_NULL_IF_NOT(a_rTable.qualify_type(pFunction));
        EMP_RET_NULL_IF_NOT((pFunction->name().len() > 0));
        pDefinedFunction = a_rTable.get_function_type(pFunction->name());
        if (pDefinedFunction == nullptr)
        {
            EMP_RET_NULL_IF_NOT(a_rTable.reg_type(pFunction));
            pDefinedFunction = pFunction;
        }

        EMP_RET_NULL_IF(pDefinedFunction == nullptr);

        lexeme_c* const pLexeme = a_rTable.get_lexeme(sName, pDefinedFunction);

        variant_c* pVariant = nullptr;
        if (pLexeme != nullptr)
        {
            pVariant = to_function<variant_c>(pLexeme);
            if (pWeight != nullptr)
            {
                if (pVariant == nullptr)
                {
                    EMP_OSE_ERROR(rName, "'@#0' is already defined as NON variant", sName);
                    return nullptr;
                }
                pDeclaredLexeme = pVariant;
            }
            else if (pVariant == nullptr)
            {
                EMP_OSE_ERROR(rName, "duplicate symbol '@#0'", sName);
                return nullptr;
            }
            else
            {
                EMP_OSE_ERROR(rName, "'@#0' is already defined AS variant", sName);
                return nullptr;
            }
        }
        else if (pWeight != nullptr)
        {
            pVariant = a_rTable.reg_variant(sName, pDefinedFunction);
            pDeclaredLexeme = pVariant;
        }
        else
        { pDeclaredLexeme = a_rTable.reg_function(sName, pDefinedFunction); }

        if (pVariant != nullptr)
        {
            EMP_RET_NULL_IF(pWeight == nullptr);
            pVariant->add(fVariant);
        }
        else if (pDeclaredLexeme != nullptr)
        { EMP_RET_NULL_IF(pWeight != nullptr); }
        else
        {
            EMP_OSE_ERROR(rName, "failed to define symbol '@#0'", sName);
            return nullptr;
        }
    }
    else
    {
        EMP_RET_NULL_IF(pWeight != nullptr);
        EMP_RET_NULL_IF(hTokens.count() != 1);

        token_c const& rType = hTokens.ref(0);
        hTokens.trim_left();

        rti::type_c const* const pType = a_rTable.get_type(rType.name());
        EMP_RET_NULL_IF(pType == nullptr);

        rti::data_c const* const pDataType = rti::to_type<rti::data_c>(pType);
        EMP_RET_NULL_IF(pDataType == nullptr);

        /*EMP_XX_IF_DEBUG(*/EMP_XX_IF_LOG(c_astring& sType = pDataType->name();)/*)*/

        lexeme_c const* const pLexeme = a_rTable.get_lexeme(rName.name(), pDataType);
        if (pLexeme != nullptr)
        {
            EMP_OSE_ERROR(rName, "duplicate symbol '@#0 #1'", sName, sType);
            return nullptr;
        }

        data_c* const pData = a_rTable.reg_data(sName, pDataType);
        EMP_RET_NULL_IF(pData == nullptr);
        pData->set_local(a_bLocal);
        pDeclaredLexeme = pData;
    }

    EMP_RET_NULL_IF(pDeclaredLexeme == nullptr);
    pName->set_lexeme(pDeclaredLexeme);
    pName->set_value(node_e::Definition);
    if (bHasBody)
    {
        EMP_RET_NULL_IF(pBody == nullptr);
        pName->leaves().add(pBody);
    }

    return pName;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_scope_or_control(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    size_t const stShellCount = a_vhTokens.count();
    EMP_RET_NULL_IF_NOT((stShellCount > 0));

    if (stShellCount == 1)
    {
        token_chc hTokens = a_vhTokens.ref(0);
        return hTokens.count() == 0 ? fill_node(m_pCurrentProgram->add_node(), nullptr) : build_ast(hTokens, a_si32Stack, a_rTable);
    }
    else
    {
        node_c* pControl = nullptr;
        token_chc& hControl = a_vhTokens.ref(0);
        if (hControl.count() > 0)
        {
            pControl = parse_call(hControl, a_si32Stack, a_rTable);
            EMP_RET_NULL_IF(pControl == nullptr);
        }

        node_c* const pBody = fill_node(m_pCurrentProgram->add_node(), m_pLeftBrace);

        token_chc& hBody = a_vhTokens.ref(1);
        if (hBody.count() > 0)
        {
            node_c* const pChild = build_ast(hBody, a_si32Stack + 1, a_rTable);
            EMP_RET_NULL_IF(pChild == nullptr);

            if (pChild->lexeme() == m_pSemiColon)
            { emp::cnt::copy(pChild->leaves(), pBody->leaves()); }
        }

        if (pControl != nullptr)
        {
            lexeme_c const* const pLexeme = pControl->lexeme();
            if (pLexeme != nullptr &&
                pLexeme->lexeme() != lexeme_e::Control)
            {
                node_c* const pRoot = fill_node(m_pCurrentProgram->add_node(), m_pSemiColon);
                pRoot->leaves().add(pControl);
                pRoot->leaves().add(pBody);
                return pRoot;
            }

            pControl->leaves().add(pBody);
            return pControl;
        }

        return pBody;
    }

// @@0 unreachable    EMP_LOG_ERROR("failed to parse scope or control");
// @@0 unreachable    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_statements(token_cvhc& EMP_SILENT(a_vhTokens), c_si32 EMP_SILENT(a_si32Stack),
    table_c& EMP_SILENT(a_rTable))
{
    //EMP_LOG_ERROR(EMP_XX_FUNCTION);
    //EMP_OSE_DEBUG_SHELL(a_hTokens);
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_assignment(token_cvhc& EMP_SILENT(a_vhTokens), c_si32 EMP_SILENT(a_si32Stack),
    table_c& EMP_SILENT(a_rTable))
{
    //EMP_LOG_ERROR(EMP_XX_FUNCTION);
    //EMP_OSE_DEBUG_SHELL(a_hTokens);
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_calls(token_cvhc& a_vhTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    size_t const stStatementCount = a_vhTokens.count();
    EMP_RET_NULL_IF_NOT((stStatementCount > 0));

    node_c* pRoot = fill_node(m_pCurrentProgram->add_node(), m_pSemiColon);

    for (size_t stIndex = 0; stIndex + 1 < stStatementCount; ++stIndex)
    {
        node_c* const pCall = parse_call(a_vhTokens.ref(stIndex), a_si32Stack, a_rTable);
        EMP_RET_NULL_IF(pCall == nullptr);
        pRoot->leaves().add(pCall);
    }

    {
        token_chc& hTokens = a_vhTokens.last_ref();
        size_t stTokenCount = hTokens.count();
        if (stTokenCount != 0)
        {
            node_c* const pStatement = build_ast(hTokens, a_si32Stack, a_rTable);
            EMP_RET_NULL_IF(pStatement == nullptr);

            if (pStatement->lexeme() == m_pSemiColon)
            { emp::cnt::copy(pStatement->leaves(), pRoot->leaves()); }
            else
            { pRoot->leaves().add(pStatement); }
        }
    }

    EMP_RET_NULL_IF_NOT((pRoot->leaves().count() > 0));
    return pRoot;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::parse_call(token_chc& a_hTokens, c_si32 a_si32Stack, table_c& a_rTable)
{
    EMP_OSE_DEBUG_SHELL(a_hTokens);
    EMP_RET_NULL_IF_NOT((a_hTokens.count() > 0));

    token_hc hTokens(a_hTokens);
    token_c const& rName = hTokens.ref(0);
    bool const bRealCall = rName.lexeme() != m_pLeftPar;

    if (bRealCall)
    { hTokens.trim_left(); }

    token_hc hArguments;
    EMP_RET_NULL_IF_NOT(eat_right_shell(a_si32Stack, m_pLeftPar, m_pRightPar, hTokens, hArguments));

    if (bRealCall == false)
    { return build_ast(hArguments, a_si32Stack + 1, a_rTable); }

    node_c* const pName = fill_node(m_pCurrentProgram->add_node(), rName);
    size_t const stArgumentCount = hArguments.count();
    if (stArgumentCount > 0)
    {
        node_c* pArgument = build_ast(hArguments, a_si32Stack + 1, a_rTable);
        EMP_RET_NULL_IF(pArgument == nullptr);

        if (pArgument->lexeme() == m_pComma)
        { emp::cnt::copy(pArgument->leaves(), pName->leaves()); }
        else
        { pName->leaves().add(pArgument); }
    }

    if (hTokens.count() == 0)
    {
        pName->set_value(node_e::Call);
        return pName;
    }

    EMP_OSE_ERROR(rName, "invalid function call : '#0'", rName.name());
    return nullptr;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::eat_token(table_c& a_rTable, astring& a_rsToken, lexeme_c*& a_rpLexeme, bool& a_rbOperator)
{
    a_rpLexeme = nullptr;
    a_rbOperator = false;

    char cChar = 0;

    c_astring& sExpression(m_pContext->expression());
    a_rsToken = m_pContext->expression();

    size_t stLen = m_pContext->expression_len();
    if ((stLen > 0) == false)
    { return true; }

    while (true)
    {
        cChar = m_pContext->get_char();
        if (is_newline(cChar))
        { m_pContext->add_line(); }
        else if (is_white(cChar) == false)
        { break; }

        stLen = m_pContext->eat_char();
        if ((stLen > 0) == false)
        {
            a_rsToken = astring();
            return true;
        }

        a_rsToken.trim_left();
    }

#define XTMP_OSE_PARSE(x_Type)\
if (stLen > 0)\
{\
    a_rsToken.trim_right(stLen); \
}\
    a_rpLexeme = a_rTable.reg_literal<x_Type>(a_rsToken);\
    EMP_RET_FALSE_IF(a_rpLexeme == nullptr, "failed to reg literal #0 #1;", ALITERAL(EMP_PP_STRING(x_Type)), a_rsToken);

    if (is_digit(cChar))
    {
        stLen = m_pContext->eat_char();
        if (stLen > 0)
        {
            c_char cLast = cChar;
            cChar = m_pContext->get_char();
            if (cLast == '0' && cChar == 'x')
            {
                while (true)
                {
                    stLen = m_pContext->eat_char();
                    if ((stLen > 0) == false)
                    { break; }
                    cChar = m_pContext->get_char();

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

                XTMP_OSE_PARSE(uint)
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

                    stLen = m_pContext->eat_char();
                    if ((stLen > 0) == false)
                    { break; }
                    cChar = m_pContext->get_char();
                }

                if (bDot)
                {
                    XTMP_OSE_PARSE(real)
                    return true;
                }
                XTMP_OSE_PARSE(sint)
                return true;
            }
        }
        else
        {
            XTMP_OSE_PARSE(sint)
            return true;
        }
    }
    else if (is_dot(cChar))
    {
        while (true)
        {
            stLen = m_pContext->eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_pContext->get_char();

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

        XTMP_OSE_PARSE(real)
        return true;
    }
    else if (is_letter(cChar))
    {
        while (true)
        {
            stLen = m_pContext->eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_pContext->get_char();

            if (is_letter(cChar) ||
                is_digit(cChar))
            { }
            else
            { break; }
        }

        a_rsToken.trim_right(stLen);
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
                if ((m_pContext->eat_char() > 1) == false)
                { return false; }
                emp::tt::silent(m_pContext->eat_char());
                return true;
            }

            stLen = m_pContext->eat_char();
            if ((stLen > 0) == false)
            { break; }
            cChar = m_pContext->get_char();

            if (is_digit(cChar) ||
                is_letter(cChar) ||
                is_dot(cChar) ||
                is_white(cChar))
            { break; }
            else if (is_newline(cChar))
            { break; }

            bOperator = true;
        }

        a_rsToken.trim_right(stLen);
        a_rbOperator = true;
        return true;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_tokens(program_c& a_rProgram)
{
    while (true)
    {
        size_t const stStartLen = m_pContext->expression_len();
        if ((stStartLen > 0) == false)
        { break; }

        token_c oToken;

        astring sToken;
        lexeme_c* pLexeme = nullptr;
        bool bOperator = false;

        EMP_RET_FALSE_IF_NOT(eat_token(a_rProgram.table(), sToken, pLexeme, bOperator));
        if ((sToken.len() > 0) == false)
        { break; }

        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("eat_token '#0'", sToken);)

        c_astring sLexeme(sToken);
        if (pLexeme != nullptr)
        {
            c_lexeme_e eLexeme = pLexeme->lexeme();
            switch (eLexeme)
            {
                case lexeme_e::Comment:
                {
                    switch (get_comment(pLexeme))
                    {
                        case comment_e::SingleComment:
                        {
                            size_t stLen = m_pContext->expression_len();
                            while (stLen > 0)
                            {
                                c_char cChar = m_pContext->get_char();
                                stLen = m_pContext->eat_char();
                                if (is_newline(cChar))
                                {
                                    m_pContext->add_line();
                                    break;
                                }
                            }
                            break;
                        }
                        case comment_e::BeginComment:
                        {
                            int siCommentStack = 1;

                            size_t stLen = m_pContext->expression_len();
                            while (stLen > 0)
                            {
                                c_char cChar = m_pContext->get_char();

                                if (is_newline(cChar))
                                { m_pContext->add_line(); }
                                else
                                {
                                    astring sComment(m_pContext->expression());
                                    sComment.trim_right(emp::tt::maxof<size_t>(sComment.len(), 2) - 2);
                                    comment_c const* const pComment = a_rProgram.table().get_comment(sComment);
                                    if (pComment != nullptr)
                                    {
                                        switch (pComment->comment())
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
                                        stLen = m_pContext->eat_char(); // @@note not using stLen but eating char
                                        if ((siCommentStack > 0) == false)
                                        {
                                            stLen = m_pContext->eat_char(); // @@note not using stLen but eating char
                                            break;
                                        }
                                    }
                                }
                                stLen = m_pContext->eat_char();
                            }

                            if (siCommentStack != 0)
                            {
                                emp::tt::silent(m_pContext->resolve(oToken, false));
                                PRINT_ERROR(oToken, STR_AFORMAT("invalid comment '#0'", sToken));
                                return false;
                            }
                            break;
                        }
                        case comment_e::EndComment:
                        case comment_e::none:
                        default:
                        {
                            emp::tt::silent(m_pContext->resolve(oToken, false));
                            PRINT_ERROR(oToken, STR_AFORMAT("invalid comment '#0'", sToken));
                            return false;
                        }
                    }
                    break;
                }
                case lexeme_e::Literal:
                case lexeme_e::Control:
                case lexeme_e::Intrinsic:
                {
                    oToken.set_lexeme(pLexeme);
                    EMP_RET_FALSE_IF_NOT(m_pContext->resolve(oToken, false));
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
                {
                    EMP_LOG_ERROR("invalid lexeme_e::'#0 : #1'", enum_to_astring(eLexeme), enum_to(eLexeme));
                    return false;
                }
            }
        }
        else if (bOperator)
        {
            size_t stParsedCount = 0;
            while (sToken.len())
            {
                pLexeme = a_rProgram.table().get_match(sToken);
                if (pLexeme != nullptr)
                {
                    oToken.set_lexeme(pLexeme);
                    if (m_pContext->resolve(oToken, true) == false)
                    {
                        PRINT_ERROR(oToken, ALITERAL("failed to resolve stack"));
                        return false;
                    }

                    stParsedCount += sToken.len();
                    oToken.set_column_number(oToken.column_number() - sLexeme.len() + stParsedCount);

                    m_vTokens.add(oToken);

                    sToken = sLexeme;
                    sToken.trim_left(stParsedCount);
                }
                else
                { sToken.trim_right(); }
            }

            if (stParsedCount != sLexeme.len())
            {
                emp::tt::silent(m_pContext->resolve(oToken, false));
                PRINT_ERROR(oToken, STR_AFORMAT("invalid operator '#0'", sLexeme));
                return false;
            }
        }
        else
        {
            oToken.set_name(sToken);
            EMP_RET_FALSE_IF_NOT(m_pContext->resolve(oToken, false));
            m_vTokens.add(oToken);
        }

        if (stStartLen == m_pContext->expression_len())
        {
            emp::tt::silent(m_pContext->resolve(oToken, false));
            PRINT_ERROR(oToken, STR_AFORMAT("unhandled token '#0'", sToken));
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool is_call(lexeme_c const* const a_pLexeme);
EMP_RETURN bool is_call(lexeme_c const* const a_pLexeme)
{
    EMP_RET_FALSE_IF(a_pLexeme == nullptr);
    c_lexeme_e eLexeme = a_pLexeme->lexeme();
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
#define enum_name ast_e
#define enum_values\
    enum_value( none       )
#include "emp_tt_enum.def.h"

struct ast_node_s
{
    ast_e m_eType;
    token_chc& m_hTokens;
    si32 m_siStack;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN node_c* compiler_c::build_ast(token_chc& a_hTokens, c_si32 a_siStack, table_c& a_rTable)
{
    vector<ast_node_s> m_vAstStack;
    m_vAstNodes.add({ ast_e::none, a_hTokens, 0});

    while (m_vAstNodes.count() > 0)
    {
        ast_node_s const& rAst = m_vAstNodes.last_ref();
    switch (rAst.m_eType)
    {
    case ast_e::none:
    {
    EMP_RET_NULL_IF(a_hTokens.count() <= 0);
    EMP_OSE_DEBUG_SHELL(a_hTokens);

    emp::cnt::vector<lexeme_s> vLexemes;
    EMP_RET_NULL_IF_NOT(get_lexemes(a_rTable, vLexemes, a_hTokens, a_siStack));

    size_t const stCount = vLexemes.count();
    if ((stCount > 0) == false)
    {
        if (a_hTokens.count() == 1)
        {
            token_c const& rToken = a_hTokens.ref(0);

            node_c* const pNode = fill_node(m_pCurrentProgram->add_node(), rToken);

            lexeme_c const* const pLexeme = rToken.lexeme();
            if (pLexeme == nullptr || is_call(pLexeme))
            { pNode->set_value(node_e::Call); }

            return pNode;
        }

        PRINT_ERROR(a_hTokens.ref(0), ALITERAL("invalid leaf"));
        return nullptr;
    }

    lexeme_s const& rLexeme = vLexemes.ref(0);
    lexeme_c* const pLexeme = rLexeme.lexeme;

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("lexeme found #0", pLexeme->name());)

    token_vhc vhTokens;
    get_shells(vLexemes, a_hTokens, vhTokens);
    size_t const stShellCount = vhTokens.count();

    if (pLexeme == m_pDeclare)
    {
        // @@0 BUG always global declaration.
        return parse_declaration(a_hTokens, a_siStack, a_rTable, false);
    }
    else if (pLexeme == m_pLeftBrace)
    { return parse_scope_or_control(vhTokens, a_siStack, a_rTable); }
    else if (pLexeme == m_pSpace)
    { return parse_statements(vhTokens, a_siStack, a_rTable); }
    else if (pLexeme == m_pAssign)
    { return parse_assignment(vhTokens, a_siStack, a_rTable); }
    else if (pLexeme == m_pRightPar)
    { return parse_call(a_hTokens, a_siStack, a_rTable); }

    node_c* pNode = fill_node(m_pCurrentProgram->add_node(), pLexeme == m_pRightBrace ? m_pSemiColon : pLexeme);

    if (is_call(pLexeme))
    { pNode->set_value(node_e::Call); }

    for (size_t stIndex = 0; stIndex < stShellCount; ++stIndex)
    {
        token_chc& hTokens = vhTokens.ref(stIndex);
        if (hTokens.count() > 0)
        {
            node_c* const pChild = build_ast(hTokens, a_siStack, a_rTable);
            EMP_RET_NULL_IF(pChild == nullptr);

            if (pChild->lexeme() == m_pSemiColon)
            { emp::cnt::copy(pChild->leaves(), pNode->leaves()); }
            else
            { pNode->leaves().add(pChild); }
        }
    }

    return pNode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_asm(emp::cnt::vector<node_c*> const& a_rvpNodes, program_c& a_rProgram, table_c& a_rTable)
{
    c_size stCount = a_rvpNodes.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(build_asm(a_rvpNodes.val(stIndex), a_rProgram, a_rTable)); }

    return stCount > 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define XTMP_OSE_UNHANDLED(x_Case)\
    case lexeme_e::x_Case:\
    {\
        EMP_LOG_ERROR(ALITERAL("unhandled lexeme #0, #1, '#2'"), static_cast<ui32>(lexeme_e::x_Case), EMP_PP_STRING(x_Case), a_pNode->name());\
        return false;\
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_arguments(node_c& a_rNode,
    program_c& a_rProgram, table_c& a_rTable)
{
    emp::cnt::vector<node_c*> const& rvpLeaves = a_rNode.leaves();
    size_t const stCount = rvpLeaves.count();
    if (stCount > 0)
    { EMP_RET_FALSE_IF_NOT(build_asm(rvpLeaves, a_rProgram, a_rTable), "failed to build argument list for call '#0'", a_rNode.name()); }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::resolve(node_c* const a_pNode, table_c& a_rTable)
{
    EMP_RET_FALSE_IF(a_pNode == nullptr);
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("trying to resolve : '#0'", a_pNode->name());)

    if (a_pNode->constant())
    {
        bool bCache = false;
        m_pStack->copy(m_pProgram);
        EMP_RET_FALSE_IF_NOT(from_ast(*a_pNode, *m_pProgram, bCache));
        size_t const stMaxCount = 1024;
        int siProcessed = m_pProcessor->run(*m_pProgram, *m_pStack, stMaxCount);
        EMP_RET_FALSE_IF(siProcessed < 0);

        size_t const stProcessed = static_cast<size_t>(siProcessed);
        if (stProcessed < stMaxCount) // @@0 why testing return false ?
        {
            //a_pNode->set_lexeme() new lexeme base on result
            EMP_LOG_ERROR("unhandled constant lexeme");
            return false;
        }

        EMP_LOG_ERROR("unhandled constant lexeme");
        return false;
    }

    emp::cnt::vector<node_c*> const& rvpLeaves = a_pNode->leaves();
    size_t const stCount = rvpLeaves.count();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    { EMP_RET_FALSE_IF_NOT(resolve(rvpLeaves.val(stIndex), a_rTable)); }

    if (a_pNode->value() != node_e::Call)
    { return true; }

    lexeme_c const* const pLexeme = a_pNode->lexeme();
    c_astring& sName = (pLexeme != nullptr && pLexeme->name().len() > 0) ? pLexeme->name() : a_pNode->name();

    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolving '#0'", sName);)

    table_c::lexeme_vm* const pOverloads = a_rTable.get_overload(sName);
    EMP_RET_FALSE_IF(pOverloads == nullptr, "unresolved symbol '#0'", sName);

    rti::type_c const* const pRealType = a_rTable.real_type();
    rti::type_c const* const pUIntType = a_rTable.uint_type();
    rti::type_c const* const pSIntType = a_rTable.sint_type();

    emp::cnt::vector<lexeme_c*> const& rvpLexemes = pOverloads->vector();
    lexeme_c* pSymbol = nullptr;

    size_t const stLexemeCount = rvpLexemes.count();

    for (size_t stLexeme = 0; stLexeme < stLexemeCount; ++stLexeme)
    {
        lexeme_c* const pOverload = rvpLexemes.val(stLexeme);
        c_lexeme_e eOverload = pOverload->lexeme();

        if (eOverload == lexeme_e::Data)
        {
            EMP_RET_FALSE_IF(stLexemeCount != 1);
            a_pNode->set_lexeme(pOverload);
            EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pOverload->qualified());)
            return true;
        }

        rti::function_c const* const pOverloadType =
            rti::to_type<rti::function_c const>(pOverload->type());


        EMP_RET_FALSE_IF(pOverloadType == nullptr);

        emp::cnt::vector<rti::type_c const*> const& rvpParameters =
            pOverloadType->parameters();

        size_t const stParameterCount = rvpParameters.count();
        size_t stArgument = 0;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            node_c const* const pLeaf = rvpLeaves.val(stIndex);
            EMP_RET_FALSE_IF(pLeaf == nullptr);

            lexeme_c const* const pLeafOp = pLeaf->lexeme();
            EMP_RET_FALSE_IF(pLeafOp == nullptr);

            rti::type_c const* const pLeafType = pLeafOp->type();
            EMP_RET_FALSE_IF(pLeafType == nullptr);

            if (pLeafType->type() == rti::type_e::Function)
            {
                rti::function_c const* const pLeafFunction =
                    rti::to_type<rti::function_c const>(pLeafType);
                EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                auto const& rvReturns = pLeafFunction->returns();
                size_t const stReturnCount = rvReturns.count();
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
            {
                if (stArgument >= stParameterCount)
                { goto NEXT_STRICT_OVERLOAD; }
                rti::type_c const* const pParamType = rvpParameters.val(stArgument);
                ++stArgument;

                EMP_RET_FALSE_IF(pParamType == nullptr);
                if (pParamType != pLeafType)
                { goto NEXT_STRICT_OVERLOAD; }
            }

            if (pOverload->lexeme() == lexeme_e::Control)
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
        a_pNode->set_lexeme(pSymbol);
        EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pSymbol->qualified());)
        return true;
    }

    for (size_t stLexeme = 0; stLexeme < stLexemeCount; ++stLexeme)
    {
        lexeme_c* const pOverload = rvpLexemes.val(stLexeme);
        rti::function_c const* const pOverloadType =
            rti::to_type<rti::function_c const>(pOverload->type());
        EMP_RET_FALSE_IF(pOverload == nullptr);
        emp::cnt::vector<rti::type_c const*> const& rvpParameters =
            pOverloadType->parameters();
        size_t const stParameterCount = rvpParameters.count();
        size_t stArgument = 0;
        for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
        {
            node_c const* const pLeaf = rvpLeaves.val(stIndex);
            EMP_RET_FALSE_IF(pLeaf == nullptr);

            rti::type_c const* const pLeafType = get_type(pLeaf->lexeme());
            EMP_RET_FALSE_IF(pLeafType == nullptr);

            if (pLeafType->type() == rti::type_e::Function)
            {
                rti::function_c const* const pLeafFunction =
                    rti::to_type<rti::function_c const>(pLeafType);
                EMP_RET_FALSE_IF(pLeafFunction == nullptr);
                auto const& rvReturns = pLeafFunction->returns();
                size_t const stReturnCount = rvReturns.count();
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
                    lexeme_c const* const pLeafLexeme = pLeaf->lexeme();
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

    EMP_RET_FALSE_IF(pSymbol == nullptr, "unresolved symbol '#0'", sName);

    rti::function_c const* const pSymbolType = rti::to_type<rti::function_c const>(pSymbol->type());
    EMP_RET_FALSE_IF(pSymbol == nullptr);
    emp::cnt::vector<rti::type_c const*> const& rvpParameters = pSymbolType->parameters();
    c_size stParameterCount = rvpParameters.count();
    size_t stArgument = 0;
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        node_c const* const pLeaf = rvpLeaves.val(stIndex);
        EMP_RET_FALSE_IF(pLeaf == nullptr);

        rti::type_c const* const pLeafType = get_type(pLeaf->lexeme());
        EMP_RET_FALSE_IF(pLeafType == nullptr);

        if (pLeafType->type() == rti::type_e::Function)
        {
            rti::function_c const* const pLeafFunction =
                rti::to_type<rti::function_c const>(pLeafType);
            EMP_RET_FALSE_IF(pLeafFunction == nullptr);
            auto const& rvReturns = pLeafFunction->returns();
            size_t const stReturnCount = rvReturns.count();
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
            EMP_RET_FALSE_IF(stArgument >= stParameterCount);
            rti::type_c const* const pParamType = rvpParameters.val(stArgument);
            ++stArgument;

            EMP_RET_FALSE_IF(pParamType == nullptr);
            if (pParamType == pLeafType)
            { continue; }
            else if (pParamType == pRealType)
            {
                lexeme_c* const pLeafLexeme = pLeaf->lexeme();
                EMP_RET_FALSE_IF(pLeafLexeme == nullptr);

#define XTMP_OSE_CAST(x_Type, x_TableType)\
                if (pLeafType == x_TableType)\
                {\
                    literal_t<x_Type>* const pLiteral = to_literal<x_Type>(pLeafLexeme);\
                    EMP_RET_FALSE_IF(pLiteral == nullptr);\
                    literal_t<real>* const pCastedLiteral = a_rTable.reg_literal(static_cast<real>(pLiteral->literal()));\
                    EMP_RET_FALSE_IF(pCastedLiteral == nullptr);\
                    node_c* const pCastedNode = fill_node(m_pCurrentProgram->add_node(), pCastedLiteral);\
                    a_pNode->leaves().set(stIndex, pCastedNode);\
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
    a_pNode->set_lexeme(pSymbol);
    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("resolved '#0'", pSymbol->qualified());)
    return true;
}

#undef XTMP_OSE_CAST
#undef XTMP_OSE_ARG
#undef XTMP_OSE_OVERLOAD

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compiler_c::build_asm(
    node_c* const a_pNode,
    program_c& a_rProgram,
    table_c& a_rTable)
{
    c_node_e eNode = a_pNode->value();
    if (eNode == node_e::Call) {}
    lexeme_c const* const pLexeme = a_pNode->lexeme();
    EMP_RET_FALSE_IF(pLexeme == nullptr);

    c_lexeme_e eLexeme = pLexeme->lexeme();
    switch (eLexeme)
    {
        case lexeme_e::Punctuation:
        {
            punctuation_c const* const pPunctuation = to_lexeme<punctuation_c const>(pLexeme);
            EMP_RET_FALSE_IF(pPunctuation == nullptr);
            c_punctuation_e ePunctuation = pPunctuation->punctuation();
            switch (ePunctuation)
            {
                case punctuation_e::LeftBrace:
                {
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));
                    auto const& rvpLeaves = a_pNode->leaves();
                    if (rvpLeaves.count() > 0)
                    { EMP_RET_FALSE_IF_NOT(build_asm(rvpLeaves, a_rProgram, a_rTable)); }
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                    return true;
                }
                case punctuation_e::RightPar:
                {
                    auto const& rvpLeaves = a_pNode->leaves();
                    size_t const stLeafCount = rvpLeaves.count();
                    EMP_RET_FALSE_IF_NOT((stLeafCount > 0));
                    size_t stAddress = 0;
                    for (size_t stLeaf = 0; stLeaf < stLeafCount; ++stLeaf)
                    {
                        node_c const* const pLeaf = rvpLeaves.val(stLeaf);
                        EMP_RET_FALSE_IF(pLeaf == nullptr);

                        lexeme_c const* const pLeafLex = pLeaf->lexeme();
                        EMP_RET_FALSE_IF(pLeafLex == nullptr);

                        lexeme_c* const pLeafLexeme = a_rTable.get_lexeme(pLeafLex->name(), pLeafLex->type());
                        data_c* const pData = to_lexeme<data_c>(pLeafLexeme);
                        if (pData != nullptr)
                        {
                            rti::data_c const* const pDataType = pData->data();
                            EMP_RET_FALSE_IF(pDataType == nullptr);

                            EMP_RET_FALSE_IF(stAddress >= emp::tt::max<ui32>::value);
                            pData->set_address(static_cast<ui32>(stAddress));
                            stAddress += pDataType->size();
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
                case node_e::Definition:
                {
                    data_c* const pData = to_lexeme<data_c>(a_rTable.get_lexeme(pLexeme->name(), pLexeme->type()));
                    EMP_RET_FALSE_IF(pData == nullptr);

                    size_t const stAddress = a_rTable.data_address();
                    EMP_RET_FALSE_IF(stAddress >= emp::tt::max<ui32>::value);
                    pData->set_address(static_cast<ui32>(stAddress));

                    rti::data_c const* const pDataType = pData->data();
                    EMP_RET_FALSE_IF(pDataType == nullptr);
                    EMP_RET_FALSE_IF_NOT(a_rTable.move_data_address(pDataType->size()));

                    auto const& rvpLeaves = a_pNode->leaves();
                    EMP_RET_FALSE_IF(rvpLeaves.count() != 1);

                    size_t const stEntryPoint = a_rProgram.exit_point();

                    EMP_RET_FALSE_IF_NOT(build_asm(rvpLeaves.ref(0), a_rProgram, a_rTable));
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_entry_point(stEntryPoint));

                    return true;
                }
                case node_e::Call:
                {
                    data_c const* const pData = to_lexeme<data_c const>(pLexeme);
                    EMP_RET_FALSE_IF(pData == nullptr);

                    rti::data_c const* const pDataType = pData->data();
                    EMP_RET_FALSE_IF(pDataType == nullptr);

                    rti::c_data_e eData = pDataType->data();

                    switch (eData)
                    {
#define XTMP_CASE(x_Data, x_Rel, x_Abs)\
                        case rti::data_e::x_Data:\
                        {\
                            EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(\
                                pData->local() ? instruction_e::x_Rel : instruction_e::x_Abs));\
                            a_rProgram.add_data<ui32>(pData->address());\
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
                }
                case node_e::none:
                default:
                {
                    EMP_LOG_ERROR("unhandled node type #0", static_cast<ui32>(eNode));
                }
            }
        }
        case lexeme_e::Control:
        {
            c_control_e eControl = get_control(pLexeme);
            switch (eControl)
            {
                case control_e::Loop:
                {
                    emp::cnt::vector<node_c*> const& rvpLeaves = a_pNode->leaves();
                    size_t const stCount = rvpLeaves.count();
                    EMP_RET_FALSE_IF(stCount < 2);

                    // push
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));

                    // index
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushSInt));
                    a_rProgram.add_data<sint>(0);

                    // jump
                    size_t const stJump = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF (stJump >= emp::tt::max<ui32>::value);
                    c_ui32 ui32Jump = static_cast<ui32>(stJump);

                    // test
                    EMP_RET_FALSE_IF_NOT(build_asm(rvpLeaves.val(0), a_rProgram, a_rTable));
                    rti::type_c const* const pType = pLexeme->type();
                    EMP_RET_FALSE_IF(pType == nullptr);
                    EMP_RET_FALSE_IF(a_rTable.get_lexeme(a_pNode->name(), pType) == nullptr);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::JumpCmp_S32));

                    // end
                    size_t const stEnd = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stEnd >= emp::tt::max<ui32>::value);
                    c_ui32 ui32End = static_cast<ui32>(stEnd);
                    a_rProgram.add_data(emp::tt::max<ui32>::value);

                    // body
                    node_c* const pBody = rvpLeaves.val(1);
                    EMP_RET_FALSE_IF(
                        pBody == nullptr ||
                        pBody->lexeme() != m_pLeftBrace ||
                        (pBody->leaves().count() > 0 &&
                        build_asm(pBody->leaves(), a_rProgram, a_rTable) == false));

                    // jump
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Jump));
                    a_rProgram.add_data(ui32Jump);

                    // break
                    size_t const stBreak = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stBreak >= emp::tt::max<ui32>::value);
                    c_ui32 ui32Break = static_cast<ui32>(stBreak);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32End, ui32Break));

                    // pop
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                    return true;
                }
                case control_e::If:
                {
                    emp::cnt::vector<node_c*> const& rvpLeaves = a_pNode->leaves();
                    size_t const stCount = rvpLeaves.count();
                    EMP_RET_FALSE_IF(stCount < 2);

                    // push
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PushScope));

                    // test
                    EMP_RET_FALSE_IF_NOT(build_asm(rvpLeaves.val(0), a_rProgram, a_rTable));
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::JumpCmp_Bool));

                    // end
                    size_t const stEnd = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stEnd >= emp::tt::max<ui32>::value);
                    c_ui32 ui32End = static_cast<ui32>(stEnd);
                    a_rProgram.add_data(emp::tt::max<ui32>::value);

                    // body
                    node_c* const pBody = rvpLeaves.val(1);
                    EMP_RET_FALSE_IF(
                        pBody == nullptr ||
                        pBody->lexeme() != m_pLeftBrace ||
                        (pBody->leaves().count() > 0 &&
                         build_asm(pBody->leaves(), a_rProgram, a_rTable) == false));

                    // break
                    size_t const stBreak = a_rProgram.exit_point();
                    EMP_RET_FALSE_IF(stBreak >= emp::tt::max<ui32>::value);
                    c_ui32 ui32Break = static_cast<ui32>(stBreak);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.set_data(ui32End, ui32Break));

                    // pop
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::PopScope));
                    return true;
                }
                case control_e::none:
                default:
                {
                    EMP_LOG_ERROR("unhandled control '#0' : #1", enum_to_astring(eControl), enum_to(eControl));
                    return false;
                }
            };
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
            EMP_RET_FALSE_IF_NOT(build_arguments(*a_pNode, a_rProgram, a_rTable));
            EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(eInstruction));
            return true;
        }
        case lexeme_e::Function:
        case lexeme_e::Variant:
        {
            switch (eNode)
            {
                case node_e::Definition:
                {
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_definition(pLexeme->index()));

                    emp::cnt::vector<node_c*> const& rvpLeaves = a_pNode->leaves();
                    size_t const stCount = rvpLeaves.count();
                    EMP_RET_FALSE_IF(stCount < 1);

                    node_c* pReturns = nullptr;
                    node_c* pParameters = nullptr;
                    node_c* pBody = nullptr;

                    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
                    {
                        node_c* const pLeaf = rvpLeaves.ref(stIndex);
                        lexeme_c const * const pLeafLexeme = pLeaf->lexeme();
                        if (pLeafLexeme == m_pLeftPar)
                        { pReturns = pLeaf; }
                        else if (pLeafLexeme == m_pRightPar)
                        { pParameters = pLeaf; }
                        else if (pLeafLexeme == m_pLeftBrace)
                        { pBody = pLeaf; }
                    }

                    if (pReturns != nullptr)
                    { EMP_RET_FALSE_IF_NOT(build_asm(pReturns, a_rProgram, a_rTable)); }

                    rti::type_c const* const pType = pLexeme->type();
                    EMP_RET_FALSE_IF(pType == nullptr);

                    rti::function_c const* const pFunctionType =rti::to_type<rti::function_c const>(pType);
                    EMP_RET_FALSE_IF(pFunctionType == nullptr);

                    size_t const stParameterSize = pFunctionType->parameter_size();
                    EMP_RET_FALSE_IF(stParameterSize >= emp::tt::max<ui32>::value);

                    if (pParameters != nullptr)
                    {
                        EMP_RET_FALSE_IF_NOT((stParameterSize > 0));
                        EMP_RET_FALSE_IF_NOT(build_asm(pParameters, a_rProgram, a_rTable));
                    }

                    EMP_RET_FALSE_IF(pBody == nullptr);
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Enter));
                    a_rProgram.add_data<ui32>(static_cast<ui32>(stParameterSize));
                    EMP_RET_FALSE_IF_NOT(build_asm(pBody, a_rProgram, a_rTable));
                    EMP_RET_FALSE_IF_NOT(a_rProgram.add_instruction(instruction_e::Return));
                    return true;
                }
                case node_e::Call:
                {
                    EMP_OSE_IF_DEBUG(EMP_LOG_INFO("#0 : '#1'",
                        eLexeme == lexeme_e::Function ? ALITERAL("Function") : ALITERAL("Variant"), a_pNode->name());)
                    EMP_RET_FALSE_IF_NOT(build_arguments(*a_pNode, a_rProgram, a_rTable));
                    if (eLexeme == lexeme_e::Function)
                    { EMP_RET_FALSE_IF_NOT(a_rProgram.add_function_call(pLexeme->index())); }
                    else if (eLexeme == lexeme_e::Variant)
                    { EMP_RET_FALSE_IF_NOT(a_rProgram.add_variant_call(pLexeme->index())); }
                    else
                    {
                        EMP_LOG_ERROR("invalid function/variant call");
                        return false;
                    }
                    return true;
                }
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

