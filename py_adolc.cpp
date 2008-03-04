#include <iostream>
#include <string>
#include "py_adolc.hpp"


using namespace std;
template<class T>
void print_vec(vector<T> vec, string msg=""){
	printf("%s",msg.c_str());
	printf("[ ");
	for(int i = 0; i!= vec.size(); ++i){
		cout<<vec[i]<<" ";
	}
	printf("]\n");
}

void print_array(double *vec, int length, string msg=""){
	printf("%s",msg.c_str());
	printf("array of length %d: [ ",length);
	for(int i = 0; i!= length; ++i){
		printf("%0.5f ",vec[i]);
	}
	printf("]\n");
}


bpn::array wrapped_gradient(uint tape_tag, bpn::array &bpn_x){
	nu::check_rank(bpn_x,1);
	vector<intp> shp(nu::shape(bpn_x));
	int N = shp[0]; // lenght of x
	double* x = (double*) nu::data(bpn_x);
	double g[N];
	gradient(tape_tag, N, x, g);
	return nu::makeNum(g, N);
}

bpn::array wrapped_jacobian(int tape_tag, bpn::array &bpn_x, int M){
	nu::check_rank(bpn_x,1);
	vector<intp> shp(nu::shape(bpn_x));
	int N = shp[0];
	double* x = (double*) nu::data(bpn_x);
	double** J = myalloc2(M,N);
	jacobian(tape_tag, M, N, x, J);
	vector<intp> J_shape(2);
	J_shape[0]=M;
	J_shape[1]=N;
	return nu::makeNum( J[0], J_shape);
}





bpn::array wrapped_function(int tape_tag, int codimension, bpn::array &x0){
	if(!nu::iscontiguous(x0)){
		printf("not a contiguous array!\n");
	}
	nu::check_rank(x0,1);
	vector<intp> shp(nu::shape(x0));
	int n = shp[0]; // lenght of x0

// 	cout<<"vec of lenght "<<n<<endl;

	/* SETUP VARIABLES */
	double* dataPtr = (double*) nu::data(x0);
	vector<double> y(codimension);
	
	function(tape_tag, codimension, n, dataPtr, &y[0]);
	return nu::makeNum( &y[0], codimension);
}

bp::dict wrapped_fos_forward(short tape_tag, int codimension, int keep, bpn::array &x0, bpn::array &direction){
	nu::check_rank(x0,1);
	nu::check_rank(direction,1);

	vector<intp> shp(nu::shape(x0));
	int n = shp[0]; // lenght of x0

	/* SETUP VARIABLES */
	double* dataPtr_x0 = (double*) nu::data(x0);
	double* dataPtr_direction = (double*) nu::data(direction);
	vector<double> y(codimension);
	vector<double> directional_derivative(codimension);

	fos_forward(tape_tag, codimension, n, keep, dataPtr_x0, dataPtr_direction, &y[0], &directional_derivative[0]);

	bpn::array ret_y 	=  nu::makeNum( &y[0], codimension);
	bpn::array ret_directional_derivative 	=  nu::makeNum( &directional_derivative[0], codimension);
	
	bp::dict retvals;
	retvals["y"] = ret_y;
	retvals["directional_derivative"] = ret_directional_derivative;
	return retvals;
}


void py_tape_doc(short tape_tag, bpn::array &x, bpn::array &y ){
	nu::check_rank(x,1);
	nu::check_rank(y,1);



	double* dataPtr_x = (double*) nu::data(x);
	double* dataPtr_y = (double*) nu::data(y);
	int n = nu::shape(x)[0];
	int m = nu::shape(y)[0];

	tape_doc(tape_tag, m , n, dataPtr_x, dataPtr_y);


}


