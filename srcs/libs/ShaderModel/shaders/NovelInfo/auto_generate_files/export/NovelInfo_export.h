
#ifndef NOVELINFO_EXPORT_H
#define NOVELINFO_EXPORT_H

#ifdef NOVELINFO_STATIC_DEFINE
#  define NOVELINFO_EXPORT
#  define NOVELINFO_NO_EXPORT
#else
#  ifndef NOVELINFO_EXPORT
#    ifdef NovelInfo_EXPORTS
        /* We are building this library */
#      define NOVELINFO_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define NOVELINFO_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef NOVELINFO_NO_EXPORT
#    define NOVELINFO_NO_EXPORT 
#  endif
#endif

#ifndef NOVELINFO_DEPRECATED
#  define NOVELINFO_DEPRECATED __declspec(deprecated)
#endif

#ifndef NOVELINFO_DEPRECATED_EXPORT
#  define NOVELINFO_DEPRECATED_EXPORT NOVELINFO_EXPORT NOVELINFO_DEPRECATED
#endif

#ifndef NOVELINFO_DEPRECATED_NO_EXPORT
#  define NOVELINFO_DEPRECATED_NO_EXPORT NOVELINFO_NO_EXPORT NOVELINFO_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NOVELINFO_NO_DEPRECATED
#    define NOVELINFO_NO_DEPRECATED
#  endif
#endif

#endif /* NOVELINFO_EXPORT_H */
