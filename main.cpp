//
// Created by madalin on 4/18/2020.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <type_traits> // orderedset
#include <algorithm> // sort
#include <queue>
#include <sstream>
#include <utility> // pair
using namespace std;

struct translatie
{
    int x, y;
    char ch;
    friend bool operator==(translatie a, translatie b)
    {
        return (a.x == b.x && a.y == b.y && a.ch == b.ch);
    }
};

bool order_in_my_set(char a, char b)
{
    if(a<b && a!='$')
        return true;
    else
    if(b=='$' && a > b)
        return true;
    else return false;
}
using MyOrderedSet = integral_constant<decltype(&order_in_my_set), &order_in_my_set>;
class Automat
{
private:
    int n, m, k, l, q0;
    vector<int> st_f;
    vector<int> st;
    vector<translatie> transl;
    vector<bool> sters;
    set<char, MyOrderedSet> alph;
    bool compara_equal(set<int> a, set<int> b)
    {
        if(a.size() != b.size())
            return false;
        auto p = a.begin();
        auto q = b.begin();
        while(p != a.end())
        {
            if (*p != *q)
                return false;
            p++;
            q++;
        }
        return true;
    }
    string reuniune_stari_componente(const vector<translatie> &transl, string &stare, char alph)
    {
        stringstream s_aux;
        set<int> vizitat;
        for(char& st : stare)
        {
            s_aux.clear();
            for(auto tr : transl)
            {
                if(tr.x == (st-'0') && tr.ch == alph && vizitat.count(tr.y) == 0)
                {
                    s_aux << tr.y;
                    vizitat.insert(tr.y);
                }
            }
        }
        return s_aux.str();
    }
    bool verifica_stari_componente_st_f(const string &stare, const vector<int> &st_f)
    {
        for(char c : stare)
        {
            if(count(st_f.begin(),st_f.end(),(c-'0')) > 0)
                return true;
        }
        return false;
    }
    int get_new_int_value(const set<pair<string,int>> &list_val, const string &str)
    {
        if(str.empty())
            return '?';
        for(auto v : list_val)
            if(v.first == str)
                return v.second;
        return 111111111;
    }


public:
    Automat() = default;
    void citeste(const string &file)
    {
        ifstream f(file);
        f >> n;
        for(int i=0;i<n;i++)
            st.push_back(i);
        f >> m;
        for(int i=0;i<m;i++) {
            char c;
            f >> c;
            alph.insert(c);
        }
        f >> q0;
        f >> k;
        st_f.resize(k);
        for(int i=0;i<k;i++)
            f >> st_f[i];
        f >> l;
        transl.resize(l);
        for(int i=0;i<l;i++)
        {
            f >> transl[i].x;
            f >> transl[i].ch;
            alph.insert(transl[i].ch);          // lambda case
            f >> transl[i].y;

        }
        sters.resize(n);

        for(int i=0;i<sters.size();i++)
            sters[i] = false;
        f.close();
    }
    void afiseaza()
    {
        cout << "?\t";
        for(auto cch : alph)
            cout << cch << "\t";
        cout << endl;
        for(int i=0;i<st.size();i++)
        {
            if(sters[i])
                continue;
            cout << st[i] << "\t";
            for(auto cch : alph)
            {
                bool first = false;
                cout << "{";
                for(auto tr : transl)
                    if (tr.x == st[i] && tr.ch == cch && !sters[tr.y])
                    {
                        if(!first)
                        {
                            first=true;
                            cout<<tr.y;
                        }
                        else
                            cout << ", " << tr.y;
                    }
                cout << "}\t";
            }
            cout << endl;
        }

    }
    void add_l_star(int val, int set_index, vector<set<int>> &l_star) {
        for (auto x : l_star[val])
        {
            if(l_star[set_index].count(static_cast<int>(x)) == 0)
            {
                l_star[set_index].insert(x);
                add_l_star(x, set_index, l_star);
            }
        }
    }
    void lnfa_to_nfa()
    {
        ///Step 1
        vector<set<int>> l_star;
        l_star.resize(n);
        for(int i=0;i<st.size();i++)
        {
            l_star[i].insert(st[i]);
            for (auto tr : transl)
                if (tr.x == st[i] && tr.ch == '$')
                    l_star[i].insert(tr.y);
        }
        for(int i=0;i<st.size();i++)
        {
            for(auto x : l_star[i])
                add_l_star(x,st[i],l_star);
        }
        ///Step 2
        alph.erase('$');
        int nr = alph.size();
        vector<vector<set<int>>> matrice_tranzitie_star;
        for(int i=0;i<nr;i++)
        {
            vector<set<int>> aux;
            aux.resize(n);
            matrice_tranzitie_star.push_back(aux);
        }
        int ll = 0;
        for(auto chx : alph)
        {
             for(int i=0;i<st.size();i++)
                for(auto x : l_star[i])
                    for(auto tr : transl)
                        if(tr.x == x && tr.ch == chx)
                        {
                            matrice_tranzitie_star[ll][i].insert(tr.y);
                        }
            ll++;
        }
        vector<vector<set<int>>> l_star_2;
        for(int i=0;i<nr;i++)
        {
            vector<set<int>> aux;
            aux.resize(n);
            l_star_2.push_back(aux);
        }
        ll=0;
        //reuniune
        for(auto chx : alph)
        {
            for(int i=0;i<n;i++)
            {
                for (auto x : matrice_tranzitie_star[ll][i])
                    for(auto xy : l_star[x])
                            l_star_2[ll][i].insert(xy);
            }
            ll++;
        }
        ///Step 3
        ll=0;
        for(auto chx : alph)
        {
            for(int i=0;i<n;i++)
            {
                for(auto x : l_star_2[ll][i])
                    for(int q=0;q<st_f.size();q++)
                        if(st_f[q] == x && count(st_f.begin(),st_f.end(), st[i]) == 0)
                        {
                            st_f.push_back(st[i]);
                        }
            }
            ll++;
        }
        k=st_f.size();
        sort(st_f.begin(),st_f.end());
        ///Step 4
        vector<int> substitut;
        substitut.resize(n);
        for(int i=0;i<substitut.size();i++)
            substitut[i] = i;
        for(int i=0;i<n-1;i++)
            for(int j=i+1;j<n;j++)
            {
                bool ok=true;
                ll = 0;
                for(auto chx : alph)
                {
                    ok = compara_equal(l_star_2[ll][i], l_star_2[ll][j]);
                    if(!ok)
                        break;
                    ll++;
                }
                if(ok && count(st_f.begin(),st_f.end(),st[i]) == count(st_f.begin(),st_f.end(),st[j]))
                {
                    sters[j] = true;
                    substitut[j] = substitut[i];
                    break;
                }
            }
        //reconstruire tranzitii
        transl.clear();
        ll=0;
        for(auto chx : alph)
        {
            for(int i=0;i<n;i++)
                for (auto y : l_star_2[ll][i])
                    if(!sters[i])
                    {
                        translatie aux;
                        aux.x = st[i];
                        aux.ch = chx;
                        aux.y = substitut[y];
                        if(count(transl.begin(),transl.end(),aux)==0)
                            transl.push_back(aux);
                    }
            ll++;
        }

    }
    void nfa_to_dfa()
    {
        ///Step1
        queue<string> coada;
        struct temp_transl{
            string x;
            char ch;
            string y;
        };
        vector<temp_transl> transl_stringuri;
        string rez;
        set<string> vizitat;
        coada.push(to_string(q0));
        vizitat.insert(to_string(q0));
        while(!coada.empty())
        {
            string q = coada.front();
            for (auto chx : alph)
            {
                rez = reuniune_stari_componente(transl,q,chx);
                if(vizitat.count(rez) == 0 && !rez.empty())
                {
                    coada.push(rez);
                    vizitat.insert(rez);
                }
                temp_transl aux;
                aux.x = q;
                aux.ch = chx;
                aux.y = rez;
                if(!aux.x.empty())
                    transl_stringuri.push_back(aux);
            }
            coada.pop();
        }
        ///Step2
        vector<string> st_f_stringuri;
        for(string st : vizitat)
        {
            if(verifica_stari_componente_st_f(st,st_f))
                st_f_stringuri.push_back(st);
        }
        ///Step 3
        int st_max = 0;
        set<pair<string,int>> list_val;
        for(auto tr : transl)
        {
            if(tr.x > st_max)
                st_max = tr.x;
            if(tr.y > st_max)
                st_max = tr.y;
        }
        for( auto x : vizitat)
        {
            if(x.length() == 1)
                list_val.insert(make_pair(x,stoi(x)));
            else
                list_val.insert(make_pair(x,++st_max));
        }
        //rebuild time
        n = list_val.size();
        transl.clear();
        st_f.clear();
        st.clear();
        sters.clear();
        sters.resize(n);
        fill(sters.begin(),sters.end(),false);
        for(auto x : st_f_stringuri)
        {
            st_f.push_back(get_new_int_value(list_val,x));
        }
        for(auto tr : transl_stringuri)
        {
            translatie nou;
            nou.ch = tr.ch;
            nou.x = get_new_int_value(list_val,tr.x);
            if(count(st.begin(),st.end(),nou.x) == 0)
                st.push_back(nou.x);
            nou.y = get_new_int_value(list_val,tr.y);
            if(nou.y != '?')
                transl.push_back(nou);
        }
        sort(st.begin(),st.end());

    }
    ~Automat()
    {

    }

};



int main()
{
    Automat automat;
    automat.citeste("../input.txt");
    automat.lnfa_to_nfa();
    automat.afiseaza();
    automat.nfa_to_dfa();
    automat.afiseaza();

    cout << endl << endl;
    Automat automat2;
    automat2.citeste("../input2.txt");
    automat2.afiseaza();
    automat2.nfa_to_dfa();
    automat2.afiseaza();
}