/*  
 *   This example reads hyperslab from the SDS.h5 file 
 *   created by h5_write.c program into two-dimensional
 *   plane of the tree-dimensional array. 
 *   Information about dataset in the SDS.h5 file is obtained. 
 */
 
#include "hdf5.h"

#define FILE        "SDS.h5"
#define DATASETNAME "IntArray" 
#define NX_SUB  3           /* hyperslab dimensions */ 
#define NY_SUB  4 
#define NX 7           /* output buffer dimensions */ 
#define NY 7 
#define NZ  3 
#define RANK         2
#define RANK_OUT     3

main ()
{
   hid_t       file, dataset;         /* handles */
   hid_t       datatype, dataspace;   
   hid_t       memspace; 
   H5T_class_t class;                 /* data type class */
   H5T_order_t order;                 /* data order */
   size_t      size;                  /* size of the data element
                                         stored in file */ 
   size_t      dimsm[3];              /* memory space dimensions */
   size_t      dims_out[2];           /* dataset dimensions */      
   herr_t      status;                             

   int         data_out[NX][NY][NZ ]; /* output buffer */
   
   size_t      count[2];              /* size of the hyperslab in the file */
   int         offset[2];             /* hyperslab offset in the file */
   size_t      count_out[3];          /* size of the hyperslab in memory */
   int         offset_out[3];         /* hyperslab offset in memory */
   int         i, j, k, status_n, rank;

for (j = 0; j < NX; j++) {
    for (i = 0; i < NY; i++) {
        for (k = 0; k < NZ ; k++)
            data_out[j][i][k] = 0;
    }
} 
 
/*
 * Open the file and the dataset.
 */
file = H5Fopen(FILE, H5ACC_DEFAULT, H5C_DEFAULT);
dataset = H5Dopen(file, DATASETNAME);

/*
 * Get datatype and dataspace handles and then query
 * dataset class, order, size, rank and dimensions.
 */

datatype  = H5Dget_type(dataset);     /* datatype handle */ 
class     = H5Tget_class(datatype);
if (class == H5T_INTEGER) printf("Data set has INTEGER type \n");
order     = H5Tget_order(datatype);
if (order == H5T_ORDER_LE) printf("Little endian order \n");

size  = H5Tget_size(datatype);
printf(" Data size is %d \n", size);

dataspace = H5Dget_space(dataset);    /* dataspace handle */
rank      = H5Pget_ndims(dataspace);
status_n  = H5Pget_dims(dataspace, dims_out);
printf("rank %d, dimensions %d x %d \n", rank, dims_out[0], dims_out[1]);

/* 
 * Define hyperslab in the datatset. 
 */
offset[0] = 1;
offset[1] = 2;
count[0]  = NX_SUB;
count[1]  = NY_SUB;
status = H5Pset_hyperslab(dataspace, offset, count, NULL);

/*
 * Define the memory dataspace.
 */
dimsm[0] = NX;
dimsm[1] = NY;
dimsm[2] = NZ ;
memspace = H5Pcreate_simple(RANK_OUT,dimsm,NULL);   

/* 
 * Define memory hyperslab. 
 */
offset_out[0] = 3;
offset_out[1] = 0;
offset_out[2] = 0;
count_out[0]  = NX_SUB;
count_out[1]  = NY_SUB;
count_out[2]  = 1;
status = H5Pset_hyperslab(memspace, offset_out, count_out, NULL);

/*
 * Read data from hyperslab in the file into the hyperslab in 
 * memory and display.
 */
status = H5Dread(dataset, H5T_NATIVE_INT, memspace, dataspace,
                 H5C_DEFAULT, data_out);
for (j = 0; j < NX; j++) {
    for (i = 0; i < NY; i++) printf("%d ", data_out[j][i][0]);
    printf("\n");
}
                                         /*  0 0 0 0 0 0 0
                                             0 0 0 0 0 0 0
                                             0 0 0 0 0 0 0
                                             3 4 5 6 0 0 0  
                                             4 5 6 7 0 0 0
                                             5 6 7 8 0 0 0
                                             0 0 0 0 0 0 0 */

/*
 * Close/release resources.
 */
H5Tclose(datatype);
H5Dclose(dataset);
H5Pclose(dataspace);
H5Pclose(memspace);
H5Fclose(file);

}     
