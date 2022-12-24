#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

// Function::~Function()
// {
//     auto delete_list = block_list;
//     for (auto &i : delete_list)
//         delete i;
//     parent->removeFunc(this);
// }

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    
    // 先传函数参数
    // std::cout << "params.size() = " << params.size() << std::endl;
    for(unsigned long int i = 0; i < params.size(); i++) {
        // std::cout << "传入函数参数列表" << std::endl;
        if(i == params.size() - 1) {
            fprintf(yyout, "%s %s", params[i]->getType()->toStr().c_str(), params[i]->toStr().c_str());
        }
        else {
            fprintf(yyout, "%s %s, ", params[i]->getType()->toStr().c_str(), params[i]->toStr().c_str());
        }
    }
    fprintf(yyout, ") {\n");

    // 再执行函数中的内容的output
    std::set<BasicBlock *> v;  // set容器拒绝重复的元素，用于存储已经访问过的BasicBlock
    std::list<BasicBlock *> q; // 双向链表，用于存储未被访问过的BasicBlock
    q.push_back(entry); // 尾部插入
    v.insert(entry);
    while (!q.empty()) {
        auto bb = q.front();
        q.pop_front(); // 头部弹出
        bb->output();
        // std::cout << "进入了Function的while循环，bb->getNo() = " << bb->getNo() << std::endl;

        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++) {
            // std::cout << "进入了Function的for循环" << std::endl;
            if (v.find(*succ) == v.end()) {
                // 如果在v中找不到该succ，说明succ还没有被访问过，那就标记为访问过(v.insert(*succ))，并加入到待访问list中
                v.insert(*succ);
                q.push_back(*succ);
                // std::cout << "Function的if判断完毕" << std::endl;
            }
        }
    }
    fprintf(yyout, "}\n");
}
