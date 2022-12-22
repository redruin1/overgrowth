//-----------------------------------------------------------------------------
//           Name: ascontext.h
//      Developer: Wolfire Games LLC
//    Description:
//        License: Read below
//-----------------------------------------------------------------------------
//
//   Copyright 2022 Wolfire Games LLC
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//-----------------------------------------------------------------------------
#pragma once

#include <Scripting/angelscript/asmodule.h>
#include <Scripting/angelscript/asarglist.h>
#include <Scripting/angelscript/asdebugger.h>
#include <Scripting/angelscript/asprofiler.h>

#include <Internal/integer.h>
#include <Internal/path.h>

#include <Utility/fixed_array.h>

#include <angelscript.h>

#include <unordered_map>
#include <string>

class asIScriptContext;
class asIScriptEngine;
class asIScriptFunction;
class SceneGraph;
class GUI;

typedef int32_t ASFunctionHandle;

struct ASExpectedFunction {
    std::string definition;
    std::string documentation;
    asIScriptFunction* func_ptr;
    bool mandatory;
    bool unloaded;
};

struct ASData {
    SceneGraph* scenegraph;
    GUI* gui;
    ASData() : scenegraph(NULL), gui(NULL) {}
};

class ASContext {
   public:
    std::string angelscript_error_string;

    asIScriptContext* ctx;
    asIScriptEngine* engine;
    ASDebugger dbg;
    ASProfiler profiler;
    ASModule module;
    bool activate_keyboard_events;

    std::string documentation;

    GUI* gui;

   private:
    std::unordered_map<uint32_t, ASFunctionHandle> mpCallBacks;

    SceneGraph* scenegraph;

    std::string context_name;

    fixed_array<ASExpectedFunction, 64> expected_functions;

   public:
    bool LoadExpectedFunctions();
    ASFunctionHandle RegisterExpectedFunction(const std::string& function_decl, bool mandatory, const std::string& function_docs = "");
    Path current_script;

    ASContext(const char* name, const ASData& as_data);
    ~ASContext();
    void run(asIScriptFunction* func, const ASArglist* args_ptr, ASArg* return_val);
    bool LoadScript(const Path& path);

    bool HasFunction(ASFunctionHandle handle);
    bool CallScriptFunction(ASFunctionHandle handle, const ASArglist* args = NULL, ASArg* return_val = NULL);
    bool HasFunction(const std::string& function_definition);
    bool CallScriptFunction(const std::string& function_name, const ASArglist* args = NULL, ASArg* return_val = NULL, bool fail_message = true);

    int CompileScript(const Path& path);

    void RegisterEnum(const char* declaration);
    void RegisterEnumValue(const char* enum_declaration, const char* enum_val_string, int enum_val);
    void RegisterGlobalFunction(const std::string& declaration,
                                const asSFuncPtr& funcPointer,
                                asDWORD callconv,
                                const std::string& docstring = "");
    void RegisterGlobalFunctionThis(const std::string& declaration,
                                    const asSFuncPtr& funcPointer,
                                    asDWORD callconv,
                                    void* ptr,
                                    const std::string& docstring = "");
    void RegisterGlobalProperty(const std::string& declaration,
                                void* pointer,
                                const std::string& docstring = "");
    void RegisterObjectType(const std::string& obj,
                            int byteSize,
                            asDWORD flags,
                            const std::string& docstring = "");
    void RegisterFuncdef(const std::string& declaration, 
                         const std::string& docstring = "");
    void RegisterObjectMethod(const std::string& obj,
                              const std::string& declaration,
                              const asSFuncPtr& funcPointer,
                              asDWORD callConv,
                              const std::string& docstring = "");
    void RegisterObjectProperty(const std::string& obj,
                                const std::string& declaration,
                                int byteOffset,
                                const std::string& docstring = "");
    void RegisterObjectBehaviour(const std::string& obj,
                                 asEBehaviours behaviour,
                                 const std::string& declaration,
                                 const asSFuncPtr& funcPointer,
                                 asDWORD callConv,
                                 const std::string& docstring = "");
    void ResetGlobals();
    void Recompile();
    void PrintGlobalVars();
    void LoadGlobalVars();
    void SaveGlobalVars();
    std::string GetCallstack();
    void DumpCallstack(std::ostream& out);
    void LogCallstack();
    void Execute(const std::string& code, bool newContext = false);
    void* GetVarPtr(const char* name);
    void* GetArrayVarPtr(const std::string& name, int index);
    int CompileScriptFromText(const std::string& text);
    void LoadScriptFromText(const std::string& text);
    bool TypeExists(const std::string& decl);
    bool Reload();
    void ExportDocs(const char* path);
    void DocsCloseBrace();
    void ActivateKeyboardEvents();
    void DeactivateKeyboardEvents();
    bool IsKeyboardEventActivated();
    std::pair<Path, int> GetCallFile();
    std::string GetASFunctionNameFromMPState(uint32_t state);
    void RegisterMPStateCallback(uint32_t state, const std::string& callbackFunction);
    bool CallMPCallBack(uint32_t state, const std::vector<char>& data);
    bool CallMPCallBack(uint32_t state, const std::vector<unsigned int>& data);
    asIScriptEngine* GetEngine();
};

void DebugLineCallback(asIScriptContext* ctx, ASModule* asmod);
