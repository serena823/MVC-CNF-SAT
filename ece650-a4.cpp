#include <sstream>
#include <list>
#include <climits>
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
#include <vector>
// defined std::cout
#include <iostream>
#include <algorithm>
using namespace std;

int count_line = 0;
int flag = 0;
int N = 0;
int A[100][100];
bool edge_confirm = false;
bool vertex_confirm = false;
vector<int> edges;
bool res;

bool gain_edges(string edge) {
    flag = 1;
    int comma_position = 0;
    string a , b ;
    edge = edge.substr(1);
    edge = edge.substr(0, edge.length() - 1);
    //cout<<"Edge: "<<edge<<endl;
    for (int i = 0; i < edge.length(); i++) {
        if (edge[i] == ',') {
            comma_position = i;
            a = edge.substr(0, comma_position);
            b = edge.substr(comma_position + 1, edge.length() - 1);
            int aa = stoi(a);
            int bb = stoi(b);
            if ((aa < 0) || (aa >= N) || (bb < 0) || (bb >= N)) {
                cerr << "Error:Invild input.\n";
                return false;
            } else {
                edges.push_back(aa);
                edges.push_back(bb);
                count_line = count_line + 1;
            }
        }
    }
    return true;
}

bool sep_edge_com(string in_edge) {

    string m = in_edge.substr(1, in_edge.length());
    int count1 = 0;
    for (int i = 0; i < m.length(); i++) {
        if ((m[i] == ',') && (m[i - 1] == '>')) {
            string edges1;
            edges1 = m.substr(count1, i - count1);
            bool b = gain_edges(edges1);
            if (!b) {
                return false;
            }
            count1 = i + 1;
            //cout << edges1 << endl;
        }
        if ((m[i] == '}') && (m[i - 1] == '>')) {
            string edges2;
            edges2 = m.substr(count1, i - count1);
            bool b = gain_edges(edges2);
            if (!b) {
                return false;
            }
            //cout << edges2 << endl;
        }
    }
    edge_confirm = true;
    return true;
}

void vertex_cover() {

    int k = 0;
    while (!res == 1) {
        // -- allocate on the heap so that we can reset later if needed
        unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        vector<vector<Minisat::Lit>> var;
        Minisat::Lit b;
        for (int j = 0; j < k; j++) {
            vector<Minisat::Lit> temp;
            for (int i = 0; i < N; i++) {

                b = Minisat::mkLit(solver->newVar());

                temp.push_back(b);
            }
            var.push_back(temp);
        }
//add_clause step 1 hengzhe

        for (int a = 0; a < k; a++) {
            for (int b = 0; b < N; b++) {
                for (int c = b + 1; c < N; c++) {
                    //std:: cout << a << "-" << b << " " << a << "-" << c <<  std::endl;
                    solver->addClause(~var[a][b], ~var[a][c]);
                }
            }
        }

//add_clause step 2 yihang/
        for (int a = 0; a < k; a++) {
            Minisat::vec<Minisat::Lit> row;
            for (int b = 0; b < N; b++) {
                // std:: cout << a << "-" << b <<  std::endl;
                row.push(var[a][b]);
            }
            solver->addClause(row);
        }

//add_clause step 3 shuzhe///
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < k; y++) {
                for (int z = y + 1; z < k; z++) {
                    // std:: cout << f << "-" << e <<  " " << g << "-" << e << std::endl;
                    solver->addClause(~var[y][x], ~var[z][x]);

                }
            }
        }

//add_clause step 4 edge /
        int size_edges = edges.size();
        for (int a = 0; a < size_edges; a += 2) {
            Minisat::vec<Minisat::Lit> E;
            int start = edges[a];
            int end = edges[a + 1];
            for (int b = 0; b < k; b++) {
                //std:: cout << b << "-" << end <<  " " << b << "-" << start << std::endl;
                E.push(var[b][end]);
                E.push(var[b][start]);
            }
            //std::cout << "E sise :: " << E.size() << std::endl;
            solver->addClause(E);
        }

        bool res = solver->solve();
//print
        if (res) {
            vector<int> result;
            for (int a = 0; a < k; a++) {
                for (int b = 0; b < N; b++) {
                    if (Minisat::toInt(solver->modelValue(var[a][b])) == 0) {
                        result.push_back(b);
                    }
                }
            }
            sort(result.begin(), result.end());
            int size_result = result.size();

            for (int i = 0; i < size_result; i++)
            {
                cout << result[i] << " ";
            }
            cout << endl;
//clear
            count_line = 0;
            flag = 0;
            N = 0;
            edge_confirm = false;
            vertex_confirm = false;
            edges.clear();
            res = false;
            result.clear();
            k = 0;
            break;
        }
        k = k + 1;
        solver.reset (new Minisat::Solver());

    }

}

int main(int argc, char** argv) {

    char in_V = 'V';
    char in_E = 'E';
    void vertex_cover();

    while (!cin.eof()) {
        bool a;
        string line;
        getline(cin, line);
        //cout << line << endl;
        istringstream input(line);
        // while there are characters in the input line
        while (!input.eof()) {
            char command;
            // parse an integer
            input >> command;
            //cout << "command is:" << command << endl;
            if (input.fail()) {
                //cerr << "Error:input failed.\n";
                break;
            } else if (command == in_V) {
                unsigned num;
                input >> num;
                if (num > 0) {
                    N = num;
                } else {
                    cerr << "Error:Vertex number should be positive.\n";
                    break;
                }
            } else if (command == in_E) {
                if (N > 0) {
                    string in_edge;
                    input >> in_edge;
                    a=sep_edge_com(in_edge);
                    if(a){
                        vertex_cover();
                    }
                   break;
                } else {
                    cerr << "Error:Please enter vertex number first.\n";
                    break;
                }
            } else {
                cerr << "Error:Invaild command.\n";
                break;
            }
        }
    }

}


