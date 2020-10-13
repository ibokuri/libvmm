//
// exceptions.hpp - Exception-related macros
//

#pragma once

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(VMM_NOEXCEPTION)
    #define VMM_THROW(exception) throw exception
    #define VMM_TRY try
    #define VMM_CATCH(exception) catch(exception)
#else
    #include <cstdlib>
    #define VMM_THROW(exception) std::abort()
    #define VMM_TRY if(true)
    #define VMM_CATCH(exception) if(false)
#endif
