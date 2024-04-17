cmake_minimum_required(VERSION 3.20)
project(BlockchainCoreWASM_APIProject)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
add_executable(WASM_APILib ./main.cpp)
target_compile_options(WASM_APILib PRIVATE -O2)
#,'_testInt','_print_point','_Alloc','_Del'
target_link_options(WASM_APILib PRIVATE -sNO_EXIT_RUNTIME=1 -sBINARYEN_ASYNC_COMPILATION=0 -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'] 
-sEXPORTED_FUNCTIONS=['_main','_WASMValidateBlockSignWithDel','_WASMValidateBlockSignNonDel','_WASMSignDataWithDel','_WASMSignDataNonDel','_WASMValidateBlockComplexityWithDel','_WASMValidateBlockComplexityNonDel','_WASMGeneratePrivateKey','_WASMGetPublicKeyFromPrivateWithDel','_WASMGetPublicKeyFromPrivateNonDel','_WASMInitStartBlock','_WASMMineBlockWithDel','_WASMMineBlockNonDel','_WASMFreeResult','_WASMFreePK','_WASMGetResultErrorSize','_WASMGetResultError','_WASMGetResultDataSize','_WASMGetResultData','_WASMGetPKErrorSize','_WASMGetPKError','_WASMGetFirstKeySize','_WASMGetFirstKey','_WASMGetSecondKeySize','_WASMGetSecondKey','_WASMClearBuf'])
target_include_directories(WASM_APILib PUBLIC ..)
target_link_libraries(WASM_APILib PRIVATE C_APILib)
# target_include_directories(WASM_APILib PUBLIC "${EMSDIRMY}/upstream/emscripten")
# target_link_libraries(WASM_APILib PUBLIC "${EMSDIRMY}/upstream/emscripten/cache/sysroot/lib")
#target_link_libraries(C_APILib PRIVATE CXXAPILib)
