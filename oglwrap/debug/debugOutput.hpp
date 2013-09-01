/** @file debugOutput.hpp
    @brief Implements ARB_Debug_Output
*/

#ifndef DEBUGOUTPUT_HPP
#define DEBUGOUTPUT_HPP

namespace oglwrap {

// Logically these should go into error.hpp, but this file
// needs this declarations, and error.hpp includes this file.
#if OGLWRAP_DEBUG
/// A conditional assert that only does anything if OGLWRAP_DEBUG is defined.
#define oglwrap_Assert(x) assert(x)
/// A global variable storing the last OpenGL error.
/** An instance of this is defined per file. */
static GLenum OGLWRAP_LAST_ERROR = GL_NO_ERROR;
#else
/// A conditional assert that only does anything if OGLWRAP_DEBUG is defined
#define oglwrap_Assert(x)
#endif

// Actual start of this file
#if OGLWRAP_DEBUG
#if OGLWRAP_USE_ARB_DEBUG_OUTPUT
/// A server side debug utility that helps letting you know what went wrong.
/** It requires a debug context, which for example SFML can't create. But
    if your window loader supports it, you should definitely use this
    when you are debugging, it is really useful. */
class DebugOutput {
private:
    /// The debug callback function
    static void DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                          const GLchar* message, GLvoid* userParam) {
        std::string srcName;
        switch(source) {
            case GL_DEBUG_SOURCE_API_ARB:
                srcName = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
                srcName = "Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
                srcName = "Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
                srcName = "Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION_ARB:
                srcName = "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER_ARB:
                srcName = "Other";
                break;
        }

        std::string errorType;
        switch(type) {
            case GL_DEBUG_TYPE_ERROR_ARB:
                errorType = "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
                errorType = "Deprecated Functionality";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
                errorType = "Undefined Behavior";
                break;
            case GL_DEBUG_TYPE_PORTABILITY_ARB:
                errorType = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE_ARB:
                errorType = "Performance";
                break;
            case GL_DEBUG_TYPE_OTHER_ARB:
                errorType = "Other";
                break;
        }

        std::string typeSeverity;
        switch(severity) {
            case GL_DEBUG_SEVERITY_HIGH_ARB:
                typeSeverity = "High";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM_ARB:
                typeSeverity = "Medium";
                break;
            case GL_DEBUG_SEVERITY_LOW_ARB:
                typeSeverity = "Low";
                break;
        }

        std::cerr << errorType << " from " << srcName <<
                  ",\t" << typeSeverity << " priority" << std::endl;
        std::cerr << "Message: " << message << std::endl;
    }
public:

    /// Activates the debug output
    /// @see glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB, glDebugMessageCallbackARB
    static void Activate() {
        if(GLEW_ARB_debug_output) {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glDebugMessageCallbackARB(DebugFunc, nullptr);
        }
    }

    /// Deactivates the debug output
    /// @see glDisable, GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB, glDebugMessageCallbackARB
    static void Deactivate() {
        if(GLEW_ARB_debug_output) {
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glDebugMessageCallbackARB(nullptr, nullptr);
        }
    }
};
#else // OGLWRAP_USE_ARB_DEBUG_OUTPUT

#define oglwrap_getFileName() __FILE__

/// The own debug output.
class DebugOutput {

    enum glError_t {
        INVALID_ENUM,
        INVALID_VALUE,
        INVALID_OPERATION,
        STACK_OVERFLOW,
        STACK_UNDERFLOW,
        OUT_OF_MEMORY,
        INVALID_FRAMEBUFFER_OPERATION,
        NUM_ERRORS
    };

    const char* glErrorNames[NUM_ERRORS] = {
        "GL_INVALID_ENUM",
        "GL_INVALID_VALUE",
        "GL_INVALID_OPERATION",
        "GL_STACK_OVERFLOW",
        "GL_STACK_UNDERFLOW",
        "GL_OUT_OF_MEMORY",
        "GL_INVALID_FRAMEBUFFER_OPERATION"
    };

    struct ErrorInfo {
        std::string funcSignature;
        std::string errors[NUM_ERRORS];

        ErrorInfo() {}
        ErrorInfo(const std::string& funcS, const std::string errs[]) : funcSignature(funcS) {
            for(int i = 0; i < NUM_ERRORS; i++) {
                errors[i] = errs[i];
            }
        }
    };

