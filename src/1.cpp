#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

#define pb push_back

struct vec3{
    double arr[3];
    
};

struct tri{
    int arr[3];
};

struct cube{
    double minX,maxX,minY,maxY,minZ,maxZ;
};



double minX =__DBL_MAX__;
double maxX =-__DBL_MAX__;
double minY =__DBL_MAX__;
double maxY =-__DBL_MAX__;
double minZ =__DBL_MAX__;
double maxZ =-__DBL_MAX__;

vector<vec3> point;
vector<tri> face;
vector<pair<int,int> > dInfo; // (terbentuk, not explored)
vector<vec3> cpoint;
vector<tri> cface;


void resetvar(){
    point.clear();
    face.clear();
    dInfo.clear();
    cpoint.clear();
    cface.clear();
    minX =__DBL_MAX__;
    maxX =-__DBL_MAX__;
    minY =__DBL_MAX__;
    maxY =-__DBL_MAX__;
    minZ =__DBL_MAX__;
    maxZ =-__DBL_MAX__;
}
vector<vec3> calcPoints(){

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
            ss >>temp1.arr[0] >> temp1.arr[1] >> temp1.arr[2];
            if(temp1.arr[0] < minX)minX =temp1.arr[0];
            if(temp1.arr[0] > maxX)maxX =temp1.arr[0];
            if(temp1.arr[1] < minY)minY =temp1.arr[1];
            if(temp1.arr[1] > maxY)maxY =temp1.arr[1];
            if(temp1.arr[2] < minZ)minZ =temp1.arr[2];
            if(temp1.arr[2] > maxZ)maxZ =temp1.arr[2];
            point.pb(temp1);
        }else if (sig == 'f'){
            int x,y,z;
            ss >> x >> y >> z;
            temp2.arr[0] =x; temp2.arr[1] = y; temp2.arr[2] =z;
            face.pb(temp2);
        }else{
            continue;
        }
    }
    cout << "File " << name << "berhasil dibaca\n"; 
}

void save(string name){
    ofstream o(name);
    o << scientific;
    o << setprecision(5);
    for (int i = 0; i < cpoint.size(); i++)
    {
        o <<"v " << cpoint[i].arr[0] << " " << cpoint[i].arr[1] << " " << cpoint[i].arr[2] << "\n";
    }
    for (int i = 0; i < cface.size(); i++)
    {
        o << "f " << cface[i].arr[0] << " " << cface[i].arr[1] << " " << cface[i].arr[2] << "\n";
    }
    
    cout << "File " << name << "berhasil disimpan\n";
}

mutex queLock;
queue<pair<cube, vector<tri*> > > tQue;
atomic<int> tQueCount =0;
atomic<int> activeCount =0;
vector<thread> threads;

void loop(){
    while(activeCount > 0 || tQueCount > 0){
        tQueCount.wait(0);
        queLock.lock();
        if(tQue.empty()){
            queLock.unlock();
            continue;
        }
        pair<cube,vector<tri*>> t = static_cast<pair<cube, vector<tri*>>&&> (tQue.front());
        tQue.pop();
        queLock.unlock();
        tQueCount--;
        activeCount++;
        voxelize(t.first,t.second);
        activeCount--;
    }
    
}

void enque(cube m, vector<tri*> tr){
    queLock.lock();
    tQue.push({m,tr});
    tQueCount++;
    tQueCount.notify_one();
    queLock.unlock();
}

//TODO mutex for accessing cpoint and cface
//TODO voxelize
void voxelize(cube m, vector<tri*> tr){
    
}


int main(){
    while(true){
        int c;
        cout <<"Pilih aksi yang ingin dilakukan\n";
        cout << "1. Voxelisasi\n";
        cout << "2. Save\n";
        cout << "3. Exit\n";
        cin >> c;
        if(c == 1){
            int maxdepth;
            string name;
            cout << "Masukkan kedalaman maksimum voxelisasi\n";
            cin >>maxdepth;
            cout << "Masukkan nama file .obj yang ingin di voxelisasi (contoh: pumpkin.obj)\n";
            cin >>name;
            read(name);
            cube c1;
            c1.maxX =maxX;c1.minX = minX;c1.minY =minY;c1.maxY = maxY;c1.minZ = minZ;c1.maxZ =maxZ;

        }else if (c == 2)
        {
            string name;
            cout << "Masukkan nama file yang ingin di simpan (contoh: pumpkin.obj)\n";
            save(name);
        }else if (c == 3)
        {
            cout << 
            "110 plis\n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣤⣄⡀⠀    \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡸⠋⠀⠘⣇⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠇⠀⠀⠀⢸⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠀⠀⠀⢸⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠇⠀⠀⠀⠀⢸⠇⠀⠀   \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡎⠀⠀⠀⠀⠀⢸⠀⠀⠀  \n"
            "⠀⠀⢀⣀⣀⣀⠀⠀⠀⠀⠀⢀⣀⣤⡤⠤⠤⠤⠤⢤⣤⣀⡤⢖⡿⠛⠉⢳⠀⠀⠀⠀⠀⢸⠀⠀⠀  \n"
            "⠀⢼⠁⠉⠉⠛⠻⢭⡓⠒⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣏⠀⠀⠀⢸⠀⠀⠀⠀⠀⡤⠀⠀⠀  \n"
            "⠀⠸⡄⠀⠀⠀⠀⢸⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠂⠀⠀⡜⠀⠀⠀⠀⢀⡇⠀⠀⠀  \n"
            "⠀⠀⢷⠀⠀⠀⠠⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⢠⠏⠀⠀⠀⠀⢸⠃⠀⠀⠀  \n"
            "⠀⠀⠈⢧⠀⢀⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡞⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀  \n"
            "⠀⠀⠀⠈⢳⡈⠁⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⣶⣶⣦⠀⠀⢹⠀⠀⠀⠀⠀⡎⠀⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⢠⣾⣟⣹⡄⠀⠀⠀⠀⡀⠀⣿⣿⣿⡇⠀⢈⣧⠤⠤⠶⠶⢷⠒⠒⠂⠀  \n"
            "⠀⠀⢀⣀⣠⡧⠄⠀⠀⠀⣾⣿⣿⣿⠇⠀⠀⠀⠙⠁⠀⠙⠻⠿⠃⠀⠨⣼⣤⣀⡀⠀⠈⢧⠀⠀⠀  \n"
            "⠘⠉⠁⠀⢸⣤⡤⠀⠀⠀⠛⢿⡿⠋⠀⠀⠀⠀⠴⠦⠀⠀⠀⠀⠀⠐⣲⣯⡀⠀⠈⠙⠓⠺⣧⣄⡀  \n"
            "⠀⣀⡤⠚⠉⢳⡴⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⠃⠀⠈⠓⢦⡀⠀⠀⢸⠀⠈  \n"
            "⠀⠁⠀⢀⡔⠉⠙⡶⢄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠴⠚⠁⠀⠀⠀⠀⠀⠀⠈⠓⠆⠀⡇⠀  \n"
            "⠀⠀⠰⠋⠀⠀⢸⡇⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠁⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠈⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡎⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠹⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠙⢆⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠄⠀⢰⠇⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⠶⠺⣇⠀⣀⡜⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢱⡄⠀⠀⠀⠹⡟⠒⢢⡀⠀⠀⠀⠀⢀⡏⠀⠀⠀⠈⠉⠉⠁⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣄⠀⠀⢀⡇⠀⠀⠻⣄⠀⠀⠀⡸⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  \n"
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢷⠶⠋⠀⠀⠀⠀⠈⣣⠶⠖⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  \n";
            break;
        }
    }
}