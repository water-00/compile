#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>
#include "SymbolTable.h"//增
class Type
{
private:
    int kind;
protected:
public:
    enum {INT, VOID, BOOL, FUNC, PTR, STRING,ARRAY};
    int size;//增
    //Type(int kind) : kind(kind) {};
    Type(int kind, int size = 0) : kind(kind), size(size){};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isBool() const {return kind == BOOL;};
    bool isPtr() const {return kind == PTR;};//增加判断类型为PTR的指令
    bool isArray() const { return kind == ARRAY; };//增
    void setKind(int kind) {this->kind = kind;};
    bool isString() const { return kind == STRING; };//增加 string
    int getSize() const { return size; };//增
};

class IntType : public Type
{
private:
    bool constant;//增加
    
public:
    //int size;
    IntType(int size, bool constant = false)
        : Type(Type::INT, size), constant(constant){};
    std::string toStr();
    bool isConst() const { return constant; };
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class BoolType : public Type
{
public:
    BoolType() : Type(Type::BOOL){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
    std::vector<SymbolEntry*> paramsSe;//增
public:
    FunctionType(Type* returnType,
                 std::vector<Type*> paramsType,
                 std::vector<SymbolEntry*> paramsSe)
        : Type(Type::FUNC),
          returnType(returnType),
          paramsType(paramsType),
          paramsSe(paramsSe)
          {};
        
    FunctionType(Type* returnType,
                 std::vector<Type*> paramsType)
        : Type(Type::FUNC),
          returnType(returnType),
          paramsType(paramsType)
          {};
          
    void setParamsType(std::vector<Type*> paramsType) {
        this->paramsType = paramsType;
    };
    std::vector<Type*> getParamsType() { return paramsType; };
    std::vector<SymbolEntry*> getParamsSe() { return paramsSe; };
    Type* getRetType() { return returnType; };
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
};

#endif
