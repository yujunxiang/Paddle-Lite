// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lite/core/op_lite.h"
#include "lite/core/op_registry.h"

namespace paddle {
namespace lite {
namespace operators {

class MeanOp : public OpLite {
 public:
  explicit MeanOp(const std::string& type) : OpLite(type) {}

  bool CheckShape() const override {
    CHECK_OR_FALSE(param_.X);
    CHECK_OR_FALSE(param_.Out);
    return true;
  }

  bool InferShape() const override {
    param_.Out->Resize(std::vector<int64_t>{1});
    return true;
  }

  bool AttachImpl(const cpp::OpDesc& opdesc, lite::Scope* scope) override {
    auto X_name = opdesc.Input("X").front();
    auto Out_name = opdesc.Output("Out").front();

    param_.X = GetVar<lite::Tensor>(scope, X_name);
    param_.Out = GetMutableVar<Tensor>(scope, Out_name);
    return true;
  }

  void AttachKernel(KernelBase* kernel) override { kernel->SetParam(param_); }

  std::string DebugString() const override { return "mean"; }

 private:
  mutable operators::MeanParam param_;
};

#ifdef LITE_WITH_TRAIN
class MeanGradOp : public OpLite {
 public:
  explicit MeanGradOp(const std::string& type) : OpLite(type) {}

  bool CheckShape() const override {
    CHECK_OR_FALSE(param_.X);
    CHECK_OR_FALSE(param_.Out_grad);
    CHECK_OR_FALSE(param_.X_grad);
    return true;
  }

  bool InferShape() const override {
    param_.X_grad->Resize(param_.X->dims());
    // param_.X_grad->set_lod(param_.X->lod());
    return true;
  }

  bool AttachImpl(const cpp::OpDesc& opdesc, lite::Scope* scope) override {
    CHECK_EQ(opdesc.InputArgumentNames().size(), 2UL);
    auto X_name = opdesc.Input("X").front();
    auto Out_grad_name = opdesc.Input(framework::GradVarName("Out")).front();
    auto X_grad_name = opdesc.Output(framework::GradVarName("X")).front();

    param_.X = GetVar<lite::Tensor>(scope, X_name);
    param_.Out_grad = GetVar<lite::Tensor>(scope, Out_grad_name);
    param_.X_grad = GetMutableVar<Tensor>(scope, X_grad_name);
    return true;
  }

  void AttachKernel(KernelBase* kernel) override { kernel->SetParam(param_); }

  std::string DebugString() const override { return "mean_grad"; }

 private:
  mutable operators::MeanGradParam param_;
};
#endif

}  // namespace operators
}  // namespace lite
}  // namespace paddle

REGISTER_LITE_OP(mean, paddle::lite::operators::MeanOp);
#ifdef LITE_WITH_TRAIN
REGISTER_LITE_OP(mean_grad, paddle::lite::operators::MeanGradOp);
#endif
