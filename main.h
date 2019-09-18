//
// Created by czx on 19-9-16.
//

#ifndef CMODEL_EXAMPLE_MAIN_H
#define CMODEL_EXAMPLE_MAIN_H
#ifndef MAIN_H_

#define MAIN_H_

#include "Vpc.h"

#include <stdio.h>

#include <string>



using namespace std;



class Cmodel {

public:

    // 用户输入参数

    string inputFile;

    VpcInputFormat inputFormat;

    int inputWidth;

    int inputHeight;

    string outputFile;

    VpcOutputFormat outputFormat;

    int outputWidth;

    int outputHeight;

    int cropLeft;

    int cropRight;

    int cropTop;

    int cropBottom;



    // 接口

    int ParseInputParamter(int argc, char** argv);

    int ImageProcess();

    int DumpOutput();



    // 析构和构造

    Cmodel()

            : inputImageBuffer(nullptr),

              outputImageBuffer(nullptr),

              alignInputBuffer(nullptr),

              alignOutputBuffer(nullptr) {}



    ~Cmodel()

    {

        delete []inputImageBuffer;

        delete []outputImageBuffer;

        delete []alignInputBuffer;

        delete []alignOutputBuffer;

    }



private:

    uint8_t* inputImageBuffer;

    uint8_t* outputImageBuffer;

    uint8_t* alignInputBuffer; // 对齐后的输入buffer

    uint8_t* alignOutputBuffer; // 对齐后的输出buffer

    int alignInputWidth;

    int alignInputHeight;

    int alignOutputWidth;

    int alignOutputHeight;



    int AlignUp(int size, int alignSize)

    {

        return (size + alignSize - 1) / alignSize * alignSize;

    }



};



#endif // MAIN_H_
#endif //CMODEL_EXAMPLE_MAIN_H
