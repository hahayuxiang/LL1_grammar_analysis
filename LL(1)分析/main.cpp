/*输入是可以含有直接左递归的文法*/
#include <iostream>
#include <algorithm>
#include <cstring>
#include <queue>
#include <stack>
#include <set>
#include <map>

const int N = 100;

//存文法规则
struct Grammar
{
	char vn = 'A';
	int cnt = 0;
	std::string vt[10];
}grammat[N];
//存FIRST集合
struct First
{
	std::string st = "";
	int cnt = 0;
	std::set<char> ed;
}first[N];
//存FOLLOW集合
struct Follow
{
	char st = 'A';
	int cnt = 0;
	std::set<char> ed;
}follow[N];

int h[N], e[N], ne[N], idx_all;//用来建FOLLOW集合依赖图
int a[N][N];//根据第i条规则的第j个判断当前的first集合在结构体数组中的位置
int num = 0, num_1 = 0, n = 0;//num是规则语句的数量，num_1是first集合的数量
std::set<char> Vt, Vn, V; //Vt终结符号， Vn非终结符号， V所有符号
std::map<std::string, int> mp1;//根据字符串查询以该字符串求出的first集合在结构体数组中的位置，和a[N][N]应该重复定义，可以处理后转换
std::map<char, int> mp2, mp3;//终结符号的查询，非终结符号的查询
std::vector<std::vector<std::string>> table(N, std::vector<std::string>(N, "")); //LL_table表
std::string ss[N]; //消除左递归后产生的新规则



void add(int a, int b) {
	e[idx_all] = b, ne[idx_all] = h[a], h[a] = idx_all++;
}

