#include "framework/core/net/operator_func.h"

namespace anakin {

template<typename Ttype, DataType Dtype, Precision Ptype>
void OperatorFunc<Ttype, Dtype, Ptype>::launch() {
    (*op)(*ctx_p, ins, outs);
}

template<typename Ttype, DataType Dtype, Precision Ptype>
void OperatorFunc<Ttype, Dtype, Ptype>::infer_shape() {
    op->_helper->InferShape(ins, outs);
}

template class OperatorFunc<NV, AK_FLOAT, Precision::FP32>;
template class OperatorFunc<NV, AK_FLOAT, Precision::FP16>;
template class OperatorFunc<NV, AK_FLOAT, Precision::INT8>;

template class OperatorFunc<ARM, AK_FLOAT, Precision::FP32>;
template class OperatorFunc<ARM, AK_FLOAT, Precision::FP16>;
template class OperatorFunc<ARM, AK_FLOAT, Precision::INT8>;

} /* namespace */

