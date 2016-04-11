extern "C" {
	#ifdef _WIN32 
	__declspec (dllexport) 
	#endif
    #pragma GCC visibility push(default)
    void* generateAnimation(char* a, int size, unsigned int& responseSize);
    #pragma GCC visibility pop
}
