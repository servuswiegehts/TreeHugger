#ifndef __DEBG__
#define __DEBG__

// --- ogl.g - global space ---------------------------------------

#ifndef GL_ARB_debug_output

       #define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB               0x8242

       #define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB               0x9143

       #define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB              0x9144

       #define GL_DEBUG_LOGGED_MESSAGES_ARB                  0x9145

       #define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB       0x8243

       #define GL_DEBUG_CALLBACK_FUNCTION_ARB                0x8244

       #define GL_DEBUG_CALLBACK_USER_PARAM_ARB              0x8245

       #define GL_DEBUG_SOURCE_API_ARB                       0x8246

       #define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB             0x8247

       #define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB           0x8248

       #define GL_DEBUG_SOURCE_THIRD_PARTY_ARB               0x8249

       #define GL_DEBUG_SOURCE_APPLICATION_ARB               0x824A

       #define GL_DEBUG_SOURCE_OTHER_ARB                     0x824B

       #define GL_DEBUG_TYPE_ERROR_ARB                       0x824C

       #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB         0x824D

       #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB          0x824E

       #define GL_DEBUG_TYPE_PORTABILITY_ARB                 0x824F

       #define GL_DEBUG_TYPE_PERFORMANCE_ARB                 0x8250

       #define GL_DEBUG_TYPE_OTHER_ARB                       0x8251

       #define GL_DEBUG_SEVERITY_HIGH_ARB                    0x9146

       #define GL_DEBUG_SEVERITY_MEDIUM_ARB                  0x9147

       #define GL_DEBUG_SEVERITY_LOW_ARB                     0x9148

 

typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLARBPROC) (unsigned int source, unsigned int type,

unsigned int severity, int count, const unsigned int* ids, bool enabled);

typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTARBPROC) (unsigned int source, unsigned int type,

unsigned int id, unsigned int severity, int length, const char* buf);

typedef void (APIENTRY *GLDEBUGPROCARB)(unsigned int source, unsigned int type, unsigned int id,

unsigned int severity, int length, const char* message, void* userParam);

typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKARBPROC) (GLDEBUGPROCARB callback,

void* userParam);

typedef unsigned int (APIENTRYP PFNGLGETDEBUGMESSAGELOGARBPROC) (unsigned int count, int bufsize,

unsigned int* sources,unsigned int* types, unsigned int* ids,

unsigned int* severities, int* lengths, char* messageLog);

#endif

 

extern PFNGLDEBUGMESSAGECONTROLARBPROC  glDebugMessageControlARB;

extern PFNGLDEBUGMESSAGEINSERTARBPROC   glDebugMessageInsertARB;

extern PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB;

extern PFNGLGETDEBUGMESSAGELOGARBPROC   glGetDebugMessageLogARB;

 

 

// --- ogl.cpp - global space -------------------------------------

PFNGLDEBUGMESSAGECONTROLARBPROC   glDebugMessageControlARB   = NULL;

PFNGLDEBUGMESSAGEINSERTARBPROC    glDebugMessageInsertARB    = NULL;

PFNGLDEBUGMESSAGECALLBACKARBPROC  glDebugMessageCallbackARB  = NULL;

PFNGLGETDEBUGMESSAGELOGARBPROC    glGetDebugMessageLogARB    = NULL;

 

 

// --- GLRenderer.cpp - CGLRenderer::InitAPI()  -------------------------------------

glDebugMessageControlARB   = (PFNGLDEBUGMESSAGECONTROLARBPROC)

wglGetProcAddress("glDebugMessageControlARB");

glDebugMessageInsertARB    = (PFNGLDEBUGMESSAGEINSERTARBPROC)

wglGetProcAddress("glDebugMessageInsertARB");

glDebugMessageCallbackARB  = (PFNGLDEBUGMESSAGECALLBACKARBPROC)

wglGetProcAddress("glDebugMessageCallbackARB");

glGetDebugMessageLogARB    = (PFNGLGETDEBUGMESSAGELOGARBPROC)

wglGetProcAddress("glGetDebugMessageLogARB");


#endif