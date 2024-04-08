
#ifndef TOOLS_EXPORT_H
#define TOOLS_EXPORT_H

#ifdef TOOLS_STATIC_DEFINE
#  define TOOLS_EXPORT
#  define TOOLS_NO_EXPORT
#else
#  ifndef TOOLS_EXPORT
#    ifdef Tools_EXPORTS
        /* We are building this library */
#      define TOOLS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define TOOLS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef TOOLS_NO_EXPORT
#    define TOOLS_NO_EXPORT 
#  endif
#endif

#ifndef TOOLS_DEPRECATED
#  define TOOLS_DEPRECATED __declspec(deprecated)
#endif

#ifndef TOOLS_DEPRECATED_EXPORT
#  define TOOLS_DEPRECATED_EXPORT TOOLS_EXPORT TOOLS_DEPRECATED
#endif

#ifndef TOOLS_DEPRECATED_NO_EXPORT
#  define TOOLS_DEPRECATED_NO_EXPORT TOOLS_NO_EXPORT TOOLS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TOOLS_NO_DEPRECATED
#    define TOOLS_NO_DEPRECATED
#  endif
#endif

#endif /* TOOLS_EXPORT_H */
