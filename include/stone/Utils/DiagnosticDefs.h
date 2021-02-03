// Define macros
#ifdef DEFINE_DIAGNOSTIC_MACROS

#if !(defined(DIAG) || (defined(ERROR) && defined(WARNING) && defined(NOTE) && \
                        defined(REMARK)))
#error Must define either DIAG or the set {ERROR,WARNING,NOTE,REMARK}
#endif

#ifndef ERROR
#define ERROR(ID, Options, Text, Signature) \
  DIAG(ERROR, ID, Options, Text, Signature)
#endif

#ifndef WARNING
#define WARNING(ID, Options, Text, Signature) \
  DIAG(WARNING, ID, Options, Text, Signature)
#endif

#ifndef NOTE
#define NOTE(ID, Options, Text, Signature) \
  DIAG(NOTE, ID, Options, Text, Signature)
#endif

#ifndef REMARK
#define REMARK(ID, Options, Text, Signature) \
  DIAG(REMARK, ID, Options, Text, Signature)
#endif

#ifndef FIXIT
#define FIXIT(ID, Text, Signature)
#endif

#undef DEFINE_DIAGNOSTIC_MACROS
#endif

// Undefine macros
#ifdef UNDEFINE_DIAGNOSTIC_MACROS

#ifndef DIAG_NO_UNDEF

#if defined(DIAG)
#undef DIAG
#endif

#undef REMARK
#undef NOTE
#undef WARNING
#undef ERROR
#undef FIXIT

#endif

#undef UNDEFINE_DIAGNOSTIC_MACROS
#endif
