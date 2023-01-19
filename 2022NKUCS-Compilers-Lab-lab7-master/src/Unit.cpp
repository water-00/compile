#include "Unit.h"

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}


void Unit::genMachineCode(MachineUnit* munit) 
{
    // std::cout << "out Function::genMachineCode\n";
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    for (auto &func : func_list)
        func->genMachineCode(builder);
    // std::cout << "out Function::genMachineCode\n";
}

Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}


