#include "LSH.h"
#include "LSH_Functions.h"
#include "Helper_Functions.h"
#include "LSH_DataTypes.h"

using namespace std;

template void compute_unassigned<double>(vector<vector<double>>* ,vector<vector<double>>* , vector<int>*, int, double**, int**);
template void compute_unassigned<double*>(vector<vector<double*>>* ,vector<vector<double*>>* , vector<int>*, int, double**, int**);

void lsh_datatype(vector<vector<double>>* lsh_dataset, vector<vector<double>>* lsh_searchset, vector<int>* centroid_ids, int Grids, int k, int L, vector<int>** clusters){

    int data_size = lsh_dataset->size();

    /* Arrays for results */
    double *min_distance = new double[data_size];
    int *nearest_centroid = new int[data_size];
    int unassigned_vectors = data_size;

    /* Initialize arrays */
    for (int i = 0; i < data_size; i++) {
        min_distance[i] = DBL_MAX;
        nearest_centroid[i] = -1;
    }

    /* ---- LSH model ---- */
    double w = 4*compute_window(lsh_dataset);
    LSH <double>* model = new LSH <double> (k, L, w);
    model->fit(lsh_dataset);

    int iterations = 0;
    /* LSH computations until all vectors are assigned to centroid or LSH_ITERATIONS reached */
    while((unassigned_vectors > k) && (iterations < LSH_ITERATIONS)){
        model->evaluate_clusters(lsh_searchset, centroid_ids, &min_distance, &nearest_centroid, &unassigned_vectors);
        iterations++;
    }

    /* If there are any unassigned vectors, assign them using brute force */
    if(unassigned_vectors > 0){
        compute_unassigned(lsh_dataset, lsh_searchset, centroid_ids, data_size, &min_distance, &nearest_centroid);
    }

    delete (model);

    /* Fill every cluster with its assigned vectors */
    for (int i = 0; i < data_size; i++){
        if (min_distance[i] == 0) continue;
        clusters[nearest_centroid[i]]->push_back(i);
    }

    /* Clean Pointers */
    delete[] min_distance;
    delete[] nearest_centroid;

}

void lsh_datatype(vector<vector<double*>>* lsh_dataset, vector<vector<double*>>* lsh_searchset, vector<int>* centroid_ids, int Grids, int k, int L, vector<int>** clusters){

    /* default 2D curves */
    int d = 2;
    double delta = 0.00006;

    /* Vectors to be used in Grid Vectorization */
    vector<vector<double>> data_vectored_curves;
    vector<vector<double>> search_vectored_curves;

    int data_size = lsh_dataset->size();

    /* Arrays for results */
    double *min_distance = new double[data_size];
    int *nearest_centroid = new int[data_size];
    int unassigned_curves = data_size;

    /* Initialize arrays */
    for (int i = 0; i < data_size; i++) {
        min_distance[i] = DBL_MAX;
        nearest_centroid[i] = -1;
    }

    /* Loop for all Grids */
    for (int i = 0; i < Grids; i++) {

        /* Vectorization */
        Grid_Vectorization(delta, d, lsh_dataset, lsh_searchset, &data_vectored_curves, &search_vectored_curves);

        /* ---- LSH model ---- */
        double w = 4 * compute_window(&data_vectored_curves);
        LSH<double>* model = new LSH<double>(k, L, w);
        model->fit(&data_vectored_curves);

        int iterations = 0;
        /* LSH computations until all curves are assigned to centroid or LSH_ITERATIONS reached */
        while((unassigned_curves > k) && (iterations < LSH_ITERATIONS)){
            model->evaluate_clusters(&search_vectored_curves, centroid_ids, &min_distance, &nearest_centroid, &unassigned_curves);
            iterations++;
        }

        delete (model);

        /* Clean Vectors */
        vector<vector<double>>().swap(data_vectored_curves);
        vector<vector<double>>().swap(search_vectored_curves);
    }

    /* If there are any unassigned curves, assign them using brute force */
    if(unassigned_curves > 0){
        compute_unassigned(lsh_dataset, lsh_searchset, centroid_ids, data_size, &min_distance, &nearest_centroid);
    }

    /* Fill every cluster with its assigned curves */
    for (int i = 0; i < data_size; i++){
        if (min_distance[i] == 0) continue;
        clusters[nearest_centroid[i]]->push_back(i);
    }

    /* Clean Pointers */
    delete[] min_distance;
    delete[] nearest_centroid;

}


/* Function to assign unassigned elements using Brute Force */

template <typename Point>
void compute_unassigned(vector<vector<Point>>* lsh_dataset,vector<vector<Point>>* lsh_searchset, vector<int>* centroid_ids, int data_size, double** min_distance, int** nearest_centroid){

    /* default metric L1 Manhattan */
    int Metric = 1;
    double curr_dist;
    /* For every element in dataset */
    for(int i = 0; i < data_size; i++){
        /* If element is not assigned to centroid */
        if((*nearest_centroid)[i] == -1){
            /* For every centroid calculate distance to element and assign to closest */
            for(int j = 0; j < lsh_searchset->size(); j++){
                curr_dist = dist(&lsh_dataset->at(i) ,&lsh_searchset->at(j), lsh_dataset->at(0).size(), Metric);
                if (curr_dist < (*min_distance)[i]) {
                    (*min_distance)[i] = curr_dist;
                    (*nearest_centroid)[i] = j;
                }
            }
        }
    }
}