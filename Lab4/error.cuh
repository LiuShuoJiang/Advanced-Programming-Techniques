/*
*
* Author: Shuojiang Liu
* Class: ECE6122
* Last Date Modified: November 6, 2023
* Description: a utility function to check errors
*/

#ifndef LAB4_ERROR_CUH
#define LAB4_ERROR_CUH

#include <iostream>

// an error checking utility function
#define CHECK(call)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        const cudaError_t errorCode = call;                                                                            \
        if (errorCode != cudaSuccess)                                                                                  \
        {                                                                                                              \
            std::cerr << "CUDA Error:" << std::endl;                                                                   \
            std::cerr << "    File:       " << __FILE__ << std::endl;                                                  \
            std::cerr << "    Line:       " << __LINE__ << std::endl;                                                  \
            std::cerr << "    Error code: " << errorCode << std::endl;                                                 \
            std::cerr << "    Error text: " << cudaGetErrorString(errorCode) << std::endl;                             \
            exit(1);                                                                                                   \
        }                                                                                                              \
    } while (0)

#endif // LAB4_ERROR_CUH
