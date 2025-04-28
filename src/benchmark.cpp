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
double x_min, y_min, z_min, x_max, y_max, z_max;

IStorageManager* mem_storege = StorageManager::createNewMemoryStorageManager();
Index* idx;

void load_data(string file_path);

void set_up_Rtree(int variant);

void insert_all (); 

void insertion_experiment (long chunk_size);
void deletion_experiment (long chunk_size);
void pointquery_experiment (long chunk_size) ;
void rangequery_experiment (int query_size) ;

int main (int argc, char* argv[]) {
    load_data("datasets/t_drive_merged_filtered_shuffled.csv");
    set_up_Rtree(atoi(argv[2])); //0 = linear, 1 = quardatic, 2 = star

    switch (atoi(argv[1])) {
        case 1:
            // Insertion
            insertion_experiment(atoi(argv[3]));
            break;
        case 2:
            // Deletion
            insert_all();
            deletion_experiment(atoi(argv[3]));
            break;
        case 3:
            // Point Query
            insert_all();
            pointquery_experiment(atoi(argv[3]));
            break;
        case 4:
            // Range Query 
            insert_all();
            rangequery_experiment(atoi(argv[3]));
            break;
        default:
            cout << "Please choose between 1-4" << endl;
            break;
    }

    
    
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

        
        double long_id, long_timestamp, long_longtitude, long_latitude;

        long_id = std::stod(id);
        long_timestamp = std::stod(timestame);
        long_longtitude = std::stod(longitude);
        long_latitude = std::stod(latitude);

                
        if (i == 0) {
            x_min = long_longtitude; 
            x_max = long_longtitude;
            y_min = long_latitude; 
            y_max = long_latitude;
            z_min = long_timestamp;
            z_max = long_timestamp;
        } else {
            if (long_longtitude < x_min ) {
                x_min = long_longtitude;
            }
            if (long_latitude < y_min ) {
                y_min = long_latitude;
            }
            if (long_timestamp < z_min ) {
                z_min = long_timestamp;
            }
            if (long_longtitude > x_max ) {
                x_max = long_longtitude;
            }
            if (long_latitude > y_max ) {
                y_max = long_latitude;
            }
            if (long_timestamp > z_max ) {
                z_max = long_timestamp;
            }
        }


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


void insertion_experiment (long chunk_size) {
    uint64_t start_time, end_time, elapsed_time = 0;

    long start_pos = 0, end_pos = chunk_size - 1, sum = 0, count = 0; 


    while(end_pos <= DATASET_SIZE - 1) {
        
        start_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();


        for (int id = start_pos ; id <= end_pos ;  id++){
            idx->index().insertData(0, nullptr,*point_list[id], id); 
        }

        end_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();

        elapsed_time = end_time - start_time;

        sum += elapsed_time;

        start_pos += chunk_size;
        end_pos += chunk_size;
        count += 1;
    }

    double average_elapsed = (double)sum/count;

    printf("%ld, %.2f\n", chunk_size, average_elapsed);  
}


void deletion_experiment (long chunk_size) {

    uint64_t start_time, end_time, elapsed_time = 0;

    long start_pos = 0, end_pos = chunk_size - 1, sum = 0, count = 0; 

    while(end_pos <= DATASET_SIZE - 1) {
        
        start_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();
    
    
        for (int id = start_pos ; id <= end_pos ;  id++){

            idx->index().deleteData(*point_list[id], id); 
        }
    
        end_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();

        // capture result
        elapsed_time = end_time - start_time;

        sum += elapsed_time;

        start_pos += chunk_size;
        end_pos += chunk_size;
        count += 1;
    }
    double average_elapsed = (double)sum/count;

    printf("%ld, %.2f\n", chunk_size, average_elapsed);  

}


void pointquery_experiment (long chunk_size) {
    uint64_t start_time, end_time, elapsed_time = 0;

    long start_pos = 0, end_pos = chunk_size - 1, sum = 0, count = 0; 

    while(end_pos <= DATASET_SIZE - 1) {


        ObjVisitor* visitor = new ObjVisitor;

        start_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();
    
        for (int id = start_pos ; id <= end_pos ;  id++){
            idx->index().pointLocationQuery(*point_list[id], *visitor); 
        }
    
        end_time = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::system_clock::now().time_since_epoch())
        .count();

        elapsed_time = end_time - start_time;

        sum += elapsed_time;

        start_pos += chunk_size;
        end_pos += chunk_size;
        count += 1;
        int nResultCount = visitor->GetResultCount();

        delete visitor;
    }
    double average_elapsed = (double)sum/count;

    printf("%ld, %.2f\n", chunk_size, average_elapsed);  
}


void rangequery_experiment (int query_size) {
    uint64_t start_time, end_time, elapsed_time = 0;

    long sum = 0, count = 0; 
    double stride = 0.5;
    double box_size = (double)query_size/100;

    double z_length = (z_max - z_min) * box_size;
    double y_length = (y_max - y_min) * box_size;
    double x_length = (x_max - x_min) * box_size;

    double z_stride = z_length * stride;
    double y_stride = y_length * stride;
    double x_stride = x_length * stride;


    double z_start = z_min;
    double z_end = z_min + z_length;

    int increase = (int)(stride*query_size);
    for (int i = query_size; i<=100; i+=increase) {
        double y_start = y_min;
        double y_end = y_min + y_length;

        for (int j = query_size; j<=100; j+=increase) {
            double x_start = x_min;
            double x_end = x_min + x_length;

            for (int k = query_size; k<=100; k+=increase) {

                // create a box_query
                double p_min[] = {x_start,y_start,z_start};
                double p_max[] = {x_end,y_end,z_end};
                // double p_min[] = {x_min,y_min,z_min};
                // double p_max[] = {x_max,y_max,z_max};

                SpatialIndex::Point* point_min = new SpatialIndex::Point(p_min, 3);
                SpatialIndex::Point* point_max = new SpatialIndex::Point(p_max, 3);

                SpatialIndex::Region* MBR = new SpatialIndex::Region(*point_min, *point_max);

                ObjVisitor* visitor = new ObjVisitor;

                start_time = std::chrono::duration_cast<std::chrono::microseconds>
                (std::chrono::system_clock::now().time_since_epoch())
                .count();
                
                idx->index().intersectsWithQuery(*MBR, *visitor); 

                end_time = std::chrono::duration_cast<std::chrono::microseconds>
                (std::chrono::system_clock::now().time_since_epoch())
                .count();


                elapsed_time = end_time - start_time;

                int nResultCount = visitor->GetResultCount();

                delete visitor;

                printf("%.6f, %.6f, %.2f,", x_start, y_start, z_start);
                printf("%.6f, %.6f, %.2f,", x_end, y_end, z_end);
                printf("%d, %d, %llu\n", query_size, nResultCount, elapsed_time);  

                x_start += x_stride;
                x_end += x_stride;
            }

            y_start += y_stride;
            y_end += y_stride;
        }

        z_start += z_stride;
        z_end += z_stride;
    }

}


// export DYLD_LIBRARY_PATH=/Users/uraiwanjansong/Research/Spatiotemporal_index/lib:$DYLD_LIBRARY_PATH
// g++  src/test_insertion.cpp -L/Users/uraiwanjansong/Research/Spatiotemporal_index/lib -lspatialindex -I/Users/uraiwanjansong/Research/Spatiotemporal_index/include -o out/test/test_insertion --std=c++11