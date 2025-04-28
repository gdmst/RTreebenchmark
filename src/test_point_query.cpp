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
vector <SpatialIndex::Point*> point_list ;
// long DATASET_SIZE = 17622266;
long DATASET_SIZE = 6000;
IStorageManager* mem_storege = StorageManager::createNewMemoryStorageManager();

Index* idx;

void load_data(string file_path);

void set_up_Rtree(int variant);

void insert_all (); 

void insertion_experiment (long chunk_size);
void deletion_experiment (long chunk_size);
void pointquery_experiment (long chunk_size) ;

int main (int argc, char* argv[]) {
    load_data("datasets/t_drive_merged_filtered_shuffled.csv");
    // argv[1] = 3 --> point query
    set_up_Rtree(atoi(argv[2])); //0 = linear, 1 = quardatic, 2 = star

    insert_all();
    pointquery_experiment(atoi(argv[3]));    
}


void set_up_Rtree(int variant) {
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

    // cout << idx->GetProperties() << endl;

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

        SpatialIndex::Point* r = new SpatialIndex::Point(p, 3);

        point_list.push_back(r);

        count++;      
        
    }

    // std:cout<< count << std::endl;
}

  
void insert_all () {

    for (int id = 0 ; id < DATASET_SIZE ;  id++){
        idx->index().insertData(0, nullptr,*point_list[id], id); 
    }
}



void pointquery_experiment (long chunk_size) {

    // double p[] = {116.45940,39.86983,1201993857}; // not found
    double p[] = {116.32060,39.90038,1202306255}; // found
    // double p[] = {116.32060,39.90038,1202306255}; // not found

    SpatialIndex::Point* r = new SpatialIndex::Point(p, 3);

    ObjVisitor* visitor = new ObjVisitor;
    idx->index().pointLocationQuery(*r, *visitor);

    int nResultCount = visitor->GetResultCount();

    delete visitor;
    delete r;
    cout << "found " << nResultCount;
}