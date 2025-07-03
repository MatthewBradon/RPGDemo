
#pragma  once



void* platformLoadDynamicLibrary(const char* dll);
void* platformLoadDynamicFunction(void* dll, const char* funName);
bool platformFreeDynamicLibrary(void* dll);
