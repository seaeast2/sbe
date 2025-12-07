#ifndef SBE_CODEGEN_H
#define SBE_CODEGEN_H

#include "ir.h"
#include <string>
#include <sstream>
#include <map>

namespace sbe {

// Simple x86-64 assembly code generator
class CodeGenerator {
public:
    CodeGenerator() = default;
    
    // Generate x86-64 assembly from IR module
    std::string generate(const IRModule& module);
    
private:
    std::stringstream output;
    std::map<std::string, int> variableOffsets;
    int stackOffset;
    
    void generateFunction(const IRFunction& func);
    void generateInstruction(const IRInstruction& inst);
    
    // Helper methods
    void emitPrologue(const IRFunction& func);
    void emitEpilogue();
    int getStackOffset(const std::string& var);
    
    void emit(const std::string& line);
    
    // Constants
    static constexpr int STACK_FRAME_SIZE = 64;
};

} // namespace sbe

#endif // SBE_CODEGEN_H
