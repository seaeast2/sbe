#ifndef SBE_IR_H
#define SBE_IR_H

#include <string>
#include <vector>
#include <memory>

namespace sbe {

// IR Instruction Types
enum class IROpcode {
    LOAD,    // Load constant or variable
    STORE,   // Store to variable
    ADD,     // Addition
    SUB,     // Subtraction
    MUL,     // Multiplication
    DIV,     // Division
    RET      // Return
};

// IR Instruction
struct IRInstruction {
    IROpcode opcode;
    std::string dest;      // destination register/variable
    std::string operand1;  // first operand
    std::string operand2;  // second operand (optional)
    int immediate;         // immediate value (for LOAD)
    
    IRInstruction(IROpcode op, const std::string& d, 
                  const std::string& op1 = "", 
                  const std::string& op2 = "",
                  int imm = 0)
        : opcode(op), dest(d), operand1(op1), operand2(op2), immediate(imm) {}
};

// IR Function
struct IRFunction {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<IRInstruction> instructions;
    
    IRFunction(const std::string& fname) : name(fname) {}
    
    void addInstruction(const IRInstruction& inst) {
        instructions.push_back(inst);
    }
};

// IR Module (collection of functions)
class IRModule {
public:
    IRModule() = default;
    
    void addFunction(const IRFunction& func);
    const std::vector<IRFunction>& getFunctions() const { return functions; }
    
    // Helper to create simple instructions
    static IRInstruction createLoad(const std::string& dest, int value);
    static IRInstruction createStore(const std::string& dest, const std::string& src);
    static IRInstruction createBinaryOp(IROpcode op, const std::string& dest,
                                       const std::string& op1, const std::string& op2);
    static IRInstruction createReturn(const std::string& value);
    
private:
    std::vector<IRFunction> functions;
};

// IR Builder - helps construct IR programmatically
class IRBuilder {
public:
    IRBuilder(IRModule& module) : module(module), currentFunction(nullptr) {}
    
    void startFunction(const std::string& name);
    void endFunction();
    
    void emitLoad(const std::string& dest, int value);
    void emitAdd(const std::string& dest, const std::string& op1, const std::string& op2);
    void emitSub(const std::string& dest, const std::string& op1, const std::string& op2);
    void emitMul(const std::string& dest, const std::string& op1, const std::string& op2);
    void emitDiv(const std::string& dest, const std::string& op1, const std::string& op2);
    void emitReturn(const std::string& value);
    
private:
    IRModule& module;
    IRFunction* currentFunction;
};

} // namespace sbe

#endif // SBE_IR_H
