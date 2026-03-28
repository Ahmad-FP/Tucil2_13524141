PENJELASAN PROGRAM
   Program ini melakukan voxelisasi pada file .obj 3D menggunakan octree subdivision.
   - Membaca file .obj dan membangun bounding box dari vertex yang ada
   - Membagi ruang secara rekursif menjadi 8 sub-kubus (octree) hingga kedalaman maksimum
   - Menggunakan multithreading (8 threaded) agar proses voxelisasi terjadi secara konkuren
   - Mendeteksi duplikasi vertex dan face untuk menghasilkan output yang bersih
   - Menyimpan hasil voxelisasi ke file .obj

REQUIREMENT
-  C++  >20
- glibc >2.34 (opsional)

CARA COMPILE
cd src
g++ -std=c++23 -o tucil Voxel.cpp 
g++ -std=c++23 -o tucil Voxel.cpp -pthread (jika tidak memliki versi glibc >2.34)

Saran Variable kedalaman maksimum
- Akibat dari pembagian kubus yang menghasilkan maksimum 8^maxdepth anakan kubus, disarankan kedalaman maksimum lebih kecil dari 7
- Pada kedalaman 10 line.obj yang hanya berisi 3 vertices dan 1 face menghasilkan file sebesar 1.40 GB

CARA MENJALANKAN
1 cd bin
2 ./tucil
3 Pilih aksi:
   - "1. Voxelisasi" Memulai proses voxelisasi
     - Masukkan kedalaman maksimum voxelisasi (semakin dalam semakin detail)
     - Masukkan nama file .obj yang ingin di voxelisasi (contoh: pumpkin.obj)
   - "2. Save" Menyimpan hasil voxelisasi ke file .obj
     - Masukkan nama file output (contoh: pumpkin_voxel.obj)
   - "3. Exit" Keluar dari program

Identitas
Nama  : Ahmad Fauzan Putra

NIM   : 13524141

Kelas : K3


