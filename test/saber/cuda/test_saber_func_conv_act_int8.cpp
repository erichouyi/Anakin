
#include "core/context.h"
#include "funcs/conv_act.h"
#include "test_saber_func_NV.h"
#include "tensor_op.h"
#include "saber_types.h"
#include <vector>

#if 0
using namespace anakin::saber;

typedef Tensor<X86, AK_FLOAT, NCHW> TensorHf4;
typedef Tensor<NV, AK_FLOAT, NCHW> TensorDf4;

typedef Tensor<X86, AK_INT8, NCHW> TensorHI84;
typedef Tensor<NV, AK_INT8, NCHW> TensorDI84;

typedef Tensor<X86, AK_INT8, NCHW_C4> TensorHC4;
typedef Tensor<NV, AK_INT8, NCHW_C4> TensorDC4;

TEST(TestSaberFuncNV, test_func_constructor) {

    typedef TargetWrapper<NV> API;
    typename API::event_t event;
    API::create_event(event);

    typedef TargetWrapper<X86> X86_API;
    typedef TargetWrapper<NV> NV_API;

    Context<NV> ctx2;

    NV_API::record_event(event, ctx2.get_compute_stream());

    int group = 1;
    int pad_h = 1;
    int pad_w = 1;
    int stride_h = 1;
    int stride_w = 1;
    int dilation_h = 1;
    int dilation_w = 1;

    int kernel_h = 3;
    int kernel_w = 3;
    int out_channels = 4;

    int img_num = 1;
    int in_channels = 4;
    int img_h = 8;
    int img_w = 8;

    bool bias_term = true;

    LOG(INFO) << "conv param: ";
    LOG(INFO) << " img_num = " << img_num;
    LOG(INFO) << " in_channels = " << in_channels;
    LOG(INFO) << " img_h = " << img_h;
    LOG(INFO) << " img_w = " << img_w;
    LOG(INFO) << " group = " << group;
    LOG(INFO) << " pad_h = " << pad_h;
    LOG(INFO) << " pad_w = " << pad_w;
    LOG(INFO) << " stride_h = " << stride_h;
    LOG(INFO) << " stride_w = " << stride_w;
    LOG(INFO) << " dilation_h = " << dilation_h;
    LOG(INFO) << " dilation_w = " << dilation_w;
    LOG(INFO) << " kernel_h = " << kernel_h;
    LOG(INFO) << " kernel_w = " << kernel_w;
    LOG(INFO) << " out_channels = " << out_channels;

    Shape img_s(img_num, in_channels, img_h, img_w);
    Shape weights_s(out_channels, in_channels, kernel_h, kernel_w);
    Shape bias_s(1, out_channels, 1, 1);

    TensorHf4 img_host;
    TensorDf4 img_dev;

    img_host.re_alloc(img_s);
    img_dev.re_alloc(img_s);

    for (int i = 0; i < img_host.size(); ++i) {
        img_host.mutable_data()[i] = 2;
    }

    img_dev.copy_from(img_host);

    TensorHI84 weights_host;
    TensorDI84 weights_dev;

    weights_host.re_alloc(weights_s);
    weights_dev.re_alloc(weights_s);

    fill_tensor_host_const(weights_host, 1);
    weights_dev.copy_from(weights_host);

    TensorHf4 bias_host;
    TensorDf4 bias_dev;

    if (bias_term) {
        bias_host.re_alloc(bias_s);
        bias_dev.re_alloc(bias_s);

        fill_tensor_host_const(bias_host, 1);
        bias_dev.copy_from(bias_host);
    }

    TensorHf4 output_host;
    TensorDf4 output_dev;

    // start Reshape & doInfer
    Context<NV> ctx1(0, 1, 1);
    ConvParam<TensorDI84> conv_param(group, pad_h, pad_w,
                                     stride_h, stride_w,
                                     dilation_h, dilation_w,
                                     &weights_dev, &bias_dev);

    ActivationParam<TensorDI84> act_param(Active_relu);
    ConvActiveParam<TensorDI84> conv_act_param(conv_param, act_param);

    std::vector<TensorDf4*> input;
    std::vector<TensorDf4*> output;

    input.push_back(&img_dev);
    output.push_back(&output_dev);

    ConvAct<NV, AK_INT8, AK_FLOAT, AK_FLOAT, NCHW> conv;
    conv.compute_output_shape(input, output, conv_act_param);

    output_dev.re_alloc(output[0]->shape());
    output_host.re_alloc(output[0]->shape());

    LOG(INFO) << "regular start with group = " << group;
    // init assume output tensor has been reshpaed by user.
    conv.init(input, output, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);

    conv(input, output, conv_act_param, ctx1);

    cudaStream_t cuda_stream = ctx1.get_compute_stream();
    output[0]->record_event(cuda_stream);

    output_dev.sync();

    print_tensor_device(output_dev);
    cudaDeviceSynchronize();

    CUDA_CHECK(cudaPeekAtLastError());
}

