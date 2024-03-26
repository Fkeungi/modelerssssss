#include "StateSpace.h"

#include <sstream>

using namespace std;

extern bool debug;

string State::getStr() const {
    string ret;
    for (const auto &t: tokens)
        ret += t.first + ": \t" + t.second + "\n";
    return ret;
}


size_t State::hash(const CPN*_cpn) const {
    static std::hash<string> hasher;

    string str;
    for (auto p : _cpn->places) {
        auto it = tokens.find(p.name);
        if (it != tokens.end())
            str += p.name + ":" + it->second + "\n";
    }
    size_t h_v = hasher(str);
    return h_v;
}

StateSpace::StateSpace(CPN* cpn_) {
    this->cpn = cpn_;
}

void StateSpace::generate(State *init_s) {
    queue<State *> q;
    if (states.find(init_s->hash(cpn)) == states.end())
        q.push(init_s);
    states[init_s->hash(cpn)] = init_s;

    static int state_cnt = 0;
    while (!q.empty()) {
        vector<Binding> &bindList = getBinding(q.front());
        state_cnt++;
        if (state_cnt % 10000 == 0 && !debug)
            cout << "State No." << state_cnt << " Processing]" << endl;
        else if (debug) {
            cout << "[State No." << state_cnt << " Processing]" << endl;
            cout << q.back()->getStr();
            string tmp = "  Fireable Transition: ";
            for (auto i : bindList)
                tmp += cpn->trans[i.t_idx].name + ", ";
            cout << BLUE << tmp << RESET << endl
                 << endl;
        }
        for (auto i : bindList) {
            // 生成下一个状态，并加入状态StateSpace.states
            State* next_s = getNextState(q.front(), i);
            if (next_s == nullptr) {
                delete next_s;
            } else {
                // 生成成功，则加入队列
                q.push(next_s);
                states[next_s->hash(cpn)] = next_s;
                lastState = next_s;  // 这一句是历史遗留问题，能跑就别动
            }
            // cout << "check point" << endl;
        }
        q.pop();
    }

    cout << "Total state num [" << state_cnt << "]" << endl;
}

/**
 * 根据弧表达式生成
*/
string StateSpace::tokenString(const string &exp) {
    if (exp.find("1`().")!=string ::npos) {
        return "C";
    }
    if (exp.find("replace.")!=string::npos) {
        string token;
        auto it = consume.find(cur_place);
        if (it == consume.end()) {
            cerr << "tokenString::place[" << cur_place << "] consume nothing."
                 << endl;
            exit(-1);
        }
        token = it->second;
        return token;
    }
    if (exp.find("assign.") != string::npos && cur_tran.find("Assignment.") != string::npos) {
        // assign类型只需找到read那一个不属于C类型的token即可
        cur_State->tokens[cur_place] = "";
        for (auto &it : consume)
            if (it.first.find(".c.") == string::npos)
                return it.second; // 非控制库所，返回
    }
    if (exp.find("write.")!=string::npos && cur_tran.find("Return.")!=string::npos) {
        // return
        // write类型需要清空当前库所
        for (auto &it : consume)
            if (it.first.find(".c.") == string::npos)
                return it.second; // 非控制库所，返回
    }
    if (exp.find("write.")!=string::npos && cur_tran.find("+.") != string::npos) {
        // +
        string ret;
        for (auto &it : consume)
            if (it.first.find(".c.") == string::npos)
                ret = to_string(atoi(ret.c_str()) + atoi(it.second.c_str()));
        return ret;
    }
    cerr << "tokenString::unknown arc expression : [" << exp << "], ";
    cerr << "transition name is [" << cur_tran << "]" << endl;
    exit(-1);
}

/**
 * 执行弧表达式，修改token，仅处理消耗token的部分
 */
