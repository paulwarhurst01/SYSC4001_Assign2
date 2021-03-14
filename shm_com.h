struct shared_use_st{
    /*
    Structure to be "masked" onto shared memory
    -> Includes 7 character array for sorting
    -> Includes an array that holds whether sub-
       array managed by a process is in order
    */
    char filtered[7];
    int needs_switch;
};