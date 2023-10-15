int main (int argc, char **argv){
	// call a function in another file
	if (argc == 2) myPrintHelloMake (argv [1]);
	else myPrintHelloMake("Nobody");
	return 0;
	}