void StateSpace::executeExp(string &tokens, const string &exp) {
    int i = 0, j = tokens.find(", ", 0);
    // i---j之间代表一个（或多个相同的）token
    while (j != string::npos) {
        size_t k = tokens.find('`', i);
        string token = tokens.substr(k + 1, j - k - 1);
        int num = atoi(tokens.substr(i, k - i).c_str());
        // cout << num << "##" << token << endl;
        if (tokenCheck(exp, token)) {
            // 选择碰到的第一个token执行
            // 消耗token
            if (num == 1) {
                tokens = tokens.substr(0, i) + tokens.substr(j);
                if (tokens.length() == 2)
                    tokens = "";
            } else {
                tokens =
                    tokens.substr(0, i) + to_string(num - 1) + tokens.substr(k);   
            }
            consume[cur_place] = token; // 记录消耗掉的token
            return;
        }
        i = j + 2;
        j = tokens.find(", ", i);
    }
}

/**
 * 根据当前状态和变迁，生成下一个状态
*/
State *StateSpace::nextState(State *s, int t) {
    cur_tran = cpn->trans[t].name;
    consume.clear();
    // 先复制，再修改token
    State *ret = new State();
    cur_State = ret;
    for (auto it : s->tokens)
        ret->tokens[it.first] = it.second;
    // 修改，消耗
    for (auto j : cpn->trans[t].pre) {
        string arc_exp =
            cpn->getArc(cpn->places[j].name, cpn->trans[t].name).name;

        cur_place = cpn->places[j].name;
        auto it = ret->tokens.find(cur_place);
        executeExp(it->second, arc_exp);
        if (it->second == "")
            ret->tokens.erase(it);
    }
    // 修改，输出
    for (auto j : cpn->trans[t].pos) {
        string arc_exp =
            cpn->getArc(cpn->trans[t].name, cpn->places[j].name).name;

        cur_place = cpn->places[j].name;
        auto it = ret->tokens.find(cur_place);
        if (it == ret->tokens.end()) 
            ret->tokens[cpn->places[j].name] = "";
        it = ret->tokens.find(cpn->places[j].name);
        // executeExp(it->second, arc_exp, true);
        if (arc_exp.find("write.")!=string::npos)
            it->second = "1`" + tokenString(arc_exp) + ", ";
        else
            it->second += "1`" + tokenString(arc_exp) + ", ";

    }
    cur_place = "error:cur_place";
    cur_tran = "error:cur_tran";
    return ret;
}

/**
 * 检查给定的token是否满足条件表达式exp
*/
bool StateSpace::tokenCheck(const string &exp, const string &t) {
    if (exp.find("1`().") != string ::npos) {
        if (t.find("C") != string::npos)
            return true;
    }
    if (exp.find("x.") != string::npos || exp.find("y.")!= string::npos) {
        // 只是读取token，一定能读取到
        // read-->x，读取弧变成变量绑定弧。y也是绑定变量
        return true;
    }
    cerr << "tokenCheck::unknown arc expression : [" << exp << "]" << endl;
    exit(-1);
}

/**
 * 判断tokens满足弧表达式
*/
bool StateSpace::satisfyExp(const string &exp, const string &tokens) {
    int i = 0, j = tokens.find(", ", 0);
    // i---j之间代表一个（或多个相同的）token
    while (j != string::npos) {
        size_t k = tokens.find('`', i);
        string token = tokens.substr(k + 1, j - k - 1);
        int num = atoi(tokens.substr(i, k - i).c_str());
        // cout << num << "##" << token << endl;
        if (tokenCheck(exp, token))
            return true;

        i = j + 2;
        j = tokens.find(", ", i);
    }
    return false;
}

/**
 * 判断变迁可发生
*/
bool StateSpace::isFireable(State *s, int t) {
    for (auto j : cpn->trans[t].pre) {
        string arc_exp =
            cpn->getArc(cpn->places[j].name, cpn->trans[t].name).name;
        
        
        auto it = s->tokens.find(cpn->places[j].name);
        if (it == s->tokens.end())
            return false;  // 没有token直接不可发生
        // cout << arc_exp << "->>";
        // cout << it->second << endl;
        if (!satisfyExp(arc_exp, it->second))
            return false;
    }
    return true;
}

vector<int> &StateSpace::getFireable(State *s) {
    static vector<int> list;
    list.clear();

    // 可能发生的变迁(前集库所中含有token)
    set<int> uncheck_trans;
    for (auto &p : s->tokens) {
        // cout << p.first << ":" << p.second << endl;
        auto &place = cpn->getPlace(p.first);
        for (auto t:place.pos)
            uncheck_trans.insert(t);
    }

    // 可以发生的变迁
    for (auto i: uncheck_trans) {
        // cout << t << ", " << cpn->trans[t].name << endl;
        if (isFireable(s, i))
            list.emplace_back(i);
    }

    return list;
}

