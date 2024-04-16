
#ifndef XMLTOOLS_EXPORT_H
#define XMLTOOLS_EXPORT_H

#ifdef XMLTOOLS_STATIC_DEFINE
#  define XMLTOOLS_EXPORT
#  define XMLTOOLS_NO_EXPORT
#else
#  ifndef XMLTOOLS_EXPORT
#    ifdef XmlTools_EXPORTS
        /* We are building this library */
#      define XMLTOOLS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define XMLTOOLS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef XMLTOOLS_NO_EXPORT
#    define XMLTOOLS_NO_EXPORT 
#  endif
#endif

#ifndef XMLTOOLS_DEPRECATED
#  define XMLTOOLS_DEPRECATED __declspec(deprecated)
#endif

#ifndef XMLTOOLS_DEPRECATED_EXPORT
#  define XMLTOOLS_DEPRECATED_EXPORT XMLTOOLS_EXPORT XMLTOOLS_DEPRECATED
#endif

#ifndef XMLTOOLS_DEPRECATED_NO_EXPORT
#  define XMLTOOLS_DEPRECATED_NO_EXPORT XMLTOOLS_NO_EXPORT XMLTOOLS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef XMLTOOLS_NO_DEPRECATED
#    define XMLTOOLS_NO_DEPRECATED
#  endif
#endif

#endif /* XMLTOOLS_EXPORT_H */
