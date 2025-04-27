#include <cstring>
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstring>


// include libspatialindex.
#include <spatialindex/SpatialIndex.h>
#include <spatialindex/capi/sidx_impl.h>
#include <spatialindex/capi/sidx_config.h>


using namespace SpatialIndex;
using namespace std;


// global

// fun_definition
vector <SpatialIndex::IShape*> point_list ;
// long DATASET_SIZE = 17622266;
long DATASET_SIZE = 500001;
IStorageManager* mem_storege = StorageManager::createNewMemoryStorageManager();

Index* idx;

void load_data(string file_path);

void set_up_Rtree(RTIndexVariant variant);

void insert_all (); 

void insertion_experiment ();

int main (int argc, char* argv[]) {
    load_data("datasets/t_drive_merged_filtered_shuffled.csv");
    set_up_Rtree(RT_Linear); //0 = linear, 1 = quardatic, 2 = star
    insertion_experiment();
}


void set_up_Rtree(RTIndexVariant variant) {
    Tools::PropertySet* ps = GetDefaults();
	Tools::Variant var ;
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = RT_RTree ; // 0
	ps->setProperty("IndexType", var);


	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = RT_Memory; // 0
	ps->setProperty("IndexStorageType", var);

  	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = variant; // 0
	ps->setProperty("IndexVariant", var);


	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 3 ;
    ps->setProperty("Dimension", var);

    idx = new Index(*ps);

    cout << idx->GetProperties() << endl;

}


void load_data(string file_path) { // preprocessing
    ifstream my_file;
    my_file.open(file_path);
    
    bool is_first_line = true;
    int count = 0;
    int i;
    string header;
    getline(my_file,header,',');
    getline(my_file,header,',');
    getline(my_file,header,',');
    getline(my_file,header ,'\n');
    for (i = 0; i < DATASET_SIZE; i++) {

        string id;
        string timestame;
        string longitude;
        string latitude;

        getline(my_file,id,',');
        getline(my_file,timestame,',');
        getline(my_file,longitude,',');
        getline(my_file,latitude ,'\n');
        
        // cout << "id " + id << endl;
        // cout << "timestamp " + timestame<< endl;
        // cout << "longitude " + longitude<< endl;
        // cout << "latitude " + latitude<< endl;
        double long_id, long_timestamp, long_longtitude, long_latitude;

        long_id = std::stod(id);
        long_timestamp = std::stod(timestame);
        long_longtitude = std::stod(longitude);
        long_latitude = std::stod(latitude);

        double p[] = {long_longtitude,long_latitude,long_timestamp};

        SpatialIndex::IShape* r = new SpatialIndex::Point(p, 3);

        point_list.push_back(r);

        count++;      
        
    }

    std:cout<< count << std::endl;
}

  
void insert_all () {

    for (int id = 0 ; id < DATASET_SIZE ;  id++){
        //  cout<< id <<endl;
        idx->index().insertData(0, nullptr,*point_list[id], id); // if error it will be throw 
    }

}


void insertion_experiment () {
    long chunk_size = 1000;
    uint64_t start_time, end_time, elapsed_time = 0;

    cout << "chunk size,average elapsed time" << endl;

    for(int i = chunk_size; i <= DATASET_SIZE; i*=2 ) {
        long start_pos = 0, end_pos = i - 1, sum = 0, count = 0; 

        cout<< i << ", ";

        while(end_pos <= DATASET_SIZE - 1) {

            // cout << "end pos" << end_pos << endl;
            
            start_time = std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now().time_since_epoch())
            .count();
        
        
            for (int id = start_pos ; id <= end_pos ;  id++){
                //  cout<< id <<endl;
                idx->index().insertData(0, nullptr,*point_list[id], id); // if error it will be throw 
            }
        
            end_time = std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now().time_since_epoch())
            .count();

            // capture result
            elapsed_time = end_time - start_time;

            // cout << elapsed_time << ", ";

            sum += elapsed_time;

            start_pos += chunk_size;
            end_pos += chunk_size;
            count += 1;
        }
        long average_elapsed = sum/count;

        cout << average_elapsed << endl;

    }
}


// export DYLD_LIBRARY_PATH=/Users/uraiwanjansong/Research/Spatiotemporal_index/lib:$DYLD_LIBRARY_PATH
// g++  src/test_insertion.cpp -L/Users/uraiwanjansong/Research/Spatiotemporal_index/lib -lspatialindex -I/Users/uraiwanjansong/Research/Spatiotemporal_index/include -o out/test/test_insertion --std=c++11