vector<Binding> &StateSpace::getBinding(State *s) {
    static vector<Binding> list;  // 设计为static
    list.clear();

    // 可能发生的变迁(前集【控制流】库所中含有token)
    set<int> uncheck_trans;
    for (auto &p : s->tokens) {
        // cout << p.first << ":" << p.second << endl;
        auto &place = cpn->getPlace(p.first);
        if (!place.isControl)
            continue;           // 限制为，控制流
        if (p.second == "")
            continue;           // 限制为，库所非空
        for (auto t:place.pos)
            uncheck_trans.insert(t);
    }

    

    // 查找可以发生的绑定，包括变量
    // 思路：将每种可能的绑定列出来，然后查找它的可行性
    for (auto t: uncheck_trans) {
        bool check = true;
        vector<vector<string>> optional_list;
        vector<string> place_list;
        // 将每一个库所的变量所有种类都解析出来
        for (auto pre_place : cpn->trans[t].pre) {
            optional_list.emplace_back();
            MultiSet ms;
            parse_MultiSet(ms, s->tokens[cpn->places[pre_place].name]);
            // cout << cpn->places[pre_place].name << endl;
            for (auto t : ms.token)
                optional_list.back().emplace_back(t);
            place_list.emplace_back(cpn->places[pre_place].name);
        }
        // 将提取到的内容验证，并组成绑定
        if (optional_list.size() == place_list.size()) {
            vector<int> iters(place_list.size(), 0);
            checkBindings(list, optional_list, place_list, cpn->trans[t].name);
        }
    }

    return list;
}

/* 0计算成功，-1则计算失败已经到末尾 */
inline int nextIterList(vector<int>& I_, 
                        const vector<vector<string>>& O_) {
    // 执行思路：各位不同进制的加法

    I_[O_.size() - 1]++;                        // 末位+1
    for (int i = O_.size() - 1; i >= 0; i--) {  // 循环，处理进位
        if (O_[i].size() <= I_[i] && i > 0) {
            I_[i] = 0;                          // 进位
            I_[i - 1]++;
            continue;
        } else if (O_[i].size() > I_[i]) {      // 到这一位不需要继续进位
            break;
        } else if (i == 0) {
            return -1;                          // 进到最高位还需要进位
        }
    }

    return 0;
}

unordered_map<string, string> var_sattisfy;  // '检查阶段'的变量绑定情况

/* 尝试绑定变量与token，如果成功则返回1，失败则返回0 */
inline int try_bind(const string &t_, const string &v_, unordered_map<string, string>&var_map) {
    // 若不存在则直接绑定成功
    if (var_map.find(v_) == var_map.end()) {
        var_map[v_] = t_;
        return 1;
    }
    // 若存在则检查是否一致，一致也可以“绑定”成功
    if (var_map[v_] == t_)
        return 1;
    // 若存在且不一致，则视为绑定失败
    // cerr << "Variable[" << v_
    //      << "] is already bound to [" << var_map[v_]
    //      << "] and cannot be bound to [" << t_
    //      << "] again" << endl;
    return 0;
}

