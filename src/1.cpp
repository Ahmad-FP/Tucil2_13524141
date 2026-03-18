#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


using namespace std;

#define pb push_back

struct vec3{
    double x,y,z;
};

struct tri{
    vec3 v0,v1,v2;
    int arr[3];
};
double minX =-__DBL_MAX__;
double maxX =__DBL_MAX__;
double minY =-__DBL_MAX__;
double maxY =__DBL_MAX__;
double minZ =-__DBL_MAX__;
double maxZ =__DBL_MAX__;

vector<vec3> point;
vector<tri> face;

void resetvar(){
    point.clear();
    face.clear();
    minX =-__DBL_MAX__;
    maxX =__DBL_MAX__;
    minY =-__DBL_MAX__;
    maxY =__DBL_MAX__;
    minZ =-__DBL_MAX__;
    maxZ =__DBL_MAX__;
}

void read(string name){
    resetvar();
    ifstream f(name);
    string l;
    while(getline(f,l)){
        istringstream ss(l);
        char sig;
        ss >> sig;
        vec3 temp1;
        tri temp2;
        tri temp3;
        if(sig == 'v'){
            ss >>temp1.x >> temp1.y >> temp1.z;
            point.pb(temp1);
        }else if (sig == 'f'){
            int x,y,z;
            ss >> x >> y >> z;
            x--;y--;z--;
            temp2.v0 = point[x]; temp2.v1 = point[y]; temp2.v2 = point[z];
            temp2.arr[0] =x+1; temp2.arr[1] = y+1; temp2.arr[2] =z+1;
            face.pb(temp2);
        }else{
            continue;
        }
    }
    cout << "File " << name << "berhasil dibaca\n"; 
}

void save(string name, vector<vec3>& cpoint, vector<tri>& cface){
    ofstream o(name);
    o << scientific;
    o << setprecision(5);
    for (int i = 0; i < cpoint.size(); i++)
    {
        o <<"v " << cpoint[i].x << " " << cpoint[i].y << " " << cpoint[i].z << "\n";
    }
    for (int i = 0; i < cface.size(); i++)
    {
        o << "f " << cface[i].arr[0] << " " << cface[i].arr[1] << " " << cface[i].arr[2] << "\n";
    }
    
    cout << "File " << name << "berhasil disimpan\n";
}


int main(){

}