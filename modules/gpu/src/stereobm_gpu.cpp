/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other GpuMaterials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "precomp.hpp"

using namespace cv;
using namespace cv::gpu;

#if !defined (HAVE_CUDA)

cv::gpu::StereoBM_GPU::StereoBM_GPU() { throw_nogpu(); }
cv::gpu::StereoBM_GPU::StereoBM_GPU(int preset_, int ndisparities_) { throw_nogpu(); }

bool cv::gpu::StereoBM_GPU::checkIfGpuCallReasonable() { throw_nogpu(); return false; }
void cv::gpu::StereoBM_GPU::operator() ( const GpuMat& left, const GpuMat& right, GpuMat& disparity) { throw_nogpu(); }
void cv::gpu::StereoBM_GPU::operator() ( const GpuMat& left, const GpuMat& right, GpuMat& disparity, const CudaStream& stream) { throw_nogpu(); }


#else /* !defined (HAVE_CUDA) */
   
cv::gpu::StereoBM_GPU::StereoBM_GPU() : preset(BASIC_PRESET), ndisp(64)  {}
cv::gpu::StereoBM_GPU::StereoBM_GPU(int preset_, int ndisparities_) : preset(preset_), ndisp(ndisparities_) 
{
    const int max_supported_ndisp = 1 << (sizeof(unsigned char) * 8);
    CV_Assert(ndisp <= max_supported_ndisp);
    CV_Assert(ndisp % 8 == 0);
}

bool cv::gpu::StereoBM_GPU::checkIfGpuCallReasonable()
{
    if (0 == getCudaEnabledDeviceCount())
        return false;

    int device = getDevice();

    int minor, major;
    getComputeCapability(device, &major, &minor);
    int numSM = getNumberOfSMs(device);

    if (major > 1 || numSM > 16)
        return true;        

    return false;
}
  
void cv::gpu::StereoBM_GPU::operator() ( const GpuMat& left, const GpuMat& right, GpuMat& disparity)
{
    CV_DbgAssert(left.rows == right.rows && left.cols == right.cols);
    CV_DbgAssert(left.type() == CV_8UC1);
    CV_DbgAssert(right.type() == CV_8UC1);

    disparity.create(left.size(), CV_8U);
    minSSD.create(left.size(), CV_32S);

    if (preset == PREFILTER_XSOBEL)
    {
         CV_Assert(!"Not implemented");
    }   

    DevMem2D disp = disparity;
    DevMem2D_<unsigned int> mssd = minSSD;    
    impl::stereoBM_GPU(left, right, disp, ndisp, mssd);     
}

void cv::gpu::StereoBM_GPU::operator() ( const GpuMat& left, const GpuMat& right, GpuMat& disparity, const CudaStream& stream)
{
    CV_Assert(!"Not implemented");
}

#endif /* !defined (HAVE_CUDA) */