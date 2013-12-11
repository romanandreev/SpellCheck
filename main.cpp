#include<algorithm>
#include<vector>
#include<string>
#include<cstdio>
#include<iostream>
#include<set>
#include<map>
#include<fstream>

using namespace std;

bool isLetter(char c) {       
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
char toLowerCase(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 'a'; 
    }
    return c;
}
string toLowerCase(string s) {
    string t = "";
    for (int i = 0; i < (int)s.size(); i++) {
        t += toLowerCase(s[i]);
    }
    return t;
}

const int alp = 'z' - 'a' + 1;
set<string> dictionary[alp];
set<string> rdictionary[alp];
void addWord(string s) {
    dictionary[(int)(s[0] - 'a')].insert(s);
    rdictionary[(int)(s[(int)s.length() - 1] - 'a')].insert(s);
}
void createDictionary(string fileName) {
    ifstream fin(fileName.c_str());
    string s;
    while (fin>>s) {
        string t;
        for (int i = 0; i < (int)s.length(); i++) {
            if (isLetter(s[i])) {
                t += toLowerCase(s[i]);
            } else {
                if ((int)t.length() > 0) {
                    addWord(t);
                    t = "";
                }
            }
        }
        if ((int)t.length() > 0) {
            addWord(t);
            t = "";
        }
    }    
}
int hammingDistance(string a, string b) {
    int n = a.size();
    int m = b.size();
    int inf = n + m;
    vector<int> dp1(n + 1), dp2(n + 1);
    for (int j = 0; j <= n; j++) {
        dp1[j] = j;
    }
    for (int i = 1; i <= m; i++) {        
        for (int j = 0; j <= n; j++) {
            dp2[j] = min(min(j > 0 ? dp2[j - 1] + 1 : inf, dp1[j] + 1),  (i > 0 && j > 0) ? (dp1[j - 1] + (int)(a[j - 1] != b[i - 1])) : inf);
        }
        dp1 = dp2;
    }
    return dp1[n];
}
string createOutName(string s) {
    size_t p = s.find(".");
    if (p == string::npos) {
        return s + "_fixed";
    } else {
        return s.substr(0, p) + "_fixed" + s.substr(p, s.length());
    }
}
bool isUpper(char c) {
    return c >= 'A' && c <= 'Z'; 
}
string makeCase(string o, string e) {
    int cntU = 0;
    int cntL = 0;
    bool firstU = isUpper(o[0]);
    for (int i = 0; i < (int)o.length(); i++) {
        if (isUpper(o[i])) {
            cntU++;
        } else {
            cntL++;
        }
    }
    if (cntU == 0) {
        return e;
    }
    if (cntU == 1 && firstU) {
        e[0] = e[0] - 'a' + 'A';
        return e;
    }
    if (cntL == 0) {
        for (int i = 0; i < (int)e.size(); i++) {
            e[i] = e[i] - 'a' + 'A';
        }
        return e;
    }
    return e;
}
map<string, string> go;
string processWord(string s0) {
    string s = toLowerCase(s0);    
    int first = s[0] - 'a';
    int end = s[(int)s.length() - 1] - 'a';
    if (dictionary[first].find(s) != dictionary[first].end()) {
        return s0;
    }
    if (go.count(s)) {
        return makeCase(s0, go[s]);
    }
    cout<<"Can't find word \""<<s<<"\", choose one of the following variants:"<<endl;
    vector<string> variants;
    variants.push_back(s);
    set<pair<int, string> > best;
    for (set<string>::iterator it = dictionary[first].begin(); it != dictionary[first].end(); ++it) {
        best.insert(make_pair(hammingDistance(s, *it), *it));
        while ((int)best.size() > 9) {
            best.erase(*best.rbegin());
        }
    }
    for (set<string>::iterator it = rdictionary[end].begin(); it != rdictionary[end].end(); ++it) {
        best.insert(make_pair(hammingDistance(s, *it), *it));
        while ((int)best.size() > 9) {
            best.erase(*best.rbegin());
        }
    }
    for (set<pair<int, string> >::iterator it = best.begin(); it != best.end(); ++it) {
        if (it->first <= 4) {
            variants.push_back(it->second);
        }
    }
    for (int i = 0; i < (int)variants.size(); i++) {
        cout<<i + 1<<") "<<variants[i]<<endl;
    } 
    string res;
    cout<<(int)variants.size() + 1<<") Enter your variant"<<endl;
    while (1) {
        int x;
        cin>>x;
        if (x >= 1 && x <= (int)variants.size()) {
            res = variants[x - 1];
            break;
        }
        if (x == (int)variants.size() + 1) {
            cin>>res;
            break;
        }
        cout<<"Incorrect, try again"<<endl;
    }
    addWord(res);
    go[s] = res;
    return makeCase(s0, res);
} 
void process(string IN, string OUT) {
    ifstream fin(IN.c_str());
    ofstream fout(OUT.c_str());
    string sr;
    while (getline(fin, sr)) {
        string s;
        for (int i = 0; i < (int)sr.length(); i++) {
            char c = sr[i];           
            if (isLetter(c)) {
                s += c;
            } else {
                if ((int)s.length() > 0) {
                    fout<<processWord(s);
                    s = "";
                }
                fout<<c;
            }
        }
        if ((int)s.length() > 0) {
            fout<<processWord(s);
            s = "";
        }
        fout<<endl;
    }
}
int main() {
    cout<<"Input dictionary file name:"<<endl;
    string dictName;
    cin>>dictName;
    createDictionary(dictName);
/*    for (int i = 0; i < alp; i++) {
        for (set<string>::iterator it = dictionary[i].begin(); it != dictionary[i].end(); ++it) {
            cerr<<*it<<endl;
        }
    }       */
    cout<<"Input error text file name:"<<endl;
    string inName, outName;
    cin>>inName;
    outName = createOutName(inName);
    process(inName, outName);
    return 0;
}