/**
 * 检查这个token是否满足弧表达式
 * 
 * 注意：1、此时传入参数中token已经分割出数量，此时不能处理多个token的情况
 *      2、默认库所类型是匹配弧表达式类型的，不做多余复杂检查
*/
int StateSpace::satisfyExp_singleExp(const string &token, 
                    const string &expression) {
    // 空的token直接判定为不满足
    if (token == "")
        return 0;
    // 首先去除弧表达式末尾的   '.2' 、 '.15'
    string exp = expression.substr(0, expression.find_last_of('.'));
    // 分类处理弧表达式
    if (exp == "1`()")                                      // 控制弧
        return token == "()" ? 1 : 0;
    if (exp.length() == 1 || exp.length() == 2)             // 单个变量绑定
        return try_bind(token, exp, var_sattisfy);
    if (exp[0] == '(' && exp[exp.length() - 1] == ')') {    // 交类型变量
        if (token[0] != '(' || token[token.length() - 1] != ')')
            return 0;
        int Lt = 1, Rt = token.find_first_of(',', Lt);
        int Lv = 1, Rv = exp.find_first_of(',', Lv);
        while (Rt != string::npos && Rv != string::npos) {
            string t = token.substr(Lt, Rt - Lt);
            string v = exp.substr(Lv, Rv - Lv);
            if (!try_bind(t, v, var_sattisfy))
                return 0;
            Lt = Rt + 1;
            Rt = token.find_first_of(',', Lt);
            Lv = Rv + 1;
            Rv = exp.find_first_of(',', Lv);
        }
        return 1;
    }

    cerr << "StateSpace::satisfyExp_singleExp: Unrecognized arc expression [" << expression << "]" << endl;
    exit(-1);
    return -777;
}

/**
 * 检查一个组合是否满足对应的弧表达式 
 * return: 0代表不满足，1代表满足
*/
int StateSpace::satisfyExp_group(const vector<int> &iter_list,
                    const vector<vector<string>> &optional_list,
                    const vector<string> &place_list,
                    const string &trans_name) {
    var_sattisfy.clear();
    for (int i = 0; i < place_list.size(); i++) {
        // 如果该弧的前面的库所是空的，那么直接判定为不满足
        if (optional_list[i].size() == 0)
            return 0;
        // 逐个弧检查是否满足
        string token = optional_list[i][iter_list[i]];
        string arc_exp = cpn->getArc(place_list[i], trans_name).name;

        if (!satisfyExp_singleExp(token, arc_exp))
            return 0;
    }
    return 1;
}

/**
 * 检查token是否可以构成一个绑定，如果可以则加入list
 * 预设前提：弧表达式之间的变量不存在公用关系（例：不存在一个变迁两个弧同时使用x变量）
*/
int StateSpace::checkBindings(vector<Binding> &list,
                    const vector<vector<string>> &optional_list,
                    const vector<string> &place_list,
                    const string &trans_name) {
    vector<int> iter_list(place_list.size(), 0);
    while (iter_list[0] < place_list.size()) {
        // 验证该组合是否满足弧表达式
        if (satisfyExp_group(iter_list, optional_list, place_list, trans_name)) {
            // 如果满足，则加入list
            Binding& b = list.emplace_back();
            b.t_idx = cpn->getIdxTransition(trans_name);
            for (int i = 0; i < iter_list.size(); i++) {
                b.place.emplace_back(place_list[i]);
                b.token.emplace_back(optional_list[i][iter_list[i]]);
            }
        }

        if (nextIterList(iter_list, optional_list)==-1)
            // 所有组合尝试完毕，可以退出
            return 0;
    }
    return -1;
}

int init_DataPlace(CPN *cpn, State *s) {
    for (auto p: cpn->places) {
        if (p.isControl) 
            continue;  // 控制流库所不需要初始化
        if (s->tokens[p.name].size() != 0)
            continue;  // 已经手动初始化的库所不再重复初始化

        if (p.color == "uint256")
            s->tokens[p.name] = "1`0";
    }
    return 0;
}

int parse_MultiSet(MultiSet &ms, const std::string &s) {
    // 已知规定，用++链接，全数字形式
    stringstream ss(s);
    int n;
    string token;
    char ch = '`';

    while (ss.peek()!=EOF) {
        ss >> n >> ch;                  // 提取数量与字符'`'
        ms.num.emplace_back(n);

        std::getline(ss, token, '+');   // 提取token名称
        ms.token.emplace_back(token);

        ss >> ch;                 // 再读取'++'
    }

    return 0;
}

unordered_map<string, string> var_NextState;  // '生成下一个状态阶段'的变量绑定情况

