
#ifndef PATH_TOOL_EXPORT_H
#define PATH_TOOL_EXPORT_H

#ifdef PATH_TOOL_STATIC_DEFINE
#  define PATH_TOOL_EXPORT
#  define PATH_TOOL_NO_EXPORT
#else
#  ifndef PATH_TOOL_EXPORT
#    ifdef Path_Tool_EXPORTS
        /* We are building this library */
#      define PATH_TOOL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define PATH_TOOL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef PATH_TOOL_NO_EXPORT
#    define PATH_TOOL_NO_EXPORT 
#  endif
#endif

#ifndef PATH_TOOL_DEPRECATED
#  define PATH_TOOL_DEPRECATED __declspec(deprecated)
#endif

#ifndef PATH_TOOL_DEPRECATED_EXPORT
#  define PATH_TOOL_DEPRECATED_EXPORT PATH_TOOL_EXPORT PATH_TOOL_DEPRECATED
#endif

#ifndef PATH_TOOL_DEPRECATED_NO_EXPORT
#  define PATH_TOOL_DEPRECATED_NO_EXPORT PATH_TOOL_NO_EXPORT PATH_TOOL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PATH_TOOL_NO_DEPRECATED
#    define PATH_TOOL_NO_DEPRECATED
#  endif
#endif

#endif /* PATH_TOOL_EXPORT_H */