bool dfs_first(char t, int idx_i, int idx_j) {
	int x = a[idx_i][idx_j];
	if (Vt.count(t)) {		
		first[x].ed.insert(t); 
		return false;
	}
	else {
		bool flag = false;
		for (int i = 0; i < num; i++) {
			char vn = grammat[i].vn;
			int cnt = grammat[i].cnt;
			if (vn == t) {
				for (int j = 0; j < cnt; j++) {
					char tt = grammat[i].vt[j][0];
					if (tt == '!') {
						flag = true;
					}
					else {
						dfs_first(tt, idx_i, idx_j);
					}
				}
				break;
			}
		}
		return flag;
	}
}
void print1(std::stack<char> s) {
	std::string ans;
	while (s.size()) {
		ans += s.top();
		s.pop();
	}
	reverse(ans.begin(), ans.end());
	std::cout << ans;
}
void print2(std::stack<char> s) {
	std::string ans;
	while (s.size()) {
		ans += s.top();
		s.pop();
	}
	std::cout << ans;
}
void input_new_rules() {
	memset(h, -1, sizeof h);	
	for (int i = 0; i < n; i ++ )  {
		std::string s = ss[i];
		int st = 0, ed = 0;
		for (int i = 0; i < s.size(); i++) {
			if (s[i] == '-' && s[i + 1] == '>') {
				grammat[num].vn = s[i - 1];
				Vn.insert(s[i - 1]);
				st = i + 1;
				i = i + 1;
			}
			else {
				if (s[i] == '|' || i == s.size() - 1) {
					if (s[i] == '|') {
						ed = i - 1;
					}
					else ed = (int)s.size();
					std::string tmp = s.substr(st + 1, ed - st);
					int cntt = grammat[num].cnt;
					grammat[num].vt[cntt] = tmp;
					grammat[num].cnt++;
					if (i == s.size() - 1) V.insert(s[i]);
					else {
						st = i;
					}
				}
				else V.insert(s[i]);
			}
		}
		num++;
	}
	for (auto t : V) {
		if (!Vn.count(t)) Vt.insert(t);
	}
}
void solve_first() {
	//求规则右部字符串的FIRST集合
	for (int i = 0; i < num; i++) {
		int cnt = grammat[i].cnt;
		for (int j = 0; j < cnt; j++) {
			std::string right = grammat[i].vt[j];
			mp1[right] = num_1;
			a[i][j] = num_1;
			first[num_1].st = right;
			for (int k = 0; k < right.size(); k++) {
				if (!dfs_first(right[k], i, j)) break;
			}
			num_1++;
		}
	}

	//根据右部求规则左部字符的FIRST集合
	for (int i = 0; i < num; i++) {
		int cnt = grammat[i].cnt;

		first[num_1].st = grammat[i].vn;
		mp1[first[num_1].st] = num_1;

		for (int j = 0; j < cnt; j++) {
			std::string right = grammat[i].vt[j];
			int backup = mp1[right];
			std::set<char> tmp = first[backup].ed;
			first[num_1].ed.insert(tmp.begin(), tmp.end());
		}
		num_1++;
	}
}
void solve_follow() {
	//求各个非终结符号的FOLLOW集合，若是依赖FIRST集合，通过查询加入其中；若是依赖其他FOLLOW集合，连一条边，由被依赖点指向需求点
	for (int i = 0; i < num; i++) {
		char t = grammat[i].vn;
		follow[i].st = t;
		mp3[t] = i;
		//文法开始符号添加#
		if (!i) {
			follow[i].ed.insert('#');
			follow[i].cnt++;
		}
		//枚举所有规则
		for (int j = 0; j < num; j++) {
			int cntt = grammat[j].cnt;
			for (int k = 0; k < cntt; k++) {
				std::string right = grammat[j].vt[k];
				int idxx = (int)right.find(t);
				//规则中包含该字符
				if (idxx != -1) {
					//该字符不在最后
					if (idxx != right.size() - 1) {
						//如果是终结符，则直接添加
						if (Vt.count(right[idxx + 1])) {
							follow[i].ed.insert(right[idxx + 1]);
							follow[i].cnt++;
						}
						//非终结符，添加其FIRST集合
						else {
							std::string hh = std::string(1, right[idxx + 1]);
							int idx_x = mp1[hh];
							std::set<char> set_tmp = first[idx_x].ed;
							follow[i].ed.insert(set_tmp.begin(), set_tmp.end());
							//如果FIRST集合中由空，则有依赖的FOLLOW集合，添加依赖边，并且删除集合中的空符号
							if (set_tmp.count('!')) {
								follow[i].ed.erase('!');
								add(j, i);//i是当前分析的非终结符， j是当前规则
							}
						}
					}
					//否则存在依赖的FOLLOW集合，添加边
					else {
						add(j, i);
					}
				}
			}
		}
	}

	//通过bfs求依赖图的最终收敛结果
	std::queue<int> q;
	//把所有的点添加进队列
	for (int i = 0; i < num; i++) {
		q.push(i);
	}
	//只要队列不空，取出队头
	while (q.size()) {
		int t = q.front();
		q.pop();
		//遍历当前点依赖的点，将所依赖的集合并入自己的集合
		for (int i = h[t]; i != -1; i = ne[i]) {
			int j = e[i];
			std::set<char> check_backup = follow[j].ed;
			follow[j].ed.insert(follow[t].ed.begin(), follow[t].ed.end());
			//如果并入后与之前没改变，表示当前点已经收敛，不需要添加进队列；反之加进队列
			if (follow[j].ed != check_backup) {
				q.push(j);
			}
		}
	}
}
void output_first_and_follow() {
	std::cout << "FIRST集合： \n";
	for (int i = 0; i < num_1; i++) {
		std::cout << "FIRST(" << first[i].st << "):  { ";
		for (auto t : first[i].ed) std::cout << t << ", ";
		std::cout << "}\n";
	}
	std::cout << "***********************************\n";

	std::cout << "FOLLOW集合：\n";
	for (int i = 0; i < num; i++) {
		std::cout << "FOLLOW(" << follow[i].st << "):  { ";
		for (auto t : follow[i].ed) std::cout << t << ", ";
		std::cout << "}\n";
	}
	std::cout << "***********************************\n";

}
void output_LL_table() {
	//备份终结符号表，删除空符号，加入#
	std::set<char> Vtt = Vt;
	Vtt.insert('#');
	Vtt.erase('!');
	int h = 0;
	//终结字符的下标
	for (auto t : Vtt) {
		mp2[t] = h;
		h++;
	}

	//初始化table表
	int n = (int)Vn.size();
	int m = (int)Vtt.size();

	//枚举每一个规则， 求规则右部的FIRST集合，在table表相应位置填上规则
	//若FIRST集合包含空符号，求规则左部的FOLLOW集合，在table表上相应位置填上规则
	for (int i = 0; i < num; i++) {
		int cnt = grammat[i].cnt;
		char left = grammat[i].vn;
		for (int j = 0; j < cnt; j++) {
			std::string right = grammat[i].vt[j];
			int idxx = mp1[right];
			std::set<char> tmp = first[idxx].ed;
			for (auto t : tmp) {
				int x = i;
				int y = mp2[t];
				table[x][y] = right;
			}
			if (tmp.count('!')) {
				tmp = follow[mp3[left]].ed;
				for (auto t : tmp) {
					int x = i;
					int y = mp2[t];
					table[x][y] = right;
				}
			}
		}
	}
	//输出LL_table表
	std::cout << "LL(1)分析表： \n";
	for (auto t : Vtt) std::cout << "\t " << t;
	std::cout << "\n";
	for (int i = 0; i < n; i++) {
		std::cout << grammat[i].vn << "\t";
		for (int j = 0; j < m; j++) {
			if (table[i][j] != "") std::cout << grammat[i].vn << "::=" << table[i][j] << "\t";
			else std::cout << table[i][j] << "\t";
		}
		std::cout << "\n";
	}
	std::cout << "***********************************\n";

}