/**
 * 获取下一个状态，基于cur_和绑定b_生成
 * 如果生成成功则返回指针，如果该状态已存在则返回nullptr
*/
State *StateSpace::getNextState(State *cur_, Binding& b_) {
    State* s = new State();
    var_NextState.clear();
    // 操作顺序：复制、删除消耗掉的token、写入新增的token、删去空库所key值、检查新状态合法性
    // 复制
    for (auto p : cur_->tokens)
        s->tokens[p.first] = p.second;
    // 删除消耗掉的token，并且将消耗的token与弧表达式绑定
    for (int i = 0; i < b_.place.size();i++) {
        removeToken(s->tokens[b_.place[i]], b_.token[i]);
        bindVar(b_.place[i], b_.t_idx, b_.token[i]);
    }
    // 写入新增的token
    for (int i = 0; i < cpn->trans[b_.t_idx].pos.size();i++) {
        string place_name = cpn->places[cpn->trans[b_.t_idx].pos[i]].name;
        addToken(s->tokens[place_name], b_.t_idx, place_name);
    }
    // 删去空库所key值
    vector<string> del;
    for (auto p : s->tokens) 
        if (p.second == "")
            del.emplace_back(p.first);
    for (auto p : del)
        s->tokens.erase(p);
    // 检查新增状态的合法性
    auto H = s->hash(cpn);
    if (states.find(H) != states.end()) {
        repeat++;
        if (debug)
            cout << "Repeat state, ignored.   Total repeat [" << repeat << "]" << endl;
        return nullptr;
    }

    return s;
}

// 检查是否有对应的绑定变量
inline void checkExpInVar(const string &e_) {
    auto it = var_NextState.find(e_);
    if (it==var_NextState.end()) {
        cerr << "StateSpace::addToken: checkExpInVar failed, unknown var["
             << e_ << "]" << endl;
        exit(-1);
    }
}

// 判断是否为二元操作, +-*/
inline bool isBinaryOp(const string &e_) {
    if (e_.find('+') != string::npos || e_.find('-') != string::npos 
        || e_.find('*') != string::npos || e_.find('/') != string::npos)
        return true;
    return false;
}

// 解析二元运算表达式，并且按照运算规则得出结果，修改ms
inline void calcExp_Bin(MultiSet &ms, const string &exp) {
    /* 目前二元运算只接受 ‘+’ ‘-’ ‘*’ ‘/’ 这四种，【也仅针对数值】 */
    int i = 0;
    while (exp[i] != '+' && exp[i] != '-' && exp[i] != '*' && exp[i] != '/')
        i++;
    char op = exp[i];

    string opL = exp.substr(0, i);  // 左操作数
    string opR = exp.substr(i + 1); // 右操作数
    int opL_i = atoi(var_NextState[opL].c_str()), opR_i = atoi(var_NextState[opR].c_str());
    string res;
    if (op == '+')
        res = to_string(opL_i + opR_i);
    else if (op == '-')
        res = to_string(opL_i - opR_i);
    else if (op == '*')
        res = to_string(opL_i * opR_i);
    else if (op == '/')
        res = to_string(opL_i / opR_i);

    ms.add(res);
}

/**
 * 在库所（字符串all_）中新增token，根据弧（由变迁t_idx_指向库所p_）表达式新增
*/
int StateSpace::addToken(std::string& all_, const int t_idx_, const std::string& p_) {
    Arc& a = cpn->getArc(cpn->trans[t_idx_].name, p_);
    string exp = a.name.substr(0, a.name.find_last_of('.'));

    MultiSet ms;
    parse_MultiSet(ms, all_);

    if (exp == "1`()"){                             // 控制弧，输出控制流token
        ms.add("()");
        all_ = ms.str();
        return 0;
    }
    if (exp.length() == 1 || exp.length() == 2) {   // 变量，直接从绑定集合找到并输出
        checkExpInVar(exp);
        ms.add(var_NextState[exp]);
        all_ = ms.str();
        return 0;
    }
    if (isBinaryOp(exp)) {                          // 二元运算，根据绑定进行运算
        calcExp_Bin(ms, exp);
        all_ = ms.str();
        return 0;
    }

    cerr << "StateSpace::addToken: Unrecognized arc expression [" << exp << "]" << endl;
    exit(-1);
    return -777;
}

