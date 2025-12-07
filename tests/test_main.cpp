#include "ir.h"
#include "codegen.h"
#include <iostream>
#include <cassert>

using namespace sbe;

void test_ir_creation() {
    std::cout << "Testing IR creation... ";
    
    IRModule module;
    IRBuilder builder(module);
    
    builder.startFunction("test_func");
    builder.emitLoad("t1", 42);
    builder.emitReturn("t1");
    builder.endFunction();
    
    assert(module.getFunctions().size() == 1);
    assert(module.getFunctions()[0].name == "test_func");
    assert(module.getFunctions()[0].instructions.size() == 2);
    
    std::cout << "PASSED\n";
}

void test_ir_arithmetic() {
    std::cout << "Testing IR arithmetic operations... ";
    
    IRModule module;
    IRBuilder builder(module);
    
    builder.startFunction("arithmetic");
    builder.emitLoad("a", 10);
    builder.emitLoad("b", 20);
    builder.emitAdd("c", "a", "b");
    builder.emitSub("d", "c", "a");
    builder.emitMul("e", "d", "b");
    builder.emitReturn("e");
    builder.endFunction();
    
    const auto& func = module.getFunctions()[0];
    assert(func.instructions.size() == 6);
    assert(func.instructions[0].opcode == IROpcode::LOAD);
    assert(func.instructions[2].opcode == IROpcode::ADD);
    assert(func.instructions[3].opcode == IROpcode::SUB);
    assert(func.instructions[4].opcode == IROpcode::MUL);
    assert(func.instructions[5].opcode == IROpcode::RET);
    
    std::cout << "PASSED\n";
}

void test_codegen() {
    std::cout << "Testing code generation... ";
    
    IRModule module;
    IRBuilder builder(module);
    
    builder.startFunction("main");
    builder.emitLoad("t1", 5);
    builder.emitLoad("t2", 3);
    builder.emitAdd("t3", "t1", "t2");
    builder.emitReturn("t3");
    builder.endFunction();
    
    CodeGenerator codegen;
    std::string assembly = codegen.generate(module);
    
    // Check that assembly contains expected elements
    assert(assembly.find(".intel_syntax noprefix") != std::string::npos);
    assert(assembly.find(".globl main") != std::string::npos);
    assert(assembly.find("push rbp") != std::string::npos);
    assert(assembly.find("pop rbp") != std::string::npos);
    assert(assembly.find("ret") != std::string::npos);
    
    std::cout << "PASSED\n";
}

void test_multiple_functions() {
    std::cout << "Testing multiple functions... ";
    
    IRModule module;
    IRBuilder builder(module);
    
    builder.startFunction("func1");
    builder.emitLoad("t1", 10);
    builder.emitReturn("t1");
    builder.endFunction();
    
    builder.startFunction("func2");
    builder.emitLoad("t1", 20);
    builder.emitReturn("t1");
    builder.endFunction();
    
    assert(module.getFunctions().size() == 2);
    assert(module.getFunctions()[0].name == "func1");
    assert(module.getFunctions()[1].name == "func2");
    
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "Running SBE Tests\n";
    std::cout << "=================\n\n";
    
    test_ir_creation();
    test_ir_arithmetic();
    test_codegen();
    test_multiple_functions();
    
    std::cout << "\nAll tests PASSED!\n";
    return 0;
}
