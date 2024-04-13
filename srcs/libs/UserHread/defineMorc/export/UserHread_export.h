
#ifndef USERHREAD_EXPORT_H
#define USERHREAD_EXPORT_H

#ifdef USERHREAD_STATIC_DEFINE
#  define USERHREAD_EXPORT
#  define USERHREAD_NO_EXPORT
#else
#  ifndef USERHREAD_EXPORT
#    ifdef UserHread_EXPORTS
        /* We are building this library */
#      define USERHREAD_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define USERHREAD_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef USERHREAD_NO_EXPORT
#    define USERHREAD_NO_EXPORT 
#  endif
#endif

#ifndef USERHREAD_DEPRECATED
#  define USERHREAD_DEPRECATED __declspec(deprecated)
#endif

#ifndef USERHREAD_DEPRECATED_EXPORT
#  define USERHREAD_DEPRECATED_EXPORT USERHREAD_EXPORT USERHREAD_DEPRECATED
#endif

#ifndef USERHREAD_DEPRECATED_NO_EXPORT
#  define USERHREAD_DEPRECATED_NO_EXPORT USERHREAD_NO_EXPORT USERHREAD_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef USERHREAD_NO_DEPRECATED
#    define USERHREAD_NO_DEPRECATED
#  endif
#endif

#endif /* USERHREAD_EXPORT_H */