    std::map<std::string, ErrorInfo> errorMap;

    glError_t getErrorIndex() const {
        switch(OGLWRAP_LAST_ERROR) {
            case GL_INVALID_ENUM:
                return INVALID_ENUM;
            case GL_INVALID_VALUE:
                return INVALID_VALUE;
            case GL_INVALID_OPERATION:
                return INVALID_OPERATION;
            case GL_STACK_OVERFLOW:
                return STACK_OVERFLOW;
            case GL_STACK_UNDERFLOW:
                return STACK_UNDERFLOW;
            case GL_OUT_OF_MEMORY:
                return OUT_OF_MEMORY;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return INVALID_FRAMEBUFFER_OPERATION;
            default:
                return NUM_ERRORS;
        }
    }

public:
    DebugOutput() {
        using namespace std;

        // The GLerrors.txt should be in the same folder as this file.
        // So we can use the __FILE__ macro to get the path to this file,
        // and replaces the "debugOutput.hpp" to "GLerrors.txt"
        std::string filename(oglwrap_getFileName());
        auto directoryPath = filename.find("debugOutput.hpp");
        assert(directoryPath != string::npos); // Maybe it got renamed?
        filename.erase(directoryPath, string::npos);
        filename.append("GLerrors.txt");

        ifstream is(filename);
        if(!is.good()){
            std::cerr <<
                "Couldn't initialize DebugOutput because GLerrors.txt is missing or corrupted." << std::endl;
        }

        // Read until EOF, or until an error occurs.
        while(is.good()) {
            string func, funcSignature, errors[NUM_ERRORS];
            string buffer, buffer2;

            // Get the first not empty row, containing the function's name
            while(getline(is, func) && func.empty());

            // Get lines until we find we ending with );
            while(getline(is, buffer)) {
                while(isspace(buffer[buffer.size() - 1])) {
                    buffer.pop_back();
                }
                funcSignature += "    " + buffer + '\n';
                if(buffer[buffer.size() - 1] == ';' && buffer[buffer.size() - 2] == ')') {
                    break;
                }
            }

            // Get the error messages
            while(getline(is, buffer) && !buffer.empty()) {

                stringstream strstream(buffer);
                strstream >> buffer2;
                for(int i = 0; i < NUM_ERRORS; i++) {
                    if(buffer2 == glErrorNames[i]) {

                        // Make the error string a bit nicer
                        string errIsGendIf(buffer2 + " is generated if ");
                        if(buffer.find(errIsGendIf) == 0) {
                            buffer.erase(0, errIsGendIf.size() - 2);
                        } else {
                            string errMayBeGendIf(buffer2 + " may be generated if ");
                            if(buffer.find(errMayBeGendIf) == 0) {
                                buffer.erase(0, errMayBeGendIf.size() - 2);
                            }
                        }
                        buffer[0] = '-';
                        buffer[1] = ' ';
                        buffer[2] = toupper(buffer[2]);

                        errors[i] += buffer + '\n';
                        break;
                    }
                }
            }

            if(errorMap.find(func) == errorMap.end()) {
                errorMap.insert(std::pair<std::string, ErrorInfo>(
                    func, ErrorInfo(funcSignature, errors)
                ));
            }
        }
    }

    void PrintError(const std::string& functionCall) {
        using namespace std;

        size_t errIdx = getErrorIndex();
        if(errIdx == NUM_ERRORS)
            return;

        size_t funcNameLen = functionCall.find_first_of('(');
        string funcName = string(functionCall.begin(), functionCall.begin() + funcNameLen);

        if(errorMap.find(funcName) != errorMap.end() && !errorMap[funcName].errors[errIdx].empty()) {
            ErrorInfo errinfo = errorMap[funcName];
            cerr << "The following OpenGL function: " << endl << endl;
            cerr << errinfo.funcSignature << endl;
            cerr << "Has generated the error because one of the following(s) were true:" << endl;
            cerr << errinfo.errors[errIdx] << std::endl;
        }
    }
};

#endif

#else // !OGLWRAP_DEBUG

class DebugOutput {
public:
    static void Activate() {}
    static void Deactivate() {}
    void PrintError(const std::string&) {}
};

#endif // OGLWRAP_DEBUG

} // namespace oglwrap

#endif