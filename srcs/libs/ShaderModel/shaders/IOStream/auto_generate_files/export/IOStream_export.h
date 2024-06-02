
#ifndef IOSTREAM_EXPORT_H
#define IOSTREAM_EXPORT_H

#ifdef IOSTREAM_STATIC_DEFINE
#  define IOSTREAM_EXPORT
#  define IOSTREAM_NO_EXPORT
#else
#  ifndef IOSTREAM_EXPORT
#    ifdef IOStream_EXPORTS
        /* We are building this library */
#      define IOSTREAM_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define IOSTREAM_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef IOSTREAM_NO_EXPORT
#    define IOSTREAM_NO_EXPORT 
#  endif
#endif

#ifndef IOSTREAM_DEPRECATED
#  define IOSTREAM_DEPRECATED __declspec(deprecated)
#endif

#ifndef IOSTREAM_DEPRECATED_EXPORT
#  define IOSTREAM_DEPRECATED_EXPORT IOSTREAM_EXPORT IOSTREAM_DEPRECATED
#endif

#ifndef IOSTREAM_DEPRECATED_NO_EXPORT
#  define IOSTREAM_DEPRECATED_NO_EXPORT IOSTREAM_NO_EXPORT IOSTREAM_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef IOSTREAM_NO_DEPRECATED
#    define IOSTREAM_NO_DEPRECATED
#  endif
#endif

#endif /* IOSTREAM_EXPORT_H */
