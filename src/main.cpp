#include <iostream>
#include <unistd.h>


#include "AST.h"
#include "CPN.h"
#include "StateSpace.h"
#include "common.h"

const bool print_AST = true;
bool debug = true;
std::string path_ast = "error file name";
std::vector<Timer> timer;

void test_Storage(StateSpace &sp);
void test_Purchase(StateSpace& sp);
void test_Timelock(StateSpace& sp);
void test_SafeMath(StateSpace& sp);

int main(int argc, char* argv[]){
    timer.emplace_back("program begin");
    
    parse_arg(argc, argv);
    
    timer.emplace_back("before parse ast");
    
    AST ast;
    ast.parse(path_ast);
    ast.traverse(print_AST);
    ast.info();

    timer.emplace_back("ast done, next build cpn");

    CPN cpn(ast);
    cpn.build();
    cpn.info();
    cpn.draw();

    timer.emplace_back("cpn done, next state space");

    StateSpace sp(&cpn);
    if (path_ast.find("storage") != std::string::npos)
        test_Storage(sp);
    // test_Storage(sp);
    if (path_ast.find("Purchase") != std::string::npos)
        test_Purchase(sp);
    if (path_ast.find("Timelock") != std::string::npos)
        test_Timelock(sp);
    if (path_ast.find("SafeMath") != std::string::npos)
        test_SafeMath(sp);

    timer.emplace_back("state space done");
    
    
    Timer::outputTime(timer);
    VmPeak();

    return 0;
}

void test_Storage(StateSpace &sp) {
    using namespace std;
    State *s = new State();
    // 初始状态
    s->tokens[sp.cpn->getPlaceByMatch("P.init.c").name] = "1`()";
    s->tokens[sp.cpn->getPlaceByMatch("retrieve.pcall").name] = "2`(3,4,)++2`(7,8,)";
    s->tokens[sp.cpn->getPlaceByMatch("store.pcall").name] = "2`(5,)++1`(9,)";
    // s->tokens[sp.cpn->getPlaceByMatch("retrieve.pcall").name] = "2`(3,4,)++1`(7,8,)++9`(13,17,)";
    // s->tokens[sp.cpn->getPlaceByMatch("store.pcall").name] = "2`(5,)++2`(9,)++8`(19,)";
    // 变量初值
    init_DataPlace(sp.cpn, s);
    cout << s->getStr() << endl;
    sp.generate(s);

    return;
}

void test_Purchase(StateSpace &sp) {
    State* s = new State();
    // 初始状态
    s->tokens[sp.cpn->getPlaceByMatch("P.init.c").name] = "1`()";
    s->tokens[sp.cpn->getPlaceByMatch("global.seller").name] = "1`0x00AA";
    s->tokens[sp.cpn->getPlaceByMatch("global.buyer").name] = "1`0x00AB";


    init_DataPlace(sp.cpn, s);
    std::cout << s->getStr() << std::endl;
    sp.generate(s);

    return;
}

void test_Timelock(StateSpace &sp) {
    State* s = new State();
    // 初始状态

    init_DataPlace(sp.cpn, s);
    std::cout << s->getStr() << std::endl;
    sp.generate(s);
    
    return;
}

void test_SafeMath(StateSpace &sp) {
    State* s = new State();
    // 初始状态
    s->tokens[sp.cpn->getPlaceByMatch("P.init.c").name] = "1`()";
    s->tokens[sp.cpn->getPlaceByMatch("add.pcall").name] = "1`(23,29,)";
    s->tokens[sp.cpn->getPlaceByMatch("sub.pcall").name] = "1`(10,7,)";
    // s->tokens[sp.cpn->getPlaceByMatch("add.pcall").name] = "1`(23,29,)++4`(1,5,)++10`(7,123,)";
    // s->tokens[sp.cpn->getPlaceByMatch("sub.pcall").name] = "1`(7,10,)++10`(3,2,)++7`(56,55,)";

    init_DataPlace(sp.cpn, s);
    std::cout << s->getStr() << std::endl;
    sp.generate(s);
    
    return;
}