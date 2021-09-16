#include<iostream>
#include<fstream>
#include<sstream>
#include<list>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
using namespace std;

class Sensitive//敏感词类
{
public:
    string w;
    int num;
    int l;//utf-8码长度
    int k;//1为汉字，0为英文
}sensitive[105];
int sensitive_sum;

class Row//文章的行类
{
public:
    string s;
    int num;
    int l;
}row[1005];
int row_sum;//行总数

class ANS//答案类
{
public:
    int mgc_num;
    int hang_num;
    int h,e;//可疑文本的行内范围
}Ans[3005];
int Ans_num;//答案条数

void search(Sensitive &M,Row &H)//输入对应的敏感词和行，不破坏行和词本身，检测到敏感字段即存入答案类
{
    int i = 0, j = 0, head,sum;
    if (M.k == 1)//敏感词为汉字
    {
        //连续3个字节相同，则进行匹配；若敏感词长度为n字节，则需匹配n/3次才算完成
        for (; j < H.l; j++) if (H.s[j] == M.w[i]&& H.s[j+1] == M.w[i+1]&& H.s[j+2] == M.w[i+2])
        {
            if (i == 0) head = j;//第一次匹配时，对文章对应位置定位
            i += 3;
            j += 2;
            if (i == M.l)//匹配完成，记录信息，从当前位置继续查找该敏感词
            {
                i = 0;
                Ans[Ans_num].mgc_num = M.num;
                Ans[Ans_num].hang_num = H.num;
                Ans[Ans_num].h = head;
                Ans[Ans_num++].e = j;
            }
            sum+=i;
        }
    }
    else//敏感词为英文
    {
        //不区分大小写，相同则匹配；若敏感单词长度为m字节，则匹配m次
        for (; j < H.l; j++) if ((H.s[j] == M.w[i]) || (H.s[j] - M.w[i] == 32 && M.w[i] <= 'Z') || (M.w[i] - H.s[j] == 32 && M.w[i] >= 'a'))
        {
            if (i == 0) head = j;
            i++;
            if (i == M.l)//匹配完成，记录信息，从当前位置继续查找该敏感词
            {
                i = 0;
                Ans[Ans_num].mgc_num = M.num;
                Ans[Ans_num].hang_num = H.num;
                Ans[Ans_num].h = head;
                Ans[Ans_num++].e = j;
            }
            sum+=i; 
        }
    }
}

void ANS_print()//答案输出到指定路径
{   
    int sum; 
    string dz;
    cin >> dz;
    ofstream f3(dz);
    if (!f3) cout << "open error!" << endl;
    f3 << "Total: " << Ans_num << endl;
    for (int i = 0; i < Ans_num; i++)
    {
        f3 << "Line" << Ans[i].hang_num + 1 << ": " << '<' << sensitive[Ans[i].mgc_num].w << "> ";
        for (int j = Ans[i].h; j <= Ans[i].e; j++) f3.put( row[Ans[i].hang_num].s[j]);
        f3 << endl;
        sum+=i;
    }
	f3.close();
}

int main()
{
    system("chcp 65001");//将控制台设置为utf-8编码
    int i = 0, j = 0;
    Ans_num = 0;
    string dz;//地址：临时存放文件的路径
    string str;//临时存放读入的词或句

    //读入敏感词文件f1，并放入敏感词类的数组（sensitive[]）中
    cin >> dz;
    ifstream f1(dz);
    if (!f1) cout << "open error!" << endl;
    while (f1 >> str)//逐词读入
    {
        sensitive[i].w = str;
        sensitive[i].k = sensitive[i].w[0] > 0 ? 0 : 1;
        sensitive[i].num = i;
        sensitive[i++].l = str.length();
    }
    sensitive_sum = i;
    f1.close();

    //读入文本文件f2，并放入文章的行类的数组（row[]）中
    cin >> dz;
    ifstream f2(dz);
    if (!f2) cout << "open error!" << endl;
    while (getline(f2,str))//逐行读入
    {
        row[j].s = str;
        row[j].num = j;
        row[j++].l = str.length();
    }
    row_sum = j;
    f2.close();

    //行-词遍历，得到结果放入答案类数组（Ans[]）中
    for (j = 0; j < row_sum; j++) for (i = 0; i < sensitive_sum; i++) search(sensitive[i], row[j]);

    //将结果输出到答案文件f3
    ANS_print();
    return 0;
}

