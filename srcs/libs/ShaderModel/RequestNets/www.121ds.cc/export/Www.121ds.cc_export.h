
#ifndef WWW_121DS_CC_EXPORT_H
#define WWW_121DS_CC_EXPORT_H

#ifdef WWW_121DS_CC_STATIC_DEFINE
#  define WWW_121DS_CC_EXPORT
#  define WWW_121DS_CC_NO_EXPORT
#else
#  ifndef WWW_121DS_CC_EXPORT
#    ifdef www_121ds_cc_EXPORTS
        /* We are building this library */
#      define WWW_121DS_CC_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define WWW_121DS_CC_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef WWW_121DS_CC_NO_EXPORT
#    define WWW_121DS_CC_NO_EXPORT 
#  endif
#endif

#ifndef WWW_121DS_CC_DEPRECATED
#  define WWW_121DS_CC_DEPRECATED __declspec(deprecated)
#endif

#ifndef WWW_121DS_CC_DEPRECATED_EXPORT
#  define WWW_121DS_CC_DEPRECATED_EXPORT WWW_121DS_CC_EXPORT WWW_121DS_CC_DEPRECATED
#endif

#ifndef WWW_121DS_CC_DEPRECATED_NO_EXPORT
#  define WWW_121DS_CC_DEPRECATED_NO_EXPORT WWW_121DS_CC_NO_EXPORT WWW_121DS_CC_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef WWW_121DS_CC_NO_DEPRECATED
#    define WWW_121DS_CC_NO_DEPRECATED
#  endif
#endif

#endif /* WWW_121DS_CC_EXPORT_H */
