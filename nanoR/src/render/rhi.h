#pragma once
#include "rhi_type.h"

namespace nanoR {

class RHI {
 public:
  virtual ~RHI() = default;
  virtual bool CreateBuffer(const RHIBufferCreateInfo &buffer_info, std::shared_ptr<RHIBuffer> &buffer) = 0;
  virtual bool CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) = 0;
  // virtual bool DeleteBuffer(std::shared_ptr<RHIBuffer> &buffer) = 0;
  virtual bool BindVertexBuffer(const RHIBindVertexBufferInfo &bind_vertex_buffer_info,
                                std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) = 0;
  virtual bool BindIndexBuffer(const RHIBindIndexBufferInfo &bind_index_buffer_info,
                               std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) = 0;
  virtual bool Draw(std::shared_ptr<RHIVertexArray> vertex_array) = 0;
};

}  // namespace nanoR