TEST(TestSaberFuncNV, test_int8_shared) {

    int group = 1;
    int pad_h = 1;
    int pad_w = 1;
    int stride_h = 1;
    int stride_w = 1;
    int dilation_h = 1;
    int dilation_w = 1;

    int kernel_h = 3;
    int kernel_w = 3;
    int out_channels = 4;

    int img_num = 1;
    int in_channels = 4;
    int img_h = 8;
    int img_w = 8;

    bool bias_term = true;

    LOG(INFO) << "conv param: ";
    LOG(INFO) << " img_num = " << img_num;
    LOG(INFO) << " in_channels = " << in_channels;
    LOG(INFO) << " img_h = " << img_h;
    LOG(INFO) << " img_w = " << img_w;
    LOG(INFO) << " group = " << group;
    LOG(INFO) << " pad_h = " << pad_h;
    LOG(INFO) << " pad_w = " << pad_w;
    LOG(INFO) << " stride_h = " << stride_h;
    LOG(INFO) << " stride_w = " << stride_w;
    LOG(INFO) << " dilation_h = " << dilation_h;
    LOG(INFO) << " dilation_w = " << dilation_w;
    LOG(INFO) << " kernel_h = " << kernel_h;
    LOG(INFO) << " kernel_w = " << kernel_w;
    LOG(INFO) << " out_channels = " << out_channels;

    Shape img_s(img_num, in_channels, img_h, img_w);
    Shape weights_s(out_channels, in_channels, kernel_h, kernel_w);
    Shape bias_s(1, out_channels, 1, 1);

    TensorHf4 img_host;
    TensorDf4 img_dev;

    img_host.re_alloc(img_s);
    img_dev.re_alloc(img_s);

    for (int i = 0; i < img_host.size(); ++i) {
        img_host.mutable_data()[i] = 2;
    }

    img_dev.copy_from(img_host);

    Shape img_s_sub(img_num, in_channels, img_h / 2, img_w / 2);

    TensorDf4 t0;
    TensorDf4 t1;

    t0.share_sub_buffer(img_dev, img_s_sub, {0, 0, 0, 0});
    t1.share_sub_buffer(img_dev, img_s_sub, {0, 0, 4, 4});

    TensorHI84 weights_host;
    TensorDI84 weights_dev;

    weights_host.re_alloc(weights_s);
    weights_dev.re_alloc(weights_s);

    fill_tensor_host_const(weights_host, 1);
    weights_dev.copy_from(weights_host);

    TensorHf4 bias_host;
    TensorDf4 bias_dev;

    if (bias_term) {
        bias_host.re_alloc(bias_s);
        bias_dev.re_alloc(bias_s);

        fill_tensor_host_const(bias_host, 1);
        bias_dev.copy_from(bias_host);
    }

    TensorHf4 output_host;
    TensorDf4 output_dev;

    TensorDf4 out0;
    TensorDf4 out1;

    output_dev.re_alloc(img_s);

    // start Reshape & doInfer

    Context<NV> ctx1(0, 1, 1);

    ConvParam<TensorDI84> conv_param(group, pad_h, pad_w,
                                     stride_h, stride_w,
                                     dilation_h, dilation_w,
                                     &weights_dev, &bias_dev);

    ActivationParam<TensorDI84> act_param(Active_relu);
    ConvActiveParam<TensorDI84> conv_act_param(conv_param, act_param);

    std::vector<TensorDf4*> input0, input1;
    std::vector<TensorDf4*> output0, output1;

    input0.push_back(&t0);
    input1.push_back(&t1);

    output0.push_back(&out0);
    output1.push_back(&out1);

    cudaMemset(output_dev.mutable_data(), 0, output_dev.size() * sizeof(float));
    cudaDeviceSynchronize();

    ConvAct<NV, AK_INT8, AK_FLOAT, AK_FLOAT, NCHW> conv0;
    ConvAct<NV, AK_INT8, AK_FLOAT, AK_FLOAT, NCHW> conv1;

    conv0.compute_output_shape(output0, input0, conv_act_param);
    out0.share_sub_buffer(output_dev, img_s_sub, {0, 0, 0, 0});

    conv1.compute_output_shape(output1, input1, conv_act_param);
    out1.share_sub_buffer(output_dev, img_s_sub, {0, 0, 4, 4});

    LOG(INFO) << "regular start with group = " << group;
    // init assume output tensor has been reshpaed by user.

    conv0.init(input0, output0, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);
    conv1.init(input1, output1, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);

    conv0(input0, output0, conv_act_param, ctx1);
    conv1(input1, output1, conv_act_param, ctx1);

    cudaStream_t cuda_stream = ctx1.get_compute_stream();
    out0.record_event(cuda_stream);
    out1.record_event(cuda_stream);

    out0.sync();
    out1.sync();

    print_tensor_device(output_dev);
    cudaDeviceSynchronize();

    CUDA_CHECK(cudaPeekAtLastError());
}