void quiry() {
	std::string quiry_s;
	std::cout << "请输入需要判断的字符串： \n";
	std::cin >> quiry_s;
	std::cout << "***********************************\n分析过程如下\n";

	//初始化分析栈begin和余留输入栈end
	std::stack<char> begin, end;
	begin.push('#');
	begin.push(grammat[0].vn);
	end.push('#');
	for (int i = (int)quiry_s.size() - 1; i >= 0; i--) {
		end.push(quiry_s[i]);
	}
	//当匹配失败或者匹配完成结束
	bool error_flag = true;
	while (error_flag && end.size()) {
		//取出两个栈的栈顶元素并打印
		char a = begin.top();
		char b = end.top();
		print1(begin);
		std::cout << "\t";
		print2(end);
		std::cout << "\t";
		begin.pop();

		//栈顶元素不相等
		if (a != b) {
			//取出栈顶元素对应的下标，访问table
			int x = mp3[a];
			int y = mp2[b];
			//如果table为空，表示出错，置出错表示并中断
			if (table[x][y] == "") {
				error_flag = false;
				break;
			}
			//否则表示分析栈栈顶元素可以替换为表中字符串，若字符串不为空，则从右向左加入栈中；字符串为空，不处理
			else {
				std::string table_t = table[x][y];
				std::cout << table_t << "\n";
				if (table_t == "!") continue;
				for (int i = (int)table_t.size() - 1; i >= 0; i--) {
					begin.push(table_t[i]);
				}
			}
		}
		//如果栈顶元素相等，则表示余留输入栈可以消除一个元素，余留输入栈弹出栈顶元素
		else {
			std::cout << "\n";
			end.pop();
		}
	}
	std::cout << "***********************************\n";
	if (error_flag) std::cout << "success! 该句子是文法所定义的句子！\n";
	else std::cout << "fail! 该句子不是文法所定义的句子！\n";
	std::cout << "***********************************\n";

}
void solve_left_recurrence(){
	std::cout << "请输入文法规则的数量 n ：";
	std::cin >> n;
	int cnt = 0;
	while (cnt < n) {
		std::cout << "请输入第 " << cnt + 1 << "条规则： \n";
		std::cin >> ss[cnt];		
		cnt++;
	}
	std::cout << "***********************************\n";
	for (int i = 0; i < n; i++) {
		std::string s = ss[i];
		for (int j = 0; j < s.size(); j++) {
			if (s[j] == '-' && s[j + 1] == '>') {
				Vn.insert(s[j - 1]);				
				j = j + 1;
			}
			else {
				if (s[j] != '|') {
					V.insert(s[j]);
				}
			}
		}
	}
	for (auto t : V) {
		if (!Vn.count(t)) Vt.insert(t);
	}
	int num = cnt;
	for (int i = 0; i < num; i++) {
		std::string s = ss[i];
		char vn = 'A';
		int st = 0, ed = 0;
		std::set<std::string> mid1, mid2;
		bool flag = false;
		for (int j = 0; j < (int)s.size(); j++) {
			if (s[j] == '-' && s[j + 1] == '>') {
				vn = s[j - 1];
				st = j + 1;
				j = j + 1;

			}
			else {
				if (s[j] == '|' || j == s.size() - 1) {
					if (s[j] == '|') {
						ed = j - 1;
					}
					else ed = (int)s.size();
					std::string tmp = s.substr(st + 1, ed - st);
					if (tmp[0] == vn) {
						flag = true;
						mid1.insert(tmp.substr(1));
					}
					else {
						mid2.insert(tmp);
					}
					if (j != s.size() - 1) st = j;
					
				}
			}
		}
		if (flag) {
			n += 1;
			char tmpp = 'A';
			bool flagg = false;
			for (char op = 'A'; op <= 'Z'; op++) {
				if (!V.count(op)) {
					tmpp = op;
					V.insert(op);
					flagg = true;
					break;
				}
			}
			if (flag == false) {
				std::cout << "符号不够用！\n";
				exit(0);
			}
			else {
				std::string new_rule1 = "";
				for (auto t : mid2) {
					if (new_rule1 == "") new_rule1 = std::string(1, vn) + "->" + t + std::string(1, tmpp);
					else {
						new_rule1 = new_rule1 + "|" + t + std::string(1, tmpp);
					}
				}
				ss[i] = new_rule1;
				std::string new_rule2 = "";
				for (auto t : mid1) {
					if (new_rule2 == "") new_rule2 = std::string(1, tmpp) + "->" + t + std::string(1, tmpp);
					else {
						new_rule2 = new_rule2 + "|" + t + std::string(1, tmpp);
					}
				}
				new_rule2 += "|!";
				ss[cnt++] = new_rule2;
			}
		}
	}
	std::cout << "经过消除直接左递归后的新文法规则： \n";
	for (int i = 0; i < n; i++) {
		std::cout << ss[i] << "\n";
	}
	std::cout << "***********************************\n";

}
int main()
{
	//处理左递归
	solve_left_recurrence();
	//输入处理
	input_new_rules();
	//求FIRST集合
	solve_first();
	//求FOLLOW集合
	solve_follow();
	//输出FIRST集合和FOLLOW集合
	output_first_and_follow();
	//输出LL_table表
	output_LL_table();
	//查询输出语句是否为该LL(1)文法
	quiry();	
	//结束
	return 0;
}