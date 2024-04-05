
#ifndef NOVEINFO_EXPORT_H
#define NOVEINFO_EXPORT_H

#ifdef NOVEINFO_STATIC_DEFINE
#  define NOVEINFO_EXPORT
#  define NOVEINFO_NO_EXPORT
#else
#  ifndef NOVEINFO_EXPORT
#    ifdef NoveInfo_EXPORTS
        /* We are building this library */
#      define NOVEINFO_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define NOVEINFO_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef NOVEINFO_NO_EXPORT
#    define NOVEINFO_NO_EXPORT 
#  endif
#endif

#ifndef NOVEINFO_DEPRECATED
#  define NOVEINFO_DEPRECATED __declspec(deprecated)
#endif

#ifndef NOVEINFO_DEPRECATED_EXPORT
#  define NOVEINFO_DEPRECATED_EXPORT NOVEINFO_EXPORT NOVEINFO_DEPRECATED
#endif

#ifndef NOVEINFO_DEPRECATED_NO_EXPORT
#  define NOVEINFO_DEPRECATED_NO_EXPORT NOVEINFO_NO_EXPORT NOVEINFO_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NOVEINFO_NO_DEPRECATED
#    define NOVEINFO_NO_DEPRECATED
#  endif
#endif

#endif /* NOVEINFO_EXPORT_H */
