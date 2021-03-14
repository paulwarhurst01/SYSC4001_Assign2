void copy_array(char arrayin[7], char arrayout[7], int debug){
    /*
    Copies one array into another element by element
    */
    if(debug == 1) printf("copying array into shared memory\n");
    for(int i = 0; i < 7; i++){
        arrayout[i] = arrayin[i];
    }
}

void print_array(char array[7]){
    /* 
    Prints an array place by place, tab separated
    */
    for(int i = 0; i < 7; i++){
        printf("%c, ", array[i]);
    }
    printf("\n");
}

void sort_array(char array[7], int debug, int lower_limit, int upper_limit){
    for(int i = lower_limit; i <= upper_limit; i++){
        if((isalpha(array[i]) == 0) && (isalpha(array[i+1]) != 0)){
            if(debug == 1){
                printf("Making switch...\nSwitched array: ");
                print_array(array);
            }
            char temp = array[i];
            array[i] = array[i + 1];
            array[i+1] = temp;
        }
    }
}