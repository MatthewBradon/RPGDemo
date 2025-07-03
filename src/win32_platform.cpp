#pragma once

#include "platform.h"
#include "EngineLibrary.h"

#include <windows.h>

void* platformLoadDynamicLibrary(const char* dll)
{
  HMODULE result = LoadLibraryA(dll);
  ENGINE_ASSERT(result, "Failed to load dll: %s", dll);

  return result;
}

void* platformLoadDynamicFunction(void* dll, const char* funName)
{
  FARPROC proc = GetProcAddress((HMODULE)dll, funName);
  ENGINE_ASSERT(proc, "Failed to load function: %s from DLL", funName);

  return (void*)proc;
}

bool platformFreeDynamicLibrary(void* dll)
{
  BOOL freeResult = FreeLibrary((HMODULE)dll);
  ENGINE_ASSERT(freeResult, "Failed to FreeLibrary");

  return (bool)freeResult;
}