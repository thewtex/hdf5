#############################
Expected output for 'h5dump tcompound.h5'
#############################
HDF5 "tcompound.h5" {
GROUP "/" {
   DATATYPE "#6632" H5T_COMPOUND {
      H5T_STD_I32BE "int";
      H5T_IEEE_F32BE "float";
   }
   DATASET "dset1" {
      DATATYPE  H5T_COMPOUND {
         H5T_STD_I32BE "a_name";
         H5T_IEEE_F32BE "b_name";
         H5T_IEEE_F64BE "c_name";
      }
      DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
      DATA {
        (0) {
            0,
            0,
            1
         },
        (1) {
            1,
            1,
            0.5
         },
        (2) {
            2,
            4,
            0.333333
         },
        (3) {
            3,
            9,
            0.25
         },
        (4) {
            4,
            16,
            0.2
         }
      }
   }
   GROUP "group1" {
      DATASET "dset2" {
         DATATYPE  "/type1"
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
         DATA {
        (0) {
            0,
            0
         },
        (1) {
            1,
            1.1
         },
        (2) {
            2,
            2.2
         },
        (3) {
            3,
            3.3
         },
        (4) {
            4,
            4.4
         }
         }
      }
      DATASET "dset3" {
         DATATYPE  "/type2"
         DATASPACE  SIMPLE { ( 3, 6 ) / ( 3, 6 ) }
         DATA {
        (0,0) {
            [ 0, 1, 2, 3 ],
            [ 1, 2, 3, 4, 5, 6,
        %s       2, 3, 4, 5, 6, 7,
        %s       3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10 ]
         },
        (0,1) {
            [ 1, 2, 3, 4 ],
            [ 2, 3, 4, 5, 6, 7,
        %s       3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11 ]
         },
        (0,2) {
            [ 2, 3, 4, 5 ],
            [ 3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12 ]
         },
        (0,3) {
            [ 3, 4, 5, 6 ],
            [ 4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13 ]
         },
        (0,4) {
            [ 4, 5, 6, 7 ],
            [ 5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14 ]
         },
        (0,5) {
            [ 5, 6, 7, 8 ],
            [ 6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15 ]
         },
        (1,0) {
            [ 1, 2, 3, 4 ],
            [ 2, 3, 4, 5, 6, 7,
        %s       3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11 ]
         },
        (1,1) {
            [ 2, 3, 4, 5 ],
            [ 3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12 ]
         },
        (1,2) {
            [ 3, 4, 5, 6 ],
            [ 4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13 ]
         },
        (1,3) {
            [ 4, 5, 6, 7 ],
            [ 5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14 ]
         },
        (1,4) {
            [ 5, 6, 7, 8 ],
            [ 6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15 ]
         },
        (1,5) {
            [ 6, 7, 8, 9 ],
            [ 7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15,
        %s       11, 12, 13, 14, 15, 16 ]
         },
        (2,0) {
            [ 2, 3, 4, 5 ],
            [ 3, 4, 5, 6, 7, 8,
        %s       4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12 ]
         },
        (2,1) {
            [ 3, 4, 5, 6 ],
            [ 4, 5, 6, 7, 8, 9,
        %s       5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13 ]
         },
        (2,2) {
            [ 4, 5, 6, 7 ],
            [ 5, 6, 7, 8, 9, 10,
        %s       6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14 ]
         },
        (2,3) {
            [ 5, 6, 7, 8 ],
            [ 6, 7, 8, 9, 10, 11,
        %s       7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15 ]
         },
        (2,4) {
            [ 6, 7, 8, 9 ],
            [ 7, 8, 9, 10, 11, 12,
        %s       8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15,
        %s       11, 12, 13, 14, 15, 16 ]
         },
        (2,5) {
            [ 7, 8, 9, 10 ],
            [ 8, 9, 10, 11, 12, 13,
        %s       9, 10, 11, 12, 13, 14,
        %s       10, 11, 12, 13, 14, 15,
        %s       11, 12, 13, 14, 15, 16,
        %s       12, 13, 14, 15, 16, 17 ]
         }
         }
      }
      DATASET "dset4" {
         DATATYPE  "/group1/type3"
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
         DATA {
        (0) {
            0,
            3
         },
        (1) {
            1,
            4
         },
        (2) {
            2,
            5
         },
        (3) {
            3,
            6
         },
        (4) {
            4,
            7
         }
         }
      }
      DATATYPE "type3" H5T_COMPOUND {
         H5T_STD_I32BE "int";
         H5T_IEEE_F32BE "float";
      }
   }
   GROUP "group2" {
      DATASET "dset5" {
         DATATYPE  "/#6632"
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
         DATA {
        (0) {
            0,
            0
         },
        (1) {
            1,
            0.1
         },
        (2) {
            2,
            0.2
         },
        (3) {
            3,
            0.3
         },
        (4) {
            4,
            0.4
         }
         }
      }
   }
   DATATYPE "type1" H5T_COMPOUND {
      H5T_STD_I32BE "int_name";
      H5T_IEEE_F32BE "float_name";
   }
   DATATYPE "type2" H5T_COMPOUND {
      H5T_ARRAY { [4] H5T_STD_I32BE } "int_array";
      H5T_ARRAY { [5][6] H5T_IEEE_F32BE } "float_array";
   }
}
}
