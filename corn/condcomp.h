#ifndef condcompH
#define condcompH
#ifdef __cplusplus
/*170221 170221
# define _HALF_COMMENT_(xxx) /##xxx
# define _COMMENT_  _HALF_COMMENT_(/)

# ifdef DEBUG
#   define DBG0
# else
#   define DBG0   _COMMENT_
# endif
# ifdef DEBUG1
#   define DBG1
# else
#   define DBG1   _COMMENT_
# endif
# ifdef DEBUG2
#   define DBG2
# else
#   define DBG2   _COMMENT_
# endif

# ifdef DETAIL_TRACE
#   define TRACE
# else
#   define TRACE   _COMMENT_
# endif
*/
/*150329 obsolete
#ifdef APPTYPE_CONSOLE
#  define COUTSAFE
#  ifdef _Windows
#     define EASYWIN
#  else
#     define EASYWIN _COMMENT_
#  endif
#else
#  define COUTSAFE _COMMENT_
#  define EASYWIN _COMMENT_
#endif
*/
/*170221 obsolete
#if  defined(__STDC__) || defined(__cplusplus)
#  define STDC
#  define KRC  _COMMENT_
#else
#  define STDC _COMMENT_
#  define KRC
#endif
*/

#endif
#endif
//condcomp
