
#ifndef PLUG_EXPORT_H
#define PLUG_EXPORT_H

#ifdef PLUG_STATIC_DEFINE
#  define PLUG_EXPORT
#  define PLUG_NO_EXPORT
#else
#  ifndef PLUG_EXPORT
#    ifdef Plug_EXPORTS
        /* We are building this library */
#      define PLUG_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define PLUG_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef PLUG_NO_EXPORT
#    define PLUG_NO_EXPORT 
#  endif
#endif

#ifndef PLUG_DEPRECATED
#  define PLUG_DEPRECATED __declspec(deprecated)
#endif

#ifndef PLUG_DEPRECATED_EXPORT
#  define PLUG_DEPRECATED_EXPORT PLUG_EXPORT PLUG_DEPRECATED
#endif

#ifndef PLUG_DEPRECATED_NO_EXPORT
#  define PLUG_DEPRECATED_NO_EXPORT PLUG_NO_EXPORT PLUG_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PLUG_NO_DEPRECATED
#    define PLUG_NO_DEPRECATED
#  endif
#endif

#endif /* PLUG_EXPORT_H */
