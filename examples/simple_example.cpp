#include "ir.h"
#include "codegen.h"
#include <iostream>

using namespace sbe;

int main() {
    // Create an IR module
    IRModule module;
    IRBuilder builder(module);
    
    // Build a simple function: int calculate() { return (10 + 20) * 3; }
    builder.startFunction("calculate");
    builder.emitLoad("t1", 10);        // t1 = 10
    builder.emitLoad("t2", 20);        // t2 = 20
    builder.emitAdd("t3", "t1", "t2"); // t3 = t1 + t2 (30)
    builder.emitLoad("t4", 3);         // t4 = 3
    builder.emitMul("t5", "t3", "t4"); // t5 = t3 * t4 (90)
    builder.emitReturn("t5");          // return t5
    builder.endFunction();
    
    // Build another function: int compute() { return (100 - 10) / 2; }
    builder.startFunction("compute");
    builder.emitLoad("t1", 100);       // t1 = 100
    builder.emitLoad("t2", 10);        // t2 = 10
    builder.emitSub("t3", "t1", "t2"); // t3 = t1 - t2 (90)
    builder.emitLoad("t4", 2);         // t4 = 2
    builder.emitDiv("t5", "t3", "t4"); // t5 = t3 / t4 (45)
    builder.emitReturn("t5");          // return t5
    builder.endFunction();
    
    // Generate x86-64 assembly code
    CodeGenerator codegen;
    std::string assembly = codegen.generate(module);
    
    // Print the generated assembly
    std::cout << "Generated Assembly Code:\n";
    std::cout << "========================\n";
    std::cout << assembly;
    std::cout << "========================\n";
    
    // Print IR summary
    std::cout << "\nIR Summary:\n";
    std::cout << "===========\n";
    for (const auto& func : module.getFunctions()) {
        std::cout << "Function: " << func.name << "\n";
        std::cout << "  Instructions: " << func.instructions.size() << "\n";
    }
    
    return 0;
}
