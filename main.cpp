#include "main.h"

#include "dvpp_config.h"

#include "idvppapi.h"
#include <opencv2/opencv.hpp>
int AlignUp(int size, int alignSize)

{

    return (size + alignSize - 1) / alignSize * alignSize;

}
int d_aug(int imgW ,int imgH, char *InData,int resizeW,int resizeH,char *&OutData)
{
    VpcUserImageConfigure imageConfigure;
    VpcInputFormat inputFormat=INPUT_YUV420_PLANNER;
    VpcOutputFormat outputFormat=OUTPUT_YUV420SP_UV;

    printf("imgW=%d,imgH=%d,resizeW=%d,resizeH=%d \n",imgW ,imgH,resizeW,resizeH);
    int alignInputWidth = AlignUp(imgW, 128);

    int alignInputHeight = AlignUp(imgH, 16);

    int alignOutputWidth = AlignUp(resizeW, 128);

    int alignOutputHeight = AlignUp(resizeH, 16);


    uint8_t * outputImageBuffer = new uint8_t[resizeW * resizeH * 3];

    uint8_t * alignInputBuffer = new uint8_t[alignInputWidth * alignInputHeight * 3];

    uint8_t * alignOutputBuffer = new uint8_t[alignOutputWidth * alignOutputHeight * 3];

    imageConfigure.bareDataAddr = alignInputBuffer;
    imageConfigure.bareDataBufferSize = alignInputWidth * alignInputHeight * 3 / 2;

    if (InData == nullptr || outputImageBuffer == nullptr ||

        alignInputBuffer == nullptr || alignOutputBuffer == nullptr)

    {

        printf("alloc buffer fail!\n");

        return -1;

    }


    imageConfigure.widthStride = alignInputWidth; // 128对齐

    imageConfigure.heightStride = alignInputHeight; // 16对齐

    imageConfigure.inputFormat = inputFormat;

    imageConfigure.outputFormat = outputFormat;



    VpcUserRoiConfigure roiConfigure;

    roiConfigure.next = nullptr;

    imageConfigure.roiConfigure = &roiConfigure;



    VpcUserRoiInputConfigure* inputConfigure = &roiConfigure.inputConfigure;

    inputConfigure->cropArea.leftOffset = 0;

    inputConfigure->cropArea.rightOffset = imgW-1;

    inputConfigure->cropArea.upOffset = 0;

    inputConfigure->cropArea.downOffset = imgH-1;



    VpcUserRoiOutputConfigure* outputConfigure = &roiConfigure.outputConfigure;

    outputConfigure->addr = alignOutputBuffer;

    outputConfigure->widthStride = alignOutputWidth;

    outputConfigure->heightStride = alignOutputHeight;

    outputConfigure->outputArea.leftOffset = 0;

    outputConfigure->outputArea.rightOffset = resizeW - 1;

    outputConfigure->outputArea.upOffset = 0;

    outputConfigure->outputArea.downOffset = resizeH - 1;

    // 拷贝y数据

    for (auto i = 0; i < imgH; i++) {

        memcpy(alignInputBuffer + i * alignInputWidth, InData + i * imgW, imgW);

    }
    int offset = imgW * imgH;

    int alignOffset = alignInputWidth * alignInputHeight;


    int vOffset = imgW * imgH / 4;

    int alignVOffset = alignInputWidth * alignInputHeight / 4;

    for (auto i = 0; i < imgH / 2; i++) {

        memcpy(alignInputBuffer + alignOffset + i * alignInputWidth / 2,

               InData + offset + i * imgW / 2,

               imgW / 2);

        memcpy(alignInputBuffer + alignOffset + alignVOffset + i * alignInputWidth / 2,

               InData + offset + vOffset + i * imgW / 2,

               imgW / 2);

    }

    // 调用处理接口

    IDVPPAPI* dvppApi = NULL;

    dvppapi_ctl_msg dvppApiCtlMsg;

    CreateDvppApi(dvppApi);

    if(dvppApi == NULL)

    {

        printf("create dvppApi fail!\n");

        return -1;

    }

    dvppApiCtlMsg.in = (void *)(&imageConfigure);

    dvppApiCtlMsg.in_size = sizeof(imageConfigure);
    printf("call vpc dvppctl process \n");
    int32_t ret = DvppCtl(dvppApi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);

    if (ret != 0)

    {

        printf("call vpc dvppctl process faild!\n");

        DestroyDvppApi(dvppApi);

        return -1;

    }
    else
    {
        printf("call vpc dvppctl process suc!\n");

    }

    DestroyDvppApi(dvppApi);

    int outImageLen = 0;

    offset = resizeW * resizeH;

     alignOffset = alignOutputWidth * alignOutputHeight;

     outImageLen = resizeW * resizeH * 3 / 2;

     for(int i = 0; i < resizeH; i++)
     {
          memcpy(outputImageBuffer + i * resizeW, alignOutputBuffer + i * alignOutputWidth, resizeW);
     }

     for(int i = 0; i < resizeH / 2; i++)
     {
          memcpy(outputImageBuffer + i * resizeW + offset, alignOutputBuffer + i * alignOutputWidth + alignOffset, resizeW);
     }
    OutData=(char *)outputImageBuffer;
    delete [] alignInputBuffer;
    delete [] alignOutputBuffer;
    return 0;

}