TEST(TestSaberFuncNV, test_func_conv_act_int8_speed) {

    typedef TargetWrapper<NV> API;
    typename API::event_t event;
    API::create_event(event);

    typedef TargetWrapper<X86> X86_API;
    typedef TargetWrapper<NV> NV_API;

    Context<NV> ctx2;

    NV_API::record_event(event, ctx2.get_compute_stream());

    int group = 1;
    int pad_h = 1;
    int pad_w = 1;
    int stride_h = 1;
    int stride_w = 1;
    int dilation_h = 1;
    int dilation_w = 1;

    int kernel_h = 3;
    int kernel_w = 3;
    int out_channels = 32;

    int img_num = 1;
    int in_channels = 64;
    int img_h = 800;
    int img_w = 1200;

    bool bias_term = true;

    LOG(INFO) << "conv param: ";
    LOG(INFO) << " img_num = " << img_num;
    LOG(INFO) << " in_channels = " << in_channels;
    LOG(INFO) << " img_h = " << img_h;
    LOG(INFO) << " img_w = " << img_w;
    LOG(INFO) << " group = " << group;
    LOG(INFO) << " pad_h = " << pad_h;
    LOG(INFO) << " pad_w = " << pad_w;
    LOG(INFO) << " stride_h = " << stride_h;
    LOG(INFO) << " stride_w = " << stride_w;
    LOG(INFO) << " dilation_h = " << dilation_h;
    LOG(INFO) << " dilation_w = " << dilation_w;
    LOG(INFO) << " kernel_h = " << kernel_h;
    LOG(INFO) << " kernel_w = " << kernel_w;
    LOG(INFO) << " out_channels = " << out_channels;

    Shape img_s(img_num, in_channels, img_h, img_w);
    Shape weights_s(out_channels, in_channels, kernel_h, kernel_w);
    Shape bias_s(1, out_channels, 1, 1);

    TensorHf4 img_host;
    TensorDf4 img_dev;

    img_host.re_alloc(img_s);
    img_dev.re_alloc(img_s);

    for (int i = 0; i < img_host.size(); ++i) {
        img_host.mutable_data()[i] = 2;
    }

    img_dev.copy_from(img_host);

    TensorHI84 weights_host;
    TensorDI84 weights_dev;

    weights_host.re_alloc(weights_s);
    weights_dev.re_alloc(weights_s);

    fill_tensor_host_const(weights_host, 1);
    weights_dev.copy_from(weights_host);

    TensorHf4 bias_host;
    TensorDf4 bias_dev;

    if (bias_term) {
        bias_host.re_alloc(bias_s);
        bias_dev.re_alloc(bias_s);

        fill_tensor_host_const(bias_host, 1);
        bias_dev.copy_from(bias_host);
    }

    TensorHf4 output_host;
    TensorDf4 output_dev;

    // start Reshape & doInfer

    Context<NV> ctx1(0, 1, 1);

    ConvParam<TensorDI84> conv_param(group, pad_h, pad_w,
                                     stride_h, stride_w,
                                     dilation_h, dilation_w,
                                     &weights_dev, &bias_dev);

    ActivationParam<TensorDI84> act_param(Active_relu);
    ConvActiveParam<TensorDI84> conv_act_param(conv_param, act_param);

    std::vector<TensorDf4*> input;
    std::vector<TensorDf4*> output;

    input.push_back(&img_dev);
    output.push_back(&output_dev);

    ConvAct<NV, AK_INT8, AK_FLOAT, AK_FLOAT, NCHW> conv;
    conv.compute_output_shape(output, input, conv_act_param);

    output_dev.re_alloc(output[0]->shape());
    output_host.re_alloc(output[0]->shape());

    // LOG(INFO) << "regular start with group = " << group;
    // init assume output tensor has been reshpaed by user.

    conv.init(input, output, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);
    conv(input, output, conv_act_param, ctx1);

    SaberTimer<NV> t1;
    int ts = 100;

    for (int i = 0; i < ts; ++i) {
        t1.start(ctx1);
        conv(input, output, conv_act_param, ctx1);
        cudaStream_t cuda_stream = ctx1.get_compute_stream();
        output[0]->record_event(cuda_stream);
        output_dev.sync();
        t1.end(ctx1);
    }

    LOG(INFO) << "conv_act_int8 average times: " << t1.get_average_ms() << " ms ";
    //    print_tensor_device(output_dev);
    cudaDeviceSynchronize();

    CUDA_CHECK(cudaPeekAtLastError());
}

