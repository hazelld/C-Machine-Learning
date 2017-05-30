#ifndef _DATA_BUILDER_H_
#define _DATA_BUILDER_H_

enum InputType {
	T_DOUBLE,
	T_STR,
};

/* Implementation of cml_data struct */
typedef struct cml_data {
	void** items;
	enum InputType* types;
	unsigned int count;
	unsigned int pos;
} cml_data;


/* struct data_pair
 *
 *	This structure contains two matrices, one for the input to the 
 *	net, and the expected output for the input. This allows for the 
 *	data to be passed around easier, and for parsing the data. 
 *
 *	Note this is not exposed at all in the public interface. 
 */
typedef struct data_pair {
	cml_data* input;
	cml_data* expected_output;
} data_pair;


/* Implementation of data_set */
typedef struct data_set {

	/* Holds all feature names */
	char** feature_names;
	enum InputType* feature_types;
	int feature_count;
	
	/* Data */
	data_pair** data;
	int count;
	
	/* Holds the two kinds of data sets; training and test. There is 
	 * currently no validation set 
	 */
	data_pair** training_set;
	data_pair** test_set;
	int training_count, test_count;
	
	/* Hold the raw data that is input */
	char** input_features;
	int input_feature_count;
	int features_specified;
	cml_data** raw_data;
	int raw_count;
} data_set;


/* Used to determine if input features have been specified */  
#define FEATURES_SPECIFIED 1
#define NO_FEATURES_SPECIFIED 0

/**
 * copy_cml_data() - Deep copy a cml_data 
 * @src: cml_data to be copied
 * @dst: Location of where cml_data should be copied
 *
 * The location that @dst points too will be allocated to hold
 * a cml_data*. This means that anything *@dst points too will be
 * lost. 
 *
 */
error_t copy_cml_data(cml_data* src, cml_data** dst);


/**
 * add_to_cml_data() - Add a value into the cml_data structure
 * @data: Data structure to put value in
 * @value: Generic value to be put into the cml_data
 *
 * Note that because the type of @value is unknown, this is a shallow
 * copy. That means whatever @value points to _must_ not be freed before
 * free_cml_data() is called, or invalid data may be used.
 *
 */
error_t add_to_cml_data(cml_data* data, void* value);


/**
 * cml_data_to_matrix() - Convert the cml_data into the matrix_t type
 * @data: Data to be converted
 * @m: Location of where to put converted matrix
 *
 * Note that *@m is allocated in this function, so careful that it does
 * not point to valid memory prior to calling this function, or it will be 
 * lost.
 */
error_t cml_data_to_matrix(cml_data* data, matrix_t** m);


/**
 * matrix_to_cml_data() - Convert matrix_t to cml_data
 * @m: Matrix to be converted
 * @data: Location of where to put converted cml_data
 *
 * Note that *@data is allocated in this function, so don't send a pointer
 * to allocated memory, or it will be lost.
 */
error_t matrix_to_cml_data(matrix_t* m, cml_data** data);

/**
 * init_data_pair() - Initialize the data pair structure
 * @input: The input data 
 * @output: The output data 
 *
 * Note the data_pair* is allocated in this function, must
 * be freed with free_data_pair().
 */
data_pair* init_data_pair(cml_data* input, cml_data* output);


/**
 * add_data_pair() - Add a data_pair into a data_set
 * @set: Data set to add the pair into
 * @pair: Pair to add into the set.
 *
 * Note the pair is shallow copied, so it must not be freed directly, must 
 * be freed using free_data_set() after this function is called.
 */
error_t add_data_pair (data_set* set, data_pair* pair);


/**
 * free_data_pair() - Data pair to free all memory
 * @pair: Data pair to free.
 */
error_t free_data_pair(data_pair* pair);


#endif