int d_aug_test(int argc, char** argv)
{
    if (argc != 7) {

        printf("input args not right.It should be like this:\n");

        printf("./cmodel_example ./1920x1080.nv12 1920 1080 ./outputImage.nv12 960 540 \n");

        return -1;

    }
    string inputFile;


    int inputWidth;

    int inputHeight;

    string outputFile;


    int outputWidth;

    int outputHeight;

    inputFile = argv[1];


    inputWidth = atoi(argv[2]);

    inputHeight = atoi(argv[3]);

    outputFile = argv[4];


    outputWidth = atoi(argv[5]);

    outputHeight = atoi(argv[6]);
    // 读取数据

    FILE* fp = fopen(inputFile.c_str(), "rb+");

    if (fp == nullptr) {

        printf("fopen input file(%s) fail!\n", inputFile.c_str());

        return -1;

    }

    fseek(fp, 0, SEEK_END);

    int fileLen = ftell(fp);

 //   if (fileLen > inputWidth * inputHeight * 3) {
//
 //       printf("input file size too large!\n");
 //       return -1;

  //  }

    fseek(fp, 0, SEEK_SET);
    uint8_t *inputImageBuffer = new uint8_t[fileLen];
    fread(inputImageBuffer, 1, fileLen, fp);

    fclose(fp);

    cv::Mat src,src2;
    src = cv::imread(inputFile);
    cv::Mat yuv(inputHeight*3/2,inputWidth,CV_8UC1);
    src2=src(cv::Rect(0,0,src.cols/2*2,src.rows/2*2));
    cv::cvtColor(src2, yuv, cv::COLOR_BGR2YUV_I420);
    inputWidth=src2.cols;
    inputHeight=src2.rows;
    char *OutData;
    //d_aug(inputWidth ,inputHeight, (char *)inputImageBuffer,outputWidth,outputHeight,OutData);  //input nv12 ouput nv12
    d_aug(inputWidth ,inputHeight, (char *)yuv.data,outputWidth,outputHeight,OutData);  //input nv12 ouput nv12

    fp = fopen(outputFile.c_str(), "wb");

    if (fp == nullptr) {
        delete [] OutData;
        printf("fopen output file(%s) fail!\n", outputFile.c_str());

        return -1;

    }

   // fwrite(OutData, 1, outputWidth*outputHeight*3/2, fp);
    cv::Mat imgout;
    cv::Mat yuv_out(outputHeight*3/2,outputWidth,CV_8UC1,OutData);
    cv::cvtColor(yuv_out, imgout, cv::COLOR_YUV2BGR_NV12);
    cv::imwrite(outputFile,imgout);
    delete [] OutData;
    fclose(fp);
}
#if 1
int main(int argc, char** argv)
{
    d_aug_test(argc, argv);
}
#endif
#if 0
// ./cmodel_test 输入文件 输入文件格式 输入文件宽 输入文件高 输出文件 输出文件格式 输出文件宽 输出文件高 抠图左偏移 抠图右偏移 抠图上偏移 抠图下偏移

int main(int argc, char** argv)

{

    Cmodel model;

    // 解析输入参数

    int ret = model.ParseInputParamter(argc, argv);

    if (ret != 0) {

        printf("input paramter error, please check carefully!\n");

        return -1;

    }



    // 调用软件Vpc，进行抠图缩放处理

    ret = model.ImageProcess();

    if (ret != 0) {

        printf("image process error, check for errors!\n");

        return -1;

    }



    // 保存输出结果

    ret = model.DumpOutput();

    if (ret == 0) {

        printf("test success!\n");

    }

    return ret;

}

#endif

int Cmodel::ParseInputParamter(int argc, char** argv)