TEST(TestSaberFuncNV, test_func_conv_act_int8_nchw_c4) {

    int group = 1;
    int pad_h = 1;
    int pad_w = 1;
    int stride_h = 1;
    int stride_w = 1;
    int dilation_h = 1;
    int dilation_w = 1;

    int kernel_h = 3;
    int kernel_w = 3;
    int out_channels = 4;

    int img_num = 1;
    int in_channels = 4;
    int img_h = 8;
    int img_w = 8;

    bool bias_term = true;

    LOG(INFO) << "conv param: ";
    LOG(INFO) << " img_num = " << img_num;
    LOG(INFO) << " in_channels = " << in_channels;
    LOG(INFO) << " img_h = " << img_h;
    LOG(INFO) << " img_w = " << img_w;
    LOG(INFO) << " group = " << group;
    LOG(INFO) << " pad_h = " << pad_h;
    LOG(INFO) << " pad_w = " << pad_w;
    LOG(INFO) << " stride_h = " << stride_h;
    LOG(INFO) << " stride_w = " << stride_w;
    LOG(INFO) << " dilation_h = " << dilation_h;
    LOG(INFO) << " dilation_w = " << dilation_w;
    LOG(INFO) << " kernel_h = " << kernel_h;
    LOG(INFO) << " kernel_w = " << kernel_w;
    LOG(INFO) << " out_channels = " << out_channels;

    // IMPORTANT, NCHW_C4, origin C must divide 4.
    Shape img_s(img_num, in_channels / 4, img_h, img_w, 4);
    Shape weights_s(out_channels, in_channels / 4, kernel_h, kernel_w, 4);
    Shape bias_s(1, out_channels, 1, 1);

    TensorHC4 img_host;
    TensorDC4 img_dev;

    img_host.re_alloc(img_s);
    img_dev.re_alloc(img_s);

    for (int i = 0; i < img_host.size(); ++i) {
        img_host.mutable_data()[i] = 2;
    }

    img_dev.copy_from(img_host);

    TensorHC4 weights_host;
    TensorDC4 weights_dev;

    weights_host.re_alloc(weights_s);
    weights_dev.re_alloc(weights_s);

    fill_tensor_host_const(weights_host, 1);
    weights_dev.copy_from(weights_host);

    TensorHf4 bias_host;
    TensorDf4 bias_dev;

    if (bias_term) {
        bias_host.re_alloc(bias_s);
        bias_dev.re_alloc(bias_s);

        fill_tensor_host_const(bias_host, 1);
        bias_dev.copy_from(bias_host);
    }

    TensorHC4 output_host;
    TensorDC4 output_dev;

    // start Reshape & doInfer

    Context<NV> ctx1(0, 1, 1);

    ConvParam<TensorDC4> conv_param(group, pad_h, pad_w,
                                    stride_h, stride_w,
                                    dilation_h, dilation_w,
                                    &weights_dev, &bias_dev);

    ActivationParam<TensorDC4> act_param(Active_relu);
    ConvActiveParam<TensorDC4> conv_act_param(conv_param, act_param);

    std::vector<TensorDC4*> input;
    std::vector<TensorDC4*> output;

    input.push_back(&img_dev);
    output.push_back(&output_dev);

    ConvAct<NV, AK_INT8, AK_INT8, AK_INT8, NCHW_C4, NCHW_C4, NCHW_C4> conv;
    conv.compute_output_shape(output, input, conv_act_param);

    output_dev.re_alloc(output[0]->shape());
    output_host.re_alloc(output[0]->shape());

    conv.init(input, output, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);
    conv(input, output, conv_act_param, ctx1);

    cudaStream_t cuda_stream = ctx1.get_compute_stream();
    output[0]->record_event(cuda_stream);
    output_dev.sync();

    print_tensor_device(output_dev);
    cudaDeviceSynchronize();

    CUDA_CHECK(cudaPeekAtLastError());
}

