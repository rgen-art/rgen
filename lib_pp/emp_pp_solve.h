//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_PP_SOLVE_H
#define EMP_PP_SOLVE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_PP_SOLVE_IMPL(...) __VA_ARGS__
#define EMP_PP_SOLVE(x_Args) EMP_PP_SOLVE_IMPL x_Args


/*#define CAT(x, y) CAT_I(x, y)
#define CAT_I(x, y) x ## y

#define APPLY(macro, args) APPLY_I(macro, args)
#define APPLY_I(macro, args) macro args

#define STRIP_PARENS(x) EVAL((STRIP_PARENS_I x), x)
#define STRIP_PARENS_I(...) 1,1

#define EVAL(test, x) EVAL_I(test, x)
#define EVAL_I(test, x) MAYBE_STRIP_PARENS(TEST_ARITY test, x)

#define TEST_ARITY(...) APPLY(TEST_ARITY_I, (__VA_ARGS__, 2, 1))
#define TEST_ARITY_I(a,b,c,...) c

#define MAYBE_STRIP_PARENS(cond, x) MAYBE_STRIP_PARENS_I(cond, x)
#define MAYBE_STRIP_PARENS_I(cond, x) CAT(MAYBE_STRIP_PARENS_, cond)(x)

#define MAYBE_STRIP_PARENS_1(x) x
#define MAYBE_STRIP_PARENS_2(x) APPLY(MAYBE_STRIP_PARENS_2_I, x)
#define MAYBE_STRIP_PARENS_2_I(...) __VA_ARGS__

STRIP_PARENS(this is a test)
STRIP_PARENS((a,b,c)) */

#define NOTHING_EXTRACT

#define EXTRACT(...) EXTRACT __VA_ARGS__

#define PASTE(x, ...) x ## __VA_ARGS__

#define EVALUATING_PASTE(x, ...) EMP_PP_FORWARD(PASTE(x, __VA_ARGS__))

#define EMP_PP_TRY_SOLVE(...) EMP_PP_FORWARD(EVALUATING_PASTE(NOTHING_, EXTRACT __VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