{

    // 判断输入参数个数是否正确

    if (argc != 13) {

        printf("input args not right.It should be like this:\n");

        printf("./cmodel_test inputImage.nv12 1 1920 1080 outputImage.rgb 0 1280 720 20 1779 10 799\n");

        return -1;

    }

    inputFile = argv[1];

    inputFormat = (VpcInputFormat)(atoi(argv[2]));

    inputWidth = atoi(argv[3]);

    inputHeight = atoi(argv[4]);

    outputFile = argv[5];

    outputFormat = (VpcOutputFormat)(atoi(argv[6]));

    outputWidth = atoi(argv[7]);

    outputHeight = atoi(argv[8]);

    cropLeft = atoi(argv[9]);

    cropRight = atoi(argv[10]);

    cropTop = atoi(argv[11]);

    cropBottom = atoi(argv[12]);



    // 打印传入信息

    printf("hello this is your input information, please check:\n");

    printf("input file:%s\n", inputFile.c_str());

    printf("input format:%d\n", inputFormat);

    printf("input width:%d\n", inputWidth);

    printf("input height:%d\n", inputHeight);

    printf("output file:%s\n", outputFile.c_str());

    printf("output format:%d\n", outputFormat);

    printf("output width:%d\n", outputWidth);

    printf("output height:%d\n", outputHeight);

    printf("crop left:%d\n", cropLeft);

    printf("crop right:%d\n", cropRight);

    printf("crop top:%d\n", cropTop);

    printf("crop bottom:%d\n", cropBottom);



    // 分配buffer

    alignInputWidth = AlignUp(inputWidth, 128);

    alignInputHeight = AlignUp(inputHeight, 16);

    alignOutputWidth = AlignUp(outputWidth, 128);

    alignOutputHeight = AlignUp(outputHeight, 16);

    inputImageBuffer = new uint8_t[inputWidth * inputHeight * 3];

    outputImageBuffer = new uint8_t[outputWidth * outputHeight * 3];

    alignInputBuffer = new uint8_t[alignInputWidth * alignInputHeight * 3];

    alignOutputBuffer = new uint8_t[alignOutputWidth * alignOutputHeight * 3];

    if (inputImageBuffer == nullptr || outputImageBuffer == nullptr ||

        alignInputBuffer == nullptr || alignOutputBuffer == nullptr)

    {

        printf("alloc buffer fail!\n");

        return -1;

    }



    // 读取数据

    FILE* fp = fopen(inputFile.c_str(), "rb+");

    if (fp == nullptr) {

        printf("fopen input file(%s) fail!\n", inputFile.c_str());

        return -1;

    }

    fseek(fp, 0, SEEK_END);

    int fileLen = ftell(fp);

    if (fileLen > inputWidth * inputHeight * 3) {

        printf("input file size too large!\n");

        return -1;

    }

    fseek(fp, 0, SEEK_SET);

    fread(inputImageBuffer, 1, fileLen, fp);

    fclose(fp);

    // 拷贝y数据

    for (auto i = 0; i < inputHeight; i++) {

        memcpy(alignInputBuffer + i * alignInputWidth, inputImageBuffer + i * inputWidth, inputWidth);

    }

    int offset = inputWidth * inputHeight;

    int alignOffset = alignInputWidth * alignInputHeight;

    int vOffset = 0;

    int alignVOffset = 0;

    // 拷贝uv数据，不同格式，uv数据量不同

    switch(inputFormat) {

        case 0: // yuv400

            break;

        case 1: // nv12

        case 2: // nv21

            for (auto i = 0; i < inputHeight / 2; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth,

                       inputImageBuffer + offset + i * inputWidth,

                       inputWidth);

            }

            break;

        case 3: // 422sp-uv

        case 4: // 422sp-vu

            for (auto i = 0; i < inputHeight; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth,

                       inputImageBuffer + offset + i * inputWidth,

                       inputWidth);

            }

            break;

        case 5: // 444sp-uv

        case 6: // 444sp-vu

            for (auto i = 0; i < inputHeight; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth * 2,

                       inputImageBuffer + offset + i * inputWidth * 2,

                       inputWidth * 2);

            }

            break;

        case 7: // 420planer

            vOffset = inputWidth * inputHeight / 4;

            alignVOffset = alignInputWidth * alignInputHeight / 4;

            for (auto i = 0; i < inputHeight / 2; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth / 2,

                       inputImageBuffer + offset + i * inputWidth / 2,

                       inputWidth / 2);

                memcpy(alignInputBuffer + alignOffset + alignVOffset + i * alignInputWidth / 2,

                       inputImageBuffer + offset + vOffset + i * inputWidth / 2,

                       inputWidth / 2);

            }

            break;

        case 8: // 422planer

            vOffset = inputWidth * inputHeight / 2;

            alignVOffset = alignInputWidth * alignInputHeight / 4;

            for (auto i = 0; i < inputHeight; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth / 2,

                       inputImageBuffer + offset + i * inputWidth / 2,

                       inputWidth / 2);

                memcpy(alignInputBuffer + alignOffset + alignVOffset + i * alignInputWidth / 2,

                       inputImageBuffer + offset + vOffset + i * inputWidth / 2,

                       inputWidth / 2);

            }

            break;

        case 9: // 444planer

            vOffset = inputWidth * inputHeight;

            alignVOffset = alignInputWidth * alignInputHeight / 4;

            for (auto i = 0; i < inputHeight; i++) {

                memcpy(alignInputBuffer + alignOffset + i * alignInputWidth,

                       inputImageBuffer + offset + i * inputWidth,

                       inputWidth);

                memcpy(alignInputBuffer + alignOffset + alignVOffset + i * alignInputWidth,

                       inputImageBuffer + offset + vOffset + i * inputWidth,

                       inputWidth);

            }

            break;

    }

    return 0;

}



