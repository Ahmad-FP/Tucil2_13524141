#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>

using namespace std;

#define pb push_back

struct vec3{
    double arr[3];
    bool operator== (const vec3& a) const{
        return arr[0] == a.arr[0] && arr[1] == a.arr[1] && arr[2] == a.arr[2];
    }
};

struct tri{
    int arr[3];
    vec3 normal;
    bool operator== (const tri& a) const{
        return arr[0] == a.arr[0] && arr[1] == a.arr[1] && arr[2] == a.arr[2] && normal == a.normal; 
    }
};
namespace std {
    template <>
    struct hash<vec3> {
        size_t operator()(const vec3& v) const {
            size_t h = 0;
            for (int i = 0; i < 3; ++i) {
                h ^= std::hash<double>{}(v.arr[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
    template <>
    struct hash<tri> {
        size_t operator()(const tri& t) const {
            size_t h = 0;
            for (int i = 0; i < 3; ++i) {
                h ^= std::hash<int>{}(t.arr[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            h ^= std::hash<vec3>{}(t.normal);
            return h;
        }
    };
}

struct cube{
    double minX,maxX,minY,maxY,minZ,maxZ;
};


vec3 cross(vec3 a, vec3 b){
    vec3 res;
    res.arr[0] = (a.arr[1] * b.arr[2] - b.arr[1] * a.arr[2]);
    res.arr[1] = (a.arr[2] * b.arr[0] - b.arr[2] * a.arr[0]);
    res.arr[2] = (a.arr[0] * b.arr[1] - b.arr[0] * a.arr[1]);
    return res;
}

double dot(vec3 a, vec3 b){
    double res = 0;
    res += (a.arr[1] * b.arr[1]) +(a.arr[0] * b.arr[0]) + (a.arr[2] * b.arr[2]);
    return res;
}

double minX =__DBL_MAX__;
double maxX =-__DBL_MAX__;
double minY =__DBL_MAX__;
double maxY =-__DBL_MAX__;
double minZ =__DBL_MAX__;
double maxZ =-__DBL_MAX__;
int maxDepth;


vector<vec3> point;
vector<tri> face;
vector<pair<int,int> > dInfo; // (terbentuk, not explored)
vector<vec3> cPoint;
vector<tri> cFace;
unordered_map<vec3,int> antiDupPoint;
unordered_set<tri> antiDupFace;





void read(string name){
    ifstream f(name);
    string l;
    while(getline(f,l)){
        istringstream ss(l);
        string sig;
        ss >> sig;
        vec3 temp1;
        tri temp2;
        tri temp3;
        if(sig == "v"){
            ss >>temp1.arr[0] >> temp1.arr[1] >> temp1.arr[2];
            if(temp1.arr[0] < minX)minX =temp1.arr[0];
            if(temp1.arr[0] > maxX)maxX =temp1.arr[0];
            if(temp1.arr[1] < minY)minY =temp1.arr[1];
            if(temp1.arr[1] > maxY)maxY =temp1.arr[1];
            if(temp1.arr[2] < minZ)minZ =temp1.arr[2];
            if(temp1.arr[2] > maxZ)maxZ =temp1.arr[2];
            point.pb(temp1);
        }else if (sig == "f"){
            int x,y,z;
            ss >> x >> y >> z;
            x--;y--;z--;
            temp2.arr[0] =x; temp2.arr[1] = y; temp2.arr[2] =z;
            face.pb(temp2);
        }else{
            continue;
        }
    }
    cout << "File " << name << " berhasil dibaca\n"; 
}

void save(string name){
    ofstream o(name);
    o << scientific;
    o << setprecision(5);
    for (int i = 0; i < cPoint.size(); i++)
    {
        o <<"v " << cPoint[i].arr[0] << " " << cPoint[i].arr[1] << " " << cPoint[i].arr[2] << "\n";
    }
    for (int i = 0; i < cFace.size(); i++)
    {
        o << "f " << cFace[i].arr[0] << " " << cFace[i].arr[1] << " " << cFace[i].arr[2] << "\n";
    }
    
    cout << "File " << name << " berhasil disimpan\n";
}

mutex queLock;
queue<pair<cube,pair< vector<tri*>,int> > > tQue;
atomic<int> tQueCount =0;
atomic<int> activeCount =0;
atomic<bool> shutdown = false;


void enque(cube m, vector<tri*> tr,int depth){
    queLock.lock();
    tQue.push({m,{tr,depth}});
    tQueCount++;
    tQueCount.notify_one();
    queLock.unlock();
}


bool checkPos(vec3 p, tri* s){
    p.arr[0] -= point[s->arr[0]].arr[0];
    p.arr[1] -= point[s->arr[0]].arr[1];
    p.arr[2] -= point[s->arr[0]].arr[2];
    if (dot(p,s->normal) > 0)
    {
        return true;
    }
    return false;
}

auto calcPointsOrder(cube parent, int order){
    double unitX = (parent.maxX + parent.minX)/2 - parent.minX;
    double unitY = (parent.maxY + parent.minY)/2 - parent.minY;
    double unitZ = (parent.maxZ + parent.minZ)/2 - parent.minZ;
    cube res;
    vector<vec3> coords(8);
    coords[0].arr[0] = parent.minX;coords[0].arr[1]=parent.minY;coords[0].arr[2] = parent.minZ;
     if (order == 1)
    {
        coords[0].arr[2] += unitZ;
    }else if (order ==2)
    {
        coords[0].arr[0] += unitX;        
    }else if (order ==3)
    {
        coords[0].arr[2] += unitZ;
        coords[0].arr[0] += unitX;
    }else if (order == 4)
    {
        coords[0].arr[1] += unitY;
    }else if (order == 5)
    {
        coords[0].arr[1] += unitY;
        coords[0].arr[2] += unitZ;
        
    }else if (order == 6)
    {
        coords[0].arr[0] += unitX;
        coords[0].arr[1] += unitY;
        
    }else if ( order ==7){
        coords[0].arr[0] += unitX;
        coords[0].arr[1] += unitY;
        coords[0].arr[2] += unitZ;

    }
    coords[1] = coords[0];
    coords[1].arr[2] += unitZ;

    coords[2] = coords[0];
    coords[2].arr[0] += unitX;        

    coords[3] = coords[0];
    coords[3].arr[2] += unitZ;
    coords[3].arr[0] += unitX;

    coords[4] = coords[0];
    coords[4].arr[1] += unitY;

    coords[5] = coords[0];
    coords[5].arr[1] += unitY;
    coords[5].arr[2] += unitZ;

    coords[6] = coords[0];
    coords[6].arr[0] += unitX;
    coords[6].arr[1] += unitY;

    coords[7] = coords[0];
    coords[7].arr[0] += unitX;
    coords[7].arr[1] += unitY;
    coords[7].arr[2] += unitZ;
    
    res.minX = coords[0].arr[0];
    res.minY = coords[0].arr[1];
    res.minZ = coords[0].arr[2];
    res.maxX = coords[7].arr[0];
    res.maxY = coords[7].arr[1];
    res.maxZ = coords[7].arr[2];
    
    
    
    
    struct resu {cube a; vector<vec3> arr;};

    return resu{res, coords};
}

vector<vec3> calcPoints(cube cur){
    double unitX = (cur.maxX + cur.minX)/2 - cur.minX;
    double unitY = (cur.maxY + cur.minY)/2 - cur.minY;
    double unitZ = (cur.maxZ + cur.minZ)/2 - cur.minZ;
    vector<vec3> coords(8);
    coords[0].arr[0] = cur.minX;coords[0].arr[1]=cur.minY;coords[0].arr[2] = cur.minZ;
     
    coords[1] = coords[0];
    coords[1].arr[2] += unitZ;

    coords[2] = coords[0];
    coords[2].arr[0] += unitX;        

    coords[3] = coords[0];
    coords[3].arr[2] += unitZ;
    coords[3].arr[0] += unitX;

    coords[4] = coords[0];
    coords[4].arr[1] += unitY;

    coords[5] = coords[0];
    coords[5].arr[1] += unitY;
    coords[5].arr[2] += unitZ;

    coords[6] = coords[0];
    coords[6].arr[0] += unitX;
    coords[6].arr[1] += unitY;

    coords[7] = coords[0];
    coords[7].arr[0] += unitX;
    coords[7].arr[1] += unitY;
    coords[7].arr[2] += unitZ;
    return coords;
}

//TODO mutex for accessing cPoint and cFace
mutex cpFlag;
mutex cfFlag;
mutex diFlag;
void createFaces(vector<vec3>& coords){
    cpFlag.lock();
    tri in;
    in.normal = {{0,0,0}};
    vector<tri> list;
    //012
    in.arr[0] = antiDupPoint[coords[0]];
    in.arr[1] = antiDupPoint[coords[1]];
    in.arr[2] = antiDupPoint[coords[2]];
    list.pb(in);
    //123
    in.arr[0] = antiDupPoint[coords[1]];
    in.arr[1] = antiDupPoint[coords[2]];
    in.arr[2] = antiDupPoint[coords[3]];
    list.pb(in);
    //135
    in.arr[0] = antiDupPoint[coords[1]];
    in.arr[1] = antiDupPoint[coords[3]];
    in.arr[2] = antiDupPoint[coords[5]];
    list.pb(in);
    //357
    in.arr[0] = antiDupPoint[coords[3]];
    in.arr[1] = antiDupPoint[coords[5]];
    in.arr[2] = antiDupPoint[coords[7]];
    list.pb(in);
    //456
    in.arr[0] = antiDupPoint[coords[4]];
    in.arr[1] = antiDupPoint[coords[5]];
    in.arr[2] = antiDupPoint[coords[6]];
    list.pb(in);
    //567
    in.arr[0] = antiDupPoint[coords[5]];
    in.arr[1] = antiDupPoint[coords[6]];
    in.arr[2] = antiDupPoint[coords[7]];
    list.pb(in);
    //024
    in.arr[0] = antiDupPoint[coords[0]];
    in.arr[1] = antiDupPoint[coords[2]];
    in.arr[2] = antiDupPoint[coords[4]];
    list.pb(in);
    //246
    in.arr[0] = antiDupPoint[coords[2]];
    in.arr[1] = antiDupPoint[coords[4]];
    in.arr[2] = antiDupPoint[coords[6]];
    list.pb(in);
    //045
    in.arr[0] = antiDupPoint[coords[0]];
    in.arr[1] = antiDupPoint[coords[4]];
    in.arr[2] = antiDupPoint[coords[5]];
    list.pb(in);
    //015
    in.arr[0] = antiDupPoint[coords[0]];
    in.arr[1] = antiDupPoint[coords[1]];
    in.arr[2] = antiDupPoint[coords[5]];
    list.pb(in);
    //267
    in.arr[0] = antiDupPoint[coords[2]];
    in.arr[1] = antiDupPoint[coords[6]];
    in.arr[2] = antiDupPoint[coords[7]];
    list.pb(in);
    //237
    in.arr[0] = antiDupPoint[coords[2]];
    in.arr[1] = antiDupPoint[coords[3]];
    in.arr[2] = antiDupPoint[coords[7]];
    list.pb(in);
    cpFlag.unlock();
    cfFlag.lock();
    for (int i = 0; i < 12; i++)
    {
        if(antiDupFace.find(list[i]) == antiDupFace.end()){
            antiDupFace.insert(list[i]);
            cFace.pb(list[i]);
        }
    }
    cfFlag.unlock();
}
//TODO voxelize
void voxelize(cube parent, vector<tri*> tr,int depth){
    if(depth != maxDepth){
        // vec3 center;                                     //abandoned feature
        // center.arr[0] = (parent.maxX +parent.minX)/2;
        // center.arr[1] = (parent.maxY +parent.minY)/2;
        // center.arr[2] = (parent.maxZ +parent.minZ)/2; 
        for (int i = 0; i < 8; i++) // divide to 8 cubes
        {
            vector<tri*> trChild;
            auto [child,childPos] = calcPointsOrder(parent,i);
            
            
            for (int j = 0; j < tr.size(); j++) // check all trig
            {
                vec3& v0 = point[tr[j]->arr[0]];
                vec3& v1 = point[tr[j]->arr[1]];
                vec3& v2 = point[tr[j]->arr[2]];
                double tMinX = min({v0.arr[0], v1.arr[0], v2.arr[0]});
                double tMaxX = max({v0.arr[0], v1.arr[0], v2.arr[0]});
                double tMinY = min({v0.arr[1], v1.arr[1], v2.arr[1]});
                double tMaxY = max({v0.arr[1], v1.arr[1], v2.arr[1]});
                double tMinZ = min({v0.arr[2], v1.arr[2], v2.arr[2]});
                double tMaxZ = max({v0.arr[2], v1.arr[2], v2.arr[2]});
                if(tMaxX < child.minX || tMinX > child.maxX ||
                   tMaxY < child.minY || tMinY > child.maxY ||
                   tMaxZ < child.minZ || tMinZ > child.maxZ) continue;


                int above = 0;
                int below = 0;
                for (int k = 0; k < 8; k++)
                {
                    if(above && below)break;
                    if(checkPos(childPos[k],tr[j])){
                        above++;
                    }else below++;
                }
                if(above && below)trChild.push_back(tr[j]);
            }
            if(trChild.size()){
                enque(child,trChild,depth+1);
                diFlag.lock();
                dInfo[depth+1].first++;
                diFlag.unlock();
            }else{
                diFlag.lock();
                dInfo[depth].second--;
                diFlag.unlock();
            }
        }
        
    }else{
        vector<vec3> cords = static_cast<vector<vec3>&&> (calcPoints(parent));
        cpFlag.lock();
        for (int i = 0; i < 8; i++)
        {
            if(antiDupPoint[cords[i]] == 0){
                antiDupPoint[cords[i]] = cPoint.size()+1;
                cPoint.push_back(cords[i]);
            }
        }
        cpFlag.unlock();
        createFaces(cords);
    }
}


void loop(){
    while(!shutdown){
        tQueCount.wait(0);
        if(shutdown)break;
        queLock.lock();
        if(tQue.empty()){
            queLock.unlock();
            continue;
        }
        pair<cube,pair< vector<tri*>,int>> t = static_cast<pair<cube, pair< vector<tri*>,int>>&&> (tQue.front());
        tQue.pop();
        queLock.unlock();
        activeCount++;
        tQueCount--;
        voxelize(t.first,t.second.first,t.second.second);
        activeCount--;
        if(activeCount == 0 && tQueCount == 0){
            // cout << "SHUTDOWN CALL\n";
            shutdown =true;
            shutdown.notify_all();
            tQueCount = 2;
            tQueCount.notify_all();
            break;
        }
        // cout << activeCount << " active " << tQueCount << " tque " << tQue.size() << " actual tque\n";
    }
    
}

void resetvar(){
    point.clear();
    face.clear();
    dInfo.clear();
    cPoint.clear();
    cFace.clear();
    minX =__DBL_MAX__;
    maxX =-__DBL_MAX__;
    minY =__DBL_MAX__;
    maxY =-__DBL_MAX__;
    minZ =__DBL_MAX__;
    maxZ =-__DBL_MAX__;
    maxDepth = 0;
    tQueCount = 0;
    shutdown = false;
    activeCount = 0;
    antiDupFace.clear();
    antiDupPoint.clear();
}

int main(){
    while(true){
        int choice;
        cout <<"Pilih aksi yang ingin dilakukan\n";
        cout << "1. Voxelisasi\n";
        cout << "2. Save\n";
        cout << "3. Reset Variabel\n";
        cout << "4. Exit\n";
        cin >> choice;
        if(choice == 1)
        {
            string name;
            cout << "Masukkan kedalaman maksimum voxelisasi (Disarankan < 6)\n";
            cin >>maxDepth;
            cout << "Masukkan nama file .obj yang ingin di voxelisasi (contoh: pumpkin.obj)\n";
            cin >>name;
            auto start = chrono::steady_clock::now();
            read(name);
            dInfo.assign(maxDepth+1,{0,0});
            cube c1;
            vector<tri*> tr;
            
            c1.maxX =maxX;c1.minX = minX;c1.minY =minY;c1.maxY = maxY;c1.minZ = minZ;c1.maxZ =maxZ;
            for (int i = 0; i < face.size(); i++)
            {
                vec3 a = point[face[i].arr[1]],b = point[face[i].arr[2]];
                vec3 o = point[face[i].arr[0]];
                a.arr[0] = a.arr[0] - o.arr[0];
                a.arr[1] = a.arr[1] - o.arr[1];
                a.arr[2] = a.arr[2] - o.arr[2];
                b.arr[0] = b.arr[0] - o.arr[0];
                b.arr[1] = b.arr[1] - o.arr[1];
                b.arr[2] = b.arr[2] - o.arr[2];
                face[i].normal = cross(a,b);
            }
            
            for (int i = 0; i < face.size(); i++)
            {
                tr.push_back(&face[i]);
            }
            
            vector<thread> threads;
            for (int i = 0; i < 8; i++)
            {
                threads.emplace_back(loop);
            }
            dInfo[0].first++;
            enque(c1,tr,0);
            shutdown.wait(false);
            
            for(auto& t : threads){
                if(t.joinable())t.join();
            }
            auto end = chrono::steady_clock::now();
            // cout << "Apakah ingin menghapus overlapping faces pada voxels?\n";
            // cout << "1. YA\n";
            // cout << "[ANY]. TIDAK\n";
            // int choice2; cin >> choice2;
            // if(choice2 == 1){
            //     cout << "Banyak faces sebelum culling " << cFace.size() <<endl;
            //     //culling function
            // }
            chrono::duration<double> t = end - start;
            cout << 
            " ▗▄▄▖▗▄▄▄▖ ▗▄▖▗▄▄▄▖▗▄▄▖\n"
            "▐▌     █  ▐▌ ▐▌ █ ▐▌   \n"
            " ▝▀▚▖  █  ▐▛▀▜▌ █  ▝▀▚▖\n"
            "▗▄▄▞▘  █  ▐▌ ▐▌ █ ▗▄▄▞▘\n";
            cout << "[STATS] Banyaknya voxel yang terbentuk " << dInfo[maxDepth].first << endl;
            cout << "[STATS] Banyaknya Vertex yang terbentuk " << cPoint.size() << endl;
            cout << "[STATS] Banyaknya faces yang terbentuk " << cFace.size() << endl;
            cout << "////////////////////////////////////////\n";
            cout << "////////////////////////////////////////\n";
            for (int i = 0; i < dInfo.size(); i++)
            {
                cout<< "[STATS] " << i+1 << ". banyaknya node dengan depth " << i << " yang terbentuk " << dInfo[i].first << endl; 
            }
            cout << "////////////////////////////////////////\n";
            cout << "////////////////////////////////////////\n";
            for (int i = 0; i < dInfo.size(); i++)
            {
                cout<< "[STATS] " << i+1 << ". banyaknya node dengan depth " << i << " yang tidak perlu ditelusuri " << abs(dInfo[i].second) << endl; 
            }
            int ocDepth = 0;
            for (int i = dInfo.size()-1; i >= 0 ; i--)
            {
                if (dInfo[i].first != 0 || dInfo[i].second != 0)
                {
                    ocDepth = i;break;
                }
                
            }
            
            cout << "////////////////////////////////////////\n";
            cout << "[STATS] Kedalaman Octree " << ocDepth<< endl;
            cout << "[STATS] Periode eksekusi fungsi voxelisasi " << t << endl;
            cout << "////////////////////////////////////////\n";
            cout << "////////////////////////////////////////\n";

        }else if (choice == 2)
        {
            string name;
            cout << "Masukkan nama file yang ingin di simpan (contoh: pumpkin.obj)\n";
            cin >> name;
            save(name);
            resetvar();
        }else if (choice == 3)
        {
            resetvar();
            cout << "Variabel telah direset\n";
        }
         else if (choice == 4)
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