TEST(TestSaberFuncNV, test_func_conv_act_int8_nchw_c4_speed) {

    int group = 1;
    int pad_h = 1;
    int pad_w = 1;
    int stride_h = 1;
    int stride_w = 1;
    int dilation_h = 1;
    int dilation_w = 1;

    int kernel_h = 3;
    int kernel_w = 3;
    int out_channels = 32;

    int img_num = 1;
    int in_channels = 16;
    int img_h = 240;
    int img_w = 720;

    bool bias_term = true;

    LOG(INFO) << "conv param: ";
    LOG(INFO) << " img_num = " << img_num;
    LOG(INFO) << " in_channels = " << in_channels;
    LOG(INFO) << " img_h = " << img_h;
    LOG(INFO) << " img_w = " << img_w;
    LOG(INFO) << " group = " << group;
    LOG(INFO) << " pad_h = " << pad_h;
    LOG(INFO) << " pad_w = " << pad_w;
    LOG(INFO) << " stride_h = " << stride_h;
    LOG(INFO) << " stride_w = " << stride_w;
    LOG(INFO) << " dilation_h = " << dilation_h;
    LOG(INFO) << " dilation_w = " << dilation_w;
    LOG(INFO) << " kernel_h = " << kernel_h;
    LOG(INFO) << " kernel_w = " << kernel_w;
    LOG(INFO) << " out_channels = " << out_channels;

    // IMPORTANT, NCHW_C4, origin C must divide 4.
    Shape img_s(img_num, in_channels / 4, img_h, img_w, 4);
    Shape weights_s(out_channels, in_channels / 4, kernel_h, kernel_w, 4);
    Shape bias_s(1, out_channels, 1, 1);

    TensorHC4 img_host;
    TensorDC4 img_dev;

    img_host.re_alloc(img_s);
    img_dev.re_alloc(img_s);

    for (int i = 0; i < img_host.size(); ++i) {
        img_host.mutable_data()[i] = 2;
    }

    img_dev.copy_from(img_host);

    TensorHC4 weights_host;
    TensorDC4 weights_dev;

    weights_host.re_alloc(weights_s);
    weights_dev.re_alloc(weights_s);

    fill_tensor_host_const(weights_host, 1);
    weights_dev.copy_from(weights_host);

    TensorHf4 bias_host;
    TensorDf4 bias_dev;

    if (bias_term) {
        bias_host.re_alloc(bias_s);
        bias_dev.re_alloc(bias_s);

        fill_tensor_host_const(bias_host, 1);
        bias_dev.copy_from(bias_host);
    }

    TensorHC4 output_host;
    TensorDC4 output_dev;

    // start Reshape & doInfer

    Context<NV> ctx1(0, 1, 1);

    ConvParam<TensorDC4> conv_param(group, pad_h, pad_w,
                                    stride_h, stride_w,
                                    dilation_h, dilation_w,
                                    &weights_dev, &bias_dev);

    ActivationParam<TensorDC4> act_param(Active_relu);
    ConvActiveParam<TensorDC4> conv_act_param(conv_param, act_param);

    std::vector<TensorDC4*> input;
    std::vector<TensorDC4*> output;

    input.push_back(&img_dev);
    output.push_back(&output_dev);

    ConvAct<NV, AK_INT8, AK_INT8, AK_INT8, NCHW_C4, NCHW_C4, NCHW_C4> conv;
    conv.compute_output_shape(output, input, conv_act_param);

    output_dev.re_alloc(output[0]->shape());
    output_host.re_alloc(output[0]->shape());

    conv.init(input, output, conv_act_param, SPECIFY, VENDER_IMPL, ctx1);
    conv(input, output, conv_act_param, ctx1);

    cudaStream_t cuda_stream = ctx1.get_compute_stream();

    int ts = 100;
    SaberTimer<NV> t1;

    for (int t = 0; t < ts; ++t) {
        t1.start(ctx1);
        conv(input, output, conv_act_param, ctx1);
        output[0]->record_event(cuda_stream);
        output_dev.sync();
        t1.end(ctx1);
    }

    LOG(INFO) << "conv act int8 elapse time: " << t1.get_average_ms() << "ms ";
    //    print_tensor_device(output_dev);
    cudaDeviceSynchronize();

    CUDA_CHECK(cudaPeekAtLastError());
}
#endif
int main(int argc, const char** argv) {
#if 0
    // initial logger
    //logger::init(argv[0]);
    Env<NV>::env_init();

    InitTest();
    RUN_ALL_TESTS(argv[0]);
    return 0;
#endif    
}