int Cmodel::ImageProcess()

{

    // 配置

    VpcUserImageConfigure imageConfigure;

    imageConfigure.bareDataAddr = alignInputBuffer;

    switch(inputFormat) {

        case 0: // yuv400

            break;

        case 1: // nv12

        case 2: // nv21

        case 7: // 420planer

            imageConfigure.bareDataBufferSize = alignInputWidth * alignInputHeight * 3 / 2;

            break;

        case 3: // 422sp-uv

        case 4: // 422sp-vu

        case 8: // 422planer

            imageConfigure.bareDataBufferSize = alignInputWidth * alignInputHeight * 2;

            break;

        case 5: // 444sp-uv

        case 6: // 444sp-vu

        case 9: // 444planer

            imageConfigure.bareDataBufferSize = alignInputWidth * alignInputHeight * 3;

            break;

    }

    imageConfigure.widthStride = alignInputWidth; // 128对齐

    imageConfigure.heightStride = alignInputHeight; // 16对齐

    imageConfigure.inputFormat = inputFormat;

    imageConfigure.outputFormat = outputFormat;



    VpcUserRoiConfigure roiConfigure;

    roiConfigure.next = nullptr;

    imageConfigure.roiConfigure = &roiConfigure;



    VpcUserRoiInputConfigure* inputConfigure = &roiConfigure.inputConfigure;

    inputConfigure->cropArea.leftOffset = cropLeft;

    inputConfigure->cropArea.rightOffset = cropRight;

    inputConfigure->cropArea.upOffset = cropTop;

    inputConfigure->cropArea.downOffset = cropBottom;



    VpcUserRoiOutputConfigure* outputConfigure = &roiConfigure.outputConfigure;

    outputConfigure->addr = alignOutputBuffer;

    outputConfigure->widthStride = alignOutputWidth;

    outputConfigure->heightStride = alignOutputHeight;

    outputConfigure->outputArea.leftOffset = 0;

    outputConfigure->outputArea.rightOffset = outputWidth - 1;

    outputConfigure->outputArea.upOffset = 0;

    outputConfigure->outputArea.downOffset = outputHeight - 1;



    // 调用处理接口

    IDVPPAPI* dvppApi = NULL;

    dvppapi_ctl_msg dvppApiCtlMsg;

    CreateDvppApi(dvppApi);

    if(dvppApi == NULL)

    {

        printf("create dvppApi fail!\n");

        return -1;

    }

    dvppApiCtlMsg.in = (void *)(&imageConfigure);

    dvppApiCtlMsg.in_size = sizeof(imageConfigure);

    int32_t ret = DvppCtl(dvppApi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);

    if (ret != 0)

    {

        printf("call vpc dvppctl process faild!\n");

        DestroyDvppApi(dvppApi);

        return -1;

    }

    DestroyDvppApi(dvppApi);

    return 0;

}



int Cmodel::DumpOutput()

{

    int outImageLen = 0;

    int offset = outputWidth * outputHeight;

    int alignOffset = alignOutputWidth * alignOutputHeight;

    if (outputFormat < OUTPUT_RGB_PACKED) {

        outImageLen = outputWidth * outputHeight * 3 / 2;

        for(int i = 0; i < outputHeight; i++)

        {

            memcpy(outputImageBuffer + i * outputWidth, alignOutputBuffer + i * alignOutputWidth, outputWidth);

        }

        for(int i = 0; i < outputHeight / 2; i++)

        {

            memcpy(outputImageBuffer + i * outputWidth + offset,

                   alignOutputBuffer + i * alignOutputWidth + alignOffset,

                   outputWidth);

        }

    } else {

        outImageLen = outputWidth * outputHeight * 3;

        for(int i = 0; i < outputHeight; i++)

        {

            memcpy(outputImageBuffer + i * outputWidth * 3,

                   alignOutputBuffer + i * alignOutputWidth * 3,

                   outputWidth * 3);

        }

    }



    FILE* fp = fopen(outputFile.c_str(), "wb");

    if (fp == nullptr) {

        printf("fopen output file(%s) fail!\n", outputFile.c_str());

        return -1;

    }

    fwrite(outputImageBuffer, 1, outImageLen, fp);

    fclose(fp);

    return 0;

}