/**
 * 将‘生成阶段’的变量与token绑定记录
 * p_库所名、t_idx_变迁下标、t_是token的字符串
*/
int StateSpace::bindVar(const std::string& p_, const int t_idx_, const std::string& t_) {
    /*  在这个阶段不需要考虑绑定是否会失败，因为在之前生成可发
    生变迁组合的时候已经验证过一次了 */
    Arc& a = cpn->getArc(p_, cpn->trans[t_idx_].name);
    string exp = a.name.substr(0, a.name.find_last_of('.'));
    // 分类绑定的具体操作
    //     注意到这个函数的实现与StateSpace::satisfyExp_singleExp非常
    // 相似，但是有区别，这里的返回值虽然有处理，但是并无实际使用
    if (exp == "1`()")
        return 1;
    if (exp.length() == 1 || exp.length()==2)
        return try_bind(t_, exp, var_NextState);
    if (exp[0] == '(' && exp[exp.length() - 1] == ')') {    
        if (t_[0] != '(' || t_[t_.length() - 1] != ')') {
            cerr << "StateSpace::bindVar: Type mismatch for arc expression[" << exp << "] and token[" << t_ << "]" << endl;
            exit(-1);
        }
        int Lt = 1, Rt = t_.find_first_of(',', Lt);
        int Lv = 1, Rv = exp.find_first_of(',', Lv);
        while (Rt != string::npos && Rv != string::npos) {
            string t = t_.substr(Lt, Rt - Lt);
            string v = exp.substr(Lv, Rv - Lv);
            if (!try_bind(t, v, var_NextState)) {
                cerr << "StateSpace::bindVar: Try to bind var[" << v << "] and token-component[" << t << "] failed, var is [" << var_NextState[v] << "]" << endl;
                exit(-1);
            }
            Lt = Rt + 1;
            Rt = t_.find_first_of(',', Lt);
            Lv = Rv + 1;
            Rv = exp.find_first_of(',', Lv);
        }
        return 1;
    }

    return 0;
}

// 从给定的所有token中删掉指定的token，若删除失败则直接报错并退出程序
int StateSpace::removeToken(std::string &all_, const std::string &t_) {
    MultiSet ms;
    parse_MultiSet(ms, all_);

    if (ms.sub(t_) == -1) {
        cerr << "Failed to remove token [" << t_ << "] from multi-set : \n\t" << all_ << endl;
        exit(-1);
    }

    ms.sort();
    all_ = ms.str();

    return 0;
}

/* 向Multi-set中新增指定数量（n_缺省1）的某个token（参数t_） */
int MultiSet::add(const std::string& t_, const int n_) {
    // 新增一定会成功，类型不匹配问题不在此处解决
    // 该函数修改了ms的内容，所以返回之前要sort一次
    for (int i = 0; i < token.size();i++) {
        if (token[i]==t_) {
            num[i] += n_;
            this->sort();
            return 0;
        }
    }
    token.emplace_back(t_);
    num.emplace_back(n_);
    this->sort();
    return 0;
}

/* 从Multi-set中删去指定数量（n_缺省1）的某个token（参数t_），返回值为-1表示失败 */
int MultiSet::sub(const std::string& t_, const int n_) {
    int i = 0;
    bool success = false;
    for (i = 0; i < num.size(); i++) {
        if (token[i] == t_) {
            num[i] -= n_;
            if (num[i] == 0) {
                num.erase(num.begin() + i);
                token.erase(token.begin() + i);
            }
            success = true;
            break;
        }
    }
    if (success == false)
        return -1;
    this->sort();
    return 0;
}

/* 生成Multi-set对应的字符串形式的token，生成过程注意排序保持唯一性 */
string MultiSet::str() {
    string ret;
    for (int i = 0; i < num.size();i++) {
        ret += to_string(num[i]) + '`' + token[i];
        if (i < num.size() - 1)
            ret += "++";
    }
    return ret;
}

/* 对Multi-set中的元素进行排序（升序），冒泡排序，字符串比较 */
void MultiSet::sort() {
    for (int i = 0; i < num.size(); i++)
        for (int j = i + 1; j < num.size(); j++) {
            if (strcmp(token[i].c_str(), token[j].c_str())>0){
                int tmp_i = num[i];
                num[i] = num[j];
                num[j] = tmp_i;

                string tmp_s = token[i];
                token[i] = token[j];
                token[j] = tmp_s;
            }
        }
}